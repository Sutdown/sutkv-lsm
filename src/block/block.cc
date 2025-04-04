#include "../../include/block/block.h"
#include "../../include/block/block_iterator.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <stdexcept>

std::vector<uint8_t> Block::encode() {
    // �����ܴ�С�����ݶ� + ƫ������(ÿ��ƫ��2�ֽ�) + Ԫ�ظ���(2�ֽ�)
    size_t total_bytes = data.size() * sizeof(uint8_t) + offsets.size() * sizeof(uint16_t) + sizeof(uint16_t);
    std::vector<uint8_t> encoded(total_bytes, 0);

    // 1. �������ݶ�
    memcpy(encoded.data(), data.data(), data.size() * sizeof(uint8_t));

    // 2. ����ƫ������
    size_t offset_pos = data.size() * sizeof(uint8_t);
    memcpy(encoded.data() + offset_pos, offsets.data(), offsets.size() * sizeof(uint16_t));

    // 3. д��Ԫ�ظ���
    size_t num_pos = data.size() * sizeof(uint8_t) + offsets.size() * sizeof(uint16_t);
    uint16_t num_elements = offsets.size();
    memcpy(encoded.data() + num_pos, &num_elements, sizeof(uint16_t));

    return encoded;
}

std::shared_ptr<Block> Block::decode(const std::vector<uint8_t>& encoded) {
    auto block = std::make_shared<Block>();

    // 1. ��ȫ�Լ��
    if (encoded.size() < sizeof(uint16_t)) {
        throw std::runtime_error("Encoded data too small");
    }

    // 2. ��ȡԪ�ظ���
    uint16_t num_elements;
    size_t num_elements_pos = encoded.size() - sizeof(uint16_t);
    memcpy(&num_elements, encoded.data() + num_elements_pos, sizeof(uint16_t));

    // 3. ��֤���ݴ�С
    size_t required_size = sizeof(uint16_t) + num_elements * sizeof(uint16_t);
    if (encoded.size() < required_size) {
        throw std::runtime_error("Invalid encoded data size");
    }

    // 4. �������λ��
    size_t offsets_section_start =
        num_elements_pos - num_elements * sizeof(uint16_t);

    // 5. ��ȡƫ������
    block->offsets.resize(num_elements);
    memcpy(block->offsets.data(), encoded.data() + offsets_section_start,
        num_elements * sizeof(uint16_t));

    // 6. �������ݶ�
    block->data.reserve(offsets_section_start); // �Ż��ڴ����
    block->data.assign(encoded.begin(), encoded.begin() + offsets_section_start);

    return block;
}

std::string Block::get_first_key() {
    if (data.empty() || offsets.empty()) {
        return "";
    }

    // ��ȡ��һ��key�ĳ��ȣ�ǰ2�ֽڣ�
    uint16_t key_len;
    memcpy(&key_len, data.data(), sizeof(uint16_t));

    // ��ȡkey
    std::string key(reinterpret_cast<char*>(data.data() + sizeof(uint16_t)), key_len);
    return key;
}

size_t Block::get_offset_at(size_t idx) const {
    if (idx > offsets.size()) {
        throw std::runtime_error("idx out of offsets range");
    }
    return offsets[idx];
}

bool Block::add_entry(const std::string& key, const std::string& value) {
    if ((cur_size() + key.size() + value.size() + 3 * sizeof(uint16_t) >
         capacity) &&
        !offsets.empty())
    {
        return false;
    }
    // ����entry��С��key����(2B) + key + value����(2B) + value
    size_t entry_size = sizeof(uint16_t) + key.size() + sizeof(uint16_t) + value.size();
    size_t old_size = data.size();
    data.resize(old_size + entry_size);

    // д��key����
    uint16_t key_len = key.size();
    memcpy(data.data() + old_size, &key_len, sizeof(uint16_t));

    // д��key
    memcpy(data.data() + old_size + sizeof(uint16_t), key.data(), key_len);

    // д��value����
    uint16_t value_len = value.size();
    memcpy(data.data() + old_size + sizeof(uint16_t) + key_len, &value_len,  sizeof(uint16_t));

    // д��value
    memcpy(data.data() + old_size + sizeof(uint16_t) + key_len + sizeof(uint16_t), value.data(), value_len);

    // ��¼ƫ��
    offsets.push_back(old_size);

    return true;
}

// ��ָ��ƫ������ȡentry��key
std::string Block::get_key_at(size_t offset) const {
    uint16_t key_len;
    memcpy(&key_len, data.data() + offset, sizeof(uint16_t));
    return std::string(reinterpret_cast<const char*>(data.data() + offset + sizeof(uint16_t)),key_len);
}

// ��ָ��ƫ������ȡentry��value
std::string Block::get_value_at(size_t offset) const {
    // �Ȼ�ȡkey����
    uint16_t key_len;
    memcpy(&key_len, data.data() + offset, sizeof(uint16_t));

    // ����value���ȵ�λ��
    size_t value_len_pos = offset + sizeof(uint16_t) + key_len;
    uint16_t value_len;
    memcpy(&value_len, data.data() + value_len_pos, sizeof(uint16_t));

    // ����value
    return std::string(reinterpret_cast<const char*>(data.data() + value_len_pos + sizeof(uint16_t)),value_len);
}

// �Ƚ�ָ��ƫ��������key��Ŀ��key
int Block::compare_key_at(size_t offset, const std::string& target) const {
    std::string key = get_key_at(offset);
    return key.compare(target);
}

// ʹ�ö��ֲ��һ�ȡvalue
// Ҫ���ڲ�������ʱ�������
std::optional<std::string> Block::get_value_binary(const std::string& key) {
    if (offsets.empty()) {
        return std::nullopt;
    }

    // ���ֲ���
    int left = 0;
    int right = offsets.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        size_t mid_offset = offsets[mid];

        int cmp = compare_key_at(mid_offset, key);

        if (cmp == 0) {
            // �ҵ�key�����ض�Ӧ��value
            return get_value_at(mid_offset);
        }
        else if (cmp < 0) {
            // �м��keyС��Ŀ��key�������Ұ벿��
            left = mid + 1;
        }
        else {
            // �м��key����Ŀ��key��������벿��
            right = mid - 1;
        }
    }

    return std::nullopt;
}

Block::Entry Block::get_entry_at(size_t offset) const {
    Entry entry;
    entry.key = get_key_at(offset);
    entry.value = get_value_at(offset);
    return entry;
}

BlockIterator Block::begin() { 
    return BlockIterator(shared_from_this(), 0); 
}

BlockIterator Block::end() {
    return BlockIterator(shared_from_this(), offsets.size());
}