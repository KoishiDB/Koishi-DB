#include "memory/memtable.h"
#include "gtest/gtest.h"

namespace koishidb {
    TEST(memtable_test, MemtableBasicTest) {
        Memtable memtable;
        for (int i = 0; i < 10; i++) {
          std::string key = "user_key" + std::to_string(i);
          std::string value = "user_value" + std::to_string(i);
          memtable.Insert(CreateMemtableKey(key, 0, value, KeyType::kTypeValue));
        }

        for (int i = 0; i < 10; i++) {
            std::string key = "user_key" + std::to_string(i);
            std::string value = "user_value" + std::to_string(i);
            std::string cur;
            memtable.Get(CreateMemtableKey(key, 0, "", KeyType::kTypeValue), &cur);
            EXPECT_EQ(Slice(cur), CreateMemtableKey(key, 0, value, KeyType::kTypeValue));
        }
    }
};