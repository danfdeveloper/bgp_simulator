#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>
#include "Announcement.h"

class BGP
{
public:
    std::unordered_map<std::string, Announcement> local_rib;
    std::unordered_map<std::string, std::vector<Announcement>> received_queue;

    virtual ~BGP() = default;

    virtual void process_announcements(uint32_t owner_asn);

    bool replace(const Announcement &existing, const Announcement &new_ann);
};
