#pragma once

#include <cstddef>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>

/*
* num_entries metadata数组长度
* MetaEntrys 元数据数组
* Hash 校验完整性
* 
* offset key_len key value_len value
*/
class BlockMeta {
	friend class BlockMetaTest;
public:
	size_t offset; // 块在文件中的偏移量
	std::string first_key;
	std::string last_key;

	static void encode_meta_to_slice(std::vector<BlockMeta>& meta_entries, std::vector<uint8_t>& metadata) {
		// 1 计算总大小
		uint32_t num_entries = meta_entries.size();
		size_t total_size = sizeof(uint32_t);
		for (auto& meta : meta_entries) {
			total_size += sizeof(uint32_t) + meta.first_key.size() + sizeof(uint32_t) + meta.last_key.size();
		}
		total_size += sizeof(uint32_t); // hash

		// 2 分配内存
		metadata.resize(total_size);
		uint8_t* ptr = metadata.data();

		// 3 写入num_entries
		std::memcpy(ptr, &num_entries, sizeof(uint32_t));
		ptr += sizeof(uint32_t);
		// 4 写入每个meta
		for (auto& meta : meta_entries) {
			// 写入offset
			uint32_t offset32 = static_cast<uint32_t>(meta.offset);
			memcpy(ptr, &offset32, sizeof(uint32_t));
			ptr += sizeof(uint32_t);
			// 写入first_key
			uint16_t first_key_len = meta.first_key.size();
			std::memcpy(ptr, &first_key_len, sizeof(uint16_t));
			ptr += sizeof(uint16_t);
			std::memcpy(ptr, meta.first_key.data(), first_key_len);
			ptr += first_key_len;
			// 写入last_key
			uint16_t last_key_len = meta.last_key.size();
			std::memcpy(ptr, &last_key_len, sizeof(uint16_t));
			ptr += sizeof(uint16_t);
			std::memcpy(ptr, meta.last_key.data(), last_key_len);
			ptr += last_key_len;
		}

		// 5 写入hash
		const uint8_t* data_start = metadata.data() + sizeof(uint32_t);
		const uint8_t* data_end = ptr;
		size_t data_len = data_end - data_start;

		uint32_t hash = std::hash<std::string_view>{}(std::string_view(reinterpret_cast<const char*>(data_start), data_len));
		std::memcpy(ptr, &hash, sizeof(uint32_t));
	}

	static std::vector<BlockMeta> decode_meta_from_slice(const std::vector<uint8_t>& metadata) {
		std::vector<BlockMeta> meta_entries;

		// 1 验证长度
		if (metadata.size() < sizeof(uint32_t)) {
			throw std::runtime_error("Metadata size is too small");
		}

		// 2 读取num_entries
		uint32_t num_entries;
		const uint8_t* ptr = metadata.data();
		std::memcpy(&num_entries, ptr, sizeof(uint32_t));
		ptr += sizeof(uint32_t);

		// 3 读取长度
		for (uint32_t i = 0; i < num_entries; ++i) {
			BlockMeta meta;

			uint32_t offset32;
			memcpy(&offset32, ptr, sizeof(uint32_t));
			meta.offset = offset32;
			ptr += sizeof(uint32_t);

			uint16_t first_key_len;
			memcpy(&first_key_len, ptr, sizeof(uint16_t));
			ptr += sizeof(uint16_t);
			meta.first_key.assign(reinterpret_cast<const char*>(ptr), first_key_len);
			ptr += first_key_len;

			uint16_t last_key_len;
			memcpy(&last_key_len, ptr, sizeof(uint16_t));
			ptr += sizeof(uint16_t);
			meta.last_key.assign(reinterpret_cast<const char*>(ptr), last_key_len);
			ptr += last_key_len;

			meta_entries.push_back(meta);
		}

		// 4 验证hash
		uint32_t stored_hash; // 存储的哈希
		memcpy(&stored_hash, ptr, sizeof(uint32_t));

		const uint8_t* data_start = metadata.data() + sizeof(uint32_t);
		const uint8_t* data_end = ptr;
		size_t data_len = data_end - data_start;

		uint32_t computed_hash = std::hash<std::string_view>{}( // 重新计算哈希
			std::string_view(reinterpret_cast<const char*>(data_start), data_len));

		if (stored_hash != computed_hash) {
			throw std::runtime_error("Metadata hash mismatch");
		}

		return meta_entries;
	}

	BlockMeta() :offset(0), first_key(""), last_key("") {};
	BlockMeta(size_t offset, const std::string& first_key, const std::string& last_key)
		: offset(offset), first_key(first_key), last_key(last_key) {}
};