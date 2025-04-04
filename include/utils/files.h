#pragma once

#include "mmap_file.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class FileObj
{
private:
	std::unique_ptr<MmapFile> m_file;
	size_t m_size;

public:
	FileObj();
	~FileObj();

	// 禁用拷贝
	FileObj(const FileObj &) = delete;
	FileObj &operator=(const FileObj &) = delete;

	// 实现移动语义
	FileObj(FileObj &&other) noexcept;

	FileObj &operator=(FileObj &&other) noexcept;

	// 文件大小
	size_t size() const;

	// 设置文件大小
	void set_size(size_t size);

	// 创建文件对象, 并写入到磁盘
	static FileObj create_and_write(const std::string &path, std::vector<uint8_t> buf);

	// 打开文件对象
	static FileObj open(const std::string &path);

	// 读取并返回切片
	std::vector<uint8_t> read_to_slice(size_t offset, size_t length);
};