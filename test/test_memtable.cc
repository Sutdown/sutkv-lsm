#include "../include/memtable/iterator.h"
#include "../include/memtable/memtable.h"
#include <vector>
#include <string>
#include <gtest/gtest.h>

// 测试基本的插入和查询操作
TEST(MemTableTest, BasicOperations) {
	MemTable memtable;

	// 测试插入和查找
	memtable.put("key1", "value1");
	EXPECT_EQ(memtable.get("key1"), "value1");

	// 测试更新
	memtable.put("key1", "new_value");
	EXPECT_EQ(memtable.get("key1"), "new_value");

	// 测试不存在的key
	EXPECT_FALSE(memtable.get("nonexistent").has_value());
}

// 测试删除操作
TEST(MemTableTest, RemoveOperation) {
	MemTable memtable;
	memtable.put("key1", "value1");
	memtable.remove("key1");
	EXPECT_FALSE(memtable.get("key1").has_value());
}

// 测试冻结表
TEST(MemTableTest, FrozenTable) {
	MemTable memtable;

	// 在当前表中插入数据
	memtable.put("key1", "value1");

	memtable.frozen_cur_table();
	memtable.put("key2", "value2");

	memtable.frozen_cur_table();
	memtable.put("key3", "value3");

	// 验证所有数据都能被访问到
	EXPECT_EQ(memtable.get("key1").value(), "value1");
	EXPECT_EQ(memtable.get("key2").value(), "value2");
	EXPECT_EQ(memtable.get("key3").value(), "value3");
}

// 测试大量数据
TEST(MemTableTest, LargeData) {
	MemTable memtable;
	// 插入大量数据
	for (int i = 0; i < 10000; i++) {
		memtable.put(std::to_string(i), std::to_string(i));
	}
	// 验证所有数据都能被访问到
	for (int i = 0; i < 10000; i++) {
		EXPECT_EQ(memtable.get(std::to_string(i)).value(), std::to_string(i));
	}
}

// 测试内存大小跟踪
TEST(MemTableTest, MemorySize) {
	MemTable memtable;
	// 插入大量数据
	for (int i = 0; i < 10000; i++) {
		memtable.put(std::to_string(i), std::to_string(i));
	}
	EXPECT_EQ(memtable.get_cur_size(), 77780);
	EXPECT_EQ(memtable.get_frozen_size(), 0);
	EXPECT_EQ(memtable.get_total_size(), 77780);
	// 冻结当前表
	memtable.frozen_cur_table();
	// 验证内存大小
	EXPECT_EQ(memtable.get_cur_size(), 0);
	EXPECT_EQ(memtable.get_frozen_size(), 77780);
	EXPECT_EQ(memtable.get_total_size(), 77780);
}

// 测试迭代器在复杂操作序列下的行为
TEST(MemTableTest, Iterator) {
	MemTable memtable;

	// one
	memtable.put("key1", "value1");
	memtable.put("key2", "value2");
	memtable.put("key3", "value3");

	std::vector<std::pair<std::string, std::string>> result1;
	for (auto it = memtable.begin(); it != memtable.end(); ++it) {
		result1.push_back(*it);
	}
	ASSERT_EQ(result1.size(), 3);
	EXPECT_EQ(result1[0].first, "key1");
	EXPECT_EQ(result1[0].second, "value1");
	EXPECT_EQ(result1[2].second, "value3");

	// two
	memtable.frozen_cur_table();
	memtable.put("key2", "value2_updated"); // 更新已存在的key
	memtable.remove("key1");                // 删除一个key
	memtable.put("key4", "value4");         // 添加一个新的key

	std::vector<std::pair<std::string, std::string>> result2;
	for (auto it = memtable.begin(); it != memtable.end(); ++it) {
		result2.push_back(*it);
	}
	ASSERT_EQ(result2.size(), 3); // key1被删除，key4被添加
	EXPECT_EQ(result2[0].first, "key2");
	EXPECT_EQ(result2[0].second, "value2_updated");
	EXPECT_EQ(result2[2].first, "key4");

	// three
	memtable.put("key1", "value1_new"); // 重新插入被删除的key
	memtable.remove("key3"); // 删除一个在第一个frozen table中的key
	memtable.put("key2", "value2_final"); // 再次更新key2
	memtable.put("key5", "value5");       // 插入新key

	std::vector<std::pair<std::string, std::string>> final_result;
	for (auto it = memtable.begin(); it != memtable.end(); ++it) {
		final_result.push_back(*it);
	}
	ASSERT_EQ(final_result.size(), 4);
	EXPECT_EQ(final_result[0].first, "key1");
	EXPECT_EQ(final_result[0].second, "value1_new");

	EXPECT_EQ(final_result[1].first, "key2");
	EXPECT_EQ(final_result[1].second, "value2_final");

	EXPECT_EQ(final_result[2].first, "key4");
	EXPECT_EQ(final_result[2].second, "value4");

	EXPECT_EQ(final_result[3].first, "key5");
	EXPECT_EQ(final_result[3].second, "value5");

	EXPECT_FALSE(memtable.get("key3").has_value());
}

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}