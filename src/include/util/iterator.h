#ifndef KOISHIDB_SRC_INCLUDE_UTIL_ITERATOR_H
#define KOISHIDB_SRC_INCLUDE_UTIL_ITERATOR_H

#include "type/slice.h"

namespace koishidb {

class Iterator {
 public:
  Iterator() = default;

  Iterator(const Iterator& that) = delete;

  Iterator& operator=(const Iterator& that) = delete;

  virtual ~Iterator() = default;

  virtual bool Valid() const = 0;

  virtual void Prev() = 0;

  virtual void Next() = 0;

  virtual void SeekToFirst() = 0;

  virtual void SeekToLast() = 0;

  virtual Slice Key() const = 0;

  virtual Slice Value() const = 0;

  // seek can move the iterator position and set the right position
  virtual bool Seek(const Slice& target) = 0;
};
};  // namespace koishidb

#endif