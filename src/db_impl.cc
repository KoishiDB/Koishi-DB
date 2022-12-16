#include "db_impl.h"

#include <fcntl.h>
#include <unistd.h>

#include <condition_variable>
#include <mutex>
#include <thread>

#include "common/option.h"
#include "disk/builder.h"
#include "disk/random_access_file.h"

namespace koishidb {

struct DBimpl::Writer {
  Writer(WriteBatch* batch) : batch(batch) { done = false; }
  Writer(const Writer& that) = default;
  WriteBatch* batch;
  bool done;
  std::condition_variable cv;
};

// manifest format
// first.
//
DBimpl::DBimpl()
    : memtable_(new Memtable()),
      immutable_memtable_(nullptr),
      cmp_(new InternalKeyComparator()) {
  // read the manifest;
  if (access("./manifest", F_OK) == -1) {
    // don't have the manifest;
    this->sstable_number_ = 0;
    this->last_sequence_ = 0;
  } else {
    // read the manifest;
    ReadManifest();
  }
}

DBimpl::~DBimpl() {
  //
  this->rwlock_.lock();
  this->shutting_down_.store(true, std::memory_order_release);
  while (this->background_compaction_schedule_ == true) {
    std::unique_lock<std::mutex> lock(this->cv_lock_, std::adopt_lock);
    this->background_work_finish_signal_.wait(lock);
  }
  this->rwlock_.unlock();
  if (memtable_ != nullptr) {
    // before we delete it, we should dump it into the disk.
    CompactMemtable();
    delete memtable_;
  }
  // Final thing, dump manifest.
  DumpManifest();
  // should be nullptr now
  assert(immutable_memtable_ == nullptr);
  for (int i = 0; i < this->file_metas_.size(); i++) {
    delete this->file_metas_[i];
  }
  delete cmp_;
}

// Manifest Format;
// file_meta count (varint32)
// last_sequence (varint64)
// file_meta encoded1
// file_meta encoded2.....
Status DBimpl::ReadManifest() {
  int fd = ::open("manifest", O_RDONLY);
  if (fd == -1) {
    LOG_ERROR("manifest corruption");
    return Status::Corruption("manifest corruption");
  }
  char buf[kManifestReserved];
  int ptr = 0;
  int n;
  while ((n = ::read(fd, buf + ptr, sizeof(buf))) > 0) {
    ptr += n;
  }
  Slice slice(buf, ptr);
  uint32_t file_meta_count;
  GetVarint32(&slice, &file_meta_count);
  uint64_t last_sequence;
  GetVarint64(&slice, &last_sequence);
  for (int i = 0; i < file_meta_count; i++) {
    FileMeta* file_meta;
    DecodeFileMeta(file_meta, &slice);
    this->file_metas_.push_back(file_meta);
  }
  this->last_sequence_ = last_sequence;
  this->sstable_number_ = file_meta_count;
  return Status::OK();
}

void DBimpl::DumpManifest() {
  int fd = ::open("manifest", O_WRONLY | O_CREAT);
  WritableFile file(fd, "manifest");
  std::string rep;
  PutVarint32(this->file_metas_.size(), &rep);
  PutVarint64(&rep, this->last_sequence_);
  file.Append(rep.data());
  file.Flush();
  for (int i = 0; i < this->file_metas_.size(); i++) {
    EncodeFileMeta(this->file_metas_[i], file);
  }
}
void DBimpl::Put(const Slice& key, const Slice& value) {
  WriteBatch write_batch;
  write_batch.Put(key, value);
  return Write(&write_batch);
}

void DBimpl::Delete(const Slice& key) {
  WriteBatch write_batch;
  write_batch.Delete(key);
  return Write(&write_batch);
}

void DBimpl::Write(WriteBatch* update) {
  Writer w(update);

  // exclusive lock mode
  std::unique_lock<std::shared_mutex> lock(rwlock_);
  writers_.push_back(&w);  //
  if (!w.done && &w != writers_.front()) {
    // don't block the other threads that invoke Put method
    std::unique_lock<std::mutex> cv_lock(cv_lock_, std::adopt_lock);
    w.cv.wait(cv_lock);
    cv_lock.release();
  }
  cv_lock_.unlock();  // to avoid deadlock and use rwlock to lock
  // the task has been finished
  if (w.done) {
    return;
  }
  MakeRoomForWrite();  //
  Writer* last_writer = &w;

  // get the write batch from the deque
  WriteBatch* writer_batch = BuildWriteBatchGroup(&last_writer);

  WriteBatchInternal::SetSequence(writer_batch, last_sequence_);

  writer_batch->InsertAll(memtable_);

  while (true) {
    Writer* ready = writers_.front();
    writers_.pop_front();
    if (ready != &w) {
      ready->done = true;
      ready->cv.notify_one();
    }

    if (ready == last_writer) break;
  }
  // Notify new head of write queue
  if (!writers_.empty()) {
    writers_.front()->cv.notify_one();
  }
}

// currently we don't support the transaction and don't provide snapshot read
bool DBimpl::Get(const Slice& key, std::string* value) {
  // shared lock mode;
  std::shared_lock<std::shared_mutex> lock(rwlock_);
  Slice memtable_key = CreateMemtableKey(key, UINT64_MAX, "", kTypeSeek);
  Memtable* mem = memtable_;
  Memtable* imm = nullptr;
  if (immutable_memtable_ != nullptr) {
    imm = immutable_memtable_;
  }
  if (mem->Get(memtable_key, value)) {
    delete memtable_key.data();
    return true;
  } else if (immutable_memtable_ != nullptr && imm->Get(memtable_key, value)) {
    delete memtable_key.data();
    return true;
  }
  delete memtable_key.data();
  Slice internal_key = CreateInternalKey(key, UINT64_MAX, kTypeSeek);
  // Find from the disk
  const Option* opt = new Option(new InternalKeyComparator());
  for (int i = 0; i < this->file_metas_.size(); i++) {
    auto file_meta = this->file_metas_[i];
    if (this->cmp_->Compare(file_meta->smallest_key.ToSlice(), internal_key) ==
            -1 &&
        this->cmp_->Compare(file_meta->largest_key.ToSlice(), internal_key) ==
            1) {
      std::string file_name = "sstable_" + file_meta->number;
      RandomAccessFile random_access_file(file_name);
      auto sstable_opt =
          SSTable::Open(opt, &random_access_file, file_meta->file_size);
      assert(sstable_opt.has_value());
      auto result = (*sstable_opt)->InternalGet(internal_key);
      if (!result.has_value()) {
        continue;
      } else {
        *value = *result;
        delete internal_key.data();
        return true;
      }
    }
  }
  delete internal_key.data();
  return false;
}

// REQUIRES hold the lock
// After invoking this function
// last_writer become the last Writer that has been written down
WriteBatch* DBimpl::BuildWriteBatchGroup(Writer** last_writer) {
  Writer* first = writers_.front();
  WriteBatch* result = first->batch;
  assert(result != nullptr);

  size_t size = WriteBatchInternal::Bytes(result);

  // Allow the group to grow up to a maximum size, but if the
  // original write is small, limit the growth so we do not slow
  // down the small write too much.
  size_t max_size = 1 << 20;
  if (size <= (128 << 10)) {
    max_size = size + (128 << 10);
  }

  *last_writer = first;
  std::deque<Writer*>::iterator iter = writers_.begin();
  ++iter;  // Advance past "first"
  for (; iter != writers_.end(); ++iter) {
    Writer* w = *iter;
    if (w != nullptr) {
      size += WriteBatchInternal::Bytes(w->batch);
      if (size > max_size) {
        // Do not make batch too big
        break;
      }
      // Append to *result
      WriteBatchInternal::Append(result, w->batch);
    }
    *last_writer = w;
  }
  return result;
}

// REQUIRE: get the exclusive lock
// TODO, we need to handler the error time.
Status DBimpl::CompactMemtable() {
  std::string new_sstable_name = "sstable_" + sstable_number_++;
  FileMeta* new_file_meta = new FileMeta();
  this->file_metas_.push_back(new_file_meta);
  Status s = BuildTable(new_sstable_name, new_file_meta,
                        this->immutable_memtable_->NewIterator());
  if (!s.ok()) {
    // recover
    sstable_number_--;
    return s;
  }
  return s;
}
// the true compaction here
// REQUIRE: get the exclusive lock
void DBimpl::BackGroundCompaction() {
  if (immutable_memtable_ != nullptr) {
    Status s = CompactMemtable();
    if (s.ok()) {
      delete immutable_memtable_;
      immutable_memtable_ = nullptr;
    }
    return;
  }
  // other conditions
}

// BackGroundCall should get the mutex first
void DBimpl::BackGroundCall() {
  // should get the mutex;
  std::unique_lock<std::shared_mutex> wlock(rwlock_);
  BackGroundCompaction();
  background_compaction_schedule_ = false;
  background_work_finish_signal_.notify_all();
}

// REQUIRE Get the mutex first
void DBimpl::MaybeScheduleCompaction() {
  // should create a thread to compact and never block it;
  // already scheduled
  if (background_compaction_schedule_ == true) {
    //
  } else if (immutable_memtable_ != nullptr) {
    // TODO: need to add major compaction later
    std::thread compaction_task([this]() {
      this->BackGroundCall();  //
    });
    compaction_task.detach();
  } else {
    // TODO(later): change the design of the disk part
  }
}

// REQUIRE(Get the Exclusive Lock)
void DBimpl::MakeRoomForWrite() {
  while (true) {
    if (memtable_->EstimatedSize() < kMaxMemtableSize) {
      return;
    } else if (immutable_memtable_ != nullptr) {
      // wait until the minor compaction done;
      std::unique_lock<std::mutex> lock(cv_lock_, std::adopt_lock);
      background_work_finish_signal_.wait(lock);
      lock.release();
    } else {
      immutable_memtable_ = memtable_;
      memtable_ = new Memtable();
      // Write to Level0;
      MaybeScheduleCompaction();
    }
  }
}
};  // namespace koishidb