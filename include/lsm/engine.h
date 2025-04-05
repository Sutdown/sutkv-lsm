#pragma once

#include "../../include/memtable/memtable.h"
#include "../../include/sst/sst.h"
#include "merge_iterator.h"
#include <cstddef>
#include <memory>
#include <unordered_map>

class LSMEngine
{
public:
  std::string data_dir;
  MemTable memtable;

  std::list<size_t> l0_sst_ids; // L0层的SST ID
  std::unordered_map<size_t, std::shared_ptr<SST>> ssts; // SST ID -> SST对象的映射

  std::shared_mutex ssts_mtx;
  std::shared_ptr<BlockCache> block_cache;

  LSMEngine(std::string path);
  ~LSMEngine();

  /* 1 读取活跃memtable，查到有效or删除记录返回
   * 2 活跃memtable未命中时，遍历冻结的memtable
   * 3 冻结的memtable未命中时，遍历sst
  */
  std::optional<std::string> get(const std::string &key);
  /* 1 写入，value为空表示删除
   * 2 memtable到阈值则冻结
   * 3 冻结memtable到达阈值，则转为sst
  */
  void put(const std::string &key, const std::string &value);

  void remove(const std::string &key);
  void flush();
  void flush_all();

  std::string get_sst_path(size_t sst_id);

  MergeIterator begin();
  MergeIterator end();
};

class LSM
{
private:
  LSMEngine engine;

public:
  LSM(std::string path);
  ~LSM();

  std::optional<std::string> get(const std::string &key);
  void put(const std::string &key, const std::string &value);
  void remove(const std::string &key);

  using LSMIterator = MergeIterator;
  LSMIterator begin();
  LSMIterator end();
};