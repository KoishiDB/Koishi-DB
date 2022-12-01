#ifndef KOISHIDB_SRC_INCLUDE_DISK_FORMAT_H
#define KOISHIDB_SRC_INCLUDE_DISK_FORMAT_H

#include "type/slice.h"
#include "common/status.h"
#include "common/common.h"
#include "util/encode.h"
#include "disk/sstable.h"
#include <optional>
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

// append to the dst, so we need to use the ptr of string
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


// class Footer
class Footer {
public:


  Footer() = default;
  Footer(const Footer&) = delete;
  Footer& operator=(const Footer&) = delete;

  void set_index(BlockHandle index_block_handle) {
    index_block_handle_ = index_block_handle;
  }

  void set_filter(BlockHandle filter_block_handle) {
    filter_block_handle_ = filter_block_handle;
  }

  BlockHandle index_handle() { return index_block_handle_; }

  BlockHandle filter_handle() { return filter_block_handle_; }

  void EncodeTo(std::string* dst);

  Status DecodeFrom(Slice* s);
private:
  BlockHandle index_block_handle_;
  BlockHandle filter_block_handle_;
};

inline void Footer::EncodeTo(std::string *dst) {
  int begin = dst->size();
  index_block_handle_.EncodeTo(dst);
  filter_block_handle_.EncodeTo(dst);
  int paddings = kFixedFooterSize - (dst->size() - begin);
  dst->append(paddings, static_cast<char>(0));
}

inline Status Footer::DecodeFrom(Slice *input) {
  auto s = index_block_handle_.DecodeFrom(input);
  if (!s.ok())
    return s;
  s = filter_block_handle_.DecodeFrom(input);
  return s;
}


// Given the block handle and return the data of the offset.
std::optional<BlockContent*> ReadBlock(RandomAccessFile* file, const BlockHandle& blockHandle);
};

#endif
