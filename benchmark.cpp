#include <benchmark/benchmark.h>
#include "itch_parser.hpp"

// TODO: Implement benchmarking logic to measure parse latency and throughput
static void BM_ITCHParser(benchmark::State& state) {
    // Setup memory-mapped dummy buffer here
    
    for (auto _ : state) {
        // Run parser loop
    }
}

BENCHMARK(BM_ITCHParser);

BENCHMARK_MAIN();
