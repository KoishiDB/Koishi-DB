#ifndef KOISHIDB_SRC_INCLUDE_TYPE_SLICE_H
#define KOISHIDB_SRC_INCLUDE_TYPE_SLICE_H

#include <cassert>
#include <cstddef>
#include <cstring>
#include <string>
namespace koishidb {
class Slice {
 public:
  Slice();
  Slice(const char* d);
  Slice(const std::string& s);
  // should be copyable
  Slice(const char* d, size_t size);
  Slice(const Slice& that) = default;
  Slice& operator=(const Slice& that) = default;
  // getters
  const char* data() const;
  size_t size() const;
  bool empty() const;

  // public functions
  char operator[](size_t n) const;
  bool operator==(const Slice& that) const;
  bool operator!=(const Slice& that) const;  // const
  int Compare(const Slice& that) const;
  void Advance(size_t n);
  void Append(const char* d, size_t n);
  void Clear();

  std::string ToString() const;

 private:
  const char* data_;
  size_t size_;
};
};  // namespace koishidb

#endif