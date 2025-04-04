#include "../include/consts.h"
#include "../include/iterator/iterator.h"
#include "../include/memtable/memtable.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

// ���Ի����Ĳ���Ͳ�ѯ����
TEST(MemTableTest, BasicOperations)
{
	MemTable memtable;

	// ���Բ���Ͳ���
	memtable.put("key1", "value1");
	EXPECT_EQ(memtable.get("key1").value(), "value1");

	// ���Ը���
	memtable.put("key1", "new_value");
	EXPECT_EQ(memtable.get("key1").value(), "new_value");

	// ���Բ����ڵ�key
	EXPECT_FALSE(memtable.get("nonexistent").has_value());
}

// ����ɾ������
TEST(MemTableTest, RemoveOperations)
{
	MemTable memtable;

	// ���벢ɾ��
	memtable.put("key1", "value1");
	memtable.remove("key1");
	EXPECT_FALSE(memtable.get("key1").has_value());

	// ɾ�������ڵ�key
	memtable.remove("nonexistent");
	EXPECT_FALSE(memtable.get("nonexistent").has_value());
}

// ���Զ�������
TEST(MemTableTest, FrozenTableOperations)
{
	MemTable memtable;

	// �ڵ�ǰ���в�������
	memtable.put("key1", "value1");
	memtable.put("key2", "value2");

	// ���ᵱǰ��
	memtable.frozen_cur_table();

	// ���µĵ�ǰ���в�������
	memtable.put("key3", "value3");

	// ��֤�������ݶ��ܱ����ʵ�
	EXPECT_EQ(memtable.get("key1").value(), "value1");
	EXPECT_EQ(memtable.get("key2").value(), "value2");
	EXPECT_EQ(memtable.get("key3").value(), "value3");
}

// ���Դ������ݲ���
TEST(MemTableTest, LargeScaleOperations)
{
	MemTable memtable;
	const int num_entries = 1000;

	// �����������
	for (int i = 0; i < num_entries; i++)
	{
		std::string key = "key" + std::to_string(i);
		std::string value = "value" + std::to_string(i);
		memtable.put(key, value);
	}

	// ��֤����
	for (int i = 0; i < num_entries; i++)
	{
		std::string key = "key" + std::to_string(i);
		std::string expected = "value" + std::to_string(i);
		EXPECT_EQ(memtable.get(key).value(), expected);
	}
}

// �����ڴ��С����
TEST(MemTableTest, MemorySizeTracking)
{
	MemTable memtable;

	// ��ʼ��СӦ��Ϊ0
	EXPECT_EQ(memtable.get_total_size(), 0);

	// ������ݺ��СӦ������
	memtable.put("key1", "value1");
	EXPECT_GT(memtable.get_cur_size(), 0);

	// ������frozen_sizeӦ������
	size_t size_before_freeze = memtable.get_total_size();
	memtable.frozen_cur_table();
	EXPECT_EQ(memtable.get_frozen_size(), size_before_freeze);
}

// ���Զ�ζ�������
TEST(MemTableTest, MultipleFrozenTables)
{
	MemTable memtable;

	// ��һ�ζ���
	memtable.put("key1", "value1");
	memtable.frozen_cur_table();

	// �ڶ��ζ���
	memtable.put("key2", "value2");
	memtable.frozen_cur_table();

	// �ڵ�ǰ�����������
	memtable.put("key3", "value3");

	// ��֤�������ݶ��ܷ���
	EXPECT_EQ(memtable.get("key1").value(), "value1");
	EXPECT_EQ(memtable.get("key2").value(), "value2");
	EXPECT_EQ(memtable.get("key3").value(), "value3");
}

// ���Ե������ڸ��Ӳ��������µ���Ϊ
TEST(MemTableTest, IteratorComplexOperations)
{
	MemTable memtable;

	// ��һ����������������
	memtable.put("key1", "value1");
	memtable.put("key2", "value2");
	memtable.put("key3", "value3");

	// ��֤��һ������
	std::vector<std::pair<std::string, std::string>> result1;
	for (auto it = memtable.begin(); it != memtable.end(); ++it)
	{
		result1.push_back(*it);
	}
	ASSERT_EQ(result1.size(), 3);
	EXPECT_EQ(result1[0].first, "key1");
	EXPECT_EQ(result1[0].second, "value1");
	EXPECT_EQ(result1[2].second, "value3");

	// ���ᵱǰ��
	memtable.frozen_cur_table();

	// �ڶ������������º�ɾ��
	memtable.put("key2", "value2_updated"); // �����Ѵ��ڵ�key
	memtable.remove("key1");								// ɾ��һ��key
	memtable.put("key4", "value4");					// ������key

	// ��֤�ڶ�������
	std::vector<std::pair<std::string, std::string>> result2;
	for (auto it = memtable.begin(); it != memtable.end(); ++it)
	{
		result2.push_back(*it);
	}
	ASSERT_EQ(result2.size(), 3); // key1��ɾ����key4�����
	EXPECT_EQ(result2[0].first, "key2");
	EXPECT_EQ(result2[0].second, "value2_updated");
	EXPECT_EQ(result2[2].first, "key4");

	// �ٴζ��ᵱǰ��
	memtable.frozen_cur_table();

	// ��������������ϲ���
	memtable.put("key1", "value1_new");		// ���²��뱻ɾ����key
	memtable.remove("key3");							// ɾ��һ���ڵ�һ��frozen table�е�key
	memtable.put("key2", "value2_final"); // �ٴθ���key2
	memtable.put("key5", "value5");				// ������key

	// ��֤���ս��
	std::vector<std::pair<std::string, std::string>> final_result;
	for (auto it = memtable.begin(); it != memtable.end(); ++it)
	{
		final_result.push_back(*it);
	}

	// ��֤����״̬
	ASSERT_EQ(final_result.size(), 4); // key1, key2, key4, key5

	// ��֤��������
	EXPECT_EQ(final_result[0].first, "key1");
	EXPECT_EQ(final_result[0].second, "value1_new");

	EXPECT_EQ(final_result[1].first, "key2");
	EXPECT_EQ(final_result[1].second, "value2_final");

	EXPECT_EQ(final_result[2].first, "key4");
	EXPECT_EQ(final_result[2].second, "value4");

	EXPECT_EQ(final_result[3].first, "key5");
	EXPECT_EQ(final_result[3].second, "value5");

	// ��֤��ɾ����keyȷʵ������
	bool has_key3 = false;
	auto res = memtable.get("key3");
	EXPECT_FALSE(res.has_value());
}

