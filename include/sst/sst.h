#pragma once

#include "../block/block.h"
#include "../block/block_cache.h"
#include "../block/blockmeta.h"
#include "../utils/files.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class SstIterator;

class SST : public std::enable_shared_from_this<SST>
{
	friend class SSTBuilder;

private:
	FileObj file;
	std::vector<BlockMeta> meta_entries;
	uint32_t meta_block_offset;
	size_t sst_id;
	std::string first_key;
	std::string last_key;
	std::shared_ptr<BlockCache> block_cache;

public:
	// 从文件中打开sst
	static std::shared_ptr<SST> open(size_t sst_id, FileObj file, std::shared_ptr<BlockCache> block_cache);
	// 创建一个sst, 只包含首尾key的元数据
	static std::shared_ptr<SST> create_sst_with_meta_only(size_t sst_id, size_t file_size,
																												const std::string &first_key, const std::string &last_key, std::shared_ptr<BlockCache> block_cache);
	// 根据索引读取block
	std::shared_ptr<Block> read_block(size_t block_idx);

	// 找到key所在的block的idx
	size_t find_block_idx(const std::string &key);

	// 根据key返回迭代器
	SstIterator get(const std::string &key);

	// 返回sst中block的数量
	size_t num_blocks() const;

	// 返回sst的首key
	std::string get_first_key() const;

	// 返回sst的尾key
	std::string get_last_key() const;

	// 返回sst的大小
	size_t sst_size() const;

	// 返回sst的id
	size_t get_sst_id() const;

	SstIterator begin();
	SstIterator end();
};

class SSTBuilder
{
private:
	Block block;
	std::string first_key;
	std::string last_key;
	std::vector<BlockMeta> meta_entries;
	std::vector<uint8_t> data;
	size_t block_size;
	std::vector<uint32_t> key_hashes;

public:
	// 创建一个sst构建器, 指定目标block的大小
	SSTBuilder(size_t block_size);
	// 添加一个key-value对
	void add(const std::string &key, const std::string &value);
	// 估计sst的大小
	size_t estimated_size() const;
	// 完成当前block的构建, 即将block写入data, 并创建新的block
	void finish_block();
	// 构建sst, 将sst写入文件并返回SST描述类
	std::shared_ptr<SST> build(size_t sst_id, const std::string &path, std::shared_ptr<BlockCache> block_cache);
};
