# Thread-Safe Ring Buffer in C++17

![Build and Test](https://github.com/YOUR_USERNAME/cpp-ring-buffer/actions/workflows/build.yml/badge.svg)

A fixed-size, thread-safe circular (ring) buffer implemented in modern C++17.

Built to practise real-time data structure patterns from embedded networking experience — similar patterns to those used in high-frequency network event queues in telecom systems.

---

## What it does

- Fixed-size FIFO queue with compile-time capacity
- Thread-safe push, pop, and peek using `std::mutex` and RAII lock guards
- No heap allocation — uses `std::array` for embedded-friendly memory
- Returns `std::optional<T>` instead of exceptions for empty/full states

---

## C++ features demonstrated

| Feature | Where |
|---|---|
| RAII (lock_guard) | `ring_buffer.h` — automatic mutex unlock |
| `std::optional` | Pop/peek return values |
| Templates | `ThreadSafeRingBuffer<T, Capacity>` |
| `std::array` | Fixed-size, stack-allocated storage |
| `std::thread` / `std::mutex` | Thread-safe access + demo in main.cpp |
| Google Test (gtest) | `tests/test_ring_buffer.cpp` |
| CMake + FetchContent | `CMakeLists.txt` |
| GitHub Actions CI/CD | `.github/workflows/build.yml` |

---

## Build & Run

**Requirements:** CMake 3.14+, C++17 compiler (GCC 9+ or Clang 9+)

```bash
# Clone the repo
git clone https://github.com/soundarya-sivasankar/cpp-ring-buffer.git
cd cpp-ring-buffer

# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build --parallel

# Run the demo
./build/main

# Run all tests
ctest --test-dir build --output-on-failure
```

---

## Project Structure

```
cpp-ring-buffer/
├── CMakeLists.txt              # Build system
├── README.md
├── include/
│   └── ring_buffer.h           # ThreadSafeRingBuffer implementation
├── src/
│   └── main.cpp                # Producer/consumer demo
├── tests/
│   └── test_ring_buffer.cpp    # Google Test unit tests
└── .github/
    └── workflows/
        └── build.yml           # CI/CD — builds and tests on every push
```

---

## About

Built by Soundarya Sivasankar — Senior Software Engineer with 6+ years of C++
experience in mission-critical embedded systems (Nokia 1830 PSS optical platform).
Based in Eindhoven, Netherlands. Open to embedded software engineering roles.

[LinkedIn](https://www.linkedin.com/in/soundarya-sivasankar)
