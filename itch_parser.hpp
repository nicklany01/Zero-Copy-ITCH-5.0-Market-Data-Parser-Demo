#pragma once

#include <cstdint>
#include <cstddef>

// TODO: Define packed C structs for message types (Add Order, Order Executed, Order Cancel)
// #pragma pack(push, 1)
// ...
// #pragma pack(pop)

// TODO: Implement endianness conversion using __builtin_bswap64 / std::byteswap

// TODO: Implement the ITCH parser logic and integration with SPSC queue
class ItchParser {
public:
    // ...
};
