#include "src/skiplist/skiplist.h"
#include<gtest/gtest.h>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>

// 基础功能
TEST(SkipListTest, Basic) {
	SkipList sl;

	sl.put("a", "1");
	EXPECT_EQ(sl.get("a").value(), "1");

	s1.put("a", "2");
	EXPECT_EQ(sl.get("a").value(), "2");

	sl.remove("a");
	EXPECT_FALSE(sl.get("a").has_value());
}

// 迭代器
TEST(SkipListTest, Iterator) {
	SkipList sl;
	std::vector<std::pair<std::string, std::string>> data = {
		{"a", "1"}, {"b", "2"}, {"c", "3"}, {"d", "4"}, {"e", "5"}
	};

	for (auto& p : data) {
		sl.put(p.first, p.second);
	}

	std::vector<std::pair<std::string, std::string>> result;
	for (auto it = sl.begin(); it != sl.end(); ++it) {
		result.push_back(*it);
	}

	EXPECT_EQ(result.size(), data.size());
	std::sort(result.begin(), result.end());
	std::sort(data.begin(), data.end());
	EXPECT_EQ(result, data);
}

// 测试大量数据插入和查找
TEST(SkipListTest, LargeDataADD) {
	SkipList sl;
	std::unordered_set<std::string> keys;
	std::vector<std::pair<std::string, std::string>> data;
	for (int i = 0; i < 10000; i++) {
		std::string key = std::to_string(i);
		std::string value = std::to_string(i * i);
		keys.insert(key);
		data.push_back({ key, value });
		sl.put(key, value);
	}

	for (auto& p : data) {
		EXPECT_EQ(sl.get(p.first).value(), p.second);
	}
}

// 测试大量数据删除
TEST(SkipListTest, LargeDataRemove) {
	SkipList sl;
	std::unordered_set<std::string> keys;
	std::vector<std::pair<std::string, std::string>> data;
	for (int i = 0; i < 10000; i++) {
		std::string key = std::to_string(i);
		std::string value = std::to_string(i * i);
		keys.insert(key);
		data.push_back({ key, value });
		sl.put(key, value);
	}

	for (int i = 0; i < 10000; i++) {
		std::string key = std::to_string(i);
		sl.remove(key);
		EXPECT_FALSE(sl.get(key).has_value());
	}
}

// 测试重复插入
TEST(SkipListTest, DuplicateInsert) {
	SkipList sl;
	sl.put("a", "1");
	sl.put("a", "2");
	EXPECT_EQ(sl.get("a").value(), "2");
}

// 测试空跳表
TEST(SkipListTest, EmptySkipList) {
	SkipList sl;
	EXPECT_FALSE(skipList.get("nonexistent_key").has_value());
	skipList.remove("nonexistent_key");
}

// 测试随机插入和删除
TEST(SkipListTest, RandomInsertAndRemove) {
	SkipList sl;
	std::unordered_set<std::string> keys;
	std::vector<std::pair<std::string, std::string>> data;
	for (int i = 0; i < 10000; i++) {
		std::string key = std::to_string(i);
		std::string value = std::to_string(i * i);
		keys.insert(key);
		data.push_back({ key, value });
		sl.put(key, value);
	}

	// 随机删除一半
	for (int i = 0; i < 10000; i++) {
		std::string key = std::to_string(i);
		if (rand() % 2) {
			sl.remove(key);
			EXPECT_FALSE(sl.get(key).has_value());
		}
	}
}

// 测试内存大小跟踪
TEST(SkipListTest, MemoryTracking) {
	SkipList sl;
	sl.put("key1", "value1");
	sl.put("key2", "value2");

	size_t expected_size = sizeof("key1") - 1 + sizeof("value1") - 1 +
		sizeof("key2") - 1 + sizeof("value2") - 1;
	EXPECT_EQ(sl.get_size(), expected_size);

	sl.remove("key1");
	expected_size -= sizeof("key1") - 1 + sizeof("value1") - 1;
	EXPECT_EQ(sl.get_size(), expected_size);

	sl.clear();
	EXPECT_EQ(sl.get_size_bytes(), 0);
}

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}