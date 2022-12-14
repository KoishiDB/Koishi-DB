#include "db_impl.h"
#include "db.h"
#include "network/helper.h"
#include "network/thread_pool.h"
#include "network/parser.h"
#include "network/connection.h"
#include "network/request.h"
#include "util/concurrent_hashmap.h"
#include "logger.h"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>


constexpr int thread_pool_size = 10;
constexpr int MAX_EVENTS = 128;
constexpr int FD_SIZE = 1024;
using koishidb::Helper;
using koishidb::ThreadPool;
using koishidb::epoll_helper;
using koishidb::Parser;
using koishidb::connection;
using koishidb::concurrent_hashmap;
using koishidb::request;
// 1\r\n
// user_key \r\n    -> Put
// user_value \r\n


// 2\r\n
// user_key \r\n    ->Get

// 3\r\n
// user_key \r\n    ->Delete


// Receiver 1\r\n -> true
// user_value\r\n
// Receiver 0\r\n -> false

concurrent_hashmap<int, std::shared_ptr<connection>> connection_storage;



int main() {
    koishidb::DB* db = new koishidb::DBimpl();

    ThreadPool thread_pool(thread_pool_size);
    // Config the listen port
    sockaddr_in server_addr{};
    server_addr.sin_port = htons(8094);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listen_fd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr));
    listen(listen_fd, thread_pool_size);
    LOG_INFO("server run in port 8096");

    // config epoll
    int epoll_fd = epoll_create(FD_SIZE);
    struct epoll_event events[MAX_EVENTS];
    epoll_helper::create_event(epoll_fd, listen_fd, EPOLLIN);
    while (true) {
        const int active = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        bool error_flag = false;
        for (int i = 0; i < active; i++) {
            const int fd = events[i].data.fd, ev = events[i].events;
            if (fd == listen_fd && (ev & (EPOLLIN | EPOLLERR))) {
                sockaddr_in client_addr{};
                socklen_t client_addr_len = sizeof(client_addr);
                int conn_fd = accept(listen_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_addr_len);
                if (conn_fd != -1) {
                    LOG_INFO("[%d] Connection Established\n", conn_fd);
                    Parser::print_client_info(reinterpret_cast<sockaddr *>(&client_addr), client_addr_len);
                    Helper::set_non_block(conn_fd);
                    epoll_helper::create_event(epoll_fd, conn_fd, EPOLLIN);

                    // add to connection_storage
                    if (connection_storage.count(conn_fd)) {
                        connection_storage.erase(conn_fd);
                    }
                    connection_storage.set(conn_fd, std::shared_ptr<connection>(new connection()));
                }
            } else if (ev & EPOLLIN) {
                int conn_fd = events[i].data.fd;
                LOG_INFO("[%d] trigger epoll_in event \n", conn_fd);
                auto connection = connection_storage.get(conn_fd);
                std::future<bool> result = thread_pool.enqueue([&]()->bool {
                    if (Parser::read(conn_fd, connection)) {
                        std::unique_ptr<request>& req =connection->req;
                        bool flag = true;
                        switch (req->op_code_) {
                            // Put
                            case 1: {
                                db->Put(req->key_, req->value_);
                                std::string server_answer = "1\r\n";
                                ::write(conn_fd, server_answer.data(), server_answer.size());
                                break;
                            }
                            case 2: {
                                auto result = db->Get(req->key_, &req->value_);
                                // Write back
                                LOG_INFO("%s", req->value_.data());
                                if (result) {
                                    std::string server_answer = "1\r\n";
                                    server_answer += req->value_;
                                    server_answer += "\r\n";
                                    ::write(conn_fd, server_answer.data(), server_answer.size());
                                } else {
                                    std::string server_answer = "0\r\n";
                                    ::write(conn_fd, server_answer.data(), server_answer.size());
                                }
                                break;
                            }
                            case 3: {
                                db->Delete(req->key_);
                                std::string server_answer = "1\r\n";
                                ::write(conn_fd, server_answer.data(), server_answer.size());
                                break;
                            }
                            // close connection
                        }
                        close(conn_fd);
                        connection_storage.erase(conn_fd);
                        return flag;
                    }
                });

            }
            // For not block, we should not invoke the get method of this future
        }
    }
    close(listen_fd);

    delete db;
    return 0;
}