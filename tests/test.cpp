//
// Created by ephir on 07.12.2025.
//
#include <gtest/gtest.h>

TEST(SampleTest, BasicAssertions) {
    EXPECT_EQ(1 + 1, 2);
}


int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
