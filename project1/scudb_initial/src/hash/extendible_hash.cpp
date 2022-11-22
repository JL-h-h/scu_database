#include <list>

#include "hash/extendible_hash.h"
#include "page/page.h"

namespace scudb {

/*
 * constructor
 * array_size: fixed array size for each bucket
 */
template <typename K, typename V>
ExtendibleHash<K, V>::ExtendibleHash(size_t size) {
  global_depth = 0;
  bucket_size = size;
  bucket_number = 1;
  buckets.push_back(make_shared<Bucket>(0));
}
template<typename K, typename V>
ExtendibleHash<K, V>::ExtendibleHash() {
  ExtendibleHash(64);
}

/*
 * helper function to calculate the hashing address of input key
 */
template <typename K, typename V>
size_t ExtendibleHash<K, V>::HashKey(const K &key) const {
  size_t tmp = hash<K>{}(key);
  return tmp;
}

/*
 * helper function to return global depth of hash table
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetGlobalDepth() const {
  lock_guard<mutex> lock(latch_table);
  return global_depth;
}

/*
 * helper function to return local depth of one specific bucket
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetLocalDepth(int bucket_id) const {
  if (buckets[bucket_id]) {
    lock_guard<mutex> lck(buckets[bucket_id]->latch_bucket);
    auto s = buckets[bucket_id]->map_k.size();
    if (s == 0) {
      return -1;
    }
    int reld = buckets[bucket_id]->local_depth;
    return reld;
  }
  return -1;
}

/*
 * helper function to return current number of bucket in hash table
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetNumBuckets() const {
  lock_guard<mutex> lock(latch_table);
  return bucket_number;
}

/*
 * lookup function to find value associate with input key
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Find(const K &key, V &value) {
  lock_guard<mutex> lck0(latch_table);
  int bucket_idx = HashKey(key) & ((1 << global_depth) - 1);
  lock_guard<mutex> lck(buckets[bucket_idx]->latch_bucket);

  auto f = buckets[bucket_idx]->map_k.find(key);
  auto e = buckets[bucket_idx]->map_k.end();
  if (f != e) {
    value = buckets[bucket_idx]->map_k[key];
    return true;
  }
  else {
    return false;
  }
  
}

template <typename K, typename V>
int ExtendibleHash<K, V>::GetIndexOfBuckets(const K &key) const{
  lock_guard<mutex> lck(latch_table);
  auto hash_key = HashKey(key);
  int index = hash_key & ((1 << global_depth) - 1);
  return index;
}

/*
 * delete <key,value> entry in hash table
 * Shrink & Combination is not required for this project
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Remove(const K &key) {
  lock_guard<mutex> lck0(latch_table);
  int bucket_idx = HashKey(key) & ((1 << global_depth) - 1);

  lock_guard<mutex> lck(buckets[bucket_idx]->latch_bucket);
  shared_ptr<Bucket> current_bucket = buckets[bucket_idx];
  auto f = current_bucket->map_k.find(key);
  auto e = current_bucket->map_k.end();
  if (f != e) {
    current_bucket->map_k.erase(key);
    return true;
  }
  else {
    return false;
  }
}

/*
 * insert <key,value> entry in hash table
 * Split & Redistribute bucket when there is overflow and if necessary increase
 * global depth
 */
template <typename K, typename V>
void ExtendibleHash<K, V>::Insert(const K &key, const V &value) {
  int bucket_idx;
  shared_ptr<Bucket> current_bucket;
  do {
    // lock_guard<mutex> lck0(latch_table);
    // bucket_idx = HashKey(key) & ((1 << global_depth) - 1);
    bucket_idx = GetIndexOfBuckets(key);
    current_bucket = buckets[bucket_idx];

    lock_guard<mutex> lck(current_bucket->latch_bucket);

    if (current_bucket->map_k.find(key) != current_bucket->map_k.end() || current_bucket->map_k.size() < bucket_size) {
      current_bucket->map_k[key] = value;
      break;
    }
    
    int mask = (1 << (current_bucket->local_depth));
    current_bucket->local_depth++;

    {
      lock_guard<mutex> lck2(latch_table);
      if (current_bucket->local_depth > global_depth) {

        size_t len = buckets.size();
        for (size_t i = 0; i < len; i++) {
          buckets.push_back(buckets[i]);
        }
        global_depth++;

      }
      bucket_number++;
      auto new_bucket = make_shared<Bucket>(current_bucket->local_depth);

      typename map<K, V>::iterator it;
      for (it = current_bucket->map_k.begin(); it != current_bucket->map_k.end(); it++) {
        if (HashKey(it->first) & mask) {
          new_bucket->map_k[it->first] = it->second;
          it = current_bucket->map_k.erase(it);
          it--;
        } 
      }
      for (size_t i = 0; i < buckets.size(); i++) {
        if (buckets[i] == current_bucket && (i & mask))
          buckets[i] = new_bucket;
      }
    }
  }while(true);
}

template class ExtendibleHash<page_id_t, Page *>;
template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
// test purpose
template class ExtendibleHash<int, std::string>;
template class ExtendibleHash<int, std::list<int>::iterator>;
template class ExtendibleHash<int, int>;
} // namespace scudb
