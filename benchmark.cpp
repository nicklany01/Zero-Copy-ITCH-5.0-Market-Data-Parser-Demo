#include "itch_parser.hpp"
#include "spsc_queue.hpp"
#include <benchmark/benchmark.h>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

constexpr size_t QUEUE_CAPACITY = 1024;

void pin_thread_to_core(int core_id) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(core_id, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

static std::unique_ptr<SpscQueue<ParsedMessage, QUEUE_CAPACITY>> g_spsc;

static void *g_file_ptr = nullptr;
static size_t g_file_size = 0;

static void setup_file() {
  if (g_file_ptr) return;
  int fd = open("test_data.itch", O_RDONLY);
  if (fd == -1) exit(1);
  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    close(fd);
    exit(1);
  }
  g_file_size = sb.st_size;
  g_file_ptr = mmap(nullptr, g_file_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
}

template <typename BaseQueue> struct SpinQueueAdapter {
  BaseQueue *q;
  SpinQueueAdapter(BaseQueue *q) : q(q) {}
  void push(const ParsedMessage &msg) {
    while (!q->push(msg)) {
    }
  }
};

static void BM_ITCHParser_ZeroCopy(benchmark::State &state) {
  setup_file();
  if (state.thread_index() == 0) {
    pin_thread_to_core(2);
    g_spsc = std::make_unique<SpscQueue<ParsedMessage, QUEUE_CAPACITY>>();
    ItchParser parser;
    SpinQueueAdapter<SpscQueue<ParsedMessage, QUEUE_CAPACITY>> adapter(g_spsc.get());

    const uint8_t *curr = static_cast<const uint8_t *>(g_file_ptr);
    const uint8_t *end = curr + g_file_size;

    for (auto _ : state) {
      if (curr + 2 > end) {
        curr = static_cast<const uint8_t *>(g_file_ptr);
      }
      uint16_t msg_len;
      __builtin_memcpy(&msg_len, curr, sizeof(uint16_t));
      msg_len = __builtin_bswap16(msg_len);
      if (curr + 2 + msg_len > end) {
        curr = static_cast<const uint8_t *>(g_file_ptr);
        continue;
      }
      char type = static_cast<char>(curr[2]);
      const void *payload_ptr = curr + 3;
      size_t payload_size = msg_len - 1;
      
      parser.parse_order(payload_ptr, payload_size, type, adapter);
      curr += 2 + msg_len;
    }
    state.SetItemsProcessed(state.iterations());
  } else if (state.thread_index() == 1) {
    pin_thread_to_core(3);
    ParsedMessage val;
    for (auto _ : state) {
      while (!g_spsc || !g_spsc->pop(val)) {
      }
      benchmark::DoNotOptimize(val);
    }
  }
}

static void BM_ITCHParser_NonZeroCopy(benchmark::State &state) {
  if (state.thread_index() == 0) {
    pin_thread_to_core(2);
    g_spsc = std::make_unique<SpscQueue<ParsedMessage, QUEUE_CAPACITY>>();
    ItchParser parser;
    SpinQueueAdapter<SpscQueue<ParsedMessage, QUEUE_CAPACITY>> adapter(g_spsc.get());

    int fd = open("test_data.itch", O_RDONLY);
    if (fd == -1) exit(1);
    uint8_t buffer[2048];

    for (auto _ : state) {
      uint16_t msg_len;
      ssize_t bytes_read = read(fd, &msg_len, sizeof(uint16_t));
      if (bytes_read <= 0 || bytes_read < (ssize_t)sizeof(uint16_t)) {
        lseek(fd, 0, SEEK_SET);
        read(fd, &msg_len, sizeof(uint16_t));
      }
      msg_len = __builtin_bswap16(msg_len);
      
      read(fd, buffer, msg_len);
      char type = static_cast<char>(buffer[0]);
      const void *payload_ptr = buffer + 1;
      size_t payload_size = msg_len - 1;
      
      parser.parse_order(payload_ptr, payload_size, type, adapter);
    }
    close(fd);
    state.SetItemsProcessed(state.iterations());
  } else if (state.thread_index() == 1) {
    pin_thread_to_core(3);
    ParsedMessage val;
    for (auto _ : state) {
      while (!g_spsc || !g_spsc->pop(val)) {
      }
      benchmark::DoNotOptimize(val);
    }
  }
}

BENCHMARK(BM_ITCHParser_ZeroCopy)->Threads(2)->UseRealTime();
BENCHMARK(BM_ITCHParser_NonZeroCopy)->Threads(2)->UseRealTime();

BENCHMARK_MAIN();
