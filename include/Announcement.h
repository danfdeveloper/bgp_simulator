#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct Announcement
{
    std::string prefix;
    std::vector<uint32_t> as_path;
    uint32_t next_asn;
    int relationship;
    bool rov_invalid;

    Announcement();
    Announcement(const std::string &p, const uint32_t &asn, const bool &invalid = false);
};
