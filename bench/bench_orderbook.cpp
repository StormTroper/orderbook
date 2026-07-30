#include "../src/orderbook.hpp"
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

using Clock = std::chrono::steady_clock;

static int64_t ns_since(Clock::time_point a, Clock::time_point b) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(b - a).count();
}

static int64_t pct(const std::vector<int64_t>& v, double p) {
    return v[static_cast<size_t>((p / 100.0) * (v.size() - 1))];
}

static void report(const char* name, std::vector<int64_t> lat, double secs) {
    std::sort(lat.begin(), lat.end());
    std::cout << name << "\n"
              << "  ops        " << lat.size() << "\n"
              << "  throughput " << static_cast<int64_t>(lat.size() / secs) << " ops/sec\n"
              << "  p50        " << pct(lat, 50)   << " ns\n"
              << "  p99        " << pct(lat, 99)   << " ns\n"
              << "  p99.9      " << pct(lat, 99.9) << " ns\n"
              << "  max        " << lat.back()     << " ns\n\n";
}

int main() {
    constexpr int     WARMUP      = 50000;
    constexpr int     SAMPLES     = 500000;
    constexpr int64_t MID         = 1000000;   // 100.0000
    constexpr int64_t TICK        = 100;       // 0.0100
    constexpr int     LEVELS      = 50;
    constexpr size_t  BOOK_TARGET = 10000;

    std::mt19937_64 rng(42);                   // fixed seed: reproducible
    std::uniform_int_distribution<int>      lvl(1, LEVELS);
    std::uniform_int_distribution<uint32_t> qty(1, 1000);
    std::uniform_int_distribution<int>      coin(0, 1);

    // noise floor: what does the measurement itself cost?
    std::vector<int64_t> clk;
    clk.reserve(100000);
    for (int i = 0; i < 100000; ++i) {
        auto a = Clock::now();
        auto b = Clock::now();
        clk.push_back(ns_since(a, b));
    }
    std::sort(clk.begin(), clk.end());
    std::cout << "clock overhead p50 = " << pct(clk, 50) << " ns\n\n";

    OrderBook book;
    uint64_t next_id = 1;
    std::vector<uint64_t> live;
    live.reserve(WARMUP + SAMPLES + BOOK_TARGET);
    size_t head = 0;

    auto make_order = [&]() {
        bool buy = coin(rng);
        int64_t price = buy ? MID - lvl(rng) * TICK
                            : MID + lvl(rng) * TICK;
        return Order{next_id++, price, 0,
                     buy ? Side::Buy : Side::Sell, qty(rng)};
    };

    // warm up to steady state
    for (int i = 0; i < WARMUP; ++i) {
        Order o = make_order();
        book.add(o);
        live.push_back(o.id);
        if (live.size() - head > BOOK_TARGET) book.cancel(live[head++]);
    }

    // measure add
    std::vector<int64_t> add_lat;
    add_lat.reserve(SAMPLES);

    auto start = Clock::now();
    for (int i = 0; i < SAMPLES; ++i) {
        Order o = make_order();               // untimed

        auto t0 = Clock::now();
        book.add(o);
        auto t1 = Clock::now();

        add_lat.push_back(ns_since(t0, t1));
        live.push_back(o.id);
        if (live.size() - head > BOOK_TARGET) book.cancel(live[head++]);
    }
    double secs = std::chrono::duration<double>(Clock::now() - start).count();
    report("add", std::move(add_lat), secs);

    // measure cancel
    std::vector<int64_t> can_lat;
    can_lat.reserve(BOOK_TARGET + 1);

    start = Clock::now();
    while (live.size() - head > 1) {
        uint64_t id = live[head++];

        auto t0 = Clock::now();
        book.cancel(id);
        auto t1 = Clock::now();

        can_lat.push_back(ns_since(t0, t1));
    }
    secs = std::chrono::duration<double>(Clock::now() - start).count();
    report("cancel", std::move(can_lat), secs);

    return 0;
}