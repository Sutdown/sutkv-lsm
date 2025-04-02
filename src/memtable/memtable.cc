#include "../../include/memtable/memtable.h"
#include "../../include/memtable/iterator.h"
#include "../../include/skiplist/skiplist.h"
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

MemTable::MemTable() : frozen_bytes(0) {
	current_table = std::make_shared<SkipList>();
}
MemTable::~MemTable() = default;

void MemTable::put(const std::string& key, const std::string& value) {
	current_table->put(key, value);
}

std::optional<std::string> MemTable::get(const std::string& key) {
	auto res = current_table->get(key);
	if (res.has_value()) {
		auto data = res.value();
		if (data!="") {
			return data;
		} else {
			return std::nullopt;
		}
	}

	for(auto &table:frozen_tables) {
		auto res = table->get(key);
		if (res.has_value()) {
			auto data = res.value();
			if (data != "") {
				return data;
			} else {
				return std::nullopt;
			};
		}
	}

	return std::nullopt;
}

void MemTable::remove(const std::string& key) {
	current_table->put(key, "");
}

void MemTable::clear() {
	current_table->clear();
}

void MemTable::frozen_cur_table() {
	frozen_bytes += current_table->get_size();
	frozen_tables.push_front(std::move(current_table));
	current_table = std::make_shared<SkipList>();
}

size_t MemTable::get_cur_size() const {
	return current_table->get_size();
}

size_t MemTable::get_frozen_size() const {
	return frozen_bytes;
}

size_t MemTable::get_total_size() const {
	return current_table->get_size() + get_frozen_size();
}

MemTableIterator MemTable::begin() const {
	return MemTableIterator(*this);
}

MemTableIterator MemTable::end() const {
	return MemTableIterator{};
}