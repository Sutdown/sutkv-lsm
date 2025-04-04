#include "../../include/utils/mmap_file.h"
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

bool MmapFile::open(const std::string &filename, bool create)
{
	filename_ = filename;

	// �򿪻򴴽��ļ�
	int flags = O_RDWR;
	if (create)
	{
		flags |= O_CREAT;
	}

	fd_ = ::open(filename.c_str(), flags, 0644);
	if (fd_ == -1)
	{
		return false;
	}

	// ��ȡ�ļ���С
	struct stat st;
	if (fstat(fd_, &st) == -1)
	{
		close();
		return false;
	}
	file_size_ = st.st_size;

	// ӳ���ļ�
	if (file_size_ > 0)
	{
		mapped_data_ =
				mmap(nullptr, file_size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
		if (mapped_data_ == MAP_FAILED)
		{
			close();
			return false;
		}
	}

	return true;
}

void MmapFile::close()
{
	if (mapped_data_ != nullptr && mapped_data_ != MAP_FAILED)
	{
		munmap(mapped_data_, file_size_);
		mapped_data_ = nullptr;
	}

	if (fd_ != -1)
	{
		::close(fd_);
		fd_ = -1;
	}

	file_size_ = 0;
}

bool MmapFile::write(const void *data, size_t size)
{
	// �����ļ���С
	if (ftruncate(fd_, size) == -1)
	{
		return false;
	}

	// ����Ѿ�ӳ�䣬�Ƚ��ӳ��
	if (mapped_data_ != nullptr && mapped_data_ != MAP_FAILED)
	{
		munmap(mapped_data_, file_size_);
	}

	// ����ӳ��
	file_size_ = size;
	mapped_data_ =
			mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
	if (mapped_data_ == MAP_FAILED)
	{
		mapped_data_ = nullptr;
		return false;
	}

	// д������
	memcpy(mapped_data_, data, size);
	return true;
}

bool MmapFile::sync()
{
	if (mapped_data_ != nullptr && mapped_data_ != MAP_FAILED)
	{
		return msync(mapped_data_, file_size_, MS_SYNC) == 0;
	}
	return true;
}

bool MmapFile::create_and_map(const std::string &path, size_t size)
{
	// ���������ļ�
	fd_ = ::open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (fd_ == -1)
	{
		return false;
	}

	// �ȵ����ļ���С
	if (ftruncate(fd_, size) == -1)
	{
		close();
		return false;
	}

	// ӳ�����ļ���С��ͬ�Ŀռ�
	mapped_data_ = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
	if (mapped_data_ == MAP_FAILED)
	{
		close();
		return false;
	}

	file_size_ = size;
	return true;
}