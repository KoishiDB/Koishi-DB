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
        ~SkipList();
        bool Get(const K& key, V& value) override;
        void Put(const K& key, const V& value) override;
        void Delete(const K& key) override;
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
            SkipList<K, V>::Node* get_n_node(int n)  {
                assert(n < level);
                return next[n];
            }
            void set_n_node(int n, SkipList<K, V>::Node* node) {
                assert(n < level);
                next[n] = node;
            }
        private:
            K key;
            V value;
            KeyType type;
            int level;
            std::vector<SkipList<K, V>::Node*> next; // contains the next node
        };

        struct Iterator {
        public:
            Iterator();
            Iterator(const Iterator& that) = default;
            Iterator& operator=(const Iterator& that) = default;
            Iterator(const SkipList<K, V>& list) { node = list.head_->get_n_node(0); }
            ~Iterator() = default;

            K Key() { return node->get_key(); };
            V Value() { return node->get_value(); } 
            KeyType Type() { return node->get_key_type(); }
            bool Valid() { return node != nullptr; }
            
            Iterator& operator++() {
                node = node->get_n_node(0);
                return *this;
            }
            Iterator operator++(int) {
                Iterator tmp = *this;
                ++*this;
                return tmp;
            }
            Iterator Next() {
                Iterator tmp = *this;
                ++tmp;
                return tmp;
            }

        private:
            SkipList<K, V>::Node* node;
        };
    private:
        SkipList<K, V>::Node* head_;
        SkipList<K, V>::Node* Find(const K& key);
    };
}



#endif
