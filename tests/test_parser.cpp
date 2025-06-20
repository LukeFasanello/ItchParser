#include <gtest/gtest.h>
#include <fstream>
#include <filesystem> 
#include "../Classes/ItchParser.hpp"

namespace fs = std::filesystem;

TEST(ParserSmoke, LogFileIsNotEmpty)
{
    const std::string logPath = "../logging.txt";
    ItchParser parser;
    parser.parseFile("tests/sample.itch");   // whatever your API is
    
    ASSERT_TRUE(fs::exists(logPath));
    ASSERT_GT(fs::file_size(logPath), 0);
}

