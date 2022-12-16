#ifndef KOISHIDB_SRC_INCLUDE_DB_IMPL_H
#define KOISHIDB_SRC_INCLUDE_DB_IMPL_H

#include <condition_variable>
#include <deque>
#include <mutex>
#include <shared_mutex>

#include "common/status.h"
#include "db.h"
#include "memory/memtable.h"

namespace koishidb {
// Is the scheduler of the DB storage part
struct FileMeta;
class DBimpl : public DB {
 private:
  struct Writer;  // forward declare
 public:
  DBimpl();

  ~DBimpl();

  void Put(const Slice& key, const Slice& value);

  bool Get(const Slice& key, std::string* value);

  void Delete(const Slice& key);

  void Write(WriteBatch* update);

  // when a new writer comes, it should always invoke this function to make room
  // for it;
  void MakeRoomForWrite();

  void MaybeScheduleCompaction();

  void BackGroundCall();

  void BackGroundCompaction();

  Status CompactMemtable();

  WriteBatch* BuildWriteBatchGroup(Writer** last_writer);

  Status ReadManifest();

  void DumpManifest();

 private:
  // declare here
  std::deque<Writer*> writers_;
  std::shared_mutex rwlock_;
  std::mutex cv_lock_;  // used only for condition variable
  Memtable* memtable_;
  Memtable* immutable_memtable_;
  std::condition_variable
      background_work_finish_signal_;  // used to notify the background
                                       // compaction done
  bool background_compaction_schedule_;
  SequenceNumber last_sequence_;  // last_sequence notify the last_key.
  std::atomic<bool> shutting_down_;
  std::vector<FileMeta*> file_metas_;  // notify the file_meta
  uint64_t sstable_number_;  // notify the newest sstable_number_ that have been
                             // not used.
  Comparator* cmp_;          // in fact is internal key comparator
};
};  // namespace koishidb

#endif