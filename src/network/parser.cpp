#include "network/parser.h"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <memory>

#include "logger.h"
#include "network/helper.h"

constexpr int MAX_BUF = 1024;

namespace koishidb {

bool Parser::read(int conn_fd, std::shared_ptr<connection> conn) {
  char buf[1024];
  int n = 0;
  conn->req = unique_ptr<request>(new request());
  string& read_storage = conn->read;
  n = ::read(conn_fd, buf, sizeof(buf));
  if (n < 0) {
    LOG_INFO("[%d] %d -> %s signal received \n", conn_fd, errno,
             ::strerror(errno));
    return false;
  }
  LOG_INFO("[%d] received %d \n", conn_fd, n);
  read_storage.append(buf, n);

  parse_all(conn, conn_fd);
  return true;
}

void Parser::parse_all(std::shared_ptr<connection> conn, int conn_fd) {
  std::string& read_data = conn->read;
  int& op_code = conn->req->op_code_;
  op_code = read_data[0] - '0';
  int ptr = 3;
  bool flag = false;
  for (int i = ptr; i < read_data.size(); i++) {
    if (read_data[i] != '\r') {
      if (flag == false) {
        conn->req->key_.push_back(read_data[i]);
      } else {
        conn->req->value_.push_back(read_data[i]);
      }
    } else {
      // skip the \r\n and set the flag = true;
      flag = true;
      i++;
    }
  }
}

void Parser::print_client_info(sockaddr* client_addr,
                               socklen_t client_addr_len) {
  char host_name[MAX_BUF], host_port[MAX_BUF];

  memset(host_name, 0, sizeof(host_name));
  memset(host_port, 0, sizeof(host_port));

  getnameinfo(client_addr, client_addr_len, host_name, sizeof(host_name),
              host_port, sizeof(host_port), 0);

  LOG_INFO("Received Request from: %s, port: %s \n", host_name, host_port);
}
};  // namespace koishidb