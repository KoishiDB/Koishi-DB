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
    EXPECT_EQ(cur, value);
  }
}

TEST(memtable_test, MemtableIteratorTest) {
  Memtable memtable;
  for (int i = 0; i < 26; i++) {
    for (int j = 0; j < 26; j++) {
      for (int k = 0; k < 10; k++) {
        std::string key = "user_key";
        key += static_cast<char>('a' + i);
        key += static_cast<char>('a' + j);
        key += std::to_string(k);
        std::string value = "user_value";
        value += static_cast<char>('a' + i);
        value += static_cast<char>('a' + j);
        value += std::to_string(k);
        memtable.Insert(CreateMemtableKey(key, 0, value, KeyType::kTypeValue));
      }
    }
  }
  Iterator* iter = memtable.NewIterator();
  iter->SeekToFirst();
  for (int i = 0; i < 26; i++) {
    for (int j = 0; j < 26; j++) {
      for (int k = 0; k < 10; k++) {
        std::string key = "user_key";
        key += static_cast<char>('a' + i);
        key += static_cast<char>('a' + j);
        key += std::to_string(k);
        std::string value = "user_value";
        value += static_cast<char>('a' + i);
        value += static_cast<char>('a' + j);
        value += std::to_string(k);
        EXPECT_EQ(iter->Key(),
                  CreateInternalKey(key.data(), 0, KeyType::kTypeValue));
        EXPECT_EQ(iter->Value(), value.data());
        iter->Next();
      }
    }
  }
}
};  // namespace koishidb