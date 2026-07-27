#include "order.hpp"
#include <iostream>
#include <cstddef>

int main() {
    std::cout << "sizeof  = " << sizeof(Order) << "\n";
    std::cout << "id      @ " << offsetof(Order, id) << "\n";
    std::cout << "side    @ " << offsetof(Order, side) << "\n";
    std::cout << "price   @ " << offsetof(Order, price) << "\n";
    std::cout << "qty     @ " << offsetof(Order, quantity) << "\n";
    std::cout << "time    @ " << offsetof(Order, timestamp) << "\n";
    return 0;
}
