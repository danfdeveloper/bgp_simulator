#include "ASGraph.h"
#include "ROV.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>

// destructor
ASGraph::~ASGraph()
{
    for (auto &[asn, as] : graph)
    {
        delete as;
    }
}

AS *ASGraph::fetch_or_make_AS(uint32_t asn)
{
    if (graph.find(asn) == graph.end())
    {
        graph[asn] = new AS(asn);
    }
    return graph[asn];
}

void ASGraph::build_from_caida(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Could not open file. " << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        else if (line[0] == '#')
        {
            continue;
        }

        std::stringstream ss(line);
        std::string field;
        std::vector<std::string> fields;

        while (std::getline(ss, field, '|'))
        {
            fields.push_back(field);
        }

        if (fields.size() < 3)
            continue;

        uint32_t asn1 = std::stoul(fields[0]);
        uint32_t asn2 = std::stoul(fields[1]);
        int relationship = std::stoi(fields[2]);

        AS *as1 = fetch_or_make_AS(asn1);
        AS *as2 = fetch_or_make_AS(asn2);

        if (relationship == -1)
        {
            as1->customers.push_back(as2);
            as2->providers.push_back(as1);
        }
        else if (relationship == 0)
        {
            as1->peers.push_back(as2);
            as2->peers.push_back(as1);
        }
    }

    file.close();

    if (check_provider_customer_cycle())
    {
        std::cerr << "Cycle detected" << std::endl;
        exit(1);
    }
}

bool ASGraph::check_provider_customer_cycle()
{
    std::unordered_set<uint32_t> visited;
    std::unordered_set<uint32_t> rec_stack;

    for (auto &[asn, as] : graph)
    {
        if (check_cycle_dfs(as, visited, rec_stack))
        {
            return true;
        }
    }
    return false;
}

bool ASGraph::check_cycle_dfs(AS *as, std::unordered_set<uint32_t> &visited, std::unordered_set<uint32_t> &rec_stack)
{
    if (rec_stack.find(as->asn) != rec_stack.end())
    {
        return true;
    }

    if (visited.find(as->asn) != visited.end())
    {
        return false;
    }

    visited.insert(as->asn);
    rec_stack.insert(as->asn);

    for (AS *provider : as->providers)
    {
        if (check_cycle_dfs(provider, visited, rec_stack))
        {
            return true;
        }
    }

    rec_stack.erase(as->asn);
    return false;
}

void ASGraph::flatten_graph()
{
    std::queue<AS *> to_be_processed;

    for (auto &[asn, as] : graph)
    {
        if (as->customers.empty())
        {
            as->prop_rank = 0;
            to_be_processed.push(as);
        }
    }

    while (!to_be_processed.empty())
    {
        AS *current = to_be_processed.front();
        to_be_processed.pop();

        for (AS *provider : current->providers)
        {
            if (provider->prop_rank < current->prop_rank + 1)
            {
                provider->prop_rank = current->prop_rank + 1;
                to_be_processed.push(provider);
            }
        }
    }

    int max_rank = 0;
    for (auto &[asn, as] : graph)
    {
        if (as->prop_rank > max_rank)
        {
            max_rank = as->prop_rank;
        }
    }

    ranks.resize(max_rank + 1);
    for (auto &[asn, as] : graph)
    {
        if (as->prop_rank >= 0)
        {
            ranks[as->prop_rank].push_back(as);
        }
    }
}

