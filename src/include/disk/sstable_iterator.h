#ifndef KOISHIDB_SRC_INCLUDE_SSTABLE_ITERATOR_H
#define KOISHIDB_SRC_INCLUDE_SSTABLE_ITERATOR_H

#include "util/iterator.h"

namespace koishidb {
class RandomAccessFile;
class Block;
class Option;
class SSTableIterator: public Iterator {
public:
  SSTableIterator(Block* indexBlock, RandomAccessFile* random_access_file, const Option* opt);
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



  Iterator* BlockReader(RandomAccessFile* random_access_file, const Slice& index_value);

  Iterator* index_iter_;
  Iterator* data_iter_;

  const Option* opt_;
  RandomAccessFile* random_access_file_;
  // used to skip the unnecessary data block set.
  std::string data_block_handle_;
};

};

#endif