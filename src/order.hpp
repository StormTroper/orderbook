#pragma once
#include <cstdint>

enum class Side { Buy, Sell };

// Fields ordered largest-first to eliminate padding.
// Naive declaration order gives sizeof == 40; this gives 32.
struct Order {
    uint64_t id;
    int64_t  price;      // implied 4 decimals: 100.25 -> 1002500
    uint64_t timestamp;  // nanoseconds
    Side     side;
    uint32_t quantity;
};
