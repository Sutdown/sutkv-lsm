#pragma once

#include "block.h"
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>
#include <chrono>

struct CacheItem{
  int sst_id;
  int block_id;
  std::shared_ptr<Block> cache_block;
  std::vector<size_t> access_time;
};

struct pair_hash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &p) const {
    return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
  }
};

struct pair_equal {
  template <class T1, class T2>
  bool operator()(const std::pair<T1, T2> &lhs, const std::pair<T1, T2> &rhs) const {
    return lhs.first == rhs.first && lhs.second == rhs.second;
  }
};

class BlockCache {
public:
  BlockCache(size_t capacity, size_t k) : capacity_(capacity), k_(k) {}
  ~BlockCache() = default;

  std::shared_ptr<Block> get(int sst_id, int block_id);
  void put(int sst_id, int block_id, std::shared_ptr<Block> block);

  double hit_rate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return total_requests == 0 ? 0.0 : static_cast<double>(total_hits) / total_requests;
  }

private:
  size_t capacity_; // 缓存的最大容量
  size_t k_;        // 缓存的最大数量
  mutable std::mutex mutex_; // 互斥锁

  std::list<CacheItem> cache_list_;
  std::unordered_map<std::pair<int, int>, std::list<CacheItem>::iterator, pair_hash, pair_equal> cache_map_;

  size_t current_time() const{
    return std::chrono::steady_clock::now().time_since_epoch().count();
  }

  void update_access_time(std::list<CacheItem>::iterator it){
    it->access_time.push_back(current_time());
    // 访问之后不会立即前置，只有当访问次数超过k_时才会前置
    if(it->access_time.size() > k_){
      it->access_time.erase(it->access_time.begin());
    }
    cache_list_.splice(cache_list_.begin(), cache_list_, it);
  }

  mutable size_t total_requests = 0; // 总请求次数
  mutable size_t total_hits = 0;     // 总命中次数
};