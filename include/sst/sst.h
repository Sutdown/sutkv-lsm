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
	std::vector<BlockMeta> meta_entries; // Ԫ����(����Ԫ���ݺͶ��������ݵ�ת��)
	uint32_t meta_block_offset; // �ֽ���
	
	size_t sst_id; // sst��id
	std::string first_key;
	std::string last_key;

public:
	// ���ļ�д��SST��
	static std::shared_ptr<SST> open(size_t sst_id, FileObj file);

	// ����һ���յ�SST, ������Ԫ����
	static std::shared_ptr<SST> create_sst_with_meta_only(size_t sst_id, size_t file_size,
														const std::string &first_key, const std::string &last_key);
	
  // ��ȡ�ļ��е�Ԫ���ݿ�
	std::shared_ptr<Block> read_block(size_t block_idx);

	// Ѱ�����ݿ������
	size_t find_block_idx(const std::string &key);

	// ��ȡָ��key��ֵ
	SstIterator get(const std::string &key);

	// ����ָ��key��ֵ
	size_t num_blocks() const;

	// ����sst�ĵ�һ��key
	std::string get_first_key() const;

	// ����sst��βkey
	std::string get_last_key() const;

	// ����sst�Ĵ�С
	size_t sst_size() const;

	// ����sst��id
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
	// ����һ��sst������, ָ��Ŀ��block�Ĵ�С
	SSTBuilder(size_t block_size);

	// ���һ��key-value��
	void add(const std::string &key, const std::string &value);

	// ����sst�Ĵ�С
	size_t estimated_size() const;

	// ��ɵ�ǰblock�Ĺ���, ����blockд��data, �������µ�block
	void finish_block();
	
	// ����sst, ��sstд���ļ�������SST������
	std::shared_ptr<SST> build(size_t sst_id, const std::string &path);
};
