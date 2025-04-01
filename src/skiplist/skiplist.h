#include<iostream>
#include<vector>
#include<memory>
#include<string>
#include<optional>
#include<random>

struct Node {
	std::string key, value;
	std::vector<std::shared_ptr<Node>> forward; // 多层前向指针

	Node(const std::string& key, const std::string& value, int level)
		: key(key), value(value), forward(level, nullptr){}
};

class SkipList {
private:
	std::shared_ptr<Node> head;
	int max_level; // 最大层数	
	int cur_level; // 当前层数
	size_t size_bytes; // 跟踪内存使用

public:
	SkipList(int max_level=16); // 构造函数
	~SkipList();             // 析构函数

	// 增删改查
	void put(const std::string& key, const std::string& value);
	std::string get(const std::string& key);
	void remove(const std::string& key);

	// 获取大小
	size_t get_size_bytes() const { return size_bytes; }
	int random_level();

	// 清理
	void clear(); 

	// 从磁盘加载
	std::vector<std::pair<std::string, std::string>> flush();

};