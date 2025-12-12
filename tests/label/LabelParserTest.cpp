//
// Created by ephir on 13.12.2025.
//

#include "gtest/gtest.h"
#include "label/LabelsParser.h"

namespace fs = std::filesystem;

class LabelsParserTest : public ::testing::Test {
protected:
    fs::path tmp_dir;

    void SetUp() override {
        tmp_dir = fs::temp_directory_path() / fs::path("labels_parser_test");
        fs::create_directories(tmp_dir);
    }

    void TearDown() override {
        fs::remove_all(tmp_dir);
    }

    [[nodiscard]] fs::path file(const std::string& name) const {
        return tmp_dir / name;
    }
};

TEST_F(LabelsParserTest, LoadMissingFileReturnsEmptyMap) {
    const auto result = LabelsParser::load_labels(file("labels.txt").string());

    EXPECT_TRUE(result.empty());
}

TEST_F(LabelsParserTest, LoadValidLabels) {
    std::ofstream out(file("labels.txt"));
    out << "a 0\n";
    out << "b 1\n";
    out << "c 2\n";
    out.close();

    auto result = LabelsParser::load_labels(file("labels.txt").string());

    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result["a"], "0");
    EXPECT_EQ(result["b"], "1");
    EXPECT_EQ(result["c"], "2");
}

TEST_F(LabelsParserTest, LoadDuplicateKeysLastWins) {
    std::ofstream out(file("labels.txt"));
    out << "a 0\n";
    out << "a 1\n";
    out.close();

    auto result = LabelsParser::load_labels(file("labels.txt").string());

    EXPECT_EQ(result["a"], "1");
}

TEST_F(LabelsParserTest, SaveCreatesFile) {
    std::map<std::string, std::string> labels{
            {"a", "10"},
            {"b", "20"}
    };

    auto path = file("labels.txt");
    LabelsParser::save_labels(path.string(), labels);

    EXPECT_TRUE(fs::exists(path));
}

TEST_F(LabelsParserTest, SaveAndLoadRoundTrip) {
    std::map<std::string, std::string> input{
            {"a", "0"},
            {"b", "1"},
            {"c", "2"}
    };

    auto path = file("labels.txt");

    LabelsParser::save_labels(path.string(), input);
    auto output = LabelsParser::load_labels(path.string());

    EXPECT_EQ(input, output);
}
