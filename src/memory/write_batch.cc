#include "memory/write_batch.h"


namespace koishidb {
    // WriteBatch has a header of  Sequence(8 bytes),count (4 bytes)
    WriteBatch::WriteBatch() {
        Clear();
    }

    WriteBatch::~WriteBatch() = default;

    void WriteBatch::Put(const Slice &key, const Slice &value) {
        WriteBatchInternal::SetCount(this, WriteBatchInternal::Count(this) + 1);
        rep_.push_back(static_cast<char>(KeyType::kTypeValue));
        PutLengthPrefixedSlice(&rep_, key);
        PutLengthPrefixedSlice(&rep_, value);
    }

    void WriteBatch::Delete(const Slice &key) {
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
            SequenceNumber tag = (number << 8) | key_type;
            number++; // the next record ?
            switch (static_cast<KeyType>(key_type)) {
                case KeyType::kTypeValue: {
                    // internal_key =  | real_key | tag(8) |
                    std::string internal_key;
                    GetPrefixedLengthData(&entries, &internal_key);
                    PutFixed64(tag, &internal_key);
                    std::string value;
                    GetPrefixedLengthData(&entries, &value);

                    // | len(varint)| internal_key | len(varint) | value |
                    PutLengthPrefixedSlice(&memtable_key, Slice(internal_key));
                    PutLengthPrefixedSlice(&memtable_key, Slice(value));
                    return {memtable_key}; //use a braced initializer list instead
                }
                case KeyType::kTypeDeletion: {
                    std::string internal_key;
                    PutFixed64(tag, &internal_key);
                    GetPrefixedLengthData(&entries, &internal_key);
                    PutLengthPrefixedSlice(&memtable_key, Slice(internal_key));
                    return {memtable_key}; //use a braced initializer list instead
                }
                default: {
                    // Panic！
                    return {};
                }
            }
        };
        for (int i = 0; i < count; i++) {
            std::string buffer;
            char key_type = entries.data()[0]; // see Put and Delete , it's a char now
            entries.Advance(1);
            Slice memtable_key = ProcessMemtableKey(key_type);
            if (memtable_key.size() != 0) {
                memtable->Insert(memtable_key);
            }
        }
    }


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