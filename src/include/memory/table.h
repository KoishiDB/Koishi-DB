#ifndef KOISHIDB_SRC_INCLUDE_MEMORY_TABLE_H
#define KOISHIDB_SRC_INCLUDE_MEMORY_TABLE_H

#include <cstddef>
// table is interface
namespace koishidb {

template <typename K, typename V>
class Table {
 public:
  Table() = default;

  virtual ~Table() = default;

  virtual bool Get(const K& key, V& value) = 0;

  virtual void Put(const K& key, const V& value) = 0;

  virtual void Delete(const K& key) = 0;

  virtual size_t EstimatedSize() = 0;  // return the current k, v pairs
};
}  // namespace koishidb

#endif