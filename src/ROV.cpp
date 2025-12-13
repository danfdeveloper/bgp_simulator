#include "ROV.h"
#include <algorithm>

void ROV::process_announcements(uint32_t owner_asn)
{
    for (auto &[prefix, announcements] : received_queue)
    {
        announcements.erase(
            std::remove_if(announcements.begin(), announcements.end(), [](const Announcement &ann)
                           { return ann.rov_invalid; }),
            announcements.end());
    }

    BGP::process_announcements(owner_asn);
}
