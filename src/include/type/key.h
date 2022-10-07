#ifndef KOISHIDB_SRC_INCLUDE_TYPE_KEY_H
#define KOISHIDB_SRC_INCLUDE_TYPE_KEY_H

#include "type/slice.h"
#include "common/common.h"
#include "util/encode.h"
#include "util/util.h"

namespace koishidb {

    // format
    // internal_key_len varint
    // internal_key ...
    // value_len ...
    // value
    struct Entry {
        Slice internal_key; // 进行一个大修改 //
        Slice value;
    };


    // parsed from
    // TODO: need some test to test parser
    inline Entry ParsedFromSlice(Slice* s) {
        Entry e;
        uint32_t len;
        GetVarint32(s, &len);
        GetFixedBytes(s, &e.internal_key, len);

        GetVarint32(s, &len);
        GetFixedBytes(s, &e.value, len);

    }

    // use a std::string buffer, append
    inline void ParsedIntoBuffer(std::string* buffer, Entry& e) {
        PutVarint32(e.internal_key.size(), buffer);
        buffer->append(e.internal_key.data());
        PutVarint32(e.value.size(), buffer);
        buffer->append(e.value.data());
};


#endif