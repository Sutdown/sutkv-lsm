#include<iostream>
#include<vector>
#include<memory>
#include<string>
#include<optional>
#include<random>

struct Node {
	std::string key, value;
	std::vector<std::shared_ptr<Node>> forward; // ���ǰ��ָ��

	Node(const std::string& key, const std::string& value, int level)
		: key(key), value(value), forward(level, nullptr){}
};

class SkipList {
private:
	std::shared_ptr<Node> head;
	int max_level; // ������	
	int cur_level; // ��ǰ����
	size_t size_bytes; // �����ڴ�ʹ��

public:
	SkipList(int max_level=16); // ���캯��
	~SkipList();             // ��������

	// ��ɾ�Ĳ�
	void put(const std::string& key, const std::string& value);
	std::string get(const std::string& key);
	void remove(const std::string& key);

	// ��ȡ��С
	size_t get_size_bytes() const { return size_bytes; }
	int random_level();

	// ����
	void clear(); 

	// �Ӵ��̼���
	std::vector<std::pair<std::string, std::string>> flush();

};