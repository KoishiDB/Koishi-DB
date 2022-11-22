#ifndef KOISHIDB_SRC_INCLUDE_UTIL_BLOOM_FILTER_H
#define KOISHIDB_SRC_INCLUDE_UTIL_BLOOM_FILTER_H

#include "type/slice.h"
#include "util/encode.h"
namespace koishidb {

// Hash function
inline uint32_t Hash(const char *data, size_t n, uint32_t seed) {
  // Similar to murmur hash
  const uint32_t m = 0xc6a4a793;
  const uint32_t r = 24;
  const char *limit = data + n;
  uint32_t h = seed ^ (n * m);

  while (data + 4 <= limit) {
    uint32_t w = DecodeFixed32(data);
    data += 4;
    h += w;
    h *= m;
    h ^= (h >> 16);
  }

  switch (limit - data) {
  case 3:
    h += static_cast<uint8_t>(data[2]) << 16;
  case 2:
    h += static_cast<uint8_t>(data[1]) << 8;
  case 1:
    h += static_cast<uint8_t>(data[0]);
    h *= m;
    h ^= (h >> r);
    break;
  }
  return h;
}



// the basic implementation of bloom filter.
class BloomFilter {
public:
  explicit BloomFilter(int bits_per_key): bits_per_key_(bits_per_key) {
    // k = m / n ln2  is the best by calculation.
    k_ = static_cast<size_t>(bits_per_key * 0.69);  // 0.69 =~ ln(2)
    if (k_ < 1) k_ = 1;
    if (k_ > 30) k_ = 30;
  }

  void CreateFilter(const Slice* keys, int n, std::string* dst) const;

  bool KeyMayMatch(const Slice& key, const Slice& bloom_filter) const;

  int bits_per_key_;
  int k_; // hash function numbers
};

}; // namespace koishidb

#endif