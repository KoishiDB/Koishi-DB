#include "memory/memtable.h"
#include "common/common.h"

namespace koishidb {

    void Memtable::Insert(const Slice& memtable_key) {
        table_->Insert(memtable_key);
    }
};