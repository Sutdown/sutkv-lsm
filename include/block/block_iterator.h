#pragma once

#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <stdexcept>

class Block;

class BlockIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = std::pair<std::string, std::string>;
	using difference_type = std::ptrdiff_t;
	using pointer = const value_type*;
	using reference = const value_type&;

	BlockIterator(std::shared_ptr<Block> b, size_t index) :block(b), current_index(index), cached_value(std::nullopt) {};
	BlockIterator() :block(nullptr), current_index(0) {};

	BlockIterator& operator++() {
		// 如果当前索引超出范围，抛出异常
		if (current_index >= block->cur_size()) {
			throw std::out_of_range("BlockIterator out of range");
		}
		++current_index;
		cached_value=std::nullopt;
		return *this;
	}

	BlockIterator operator++(int) {
		BlockIterator tmp = *this;
		++(*this);
		return tmp;
	}

	bool operator==(const BlockIterator& other) const {
		if (block == nullptr && other.block == nullptr) {
			return true;
		}
		if (block == nullptr || other.block == nullptr) {
			return false;
		}
		return block == other.block && current_index == other.current_index;
	}

	bool operator!=(const BlockIterator& other) const {
		return !(*this == other);
	}

	value_type operator*() const {
		if (!block || current_index >= block->cur_size()) {
			throw std::out_of_range("BlockIterator out of range");
		}
		// 使用缓存的值，避免重复计算
		if (!cached_value.has_value()) {
			size_t offset = block->get_offset_at(current_index);
			cached_value = std::make_pair(block->get_key_at(offset), block->get_value_at(offset));
		}
		return *cached_value;
	}

private:
	std::shared_ptr<Block> block; // 指向Block的智能指针
	size_t current_index; // 当前项的索引
	mutable std::optional<value_type> cached_value; // 缓存当前项的值
};