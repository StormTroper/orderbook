# Limit Order Book

A price-time-priority limit order book and matching engine in C++.

## Notes

**Prices are integers, not floats.** `0.1 + 0.2 == 0.3` is false in IEEE 754
(it evaluates to 0.30000000000000004441). Matching depends on exact price
equality, so prices are stored as int64 with 4 implied decimal places —
100.25 is stored as 1002500. This follows the NASDAQ ITCH convention.

**Struct layout matters.** Naive field ordering gave `sizeof(Order) == 40`:
8-byte alignment requirements left two 4-byte holes. Ordering fields
largest-first packs to 32 bytes, fitting two orders per 64-byte cache line
instead of one.

## Status

- [x] Order representation
- [ ] Order book (bid/ask price levels)
- [ ] Matching engine
- [ ] Benchmarks (p50/p99/p99.9)
- [ ] ITCH feed parser
