#include "BGP.h"

void BGP::process_announcements(uint32_t owner_asn)
{
    for (auto &[prefix, announcements] : received_queue)
    {
        for (auto &ann : announcements)
        {
            ann.as_path.insert(ann.as_path.begin(), owner_asn);

            auto [it, inserted] = local_rib.try_emplace(prefix, ann);
            if (!inserted && replace(it->second, ann))
            {
                it->second = ann;
            }
        }
    }
    received_queue.clear();
}

bool BGP::replace(const Announcement &existing, const Announcement &new_ann)
{
    if (new_ann.relationship < existing.relationship)
        return true;
    else if (new_ann.relationship > existing.relationship)
        return false;
    else if (new_ann.as_path.size() < existing.as_path.size())
        return true;
    else if (new_ann.as_path.size() > existing.as_path.size())
        return false;

    return new_ann.next_asn < existing.next_asn;
}
