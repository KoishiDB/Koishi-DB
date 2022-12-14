#ifndef KOISHIDB_SRC_INCLUDE_PARSER_H
#define KOISHIDB_SRC_INCLUDE_PARSER_H
#include <netdb.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fstream>
#include <memory>
#include "network/connection.h"

namespace koishidb {
    class Parser {
    public:
        static void print_client_info(sockaddr* client_addr, socklen_t client_addr_len);

        static bool read(int epoll_fd, int conn_fd, std::shared_ptr<connection> conn);

    private:
        static void parse_all(std::shared_ptr<connection> conn, int conn_fd);

    };

};
#endif