#include "util/bloom_filter.h"
#include "gtest/gtest.h"


#include <vector>
namespace koishidb {
  TEST(BloomFilter_test, BasicTest) {
    BloomFilter filter(10);

    std::vector<Slice> keys;
    keys.push_back("hello");
    keys.push_back("foo");
    keys.push_back("bar");
    keys.push_back("count");

    std::string dst;
    filter.CreateFilter(&keys[0], keys.size(), &dst);

    EXPECT_TRUE(filter.KeyMayMatch("hello", dst.data()));
    EXPECT_TRUE(filter.KeyMayMatch("foo", dst.data()));
    EXPECT_TRUE(filter.KeyMayMatch("bar", dst.data()));
    EXPECT_FALSE(filter.KeyMayMatch("xxxxasdsadasdsasda", dst.data()));
    EXPECT_FALSE(filter.KeyMayMatch("yyyyy", dst.data()));
  }
};
