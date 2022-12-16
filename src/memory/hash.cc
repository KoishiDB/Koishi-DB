#include "memory/hash.h"

namespace koishidb {
int HashTable::Get(int key) { return m[key]; }
void HashTable::Put(int key, int value) { m[key] = value; }
}  // namespace koishidb
