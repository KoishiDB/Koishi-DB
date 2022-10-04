#ifndef KOISHIDB_SRC_INCLUDE_MEMORY_EXTENDIBLE_HASH_H
#define KOISHIDB_SRC_INCLUDE_MEMORY_EXTENDIBLE_HASH_H

#include "memory/table.h"
namespace koishidb {
    // the same namespace
    template<typename K, typename V>
    class ExtendibleHashTable: Table<K, V> {
        explicit ExtendibleHashTable();
        ~ExtendibleHashTable() override = default;

        bool Get(const K& key) override;
        void Put(const K& key, const V& value) override;
        void Delete(const K& key) override;
    };
};


#endif //KOISHIDB_SRC_INCLUDE_EXTENDIBLE_HASH_H
