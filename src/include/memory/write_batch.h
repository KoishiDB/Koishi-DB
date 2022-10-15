#ifndef KOISHIDB_SRC_INCLUDE_MEMORY_WRITE_BATCH_H
#define KOISHIDB_SRC_INCLUDE_MEMORY_WRITE_BATCH_H

#include <string>
#include <vector>

#include "util/util.h"
#include "common/common.h"
#include "memory/memtable.h"

namespace koishidb {
    // WriteBatch format

    // kHeader = 8
    // sequence(4) | count(4) -> KeyType varint -> user_key varint_ value
    class WriteBatch {
    public:
        WriteBatch();

        WriteBatch(const WriteBatch &that) = default;

        WriteBatch &operator=(const WriteBatch &that) = default;

        ~WriteBatch();


        void Put(const Slice &k, const Slice &v);

        void Delete(const Slice &K);

        // iterate the rep_ to insert into the skiplist
        void InsertAll(Memtable *memtable);

        void Append(const WriteBatch &that);

        size_t EstimatedSize() { return rep_.size(); }

        void Clear();

    private:
        friend class WriteBatchInternal;

        std::string rep_;
    };

    // provide static member functions to hide the interface that we don't want to expose to outer
    class WriteBatchInternal {
    public:
        static int Count(const WriteBatch *w);

        static void SetCount(WriteBatch *w, int count);

        static SequenceNumber Sequence(WriteBatch *w);

        static void SetSequence(WriteBatch *w, SequenceNumber seq);

        static void Append(const WriteBatch *src, WriteBatch *dst);

        static void InsertIntoMemtable(WriteBatch *src, Memtable *memtable);

        static size_t Bytes(WriteBatch *w) { return w->rep_.size(); };

    };
};

#endif