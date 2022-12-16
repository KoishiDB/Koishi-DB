#ifndef KOISHIDB_SRC_INCLUDE_DISK_FILTERBLOCK_READER_H
#define KOISHIDB_SRC_INCLUDE_DISK_FILTERBLOCK_READER_H

#include "type/slice.h"
#include "util/bloom_filter.h"

namespace koishidb {

class FilterBlockReader {
 public:
  // REQUIRES: "contents" must stay alive !
  FilterBlockReader(const Slice& contents);
  bool KeyMayMatch(uint64_t block_offset, const Slice& key);

 private:
  const BloomFilter bloom_;
  const char* data_;    // Pointer to filter data (at block-start)
  const char* offset_;  // Pointer to beginning of offset array (at block-end)
  size_t num_;          // Number of entries in offset array
  size_t base_lg_;      // Encoding parameter (see kFilterBaseLg in .cc file)
};

};  // namespace koishidb

#endif