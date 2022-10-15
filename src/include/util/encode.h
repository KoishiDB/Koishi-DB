#ifndef KOISHIDB_SRC_INCLUDE_UTIL_ENCODE_H
#define KOISHIDB_SRC_INCLUDE_UTIL_ENCODE_H

// use varint to compress data and save memory
#include <cstddef>
#include <cassert>

#include "type/slice.h"

namespace koishidb {
    
    size_t EncodeVarint32(uint32_t value, char* dst);

    const char* DecodeVarint32(const char* dst, uint32_t* value);

    void PutVarint32(uint32_t value, std::string* dst);

    void GetVarint32(Slice* dst, uint32_t* value);

    void GetFixedBytes(Slice* dst, Slice* result, size_t n);

    void PutLengthPrefixedSlice(std::string* dst, const Slice& value);

    inline void EncodeFixed32(char* dst, uint32_t value){
        uint8_t* const buffer = reinterpret_cast<uint8_t*>(dst);
        buffer[0] = static_cast<uint8_t>(value);
        buffer[1] = static_cast<uint8_t>(value >> 8);
        buffer[2] = static_cast<uint8_t>(value >> 16);
        buffer[3] = static_cast<uint8_t>(value >> 24);
    }
    inline void EncodeFixed64(char* dst, uint64_t value){
        uint8_t* const buffer = reinterpret_cast<uint8_t*>(dst);

        buffer[0] = static_cast<uint8_t>(value);
        buffer[1] = static_cast<uint8_t>(value >> 8);
        buffer[2] = static_cast<uint8_t>(value >> 16);
        buffer[3] = static_cast<uint8_t>(value >> 24);
        buffer[4] = static_cast<uint8_t>(value >> 32);
        buffer[5] = static_cast<uint8_t>(value >> 40);
        buffer[6] = static_cast<uint8_t>(value >> 48);
        buffer[7] = static_cast<uint8_t>(value >> 56);
    }

    inline uint32_t DecodeFixed32(const char* ptr) {
        const uint8_t* const buffer = reinterpret_cast<const uint8_t*>(ptr);

        return (static_cast<uint32_t>(buffer[0])) |
               (static_cast<uint32_t>(buffer[1]) << 8) |
               (static_cast<uint32_t>(buffer[2]) << 16) |
               (static_cast<uint32_t>(buffer[3]) << 24);
    }

    inline uint64_t DecodeFixed64(const char* ptr) {
        const uint8_t* const buffer = reinterpret_cast<const uint8_t*>(ptr);

        return (static_cast<uint64_t>(buffer[0])) |
               (static_cast<uint64_t>(buffer[1]) << 8) |
               (static_cast<uint64_t>(buffer[2]) << 16) |
               (static_cast<uint64_t>(buffer[3]) << 24) |
               (static_cast<uint64_t>(buffer[4]) << 32) |
               (static_cast<uint64_t>(buffer[5]) << 40) |
               (static_cast<uint64_t>(buffer[6]) << 48) |
               (static_cast<uint64_t>(buffer[7]) << 56);
    }

};
#endif