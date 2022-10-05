#ifndef KOISHIDB_SRC_INCLUDE_UTIL_UTIL_H
#define KOISHIDB_SRC_INCLUDE_UTIL_UTIL_H

#include "common/common.h"
#include <cstdlib>
#include <ctime>

namespace koishidb {
    // init the random seed
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