#include "orderbook.hpp"
#include <iostream>

int main() {
    OrderBook book;
    book.add({1, 1002500, 0, Side::Buy, 100});
    book.add({2, 1002400, 0, Side::Buy, 200});

    book.submit({3, 1002400, 0, Side::Sell, 250});

    std::cout << "best bid = " << book.best_bid().value_or(-1) << "\n";
    std::cout << "best ask = " << book.best_ask().value_or(-1) << "\n";
    return 0;
}