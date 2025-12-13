BGP Simulator - Dan Foster

COMPILING
c++ -std=c++17 -I include/ src/*.cpp -o bgp_simulator
EXECUTING
./bgp_simulator --relationships many/caida.csv --announcements many/anns.csv --rov-asns many/rov_asns.csv

Design Decisions:
- Structs rather than classes - no need for getters when everything is public
- Raw pointers (AS*) instead of smart ones, faster and simpler if manual cleanup is done correctly
- Each AS's providers, peers, and customers are organized separately in order to make propagation behavior clear
- Policy can be overwritten when ROV is implemented since ROV inherits from BGP
- Uses unordered_maps as a hashmap in order to have O(1) lookup time
- Ranks utilized for batched processing instead of just a queue, gaurantees BGP accurate processing
- When outputting, use a buffer with one write command rather than many write commands in order to save time