#include "disk/block_builder.h"
#include "util/comparator.h"
#include "common/option.h"
#include "gtest/gtest.h"

namespace koishidb {
    TEST(BlockBuilder_test, BlockBuilderBasicTest) {
       BlockBuilder* block_builder = new BlockBuilder(new Option());
       int tot_size = 0;
       Slice key1("aaaa"), value1("2323232322323");
       Slice key2("bbbb"), value2("xxxxsssssljlee");
       tot_size += EncodeVarint32Length(key1.size());
       tot_size += EncodeVarint32Length(value1.size());
       tot_size += EncodeVarint32Length(key2.size());
       tot_size += EncodeVarint32Length(value2.size());
       tot_size += key1.size();
       tot_size += value1.size();
       tot_size += key2.size();
       tot_size += value2.size();

       block_builder->Add(key1, value1);
       block_builder->Add(key2, value2);

       EXPECT_EQ(block_builder->EstimatedSize(), tot_size);
    }
};