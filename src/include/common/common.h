#ifndef KOISHIDB_SRC_INCLUDE_COMMON_COMMON_H
#define KOISHIDB_SRC_INCLUDE_COMMON_COMMON_H

#include <cstddef>

// this file defines the constant that may be used in other files
namespace koishidb {
    constexpr int kSkipListNodeMaxLevel = 20;
    constexpr int kMemtableMaxSize = 4096; // max mutable memtable size
    constexpr int kWriteBatchReserveCapacity = 20;
    constexpr int kWriteBatchHeader = 8;
    constexpr int kL0MaxFiles = 10;// should be modified later
    constexpr int kMaxMemtableSize = 4096;

    using SequenceNumber = long long;

    // enum class
    enum class KeyType {
        kTypeValue = 0x01,
        kTypeDeletion = 0x02,
    };



};

#endif