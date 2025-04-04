#pragma once

#include "../../include/block/block.h"
#include "../../include/block/blockmeta.h"
#include "../../include/utils/files.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class SstIterator;

/*
 * block section: key-value
 * meta section : offset, firstkey, lastkey(nums, hash)
 * extra
*/

class SST : public std::enable_shared_from_this<SST>
{
	friend class SSTBuilder;

private:
	FileObj file;
	std::vector<BlockMeta> meta_entries; // 元数据(包含元数据和二进制数据的转换)
	uint32_t meta_block_offset; // 字节流
	
	size_t sst_id; // sst的id
	std::string first_key;
	std::string last_key;

public:
	// 打开文件写入SST中
	static std::shared_ptr<SST> open(size_t sst_id, FileObj file);

	// 创建一个空的SST, 仅包含元数据
	static std::shared_ptr<SST> create_sst_with_meta_only(size_t sst_id, size_t file_size,
														const std::string &first_key, const std::string &last_key);
	
  // 读取文件中的元数据块
	std::shared_ptr<Block> read_block(size_t block_idx);

	// 寻找数据块的索引
	size_t find_block_idx(const std::string &key);

	// 读取指定key的值
	SstIterator get(const std::string &key);

	// 返回指定key的值
	size_t num_blocks() const;

	// 返回sst的第一个key
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
	std::shared_ptr<SST> build(size_t sst_id, const std::string &path);
};
