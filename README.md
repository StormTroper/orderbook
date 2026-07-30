# Limit Order Book

A price-time-priority limit order book and matching engine in C++20.

## Design notes

**Prices are integers, not floats.** `0.1 + 0.2 == 0.3` is false in IEEE 754
(it evaluates to 0.30000000000000004441). Matching depends on exact price
equality, so prices are stored as int64 with 4 implied decimal places —
100.25 is stored as 1002500. This follows the NASDAQ ITCH convention.

**Struct layout matters.** Naive field ordering gave `sizeof(Order) == 40`:
8-byte alignment requirements left two 4-byte holes. Ordering fields
largest-first packs to 32 bytes, fitting two orders per 64-byte cache line
instead of one.

**`std::list` for price levels, not `std::vector`.** List iterators remain
valid across insertion and erasure of other elements, which is what makes
the order index viable. A vector reallocation would invalidate every stored
iterator. The tradeoff is worse cache locality — each node is a separate
allocation. A pooled-allocation approach would recover that, and is a
candidate optimisation once benchmarks exist.

**Fills execute at the resting order's price, not the aggressor's.** A limit
price is a boundary, not an execution price — buyers may pay below their
ceiling, sellers may receive above their floor. Price improvement falls out
of taking the level price as the fill price.

## Complexity

| Operation | Complexity | Notes |
|---|---|---|
| `add` | O(log n) | map lookup over price levels |
| `submit` | O(log n) per level swept | plus O(1) per fill |
| `cancel` | O(log n) | hash lookup O(1), then a map lookup for the level |
| `best_bid` / `best_ask` | O(1) | `begin()` on an ordered map |

`cancel` is **not** O(1). The index removes the O(n) scan across the whole
book, but the price-to-level lookup is still logarithmic. Storing the map
iterator instead of the price would make it genuinely O(1) — map iterators
are also stable — at the cost of handling the two comparator types.

## Invariants under test

1. Quantity is conserved — a fill reduces the resting order by exactly the traded amount
2. Fully consumed price levels are erased, never left empty
3. Unfilled remainder rests in the book
4. Cancellation removes only the targeted order
5. Every index entry points at an order that still exists

## Status

- [x] Order representation
- [x] Order book (bid/ask price levels)
- [x] Matching engine, both sides, multi-level sweep
- [x] Cancellation via order index
- [x] Invariant tests (GoogleTest)
- [ ] Benchmarks (p50/p99/p99.9)
- [ ] Flat-array price levels
- [ ] ITCH feed parser