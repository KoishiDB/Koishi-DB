#ifndef KOISHIDB_SRC_INCLUDE_CONCURRENT_HASHMAP_H
#define KOISHIDB_SRC_INCLUDE_CONCURRENT_HASHMAP_H
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
namespace koishidb {
using std::shared_mutex, std::shared_lock, std::unique_lock;
using std::unordered_map;

template <class K, class V>
class concurrent_hashmap {
 public:
  int count(K key);
  V get(K key);
  void set(K key, V value);
  void erase(K key);

 private:
  unordered_map<K, V> data;
  mutable shared_mutex mutex;
};

template <class K, class V>
int concurrent_hashmap<K, V>::count(K key) {
  shared_lock<shared_mutex> lock(this->mutex);
  return this->data.count(key);
}

template <class K, class V>
V concurrent_hashmap<K, V>::get(K key) {
  shared_lock<shared_mutex> lock(this->mutex);
  return this->data[key];
}

template <class K, class V>
void concurrent_hashmap<K, V>::set(K key, V value) {
  unique_lock<shared_mutex> lock(this->mutex);
  this->data[key] = value;
}

template <class K, class V>
void concurrent_hashmap<K, V>::erase(K key) {
  unique_lock<shared_mutex> lock(this->mutex);
  if (this->data.count(key)) {
    this->data.erase(key);
  }
}
};  // namespace koishidb

#endif