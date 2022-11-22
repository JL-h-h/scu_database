/**
 * LRU implementation
 */
#include "buffer/lru_replacer.h"
#include "page/page.h"

namespace scudb {

template <typename T> LRUReplacer<T>::LRUReplacer() {
  head_node = make_shared<Node>();
  tail_node = make_shared<Node>();

  head_node->next_node = tail_node;
  tail_node->prev_node = head_node;
}

template <typename T> LRUReplacer<T>::~LRUReplacer() {}

/*
 * Insert value into LRU
 */
template <typename T> void LRUReplacer<T>::Insert(const T &value) {
  lock_guard<mutex> lock(latch);

  shared_ptr<Node> current_node;

  if (node_map.find(value) != node_map.end()) {
    current_node = node_map[value];
    shared_ptr<Node> prev = current_node->prev_node;
    shared_ptr<Node> next = current_node->next_node;
    prev->next_node = next;
    next->prev_node = prev;
  } 
  else {
    current_node = make_shared<Node>(value);
  }

  shared_ptr<Node> node = tail_node->prev_node;

  current_node->next_node = tail_node;
  current_node->prev_node = node;
  node->next_node = current_node;
  tail_node->prev_node = current_node;
  node_map[value] = current_node;
}

/* If LRU is non-empty, pop the head member from LRU to argument "value", and
 * return true. If LRU is empty, return false
 */
template <typename T> bool LRUReplacer<T>::Victim(T &value) {
  lock_guard<mutex> lock(latch);

  if (node_map.empty()) {
    return false;
  }
  else {
    shared_ptr<Node> node = head_node->next_node;
    head_node->next_node = node->next_node;
    node->next_node->prev_node = head_node;
    value = node->value;
    node_map.erase(node->value);
    return true;
  }
}

/*
 * Remove value from LRU. If removal is successful, return true, otherwise
 * return false
 */
template <typename T> bool LRUReplacer<T>::Erase(const T &value) {
  lock_guard<mutex> lock(latch);

  auto f = node_map.find(value);
  auto e = node_map.end();
  if (f != e) {
    shared_ptr<Node> current_node = node_map[value];
    current_node->prev_node->next_node = current_node->next_node;
    current_node->next_node->prev_node = current_node->prev_node;
  }
  return node_map.erase(value);
}

template <typename T> size_t LRUReplacer<T>::Size() { 
  lock_guard<mutex> lock(latch);
  auto s = node_map.size();
  return s;
}

template class LRUReplacer<Page *>;
// test only
template class LRUReplacer<int>;

} // namespace scudb
