#include "network/helper.h"

#include <fcntl.h>

#include "logger.h"
namespace koishidb {
void Helper::set_non_block(const int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    LOG_WARN("[%d] set non-block io failed", fd);
    return;
  }
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void epoll_helper::create_event(int epoll_fd, int fd, int state) {
  struct epoll_event event {};
  event.events = state;
  event.data.fd = fd;

  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
}

void epoll_helper::delete_event(int epoll_fd, int fd, int state) {
  struct epoll_event event {};
  event.events = state;
  event.data.fd = fd;

  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event);
}

void epoll_helper::modify_event(int epoll_fd, int fd, int state) {
  struct epoll_event event {};
  event.events = state;
  event.data.fd = fd;

  epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
}
};  // namespace koishidb