
#include "ItchParser.hpp"

#include <iostream>
#include <vector>
#include <cstdint>
#include <arpa/inet.h>
#include <iomanip>
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;

//-------------------------------------------------------------------------------------------------
ItchParser::ItchParser() :
    order_book{},
    vwap_map{},
    executed_trade_book{},
    curr_time{}
{

    order_book.reserve(1'000'000);
    vwap_map.reserve(10'000);
    executed_trade_book.reserve(1'000'000);

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
std::string ItchParser::formatTimestamp(uint64_t nanoseconds)
{
    uint64_t total_seconds = nanoseconds / 1'000'000'000;
    uint64_t hours   = total_seconds / 3600;
    uint64_t minutes = (total_seconds % 3600) / 60;
    uint64_t seconds = total_seconds % 60;

    uint64_t millis = (nanoseconds / 1'000'000) % 1000;
    uint64_t micros = (nanoseconds / 1'000) % 1000;

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << hours << ":"
        << std::setw(2) << minutes << ":"
        << std::setw(2) << seconds << "."
        << std::setw(3) << millis
        << std::setw(3) << micros;

    return oss.str();

}

//-------------------------------------------------------------------------------------------------
void ItchParser::printVWAPMap()
{
    std::cout << "Logging VWAPS at Time:" << formatTimestamp(curr_time) << '\n';
    logFile << "VWAPs at Time: " << formatTimestamp(curr_time) << '\n';

    for (auto it = vwap_map.begin(); it != vwap_map.end(); ++it)
    {
        logFile << charToString(it->first.data(), 8) << ": " << it->second.getVWAP() << '\n';
    }
}

//-------------------------------------------------------------------------------------------------
void ItchParser::updateVWAP(const char* stock, uint32_t price, uint64_t shares)
{
    StockSymbol stock_symbol;
    std::memcpy(stock_symbol.data(), stock, 8);

    auto it = vwap_map.find(stock_symbol);
    if (it != vwap_map.end())
    {
        it->second.updateVWAP(price, shares);
    }
    else
    {
        vwap_map.try_emplace(stock_symbol, price, shares);
    }
}

//-------------------------------------------------------------------------------------------------
size_t ItchParser::parseFile(std::string filePath)
{ 

    /* ---------- build Output/<input-stem>.log ---------- */
    fs::path in  = filePath;                      // "/path/to/01302019.itch"
    fs::path outDir = "Output";                    // relative; can be absolute
    fs::create_directories(outDir);                // safe: does nothing if exists

    fs::path logPath = outDir / (in.stem().string() + ".log");
    //  -> "Output/01302019.log"

    logFile.open(logPath, std::ios::out);          // replace "logging.txt"
    if (!logFile) {
        std::cerr << "Could not open " << logPath << '\n';
        return 0;
    }
    logFile << "Log file opened for " << in.filename() << '\n';
    logFile.flush();

    std::ifstream itch_file(filePath, std::ios::binary);

    if (!itch_file)
    {
        std::cout << "Itch File could not be opened\n";
        return 0;
    }

    std::cout << "Itch file opened successfully\n";

    std::size_t msgCount = 0;

    while(itch_file.good())
    {
        ITCHMessageHeader msg_header;
        if (readMessageHeader(itch_file, msg_header))
        {
            if (MESSAGE_TYPES.find(msg_header.msgType) != MESSAGE_TYPES.end())
            {
                //Print every hour hour
                if (curr_time + 3600000000000 <= msg_header.time_stamp)
                {
                    curr_time = msg_header.time_stamp;
                    printVWAPMap();
                }

                bool loop_break = false;
                switch (msg_header.msgType)
                {

                    case 'S':
                    {
                        if (!processS(itch_file))
                        {
                            loop_break = true;
                        }

                        break;
                    }

                    case 'A':
                    {
                        if (!processAorF(itch_file))
                        {
                            loop_break = true;
                        }

                        break;
                    }

                    case 'F':
                    {
                        if (!processAorF(itch_file))
                        {
                            loop_break = true;
                        }

                        //Ignore the attribution
                        itch_file.ignore(size_t(4));

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

                    case 'C':
                    {
                        if (!processC(itch_file))
                        {
                            loop_break = true;
                        }
                        break;
                    }

                    case 'X':
                    {
                        if (!processX(itch_file))
                        {
                            loop_break = true;
                        }
                        break;
                    }

                    case 'D':
                    {
                        if (!processD(itch_file))
                        {
                            loop_break = true;
                        }
                        break;
                    }

                    case 'U':
                    {
                        if (!processU(itch_file))
                        {
                            loop_break = true;
                        }
                        break;
                    }

                    case 'P':
                    {
                        if (!processP(itch_file))
                        {
                            loop_break = true;
                        }
                        break;
                    }

                    case 'Q':
                    {
                        if (!processQ(itch_file))
                        {
                            loop_break = true;
                        }
                        break;
                    }

                    case 'B':
                    {
                        if (!processB(itch_file))
                        {
                            loop_break = true;
                        }
                        break;
                    }

                    default:
                    {
                        itch_file.ignore(msg_header.msg_size - static_cast<u_int8_t>(11));
                        break;
                    }

                }

                ++msgCount;
                //We were unable to read a message
                if (loop_break)
                {
                    break;
                }
            }
    
            else
            {
                std::cout << "Unknown Message Encountered\n";
                break;
            }
        }
        else
        {
            std::cout << "Unable to read message header.";
        }
    }

    std::cout << "Closing Itch File\n";
    logFile.flush();
    return msgCount;

}

//-------------------------------------------------------------------------------------------------
bool ItchParser::readMessageHeader(std::ifstream &file, ITCHMessageHeader &header)
{
    if (!file.read(reinterpret_cast<char*>(&header), 7))
    {
        std::cout << "Could not read msgType, locate, and tracking_num\n";
        return false;
    }

    //Converting to little endian using network to host short
    header.locate = ntohs(header.locate);
    header.tracking_num = ntohs(header.tracking_num);

    uint64_t ts_raw = 0;
    if (!file.read(reinterpret_cast<char*>(&ts_raw) + 2, 6))
    {

        std::cout << "Could not read timestamp\n";
        return false;

    }
    header.time_stamp = __builtin_bswap64(ts_raw);

    return true;
}

//-------------------------------------------------------------------------------------------------
bool ItchParser::processS(std::ifstream &file)
{
    char event_code;
    if (!file.read(&event_code, sizeof(event_code)))
    {
        std::cout << "Could not read type S message\n";
        return false;
    }

    switch(event_code)
    {
        case 'O':
            logFile << "Start of Messages\n";
            std::cout << "Start of Messages\n";
            break;
        case 'S':
            logFile << "Start of System hours\n";
            std::cout << "Start of System hours\n";
            break;
        case 'Q':
            logFile << "Start of Market hours\n";
            std::cout << "Start of Market hours\n";
            break;
        case 'M':
            logFile << "End of Market hours\n";
            std::cout << "End of Market hours\n";
            break;
        case 'E':
            logFile << "End of System hours\n";
            std::cout << "End of System hours\n";
            break;
        case 'C':
            logFile << "End of Messages\n";
            std::cout << "End of Messages\n";
            return false;
            break;
        default:
            logFile << "Unrecognized Event Code\n";
            std::cout << "Unrecognized Event Code\n";
            break;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
bool ItchParser::processAorF(std::ifstream &file)
{

    //We've read the message header, we now need to read the rest of the contents of the message
    TypeA msg;
    if (!file.read(reinterpret_cast<char*>(&msg), sizeof(msg)))
    {
        std::cout << "Could not read type A/F message\n";
        return false;
    }

    //Convert to little endian
    msg.order_ref = __builtin_bswap64(msg.order_ref);
    msg.shares = ntohl(msg.shares);
    msg.price = ntohl(msg.price);

    order_book.try_emplace(msg.order_ref, msg.shares, msg.price, msg.stock);

    return true;

}

//-------------------------------------------------------------------------------------------------
bool ItchParser::processE(std::ifstream &file)
{
    TypeE msg;
    if (!file.read(reinterpret_cast<char*>(&msg), sizeof(msg)))
    {
        std::cout << "Could not read type E message\n";
        return false;
    }

    //Convert to little endian
    msg.order_ref = __builtin_bswap64(msg.order_ref);
    msg.shares = ntohl(msg.shares);
    msg.match_num = __builtin_bswap64(msg.match_num);

    //Modify Order book
    auto it = order_book.find(msg.order_ref);
    if (it != order_book.end())
    {
        updateVWAP(it->second.stock, it->second.price, msg.shares);

        executed_trade_book.try_emplace(msg.match_num, msg.shares, it->second.price, it->second.stock);

        //Subtract the shares from the order executed message from the shares in the order
        if (it->second.shares < msg.shares)
        {
            logFile << "Warning: Trying to subtract more shares than available\n";
            order_book.erase(it);
        }

        else
        {
            it->second.shares -= msg.shares;

            if (it->second.shares == 0)
            {
                order_book.erase(it);
            }
        }
    }
    else
    {
        // logFile << "Process E - Message with Order Reference " << msg.order_ref <<
        // " not found in order book\n";
    }


    return true;
}

//-------------------------------------------------------------------------------------------------
bool ItchParser::processC(std::ifstream &file)
{

    TypeC msg;
    if (!file.read(reinterpret_cast<char*>(&msg), sizeof(msg)))
    {
        std::cout << "Could not read type C message\n";
        return false;
    }

    //Convert to little endian
    msg.order_ref = __builtin_bswap64(msg.order_ref);
    msg.shares = ntohl(msg.shares);
    msg.match_num = __builtin_bswap64(msg.match_num);
    msg.price = ntohl(msg.price);

    //Modify Order book
    auto it = order_book.find(msg.order_ref);
    if (it != order_book.end())
    {

        //Only update the vwap if this message is printable
        if ('Y' == msg.printable)
        {
            updateVWAP(it->second.stock, msg.price, msg.shares);
        }

        executed_trade_book.try_emplace(msg.match_num, msg.shares, msg.price, it->second.stock);

        //Subtract the shares from the order executed message from the shares in the order
        if (it->second.shares < msg.shares)
        {
            logFile << "Warning: Trying to subtract more shares than available\n";
            order_book.erase(it);
        }

        else
        {
            it->second.shares -= msg.shares;
            
            if (it->second.shares == 0)
            {
                order_book.erase(it);
            }
        }
    }
    else
    {
        // logFile << "Process C - Message with Order Reference " << msg.order_ref <<
        // " not found in order book\n";
    }


    return true;
}

//-------------------------------------------------------------------------------------
bool ItchParser::processX(std::ifstream &file)
{

    TypeX msg;
    if (!file.read(reinterpret_cast<char*>(&msg), sizeof(msg)))
    {
        std::cout << "Could not read type X message\n";
        return false;
    }

    //Convert to little endian
    msg.order_ref = __builtin_bswap64(msg.order_ref);
    msg.cancelled_shares = ntohl(msg.cancelled_shares);

    //Find the message order reference in the book
    auto it = order_book.find(msg.order_ref);
    if (it != order_book.end())
    {
        //Subtract the shares from the order executed message from the shares in the order
        if (it->second.shares < msg.cancelled_shares)
        {
            logFile << "Warning: Trying to subtract more shares than available\n";
            it->second.shares = 0;
        }

        else
        {
            it->second.shares -= msg.cancelled_shares;
        }
    }
    else
    {
        // logFile << "Process X - Message with Order Reference " << msg.order_ref <<
        // " not found in order book\n";
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool ItchParser::processD(std::ifstream &file)
{

    uint64_t order_ref;
    if (!file.read(reinterpret_cast<char*>(&order_ref), sizeof(order_ref)))
    {
        std::cout << "Could not read type D message\n";
        return false;
    }

    //Convert to little endian
    order_ref = __builtin_bswap64(order_ref);

    //Find the message order reference in the book
    auto it = order_book.find(order_ref);
    if (it != order_book.end())
    {
        order_book.erase(it);
    }
    else
    {
        // logFile << "Process D - Message with Order Reference " << order_ref <<
        // " not found in order book\n";
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool ItchParser::processU(std::ifstream &file)
{

    TypeU msg;
    if (!file.read(reinterpret_cast<char*>(&msg), sizeof(msg)))
    {
        std::cout << "Could not read type U message\n";
        return false;
    }

    //Convert to little endian
    msg.orig_order_ref = __builtin_bswap64(msg.orig_order_ref);
    msg.new_order_ref = __builtin_bswap64(msg.new_order_ref);
    msg.new_shares = ntohl(msg.new_shares);
    msg.new_price = ntohl(msg.new_price);

    auto it = order_book.find(msg.orig_order_ref);
    if (it != order_book.end())
    {
        order_book.try_emplace(msg.new_order_ref, msg.new_shares, msg.new_price, it->second.stock);
        order_book.erase(it);
    }
    else
    {
        // logFile << "Process U - Message with Order Reference " << msg.orig_order_ref <<
        // " not found in order book\n";
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool ItchParser::processP(std::ifstream &file)
{

    TypeP msg;
    if (!file.read(reinterpret_cast<char*>(&msg), sizeof(msg)))
    {
        std::cout << "Could not read type P message\n";
        return false;
    }

    //Convert to little endian
    msg.order_ref = __builtin_bswap64(msg.order_ref);
    msg.shares = ntohl(msg.shares);
    msg.price = ntohl(msg.price);
    msg.match_num = __builtin_bswap64(msg.match_num);

    updateVWAP(msg.stock, msg.price, msg.shares);

    //Add executed order to executed order book
    executed_trade_book.try_emplace(msg.match_num, msg.shares, msg.price, msg.stock);

    return true;

}

//-------------------------------------------------------------------------------------
bool ItchParser::processQ(std::ifstream &file)
{
    TypeQ msg;
    if (!file.read(reinterpret_cast<char*>(&msg), sizeof(msg)))
    {
        std::cout << "Could not read type Q message\n";
        return false;
    }

    //Convert to little endian
    msg.shares = __builtin_bswap64(msg.shares);
    msg.price = ntohl(msg.price);
    msg.match_num = __builtin_bswap64(msg.match_num);

    if (msg.shares > 0)
    {
        updateVWAP(msg.stock, msg.price, msg.shares);
        executed_trade_book.try_emplace(msg.match_num, msg.shares, msg.price, msg.stock);
    }

    return true;

}

//-------------------------------------------------------------------------------------
bool ItchParser::processB(std::ifstream &file)
{

    uint64_t match_num;
    if (!file.read(reinterpret_cast<char*>(&match_num), sizeof(match_num)))
    {
        std::cout << "Could not read type B message\n";
        return false;
    }

    //Convert to little endian
    match_num = __builtin_bswap64(match_num);

    //Find the message order reference in the book
    auto it = executed_trade_book.find(match_num);
    if (it != executed_trade_book.end())
    {
        StockSymbol stock_symbol;
        std::memcpy(stock_symbol.data(), it->second.stock, 8);
        auto vwap_it = vwap_map.find(stock_symbol);
        if (vwap_it != vwap_map.end())
        {
            vwap_it->second.processBrokenTrade(it->second.price, it->second.shares);
        }

        executed_trade_book.erase(it);
    }
    else
    {
        // logFile << "Process B - Message with Order Reference " << match_num <<
        // " not found in order book\n";
    }

    return true;
}