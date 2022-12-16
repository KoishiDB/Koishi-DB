#ifndef KOISHIDB_SRC_INCLUDE_DISK_RANDOM_ACCESS_FILE_H
#define KOISHIDB_SRC_INCLUDE_DISK_RANDOM_ACCESS_FILE_H

#include "common/status.h"
#include "type/slice.h"

namespace koishidb {
// random_access_file is a thread-safe class
// it provides users of reading the content of the file by giving the name of
// the filename
class RandomAccessFile {
 public:
  RandomAccessFile(std::string filename);
  Status status() const { return status_; }

  // Read up to "n" bytes from the file starting at "offset".
  // "scratch[0..n-1]" may be written by this routine.  Sets "*result"
  // to the data that was read (including if fewer than "n" bytes were
  // successfully read).  May set "*result" to point at data in
  // "scratch[0..n-1]", so "scratch[0..n-1]" must be live when
  // "*result" is used.  If an error was encountered, returns a non-OK
  // status.
  //
  // Safe for concurrent use by multiple threads.
  Status Read(uint64_t offset, size_t n, Slice* result, char* scratch);
  ~RandomAccessFile();

 private:
  int fd_;
  const std::string filename_;
  Status status_;
};

};  // namespace koishidb

#endif