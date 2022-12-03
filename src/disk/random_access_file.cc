#include <fcntl.h>
#include <unistd.h>
#include "disk/random_access_file.h"

namespace koishidb {
  RandomAccessFile::RandomAccessFile(std::string filename): filename_(filename) {
     fd_ = ::open(filename.c_str(), O_RDONLY);
     if (fd_ == -1) {
       status_.Corruption("Open file error, check the filename");
     }
  }

  Status RandomAccessFile::Read(uint64_t offset, size_t n, Slice *result, char *scratch) {
        // use pread
      assert(fd_ != -1);
      ssize_t read_size = ::pread(fd_, scratch, n, static_cast<off_t>(offset));
      *result = Slice(scratch, (read_size < 0) ? 0 : read_size);
      if (read_size < 0) {
        status_.Corruption("read file error");
      }
      return status_;
  }

  RandomAccessFile::~RandomAccessFile() {
    assert(fd_ != -1);
    ::close(fd_);
  }
};