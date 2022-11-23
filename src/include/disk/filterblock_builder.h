#ifndef KOISHIDB_SRC_INCLUDE_FILTERBLOCK_BUILDER_H
#define KOISHIDB_SRC_INCLUDE_FILTERBLOCK_BUILDER_H

#include <vector>
#include "type/slice.h"
#include "util/bloom_filter.h"
#include "common/common.h"
#include "common/status.h"
namespace koishidb {

  class FilterBlockBuilder {
  public:
    FilterBlockBuilder(): bloom_(kBloomFilterPerKeySize) {}

    FilterBlockBuilder(const FilterBlockBuilder& that) = delete;
    FilterBlockBuilder& operator=(const FilterBlockBuilder& that) = delete;

    void AddKey(const Slice& key);

    void StartBlock(uint64_t offset);

    Slice Finish();

  private:
    void GenerateFilter();

    BloomFilter bloom_;
    std::string keys_;  // why do we need this -> for memory error easier to debug
    std::vector<uint32_t> start_;
    std::vector<Slice> tmp_keys_;
    std::vector<uint32_t> filter_offsets_;
    std::string result_; // final result block
  };

};


#endif