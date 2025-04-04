#pragma once

#include "../../include/block/block.h"
#include "../../include/block/blockmeta.h"
#include "../../include/utils/files.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

// 读取SST文件的类
// block section（存储很多key-value对）
// meta section（可以编码解码保存在内存中，里面记录了数据块的偏移量，最大最小key，从而实现快速定位）
// extra information
class SST :public std::enable_shared_from_this<SST> {
	friend class SSTBuilder;

private:
	FileObj file;
	std::vector<BlockMeta> block_metas; // 块元数据
	uint32_t meta_block_offset;
	size_t sst_id;

	std::string first_key; // 第一个key
	std::string last_key; // 最后一个key

public:
	static SST open(size_t sst_id, FileObj file);
	static SST create_sst_with_meta_only(size_t sst_id, size_t file_size,
		const std::string& first_key, const std::string& last_key);

	std::shared_ptr<Block> read_block(size_t block_idx);
	size_t find_block_idx(const std::string& key);

	size_t num_blocks() const;
	size_t sst_size() const;
	size_t get_sst_id() const;

	std::string get_first_key() const;
	std::string get_last_key() const;
};

// 构建SST文件的类
class SSTBuilder {
private:
	std::string first_key;
	std::string last_key;
	std::vector<BlockMeta> block_metas;

	Block block;
	std::vector<uint8_t> block_data;
	std::vector<uint32_t> key_hashes;
	size_t block_size;
public:
	// 创建一个sst构建器，指定目标block的大小
	SSTBuilder(size_t block_size);

	void add(const std::string& key, const std::string& value);

	size_t estimated_size() const;

	void finish_block();

	SST build(size_t sst_id, const std::string& path);
};