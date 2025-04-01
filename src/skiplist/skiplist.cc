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
		// ����ʵ��update����С��key������㣨���Ժ�key��ͬ��
		// ���յ�cur��ȻС��key
		while (cur->forward[i] != nullptr && cur->forward[i]->key < key) {
			cur = cur->forward[i];
		}
		update[i] = cur; // ÿ�����ʵ��ֻ֪���ýڵ���Լ�����Ľ��
	}

	// ���cur�Ǵ��ڵ���key�ĵ�һ�����
	cur = cur->forward[0];
	
	// ���key���ڣ�����value
	if (cur && cur->key == key) {
		size_bytes += value.size() - cur->value.size();
		cur->value = value;
		return;
	}

	// ���key�����ڣ������½ڵ�
	int level = random_level();
	if (level > cur_level) {
		// ����½ڵ�Ĳ������ڵ�ǰ��������ô����update
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
		// ����ÿһ���ָ�룬����Ŀ����
		for (int i = 0; i < cur_level; i++) {
			if (update[i]->forward[i] != cur) {
				break;
			}
			// ɾ�����
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

// ʵ�ָ��ʾ���
// ����÷ɭ��ת�㷨�����ɣ�0��1��֮��ľ���
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

// ������ײ�������
std::vector<std::pair<std::string, std::string>> SkipList::flush() {
	std::vector<std::pair<std::string, std::string>> res;
	auto cur = head->forward[0];
	while (cur != nullptr) {
		res.push_back({ cur->key, cur->value });
		cur = cur->forward[0];
	}
	return res;
}