#ifndef KOISHIDB_SRC_INCLUDE_DISK_BLOCK_BUILDER_H
#define KOISHIDB_SRC_INCLUDE_DISK_BLOCK_BUILDER_H

#include "type/slice.h"


namespace koishidb {
    struct Option;

    class BlockBuilder {
    public:
        // varint -> key_length
        // internal_key
        // varint -> value_
        // value
        // const can only be copied to const
        BlockBuilder(const Option* opt) : opt_(opt) {}

        void Add(const Slice& key, const Slice& value);

        size_t EstimatedSize();
        // By invoke this to finish build BlockBuilder
        Slice Finish();

        void Reset();
    private:
        std::string rep_;
        std::string last_key_; // Add the new key should
        const Option* opt_;
        bool is_finished_;
    };
};

#endif