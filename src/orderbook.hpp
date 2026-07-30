#pragma once
#include "order.hpp"
#include <map>
#include <list>
#include <unordered_map>
#include <optional>
#include <algorithm>
#include <iostream>

class OrderBook {
public:
    void add(const Order& o) {
        if (o.side == Side::Buy) {
            auto& level = bids_[o.price];
            auto it = level.insert(level.end(), o);
            index_[o.id] = OrderLocation{o.side, o.price, it};
        } else {
            auto& level = asks_[o.price];
            auto it = level.insert(level.end(), o);
            index_[o.id] = OrderLocation{o.side, o.price, it};
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
                    index_.erase(resting.id);
                    level->second.pop_front();
                    if (level->second.empty()) asks_.erase(level);
                }
            }
        } else {
            while (o.quantity > 0 && !bids_.empty()
                   && bids_.begin()->first >= o.price) {

                auto level = bids_.begin();
                auto& resting = level->second.front();

                uint32_t traded = std::min(o.quantity, resting.quantity);
                o.quantity       -= traded;
                resting.quantity -= traded;

                std::cout << "FILL " << traded << " @ " << level->first << "\n";

                if (resting.quantity == 0) {
                    index_.erase(resting.id);
                    level->second.pop_front();
                    if (level->second.empty()) bids_.erase(level);
                }
            }
        }
        if (o.quantity > 0) add(o);
    }

    bool cancel(uint64_t id) {
        auto idx = index_.find(id);
        if (idx == index_.end()) return false;

        const OrderLocation& loc = idx->second;

        if (loc.side == Side::Buy) {
            auto level = bids_.find(loc.price);
            level->second.erase(loc.it);
            if (level->second.empty()) bids_.erase(level);
        } else {
            auto level = asks_.find(loc.price);
            level->second.erase(loc.it);
            if (level->second.empty()) asks_.erase(level);
        }

        index_.erase(idx);
        return true;
    }

    std::optional<int64_t> best_bid() const {
        if (bids_.empty()) return std::nullopt;
        return bids_.begin()->first;
    }

    std::optional<int64_t> best_ask() const {
        if (asks_.empty()) return std::nullopt;
        return asks_.begin()->first;
    }

    uint32_t quantity_at(Side side, int64_t price) const {
        uint32_t total = 0;
        if (side == Side::Buy) {
            auto it = bids_.find(price);
            if (it == bids_.end()) return 0;
            for (const auto& o : it->second) total += o.quantity;
        } else {
            auto it = asks_.find(price);
            if (it == asks_.end()) return 0;
            for (const auto& o : it->second) total += o.quantity;
        }
        return total;
    }

    size_t index_size() const { return index_.size(); }

private:
    struct OrderLocation {
        Side side;
        int64_t price;
        std::list<Order>::iterator it;
    };

    std::map<int64_t, std::list<Order>, std::greater<int64_t>> bids_;
    std::map<int64_t, std::list<Order>> asks_;
    std::unordered_map<uint64_t, OrderLocation> index_;
};