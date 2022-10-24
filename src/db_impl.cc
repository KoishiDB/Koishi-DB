#include "db_impl.h"
#include <condition_variable>
#include <mutex>
#include <thread>


namespace koishidb {

    struct DBimpl::Writer {
        Writer(WriteBatch* batch): batch(batch) {}
        Writer(const Writer& that) = default;
        WriteBatch* batch;
        bool done;
        std::condition_variable cv;
    };

  void DBimpl::Put(const Slice &key, const Slice &value) {
      WriteBatch write_batch;
      write_batch.Put(key, value);
      return Write(&write_batch);
  }

  void DBimpl::Delete(const Slice &key) {
      WriteBatch write_batch;
      write_batch.Delete(key);
      return Write(&write_batch);
  }
 // index
  // sstable index block -> block->
  // sstable -> index block
  // filter block
  void DBimpl::Write(WriteBatch *update) {
    Writer w(update);

    // exclusive lock mode
    std::unique_lock<std::shared_mutex> lock(rwlock_);
    writers_.push_back(&w); //
    if (!w.done && &w != writers_.front()) {
        // don't block the other threads that invoke Put method
        std::unique_lock<std::mutex> cv_lock(cv_lock_, std::adopt_lock);
        w.cv.wait(cv_lock);
        cv_lock.release();
    }
    cv_lock_.unlock(); // to avoid dead lock and use rwlock to lock
    // the task has been finished
    if (w.done) {

        return;
    }

    MakeRoomForWrite(); //
    Writer *last_writer = &w;

    // TODO() BuildWriteBatch
    // get the write batch from the deque
    WriteBatch* writer_batch = BuildWriteBatch();

    writer_batch->InsertAll(memtable_);

    while (true) {
          Writer* ready = writers_.front();
          writers_.pop_front();
          ready->cv.notify_one();
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
       CreateMemtableKey(key, UINT64_MAX, "", kTypeSeek);
       Memtable* mem = memtable_;
       Memtable* imm = nullptr;
       if (immutable_memtable_ != nullptr) {
           imm = immutable_memtable_;
       }
       if (mem->Get(key, value)) {
           return true;
       } else if (immutable_memtable_ != nullptr && imm->Get(key, value)) {
           return true;
       }

       // Find from the disk

       return false;
    }

    void DBimpl::CompactMemtable() {

  }
    // the true compaction here
    // REQUIRE: get the exclusive lock
    void DBimpl::BackGroundCompaction() {
        if (immutable_memtable_ != nullptr) {
            CompactMemtable();
            return ;
        }
        // other conditions
    }

    void DBimpl::BackGroundCall() {
      // should get the mutex;
      std::unique_lock<std::shared_mutex> wlock(rwlock_);
      BackGroundCompaction();
      background_compaction_schedule_ = false;
      background_work_finish_signal_.notify_all();
  }

    // why we need MaybeScheduleCompaction ->
    // REQUIRE Get the mutex first
    void DBimpl::MaybeScheduleCompaction() {
        // should create a thread to compact and never block it;
        // already scheduled
        if (background_compaction_schedule_ == true) {

        } else if (immutable_memtable_ != nullptr) {
            // TODO: need to add major compaction later
            std::thread compaction_task([this](){
                this->BackGroundCall(); //
            });
            compaction_task.detach();
        } else {

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
};