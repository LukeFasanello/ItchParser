#pragma once
#include <cstdint>

class VWAPCalc
{

public:

    VWAPCalc(const uint32_t price, const uint64_t shares);

    ~VWAPCalc(){}

    void updateVWAP(const uint32_t price, const uint64_t shares);

    void processBrokenTrade(const uint32_t price, const uint64_t shares);

    double getVWAP();


private:

    //Summation of shares
    uint64_t total_shares;

    //Summation of price x shares
    uint64_t total_value;

};