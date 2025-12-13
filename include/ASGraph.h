#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <cstdint>
#include "AS.h"

class ASGraph
{
public:
    std::unordered_map<uint32_t, AS *> graph;
    std::vector<std::vector<AS *>> ranks;

    ~ASGraph();

    AS *fetch_or_make_AS(uint32_t asn);
    void build_from_caida(const std::string &filename);
    bool check_provider_customer_cycle();
    bool check_cycle_dfs(AS *as, std::unordered_set<uint32_t> &visited,
                         std::unordered_set<uint32_t> &rec_stack);
    void flatten_graph();
    void seed_announcements(const std::string &filename);
    void load_rov_asns(const std::string &filename);
    void propagate();
    void export_to_csv(const std::string &filename);
};
