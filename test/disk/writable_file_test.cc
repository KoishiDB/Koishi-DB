#include <fcntl.h>
#include <unistd.h>


#include "disk/writable_file.h"
#include "gtest/gtest.h"
namespace koishidb {
  TEST(WritableFile_test, BasicTest) {
    std::string temp_file = "./test_writable_file";
    int fd = ::open(temp_file.c_str(), O_WRONLY | O_CREAT, 0777);
    if (fd == -1) {
      return;
    }
    WritableFile writable_file(fd, temp_file);

    const char* contents = "this is a test for writable file and it should work out";
    writable_file.Append(contents);
    writable_file.Flush();
    ::close(fd); // for read test

    fd = ::open(temp_file.c_str(), O_RDONLY);
    if (fd == -1) {
      // Tidy up here
      remove(temp_file.c_str());
      return;
    }
    int read_size;
    char buf[1000];
    read_size = ::read(fd, buf, sizeof(buf));
    if (read_size < 0) {
      remove(temp_file.c_str());
      return;
    }
    EXPECT_EQ(strncmp(contents, buf, read_size), 0);

    int result = remove(temp_file.c_str());
    if (result == -1) {
      return;
    }

  }
};