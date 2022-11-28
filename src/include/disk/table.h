#ifndef KOISHIDB_SRC_INCLUDE_DISK_TABLE_H
#define KOISHIDB_SRC_INCLUDE_DISK_TABLE_H

#include <stddef.h>
#include <optional>
#include "util/iterator.h"


namespace koishidb {
using BlockContent = Slice;
class Option;
class RandomAccessFile;
class Status;

class BlockHandle;
  // A table is a sorted string and use RandomAccessFile to get the data.
  // It can be safely accessed by multiple threads.

  // Read the Table is the opposite operation of the table builder
  class Table {
  public:

    static std::optional<Table*> Open(const Option* opt, RandomAccessFile* file, size_t file_size);

    Table(const Table& that) = delete;
    Table& operator=(const Table& that) = delete;

    ~Table() {
      delete rep_;
    }

    // we don't need the read option now, and maybe it can be implemented later
    Iterator* NewIterator() const;


  private:
    // TODO add the table cache


    struct Rep;
    explicit Table(Rep* rep): rep_(rep) {}
    Rep* rep_;

  };
};

#endif