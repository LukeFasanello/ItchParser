
#include "VWAPCalc.hpp"


VWAPCalc::VWAPCalc() :
    total_shares{0},
    total_value{0}
{}

void VWAPCalc::updateVWAP(const uint32_t price, const uint32_t shares)
{
    total_shares += shares;

    //Static cast to prevent overflow
    total_value += static_cast<uint64_t>(price) * shares;
}

double VWAPCalc::getVWAP()
{
    if (total_shares == 0)
    {
        return 0.0;
    }

    return static_cast<double>(total_value)/total_shares;
}