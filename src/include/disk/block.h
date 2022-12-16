#ifndef KOISHIDB_SRC_INCLUDE_DISK_BLOCK_H
#define KOISHIDB_SRC_INCLUDE_DISK_BLOCK_H

#include "type/slice.h"
namespace koishidb {
// Block
struct Block {
  Block(const Slice* block_data)
      : data(block_data->data()), size(block_data->size()) {}
  Block() = default;  // used only by test
  const char* data;   // underlying data
  size_t size;
};

};  // namespace koishidb

#endif