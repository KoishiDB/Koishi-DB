#include "memory/memtable.h"
#include "common/common.h"

namespace koishidb {

    // Look how the iterator design
    class MemtableIterator: public Iterator {
    public:
        MemtableIterator(Memtable::Table* table): iter_(table) {}
        bool Valid() const override {
            return iter_.Valid();
        }
        void Next() override {
            iter_++;
        }
        void Prev() override {
           // we don't need this method in memtable iterator
           // iff need, we can append it later
        }
        Slice Key() const override {
          Slice memtable_key = iter_.Key();
          Slice internal_key;
          ExtractInternalKey(memtable_key, &internal_key);
          return internal_key;
        }
        Slice Value() const override {
            Slice memtable_key = iter_.Key();
            uint32_t internal_key_len;
            GetVarint32(&memtable_key, &internal_key_len);
            memtable_key.Advance(internal_key_len);
            uint32_t  value_len;
            GetVarint32(&memtable_key, &value_len);
            return Slice(memtable_key.data(), value_len);
        }
        void SeekToFirst() override {
            assert(iter_.Valid());
        }
        void SeekToLast() override {
            // we don't need this method currently, we can do it later;
        }
    private:
        // private name
        Memtable::Table::Iterator iter_;
    };


    bool Memtable::Get(const Slice& memtable_key, std::string* result) {
        Slice ret;
        bool flag =  table_->FindFirstGreaterOrEqual(memtable_key, &ret);
        if (flag == false) {
            return false;
        }
        result->append(ret.data(), ret.size());
        return true;
    }

    void Memtable::Insert(const Slice& memtable_key) {
        table_->Insert(memtable_key);
    }

    Iterator* Memtable::NewIterator() {
        Iterator* iter = new MemtableIterator(table_);
        return iter;
    }
};