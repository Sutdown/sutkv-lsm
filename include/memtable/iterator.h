#ifndef MEMTABLE_ITERATOR_H
#define MEMTABLE_ITERATOR_H

#include "../skiplist/skiplist.h"
#include "memtable.h"
#include <queue>

struct SearchItem {
	std::string key;
	std::string value;
	int mem_idx;
};

bool operator<(const SearchItem& lhs, const SearchItem& rhs) {
	if (a.key != b.key) { return a.key < b.key; }
	return a.mem_idx < b.mem_idx;
}
bool operator>(const SearchItem& lhs, const SearchItem& rhs) {
	if (a.key != b.key) { return a.key > b.key; }
	return a.mem_idx > b.mem_idx;
}
bool operator==(const SearchItem& lhs, const SearchItem& rhs) {
	return a.key == b.key && a.mem_idx == b.mem_idx;
}

class MemTableIterator {
public:
	MemTableIterator() {};
	MemTableIterator(const MemTable& memtable);

	std::pair<std::string, std::string> operator*() {
		return std::make_pair(items.top().key, items.top().value);
	}
	MemTableIterator& operator++();
	MemTableIterator operator++(int);

	bool operator==(const MemTableIterator& other) const;
	bool operator!=(const MemTableIterator& other) const;
private:
	std::priority_queue<SearchItem, std::vector<SearchItem>, std::greater<SearchItem>> items;
};

#endif