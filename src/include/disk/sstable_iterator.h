#ifndef KOISHIDB_SRC_INCLUDE_SSTABLE_ITERATOR_H
#define KOISHIDB_SRC_INCLUDE_SSTABLE_ITERATOR_H

#include "util/iterator.h"

namespace koishidb {
class SSTable;
class Block;
class SSTableIterator: public Iterator {
public:
  SSTableIterator(Block* indexBlock);
  SSTableIterator(const SSTableIterator& that) = delete;
  SSTableIterator& operator=(const SSTableIterator& that) = delete;

  ~SSTableIterator();
  bool Valid() const ;

  void Prev();

  void Next();

  void SeekToFirst();

  void SeekToLast();

  Slice Key() const;

  Slice Value() const;

  // seek can move the iterator position and set the right position
  bool Seek(const Slice& target);
private:

  void UpdateDataIterator();

  SSTable* table_;

  Iterator* BlockReader(SSTable* table, const Slice& index_value);

  Iterator* index_iter_;
  Iterator* data_iter_;

  // used to skip the unnecessary data block set.
  std::string data_block_handle_;
};

};

#endif