#pragma once

#include <cstddef>
#include <fcntl.h>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

class MmapFile
{
private:
	int fd_;							 // �ļ�������
	void *mapped_data_;		 // ӳ����ڴ��ַ
	size_t file_size_;		 // �ļ���С
	std::string filename_; // �ļ���

public:
	MmapFile() : fd_(-1), mapped_data_(nullptr), file_size_(0) {}
	~MmapFile() { close(); }

	// ���ļ���ӳ�䵽�ڴ�
	bool open(const std::string &filename, bool create = false);

	// �����ļ���ӳ�䵽�ڴ�
	bool create_and_map(const std::string &path, size_t size);

	// �ر��ļ�
	void close();

	// ��ȡӳ����ڴ�ָ��
	void *data() const { return mapped_data_; }

	// ��ȡ�ļ���С
	size_t size() const { return file_size_; }

	// д�����ݣ������Ҫ��
	bool write(const void *data, size_t size);

	// ͬ��������
	bool sync();

private:
	// ��ֹ����
	MmapFile(const MmapFile &) = delete;
	MmapFile &operator=(const MmapFile &) = delete;
};