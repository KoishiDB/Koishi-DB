#ifndef KOISHIDB_SRC_INCLUDE_UTIL_ENCODE_H
#define KOISHIDB_SRC_INCLUDE_UTIL_ENCODE_H

// use varint to compress data and save memory
#include <cstddef>
#include <cassert>

#include "type/slice.h"

namespace koishidb {
    
    size_t EncodeVarint32(uint32_t value, char* dst);

    size_t EncodeVarint32Length(uint32_t value);

    char* EncodeVarint64(char* dst, uint64_t v);

    void PutVarint64(std::string* dst, uint64_t v);

    const char* DecodeVarint32(const char* dst, uint32_t* value);

    const char* GetVarint64Ptr(const char* p, const char* limit, uint64_t* value);

    // bool indicate whether the data is valid.
    bool GetVarint64(Slice* input, uint64_t* value);

    void PutVarint32(uint32_t value, std::string* dst);

    void GetVarint32(Slice* dst, uint32_t* value);

    void GetFixedBytes(Slice* dst, Slice* result, size_t n);

    void PutLengthPrefixedSlice(std::string* dst, const Slice& value);

    void GetLengthPrefixedSlice(std::string* dst, Slice* src);

    uint32_t DecodeFixed32(const char* ptr);

    uint64_t DecodeFixed64(const char* ptr);

    void PutFixed32(std::string* dst, uint32_t value);

    void PutFixed64(std::string *dst, uint64_t value);

    void EncodeFixed32(char* dst, uint32_t value);

    void EncodeFixed64(char* dst, uint64_t value);


};
#endif