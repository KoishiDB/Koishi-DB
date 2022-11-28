#include "disk/block.h"
#include "disk/block_iterator.h"
#include "disk/block_builder.h"
#include "common/option.h"

#include <iostream>

#include "gtest/gtest.h"
namespace koishidb {
    TEST(BlockIterator_test, BlockIteratorBasicTest) {
        BlockBuilder* block_builder = new BlockBuilder(new Option());
        block_builder->Add("aaa", "123");
        block_builder->Add("bbb", "456");
        block_builder->Add("ccc", "789");
        block_builder->Finish();

        Block b;
        std::string rep = block_builder->Data();
        b.data = rep.data();
        b.size = rep.size();

        BlockIterator* iter = new BlockIterator(&b);
        iter->SeekToFirst();
        // should invoke next first
        iter->Next();
        EXPECT_EQ(iter->Key(), "aaa");
        EXPECT_EQ(iter->Value(), "123");
        iter->Next();
        EXPECT_EQ(iter->Key(), "bbb");
        EXPECT_EQ(iter->Value(), "456");
        iter->Next();
        EXPECT_EQ(iter->Key(), "ccc");
        EXPECT_EQ(iter->Value(), "789");
        iter->Next();
        EXPECT_FALSE(iter->Valid());
    }
};