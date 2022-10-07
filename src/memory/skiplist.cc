#include "memory/skiplist.h"
#include "type/slice.h"

#include "logger.h"
// SkipList needs external synchronization

namespace koishidb {
    template<typename K, typename Comparator>
    SkipList<K, Comparator>::SkipList(Comparator cmp) : cmp_(cmp) {
        head_ = std::make_shared<SkipList<K, Comparator>::Node>(kSkipListNodeMaxLevel);
    }
//    template<typename K, typename V>
//    std::shared_ptr<typename SkipList<K, V>::Node> SkipList<K, V>::Find(const K &key) {
//        auto ptr = head_;
//        int cur_level = kSkipListNodeMaxLevel - 1;
//        while (ptr != nullptr) {
//            auto next_ptr = ptr->get_n_node(cur_level);
//            if (next_ptr != nullptr && next_ptr->get_key() < key) {
//                ptr = next_ptr;
//                continue;
//            }
//            if (next_ptr != nullptr && next_ptr->get_key() == key) {
//                return next_ptr;
//            }
//            if (cur_level > 0) {
//                cur_level--;
//            } else {
//                break;
//            }
//        }
//        return nullptr;
//    }
//
//    //  >= 大于  >=
    template<typename K, typename Comparator>
    bool SkipList<K, Comparator>::Insert(const Slice &memtable_key) {
        // TODO()
    }

    template<typename K, typename Comparator>
    bool SkipList<K, Comparator>::FindFirstGreaterOrEqual(Slice &memtable_key, Slice *result) {
        // TODO()
    }


    // the template class that might be used
//    template class SkipList<int, int>;
//    template class SkipList<std::string, std::string>;
//    template class SkipList<Slice, Slice>;
};