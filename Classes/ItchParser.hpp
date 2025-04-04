#pragma once

#include "VWAPCalc.hpp"
#include "../Messages/itch_message_sizes.hpp"

#include <string>

class ItchParser
{
    public:

        ItchParser();

        ~ItchParser(){}

        void parseFile(std::string filePath);


    private:

        bool readMessageHeader(std::ifstream &file, ITCHMessageHeader &header);

        //Processes message type 'A' - Add Order – No MPID Attribution
        void processA(std::ifstream &file);

        void processF(){};

        //Processes message type 'E' - Order ExecutedMessage
        void processE(std::ifstream &file);

        void processC(){};

        void processX(){};

        void processD(){};

        void processU(){};

        void processP(){};

        void processQ(){};

        void processB(){};

        //Order Book - Key: Order Reference, Value: Order Struct
        std::unordered_map<uint64_t, AddOrderMessage> order_book;

        //VWAP Map - Key: Stock Symbol, Value: VWAPCalc object
        std::unordered_map<std::string, VWAPCalc> vwap_map;
};