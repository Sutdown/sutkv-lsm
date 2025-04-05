#include "../../include/block/block_cache.h"
#include "../../include/block/block.h"

std::shared_ptr<Block> BlockCache::get(int sst_id, int block_id){
  std::lock_guard<std::mutex> lock(mutex_);
  ++total_requests;
  auto key = std::make_pair(sst_id, block_id);
  auto it = cache_map_.find(key);
  if(it == cache_map_.end()){
    return nullptr;
  }
  ++total_hits;
  update_access_time(it->second);
  return it->second->cache_block;
}

void BlockCache::put(int sst_id, int block_id, std::shared_ptr<Block> block){
  std::lock_guard<std::mutex> lock(mutex_);
  auto key = std::make_pair(sst_id, block_id);
  auto it = cache_map_.find(key);
  if(it != cache_map_.end()){
    // 如果已经存在，更新缓存
    it->second->cache_block = block;
    update_access_time(it->second);
  }else{
    // 如果不存在，创建新的缓存项
    if (cache_list_.size() >= capacity_) {
      // 移除最久未使用的缓存项
      cache_map_.erase(std::make_pair(cache_list_.back().sst_id, cache_list_.back().block_id));
      cache_list_.pop_back();
    }

    CacheItem new_item = {sst_id, block_id, block, {current_time()}};
    cache_list_.push_front(new_item);
    cache_map_[key] = cache_list_.begin();
  }
}