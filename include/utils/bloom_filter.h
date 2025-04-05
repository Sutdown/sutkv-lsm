#pragma once

#include <cmath>
#include <functional>
#include <string>
#include <vector>

class BloomFilter{
public:
  BloomFilter();
  /*
  当k=ln2*(m/n)时，错误率f最小，f=(1/2)^k
  m为位数组大小，n为元素个数，k为哈希函数的数量。
  */
  BloomFilter(size_t expected_elements, double false_positive_rate);
  BloomFilter(size_t expected_elements, double false_positive_rate, size_t num_bits);

  void add(const std::string &key);
  bool possibly_contains(const std::string &key) const;

  void clear();

  std::vector<uint8_t> encode();
  static BloomFilter decode(const std::vector<uint8_t> &data);

private:
  size_t expected_elements_;  // 位数组大小
  double false_positive_rate_; // 假阳性率
  size_t num_bits_;
  size_t num_hashes_;
  std::vector<bool> bits_;

private:
  size_t hash1(const std::string &key) const;
  size_t hash2(const std::string &key) const;
  size_t hash(const std::string &key, size_t idx) const;
};