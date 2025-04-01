#include "skiplist.h"

SkipList::SkipList(int max_level) : max_level(max_level), cur_level(1), size_bytes(0) {
	head = std::make_shared<Node>("", "", max_level);
}

SkipList::~SkipList() {
	clear();
}

void SkipList::put(const std::string& key, const std::string& value) {
	std::vector<std::shared_ptr<Node>> update(max_level, nullptr);
	auto cur = head;
	for (int i = cur_level - 1; i >= 0; i--) {
		// 这其实用update保存小于key的最大结点（可以和key相同）
		// 最终的cur依然小于key
		while (cur->forward[i] != nullptr && cur->forward[i]->key < key) {
			cur = cur->forward[i];
		}
		update[i] = cur; // 每个结点实际只知道该节点和自己后面的结点
	}

	// 这个cur是大于等于key的第一个结点
	cur = cur->forward[0];
	
	// 如果key存在，更新value
	if (cur && cur->key == key) {
		size_bytes += value.size() - cur->value.size();
		cur->value = value;
		return;
	}

	// 如果key不存在，插入新节点
	int level = random_level();
	if (level > cur_level) {
		// 如果新节点的层数大于当前层数，那么更新update
		for (int i = cur_level; i < level; i++) {
			update[i] = head;
		}
		cur_level = level;
	}

	auto new_node = std::make_shared<Node>(key, value, level);
	size_bytes += key.size() + value.size();
	for (int i = 0; i < level; i++) {
		new_node->forward[i] = update[i]->forward[i];
		update[i]->forward[i] = new_node;
	}
}

std::string SkipList::get(const std::string& key) {
	auto cur = head;
	for (int i = cur_level - 1; i >= 0; i--) {
		while (cur->forward[i] != nullptr && cur->forward[i]->key < key) {
			cur = cur->forward[i];
		}
	}

	cur = cur->forward[0];
	if (cur && cur->key == key) {
		return cur->value;
	}
	return std::nullopt;
}

void SkipList::remove(const std::string& key) {
	std::vector<std::shared_ptr<Node>> update(max_level, nullptr);
	auto cur = head;

	for (int i = cur_level - 1; i >= 0; i--) {
		while (cur->forward[i] != nullptr && cur->forward[i]->key < key) {
			cur = cur->forward[i];
		}
		update[i] = cur;
	}

	cur = cur->forward[0];
	if (cur && cur->key == key) {
		// 更新每一层的指针，跳过目标结点
		for (int i = 0; i < cur_level; i++) {
			if (update[i]->forward[i] != cur) {
				break;
			}
			// 删除结点
			update[i]->forward[i] = cur->forward[i];
		}
		size_bytes -= cur->key.size() + cur->value.size();
		while (cur_level > 1 && head->forward[cur_level - 1] == nullptr) {
			cur_level--;
		}
	}
}

void SkipList::clear() {
	head = std::make_shared<Node>("", "", max_level);
	size_bytes = 0;
}

// 实现概率均衡
// 基于梅森旋转算法，生成（0，1）之间的均衡
int SkipList::random_level() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 1);
	int level = 1;
	while (dis(gen) && level < max_level) {
		level++;
	}
	return level;
}

// 遍历最底层链表即可
std::vector<std::pair<std::string, std::string>> SkipList::flush() {
	std::vector<std::pair<std::string, std::string>> res;
	auto cur = head->forward[0];
	while (cur != nullptr) {
		res.push_back({ cur->key, cur->value });
		cur = cur->forward[0];
	}
	return res;
}