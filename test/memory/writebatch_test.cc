
#include "gtest/gtest.h"
#define DEBUG
#include "memory/write_batch.h"
namespace koishidb {
    TEST(WriteBatch_test, WriteBatchIteratorTest) {
        WriteBatch w;
        for (int i = 0; i < 1000; i++) {
            std::string key = "user_key" + std::to_string(i);
            std::string value = "user_value" + std::to_string(i);
            w.Put(key, value);
        }
        EXPECT_EQ(WriteBatchInternal::Count(&w), 1000);
        auto result = w.Iterate();
        for (int i = 0; i < 1000; i++) {
            std::string key = "user_key" + std::to_string(i);
            std::string value = "user_value" + std::to_string(i);
            EXPECT_EQ(result[i].first, key);
            EXPECT_EQ(result[i].second, value);
        }
    }
}
