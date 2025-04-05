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

        //Format a timestamp from nano seconds
        std::string formatTimestamp(uint64_t nanoseconds);

        //Print the vwap_map
        void printVWAPMap();

        // Process System Event – No MPID Attribution
        bool processS(std::ifstream &file);

        // Process Add Order – No MPID Attribution
        bool processAorF(std::ifstream &file);

        // Process Order ExecutedMessage
        bool processE(std::ifstream &file);

        // Process Order Executed with Price Message
        bool processC(std::ifstream &file);

        // Process Order Cancel Message
        bool processX(std::ifstream &file);

        // Process Order Delete Message
        bool processD(std::ifstream &file);

        // Process Order Replace Message
        bool processU(std::ifstream &file);

        // Process Trade Message (Non-Cross)
        bool processP(std::ifstream &file);

        // Process Cross Trade Message
        bool processQ(std::ifstream &file);

        // Process Broken Trade / Order Execution Message
        bool processB(std::ifstream &file);

        //Update the VWAP for a stock
        void updateVWAP(const char* stock, uint32_t price, uint64_t shares);

        //Order Book - Key: Order Reference, Value: Order Struct
        std::unordered_map<uint64_t, OrderReference> order_book;

        //StockSymbol
        using StockSymbol = std::array<char, 8>;

        //StockSymbol hasher functor
        struct StockSymbolHasher
        {
            //Overloading to make struct callable
            std::size_t operator()(const StockSymbol& symbol) const
            {
                //Create default hasher for std::string_view
                return std::hash<std::string_view>()(
                    //Call it with a string_view we construct from the symbol data
                    std::string_view(symbol.data(), symbol.size())

                );

            }
        };

        //VWAP Map - Key: Stock Symbol, Value: VWAPCalc object
        std::unordered_map<StockSymbol, VWAPCalc, StockSymbolHasher> vwap_map;

        //Executed Trade Book - Key: Match Number, Value: OrderReference struct
        std::unordered_map<uint64_t, OrderReference> executed_trade_book;

        //Current time
        uint64_t curr_time;

        //Log file
        std::ofstream logFile;
};