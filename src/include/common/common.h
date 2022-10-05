#ifndef KOISHIDB_SRC_INCLUDE_COMMON_COMMON_H
#define KOISHIDB_SRC_INCLUDE_COMMON_COMMON_H


// this file defines the constant that may be used in other files
namespace koishidb {
    constexpr int kSkipListNodeMaxLevel = 20;
    constexpr int kMemtableMaxSize = 4096; // max mutable memtable size
    // enum class
    enum class KeyType {
        kTypeDeletion,
        kTypeValue,
    };
};

#endif