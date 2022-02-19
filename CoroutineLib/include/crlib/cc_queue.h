#pragma once
#include <optional>
#include <atomic>
#include <memory>
#include <vector>
#include "cc_api.h"

namespace crlib {
    template<typename T>
    struct Concurrent_Queue_t {
    protected:
        size_t size;
        std::vector<std::optional<T>> data;
        std::atomic_size_t head;
        std::atomic_size_t tail;
        std::atomic_size_t amount;

    public:
        explicit Concurrent_Queue_t(size_t size) : size(size), data(std::nullopt, size), head(0), tail(0), amount(0) {

        }
        Concurrent_Queue_t(const Concurrent_Queue_t&) = delete;
        Concurrent_Queue_t& operator=(const Concurrent_Queue_t&) = delete;

        bool push(T val) {
            size_t t;
            size_t next;
            do {
                t = tail.load();
                next = t + 1;
                if (next == size) {
                    next  = 0;
                }
            } while(!tail.compare_exchange_strong(t, next));
        }
    };
}