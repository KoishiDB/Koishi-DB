#ifndef KOISHIDB_SRC_INCLUDE_MEMORY_SKIPLIST_H
#define KOISHIDB_SRC_INCLUDE_MEMORY_SKIPLIST_H

#include "common/common.h"
#include "util/util.h"
#include "type/slice.h"
//#include "logger.h"

#include <vector>
#include <memory>
#include <cstdlib>
#include <cassert>
#include <shared_mutex>
#include <utility>

namespace koishidb {

    // if you want to create a skiplist object
    // use construct: SkipList(Comparator)
    template<typename K, typename Comparator>
    class SkipList {
    public:
        SkipList(const SkipList& that) = delete;
        SkipList& operator=(const SkipList& that) = delete;
        explicit SkipList(Comparator cmp);
        ~SkipList();
        void Insert(const K& memtable_key);
        bool FindFirstGreaterOrEqual(const K& memtable_key, K* result) const ;
        struct Node {
        public:

            const K& Key() const { return memtable_key_; }
            int get_level() const { return level; }
            Node(K memtable_key) : memtable_key_(memtable_key) {
                level = RandomHeight();
                next.assign(level, nullptr);
            }
            // only be used with the dummy node;
            Node(int level) {
                this->level = level;
                next.assign(level, nullptr);
            }
            SkipList<K, Comparator>::Node* get_n_node(int n) const  {
                assert(n < level);
                return next[n];
            }
            void set_n_node(int n, SkipList<K, Comparator>::Node* node)  {
                assert(n < level);
                next[n] = node;
            }
        private:
            K memtable_key_;
            int level;
            std::vector<SkipList<K, Comparator>::Node*> next; // contains the next node
        };

        class Iterator {
        public:
            // Inner can remove the typename qualifier
            Iterator(const Iterator& that) = default;
            Iterator& operator=(const Iterator& that) = default;
            Iterator(const SkipList<K, Comparator>* list): list_(list) { node_ = list->head_->get_n_node(0); }
            ~Iterator() = default;

            K Key() const { return node_->Key(); };
            bool Valid() const { return node_ != nullptr; }

            Iterator& operator++() {
                node_ = node_->get_n_node(0);
                return *this;
            }
            Iterator operator++(int) {
                Iterator tmp = *this;
                ++*this;
                return tmp;
            }
            const K& operator*() const {
                return node_->Key();
            }
            Iterator Next() {
                Iterator tmp = *this;
                ++tmp;
                return tmp;
            }

        private:
          Node* node_;
          const SkipList* list_;
        };


    private:
        Comparator cmp_;
        std::shared_mutex rwlock_;
        SkipList<K, Comparator>::Node* head_;
        size_t size_;
    };

    template<typename K, typename Comparator>
    SkipList<K, Comparator>::SkipList(Comparator cmp) : cmp_(cmp) {
        head_ = new SkipList<K, Comparator>::Node(kSkipListNodeMaxLevel);
    }

    template<typename K, typename Comparator>
    SkipList<K, Comparator>::~SkipList() {
        auto ptr = head_;
        auto cur = ptr->get_n_node(0);
        while (cur != nullptr) {
            auto next = cur->get_n_node(0);
            delete cur;
            cur = next;
        }
        delete head_;
    }

    template<typename K, typename Comparator>
    void SkipList<K, Comparator>::Insert(const K &memtable_key) {
        
        // if we could make sure that the key inserted is unique
        // we don't need the following code;
        // K result;
        // bool ok = FindFirstGreaterOrEqual(memtable_key, &result);
        // if (ok == true && cmp_(result, memtable_key) == 0) {
        //     return;
        // }

        auto new_node = new SkipList<K, Comparator>::Node(memtable_key);
        auto ptr = head_;
        int cur_level = new_node->get_level() - 1;
        while (cur_level >= 0) {
            auto next_ptr = ptr->get_n_node(cur_level);
            if (next_ptr != nullptr && cmp_(next_ptr->Key(), memtable_key) < 0) {
                ptr = next_ptr;
                continue;
            }
            ptr->set_n_node(cur_level, new_node);
            new_node->set_n_node(cur_level, next_ptr);
            cur_level--;
        }
    }

    // if there is no element meet the requirement return false
    // else true
    template<typename K, typename Comparator>
    bool SkipList<K, Comparator>::FindFirstGreaterOrEqual(const K &memtable_key, K *result) const {
        auto ptr = head_;
        int cur_level = kSkipListNodeMaxLevel - 1;
        while (ptr != nullptr) {
            auto next_ptr = ptr->get_n_node(cur_level);
            if (next_ptr != nullptr && cmp_(next_ptr->Key(), memtable_key) < 0) {
                ptr = next_ptr;
                continue;
            }
            if (next_ptr != nullptr && cmp_(next_ptr->Key(), memtable_key) == 0) {
                *result = next_ptr->Key();
                return true;
            }

            if (cur_level == 0) {
                if (next_ptr != nullptr) {
                    *result = next_ptr->Key();
                    return true;
                }
                break;
            } else {
                cur_level--;
            }
        }

        return false;
    }

}

#endif
