#ifndef KOISHIDB_SRC_INCLUDE_UTIL_UTIL_H
#define KOISHIDB_SRC_INCLUDE_UTIL_UTIL_H

#include "common/common.h"
#include "type/slice.h"
#include "util/encode.h"
#include <cstdlib>
#include <ctime>

namespace koishidb {

    // A helper class for divide the memtable_key part
    class LookupKey {
    public:
        LookupKey(const Slice& memtable_key): data_(memtable_key.data()), size_(memtable_key.size()) {
            Slice tmp = memtable_key;
            uint32_t internal_key_len;
            GetVarint32(&tmp, &internal_key_len);
            kstart_ = tmp.data() - data_;
            kend_ = kstart_ + internal_key_len;
            tmp.Advance(internal_key_len);
            uint32_t value_len;
            GetVarint32(&tmp, &value_len);
            vstart_ = tmp.data() - data_;
        }
        Slice MemtableKey() {
            return Slice(data_, size_);
        }
        Slice InternalKey() {
            return Slice(data_ + kstart_, kend_ - kstart_);
        }
        Slice UserKey() {
            return Slice(data_ + kstart_, kend_ - kstart_ - 8); // Sub the 8 bytes of tag
        }
        Slice Value() {
            return Slice(data_ + vstart_, size_ - vstart_);
        }

        SequenceNumber GetSequence() {
            return (*reinterpret_cast<const SequenceNumber*> (data_ + kend_ - 8) >> 8);
        }

        KeyType GetKeyType() {
            uint64_t tag = *reinterpret_cast<const SequenceNumber*>(data_ + kend_ - 8);
            tag = tag & (0xff);
            if (tag == 0x01) {
                return KeyType::kTypeValue;
            } else {
                return KeyType::kTypeDeletion;
            }
        }

    private:
        const char* data_;
        const size_t size_;
        uint32_t kstart_; // key start
        uint32_t kend_; // key end
        uint32_t vstart_; // value start

    };
    // use Slice rather Slice& to make the metable_key untouched
    void ExtractInternalKey(Slice metable_key, Slice* internal_key);

    void ExtractUserKey(const Slice& internal_key, Slice* user_key, SequenceNumber* number);

    Slice CreateInternalKey(const Slice& user_key, SequenceNumber snapshot, KeyType type);

    Slice CreateMemtableKey(const Slice& user_key, SequenceNumber snapshot, const Slice& value, KeyType type);


    // init to make skiplist node height random
    inline void rng() {
        srand(time(NULL));
    }
    inline int RandomHeight() {
        int height = 1;
        while (height < kSkipListNodeMaxLevel && rand() % 2 == 0) {
            height++;
        }
        return height;
    }

}

#endif