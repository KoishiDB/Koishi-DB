#ifndef KOISHIDB_SRC_INCLUDE_DISK_WRITABLE_FILE_H
#define KOISHIDB_SRC_INCLUDE_DISK_WRITABLE_FILE_H

#include <unistd.h>
#include <string>
#include "common/common.h"
#include "common/status.h"

namespace koishidb {

  // Only Support POSIX system call
  class WritableFile {
  public:
    WritableFile(int fd, std::string filename): pos_(0), filename_(filename), dir_(Dirname(filename)), fd_(fd) {}

    ~WritableFile() {
      if (fd_ > 0) {
        ::close(fd_);
      }
    }

    Status Append(const Slice& content);

    Status Flush();

  private:
    // don't need to be defined multiple times
    Status FlushBuffer();

    // static function can't use the unstatic memeber
    Status WriteUnbuffered(const char* data, size_t size);

    static std::string Dirname(const std::string& filename);

    static Status PosixError(const std::string& context, int error_number);


    // TODO, can add the manifest later to restore
    std::string dir_;
    std::string filename_;
    int fd_;
    size_t pos_;
    char buf_[kWritableFileBuffer];
  };
};

#endif