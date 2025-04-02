#include"../../include/memtable/iterator.h"
#include <vector>

MemTableIterator::MemTableIterator(const MemTable& memtable) {
	auto cur_table = memtable.cur_table;

	// 获得当前表中的所有元素
	for (auto iter = cur_table->begin(); iter != cur_table->end(); iter++) {
		items.push(SearchItem{ iter.get_key(), iter.get_value(), 0 });
	}

	// 遍历所有冻结表中的元素，每个冻结表层级不同
	int level = 1;
	for (auto ft = memtable.frozen_tables.begin(); ft != memtable.frozen_tables.end(); ft++) {
		auto table = *ft;
		for (auto iter = table->begin(); iter != table->end(); iter++) {
			items.push(SearchItem{ iter.get_key(), iter.get_value(), level });
		}
		level++;
	}

	// 删除空值以及键重复的元素
	// 头插法，后进的元素在链表前面，层级小，优先级高
	while (!items.empty() && items.top().value.empty()) {
		auto del_key = items.top().key;
		while (!items.empty() && items.top().key == del_key) {
			items.pop();
		}
	}
}

MemTableIterator& MemTableIterator::operator++() {
	if (items.empty()) return *this;
	// 弹出队列的第一个元素
	auto top = items.top();
	items.pop();

	// 删除和top.key相同的元素
	// 只有弹出的第一个元素是有效的（追加写）
	while (!items.empty() && items.top().key == top.key) {
		items.pop();
	}

	// 删除空值和键重复的元素
	while (!items.empty() && items.top().value.empty()) {
		auto del_key = items.top().key;
		while (!items.empty() && items.top().key == del_key) {
			items.pop();
		}
	}
	
	return *this;
}

MemTableIterator MemTableIterator::operator++(int) {
	MemTableIterator tmp = *this;
	++*this;
	return tmp;
}

bool MemTableIterator::operator==(const MemTableIterator& other) const {
	if (items.empty() && other.items.empty()) return true;
	if (items.empty() || other.items.empty()) return false;
	return items.top().key == other.items.top().key && items.top().value == other.items.top().value;
}

bool MemTableIterator::operator!=(const MemTableIterator& other) const {
	return !(*this==other);
}