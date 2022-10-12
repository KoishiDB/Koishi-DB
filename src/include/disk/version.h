#ifndef KOISHIDB_INCLUDE_DISK_VERSION_H
#define KOISHIDB_INCLUDE_DISK_VERSION_H
#include <type/key.h>

namespace koishidb {
    // file meta data
    struct FileMeta {
        uint64_t file_size;
        uint64_t number;
        int refs;
        InternalKey smallest_key;
        InternalKey largest_key;
    };

    // Version edit
};

#endif