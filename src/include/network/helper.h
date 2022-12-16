#ifndef KOISHIDB_SRC_INCLUDE_NETWORK_HELPER_H
#define KOISHIDB_SRC_INCLUDE_NETWORK_HELPER_H

#include <sys/epoll.h>
namespace koishidb {
class Helper {
 public:
  static void set_non_block(const int fd);
};

class epoll_helper {
 public:
  static void create_event(int epoll_fd, int fd, int state);
  static void delete_event(int epoll_fd, int fd, int state);
  static void modify_event(int epoll_fd, int fd, int state);
};

};  // namespace koishidb

#endif