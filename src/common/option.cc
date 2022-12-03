#include "common/option.h"
#include "memory/memtable.h"
#include "util/comparator.h"

namespace koishidb {
    Option::Option(): cmp(new MemtableKeyComparator()) {}
    // To create a cmp->
    Option::~Option() {
        if (cmp != nullptr) {
            delete cmp;
        }
    }
    Option::Option(const Comparator* cmp): cmp(cmp) {}
};