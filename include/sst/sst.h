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

/**
 * SST�ļ��Ľṹ, �ο��� https://skyzh.github.io/mini-lsm/week1-04-sst.html
 * ------------------------------------------------------------------------
 * |         Block Section         |  Meta Section | Extra                |
 * ------------------------------------------------------------------------
 * | data block | ... | data block |    metadata   | metadata offset (32) |
 * ------------------------------------------------------------------------

 * ����, metadata ��һ���������һЩ������Ϣ, ����ÿ��Ԫ����һ�� BlockMeta
 �����γ� MetaEntry, MetaEntry �ṹ����:
 * ---------------------------------------------------------------------------------------------------
 * | offset(32) | 1st_key_len(16) | 1st_key(1st_key_len) | last_key_len(16) |
 last_key(last_key_len) |
 * ---------------------------------------------------------------------------------------------------

 * Meta Section �Ľṹ����:
 * ---------------------------------------------------------------
 * | num_entries (32) | MetaEntry | ... | MetaEntry | Hash (32) |
 * ---------------------------------------------------------------
 * ����, num_entries ��ʾ metadata ����ĳ���, Hash �� metadata
 ����Ĺ�ϣֵ(ֻ�������鲿��, ������ num_entries ), ����У�� metadata ��������
 */

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

public:
	// ���ļ��д�sst
	static std::shared_ptr<SST> open(size_t sst_id, FileObj file);
	// ����һ��sst, ֻ������βkey��Ԫ����
	static std::shared_ptr<SST>
	create_sst_with_meta_only(size_t sst_id, size_t file_size,
														const std::string &first_key,
														const std::string &last_key);
	// ����������ȡblock
	std::shared_ptr<Block> read_block(size_t block_idx);

	// �ҵ�key���ڵ�block��idx
	size_t find_block_idx(const std::string &key);

	// ����key���ص�����
	SstIterator get(const std::string &key);

	// ����sst��block������
	size_t num_blocks() const;

	// ����sst����key
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
