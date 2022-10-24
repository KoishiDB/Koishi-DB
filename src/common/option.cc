#include "common/option.h"
#include "memory/memtable.h"

namespace koishidb {
    Option::Option(): cmp(new MemtableKeyComparator()) {}
};