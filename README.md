# BGP Simulator

An accurate simulator for modeling how Autonomous Systems (AS) handle and route announcements on the internet using the Border Gateway Protocol (BGP). This project implements BGP routing policies, relationship-based path selection, and Route Origin Validation (ROV) security features.

## Features

- **BGP Routing Simulation**: Accurately models BGP announcement propagation across provider-customer and peer-peer relationships
- **Relationship-Based Routing**: Implements BGP decision-making algorithm that prioritizes routes based on AS relationships (customer > peer > provider)
- **Route Origin Validation (ROV)**: Filters invalid route announcements to prevent routing hijacking attacks
- **Graph-Based Architecture**: Uses efficient graph data structures to represent AS relationships and process routing information
- **Cycle Detection**: Validates AS relationship graphs to ensure no invalid provider-customer cycles exist
- **Topological Processing**: Implements rank-based propagation system to process announcements in correct dependency order
- **CSV Data Processing**: Processes CAIDA relationship datasets and exports routing information bases (RIBs)

## Requirements

- C++17 compatible compiler (g++, clang++, etc.)
- Standard C++ library

## Compilation

```bash
c++ -std=c++17 -I include/ src/*.cpp -o bgp_simulator
```

## Usage

```bash
./bgp_simulator --relationships <caida_file> --announcements <announcements_file> --rov-asns <rov_asns_file> [--output <output_file>]
```

### Arguments

- `--relationships <file>`: Path to CAIDA AS relationship file (required)
- `--announcements <file>`: Path to announcements CSV file (required)
- `--rov-asns <file>`: Path to ROV-enabled ASNs file (required)
- `--output <file>`: Path to output CSV file (optional, defaults to `ribs.csv`)

## Input File Formats

### CAIDA Relationship File

Format: `ASN1|ASN2|relationship`

- `relationship = -1`: ASN1 is a provider of ASN2 (ASN2 is customer of ASN1)
- `relationship = 0`: ASN1 and ASN2 are peers
- Lines starting with `#` are treated as comments

Example:
```
1|2|-1
2|3|0
3|4|-1
```

### Announcements File

Format: CSV with header row, then `ASN,prefix,rov_invalid`

- `ASN`: The ASN making the announcement
- `prefix`: The IP prefix being announced (e.g., "192.168.0.0/16")
- `rov_invalid`: Boolean indicating if the announcement is ROV-invalid ("True"/"False" or "1"/"0")

Example:
```csv
ASN,prefix,rov_invalid
1,192.168.0.0/16,False
2,10.0.0.0/8,True
```

### ROV ASNs File

Format: One ASN per line

Lists ASNs that have ROV enabled and should filter invalid announcements.

Example:
```
1
25
27
```

## Output Format

The simulator generates a CSV file containing the Routing Information Base (RIB) for each AS. The output format is:

```csv
ASN,prefix,as_path
```

Where:
- `ASN`: The Autonomous System Number
- `prefix`: The IP prefix in the routing table
- `as_path`: The AS path to reach the prefix (comma-separated ASNs)

## Project Structure

```
courseProject/
├── include/           # Header files
│   ├── AS.h          # Autonomous System structure
│   ├── ASGraph.h     # Graph management and propagation
│   ├── Announcement.h # BGP announcement structure
│   ├── BGP.h         # Base BGP routing policy
│   └── ROV.h         # ROV-enhanced BGP policy
├── src/              # Source files
│   ├── main.cpp      # Entry point and CLI
│   ├── AS.cpp        # AS implementation
│   ├── ASGraph.cpp   # Graph operations
│   ├── Announcement.cpp
│   ├── BGP.cpp       # BGP routing logic
│   └── ROV.cpp       # ROV filtering logic
├── tests/            # Test cases
│   ├── compare_ouput.sh # For comparing output out of order
│   ├── prefix/       # Prefix-related tests
│   └── subprefix/    # Subprefix-related tests
└── README.md
```

## How It Works

1. **Graph Construction**: Reads CAIDA relationship data and builds a graph of AS relationships
2. **Cycle Validation**: Performs DFS cycle detection to ensure no invalid provider-customer cycles
3. **Graph Flattening**: Assigns propagation ranks to ASes based on customer-provider hierarchy
4. **ROV Configuration**: Loads ROV-enabled ASNs and replaces their BGP policies with ROV policies
5. **Announcement Seeding**: Initializes announcements from the input file
6. **Propagation**: Processes announcements in rank order:
   - Customers propagate to providers (upward)
   - Providers propagate to customers (downward)
   - Peers exchange announcements (lateral)
7. **RIB Processing**: Each AS processes received announcements using BGP decision algorithm:
   - Relationship preference (customer > peer > provider)
   - Shortest AS path
   - Tie-breaking by next ASN
8. **Export**: Writes final RIBs to CSV output file

## BGP Decision Algorithm

The simulator implements a simplified BGP decision process:

1. **Relationship Priority**: Customer routes preferred over peer routes, which are preferred over provider routes
2. **Path Length**: Shorter AS paths are preferred
3. **Tie-Breaking**: Lower next ASN value breaks ties

## Author

Dan Foster (danfosterdev@gmail.com)
