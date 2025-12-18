#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <cstring>
#include "ASGraph.h"

int main(int argc, char *argv[])
{
    auto start_time = std::chrono::high_resolution_clock::now();

    std::string caida_file;
    std::string announcements_file;
    std::string rov_asns_file;
    std::string output_file = "ribs.csv";

    // Parse command-line arguments with flags
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--relationships") == 0 && i + 1 < argc)
        {
            caida_file = argv[++i];
        }
        else if (strcmp(argv[i], "--announcements") == 0 && i + 1 < argc)
        {
            announcements_file = argv[++i];
        }
        else if (strcmp(argv[i], "--rov-asns") == 0 && i + 1 < argc)
        {
            rov_asns_file = argv[++i];
        }
        else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc)
        {
            output_file = argv[++i];
        }
    }

    // Validate required arguments
    if (caida_file.empty() || announcements_file.empty() || rov_asns_file.empty())
    {
        std::cout << "Usage: " << argv[0]
                  << " --relationships <file> --announcements <file> --rov-asns <file> [--output <file>]\n";
        return 1;
    }

    std::cout << "Building AS Graph" << std::endl;
    ASGraph graph;
    graph.build_from_caida(caida_file);
    std::cout << "Graph built ( " << graph.graph.size() << " ASes)" << std::endl;

    std::cout << "Flattening graph" << std::endl;
    graph.flatten_graph();

    graph.load_rov_asns(rov_asns_file);

    graph.seed_announcements(announcements_file);

    std::cout << "Propagating announcements (wait)" << std::endl;
    graph.propagate();

    std::cout << "Exporting to " << output_file << std::endl;

    std::vector<uint32_t> check_asns = {1, 2, 3, 4, 25, 27};
    for (uint32_t asn : check_asns)
    {
        if (graph.graph.find(asn) != graph.graph.end())
        {
            int count = graph.graph[asn]->policy->local_rib.size();
            std::cout << "ASN " << asn << " has " << count << " announcements" << std::endl;
        }
        else
        {
            std::cout << "ASN " << asn << " not found in graph" << std::endl;
        }
    }

    graph.export_to_csv(output_file);

    std::cout << "Done! Output written to " << output_file << std::endl;
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Time taken: " << duration.count() << "ms" << std::endl;
    return 0;
}
