#include <gtest/gtest.h>
#include "../src/orderbook.hpp"

// A partial fill must reduce the RESTING order, not a copy of it.
TEST(OrderBook, PartialFillReducesRestingQuantity) {
    OrderBook book;
    book.add({1, 1002500, 0, Side::Buy, 100});
    book.add({2, 1002400, 0, Side::Buy, 200});

    book.submit({3, 1002400, 0, Side::Sell, 250});

    EXPECT_EQ(book.quantity_at(Side::Buy, 1002500), 0u);
    EXPECT_EQ(book.quantity_at(Side::Buy, 1002400), 50u);
}

// A fully consumed level must be erased, not left empty.
TEST(OrderBook, FullyFilledLevelIsRemoved) {
    OrderBook book;
    book.add({1, 1002700, 0, Side::Sell, 150});

    book.submit({2, 1002700, 0, Side::Buy, 150});

    EXPECT_FALSE(book.best_ask().has_value());
    EXPECT_EQ(book.quantity_at(Side::Sell, 1002700), 0u);
}

// Unfilled remainder must rest in the book.
TEST(OrderBook, UnfilledRemainderRests) {
    OrderBook book;
    book.add({1, 1002700, 0, Side::Sell, 150});

    book.submit({2, 1002700, 0, Side::Buy, 200});

    EXPECT_FALSE(book.best_ask().has_value());
    EXPECT_EQ(book.best_bid().value(), 1002700);
    EXPECT_EQ(book.quantity_at(Side::Buy, 1002700), 50u);
}