#include"../../include/memtable/iterator.h"
#include <vector>

MemTableIterator::MemTableIterator(const MemTable& memtable) {
	auto cur_table = memtable.cur_table;

	// ��õ�ǰ���е�����Ԫ��
	for (auto iter = cur_table->begin(); iter != cur_table->end(); iter++) {
		items.push(SearchItem{ iter.get_key(), iter.get_value(), 0 });
	}

	// �������ж�����е�Ԫ�أ�ÿ�������㼶��ͬ
	int level = 1;
	for (auto ft = memtable.frozen_tables.begin(); ft != memtable.frozen_tables.end(); ft++) {
		auto table = *ft;
		for (auto iter = table->begin(); iter != table->end(); iter++) {
			items.push(SearchItem{ iter.get_key(), iter.get_value(), level });
		}
		level++;
	}

	// ɾ����ֵ�Լ����ظ���Ԫ��
	// ͷ�巨�������Ԫ��������ǰ�棬�㼶С�����ȼ���
	while (!items.empty() && items.top().value.empty()) {
		auto del_key = items.top().key;
		while (!items.empty() && items.top().key == del_key) {
			items.pop();
		}
	}
}

MemTableIterator& MemTableIterator::operator++() {
	if (items.empty()) return *this;
	// �������еĵ�һ��Ԫ��
	auto top = items.top();
	items.pop();

	// ɾ����top.key��ͬ��Ԫ��
	// ֻ�е����ĵ�һ��Ԫ������Ч�ģ�׷��д��
	while (!items.empty() && items.top().key == top.key) {
		items.pop();
	}

	// ɾ����ֵ�ͼ��ظ���Ԫ��
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