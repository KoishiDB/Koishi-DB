#include "util/util.h"
#include "gtest/gtest.h"

namespace koishidb {
  TEST(Util_test, UtilBasicTest) {
      Slice user_key("hhhhhsdsadas");
      Slice value("232323");
      SequenceNumber number = 123;
      KeyType type = KeyType::kTypeValue;
      Slice memtable_key =  CreateMemtableKey(user_key, number, value, type);
      LookupKey look_up_key(memtable_key);
      // Random test1
      EXPECT_EQ(user_key, look_up_key.UserKey());
      EXPECT_EQ(value, look_up_key.Value());
      EXPECT_EQ(number, look_up_key.GetSequence());
      EXPECT_EQ(type, look_up_key.GetKeyType());

      // Random test2
      Slice user_key2("23333xincheng");
      SequenceNumber number2 = 23333;
      KeyType type2 = KeyType::kTypeDeletion;
      Slice memtable_key2 = CreateMemtableKey(user_key2, number2, Slice(), type2);
      LookupKey look_up_key2(memtable_key2);
      EXPECT_EQ(user_key2, look_up_key2.UserKey());
      EXPECT_EQ(number2, look_up_key2.GetSequence());
      EXPECT_EQ(type2, look_up_key2.GetKeyType());
  }
};