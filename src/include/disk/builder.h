#ifndef KOISHIDB_SRC_INCLUDE_DISK_BUILDER_H
#define KOISHIDB_SRC_INCLUDE_DISK_BUILDER_H

#include <string>
#include "disk/format.h"
#include "util/iterator.h"
#include "common/status.h"

namespace koishidb {
    // use an iterator to build table
    // maybe add some options later
    // Generate a table and write the file meta to the manifest
    Status BuildTable(const std::string& file_name, FileMeta* file_meta, Iterator* iter);
};
#endif