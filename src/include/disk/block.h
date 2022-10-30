#ifndef KOISHIDB_SRC_INCLUDE_DISK_BLOCK_H
#define KOISHIDB_SRC_INCLUDE_DISK_BLOCK_H

#include "type/slice.h"
namespace koishidb {
  // Block
  struct Block {
      const char* data; // underlying data
      size_t size;
      bool owend;
  };
};

#endif