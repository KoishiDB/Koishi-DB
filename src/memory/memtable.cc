#include "memory/memtable.h"
#include "common/common.h"

namespace koishidb {

    bool Memtable::Get(const Slice& memtable_key, std::string* result) {
       // TODO
    }

    void Memtable::Insert(const Slice& memtable_key) {
        table_->Insert(memtable_key);
    }
};