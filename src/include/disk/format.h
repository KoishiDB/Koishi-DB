#ifndef KOISHIDB_SRC_INCLUDE_DISK_FORMAT_H
#define KOISHIDB_SRC_INCLUDE_DISK_FORMAT_H

#include "type/slice.h"
#include "common/status.h"
#include "util/encode.h"
namespace koishidb {

class BlockHandle {
public:
  // Maximum encoding length of a BlockHandle
  enum { kMaxEncodedLength = 10 + 10 };

  BlockHandle();
  // The offset of the block in the file.
  uint64_t offset() const { return offset_; }
  void set_offset(uint64_t offset) { offset_ = offset; }

  // The size of the stored block
  uint64_t size() const { return size_; }
  void set_size(uint64_t size) { size_ = size; }

  void EncodeTo(std::string* dst) const;
  Status DecodeFrom(Slice* input);

private:
  uint64_t offset_;
  uint64_t size_;
};

inline BlockHandle::BlockHandle(): offset_(~static_cast<uint64_t>(0)), size_(~static_cast<uint64_t>(0)) {}
inline void BlockHandle::EncodeTo(std::string *dst) const {
  // Sanity check that all fields have been set

  // why we should put it to the format, Because we use the putvarint
  assert(offset_ != ~static_cast<uint64_t>(0));
  assert(size_ != ~static_cast<uint64_t>(0));
  PutVarint64(dst, offset_);
  PutVarint64(dst, size_);
}
inline Status BlockHandle::DecodeFrom(Slice* input) {
  if (GetVarint64(input, &offset_) && GetVarint64(input, &size_)) {
    return Status::OK();
  } else {
    return Status::Corruption("bad block handle");
  }
}


};

#endif
