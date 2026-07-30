#pragma once
#include "order.hpp"
#include <map>
#include <list>
#include <optional>

class OrderBook {
public:
    void add(const Order& o) {
        if (o.side == Side::Buy) {
            bids_[o.price].push_back(o);
        } else {
            asks_[o.price].push_back(o);
        }
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
