#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include "ring_buffer.h"

// ─────────────────────────────────────────────────────────────────────────────
// Unit tests for ThreadSafeRingBuffer
//
// Structure: TEST(SuiteName, TestName)
//   - Each test is fully independent
//   - EXPECT_* continues the test on failure
//   - ASSERT_* stops the test immediately on failure
// ─────────────────────────────────────────────────────────────────────────────


// ── Basic behaviour ───────────────────────────────────────────────────────────

TEST(RingBufferTest, StartsEmpty) {
    ThreadSafeRingBuffer<int, 4> buf;
    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(buf.size(), 0u);
}

TEST(RingBufferTest, CapacityIsCorrect) {
    ThreadSafeRingBuffer<int, 8> buf;
    EXPECT_EQ(buf.capacity(), 8u);
}

TEST(RingBufferTest, PushAndPopSingleItem) {
    ThreadSafeRingBuffer<int, 4> buf;
    EXPECT_TRUE(buf.push(42));

    auto result = buf.pop();
    ASSERT_TRUE(result.has_value());    // ASSERT: stop test if no value
    EXPECT_EQ(result.value(), 42);
}

TEST(RingBufferTest, FIFOOrder) {
    // First in, first out
    ThreadSafeRingBuffer<int, 4> buf;
    buf.push(10);
    buf.push(20);
    buf.push(30);

    EXPECT_EQ(buf.pop().value(), 10);
    EXPECT_EQ(buf.pop().value(), 20);
    EXPECT_EQ(buf.pop().value(), 30);
}


// ── Boundary conditions ───────────────────────────────────────────────────────

TEST(RingBufferTest, ReturnsFalseWhenFull) {
    ThreadSafeRingBuffer<int, 2> buf;
    EXPECT_TRUE(buf.push(1));
    EXPECT_TRUE(buf.push(2));
    EXPECT_FALSE(buf.push(3));  // Buffer is full
    EXPECT_TRUE(buf.full());
}

TEST(RingBufferTest, ReturnsNulloptWhenEmpty) {
    ThreadSafeRingBuffer<int, 4> buf;
    auto result = buf.pop();
    EXPECT_FALSE(result.has_value());
}

TEST(RingBufferTest, PeekDoesNotRemoveItem) {
    ThreadSafeRingBuffer<int, 4> buf;
    buf.push(99);

    auto peeked = buf.peek();
    ASSERT_TRUE(peeked.has_value());
    EXPECT_EQ(peeked.value(), 99);
    EXPECT_EQ(buf.size(), 1u);  // Item still in buffer after peek
}

TEST(RingBufferTest, FillDrainThenFillAgain) {
    // Tests the circular wrap-around — the most important ring buffer behaviour
    ThreadSafeRingBuffer<int, 3> buf;

    buf.push(1); buf.push(2); buf.push(3); // Fill
    buf.pop();   buf.pop();                 // Drain partially — head wraps around

    buf.push(4); buf.push(5);              // Fill again, using wrapped positions

    EXPECT_EQ(buf.pop().value(), 3);
    EXPECT_EQ(buf.pop().value(), 4);
    EXPECT_EQ(buf.pop().value(), 5);
    EXPECT_TRUE(buf.empty());
}

TEST(RingBufferTest, WorksWithStrings) {
    // Templates — works with any type, not just int
    ThreadSafeRingBuffer<std::string, 3> buf;
    buf.push("hello");
    buf.push("world");
    EXPECT_EQ(buf.pop().value(), "hello");
    EXPECT_EQ(buf.pop().value(), "world");
}


// ── Thread safety ─────────────────────────────────────────────────────────────

TEST(RingBufferTest, ConcurrentPushesDoNotCorrupt) {
    // 4 threads each push 25 items = 100 total
    // If the mutex is broken, size() will be wrong or it will crash
    ThreadSafeRingBuffer<int, 100> buf;
    std::vector<std::thread> threads;

    for (int t = 0; t < 4; ++t) {
        threads.emplace_back([&buf]() {
            for (int i = 0; i < 25; ++i) {
                buf.push(i);
            }
        });
    }

    for (auto& t : threads) t.join();

    EXPECT_EQ(buf.size(), 100u);
    EXPECT_TRUE(buf.full());
}

TEST(RingBufferTest, ConcurrentPushAndPop) {
    // Producer pushes 50 items, consumer pops 50 items — no crash, no data loss
    ThreadSafeRingBuffer<int, 10> buf;
    int popped_count = 0;

    std::thread producer([&buf]() {
        for (int i = 0; i < 50; ++i) {
            while (!buf.push(i)) {
                std::this_thread::yield(); // Wait if full
            }
        }
    });

    std::thread consumer([&buf, &popped_count]() {
        for (int i = 0; i < 50; ++i) {
            std::optional<int> val;
            while (!(val = buf.pop()).has_value()) {
                std::this_thread::yield(); // Wait if empty
            }
            ++popped_count;
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(popped_count, 50);
}
