#include "Classes/ItchParser.hpp"
#include <iostream>
#include <fstream>

int main() {

    std::string path;
    std::cout << "Enter ITCH file path: ";
    std::getline(std::cin, path);

    //TODO: Allow user to enter the path of the file they want to parse
    ItchParser parser;
    parser.parseFile(path);
    return 0;
}