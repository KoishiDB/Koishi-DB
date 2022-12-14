#include "memory/write_batch.h"
#include "util/encode.h"

namespace koishidb {
    // WriteBatch has a header of  Sequence(8 bytes),count (4 bytes)
    WriteBatch::WriteBatch() {
        Clear();
    }

    void WriteBatch::Put(const Slice &key, const Slice &value) {
        assert(rep_.size() >= kWriteBatchHeader);
        WriteBatchInternal::SetCount(this, WriteBatchInternal::Count(this) + 1);
        rep_.push_back(static_cast<char>(KeyType::kTypeValue));
        PutLengthPrefixedSlice(&rep_, key);
        PutLengthPrefixedSlice(&rep_, value);
    }

    void WriteBatch::Delete(const Slice &key) {
        assert(rep_.size() >= kWriteBatchHeader);
        WriteBatchInternal::SetCount(this, WriteBatchInternal::Count(this) + 1);
        rep_.push_back(static_cast<char>(KeyType::kTypeDeletion));
        PutLengthPrefixedSlice(&rep_, key);
    }

    void WriteBatch::Clear() {
        rep_.clear();
        rep_.resize(kWriteBatchHeader);
    }

    void WriteBatch::Append(const WriteBatch &that) {
        WriteBatchInternal::Append(&that, this);
    };




    void WriteBatch::InsertAll(Memtable *memtable) {
        int count = WriteBatchInternal::Count(this);
        SequenceNumber number = WriteBatchInternal::Sequence(this);
        Slice entries(rep_);
        // skip the header (12 bytes)
        entries.Advance(kWriteBatchHeader);
        auto ProcessMemtableKey = [&](char key_type) -> Slice {
            std::string memtable_key;
            number++;
            switch (static_cast<KeyType>(key_type)) {
                case KeyType::kTypeValue: {
                    // internal_key =  | real_key | tag(8) |
                    std::string user_key;
                    GetLengthPrefixedSlice(&user_key, &entries);
                    std::string value;
                    GetLengthPrefixedSlice(&value, &entries);
                    // | len(varint)| internal_key | len(varint) | value |
                    return CreateMemtableKey(user_key.data(), number, value.data(), KeyType::kTypeValue);
                }
                case KeyType::kTypeDeletion: {
                    std::string user_key;
                    GetLengthPrefixedSlice(&user_key, &entries);
                    return  CreateMemtableKey(user_key.data(), number, " ", KeyType::kTypeDeletion);
                }
                default: {
                    // Panic！
                    return {};
                }
            }
        };
        for (int i = 0; i < count; i++) {
            char key_type = entries.data()[0]; // see Put and Delete , it's a char now
            entries.Advance(1);
            Slice memtable_key = ProcessMemtableKey(key_type);
            if (memtable_key.size() != 0) {
                memtable->Insert(memtable_key);
            }
        }
    }
    size_t WriteBatch::EstimatedSize() {
        return rep_.size();
    }
    
    


    // This part is created by Ciro Long and Modified by xincheng
    //
    // ------------ WriteBatchInternal --------------------
    //
    int WriteBatchInternal::Count(const WriteBatch *w) {
        return static_cast<int>(DecodeFixed32(w->rep_.data() + 8));
    }

    void WriteBatchInternal::SetCount(WriteBatch *w, int count) {
        EncodeFixed32(&w->rep_[8], count);
    }

    SequenceNumber WriteBatchInternal::Sequence(WriteBatch *w) {
        return SequenceNumber(DecodeFixed64(w->rep_.data()));
    }

    void WriteBatchInternal::SetSequence(WriteBatch *w, SequenceNumber seq) {
        EncodeFixed64(w->rep_.data(), seq);
    }

    void WriteBatchInternal::Append(const WriteBatch *src, WriteBatch *dst) {
        SetCount(dst, Count(src) + Count(dst));
        assert(src->rep_.size() >= kWriteBatchHeader);
        dst->rep_.append(src->rep_.data() + kWriteBatchHeader,
                         src->rep_.size() - kWriteBatchHeader);
    }

};