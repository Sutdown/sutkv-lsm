#pragma once

#include "../../include/block/block.h"
#include "../../include/block/blockmeta.h"
#include "../../include/utils/files.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

// ��ȡSST�ļ�����
// block section���洢�ܶ�key-value�ԣ�
// meta section�����Ա�����뱣�����ڴ��У������¼�����ݿ��ƫ�����������Сkey���Ӷ�ʵ�ֿ��ٶ�λ��
// extra information
class SST :public std::enable_shared_from_this<SST> {
	friend class SSTBuilder;

private:
	FileObj file;
	std::vector<BlockMeta> block_metas; // ��Ԫ����
	uint32_t meta_block_offset;
	size_t sst_id;

	std::string first_key; // ��һ��key
	std::string last_key; // ���һ��key

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

// ����SST�ļ�����
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
	// ����һ��sst��������ָ��Ŀ��block�Ĵ�С
	SSTBuilder(size_t block_size);

	void add(const std::string& key, const std::string& value);

	size_t estimated_size() const;

	void finish_block();

	SST build(size_t sst_id, const std::string& path);
};