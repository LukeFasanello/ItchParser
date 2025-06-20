#include <gtest/gtest.h>
#include <fstream>
#include <filesystem> 
#include "../Classes/ItchParser.hpp"

namespace fs = std::filesystem;

TEST(ParserSmoke, LogFileIsNotEmpty)
{
    ItchParser parser;
    parser.parseFile("../tests/sample.itch");   // whatever your API is
    EXPECT_TRUE(fs::exists("Output/sample.log"));
    ASSERT_GT(fs::file_size("Output/sample.log"), 0);

    fs::remove_all("Output");
}

