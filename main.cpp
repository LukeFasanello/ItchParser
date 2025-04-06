#include "Classes/ItchParser.hpp"
#include <iostream>
#include <fstream>

int main() {

    std::string path;
    std::cout << "Enter ITCH file path: ";
    std::getline(std::cin, path);

    ItchParser parser;
    parser.parseFile(path);
    return 0;
}