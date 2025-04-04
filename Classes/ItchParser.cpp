
#include "ItchParser.hpp"

#include <iostream>
#include <vector>
#include <cstdint>
#include <arpa/inet.h>

//-------------------------------------------------------------------------------------------------
ItchParser::ItchParser() :
    order_book{},
    vwap_map{},
    executed_trade_book{}
{
    logFile.open("logging.txt");

    if (!logFile)
    {
        std::cerr << "Log File not opened" << std::endl;
    }
    else
    {
        logFile << "Log File Opened" << std::endl;
    }

}

//-------------------------------------------------------------------------------------------------
std::string ItchParser::charToString(const char* p_orig, size_t p_length)
{
    std::string ret(p_orig, p_length);

    //Find last character that is not a space, remove everything past it
    ret.erase(ret.find_last_not_of(' ')+1);
    return ret;

}

//-------------------------------------------------------------------------------------------------
void ItchParser::printVWAPMap()
{

    for (auto it = vwap_map.begin(); it != vwap_map.end(); ++it)
    {
        std::cout << it->first << ": " << it->second.getVWAP() << std::endl;
    }
}

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

                //Print every half hour
                if (curr_time + 1800000000000 <= msg_header.time_stamp)
                {
                    printVWAPMap();
                    curr_time = msg_header.time_stamp;
                }

                bool loop_break = false;
                switch (msg_header.msgType)
                {

                    case 'S':
                    {
                        logFile << "System event!" << std::endl;
                        itch_file.ignore(msg_header.msg_size - static_cast<u_int8_t>(11));
                        break;
                    }

                    case 'A':
                    {
                        if (!processA(itch_file))
                        {
                            loop_break = true;
                        }

                        break;
                    }

                    case 'E':
                    {
                        if (!processE(itch_file))
                        {
                            loop_break = true;
                        }
                        break;
                    }

                    default:
                    {
                        //TODO: Uncomment here once we have processing for messages we want. This will allow us to ignore messages we don't care about
                        itch_file.ignore(msg_header.msg_size - static_cast<u_int8_t>(11));
                        break;
                    }

                }

                //We were unable to read a message
                if (loop_break)
                {
                    break;
                }
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

    //Converting to little endian using network to host short
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
bool ItchParser::processA(std::ifstream &file)
{
    logFile << "Add Order!" << std::endl;

    //We've read the message header, we now need to read the rest of the contents of the message
    TypeA msg;
    if (!file.read(reinterpret_cast<char*>(&msg), 25))
    {
        std::cout << "Could not read type A message" << std::endl;
        return false;
    }

    //Convert to little endian
    msg.order_ref = __builtin_bswap64(msg.order_ref);
    msg.shares = ntohl(msg.shares);
    msg.price = ntohl(msg.price);

    //TODO: Potential optimization we could make here by not storing the entire type A message
    order_book.emplace(msg.order_ref, msg);
    logFile << msg.order_ref << std::endl;

    return true;

}

//TODO: Implement
//-------------------------------------------------------------------------------------------------
bool ItchParser::processE(std::ifstream &file)
{
    logFile << "Execute Order!" << std::endl;

    TypeE msg;
    if (!file.read(reinterpret_cast<char*>(&msg), 20))
    {
        std::cout << "Could not read type E message" << std::endl;
        return false;
    }

    //Convert to little endian
    msg.order_ref = __builtin_bswap64(msg.order_ref);
    msg.shares = ntohl(msg.shares);
    msg.match_num = __builtin_bswap64(msg.match_num);

    //TODO: Modify Order book
    auto it = order_book.find(msg.order_ref);
    if (it != order_book.end())
    {

        //Modify VWAP for stock
        std::string stock_symbol = charToString(it->second.stock, sizeof(it->second.stock));
        auto vwap_it = vwap_map.find(stock_symbol);
        if (vwap_it != vwap_map.end())
        {
            //Take the price from the order book, shares from the execute order message
            vwap_it->second.updateVWAP(it->second.price, msg.shares);
        }

        else
        {
            vwap_map.emplace(stock_symbol, VWAPCalc(it->second.price, msg.shares));
        }

        //TODO: Add executed order to executed order book
        executed_trade_book.emplace(msg.match_num, OrderReference{msg.shares, it->second.price, stock_symbol});


        //Subtract the shares from the order executed message from the shares in the order
        it->second.shares -= msg.shares;

        if (it->second.shares == 0)
        {
            order_book.erase(it);
        }
    }
    else
    {
        logFile << "Message with Order Reference " << msg.order_ref <<
        " not found in order book" << std::endl;
    }


    return true;
}