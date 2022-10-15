#include "memory/write_batch.h"


namespace koishidb {
    // WriteBatch has a header of  Sequence(4 bytes),count (4 bytes)
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
        entries.Advance(8);
        auto ProcessMemtableKey = [&](char key_type) -> Slice {
            std::string buffer;
            SequenceNumber tag = (number << 8) | key_type;
            number++;
            PutLengthPrefixedSlice(&buffer, entries);
            const char *tag_ptr = reinterpret_cast<const char *>(&tag);
            buffer.append(tag_ptr, 8);
            if (key_type == static_cast<char>(KeyType::kTypeValue)) {
                PutLengthPrefixedSlice(&buffer, entries);
            }
            return Slice(buffer);
        };
        for (int i = 0; i < count; i++) {
            std::string buffer;
            char key_type = entries.data()[0];
            entries.Advance(1); //
            Slice memtable_key = ProcessMemtableKey(key_type);
            memtable->Insert(memtable_key);
        }
    }


    //
    // ------------ WriteBatchInternal --------------------
    //
    int WriteBatchInternal::Count(const WriteBatch *w) {
        return DecodeFixed32(w->rep_.data() + 4);
    }

    void WriteBatchInternal::SetCount(WriteBatch *w, int count) {
        EncodeFixed32(&w->rep_[4], count);
    }

    SequenceNumber WriteBatchInternal::Sequence(WriteBatch *w) {
        return SequenceNumber(DecodeFixed32(w->rep_.data()));
    }

    void WriteBatchInternal::SetSequence(WriteBatch *w, SequenceNumber seq) {
        EncodeFixed32(w->rep_.data(), seq);
    }

    void WriteBatchInternal::Append(const WriteBatch *src, WriteBatch *dst) {
        SetCount(dst, Count(src) + Count(dst));
        assert(src->rep_.size() >= kWriteBatchHeader);
        dst->rep_.append(src->rep_.data() + kWriteBatchHeader,
                         src->rep_.size() - kWriteBatchHeader);
    }

};