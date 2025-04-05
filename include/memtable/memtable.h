#ifndef MEMTABLE_H
#define MEMTABLE_H

#include "../iterator/iterator.h"
#include "../skiplist/skiplist.h"
#include <cstddef>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>

class SST;
class SSTBuilder;

class MemTable {
	friend class HeapIterator;

public:
	MemTable();
	~MemTable();

	void put(const std::string& key, const std::string& value);
	std::optional<std::string> get(const std::string& key);
	void remove(const std::string& key);

	void clear();
	std::shared_ptr<SST> flush_last(SSTBuilder &builder, std::string &sst_path, size_t sst_id);

	void frozen_cur_table();

	size_t get_cur_size();
	size_t get_frozen_size();
	size_t get_total_size();

	HeapIterator begin();
	HeapIterator end();

private:
	std::shared_ptr<SkipList> current_table; // Current table
	std::list<std::shared_ptr<SkipList>> frozen_tables; // List of frozen tables
	size_t frozen_bytes; // ͷ�巨
	std::shared_mutex rx_mtx; // ��д��
};

#endif