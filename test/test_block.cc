#include "../include/block/block.h"
#include "../include/block/block_iterator.h"
#include <gtest/gtest.h>

class BlockTest : public ::testing::Test {
protected:
    // Ԥ����ı�������
    std::vector<uint8_t> getEncodedBlock() {
        /*
        Block layout (3 entries):
        Entry1: key="apple", value="red"
        Entry2: key="banana", value="yellow"
        Entry3: key="orange", value="orange"
        */
        std::vector<uint8_t> encoded = {
            // Data Section
            // Entry 1: "apple" -> "red"
            5, 0,                    // key_len = 5
            'a', 'p', 'p', 'l', 'e', // key
            3, 0,                    // value_len = 3
            'r', 'e', 'd',           // value

            // Entry 2: "banana" -> "yellow"
            6, 0,                         // key_len = 6
            'b', 'a', 'n', 'a', 'n', 'a', // key
            6, 0,                         // value_len = 6
            'y', 'e', 'l', 'l', 'o', 'w', // value

            // Entry 3: "orange" -> "orange"
            6, 0,                         // key_len = 6
            'o', 'r', 'a', 'n', 'g', 'e', // key
            6, 0,                         // value_len = 6
            'o', 'r', 'a', 'n', 'g', 'e', // value

            // Offset Section (ÿ��entry����ʼλ��)
            0, 0,  // offset[0] = 0
            12, 0, // offset[1] = 12 (�ڶ���entry����ʼλ��)
            28, 0, // offset[2] = 24 (������entry����ʼλ��)

            // Num of elements
            3, 0 // num_elements = 3
        };
        return encoded;
    }
};

// ���Խ���
TEST_F(BlockTest, DecodeTest) {
    auto encoded = getEncodedBlock();
    auto block = Block::decode(encoded);

    // ��֤��һ��key
    EXPECT_EQ(block->get_first_key(), "apple");

    // ��֤����key-value��
    EXPECT_EQ(block->get_value_binary("apple").value(), "red");
    EXPECT_EQ(block->get_value_binary("banana").value(), "yellow");
    EXPECT_EQ(block->get_value_binary("orange").value(), "orange");
}

// ���Ա���
TEST_F(BlockTest, EncodeTest) {
    Block block;
    block.add_entry("apple", "red");
    block.add_entry("banana", "yellow");
    block.add_entry("orange", "orange");

    auto encoded = block.encode();

    // ���벢��֤
    auto decoded = Block::decode(encoded);
    EXPECT_EQ(decoded->get_value_binary("apple").value(), "red");
    EXPECT_EQ(decoded->get_value_binary("banana").value(), "yellow");
    EXPECT_EQ(decoded->get_value_binary("orange").value(), "orange");
}

// ���Զ��ֲ���
TEST_F(BlockTest, BinarySearchTest) {
    Block block;
    block.add_entry("apple", "red");
    block.add_entry("banana", "yellow");
    block.add_entry("orange", "orange");

    // ���Դ��ڵ�key
    EXPECT_EQ(block.get_value_binary("apple").value(), "red");
    EXPECT_EQ(block.get_value_binary("banana").value(), "yellow");
    EXPECT_EQ(block.get_value_binary("orange").value(), "orange");

    // ���Բ����ڵ�key
    EXPECT_FALSE(block.get_value_binary("grape").has_value());
    EXPECT_FALSE(block.get_value_binary("").has_value());
}

// ���Ա߽����
TEST_F(BlockTest, EdgeCasesTest) {
    Block block;

    // ��block
    EXPECT_EQ(block.get_first_key(), "");
    EXPECT_FALSE(block.get_value_binary("any").has_value());

    // ��ӿ�key��value
    block.add_entry("", "");
    EXPECT_EQ(block.get_first_key(), "");
    EXPECT_EQ(block.get_value_binary("").value(), "");

    // ��Ӱ��������ַ���key��value
    block.add_entry("key\0with\tnull", "value\rwith\nnull");
    std::string special_key("key\0with\tnull");
    std::string special_value("value\rwith\nnull");
    EXPECT_EQ(block.get_value_binary(special_key).value(), special_value);
}

// ���Դ�������
TEST_F(BlockTest, LargeDataTest) {
    Block block;
    const int n = 1000;

    // ��Ӵ�������
    for (int i = 0; i < n; i++) {
        // ʹ�� std::format �� sprintf ���в���
        char key_buf[16];
        snprintf(key_buf, sizeof(key_buf), "key%03d", i); // ���㵽3λ
        std::string key = key_buf;

        char value_buf[16];
        snprintf(value_buf, sizeof(value_buf), "value%03d", i);
        std::string value = value_buf;

        block.add_entry(key, value);
    }

    // ��֤��������
    for (int i = 0; i < n; i++) {
        char key_buf[16];
        snprintf(key_buf, sizeof(key_buf), "key%03d", i);
        std::string key = key_buf;

        char value_buf[16];
        snprintf(value_buf, sizeof(value_buf), "value%03d", i);
        std::string expected_value = value_buf;

        EXPECT_EQ(block.get_value_binary(key).value(), expected_value);
    }
}

// ���Դ�����
TEST_F(BlockTest, ErrorHandlingTest) {
    // ���Խ�����Ч����
    std::vector<uint8_t> invalid_data = { 1, 2, 3 }; // ̫��
    EXPECT_THROW(Block::decode(invalid_data), std::runtime_error);

    // ���Կ�vector
    std::vector<uint8_t> empty_data;
    EXPECT_THROW(Block::decode(empty_data), std::runtime_error);
}

// ���Ե�����
TEST_F(BlockTest, IteratorTest) {
    // ʹ�� make_shared ���� Block
    auto block = std::make_shared<Block>();

    // 1. ���Կ�block�ĵ�����
    EXPECT_EQ(block->begin(), block->end());

    // 2. �����������
    const int n = 100;
    std::vector<std::pair<std::string, std::string>> test_data;

    for (int i = 0; i < n; i++) {
        char key_buf[16], value_buf[16];
        snprintf(key_buf, sizeof(key_buf), "key%03d", i);
        snprintf(value_buf, sizeof(value_buf), "value%03d", i);

        block->add_entry(key_buf, value_buf);
        test_data.emplace_back(key_buf, value_buf);
    }

    // 3. �������������������ȷ��
    size_t count = 0;
    for (const auto& [key, value] : *block) { // ע������ʹ�� *block
        EXPECT_EQ(key, test_data[count].first);
        EXPECT_EQ(value, test_data[count].second);
        count++;
    }
    EXPECT_EQ(count, test_data.size());

    // 4. ���Ե������ıȽϺ��ƶ�
    auto it = block->begin();
    EXPECT_EQ((*it).first, "key000");
    ++it;
    EXPECT_EQ((*it).first, "key001");
    it++;
    EXPECT_EQ((*it).first, "key002");

    // 5. ���Ա����ĵ���
    auto encoded = block->encode();
    auto decoded_block = Block::decode(encoded);
    count = 0;
    for (const auto& [key, value] : *decoded_block) {
        EXPECT_EQ(key, test_data[count].first);
        EXPECT_EQ(value, test_data[count].second);
        count++;
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}