// question
TEST(MemTableTest, ConcurrentOperations)
{
	MemTable memtable;
	const int num_readers = 4;			 // ���߳���
	const int num_writers = 2;			 // д�߳���
	const int num_operations = 1000; // ÿ���̵߳Ĳ�����

	// ����ͬ�������̵߳Ŀ�ʼ
	std::atomic<bool> start{false};
	// ���ڵȴ������߳����
	std::atomic<int> completion_counter{num_readers + num_writers +
																			1}; // +1 for freeze thread

	// ��¼д��ļ���������֤
	std::vector<std::string> inserted_keys;
	std::mutex keys_mutex;

	// д�̺߳���
	auto writer_func = [&](int thread_id)
	{
		while (!start)
		{
			std::this_thread::yield();
		}

		for (int i = 0; i < num_operations; ++i)
		{
			std::string key =
					"key_" + std::to_string(thread_id) + "_" + std::to_string(i);
			std::string value =
					"value_" + std::to_string(thread_id) + "_" + std::to_string(i);

			if (i % 3 == 0)
			{
				// �������
				memtable.put(key, value);
				{
					std::lock_guard<std::mutex> lock(keys_mutex);
					inserted_keys.push_back(key);
				}
			}
			else if (i % 3 == 1)
			{
				// ɾ������
				memtable.remove(key);
			}
			else
			{
				// ���²���
				memtable.put(key, value + "_updated");
			}

			std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));
		}

		completion_counter--;
	};

	// ���̺߳���
	auto reader_func = [&](int thread_id)
	{
		while (!start)
		{
			std::this_thread::yield();
		}

		int found_count = 0;
		for (int i = 0; i < num_operations; ++i)
		{
			// ���ѡ��һ���Ѳ����key���в�ѯ
			std::string key_to_find;
			{
				std::lock_guard<std::mutex> lock(keys_mutex);
				if (!inserted_keys.empty())
				{
					key_to_find = inserted_keys[rand() % inserted_keys.size()];
				}
			}

			if (!key_to_find.empty())
			{
				auto result = memtable.get(key_to_find);
				if (result.has_value())
				{
					found_count++;
				}
			}

			// ÿ��һ��ʱ�����һ�α�������
			if (i % 100 == 0)
			{
				std::vector<std::pair<std::string, std::string>> items;
				for (auto it = memtable.begin(); it != memtable.end(); ++it)
				{
					items.push_back(*it);
				}
			}

			std::this_thread::sleep_for(std::chrono::microseconds(rand() % 50));
		}

		completion_counter--;
	};

	// �����̺߳���
	auto freeze_func = [&]()
	{
		while (!start)
		{
			std::this_thread::yield();
		}

		// ����ִ�ж������
		for (int i = 0; i < 5; ++i)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			memtable.frozen_cur_table();

			// ��֤�����ı�
			size_t frozen_size = memtable.get_frozen_size();
			EXPECT_GE(frozen_size, 0);

			// ��֤�ܴ�С
			size_t total_size = memtable.get_total_size();
			EXPECT_GE(total_size, frozen_size);
		}

		completion_counter--;
	};

	// ����������д�߳�
	std::vector<std::thread> writers;
	for (int i = 0; i < num_writers; ++i)
	{
		writers.emplace_back(writer_func, i);
	}

	// �������������߳�
	std::vector<std::thread> readers;
	for (int i = 0; i < num_readers; ++i)
	{
		readers.emplace_back(reader_func, i);
	}

	// ���������������߳�
	std::thread freeze_thread(freeze_func);

	// ���߳�һ��ʱ�����ȴ�״̬
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// ��¼��ʼʱ��
	auto start_time = std::chrono::high_resolution_clock::now();

	// ���Ϳ�ʼ�ź�
	start = true;

	// �ȴ������߳����
	while (completion_counter > 0)
	{
		std::this_thread::yield();
	}

	// ��¼����ʱ��
	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			end_time - start_time);

	// �ȴ������߳̽���
	for (auto &w : writers)
	{
		w.join();
	}
	for (auto &r : readers)
	{
		r.join();
	}
	freeze_thread.join();

	// ��֤����״̬
	size_t final_size = 0;
	for (auto it = memtable.begin(); it != memtable.end(); ++it)
	{
		final_size++;
	}

	// ���ͳ����Ϣ
	// std::cout << "Concurrent test completed in " << duration.count()
	//           << "ms\nFinal memtable size: " << final_size
	//           << "\nTotal size: " << memtable.get_total_size()
	//           << "\nFrozen size: " << memtable.get_frozen_size() << std::endl;

	// ������ȷ�Լ��
	EXPECT_GT(memtable.get_total_size(), 0);						 // �ܴ�СӦ�ô���0
	EXPECT_LE(final_size, num_writers * num_operations); // ��С��Ӧ����������ֵ
}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}