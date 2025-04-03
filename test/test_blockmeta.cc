#include "../include/block/blockmeta.h"
#include <gtest/gtest.h>

class  BlockMetaTest : public ::testing::Test {
    friend class BlockMeta;
protected:
    // ����һ������õ� BlockMeta��ȷ�� key ����
    std::vector<BlockMeta> createTestMetas() {
        std::vector<BlockMeta> metas;

        // Block 1: offset=0, "a100" -> "a199"
        BlockMeta meta1;
        meta1.offset = 0;
        meta1.first_key = "a100";
        meta1.last_key = "a199";
        metas.push_back(meta1);

        // Block 2: offset=100, "a200" -> "a299"
        BlockMeta meta2;
        meta2.offset = 100;
        meta2.first_key = "a200";
        meta2.last_key = "a299";
        metas.push_back(meta2);

        // Block 3: offset=200, "a300" -> "a399"
        BlockMeta meta3;
        meta3.offset = 200;
        meta3.first_key = "a300";
        meta3.last_key = "a399";
        metas.push_back(meta3);

        return metas;
    }
};

// ���Ի����ı���ͽ��빦��
TEST_F(BlockMetaTest, BasicEncodeDecodeTest) {
    // 1. ������������
    auto original_metas = createTestMetas();
    std::vector<uint8_t> encoded_data;

    // 2. ����
    BlockMeta::encode_meta_to_slice(original_metas, encoded_data);
    EXPECT_FALSE(encoded_data.empty());

    // 3. ����
    auto decoded_metas = BlockMeta::decode_meta_from_slice(encoded_data);

    // 4. ��֤
    ASSERT_EQ(original_metas.size(), decoded_metas.size());
    for (size_t i = 0; i < original_metas.size(); i++) {
        EXPECT_EQ(original_metas[i].offset, decoded_metas[i].offset);
        EXPECT_EQ(original_metas[i].first_key, decoded_metas[i].first_key);
        EXPECT_EQ(original_metas[i].last_key, decoded_metas[i].last_key);
    }
}

// ���Կ����ݵĴ���
TEST_F(BlockMetaTest, EmptyMetaTest) {
    std::vector<BlockMeta> empty_metas;
    std::vector<uint8_t> encoded_data;

    // ���������
    BlockMeta::encode_meta_to_slice(empty_metas, encoded_data);
    EXPECT_FALSE(encoded_data.empty()); // ����Ӧ�ð���Ԫ�ظ����͹�ϣֵ

    // ���������
    auto decoded_metas = BlockMeta::decode_meta_from_slice(encoded_data);
    EXPECT_TRUE(decoded_metas.empty());
}

// ���������ַ��Ĵ���
TEST_F(BlockMetaTest, SpecialCharTest) {
    std::vector<BlockMeta> metas;
    BlockMeta meta;
    meta.offset = 0;
    meta.first_key = std::string("key\0with\0null", 12);
    meta.last_key = std::string("value\0with\0null", 14);
    metas.push_back(meta);

    std::vector<uint8_t> encoded_data;
    BlockMeta::encode_meta_to_slice(metas, encoded_data);

    auto decoded_metas = BlockMeta::decode_meta_from_slice(encoded_data);
    ASSERT_EQ(decoded_metas.size(), 1);
    EXPECT_EQ(decoded_metas[0].first_key, std::string("key\0with\0null", 12));
    EXPECT_EQ(decoded_metas[0].last_key, std::string("value\0with\0null", 14));
}

// ���Դ�����
TEST_F(BlockMetaTest, ErrorHandlingTest) {
    // ���Խ�����Ч����
    std::vector<uint8_t> invalid_data = { 1, 2, 3 }; // ̫��
    EXPECT_THROW(BlockMeta::decode_meta_from_slice(invalid_data),
        std::runtime_error);

    // ���Կ�vector
    std::vector<uint8_t> empty_data;
    EXPECT_THROW(BlockMeta::decode_meta_from_slice(empty_data),
        std::runtime_error);

    // �����𻵵����ݣ��޸Ĺ�ϣֵ��
    auto metas = createTestMetas();
    std::vector<uint8_t> encoded_data;
    BlockMeta::encode_meta_to_slice(metas, encoded_data);
    encoded_data.back() ^= 1; // �޸����һ���ֽڣ���ϣֵ��һ���֣�
    EXPECT_THROW(BlockMeta::decode_meta_from_slice(encoded_data),
        std::runtime_error);
}

// �޸Ĵ����������ԣ�ȷ�� key ����
TEST_F(BlockMetaTest, LargeDataTest) {
    std::vector<BlockMeta> large_metas;
    const int n = 1000;

    // ����������������
    for (int i = 0; i < n; i++) {
        BlockMeta meta;
        meta.offset = i * 100;

        // ʹ�ø�ʽ��ȷ�� key ����
        char first_key[16], last_key[16];
        snprintf(first_key, sizeof(first_key), "key%03d00", i); // ����: key00000
        snprintf(last_key, sizeof(last_key), "key%03d99", i);   // ����: key00099

        meta.first_key = first_key;
        meta.last_key = last_key;
        large_metas.push_back(meta);
    }

    // ����ͽ���
    std::vector<uint8_t> encoded_data;
    BlockMeta::encode_meta_to_slice(large_metas, encoded_data);
    auto decoded_metas = BlockMeta::decode_meta_from_slice(encoded_data);

    // ��֤
    ASSERT_EQ(large_metas.size(), decoded_metas.size());
    for (int i = 0; i < n; i++) {
        EXPECT_EQ(large_metas[i].offset, decoded_metas[i].offset);
        EXPECT_EQ(large_metas[i].first_key, decoded_metas[i].first_key);
        EXPECT_EQ(large_metas[i].last_key, decoded_metas[i].last_key);

        // ��֤���� block ֮���˳��
        if (i > 0) {
            EXPECT_LT(decoded_metas[i - 1].last_key, decoded_metas[i].first_key);
        }
    }
}

// ���˳���Բ���
TEST_F(BlockMetaTest, OrderTest) {
    auto metas = createTestMetas();

    // ��֤ÿ�� block �ڵ�˳��
    for (const auto& meta : metas) {
        EXPECT_LT(meta.first_key, meta.last_key);
    }

    // ��֤���� block ֮���˳��
    for (size_t i = 1; i < metas.size(); i++) {
        EXPECT_LT(metas[i - 1].last_key, metas[i].first_key);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}