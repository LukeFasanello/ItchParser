#pragma once

#include "VWAPCalc.hpp"
#include "../Messages/MessageStructures.hpp"

#include <string>
#include <fstream>

class ItchParser
{
    public:

        ItchParser();

        ~ItchParser(){}

        void parseFile(std::string filePath);


    private:

        bool readMessageHeader(std::ifstream &file, ITCHMessageHeader &header);

        //Convert a char array to a std::string
        std::string charToString(const char* p_orig, size_t p_length);

        //Print the vwap_map
        void printVWAPMap();

        //Processes message type 'A' - Add Order – No MPID Attribution
        bool processA(std::ifstream &file);

        void processF(){};

        //Processes message type 'E' - Order ExecutedMessage
        bool processE(std::ifstream &file);

        void processC(){};

        void processX(){};

        void processD(){};

        void processU(){};

        void processP(){};

        void processQ(){};

        void processB(){};

        //Order Book - Key: Order Reference, Value: Order Struct
        std::unordered_map<uint64_t, TypeA> order_book;

        //VWAP Map - Key: Stock Symbol, Value: VWAPCalc object
        std::unordered_map<std::string, VWAPCalc> vwap_map;

        //Executed Trade Book - Key: Match Number, Value: OrderReference struct
        std::unordered_map<uint64_t, OrderReference> executed_trade_book;

        //Current time
        uint64_t curr_time;

        //Log file
        std::ofstream logFile;
};