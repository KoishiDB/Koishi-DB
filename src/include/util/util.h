#ifndef KOISHIDB_SRC_INCLUDE_UTIL_UTIL_H
#define KOISHIDB_SRC_INCLUDE_UTIL_UTIL_H

#include "common/common.h"
#include "type/slice.h"
#include "util/encode.h"
#include <cstdlib>
#include <ctime>

namespace koishidb {


    // use Slice rather Slice& to make the metable_key untouched
    inline void ExtractInternalKey(Slice memtable_key, Slice* internal_key) {
        // In fact, metable_key is the entry that inserted into the skiplist
        uint32_t internal_key_len;
        GetVarint32(&memtable_key, &internal_key_len);
        GetFixedBytes(&memtable_key, internal_key, internal_key_len);
    }

    inline void ExtractUserKey(const Slice& internal_key, Slice* user_key, SequenceNumber* number) {
        *user_key = Slice(internal_key.data(), internal_key.size() - 8); //
        *number = *reinterpret_cast<const SequenceNumber*>(internal_key.data()) >> 8;
    }

    // init to make skiplist node height random
    inline void rng() {
        srand(time(NULL));
    }
    inline int RandomHeight() {
        int height = 1;
        while (height < kSkipListNodeMaxLevel && rand() % 2 == 0) {
            height++;
        }
        return height;
    }


}

#endif