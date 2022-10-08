#ifndef KOISHIDB_SRC_INCLUDE_MEMORY_SKIPLIST_H
#define KOISHIDB_SRC_INCLUDE_MEMORY_SKIPLIST_H

#include "common/common.h"
#include "util/util.h"
#include "type/slice.h"

#include <vector>
#include <memory>
#include <cstdlib>
#include <cassert>
#include <shared_mutex>

namespace koishidb {

    // key should implement std::less() to compare
    template<typename K, typename Comparator>
    class SkipList {
    public:
        explicit SkipList(Comparator cmp);
        ~SkipList() = default;
        bool Insert(const Slice& memtable_key);
        // user_key -> sequence
        bool FindFirstGreaterOrEqual(Slice& memtable_key, Slice* result);
        struct Node {
        public:

            Slice Key() const { return memtable_key_; }
            int get_level() const { return level; }
            Node(Slice memtable_key) : memtable_key_(memtable_key) {
                level = RandomHeight();
                next.assign(level, nullptr);
            }
            // only be used with the dummy node;
            Node(int level) {
                this->level = level;
                next.assign(level, nullptr);
            }
            std::shared_ptr<SkipList<K, Comparator>::Node>& get_n_node(int n)  {
                assert(n < level);
                return next[n];
            }

        private:
            Slice memtable_key_;
            int level;
            std::vector<std::shared_ptr<SkipList<K, Comparator>::Node>> next; // contains the next node
        };
    private:
        Comparator cmp_;
        std::shared_mutex rwlock_;
        std::shared_ptr<SkipList<K, Comparator>::Node> head_;
        size_t size_;
    };
}



#endif
