#ifndef KOISHIDB_SRC_INCLUDE_MEMORY_WRITE_BATCH_H
#define KOISHIDB_SRC_INCLUDE_MEMORY_WRITE_BATCH_H

#include <string>
#include <vector>

#include "util/util.h"
#include "common/common.h"

namespace koishidb {
    // WriteBatch to
    class WriteBatch {
    public:
        WriteBatch() {}

        WriteBatch(const WriteBatch& that) = default;

        WriteBatch& operator=(const WriteBatch& that) = default;

        void Put(const Slice& k, const Slice& v);

        void Delete(const Slice& K);

        void Append(const WriteBatch& that);
        // iterate the rep_ to insert into the skiplist
        bool Iterator();

        size_t EstimatedSize();
    private:

        std::string rep_;
    };
};

#endif