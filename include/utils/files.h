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
	FileObj();
	~FileObj();

	FileObj(const FileObj&) = delete;
	FileObj& operator=(const FileObj&) = delete;

	FileObj(FileObj&& other) noexcept;
	FileObj& operator=(FileObj&& other) noexcept;

	size_t size() const;
	void set_size(size_t size);

	static FileObj create_and_write(const std::string& path, const std::vector<uint8_t>& data);
	static FileObj open(const std::string& path);
	std::vector<uint8_t> read_to_slice(size_t offset, size_t len) const;
};