#include "util/encode.h"
#include "gtest/gtest.h"

#include <string>

namespace koishidb {
    TEST(encode_test, RandomTest) {
        srand(0);
        for (int i = 0; i < 100; ++i) {
            std::string a;
            uint32_t x = rand();
            char dst[10];
            size_t len = EncodeVarint32(x, dst);
            PutVarint32(x, &a);
            uint32_t y;
            Slice b(a + std::string("xxx"));
            GetVarint32(&b, &y);
            EXPECT_EQ(x, y);
            EXPECT_EQ(b.ToString(), std::string("xxx"));
        }
    }
};