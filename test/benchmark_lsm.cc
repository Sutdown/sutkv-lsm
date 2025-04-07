#include <benchmark/benchmark.h>
#include "../include/lsm/engine.h"
#include <string>
#include <vector>
#include <random>

// 生成随机字符串
std::string generateRandomString(size_t length)
{
  static const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, charset.size() - 1);

  std::string result;
  result.reserve(length);
  for (size_t i = 0; i < length; ++i)
  {
    result += charset[dis(gen)];
  }
  return result;
}

// 插入操作性能测试
static void BM_Insertion(benchmark::State &state)
{
  std::string dataDir = "benchmark_lsm_data";
  LSM lsm(dataDir);

  std::vector<std::string> keys;
  std::vector<std::string> values;
  size_t numOperations = state.range(0);

  // 生成随机键值对
  for (size_t i = 0; i < numOperations; ++i)
  {
    keys.push_back(generateRandomString(10));
    values.push_back(generateRandomString(20));
  }

  for (auto _ : state)
  {
    for (size_t i = 0; i < numOperations; ++i)
    {
      lsm.put(keys[i], values[i]);
    }
  }

  state.SetItemsProcessed(numOperations * state.iterations());
  state.SetBytesProcessed((10 + 20) * numOperations * state.iterations());
}
BENCHMARK(BM_Insertion)->Arg(1000)->Arg(10000)->Arg(100000);

// 查询操作性能测试
static void BM_Query(benchmark::State &state)
{
  std::string dataDir = "benchmark_lsm_data";
  LSM lsm(dataDir);

  std::vector<std::string> keys;
  std::vector<std::string> values;
  size_t numOperations = state.range(0);

  // 生成随机键值对并插入数据
  for (size_t i = 0; i < numOperations; ++i)
  {
    keys.push_back(generateRandomString(10));
    values.push_back(generateRandomString(20));
    lsm.put(keys[i], values[i]);
  }

  for (auto _ : state)
  {
    for (size_t i = 0; i < numOperations; ++i)
    {
      auto value = lsm.get(keys[i]);
      if (!value.has_value())
      {
        state.SkipWithError("Key not found during query test.");
      }
    }
  }

  state.SetItemsProcessed(numOperations * state.iterations());
  state.SetBytesProcessed(10 * numOperations * state.iterations());
}
BENCHMARK(BM_Query)->Arg(1000)->Arg(10000)->Arg(100000);

// 更新操作性能测试
static void BM_Update(benchmark::State &state)
{
  std::string dataDir = "benchmark_lsm_data";
  LSM lsm(dataDir);

  std::vector<std::string> keys;
  std::vector<std::string> values;
  std::vector<std::string> newValues;
  size_t numOperations = state.range(0);

  // 生成随机键值对并插入数据
  for (size_t i = 0; i < numOperations; ++i)
  {
    keys.push_back(generateRandomString(10));
    values.push_back(generateRandomString(20));
    lsm.put(keys[i], values[i]);
  }

  // 生成新的随机值
  for (size_t i = 0; i < numOperations; ++i)
  {
    newValues.push_back(generateRandomString(20));
  }

  for (auto _ : state)
  {
    for (size_t i = 0; i < numOperations; ++i)
    {
      lsm.put(keys[i], newValues[i]);
    }
  }

  state.SetItemsProcessed(numOperations * state.iterations());
  state.SetBytesProcessed((10 + 20) * numOperations * state.iterations());
}
BENCHMARK(BM_Update)->Arg(1000)->Arg(10000)->Arg(100000);

// 删除操作性能测试
static void BM_Deletion(benchmark::State &state)
{
  std::string dataDir = "benchmark_lsm_data";
  LSM lsm(dataDir);

  std::vector<std::string> keys;
  std::vector<std::string> values;
  size_t numOperations = state.range(0);

  // 生成随机键值对并插入数据
  for (size_t i = 0; i < numOperations; ++i)
  {
    keys.push_back(generateRandomString(10));
    values.push_back(generateRandomString(20));
    lsm.put(keys[i], values[i]);
  }

  for (auto _ : state)
  {
    for (size_t i = 0; i < numOperations; ++i)
    {
      lsm.remove(keys[i]);
    }
  }

  state.SetItemsProcessed(numOperations * state.iterations());
  state.SetBytesProcessed(10 * numOperations * state.iterations());
}
BENCHMARK(BM_Deletion)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK_MAIN();