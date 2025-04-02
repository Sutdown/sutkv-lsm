#include "../include/memtable/iterator.h"
#include "../include/memtable/memtable.h"
#include <vector>
#include <string>
#include <gtest/gtest.h>

// ���Ի����Ĳ���Ͳ�ѯ����
TEST(MemTableTest, BasicOperations) {
	MemTable memtable;

	// ���Բ���Ͳ���
	memtable.put("key1", "value1");
	EXPECT_EQ(memtable.get("key1"), "value1");

	// ���Ը���
	memtable.put("key1", "new_value");
	EXPECT_EQ(memtable.get("key1"), "new_value");

	// ���Բ����ڵ�key
	EXPECT_FALSE(memtable.get("nonexistent").has_value());
}

// ����ɾ������
TEST(MemTableTest, RemoveOperation) {
	MemTable memtable;
	memtable.put("key1", "value1");
	memtable.remove("key1");
	EXPECT_FALSE(memtable.get("key1").has_value());
}

// ���Զ����
TEST(MemTableTest, FrozenTable) {
	MemTable memtable;

	// �ڵ�ǰ���в�������
	memtable.put("key1", "value1");

	memtable.frozen_cur_table();
	memtable.put("key2", "value2");

	memtable.frozen_cur_table();
	memtable.put("key3", "value3");

	// ��֤�������ݶ��ܱ����ʵ�
	EXPECT_EQ(memtable.get("key1").value(), "value1");
	EXPECT_EQ(memtable.get("key2").value(), "value2");
	EXPECT_EQ(memtable.get("key3").value(), "value3");
}

// ���Դ�������
TEST(MemTableTest, LargeData) {
	MemTable memtable;
	// �����������
	for (int i = 0; i < 10000; i++) {
		memtable.put(std::to_string(i), std::to_string(i));
	}
	// ��֤�������ݶ��ܱ����ʵ�
	for (int i = 0; i < 10000; i++) {
		EXPECT_EQ(memtable.get(std::to_string(i)).value(), std::to_string(i));
	}
}

// �����ڴ��С����
TEST(MemTableTest, MemorySize) {
	MemTable memtable;
	// �����������
	for (int i = 0; i < 10000; i++) {
		memtable.put(std::to_string(i), std::to_string(i));
	}
	EXPECT_EQ(memtable.get_cur_size(), 77780);
	EXPECT_EQ(memtable.get_frozen_size(), 0);
	EXPECT_EQ(memtable.get_total_size(), 77780);
	// ���ᵱǰ��
	memtable.frozen_cur_table();
	// ��֤�ڴ��С
	EXPECT_EQ(memtable.get_cur_size(), 0);
	EXPECT_EQ(memtable.get_frozen_size(), 77780);
	EXPECT_EQ(memtable.get_total_size(), 77780);
}

// ���Ե������ڸ��Ӳ��������µ���Ϊ
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
	memtable.put("key2", "value2_updated"); // �����Ѵ��ڵ�key
	memtable.remove("key1");                // ɾ��һ��key
	memtable.put("key4", "value4");         // ���һ���µ�key

	std::vector<std::pair<std::string, std::string>> result2;
	for (auto it = memtable.begin(); it != memtable.end(); ++it) {
		result2.push_back(*it);
	}
	ASSERT_EQ(result2.size(), 3); // key1��ɾ����key4�����
	EXPECT_EQ(result2[0].first, "key2");
	EXPECT_EQ(result2[0].second, "value2_updated");
	EXPECT_EQ(result2[2].first, "key4");

	// three
	memtable.put("key1", "value1_new"); // ���²��뱻ɾ����key
	memtable.remove("key3"); // ɾ��һ���ڵ�һ��frozen table�е�key
	memtable.put("key2", "value2_final"); // �ٴθ���key2
	memtable.put("key5", "value5");       // ������key

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