#include "AS.h"

// custom
AS::AS(uint32_t asn) : asn(asn), prop_rank(-1), policy(nullptr)
{
    policy = new BGP();
}

// destructor
AS::~AS()
{
    delete policy;
}
