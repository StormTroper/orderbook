#pragma once
#include "order.hpp"
#include <map>
#include <list>
#include <optional>
#include <algorithm>
#include <iostream>

class OrderBook {
public:
    void add(const Order& o) {
        if (o.side == Side::Buy) {
            bids_[o.price].push_back(o);
        } else {
            asks_[o.price].push_back(o);
        }
    }

    void submit(Order o) {
        if (o.side == Side::Buy) {
            while (o.quantity > 0 && !asks_.empty()
                   && asks_.begin()->first <= o.price) {

                auto level = asks_.begin();
                auto& resting = level->second.front();

                uint32_t traded = std::min(o.quantity, resting.quantity);
                o.quantity       -= traded;
                resting.quantity -= traded;

                std::cout << "FILL " << traded << " @ " << level->first << "\n";

                if (resting.quantity == 0) {
                    level->second.pop_front();
                    if (level->second.empty()) asks_.erase(level);
                }
            }
        }
        if (o.quantity > 0) add(o);
    }

    std::optional<int64_t> best_bid() const {
        if (bids_.empty()) return std::nullopt;
        return bids_.begin()->first;
    }

    std::optional<int64_t> best_ask() const {
        if (asks_.empty()) return std::nullopt;
        return asks_.begin()->first;
    }

private:
    std::map<int64_t, std::list<Order>, std::greater<int64_t>> bids_;
    std::map<int64_t, std::list<Order>> asks_;
};