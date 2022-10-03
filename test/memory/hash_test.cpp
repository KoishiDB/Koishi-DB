#include "memory/hash.h"

#include "gtest/gtest.h"

namespace koishidb {

    TEST(Starter_test, HashTableTest) {
        koishidb::HashTable hash_table;
        hash_table.Put(1, 2);
        EXPECT_EQ(hash_table.Get(1), 2);
    }

};