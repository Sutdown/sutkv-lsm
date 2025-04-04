#pragma once

#include "mmap_file.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class FileObj {
private:
	std::unique_ptr<MmapFile> mmap_file;
	size_t m_size;

public:
	FileObj():mmap_file(std::make_unique<MmapFile>()) {}
	~FileObj() = default;

	FileObj(const FileObj&) = delete;
	FileObj& operator=(const FileObj&) = delete;

	FileObj(FileObj&& other) noexcept : mmap_file(std::move(other.mmap_file)), m_size(other.m_size) {
		other.m_size = 0;
	}
	FileObj& operator=(FileObj&& other) noexcept {
		if (this != &other) {
			mmap_file = std::move(other.mmap_file);
			m_size = other.m_size;
			other.m_size = 0;
		}
		return *this;
	}

	size_t size() const { return mmap_file->size(); }

	void set_size(size_t size) { m_size = size; }

	static FileObj create_and_write(const std::string& path, const std::vector<uint8_t>& data);
	static FileObj open(const std::string& path);
	std::vector<uint8_t> read_to_slice(size_t offset, size_t len) const;
};