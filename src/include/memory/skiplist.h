#ifndef KOISHIDB_SRC_INCLUDE_MEMORY_SKIPLIST_H
#define KOISHIDB_SRC_INCLUDE_MEMORY_SKIPLIST_H

#include "memory/table.h"
#include "common/common.h"

#include <vector>
#include <memory>
#include <cstdlib>

namespace koishidb {

    // util function
    int RandomHeight() {
        int height = 1;
        while (height <= kSkipListNodeMaxLevel && rand() % 2 == 0) {
            height++;
        }
        return height;
    }

    // key should implement std::less() to compare
    template<typename K, typename V>
    class SkipList: public Table<K, V> {
    public:
        explicit SkipList();
        ~SkipList() override = default;
        bool Get(const K& key, const V& value) override;
        void Put(const K& key, const V& value) override;
        void Delete(const K& key) override;

        // SkipList Node
        struct Node {
        public:
            K get_key() const { return key; }
            V get_value() const { return value; }
            KeyType get_key_type() const { return type; }
            int get_level() const { return level; }
            void set_key_type(KeyType& type) { type = type; }
            void set_value(V& value) { value = value; }


            Node(K key, V value, KeyType type) : key(key), value(value), type(type) {
                level = RandomHeight();
                next.assign(level, nullptr);
            }
            // only be used with the dummy node;
            Node(int level) {
                level = level;
                next.assign(level, nullptr);
            }
            std::shared_ptr<SkipList<K, V>::Node> get_n_node(int n) const {
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
        std::shared_ptr<SkipList<K, V>::Node> Find(const K& key);
    };
}

#endif
