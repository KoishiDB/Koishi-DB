#include "util/bloom_filter.h"

namespace koishidb {
static uint32_t BloomHash(const Slice &key) {
  return Hash(key.data(), key.size(), 0x66ccff);
}

// BloomFilter design borrow by Level db
void BloomFilter::CreateFilter(const Slice *keys, int n, std::string *dst) const {
  // Compute bloom filter size (in both bits and bytes)
  size_t bits = n * bits_per_key_;

  // For small n, we can see a very high false positive rate.  Fix it
  // by enforcing a minimum bloom filter length.
  if (bits < 64) bits = 64;

  size_t bytes = (bits + 7) / 8;
  bits = bytes * 8;

  const size_t init_size = dst->size();
  dst->resize(init_size + bytes, 0);
  dst->push_back(static_cast<char>(k_));  // Remember # of probes in filter
  char* array = &(*dst)[init_size];
  for (int i = 0; i < n; i++) {
    // Use double-hashing to generate a sequence of hash values.
    // See analysis in [Kirsch,Mitzenmacher 2006].
    uint32_t h = BloomHash(keys[i]);
    const uint32_t delta = (h >> 17) | (h << 15);  // Rotate right 17 bits
    for (size_t j = 0; j < k_; j++) {
      const uint32_t bitpos = h % bits;
      array[bitpos / 8] |= (1 << (bitpos % 8));
      h += delta;
    }
  }
}

bool BloomFilter::KeyMayMatch(const Slice &key, const Slice &bloom_filter) const {
    uint32_t h = Hash(key.data(), key.size(), 1);
    for (size_t i = 0; i + 4 <= bloom_filter.size(); i += 4) {
      if (h == DecodeFixed32(bloom_filter.data() + i)) {
        return true;
      }
    }
    return false;
}




};