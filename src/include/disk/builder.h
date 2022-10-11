#ifndef KOISHIDB_SRC_INCLUDE_DISK_BUILDER_H
#define KOISHIDB_SRC_INCLUDE_DISK_BUILDER_H

#include <string>
#include "disk/version.h"
#include "util/iterator.h"

namespace koishidb {
    // use an iterator to build table
    // maybe add some options later
    void BuildTable(const std::string& db_name, FileMeta* file_meta, Iterator* iter);
};
#endif