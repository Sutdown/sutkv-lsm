#include "../../include/utils/files.h"
#include <cstring>
#include <stdexcept>

// 将缓冲区内容写入磁盘
FileObj FileObj::create_and_write(const std::string& path, const std::vector<uint8_t>& buf) {
	FileObj file_obj;

	if (!file_obj.mmap_file->create_and_map(path, buf.size())) {
		throw std::runtime_error("Failed to create and map file: " + path);
	}
	memcpy(file_obj.mmap_file->data(), buf.data(), buf.size());
	file_obj.mmap_file->sync();
	return std::move(file_obj);
}

FileObj FileObj::open(const std::string& path) {
	FileObj file_obj;
	if (!file_obj.mmap_file->open(path)) {
		throw std::runtime_error("Failed to open file: " + path);
	}
	return std::move(file_obj);
}

// 读取文件到内存
std::vector<uint8_t> FileObj::read_to_slice(size_t offset, size_t len) const {
	if (offset + len > mmap_file->size()) {
		throw std::out_of_range("Read out of range");
	}
	std::vector<uint8_t> res(len);
	const uint8_t* data = static_cast<const uint8_t*>(mmap_file->data());
	memcpy(res.data(), data + offset, len);

	return data;
}