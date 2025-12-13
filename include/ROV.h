#pragma once

#include "BGP.h"
#include <cstdint>

class ROV : public BGP
{
public:
    void process_announcements(uint32_t owner_asn) override;
};
