#include "memory/skiplist.h"
#include "type/slice.h"
#include "gtest/gtest.h"



namespace koishidb {
    TEST(SkipList_test, SkipListSampleTest) {
        SkipList<int, int> list;
        for (int i = 1; i <= 100; i++) {
            list.Put(i, i + 1);
        }
        for (int i = 1; i <= 100; i++) {
            int val;
            list.Get(i, val);
            EXPECT_EQ(val, i + 1);
        }
        for (int i = 1; i <= 100; i += 2) {
            list.Delete(i);
        }
        for (int i = 1; i <= 100; i += 2) {
            int val;
            bool flag = list.Get(i, val);
            EXPECT_EQ(flag, false);
        }
    }

    TEST(SkipList_test, SkipListStringTest) {
        SkipList<std::string, std::string> list;
        list.Put("koishi", "cute");
        list.Put("cccc", "23333");
        list.Put("web", "cookie");
        list.Put("cs", "2011");

        std::string ret;
        list.Get("cs", ret);
        EXPECT_EQ(ret, "2011");
        list.Delete("cccc");
        bool flag = list.Get("cccc", ret);
        EXPECT_FALSE(flag);
    }

    TEST(SkipList_test, SkipListSliceTest) {
        SkipList<Slice, Slice> list;
        list.Put(Slice("koishi"), Slice("cute"));
        list.Put(Slice("slice"), Slice("test"));
        Slice ret;
        list.Get(Slice("slice"), ret);
        EXPECT_EQ(ret.data(), "test");
        list.Delete(Slice("slice"));
        bool flag = list.Get(Slice("slice"), ret);
        EXPECT_FALSE(flag);
    }

    TEST(SkipList_test, SkipListRandomTest) {
        srand(time(NULL));
        SkipList<int, int> list;
        std::vector<int> right(10000, -1);
        for (int i = 0; i < 10000; ++i) {
            int x = rand() % 10000, y = rand() % 10000;
            list.Put(x, y);
            right[x] = y;
        }
        for (int i = 0; i < 10000; ++i) {
            int ret;
            bool flag = list.Get(i, ret);
            EXPECT_EQ(flag, right[i] != -1);
            if (right[i] != -1) {
            EXPECT_EQ(ret, right[i]);
            }
        }
        for (int i = 0; i < 10000; ++i) {
            int x = rand() % 10000;
            right[x] = -1;
            list.Delete(x);
        }
        for (int i = 0; i < 10000; ++i) {
            int ret;
            bool flag = list.Get(i, ret);
            EXPECT_EQ(flag, right[i] != -1);
        }
    };
};