//
// Created by ephir on 07.12.2025.
//

#include "label/Label.h"

#include <gtest/gtest.h>
#include <vector>
#include <string>

TEST(GenerateLabelsTest, SmallNumbers) {
    std::vector<std::string> expected = {"a", "b", "c", "d"};
    EXPECT_EQ(generate_labels(4), expected);

    expected = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
    EXPECT_EQ(generate_labels(10), expected);
}

TEST(GenerateLabelsTest, ExactlyAlphabetSize) {
    std::vector<std::string> expected;
    for (char c = 'a'; c <= 'z'; ++c)
        expected.push_back(std::string(1, c));
    EXPECT_EQ(generate_labels(26), expected);
}

TEST(GenerateLabelsTest, MoreThanAlphabetSize) {
    const std::vector<std::string> result = generate_labels(30);
    EXPECT_EQ(result.size(), 30);

    for (size_t i = 0; i < 26; ++i) {
        std::string expected = "aa";
        expected[1] += i;
        EXPECT_EQ(result[i], expected);
    }

    EXPECT_EQ(result[26], "ba");
    EXPECT_EQ(result[27], "bb");
    EXPECT_EQ(result[28], "bc");
    EXPECT_EQ(result[29], "bd");
}

TEST(GenerateLabelsTest, LargeNumber) {
    constexpr size_t number = 1000;
    const auto labels = generate_labels(number);
    EXPECT_EQ(labels.size(), number);

    const size_t length = labels[0].size();
    for (const auto& s : labels)
        EXPECT_EQ(s.size(), length);
}
