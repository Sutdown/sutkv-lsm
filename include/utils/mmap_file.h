/*
* 内存映射
* 按需加载
* 数据同步
*/

class MmapFile {
private:
	int fd_; // 文件描述符
	size_t file_size_; // 文件大小
	void* mmaped_data_; // 映射的内存指针
	std::string filename_; // 文件名

	mapFile(const MmapFile&) = delete;
	MmapFile& operator=(const MmapFile&) = delete;

public:
	MmapFile() :fd_(-1), file_size_(0), mmaped_data_(nullptr) {}
	~MmapFile() { close(); }

	bool open(const std::string& filename, bool create = false);
	bool create_and_map(const std::string& path, size_t size);

	void close();
	void* data() const { return mmaped_data_; }
	size_t size() const { return file_size_; }

	bool sync();
	bool write(const void* data, size_t size);
};