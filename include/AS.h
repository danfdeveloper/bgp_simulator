#pragma once

#include <vector>
#include <cstdint>
#include "BGP.h"

struct AS
{
    uint32_t asn;
    std::vector<AS *> providers;
    std::vector<AS *> customers;
    std::vector<AS *> peers;
    int prop_rank;
    BGP *policy;

    AS(uint32_t asn);
    ~AS();
};
