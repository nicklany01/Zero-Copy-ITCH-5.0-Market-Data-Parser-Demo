# Zero-Copy ITCH 5.0 Market Data Parser

This project is a high-performance, zero-copy parser for NASDAQ ITCH 5.0 binary
market data packets written in C++20.

## Project Goals

- Parse binary exchange packets directly from a memory-mapped file or network
  buffer.
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

### Benchmark Results (Zero-Copy vs. Non-Zero-Copy)

Running the parser on a test dataset yields a massive performance difference
between mapping the file directly into memory (Zero-Copy) versus executing
`read()` system calls to copy the data into a local buffer first
(Non-Zero-Copy).

```text
=== Profiling Zero-Copy Parser ===
Run on (16 X 4853.59 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 32768 KiB (x1)
-----------------------------------------------------------------------------------------------------
Benchmark                                           Time             CPU   Iterations UserCounters...
-----------------------------------------------------------------------------------------------------
BM_ITCHParser_ZeroCopy/real_time/threads:2       40.4 ns         40.3 ns     16540688 items_per_second=12.3865M/s

 Performance counter stats for './build/benchmark --benchmark_filter=BM_ITCHParser_ZeroCopy':

        71,299,661      L1-dcache-load-misses                                                 
        47,194,422      cache-misses                                                          
                39      context-switches                                                      

=== Profiling Non-Zero-Copy Parser ===
Run on (16 X 4853.59 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 32768 KiB (x1)
--------------------------------------------------------------------------------------------------------
Benchmark                                              Time             CPU   Iterations UserCounters...
--------------------------------------------------------------------------------------------------------
BM_ITCHParser_NonZeroCopy/real_time/threads:2       2511 ns         2503 ns       277756 items_per_second=199.15k/s

 Performance counter stats for './build/benchmark --benchmark_filter=BM_ITCHParser_NonZeroCopy':

        43,923,442      L1-dcache-load-misses                                                 
         1,949,803      cache-misses                                                          
                27      context-switches
```

**Conclusion:** The zero-copy architecture (12.4+ Million messages per second)
is roughly 62x faster than standard file stream parsing (200k messages per
second). By bypassing the kernel-to-user memory copy and forcing explicit
`MOVBE` byte-swap instructions via static casts, latency drops from 2500ns per
message down to just 40ns per message end-to-end.

