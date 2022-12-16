#ifndef KOISHIDB_SRC_INCLUDE_REQUEST_H
#define KOISHIDB_SRC_INCLUDE_REQUEST_H

#include <string>
#include <unordered_map>
namespace koishidb {
using std::string, std::unordered_map;
class request {
 public:
  int op_code_;
  std::string key_, value_;
};
};  // namespace koishidb

#endif