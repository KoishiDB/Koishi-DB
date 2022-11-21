#include "common/option.h"
#include "memory/memtable.h"

namespace koishidb {
    Option::Option(): cmp(new MemtableKeyComparator()) {}
    // To create a cmp->
    Option::Option(const Comparator* cmp): cmp(cmp) {}
};