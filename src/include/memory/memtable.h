#ifndef SRC_INCLUDE_MEMORY_MEMTABLE_H
#define SRC_INCLUDE_MEMORY_MEMTABLE_H

#include "memory/table.h"
#include "memory/skiplist.h"
#include <string>
#include <shared_mutex>
#include <queue>

namespace koishidb {
  class Memtable {
  public:
      Memtable(Table<std::string, std::string>* table) : table_(table) {}

      Memtable(const Memtable& that) = delete;

      Memtable& operator=(const Memtable& that) = delete; // can't be copied

      ~Memtable() {
          delete table_;
      }
      size_t EstimatedSize();

      // TODO: add the status later and use iterator to seek key and value later.
      bool MemtableGet(const std::string& key, std::string& value);

      void MemtablePut(const std::string& key, const std::string& value);

      void MemtableDelete(const std::string& key);
  private:
      Table<std::string, std::string>* table_; // table might be skiplist, balanced tree or other data structs that
                                               // implement the interface of table
      int ref_; // reference

      bool is_mutable_;

      std::shared_mutex rwlock_; // reader and writer lock, when is_mutable_ == true, should be used
  };

  // TODO
  class MemtableView {
  public:
      MemtableView() = default;

      ~MemtableView() {
          if (immutable_memtable_ != nullptr) {
              delete immutable_memtable_;
          }
          delete memtable_;
      }

      bool Get(const std::string& key, std::string& value);

      void Put(const std::string& key, const std::string& value);

      void Delete(const std::string& key);
  private:
      Memtable* memtable_;

      Memtable* immutable_memtable_;
  };
};

#endif