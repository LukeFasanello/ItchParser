#pragma once
#include <unordered_map>
#include <unordered_set>
#include <cstdint>


const uint8_t MESSAGE_HEADER_SIZE = 11;

// Denotes the message size in bytes for each message type
const std::unordered_set<char> MESSAGE_TYPES = {
    'S', 'R','H', 'Y', 'L', 'V', 'W', 'K', 'J', 'h',
    'A', 'F', 'E', 'C', 'X', 'D', 'U', 'P', 'Q', 'B', 'I',
    'N', 'O'};

#pragma pack(push, 1) 
struct ITCHMessageHeader {
    //1
    char spacer;
    //1
    uint8_t msg_size;
    // 1
    char msgType;
    // 2
    uint16_t locate;
    // 2
    uint16_t tracking_num;
    // 6 - Need to shift after unpacking
    uint64_t time_stamp;
};

//Type A message
struct TypeA {
    // 8
    uint64_t order_ref;
    // 1
    char buy_sell;
    // 4
    uint32_t shares;
    // 8
    char stock[8];
    // 4
    uint32_t price;
};

//Type E message
struct TypeE {
    // 8
    uint64_t order_ref;
    // 4
    uint32_t shares;
    // 8
    uint64_t match_num;
};

//Order reference data needed for potential broken trades
struct OrderReference {
    // 4
    uint32_t shares;
    // 4
    uint32_t price;
    // 8
    std::string stock_symbol;
};

#pragma pack(pop)