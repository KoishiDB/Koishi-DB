#ifndef KOISHIDB_SRC_INCLUDE_DISK_BLOCK_ITERATOR_H
#define KOISHIDB_SRC_INCLUDE_DISK_BLOCK_ITERATOR_H

#include "util/iterator.h"
#include "util/comparator.h"
#include "common/common.h"
// Block iterator 底层是一个

namespace koishidb {
  struct Block;
  class Option;
  class BlockIterator:public Iterator {
  public:
      // default the internalkey iterator
      BlockIterator(Block* block, const Option* opt);




      ~BlockIterator() = default;

      BlockIterator(const BlockIterator& that) = delete;

      BlockIterator& operator=(const BlockIterator& that) = delete;

      Slice Key() const override;

      Slice Value() const override;

      bool Valid() const override;

      bool Seek(const Slice& target) override;

      void SeekToFirst() override;

      void SeekToLast() override;

      void Next() override;

      void Prev() override;
  private:
      std::string key_;
      const char* const data_;
      const size_t size_;
      const Comparator* const cmp_; // used to compare internal key;
      uint32_t offset_; // current offset of the data_
      Slice value_; // the value of the key
  };
};
#endif