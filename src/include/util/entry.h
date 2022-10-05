#ifndef KOISHIDB_SRC_INCLUDE_UTIL_ENTRY_H
#define KOISHIDB_SRC_INCLUDE_UTIL_ENTRY_H

#include "type/slice.h"
#include "common/common.h"
#include "util/encode.h"

namespace koishidb {

// entry is (keyType, key, value) tuple
// the format that should be stored in the memtable is as below
// keyType 1byte (1 means KeyValue, 2 means KeyValueDeletion)
// key_len varint
// key key_len bytes
// value_len varint
// value value_len bytes
    struct Entry {
        KeyType type;
        std::string key;
        std::string value;
    };
    // parsed from
    inline Entry ParsedFromSlice(Slice* s) {
        Entry e;
        e.type = (*s)[0] == 1 ? KeyType::kTypeValue : KeyType::kTypeDeletion;
        s->Advance(1);
        uint32_t len;
        GetVarint32(s, &len);
        GetFixedBytes(s, &e.key, len);

        GetVarint32(s, &len);
        GetFixedBytes(s, &e.value, len);
    }

    // use a std::string buffer
    inline void ParsedIntoBuffer(std::string* buffer, Entry& e) {
        buffer->push_back(e.type == KeyType::kTypeValue ? 1 : 0);
        PutVarint32(e.key.size(), buffer);
        buffer->append(e.key);
        PutVarint32(e.value.size(), buffer);
        buffer->append(e.value);
    }
};


#endif