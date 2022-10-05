#ifndef KOISHIDB_SRC_INCLUDE_MEMORY_SKIPLIST_H
#define KOISHIDB_SRC_INCLUDE_MEMORY_SKIPLIST_H

#include "memory/table.h"
#include "common/common.h"
#include "util/util.h"


#include <vector>
#include <memory>
#include <cstdlib>
#include <cassert>

namespace koishidb {

    // key should implement std::less() to compare
    template<typename K, typename V>
    class SkipList: public Table<K, V> {
    public:
        explicit SkipList();
        ~SkipList() override = default;
        bool Get(const K& key, V& value) override;
        void Put(const K& key, const V& value) override;
        void Delete(const K& key) override;
        size_t EstimatedSize() override; // estimate the size that the space when the skiplist dump to the disk
        // SkipList Node
        struct Node {
        public:
            K get_key() const { return key; }
            V get_value() const { return value; }
            KeyType get_key_type() const { return type; }
            int get_level() const { return level; }
            void set_key_type(const KeyType& type) { this->type = type; }
            void set_value(const V& value) { this->value = value; }

            Node(K key, V value, KeyType type) : key(key), value(value), type(type) {
                level = RandomHeight();
                next.assign(level, nullptr);
            }
            // only be used with the dummy node;
            Node(int level) {
                this->level = level;
                next.assign(level, nullptr);
            }
            std::shared_ptr<SkipList<K, V>::Node>& get_n_node(int n)  {
                assert(n < level);
                return next[n];
            }

        private:
            K key;
            V value;
            KeyType type;
            int level;
            std::vector<std::shared_ptr<SkipList<K, V>::Node>> next; // contains the next node
        };
    private:
        std::shared_ptr<SkipList<K, V>::Node> head_;
        size_t size_;
        std::shared_ptr<SkipList<K, V>::Node> Find(const K& key);
    };
}



#endif
