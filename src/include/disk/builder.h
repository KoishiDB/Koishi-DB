#ifndef KOISHIDB_SRC_INCLUDE_DISK_BUILDER_H
#define KOISHIDB_SRC_INCLUDE_DISK_BUILDER_H

#include <string>

#include "common/status.h"
#include "disk/format.h"
#include "util/iterator.h"

namespace koishidb {
// use an iterator to build table
// maybe add some options later
// Generate a table and write the file meta to the manifest
Status BuildTable(const std::string& file_name, FileMeta* file_meta,
                  Iterator* iter);
};  // namespace koishidb
#endif