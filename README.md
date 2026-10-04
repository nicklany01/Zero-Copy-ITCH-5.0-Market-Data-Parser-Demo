# Zero-Copy ITCH 5.0 Market Data Parser

This project is a high-performance, zero-copy parser for NASDAQ ITCH 5.0 binary market data packets written in C++20.

## Project Goals

- Parse binary exchange packets directly from a memory-mapped file or network buffer.
- Stream parsed messages into an SPSC Queue with zero memory copies.
- Convert network byte order to host byte order using compiler intrinsics.

## Technical Specifications

- **Parsing Latency:** Target under 20 nanoseconds per message.
- **Throughput:** Target over 30 million messages/sec.
- **Memory Optimizations:**
  - Zero dynamic heap allocations during parsing.
  - Direct casting of byte buffers to packed C structs.
- **Data Ingestion:** Uses `mmap(..., MAP_SHARED)` for raw dump file processing.

## Build and Run

To build the project and run the provided test harness:

```bash
make
./build/main
```

## Benchmarks

Run the benchmark suite and `perf` profile via:

```bash
make run-benchmark
```