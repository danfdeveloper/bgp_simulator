BGP Simulator - Dan Foster

An accurate simlulator for mapping how Autonomous Systems (AS) handle and route announcements made on the internet with Border Gateway Protocol.

COMPILING
c++ -std=c++17 -I include/ src/*.cpp -o bgp_simulator
EXECUTING
./bgp_simulator --relationships many/caida.csv --announcements many/anns.csv --rov-asns many/rov_asns.csv



