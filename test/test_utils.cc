#include "../include/utils/files.h"
#include <filesystem>
#include <gtest/gtest.h>
#include <random>



int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}