#include "../../include/utils/mmap_file.h"
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

bool MmapFile::open(const std::string& filename, bool create) {
	filename_ = filename;

	// 打开文件
	int flags = O_RDWR;
	if (create) { flags |= O_CREAT; }

	fd_ = open(filename_.c_str(), flags, 0644);
	if (fd_ = -1) { return false; }

	// 获取文件大小
	struct stat st;
	if (fstat(fd_, &st) == -1) {
		close();
		return false;
	}
	file_size_ = st.st_size;

	if (file_size_ > 0) {
		mmaped_data_ = mmap(nullptr, file_size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
		if (mmaped_data_ == MAP_FAILED) {
			close();
			return false;
		}
	}

	return true;
}

void MmapFile::close() {
	if (mmapped_data_ != nullptr && mmaped_data_ != MAP_FAILED) {
		munmap(mmaped_data_, file_size_);
		mmaped_data_ = nullptr;
	}

	if (fd_ != -1) {
		::close(fd_);
		fd_ = -1;
	}

	file_size_ = 0;
}

bool MmapFile::create_and_map(const std::string& path, size_t size) {
	fd_ = open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (fd_ == -1) {
		return false;
	}

	if (ftruncate(fd_, size) == -1) {
		close();
		return false;
	}
		
	mapped_data_ = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
	if (mapped_data_ == MAP_FAILED) {
		close();
		return false;
	}

	file_size_ = size;
	return true;
}

bool MmapFile::sync() {
	if (mmaped_data_ != nullptr && mmaped_data_ != MAP_FAILED) {
		if (msync(mmaped_data_, file_size_, MS_SYNC) == -1) {
			return false;
		}
	}
	return true;
}

bool MmapFile::write(const void* data, size_t size) {
	// 调整文件大小
	if (ftruncate(fd_, size) == -1) {
		return false;
	}

	// 解除内存映射
	if (mmaped_data_ != nullptr && mmaped_data_ != MAP_FAILED) {
		munmap(mapped_data_, file_size_);
	}

	// 重新映射
	mmaped_data_ = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
	if (mmaped_data_ == MAP_FAILED) {
		mmaped_data_ = nullptr;
		return false;
	}
	file_size_ = size;

	memcpy(mmaped_data_, data, size); // 将数据写入映射内存
	return true;
}

