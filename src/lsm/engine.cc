#include "../../include/lsm/engine.h"
#include "../../include/consts.h"
#include "../../include/sst/sst.h"
#include "../../include/sst/sst_iterator.h"
#include <filesystem>
#include <vector>

/* LSMEngine */
LSMEngine::LSMEngine(std::string path) : data_dir(path)
{
  // 创建数据目录
  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directory(path);
  } else {
    // 如果目录存在，则检查是否有 sst 文件并加载
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
      if (!entry.is_regular_file()) {
        continue;
      }

      std::string filename = entry.path().filename().string();
      // SST文件名格式为: sst_{id}
      if (filename.substr(0, 4) != "sst_") {
        continue;
      }

      // 提取SST ID
      std::string id_str = filename.substr(4, filename.length() - 4); // 4 for "sst_"
      if (id_str.empty()) {
        continue;
      }
      size_t sst_id = std::stoull(id_str);

      // 初始化 block_cahce
      block_cache = std::make_shared<BlockCache>(LSMmm_BLOCK_CACHE_CAPACITY, LSMmm_BLOCK_CACHE_K);

      // 加载SST文件, 初始化时需要加写锁
      std::unique_lock<std::shared_mutex> lock(ssts_mtx); // 写锁
      std::string sst_path = get_sst_path(sst_id);
      auto sst = SST::open(sst_id, FileObj::open(sst_path), block_cache);
      ssts[sst_id] = sst;

      // 所有加载的SST都属于L0层
      l0_sst_ids.push_back(sst_id);
    }

    // 按ID排序，确保顺序一致
    l0_sst_ids.sort();
  }
}

LSMEngine::~LSMEngine()
{
  // 需要将所有内存表写入磁盘
  flush_all();
}

std::optional<std::string> LSMEngine::get(const std::string &key)
{
  // 1. 先查找 memtable
  auto value = memtable.get(key);
  if (value.has_value()) {
    if (value.value().size() > 0) {
      // 值存在且不为空（没有被删除）
      return value;
    } else {
      // 空值表示被删除了
      return std::nullopt;
    }
  }

  // 2. l0 sst中查询
  std::shared_lock<std::shared_mutex> rlock(ssts_mtx);
  for (auto &sst_id : l0_sst_ids) {
    auto sst = ssts[sst_id];
    auto sst_iterator = sst->get(key);
    if (sst_iterator != sst->end()) {
      if ((sst_iterator)->second.size() > 0) {
        // 值存在且不为空
        return sst_iterator->second;
      } else {
        // 空值表示被删除
        return std::nullopt;
      }
    }
  }

  return std::nullopt;
}

void LSMEngine::put(const std::string &key, const std::string &value)
{
  memtable.put(key, value);

  // 如果 memtable 太大，需要刷新到磁盘
  if (memtable.get_total_size() >= LSM_TOL_MEM_SIZE_LIMIT) {
    flush();
  }
}

void LSMEngine::remove(const std::string &key)
{
  // 在 LSM 中，删除实际上是插入一个空值
  memtable.remove(key);
}

void LSMEngine::flush()
{
  if (memtable.get_total_size() == 0) {
    return;
  }

  // 1. 创建新的 SST ID
  size_t new_sst_id = l0_sst_ids.empty() ? 0 : l0_sst_ids.back() + 1;

  // 2. 准备 SSTBuilder
  SSTBuilder builder(LSM_BLOCK_SIZE); // 4KB block size

  // 3. 将 memtable 中最旧的表写入 SST
  auto sst_path = get_sst_path(new_sst_id);
  auto new_sst = memtable.flush_last(builder, sst_path, new_sst_id, block_cache);
  
  std::unique_lock<std::shared_mutex> lock(ssts_mtx); // 写锁
  // 4. 更新内存索引
  ssts[new_sst_id] = new_sst;

  // 5. 更新 sst_ids
  l0_sst_ids.push_back(new_sst_id);
}

void LSMEngine::flush_all()
{
  while (memtable.get_total_size() > 0) {
    flush();
  }
}

std::string LSMEngine::get_sst_path(size_t sst_id)
{
  // sst的文件路径格式为: data_dir/sst_<sst_id>，sst_id格式化为4位数字
  std::stringstream ss;
  ss << data_dir << "/sst_" << std::setfill('0') << std::setw(4) << sst_id;
  return ss.str();
}

MergeIterator LSMEngine::begin()
{
  std::vector<SearchItem> item_vec;
  std::shared_lock<std::shared_mutex> lock(ssts_mtx);
  for (auto &sst_id : l0_sst_ids) {
    auto sst = ssts[sst_id];
    for (auto iter = sst->begin(); iter != sst->end(); ++iter) {
      item_vec.emplace_back(iter.key(), iter.value(), sst_id);
    }
  }
  // item_vec l0层中SST中的每对检查项
  // l0_iter是l0层的迭代器，用来合并
  HeapIterator l0_iter(item_vec);

  auto mem_iter = memtable.begin(); // memtable的迭代器

  // 合并l0层和memtable的迭代器
  return MergeIterator(mem_iter, l0_iter);
}

MergeIterator LSMEngine::end() { return MergeIterator{}; }

/* LSM */
LSM::LSM(std::string path) : engine(path) {}

LSM::~LSM()
{
  // 确保所有数据都已经刷新到磁盘
  engine.flush();
}

std::optional<std::string> LSM::get(const std::string &key)
{
  return engine.get(key);
}
void LSM::put(const std::string &key, const std::string &value)
{
  engine.put(key, value);
}
void LSM::remove(const std::string &key) { engine.remove(key); }

LSM::LSMIterator LSM::begin() { return engine.begin(); }
LSM::LSMIterator LSM::end() { return engine.end(); }