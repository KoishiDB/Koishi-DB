#include "memory/write_batch.h"


namespace koishidb {
    // WriteBatch has a header of count (4 bytes), Sequence(4 bytes)
    WriteBatch::WriteBatch() {
        rep_.resize(kWriteBatchHeader);
    }

    void WriteBatch::Put(const Slice& key, const Slice& value) {
        WriteBatchInternal::SetCount(this, WriteBatchInternal::Count(this) + 1);
        rep_.push_back(static_cast<char>(KeyType::kTypeValue));
        PutLengthPrefixedSlice(&rep_, key);
        PutLengthPrefixedSlice(&rep_, value);
    }

    void WriteBatch::Delete(const Slice& key) {
        WriteBatchInternal::SetCount(this, WriteBatchInternal::Count(this) + 1);
        rep_.push_back(static_cast<char>(KeyType::kTypeDeletion));
        PutLengthPrefixedSlice(&rep_, key);
    }

    void WriteBatch::Clear() {
        rep_.clear();
        rep_.resize(kWriteBatchHeader);
    }

    void WriteBatch::Append(const WriteBatch& that) {
        rep_.append(that.rep_.data() + 8, that.rep_.data() - 8);
    };

    void WriteBatch::InsertAll(Memtable* memtable) {
        int count = WriteBatchInternal::Count(this);
        SequenceNumber number = WriteBatchInternal::GetSequenceNumber(this);
        Slice entries(rep_);
        entries.Advance(8);
        // TODO, we should use the util function here
        auto ProcessMemtableKey = [&](char key_type) -> Slice {
            std::string buffer;
            SequenceNumber tag = (number << 8) | key_type;
            number++;
            PutLengthPrefixedSlice(&buffer, entries);
            const char* tag_ptr = reinterpret_cast<const char*>(&tag);
            buffer.append(tag_ptr, 8);
            if (key_type == static_cast<char>(KeyType::kTypeValue)) {
                PutLengthPrefixedSlice(&buffer, entries);
            }
            return Slice(buffer); //
        };
        for (int i = 0; i < count; i++) {
            std::string buffer;
            char key_type = entries.data()[0];
            entries.Advance(1); //
            Slice memtable_key = ProcessMemtableKey(key_type);
            memtable->Insert(memtable_key);
        }
    }
};