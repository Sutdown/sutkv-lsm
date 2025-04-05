/*
#include "lsm/engine.h"
#include <iostream>
#include <string>

int main()
{
  // 创建LSM实例，指定数据存储目录
  LSM lsm("example_data");

  // 插入数据
  lsm.put("key1", "value1");
  lsm.put("key2", "value2");
  lsm.put("key3", "value3");

  // 查询数据
  auto value1 = lsm.get("key1");
  if (value1.has_value()) {
    std::cout << "key1: " << value1.value() << std::endl;
  } else {
    std::cout << "key1 not found" << std::endl;
  }

  // 更新数据
  lsm.put("key1", "new_value1");
  auto new_value1 = lsm.get("key1");
  if (new_value1.has_value()) {
    std::cout << "key1: " << new_value1.value() << std::endl;
  } else {
    std::cout << "key1 not found" << std::endl;
  }

  // 删除数据
  lsm.remove("key2");
  auto value2 = lsm.get("key2");
  if (value2.has_value()) {
    std::cout << "key2: " << value2.value() << std::endl;
  } else {
    std::cout << "key2 not found" << std::endl;
  }

  // 遍历所有数据
  std::cout << "All key-value pairs:" << std::endl;
  for (auto it = lsm.begin(); it != lsm.end(); ++it) {
     std::cout << it->first << ": " << it->second << std::endl;
  }

  return 0;
}
*/

#include "lsm/engine.h"
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <random>

// 生成随机字符串
std::string generateRandomString(size_t length)
{
  static const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, charset.length() - 1);

  std::string result;
  result.reserve(length);
  for (size_t i = 0; i < length; ++i)
  {
    result += charset[dis(gen)];
  }
  return result;
}

// 执行单个操作的性能测试
void performSingleOperationTest(const std::string &operationName, size_t numOperations, const std::vector<std::string> &keys, const std::vector<std::string> &values, LSM &lsm, void (LSM::*func)(const std::string &, const std::string &))
{
  auto start = std::chrono::high_resolution_clock::now();
  for (size_t i = 0; i < numOperations; ++i)
  {
    (lsm.*func)(keys[i], values[i]);
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << operationName << ": " << numOperations << " operations in " << duration << " ms. "
            << static_cast<double>(numOperations) / (duration / 1000.0) << " ops/sec" << std::endl;
}

// 执行删除操作的性能测试
void performDeleteOperationTest(const std::string &operationName, size_t numOperations, const std::vector<std::string> &keys, LSM &lsm, void (LSM::*func)(const std::string &))
{
  auto start = std::chrono::high_resolution_clock::now();
  for (size_t i = 0; i < numOperations; ++i)
  {
    (lsm.*func)(keys[i]);
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << operationName << ": " << numOperations << " operations in " << duration << " ms. "
            << static_cast<double>(numOperations) / (duration / 1000.0) << " ops/sec" << std::endl;
}

// 性能测试函数
void performPerformanceTest(LSM &lsm, size_t numOperations)
{
  std::vector<std::string> keys;
  std::vector<std::string> values;

  // 生成随机键值对
  for (size_t i = 0; i < numOperations; ++i)
  {
    keys.push_back(generateRandomString(10));
    values.push_back(generateRandomString(20));
  }

  // 插入操作性能测试
  performSingleOperationTest("Insertion", numOperations, keys, values, lsm, &LSM::put);

  // 查询操作性能测试
  auto start = std::chrono::high_resolution_clock::now();
  for (size_t i = 0; i < numOperations; ++i)
  {
    auto value = lsm.get(keys[i]);
    if (!value.has_value())
    {
      std::cerr << "Error: Key " << keys[i] << " not found during query test." << std::endl;
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << "Query: " << numOperations << " operations in " << duration << " ms. "
            << static_cast<double>(numOperations) / (duration / 1000.0) << " ops/sec" << std::endl;

  // 更新操作性能测试
  std::vector<std::string> newValues;
  for (size_t i = 0; i < numOperations; ++i)
  {
    newValues.push_back(generateRandomString(20));
  }
  performSingleOperationTest("Update", numOperations, keys, newValues, lsm, &LSM::put);

  // 删除操作性能测试
  performDeleteOperationTest("Deletion", numOperations, keys, lsm, &LSM::remove);
}

int main()
{
  // 创建LSM实例，指定数据存储目录
  LSM lsm("example_data");

  // 性能测试操作次数
  size_t numOperations = 10000;

  performPerformanceTest(lsm, numOperations);

  return 0;
}
