#include "orderbook.hpp"
#include <iostream>

int main() {
    OrderBook book;
    book.add({1, 1002500, 0, Side::Buy,  100});
    book.add({2, 1002400, 0, Side::Buy,  200});
    book.add({3, 1002700, 0, Side::Sell, 150});
    book.add({4, 1002800, 0, Side::Sell,  50});

    std::cout << "best bid = " << book.best_bid().value_or(-1) << "\n";
    std::cout << "best ask = " << book.best_ask().value_or(-1) << "\n";
    return 0;
}
