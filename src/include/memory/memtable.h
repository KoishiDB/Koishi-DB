#ifndef SRC_INCLUDE_MEMORY_MEMTABLE_H
#define SRC_INCLUDE_MEMORY_MEMTABLE_H

#include "memory/table.h"
#include "memory/skiplist.h"
#include "util/comparator.h"
#include "util/iterator.h"
#include <string>
#include <shared_mutex>
#include <queue>
#include <atomic>

#include "logger.h"
#include <iostream>

namespace koishidb {

  class MemtableIterator;
  // For memtable key, iff the user_key is the same, then the larger the sequence is, that smaller the memtablekey is.
  class MemtableKeyComparator: public Comparator {
  public:
      MemtableKeyComparator() = default;
      int Compare(const Slice& memtable_key1, const Slice& memtable_key2) const {
          LookupKey key1(memtable_key1), key2(memtable_key2);
          Slice user_key1 = key1.UserKey(), user_key2 = key2.UserKey();
          SequenceNumber s1 = key1.GetSequence(), s2 = key2.GetSequence();
          int r = user_key1.Compare(user_key2);
          if (r == 0) {
              if (s1 == s2) {
                  return 0;
              }
              if (s1 > s2) {
                  return -1;
              }
              if (s1 < s2) {
                  return 1;
              }
          }
          return r;
      }
      std::string Name() const {
          return "MemtableKeyComparator";
      }
  };

  class Memtable {
  public:
      // every memtable maintain a skiplist

      Memtable() : table_(new SkipList<Slice, MemtableKeyComparator>(new MemtableKeyComparator())) {}

      Memtable(const Memtable& that) = delete;

      Memtable& operator=(const Memtable& that) = delete; // can't be copied

      ~Memtable() {
          delete table_;
      }
      size_t EstimatedSize();

      // TODO: add the status later and use iterator to seek key and value later.
      bool Get(const Slice& memtable_key, std::string* result);

      void Insert(const Slice& memtable_key);

      Iterator* NewIterator();

  private:
      friend class MemtableIterator;

      using Table = SkipList<Slice, MemtableKeyComparator>;

      Table* table_;

      int ref_; // reference

      std::atomic<bool> imm_;  // whether an immutable memtable exists

      std::shared_mutex rwlock_; // reader and writer lock, when is_mutable_ == true, should be used

      size_t size_;
  };

};

#endif