#ifndef SRC_INCLUDE_MEMORY_MEMTABLE_H
#define SRC_INCLUDE_MEMORY_MEMTABLE_H

#include "memory/table.h"
#include "memory/skiplist.h"
#include "util/comparator.h"
#include <string>
#include <shared_mutex>
#include <queue>
#include <atomic>

namespace koishidb {
  class Memtable {
  public:
      Memtable() : table_(new SkipList<Slice, Comparator>(Comparator())) {}

      Memtable(const Memtable& that) = delete;

      Memtable& operator=(const Memtable& that) = delete; // can't be copied

      ~Memtable() {
          delete table_;
      }
      size_t EstimatedSize();

      // TODO: add the status later and use iterator to seek key and value later.
      bool Get(const Slice& memtable_key, std::string* result);

      void Insert(const Slice& memtable_key);

  private:

      SkipList<Slice, Comparator>* table_; // table might be skiplist, balanced tree or other data structs that
                                               // implement the interface of table
      int ref_; // reference

      std::atomic<bool> imm_;  //

      std::shared_mutex rwlock_; // reader and writer lock, when is_mutable_ == true, should be used
  };

};

#endif