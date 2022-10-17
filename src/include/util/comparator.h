#ifndef KOISHIDB_SRC_INCLUDE_UTIL_COMPARATOR_H
#define KOISHIDB_SRC_INCLUDE_UTIL_COMPARATOR_H

#include "type/slice.h"
#include "util/util.h"
namespace koishidb {
  class Comparator {
  public:
     virtual ~Comparator() = default;

     virtual int Compare(const Slice& a, const Slice& b) const = 0;

     virtual std::string Name() const = 0; // return the Comparator Name
  };
};

#endif