#include <gtest/gtest.h>
#include <fstream>
#include "../Classes/ItchParser.hpp"

TEST(ParserSmoke, LogFileIsNotEmpty)
{
    const std::string logPath = "../logging.txt";
    ItchParser parser;
    parser.parseFile("tests/sample.itch");   // whatever your API is

    namespace fs = std::filesystem;
    ASSERT_TRUE(fs::exists(logPath));
    ASSERT_GT(fs::file_size(logPath), 0);
}

