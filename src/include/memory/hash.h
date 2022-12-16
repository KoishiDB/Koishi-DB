#ifndef KOISHIDB_HASH_H
#define KOISHIDB_HASH_H

#include <unordered_map>
namespace koishidb {
class HashTable {
 public:
  void Put(int key, int value);
  int Get(int key);

 private:
  std::unordered_map<int, int> m;
};

}  // namespace koishidb

#endif  // KOISHIDB_HASH_H
