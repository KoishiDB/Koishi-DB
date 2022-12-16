#include <fcntl.h>
#include <unistd.h>

#include "disk/format.h"
#include "gtest/gtest.h"
#include "logger.h"

namespace koishidb {
TEST(Manifest_test, BasicTest) {
  std::string temp_file = "manifest";
  int fd = ::open(temp_file.c_str(), O_WRONLY | O_CREAT, 0777);
  if (fd == -1) {
    LOG_ERROR("error happens");
    return;
  }
  WritableFile writable_file(fd, temp_file);
  FileMeta file_meta1;
  file_meta1.number = 2;
  file_meta1.file_size = 1412412;
  file_meta1.smallest_key = TransToInternalKey("user_key1111");
  file_meta1.largest_key = TransToInternalKey("user_key2222");

  FileMeta file_meta2;
  file_meta2.number = 321;
  file_meta2.file_size = 14122;
  file_meta2.smallest_key = TransToInternalKey("user_key3333");
  file_meta2.largest_key = TransToInternalKey("user_key4444");
  EncodeFileMeta(&file_meta1, writable_file);
  EncodeFileMeta(&file_meta2, writable_file);
  ::close(fd);
  fd = ::open(temp_file.c_str(), O_RDONLY);
  if (fd == -1) {
    LOG_ERROR("error happens");
    return;
  }
  char buf[1024];
  int n;
  std::string data;
  while ((n = ::read(fd, buf, sizeof(buf))) > 0) {
    data.append(buf, n);
  }
  Slice s = data;
  FileMeta* meta1, *meta2;
  DecodeFileMeta(&meta1, &s);
  DecodeFileMeta(&meta2, &s);
  EXPECT_EQ(s.size(), 0);
  EXPECT_EQ(meta1->number, 2);
  EXPECT_EQ(meta1->file_size, 1412412);
  EXPECT_EQ(meta1->smallest_key.ToString(), "user_key1111");
  EXPECT_EQ(meta1->largest_key.ToString(), "user_key2222");
  EXPECT_EQ(meta2->number, 321);
  EXPECT_EQ(meta2->file_size, 14122);
  EXPECT_EQ(meta2->smallest_key.ToString(), "user_key3333");
  EXPECT_EQ(meta2->largest_key.ToString(), "user_key4444");
  // tidy up
  Remove(temp_file);
}
};  // namespace koishidb