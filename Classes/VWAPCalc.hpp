#pragma once
#include <cstdint>

class VWAPCalc
{

public:

    VWAPCalc();

    ~VWAPCalc(){}

    void updateVWAP(const uint32_t price, const uint32_t shares);

    double getVWAP();


private:

    //Summation of shares
    uint64_t total_shares;

    //Summation of price x shares
    uint64_t total_value;

};