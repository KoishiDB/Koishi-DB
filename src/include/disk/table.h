#ifndef KOISHIDB_SRC_INCLUDE_DISK_TABLE_H
#define KOISHIDB_SRC_INCLUDE_DISK_TABLE_H

#include <stddef.h>
#include <optional>
#include <memory>
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
  class SSTable {
  public:

    static std::optional<SSTable*> Open(const Option* opt, RandomAccessFile* file, size_t file_size);

    SSTable(const SSTable& that) = delete;
    SSTable& operator=(const SSTable& that) = delete;

    ~SSTable() {
      delete rep_;
    }

    // we don't need the read option now, and maybe it can be implemented later
    // This iterator needs to manually delete, maybe it can be replaced by unique_ptr?
    Iterator* NewIterator() const;

    RandomAccessFile* random_access_file() const { return file_; }
  private:
    // TODO add the table cache

    struct Rep;
    explicit SSTable(Rep* rep): rep_(rep) {}
    Rep* rep_;
    RandomAccessFile* file_; //used to create the table iterator only
  };
};

#endif