#ifndef KOISHIDB_SRC_INCLUDE_TABLE_ITERATOR_H
#define KOISHIDB_SRC_INCLUDE_TABLE_ITERATOR_H

#include "util/iterator.h"

namespace koishidb {
class SSTable;
class Block;
class TableIterator: public Iterator {
public:
  TableIterator(Block* indexBlock);
  TableIterator(const TableIterator& that) = delete;
  TableIterator& operator=(const TableIterator& that) = delete;

  ~TableIterator();
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