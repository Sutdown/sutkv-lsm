#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class BlockIterator;

/*
* data 存放数据（key,value)
* offset 存放所有项的偏移量，用于快速定位
* Extra Information 存放项的数量
*/

// public std::enable_shared_from_this<Block> 允许获取对象的智能指针
class Block : public::std::enable_shared_from_this<Block> {
	friend class BlockIterator;

private:
	std::vector<uint8_t> data;		// 存放数据
	std::vector<uint16_t> offsets;  // 存放偏移量
	size_t capacity;

	struct Entry {
		std::string key;
		std::string value;
	};
	Entry get_entry_at(size_t offset) const;
	std::string get_key_at(size_t offset) const;
	std::string get_value_at(size_t offset) const;
	int compare_key_at(size_t offset, const std::string& target) const;

public:
	Block() = default;
	Block(size_t capacity) : capacity(capacity) {}

	// 编码解码
	std::vector<uint8_t> encode();
	static std::shared_ptr<Block> decode(const std::vector<uint8_t>& encoded);
	static std::shared_ptr<Block> decode(const std::vector<uint8_t> &encoded,
																			 bool with_hash = false);
	
	// 获取数据
	std::string get_first_key();
	size_t get_offset_at(size_t idx) const;

	// 插入数据，查找数据
	bool add_entry(const std::string& key, const std::string& value);
	std::optional<std::string> get_value_binary(const std::string& key);
	
	size_t cur_size() const {
		return data.size() + offsets.size() * sizeof(uint16_t) + sizeof(uint16_t);
	}
	bool is_empty() const { return offsets.empty(); }

	BlockIterator begin();
	BlockIterator end();
};