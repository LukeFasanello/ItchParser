
#include "ItchParser.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <arpa/inet.h>  // For byte order conversions

//-------------------------------------------------------------------------------------------------
ItchParser::ItchParser() :
    order_book{},
    vwap_map{}
{}

//-------------------------------------------------------------------------------------------------
void ItchParser::parseFile(std::string filePath)
{

    std::ifstream itch_file(filePath, std::ios::binary);

    if (!itch_file)
    {
        std::cout << "Itch File not opened" << std::endl;
        return;
    }

    std::cout << "Itch file opened successfully" << std::endl;

    while(itch_file)
    {
        ITCHMessageHeader msg_header;
        if (readMessageHeader(itch_file, msg_header))
        {
            if (MESSAGE_TYPES.find(msg_header.msgType) != MESSAGE_TYPES.end())
            {
                switch (msg_header.msgType)
                {

                    case 'S':
                    {
                        std::cout << "System event!" << std::endl;
                    }

                    case 'A':
                    {
                        processA(itch_file);
                    }

                    case 'E':
                    {
                        processE(itch_file);
                    }

                    default:
                    {
                        //TODO: Uncomment here once we have processing for messages we want. This will allow us to ignore messages we don't care about
                        //itch_file.ignore(msg_header.msg_size - static_cast<u_int8_t>(11));
                    }

                }
    
                //Won't need this once we process for each message
                itch_file.ignore(msg_header.msg_size - static_cast<u_int8_t>(11));
            }
    
            else
            {
                std::cout << "Unknown Message Encountered" << std::endl;
                break;
            }
        }
        else
        {
            std::cout << "Unable to read message header.";
        }
    }

    std::cout << "Closing Itch File" << std::endl;

}

//-------------------------------------------------------------------------------------------------
bool ItchParser::readMessageHeader(std::ifstream &file, ITCHMessageHeader &header)
{
    if (!file.read(reinterpret_cast<char*>(&header), 7))
    {
        std::cout << "Could not read msgType, locate, and tracking_num" << std::endl;
        return false;
    }

    //Converting to little endian
    header.locate = ntohs(header.locate);
    header.tracking_num = ntohs(header.tracking_num);

    uint64_t ts_raw = 0;
    if (!file.read(reinterpret_cast<char*>(&ts_raw) + 2, 6))
    {

        std::cout << "Could not read timestamp" << std::endl;
        return false;

    }
    header.time_stamp = __builtin_bswap64(ts_raw);

    return true;
}

//TODO: Implement
//-------------------------------------------------------------------------------------------------
void ItchParser::processA(std::ifstream &file)
{
    std::cout << "Add Order!" << std::endl;

}

//TODO: Implement
//-------------------------------------------------------------------------------------------------
void ItchParser::processE(std::ifstream &file)
{
    std::cout << "Execute Order!" << std::endl;
}