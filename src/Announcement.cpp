#include "Announcement.h"

// default
Announcement::Announcement() : next_asn(0), relationship(0), rov_invalid(false) {}

// custom
Announcement::Announcement(const std::string &p, const uint32_t &asn, const bool &invalid) : prefix(p), next_asn(asn), relationship(0), rov_invalid(invalid)
{
    as_path.push_back(asn);
}
