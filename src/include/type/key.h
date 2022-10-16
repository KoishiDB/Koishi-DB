#ifndef KOISHIDB_SRC_INCLUDE_TYPE_KEY_H
#define KOISHIDB_SRC_INCLUDE_TYPE_KEY_H

#include "type/slice.h"
#include "common/common.h"
#include "util/encode.h"
#include "util/util.h"

namespace koishidb {

    // We don't need to Look up key, we can use the CreateMemtableKey->


    class InternalKey {
        // TODO,
    public:
        InternalKey() = default;
        InternalKey(const Slice& user_key, SequenceNumber sequence, KeyType type);
    private:
        std::string rep_;
    };
    InternalKey TransToInternalKey(const Slice& key);
};


#endif