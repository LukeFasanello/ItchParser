#include "Classes/ItchParser.hpp"
#include <iostream>
#include <fstream>
#include <chrono>

int main() {

    std::string path;
    std::cout << "Enter ITCH file path: ";
    std::getline(std::cin, path);

    using clock = std::chrono::steady_clock;

    auto t0 = clock::now();
    ItchParser parser;
    size_t nMessages = parser.parseFile(path);
    auto t1 = clock::now();


    double seconds = std::chrono::duration<double>(t1 - t0).count();
    double msgPerSec = nMessages / seconds;

    std::cout << "Parsed " << nMessages << " messages in "
          << seconds << " s  (" << msgPerSec/1e6 << " M msg/s)\n";

    return 0;
}