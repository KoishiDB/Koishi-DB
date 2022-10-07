#ifndef KOISHIDB_SRC_INCLUDE_UTIL_ITERATOR_H
#define KOISHIDB_SRC_INCLUDE_UTIL_ITERATOR_H

#include <string>

namespace koishidb {
  // Iterator interface
  class Iterator {
      Iterator();

      Iterator(const Iterator& that) = delete;

      Iterator& operator=(const Iterator& that) = delete;

      virtual ~Iterator() = default;

      // iterator function
      virtual bool Valid() = 0; //

      virtual void Prev() = 0;

      virtual void Next() = 0;

      virtual std::string Key() = 0;

      virtual std::string Value() = 0;

  };
};

#endif