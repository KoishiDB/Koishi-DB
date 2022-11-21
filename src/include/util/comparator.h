#ifndef KOISHIDB_SRC_INCLUDE_UTIL_COMPARATOR_H
#define KOISHIDB_SRC_INCLUDE_UTIL_COMPARATOR_H

#include "type/slice.h"
#include "util/util.h"
#include "common/common.h"

namespace koishidb {
  class Comparator {
  public:

     virtual ~Comparator() = default;

     // -1 means a < b, 1 means a > b, 0 means a == b
     virtual int Compare(const Slice& a, const Slice& b) const = 0;

     virtual std::string Name() const = 0; // return the Comparator Name

  };
};

#endif