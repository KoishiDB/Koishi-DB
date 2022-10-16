#ifndef KOISHIDB_SRC_INCLUDE_UTIL_ENCODE_H
#define KOISHIDB_SRC_INCLUDE_UTIL_ENCODE_H

// use varint to compress data and save memory
#include <cstddef>
#include <cassert>

#include "type/slice.h"

namespace koishidb {

    size_t EncodeVarint32(uint32_t value, char *dst);

    const char *DecodeVarint32(const char *dst, uint32_t *value);

    void PutVarint32(uint32_t value, std::string *dst);

    void GetVarint32(Slice *dst, uint32_t *value);

    void GetFixedBytes(Slice *dst, Slice *result, size_t n);

    void PutLengthPrefixedSlice(std::string *dst, const Slice &value);

    void EncodeFixed32(char *dst, uint32_t value);

    void EncodeFixed64(char *dst, uint64_t value);

    uint32_t DecodeFixed32(const char *ptr);

    uint64_t DecodeFixed64(const char *ptr);

    void PutFixed32(uint32_t value, std::string *dst);

    void PutFixed64(uint64_t value, std::string *dst);
};
#endif