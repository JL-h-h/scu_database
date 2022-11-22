/**
 * lru_replacer.h
 *
 * Functionality: The buffer pool manager must maintain a LRU list to collect
 * all the pages that are unpinned and ready to be swapped. The simplest way to
 * implement LRU is a FIFO queue, but remember to dequeue or enqueue pages when
 * a page changes from unpinned to pinned, or vice-versa.
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <mutex>
#include "buffer/replacer.h"
#include "hash/extendible_hash.h"

using namespace std;
namespace scudb {

template <typename T> class LRUReplacer : public Replacer<T> {

  struct Node {
    Node() {};
    Node(T value){
      this->value = value;
    };
    T value;
    shared_ptr<Node> prev_node;
    shared_ptr<Node> next_node;
  };

public:
  // do not change public interface
  LRUReplacer();

  ~LRUReplacer();

  void Insert(const T &value);

  bool Victim(T &value);

  bool Erase(const T &value);

  size_t Size();

private:
  // add your member variables here
  shared_ptr<Node> head_node;
  shared_ptr<Node> tail_node;
  map<T,shared_ptr<Node>> node_map;
  mutable mutex latch;
};

} // namespace scudb
