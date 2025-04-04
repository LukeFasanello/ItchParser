#include "Classes/ItchParser.hpp"
#include <iostream>
#include <fstream>

int main() {

    //TODO: Allow user to enter the path of the file they want to parse
    ItchParser parser;
    parser.parseFile("/Users/lukefasanello/Desktop/ItchParserProject/ItchParser/Data/01302019.NASDAQ_ITCH50");

    return 0;
}