#include "util/encode.h"

namespace koishidb {
    // use space and never use tab
    size_t EncodeVarint32(uint32_t value, char *dst) {
        uint8_t *ptr = reinterpret_cast<uint8_t *> (dst);
        size_t size = 0;
        static const uint8_t B = 128;
        while (true) {
            size++;
            if (value < B) {
                *(ptr++) = value;
                break;
            } else {
                *(ptr++) = (value & 0x7F) | B;
                value >>= 7;
            }
        }
        return size;
    }

    // TODO: need to handle the invalid data
    const char *DecodeVarint32(const char *dst, uint32_t *value) {
        // uint32_t at most 5 bytes
        const uint8_t *ptr = reinterpret_cast<const uint8_t *>(dst);
        uint32_t part0 = *(ptr++);
        if (part0 < 0x80) {
            *value = part0;
            return reinterpret_cast<const char *> (ptr);
        }
        part0 -= 0x80;
        uint32_t part1 = *(ptr++);
        part0 += part1 << 7;
        if (part1 < 0x80) {
            *value = part0;
            return reinterpret_cast<const char *> (ptr);
        }
        part0 -= 0x80 << 7;
        uint32_t part2 = *(ptr++);
        part0 += part2 << 14;
        if (part2 < 0x80) {
            *value = part0;
            return reinterpret_cast<const char *> (ptr);
        }
        part0 -= 0x80 << 14;
        uint32_t part3 = *(ptr++);
        part0 += part3 << 21;
        if (part3 < 0x80) {
            *value = part0;
            return reinterpret_cast<const char *> (ptr);
        }
        part0 -= 0x80 << 21;
        uint32_t part4 = *ptr;
        part0 += part4 << 28;
        *value = part0;
        return reinterpret_cast<const char *> (ptr);
    }

    void PutVarint32(uint32_t value, std::string *dst) {
        char buf[5];
        size_t advance = EncodeVarint32(value, buf);
        dst->append(buf, advance);
    }

    void GetVarint32(Slice *dst, uint32_t *value) {
        const char *data = dst->data();
        const char *limit = data + dst->size();
        const char *q = DecodeVarint32(data, value);

        // 这一行Slice是一个tmp 对象吗？
        *dst = Slice(q, limit - q);
    }

    // GetFixedBytes to result
    void GetFixedBytes(Slice *dst, Slice *result, size_t n) {
        assert(n <= dst->size());
        // can use move assignment to speed up
        *result = Slice(dst->data(), n);
        *dst = Slice(dst->data() + n, dst->size() - n);
    }

    void PutLengthPrefixedSlice(std::string *dst, const Slice &value) {
        PutVarint32(value.size(), dst);
        dst->append(value.data(), value.size());
    }

    // the data_ will auto advance
    // LengthPrefixed -> varint + value
    void GetLengthPrefixedSlice(std::string *dst, Slice *src) {
        uint32_t len;
        Slice tmp = *src; //
        GetVarint32(src, &len);
        // dst->append
        size_t offset = src->data() - tmp.data();
        dst->append(tmp.data(), offset);
        dst->append(src->data(), len);
        src->Advance(len);
    }


    void EncodeFixed32(char *dst, uint32_t value) {
        uint8_t *const buffer = reinterpret_cast<uint8_t *>(dst);
        buffer[0] = static_cast<uint8_t>(value);
        buffer[1] = static_cast<uint8_t>(value >> 8);
        buffer[2] = static_cast<uint8_t>(value >> 16);
        buffer[3] = static_cast<uint8_t>(value >> 24);
    }

    void EncodeFixed64(char *dst, uint64_t value) {
        uint8_t *const buffer = reinterpret_cast<uint8_t *>(dst);

        buffer[0] = static_cast<uint8_t>(value);
        buffer[1] = static_cast<uint8_t>(value >> 8);
        buffer[2] = static_cast<uint8_t>(value >> 16);
        buffer[3] = static_cast<uint8_t>(value >> 24);
        buffer[4] = static_cast<uint8_t>(value >> 32);
        buffer[5] = static_cast<uint8_t>(value >> 40);
        buffer[6] = static_cast<uint8_t>(value >> 48);
        buffer[7] = static_cast<uint8_t>(value >> 56);
    }

    uint32_t DecodeFixed32(const char *ptr) {
        const uint8_t *const buffer = reinterpret_cast<const uint8_t *>(ptr);

        return (static_cast<uint32_t>(buffer[0])) |
               (static_cast<uint32_t>(buffer[1]) << 8) |
               (static_cast<uint32_t>(buffer[2]) << 16) |
               (static_cast<uint32_t>(buffer[3]) << 24);
    }

    uint64_t DecodeFixed64(const char *ptr) {
        const uint8_t *const buffer = reinterpret_cast<const uint8_t *>(ptr);

        return (static_cast<uint64_t>(buffer[0])) |
               (static_cast<uint64_t>(buffer[1]) << 8) |
               (static_cast<uint64_t>(buffer[2]) << 16) |
               (static_cast<uint64_t>(buffer[3]) << 24) |
               (static_cast<uint64_t>(buffer[4]) << 32) |
               (static_cast<uint64_t>(buffer[5]) << 40) |
               (static_cast<uint64_t>(buffer[6]) << 48) |
               (static_cast<uint64_t>(buffer[7]) << 56);
    }


    void PutFixed32(uint32_t value, std::string *dst) {
        char buf[sizeof(value)];
        EncodeFixed32(buf, value);
        dst->append(buf, sizeof(buf));
    }

    void PutFixed64(uint64_t value, std::string *dst) {
        char buf[sizeof(value)];
        EncodeFixed64(buf, value);
        dst->append(buf, sizeof(buf));
    }
};