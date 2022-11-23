#include "disk/filterblock_reader.h"
#include "disk/filterblock_builder.h"

#include "gtest/gtest.h"
#include <iostream>
namespace koishidb {
  TEST(Filterblock_test, SingleChunkTest) {
    FilterBlockBuilder builder;
    builder.StartBlock(100);
    builder.AddKey("foo");
    builder.AddKey("bar");
    builder.AddKey("box");
    builder.StartBlock(200);
    builder.AddKey("box");
    builder.StartBlock(300);
    builder.AddKey("hello");
    Slice block = builder.Finish();
    FilterBlockReader reader( block);
    ASSERT_TRUE(reader.KeyMayMatch(100, "foo"));
    ASSERT_TRUE(reader.KeyMayMatch(100, "bar"));
    ASSERT_TRUE(reader.KeyMayMatch(100, "box"));
    ASSERT_TRUE(reader.KeyMayMatch(100, "hello"));
    ASSERT_TRUE(reader.KeyMayMatch(100, "foo"));
    ASSERT_TRUE(!reader.KeyMayMatch(100, "missing"));
    ASSERT_TRUE(!reader.KeyMayMatch(100, "other"));
  }

  TEST(Filterblock_test, MutipleChunkTest) {
    FilterBlockBuilder builder;

    // First filter
    builder.StartBlock(0);
    builder.AddKey("foo");
    builder.StartBlock(2000);
    builder.AddKey("bar");

    // Second filter
    builder.StartBlock(3100);
    builder.AddKey("box");

    // Third filter is empty

    // Last filter
    builder.StartBlock(9000);
    builder.AddKey("box");
    builder.AddKey("hello");

    Slice block = builder.Finish();
    FilterBlockReader reader(block);

    // Check first filter
    ASSERT_TRUE(reader.KeyMayMatch(0, "foo"));
    ASSERT_TRUE(reader.KeyMayMatch(2000, "bar"));
    ASSERT_TRUE(!reader.KeyMayMatch(0, "box"));
    ASSERT_TRUE(!reader.KeyMayMatch(0, "hello"));

    // Check second filter
    ASSERT_TRUE(reader.KeyMayMatch(3100, "box"));
    ASSERT_TRUE(!reader.KeyMayMatch(3100, "foo"));
    ASSERT_TRUE(!reader.KeyMayMatch(3100, "bar"));
    ASSERT_TRUE(!reader.KeyMayMatch(3100, "hello"));

    // Check third filter (empty)
    ASSERT_TRUE(!reader.KeyMayMatch(4100, "foo"));
    ASSERT_TRUE(!reader.KeyMayMatch(4100, "bar"));
    ASSERT_TRUE(!reader.KeyMayMatch(4100, "box"));
    ASSERT_TRUE(!reader.KeyMayMatch(4100, "hello"));

    // Check last filter
    ASSERT_TRUE(reader.KeyMayMatch(9000, "box"));
    ASSERT_TRUE(reader.KeyMayMatch(9000, "hello"));
    ASSERT_TRUE(!reader.KeyMayMatch(9000, "foo"));
    ASSERT_TRUE(!reader.KeyMayMatch(9000, "bar"));
  }
};

