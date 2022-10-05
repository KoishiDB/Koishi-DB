#include "util/encode.h"
#include "type/slice.h"

#include "gtest/gtest.h"


namespace koishidb {
  TEST(Encode_test, EncodeSampleTest) {
      std::string dst = "";

      std::string key = "key";
      uint32_t key_len = key.size();
      PutVarint32(key_len, &dst);
      dst += key;

      std::string value = "value";
      uint32_t value_len = value.size();
      PutVarint32(value_len, &dst);
      dst += value;

      Slice bytes(dst);

      uint32_t res;
      GetVarint32(&bytes, &res);
      EXPECT_EQ(res, key_len);
      std::string result;
      GetFixedBytes(&bytes, &result, res);
      EXPECT_EQ(result, key);

      GetVarint32(&bytes, &res);
      EXPECT_EQ(res, value_len);
      result.clear();
      GetFixedBytes(&bytes, &result, res);
      EXPECT_EQ(result, value);
  }


};
