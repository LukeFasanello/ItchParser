
#include "VWAPCalc.hpp"
#include <iostream>


VWAPCalc::VWAPCalc(const uint32_t price, const uint64_t shares) :
    total_shares{0},
    total_value{0}
{
    updateVWAP(price, shares);
}

void VWAPCalc::updateVWAP(const uint32_t price, const uint64_t shares)
{
    total_shares += shares;

    //Static cast to prevent overflow
    total_value += static_cast<uint64_t>(price) * shares;
}

void VWAPCalc::processBrokenTrade(const uint32_t price, const uint64_t shares)
{

    uint64_t sub = static_cast<uint64_t>(price) * shares;

    if (shares > total_shares || sub > total_value) {
        std::cerr << "Warning: Attempted to subtract more than accumulated\n";
        return;
    }

    total_shares -= shares;

    //Static cast to prevent overflow
    total_value -= static_cast<uint64_t>(price) * shares;
}

double VWAPCalc::getVWAP()
{
    if (total_shares == 0)
    {
        return 0.0;
    }

    return static_cast<double>(total_value)/total_shares/10000.0;
}