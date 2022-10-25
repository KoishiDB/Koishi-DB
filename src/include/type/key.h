#ifndef KOISHIDB_SRC_INCLUDE_TYPE_KEY_H
#define KOISHIDB_SRC_INCLUDE_TYPE_KEY_H

#include "type/slice.h"
#include "common/common.h"
#include "util/encode.h"
#include "util/util.h"
#include "util/comparator.h"

#include <string>

namespace koishidb {

    // We don't need to Look up key, we can use the CreateMemtableKey->


    // InternalKey Comparator
    class InternalKey {
    public:
        InternalKey() = default;
        InternalKey(const std::string& rep);
        InternalKey(const Slice& user_key, SequenceNumber sequence, KeyType type);
        InternalKey& operator=(const InternalKey& that) = default;

        std::string ToString() const;
        Slice ToSlice() const;
        void Clear();
        bool Valid() const;
    private:
        std::string rep_;
    };
    InternalKey TransToInternalKey(const Slice& key);

    class InternalKeyComparator : public Comparator {
    private:
        const Comparator* internal_key_comparator_;
    public:
        explicit InternalKeyComparator(const Comparator* c) : internal_key_comparator_(c) {}
        std::string Name() const override;
        int Compare(const Slice& a, const Slice& b) const override;

        const Comparator* internal_key_comparator() const { return internal_key_comparator_; }

        int Compare(const InternalKey& a, const InternalKey& b) const;
    }; // class InternalKeyComparator
};


#endif