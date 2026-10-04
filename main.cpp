#include <iostream>
#include "itch_parser.hpp"

// TODO: Overload global operator new to assert false (prevent heap allocations during parsing)
// void* operator new(std::size_t size) {
//     ...
// }

int main() {
    std::cout << "Zero-Copy ITCH 5.0 Market Data Parser" << std::endl;
    
    // TODO: Ingest PCAP or raw dump files via mmap(..., MAP_SHARED)
    
    // TODO: Initialize SPSC Queue and ItchParser
    
    return 0;
}
