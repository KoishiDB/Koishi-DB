#include "util/util.h"
namespace koishidb {

    // need to add some test

    void ExtractInternalKey(Slice memtable_key, Slice* internal_key) {
        // In fact, metable_key is the entry that inserted into the skiplist
        uint32_t internal_key_len;
        GetVarint32(&memtable_key, &internal_key_len);
        GetFixedBytes(&memtable_key, internal_key, internal_key_len);
    }

    void ExtractUserKey(Slice& internal_key, Slice* user_key, SequenceNumber* number) {
        *user_key = Slice(internal_key.data(), internal_key.size() - 8);
        *number = (*reinterpret_cast<const SequenceNumber*>(internal_key.data() + user_key->size()) >> 8);
    }

    Slice CreateInternalKey(const Slice& user_key, SequenceNumber snapshot) {
        size_t usize = user_key.size();
        size_t needed = usize + 8;
        SequenceNumber tag = snapshot << 8 | static_cast<char>(KeyType::kTypeValue);
        // need to check here, user_key should not be too big
        char* dst = new char[needed];
        memcpy(dst, user_key.data(), usize);
        *reinterpret_cast<SequenceNumber *>(dst + usize) = tag;

        return Slice(dst, needed);
    }

    // Create a memtable key
    Slice CreateMemtableKey(const Slice& user_key, SequenceNumber snapshot) {
        size_t usize = user_key.size();
        size_t needed = usize + 13;  // A conservative size
        char *dst = new char[needed];
        size_t advance = EncodeVarint32(usize + 8, dst); // usize + 8 == internal_key length;
        memcpy(dst + advance, user_key.data(), usize);
        uint64_t tag = snapshot << 8 | static_cast<char>(KeyType::kTypeValue);
        const char* ptr = reinterpret_cast<const char*>(&tag);
        for (int i = 0; i < 8; i++) {
            // dst + advance + usize
            dst[advance + usize + i] = ptr[i];
        }
        size_t size = usize + 8 + advance;
        return Slice(dst, size);
    }

}