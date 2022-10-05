#include "memory/memtable.h"
#include "common/common.h"

namespace koishidb {
    bool Memtable::MemtableGet(const std::string &key, std::string &value) {
        bool flag = table_->Get(key, value);
        return flag;
    }

    void Memtable::MemtablePut(const std::string &key, const std::string &value) {
        table_->Put(key, value);
    }

    void Memtable::MemtableDelete(const std::string &key) {
        table_->Delete(key);
    }

    size_t Memtable::EstimatedSize() {
        return table_->EstimatedSize();
    }

    // TODO: need synchronization here when access to mutable memtable
    bool MemtableView::Get(const std::string &key, std::string &value) {
        bool flag = memtable_->MemtableGet(key, value);
        if (flag == true) {
            return flag;
        }
        if (immutable_memtable_ == nullptr) {
            return false;
        }
        flag |= immutable_memtable_->MemtableGet(key, value);
        return true;
    }

    void MemtableView::Put(const std::string &key, const std::string &value) {
        // use condition variable to inform, do it later;
        // while (condition()) wait...
        //
        memtable_->MemtablePut(key, value);
        if (memtable_->EstimatedSize() >= kMemtableMaxSize) {
            immutable_memtable_ = memtable_;
            memtable_ = new Memtable(new SkipList<std::string, std::string>());
        }

    }

    void MemtableView::Delete(const std::string &key) {
        memtable_->MemtableDelete(key);
    }
};