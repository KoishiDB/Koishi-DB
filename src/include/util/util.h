#ifndef KOISHIDB_SRC_INCLUDE_UTIL_UTIL_H
#define KOISHIDB_SRC_INCLUDE_UTIL_UTIL_H

#include "common/common.h"
#include "type/slice.h"
#include "util/encode.h"
#include <cstdlib>
#include <ctime>

namespace koishidb {


    // use Slice rather Slice& to make the metable_key untouched
    void ExtractInternalKey(Slice metable_key, Slice* internal_key);

    void ExtractUserKey(Slice& internal_key, Slice* user_key, SequenceNumber* number);

    Slice CreateInternalKey(const Slice& user_key, SequenceNumber snapshot);

    Slice CreateMemtableKey(const Slice& user_key, SequenceNumber snapshot);

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