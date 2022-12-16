#include "disk/sstable.h"

#include <fcntl.h>

#include "common/option.h"
#include "disk/random_access_file.h"
#include "disk/sstable_iterator.h"
#include "disk/table_builder.h"
#include "disk/writable_file.h"
#include "gtest/gtest.h"
#include "util/comparator.h"
namespace koishidb {

class UserComparator : public Comparator {
  int Compare(const Slice& a, const Slice& b) const { return a.Compare(b); }
  std::string Name() const { return "user_key_comparator"; }
};

TEST(Table_test, BasicTest) {
  std::string temp_file = "./temp";
  int fd = ::open(temp_file.c_str(), O_WRONLY | O_CREAT, 0777);
  if (fd == -1) {
    return;
  }
  WritableFile writable_file(fd, temp_file);
  const Option* opt = new Option(new UserComparator());
  TableBuilder table_builder(opt, &writable_file);
  for (int i = 1; i < 10; i++) {
    std::string key = "temp_test_key" + std::to_string(i);
    std::string value = "temp_test_val" + std::to_string(i);
    table_builder.Add(key.data(), value.data());
  }
  table_builder.Finish();
  EXPECT_EQ(table_builder.NumEntries(), 9);
  uint64_t file_size = table_builder.FileSize();
  RandomAccessFile random_access_file(temp_file);
  auto sstable_opt = SSTable::Open(opt, &random_access_file, file_size);
  EXPECT_TRUE(sstable_opt.has_value());
  SSTable* sstable = sstable_opt.value();
  Iterator* sstable_iter = sstable->NewIterator();

  sstable_iter->SeekToFirst();
  for (int i = 1; i < 10; i++) {
    sstable_iter->Next();
    std::string key = "temp_test_key" + std::to_string(i);
    std::string value = "temp_test_val" + std::to_string(i);
    EXPECT_EQ(
        std::string(sstable_iter->Key().data(), sstable_iter->Key().size()),
        key);
    EXPECT_EQ(
        std::string(sstable_iter->Value().data(), sstable_iter->Value().size()),
        value);
  }
  EXPECT_TRUE(sstable_iter->Valid());
  sstable_iter->Next();
  EXPECT_FALSE(sstable_iter->Valid());

  for (int i = 1; i < 10; i++) {
    // std::optional can compare directly;
    // if std::optional == std::nullopt, then std::nulloptr < other object ==
    // true forever
    std::string key = "temp_test_key" + std::to_string(i);
    std::string value = "temp_test_val" + std::to_string(i);
    EXPECT_EQ(value, sstable->InternalGet(key));
  }

  for (int i = 11; i < 20; i++) {
    std::string key = "temp_test_key" + std::to_string(i);
    EXPECT_EQ(std::nullopt, sstable->InternalGet(key));
  }

  delete opt;
  // Tidy up
  int result = remove(temp_file.c_str());
  if (result == -1) {
    return;
  }
}
};  // namespace koishidb
