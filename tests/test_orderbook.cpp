#include <gtest/gtest.h>
#include "../src/orderbook.hpp"

TEST(OrderBook, PartialFillReducesRestingQuantity) {
    OrderBook book;
    book.add({1, 1002500, 0, Side::Buy, 100});
    book.add({2, 1002400, 0, Side::Buy, 200});
    book.submit({3, 1002400, 0, Side::Sell, 250});

    EXPECT_EQ(book.quantity_at(Side::Buy, 1002500), 0u);
    EXPECT_EQ(book.quantity_at(Side::Buy, 1002400), 50u);
}

TEST(OrderBook, FullyFilledLevelIsRemoved) {
    OrderBook book;
    book.add({1, 1002700, 0, Side::Sell, 150});
    book.submit({2, 1002700, 0, Side::Buy, 150});

    EXPECT_FALSE(book.best_ask().has_value());
}

TEST(OrderBook, UnfilledRemainderRests) {
    OrderBook book;
    book.add({1, 1002700, 0, Side::Sell, 150});
    book.submit({2, 1002700, 0, Side::Buy, 200});

    EXPECT_FALSE(book.best_ask().has_value());
    EXPECT_EQ(book.best_bid().value(), 1002700);
    EXPECT_EQ(book.quantity_at(Side::Buy, 1002700), 50u);
}

TEST(OrderBook, CancelRemovesOrder) {
    OrderBook book;
    book.add({1, 1002500, 0, Side::Buy, 100});
    book.add({2, 1002500, 0, Side::Buy, 200});

    EXPECT_TRUE(book.cancel(1));
    EXPECT_EQ(book.quantity_at(Side::Buy, 1002500), 200u);
}

TEST(OrderBook, CancelLastOrderErasesLevel) {
    OrderBook book;
    book.add({1, 1002500, 0, Side::Buy, 100});

    EXPECT_TRUE(book.cancel(1));
    EXPECT_FALSE(book.best_bid().has_value());
}

TEST(OrderBook, CancelUnknownIdReturnsFalse) {
    OrderBook book;
    EXPECT_FALSE(book.cancel(999));
}

// The index must not outlive the orders it points at.
// If submit() forgot to clean up, this would return true
// and then erase through a dangling iterator.
TEST(OrderBook, CancelFilledOrderReturnsFalse) {
    OrderBook book;
    book.add({1, 1002700, 0, Side::Sell, 150});
    book.submit({2, 1002700, 0, Side::Buy, 150});

    EXPECT_FALSE(book.cancel(1));
    EXPECT_EQ(book.index_size(), 0u);
}