void ASGraph::seed_announcements(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Could not open file" << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line);

    int seeded_count = 0;
    int not_found_count = 0;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string asn_string, prefix, rov_string;

        std::getline(ss, asn_string, ',');
        std::getline(ss, prefix, ',');
        std::getline(ss, rov_string, ',');

        asn_string.erase(0, asn_string.find_first_not_of(" \t\r\n"));
        asn_string.erase(asn_string.find_last_not_of(" \t\r\n") + 1);
        prefix.erase(0, prefix.find_first_not_of(" \t\r\n"));
        prefix.erase(prefix.find_last_not_of(" \t\r\n") + 1);
        rov_string.erase(0, rov_string.find_first_not_of(" \t\r\n"));
        rov_string.erase(rov_string.find_last_not_of(" \t\r\n") + 1);

        uint32_t asn = std::stoul(asn_string);
        bool rov_invalid = (rov_string == "True" || rov_string == "true" || rov_string == "1");

        if (graph.find(asn) != graph.end())
        {
            Announcement ann(prefix, asn, rov_invalid);
            graph[asn]->policy->local_rib[prefix] = ann;
            seeded_count++;
        }
        else
        {
            not_found_count++;
            std::cout << "ASN: " << asn << " wasn't found in the graph" << std::endl;
        }
    }

    // std::cout << seeded_count << " announcements were made" << std::endl;
    if (not_found_count > 0)
    {
        std::cout << not_found_count << " ASNs weren't found in the graph" << std::endl;
    }

    file.close();
}

void ASGraph::load_rov_asns(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Could not open file " << filename << std::endl;
        return;
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        uint32_t asn = std::stoul(line);
        if (graph.find(asn) != graph.end())
        {
            delete graph[asn]->policy;
            graph[asn]->policy = new ROV();
        }
    }

    file.close();
}

void ASGraph::propagate()
{
    // customers to providers
    for (size_t rank = 0; rank < ranks.size(); rank++)
    {
        for (AS *as : ranks[rank])
        {
            for (const auto &[prefix, ann] : as->policy->local_rib)
            {
                for (AS *provider : as->providers)
                {
                    auto &queue = provider->policy->received_queue[prefix];
                    queue.emplace_back(ann);
                    queue.back().next_asn = as->asn;
                    queue.back().relationship = 1;
                }
            }
        }

        if (rank + 1 < ranks.size())
        {
            for (AS *as : ranks[rank + 1])
            {
                as->policy->process_announcements(as->asn);
            }
        }
    }

    // peer to peer
    for (auto &[asn, as] : graph)
    {
        for (const auto &[prefix, ann] : as->policy->local_rib)
        {
            for (AS *peer : as->peers)
            {
                auto &queue = peer->policy->received_queue[prefix];
                queue.emplace_back(ann);
                queue.back().next_asn = as->asn;
                queue.back().relationship = 2;
            }
        }
    }

    for (auto &[asn, as] : graph)
    {
        as->policy->process_announcements(as->asn);
    }

    // providers to cusomters
    for (int rank = (int)ranks.size() - 1; rank >= 0; rank--)
    {
        for (AS *as : ranks[rank])
        {
            for (const auto &[prefix, ann] : as->policy->local_rib)
            {
                for (AS *customer : as->customers)
                {
                    auto &queue = customer->policy->received_queue[prefix];
                    queue.emplace_back(ann);
                    queue.back().next_asn = as->asn;
                    queue.back().relationship = 3;
                }
            }
        }

        if (rank > 0)
        {
            for (AS *as : ranks[rank - 1])
            {
                as->policy->process_announcements(as->asn);
            }
        }
    }
}

void ASGraph::export_to_csv(const std::string &filename)
{
    std::ofstream out(filename);
    if (!out.is_open())
    {
        std::cerr << "Could not open file" << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << "asn,prefix,as_path\n";

    for (auto &[asn, as] : graph)
    {
        for (auto &[prefix, ann] : as->policy->local_rib)
        {
            buffer << asn << "," << prefix << ",\"(";
            for (size_t i = 0; i < ann.as_path.size(); i++)
            {
                buffer << ann.as_path[i];

                if (ann.as_path.size() == 1 || i < ann.as_path.size() - 1)
                {
                    buffer << ",";
                    if (i < ann.as_path.size() - 1)
                        buffer << " ";
                }
            }
            buffer << ")\"\n";
        }
    }

    out << buffer.str();
    out.close();
}
