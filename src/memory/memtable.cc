#include "memory/memtable.h"
#include "common/common.h"

namespace koishidb {

    // We don't need the memtable Iterator to seek for key, we just need it to iterate the memtable and dump it to
    // disk
    class MemtableIterator: public Iterator {
    public:

        ~MemtableIterator() = default;

        MemtableIterator(Memtable::Table* table): iter_(table) {}
        bool Valid() const override {
            return iter_.Valid();
        }
        void Next() override {
            iter_++;
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


    private:
        // private name
        Memtable::Table::Iterator iter_;
        // this function is useless, we don't and should not use it
        bool Seek(const Slice& target) override {
            return false;
        }

        void SeekToLast() override {}

        void Prev() override {}
    };


    bool Memtable::Get(const Slice& memtable_key, std::string* result) {
        Slice ret;
        bool flag = table_->FindFirstGreaterOrEqual(memtable_key, &ret);
        if (flag == false) {
            return false;
        }
        ExtractValueFromMemtable(ret, result);
        return true;
    }

    // We should always use the CreateMemtableKey to create memtable_key and insert it into the
    // memtable, for we will call the delete method in BuildTable function
    void Memtable::Insert(const Slice& memtable_key) {
        table_->Insert(memtable_key);
    }

    Iterator* Memtable::NewIterator() {
        Iterator* iter = new MemtableIterator(table_);
        return iter;
    }
};