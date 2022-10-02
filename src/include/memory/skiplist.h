#ifndef KOISHIDB_SRC_INCLUDE_MEMORY_SKIPLIST_H
#define KOISHIDB_SRC_INCLUDE_MEMORY_SKIPLIST_H

#include "memory/table.h"
namespace koishidb {
    template<typename K, typename V>
    class Skiplist: public Table<K, V> {
        explicit Skiplist();
        ~Skiplist() override = default;
        bool Get(const K& key) override;
        void Put(const K& key, const V& value) override;
        void Delete(const K& key) override;
    };
}

#endif
