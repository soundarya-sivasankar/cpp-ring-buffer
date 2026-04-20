#pragma once

#include <array>
#include <mutex>
#include <optional>

// ─────────────────────────────────────────────────────────────────────────────
// ThreadSafeRingBuffer<T, Capacity>
//
// A fixed-size, thread-safe circular buffer.
//   T        : type of elements stored (e.g. int, float, custom struct)
//   Capacity : max number of elements — set at compile time
//
// Design choices:
//   - std::mutex + std::lock_guard  → thread safety via RAII (no manual unlock)
//   - std::optional return values   → no exceptions for normal empty/full cases
//   - std::array                    → fixed stack allocation, no heap (good for embedded)
//   - Template                      → works with any type
// ─────────────────────────────────────────────────────────────────────────────

template <typename T, std::size_t Capacity>
class ThreadSafeRingBuffer {
public:

    // ── Constructors ──────────────────────────────────────────────────────────

    ThreadSafeRingBuffer() = default;

    // Disallow copying — std::mutex cannot be copied
    ThreadSafeRingBuffer(const ThreadSafeRingBuffer&)            = delete;
    ThreadSafeRingBuffer& operator=(const ThreadSafeRingBuffer&) = delete;


    // ── Public API ────────────────────────────────────────────────────────────

    // Push an item into the buffer.
    // Returns true on success, false if the buffer is full.
    bool push(const T& item) {
        // lock_guard acquires the mutex here.
        // When 'lock' goes out of scope (end of function), mutex is released
        // automatically — this is RAII. No manual unlock. No risk of forgetting.
        std::lock_guard<std::mutex> lock(mutex_);

        if (is_full()) {
            return false;   // Buffer full — caller decides what to do
        }

        buffer_[head_] = item;
        head_ = next_index(head_);
        ++size_;
        return true;
    }

    // Pop the oldest item from the buffer.
    // Returns std::nullopt if the buffer is empty.
    std::optional<T> pop() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (is_empty()) {
            return std::nullopt;    // Nothing to return
        }

        T item = buffer_[tail_];
        tail_ = next_index(tail_);
        --size_;
        return item;
    }

    // Peek at the oldest item without removing it.
    std::optional<T> peek() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (is_empty()) return std::nullopt;
        return buffer_[tail_];
    }

    // Thread-safe queries
    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return is_empty();
    }

    bool full() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return is_full();
    }

    // constexpr — evaluated at compile time, no runtime cost
    static constexpr std::size_t capacity() {
        return Capacity;
    }


private:
    // ── Internal helpers ──────────────────────────────────────────────────────
    // These are called only when the mutex is already held by the caller

    bool is_empty() const { return size_ == 0; }
    bool is_full()  const { return size_ == Capacity; }

    // Wraps index back to 0 when it reaches Capacity
    // This is what makes it "circular"
    std::size_t next_index(std::size_t idx) const {
        return (idx + 1) % Capacity;
    }


    // ── Member variables ──────────────────────────────────────────────────────

    std::array<T, Capacity> buffer_{};  // Fixed-size storage on the stack
    std::size_t head_  = 0;             // Next write position
    std::size_t tail_  = 0;             // Next read position
    std::size_t size_  = 0;             // Current number of elements

    // mutable = allowed to be modified even in const methods (e.g. peek/size)
    mutable std::mutex mutex_;
};
