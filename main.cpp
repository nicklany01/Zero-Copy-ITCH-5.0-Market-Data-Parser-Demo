#include "itch_parser.hpp"
#include "spsc_queue.hpp"
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <new>

bool g_parsing = false;

// Overload global operator new to prevent heap allocations during parsing
void* operator new(std::size_t size) {
    if (g_parsing) {
        std::cerr << "CRITICAL ERROR: Unexpected heap allocation of " << size << " bytes during parsing!\n";
        std::abort();
    }
    void* p = std::malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}

void operator delete(void* ptr) noexcept {
    std::free(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept {
    std::free(ptr);
}

int main() {
  int fd = open("test_data.itch", O_RDONLY);
  if (fd == -1) {
    std::cerr << "Failed to open test_data.itch\n";
    return 1;
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    std::cerr << "Failed to get file stats\n";
    close(fd);
    return 1;
  }

  void *ptr = mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd); // It is safe to close the fd after mmap

  if (ptr == MAP_FAILED) {
    std::cerr << "Failed to mmap file\n";
    return 1;
  }

  SpscQueue<ParsedMessage, 1024> q;
  ItchParser parser;

  const uint8_t *curr = static_cast<const uint8_t *>(ptr);
  const uint8_t *end = curr + sb.st_size;

  // Make sure we have at least 2 bytes left to read the length
  g_parsing = true; // Enable heap allocation detection
  while (curr + 2 <= end) {
    uint16_t msg_len;
    __builtin_memcpy(&msg_len, curr, sizeof(uint16_t));
    msg_len = __builtin_bswap16(msg_len);
    curr += 2;

    // Check if the remaining file size is at least as large as the message
    if (curr + msg_len > end) {
      std::cerr << "Warning: Truncated message at end of file\n";
      break;
    }

    char type = static_cast<char>(*curr);
    const void *payload_ptr = curr + 1;
    size_t payload_size = msg_len - 1;

    parser.parse_order(payload_ptr, payload_size, type, q);

    curr += msg_len;
  }
  g_parsing = false; // Disable heap allocation detection

  munmap(ptr, sb.st_size);
  return 0;
}
