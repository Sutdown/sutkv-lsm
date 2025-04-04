#pragma once
#include "../../include/iterator/iterator.h"
#include "../block/block_iterator.h"
#include <cstddef>
#include <memory>

class SST;

class SstIterator {
  friend SST;

private:
  std::shared_ptr<SST> m_sst;
  size_t m_block_idx;
  std::shared_ptr<BlockIterator> m_block_it;

public:
  using value_type = std::pair<std::string, std::string>;

  // 创建迭代器, 并移动到第一个key
  SstIterator(std::shared_ptr<SST> sst) : m_sst(sst), m_block_idx(0), m_block_it(nullptr) {
    if (m_sst) {
      seek_first();
    }
  }

  // 创建迭代器, 并移动到第指定key
  SstIterator(std::shared_ptr<SST> sst, const std::string &key) : m_sst(sst), m_block_idx(0), m_block_it(nullptr) {
    if (m_sst) {
      seek(key);
    }
  }

  void seek_first(){
    if (!m_sst || m_sst->num_blocks() == 0) {
      m_block_it = nullptr;
      return;
    }

    m_block_idx = 0;
    auto block = m_sst->read_block(m_block_idx);
    m_block_it = std::make_shared<BlockIterator>(block);
  }

  void seek(const std::string &key){
    if (!m_sst) {
      m_block_it = nullptr;
      return;
    }

    try {
      m_block_idx = m_sst->find_block_idx(key);
      auto block = m_sst->read_block(m_block_idx);
      if (!block) {
        m_block_it = nullptr;
        return;
      }
      m_block_it = std::make_shared<BlockIterator>(block, key);
    } catch (const std::exception &) {
      m_block_it = nullptr;
      throw std::runtime_error("could not read a block from m_sst");
    }
  }

  bool is_end() { return !m_block_it; }

  std::string key(){
    if (!m_block_it) {
      throw std::runtime_error("Iterator is invalid");
    }
    return (**m_block_it).first;
  }
  
  std::string value(){
    if (!m_block_it) {
      throw std::runtime_error("Iterator is invalid");
    }
    return (**m_block_it).second;
  }

  SstIterator &operator++(){
    if (!m_block_it) { // 添加空指针检查
      return *this;
    }
    ++(*m_block_it);
    if (m_block_it->is_end()) {
      m_block_idx++;
      if (m_block_idx < m_sst->num_blocks()) {
        // 读取下一个block
        auto next_block = m_sst->read_block(m_block_idx);
        BlockIterator new_blk_it(next_block, 0);
        (*m_block_it) = new_blk_it;
      } else {
        // 没有下一个block
        m_block_it = nullptr;
      }
    }
    return *this;
  }

  SstIterator operator++(int) = delete;

  bool operator==(const SstIterator &other) const{
    return m_sst == other.m_sst && m_block_idx == other.m_block_idx &&
           *m_block_it == *other.m_block_it;
  }

  bool operator!=(const SstIterator &other) const{
    return !(*this == other);
  }

  value_type operator*() const{
    if (!m_block_it) {
      throw std::runtime_error("Iterator is invalid");
    }
    return (**m_block_it);
  }
};