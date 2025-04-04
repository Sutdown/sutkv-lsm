#pragma once

#include "../../include/iterator/iterator.h"
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <utility>

class Block;

class BlockIterator
{
public:
	// ��׼���������Ͷ���
	using iterator_category = std::forward_iterator_tag;
	using value_type = std::pair<std::string, std::string>;
	using difference_type = std::ptrdiff_t;
	using pointer = const value_type *;
	using reference = const value_type &;

	// ���캯��
	BlockIterator(std::shared_ptr<Block> b, size_t index);
	BlockIterator(std::shared_ptr<Block> b, const std::string &key);
	BlockIterator(std::shared_ptr<Block> b);
	BlockIterator() : block(nullptr), current_index(0) {} // end iterator

	// ����������
	BlockIterator &operator++();
	BlockIterator operator++(int) = delete;
	bool operator==(const BlockIterator &other) const;
	bool operator!=(const BlockIterator &other) const;
	value_type operator*() const;
	bool is_end();

private:
	std::shared_ptr<Block> block;										// ָ�������� Block
	size_t current_index;														// ��ǰλ�õ�����
	mutable std::optional<value_type> cached_value; // ���浱ǰֵ
};