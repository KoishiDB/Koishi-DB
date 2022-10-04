#include "memory/skiplist.h"

// SkipList needs external synchronization

namespace koishidb {
    template<typename K, typename V>
    SkipList<K, V>::SkipList() {
        head_ = std::make_shared<SkipList<K, V>::Node>(kSkipListNodeMaxLevel);
    }

    template<typename K, typename V>
    std::shared_ptr<typename SkipList<K, V>::Node> SkipList<K, V>::Find(const K &key) {
        auto ptr = head_;
        while (true) {
            if (ptr->get_key() == key) {
                return ptr;
            }
            int i;
            for (int i = ptr->get_level() - 1; i >= 0; i--) {
                auto next_ptr = ptr->get_n_node(i);
                if (next_ptr->get_key() < key) {
                    ptr = next_ptr;
                    break;
                }
            }
            if (i == -1) {
                break;
            }
        }
        return nullptr;
    }

    template<typename K, typename V>
    bool SkipList<K, V>::Get(const K &key, const V& value) {
        auto ptr = Find(key);
        if (ptr == nullptr || ptr->get_key_type() == KeyType::kTypeDeletion) {
            return false;
        }
        value = ptr->get_value();
    }

    template<typename K, typename V>
    void SkipList<K, V>::Put(const K &key, const V &value) {
        auto ptr = Find(key);
        if (ptr != nullptr) {
            ptr->set_key_type(KeyType::kTypeValue);
            ptr->set_value(value);
            return;
        }
        auto new_node = std::make_unique<SkipList<K, V>::Node>(key, value, KeyType::kTypeValue);
        // insert
        ptr = head_;
        for (int i = new_node->get_level() - 1; i >= 0; i--) {
            while (true) {
                auto next_ptr = ptr->get_n_node(i);
                while (next_ptr != nullptr && next_ptr->get_key() < key) {
                    ptr = next_ptr;
                }
                // if next_ptr.get_key() >
                if (next_ptr == nullptr) {
                    ptr->get_n_node(i) = new_node;
                    break;
                } else {
                    // next_ptr->key > key
                    // ptr->key < key
                    ptr->get_n_node(i) = new_node;
                    new_node->get_n_node(i) = next_ptr;
                }
            }
        }
    }

    template<typename K, typename V>
    void SkipList<K, V>::Delete(const K &key) {
        auto ptr = Find(key);
        if (ptr != nullptr) {
            ptr->set_key_type(KeyType::kTypeDeletion);
        }
    }
};