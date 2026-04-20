#include <iostream>
#include <thread>
#include <chrono>
#include "ring_buffer.h"

// ─────────────────────────────────────────────────────────────────────────────
// Demo: one producer thread + one consumer thread sharing a ring buffer
//
// This is a classic embedded/networking pattern:
// - Producer generates data (e.g. incoming network packets)
// - Consumer processes it at its own pace
// - Ring buffer decouples them safely
// ─────────────────────────────────────────────────────────────────────────────

int main() {
    // Shared buffer — capacity 5, stores integers
    ThreadSafeRingBuffer<int, 5> buffer;

    // Producer thread: pushes values 0-7
    std::thread producer([&buffer]() {
        for (int i = 0; i < 8; ++i) {
            if (buffer.push(i)) {
                std::cout << "[Producer] Pushed:  " << i << "\n";
            } else {
                std::cout << "[Producer] FULL — dropped: " << i << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    // Consumer thread: pops values at a slower rate
    std::thread consumer([&buffer]() {
        for (int i = 0; i < 8; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(80));
            auto val = buffer.pop();
            if (val.has_value()) {
                std::cout << "[Consumer] Popped:  " << val.value() << "\n";
            } else {
                std::cout << "[Consumer] EMPTY\n";
            }
        }
    });

    producer.join();
    consumer.join();

    std::cout << "\nFinal buffer size: " << buffer.size() << "\n";
    return 0;
}
