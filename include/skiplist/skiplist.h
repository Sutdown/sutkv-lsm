#ifndef SKIPLIST_H
#define SKIPLIST_H

#include<iostream>
#include<vector>
#include<memory>
#include<string>
#include<optional>
#include<random>

struct Node {
	std::string key, value;
	std::vector<std::shared_ptr<Node>> forward; // 多层前向指针

	Node(const std::string& key, const std::string& value, int level)
		: key(key), value(value), forward(level, nullptr){}
};

class SkipListIterator {
public:
	SkipListIterator(std::shared_ptr<Node> node) : cur(node) {}

	std::pair<std::string, std::string> operator*() const { 
		return { cur->key, cur->value };
	} // 解引用

	SkipListIterator& operator++() {
		if (cur)cur = cur->forward[0];
		return *this;
	} // 前置++
	SkipListIterator operator++(int) {
		SkipListIterator tmp = *this;
		++(*this);
		return tmp;
	} // 后置++

	bool operator==(const SkipListIterator &other) const {
		return cur == other.cur;
	}
	bool operator!=(const SkipListIterator &other) const {
		return !(*this == other);
	}

	std::string get_key() const { return cur->key; }
	std::string get_value() const { return cur->value; }

	bool is_valid() const { return !cur->value.empty(); }

private:
	std::shared_ptr<Node> cur;
};

class SkipList {
private:
	std::shared_ptr<Node> head;
	int max_level; // 最大层数	
	int cur_level; // 当前层数
	size_t size_bytes; // 跟踪内存使用

public:
	SkipList(int max_level=16); // 构造函数
	~SkipList();             // 析构函数

	// 增删改查
	void put(const std::string& key, const std::string& value);
	std::optional<std::string> get(const std::string& key);
	void remove(const std::string& key);

	// 获取大小
	size_t get_size() const { return size_bytes; }
	int random_level();

	// 清理
	void clear(); 

	// 从磁盘加载
	std::vector<std::pair<std::string, std::string>> flush();

	SkipListIterator begin() const { return SkipListIterator(head->forward[0]); }
	SkipListIterator end() const { return SkipListIterator(nullptr); }
};

#endif // SKIPLIST_H