#include <sys/errno.h>


#include "disk/writable_file.h"



namespace koishidb {
  Status WritableFile::Append(const Slice &content) {
    size_t write_size = content.size();
    const char* write_data = content.data();

    // Fit as much as possible into buffer.
    size_t copy_size = std::min(write_size, kWritableFileBuffer - pos_);
    std::memcpy(buf_ + pos_, write_data, copy_size);
    write_data += copy_size;
    write_size -= copy_size;
    pos_ += copy_size;
    if (write_size == 0) {
      return Status::OK();
    }
    // can't fit it the buffer, should Flush at first
    Status status = FlushBuffer();
    if (!status.ok()) {
      return status;
    }
    // Small writes go to buffer, large writes are written directly.
    if (write_size < kWritableFileBuffer) {
      std::memcpy(buf_, write_data, write_size);
      pos_ = write_size;
      return Status::OK();
    }
    return WriteUnbuffered(write_data, write_size);
  }

  Status WritableFile::FlushBuffer() {
    Status status = WriteUnbuffered(buf_, pos_);
    pos_ = 0;
    return status;
  }

  Status WritableFile::WriteUnbuffered(const char *data, size_t size) {
    while (size > 0) {
      ssize_t write_result = ::write(fd_, data, size);
      if (write_result < 0) {
        if (errno == EINTR) {
          continue;  // Retry
        }
        return PosixError(filename_, errno);
      }
      data += write_result;
      size -= write_result;
    }
    return Status::OK();
  }

  // static
  Status WritableFile::PosixError(const std::string &context, int error_number){
    if (error_number == ENOENT) {
      return Status::NotFound(context, std::strerror(error_number));
    } else {
      return Status::IOError(context, std::strerror(error_number));
    }
  }
}; // namespace koishidb