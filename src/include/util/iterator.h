#ifndef KOISHIDB_SRC_INCLUDE_UTIL_ITERATOR_H
#define KOISHIDB_SRC_INCLUDE_UTIL_ITERATOR_H

#include "type/slice.h"

namespace koishidb {
  // Iterator interface
  class Iterator {
  public:
      Iterator();

      Iterator(const Iterator& that) = delete;

      Iterator& operator=(const Iterator& that) = delete;

      virtual ~Iterator() = default;

      virtual bool Valid() = 0;

      virtual void Prev() = 0;

      virtual void Next() = 0;

      virtual void SeekToFirst() = 0;

      virtual void SeekToLast() = 0;

      virtual Slice Key() = 0;

      virtual Slice Value() = 0;

  };
};

#endif