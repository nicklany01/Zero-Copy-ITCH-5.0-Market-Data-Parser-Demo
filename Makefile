CXX = g++
CXXFLAGS_BASE = -std=c++20 -Wall -Wextra -Wpedantic -pthread

RELEASE_FLAGS = -O3 -march=native -DNDEBUG

TSAN_FLAGS = -O1 -g -fsanitize=thread

ASAN_FLAGS = -O1 -g -fsanitize=address,undefined

.PHONY: all release tsan asan benchmark run-benchmark clean

all: release tsan asan

release:
	mkdir -p build
	$(CXX) $(CXXFLAGS_BASE) $(RELEASE_FLAGS) -o build/main main.cpp

tsan:
	mkdir -p build
	$(CXX) $(CXXFLAGS_BASE) $(TSAN_FLAGS) -o build/main_tsan main.cpp

asan:
	mkdir -p build
	$(CXX) $(CXXFLAGS_BASE) $(ASAN_FLAGS) -o build/main_asan main.cpp

benchmark:
	mkdir -p build
	$(CXX) $(CXXFLAGS_BASE) $(RELEASE_FLAGS) -o build/benchmark benchmark.cpp -lbenchmark -lbenchmark_main

run-benchmark: benchmark
	sudo perf stat -e L1-dcache-load-misses,cache-misses,context-switches ./build/benchmark --benchmark_filter=BM_ITCHParser

clean:
	rm -rf build/
