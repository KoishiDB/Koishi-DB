#include "memory/skiplist.h"
#include "type/slice.h"

#include "logger.h"
// SkipList needs external synchronization

namespace koishidb {
    template<typename K, typename V>
    SkipList<K, V>::SkipList() {
        rng();
        head_ = new SkipList<K, V>::Node(kSkipListNodeMaxLevel);
    }

    template<typename K, typename V>
    SkipList<K, V>::~SkipList() {
        auto ptr = head_;
        auto cur = ptr->get_n_node(0);
        while (cur != nullptr) {
            auto next = cur->get_n_node(0);
            delete cur;
            cur = next;
        }
        delete head_;
    }
    
    template<typename K, typename V>
    typename SkipList<K, V>::Node* SkipList<K, V>::Find(const K &key) {
        auto ptr = head_;
        int cur_level = kSkipListNodeMaxLevel - 1;
        while (ptr != nullptr) {
            auto next_ptr = ptr->get_n_node(cur_level);
            if (next_ptr != nullptr && next_ptr->get_key() < key) {
                ptr = next_ptr;
                continue;
            }
            if (next_ptr != nullptr && next_ptr->get_key() == key) {
                return next_ptr;
            }
            if (cur_level > 0) {
                cur_level--;
            } else {
                break;
            }
        }
        return nullptr;
    }

    template<typename K, typename V>
    bool SkipList<K, V>::Get(const K &key, V& value) {
        auto ptr = Find(key);
        if (ptr == nullptr || ptr->get_key_type() == KeyType::kTypeDeletion) {
            return false;
        }
        value = ptr->get_value();
        return true;
    }

    template<typename K, typename V>
    void SkipList<K, V>::Put(const K &key, const V &value) {
        auto ptr = Find(key);
        if (ptr != nullptr) {
            ptr->set_key_type(KeyType::kTypeValue);
            ptr->set_value(value);
            return;
        }
        auto new_node = new SkipList<K, V>::Node(key, value, KeyType::kTypeValue);
        ptr = head_;
        int cur_level = new_node->get_level() - 1;
        while (cur_level >= 0) {
            auto next_ptr = ptr->get_n_node(cur_level);
            if (next_ptr != nullptr && next_ptr->get_key() < key) {
                ptr = next_ptr;
                continue;
            }
            ptr->set_n_node(cur_level, new_node);
            new_node->set_n_node(cur_level, next_ptr);
            cur_level--;
        }
    }

    template<typename K, typename V>
    void SkipList<K, V>::Delete(const K &key) {
        auto ptr = Find(key);
        if (ptr != nullptr) {
            ptr->set_key_type(KeyType::kTypeDeletion);
            return;
        }
        V tmp;
        auto new_node = new SkipList<K, V>::Node(key, tmp, KeyType::kTypeDeletion);
        ptr = head_;
        int cur_level = new_node->get_level() - 1;
        while (cur_level >= 0) {
            auto next_ptr = ptr->get_n_node(cur_level);
            if (next_ptr != nullptr && next_ptr->get_key() < key) {
                ptr = next_ptr;
                continue;
            }
            ptr->set_n_node(cur_level, new_node);
            new_node->set_n_node(cur_level, next_ptr);
            cur_level--;
        }
    }

    // the template class that might be used
    template class SkipList<int, int>;
    template class SkipList<std::string, std::string>;
    template class SkipList<Slice, Slice>;
};