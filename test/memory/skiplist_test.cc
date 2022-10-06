#include "memory/skiplist.h"
#include "type/slice.h"
#include "gtest/gtest.h"



namespace koishidb {
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
        srand(0);
        int num = 10000;
        SkipList<int, int> list;
        std::vector<int> right(num, -1);
        for (int i = 0; i < num; ++i) {
            int x = rand() % num, y = rand() % num;
            list.Put(x, y);
            right[x] = y;
        }
        for (int i = 0; i < num; ++i) {
            int ret;
            bool flag = list.Get(i, ret);
            EXPECT_EQ(flag, right[i] != -1);
            if (right[i] != -1) {
            EXPECT_EQ(ret, right[i]);
            }
        }
        for (int i = 0; i < num; ++i) {
            int x = rand() % num;
            right[x] = -1;
            list.Delete(x);
        }
        for (int i = 0; i < num; ++i) {
            int ret;
            bool flag = list.Get(i, ret);
            EXPECT_EQ(flag, right[i] != -1);
        }
    };

    TEST(SkipList_test, SkipListIteratorTest1) {
        srand(0);
        int num = 10000;
        SkipList<int, int> list;
        std::vector<int> right(num, -1);
        for (int i = 0; i < num; ++i) {
            int x = rand() % num, y = rand() % num;
            list.Put(x, y);
            right[x] = y;
        }
        for (int i = 0; i < num; ++i) {
            int x = rand() % num;
            right[x] = -1;
            list.Delete(x);
        }
        
        SkipList<int, int>::Iterator it(list);
        std::vector<int> test(num, -1);
        while (it.Valid()) {
            int i = it.Key();
            if (it.Type() != KeyType::kTypeDeletion) {
                test[i] = it.Value();                
            }
            ++it;
        }
        for (int i = 0; i < num; ++i) {
            EXPECT_EQ(test[i], right[i]);
        }
    };

    TEST(SkipList_test, SkipListIteratorTest2) {
        srand(0);
        int num = 10000;
        SkipList<int, int> list;
        std::vector<int> right(num, -1);
        for (int i = 0; i < num; ++i) {
            int x = rand() % num, y = rand() % num;
            list.Put(x, y);
            right[x] = y;
        }
        for (int i = 0; i < num; ++i) {
            int x = rand() % num;
            right[x] = -1;
            list.Delete(x);
        }
        
        SkipList<int, int>::Iterator it(list);
        std::vector<int> test(num, -1);
        while (it.Valid()) {
            int i = it.Key();
            if (it.Type() != KeyType::kTypeDeletion) {
                test[i] = it.Value();                
            }
            it++;
        }
        for (int i = 0; i < num; ++i) {
            EXPECT_EQ(test[i], right[i]);
        }
    };

    TEST(SkipList_test, SkipListIteratorTest3) {
        srand(0);
        int num = 10000;
        SkipList<int, int> list;
        std::vector<int> right(num, -1);
        for (int i = 0; i < num; ++i) {
            int x = rand() % num, y = rand() % num;
            list.Put(x, y);
            right[x] = y;
        }
        for (int i = 0; i < num; ++i) {
            int x = rand() % num;
            right[x] = -1;
            list.Delete(x);
        }
        
        SkipList<int, int>::Iterator it(list);
        std::vector<int> test(num, -1);
        while (it.Valid()) {
            int i = it.Key();
            if (it.Type() != KeyType::kTypeDeletion) {
                test[i] = it.Value();                
            }
            it = it.Next();
        }
        for (int i = 0; i < num; ++i) {
            EXPECT_EQ(test[i], right[i]);
        }
    };
};