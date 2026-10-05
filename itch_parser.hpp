#pragma once

#include "spsc_queue.hpp"
#include <cstddef>
#include <cstdint>

#pragma pack(push, 1)

// 1.3.1 Add Order
struct MsgAddOrder {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint8_t timestamp[6]; // 48-bit nanoseconds since midnight
  uint64_t order_reference;
  char buy_sell_indicator; // 'B' = Buy, 'S' = Sell
  uint32_t shares;
  char stock[8];  // Space-padded ASCII
  uint32_t price; // 4 implied decimals
};
static_assert(sizeof(MsgAddOrder) == 35, "MsgAddOrder size must be 35 bytes");

// 1.3.2 Add Order with MPID Attribution
struct MsgAddOrderMPID {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint8_t timestamp[6];
  uint64_t order_reference;
  char buy_sell_indicator; // 'B' = Buy, 'S' = Sell
  uint32_t shares;
  char stock[8];       // Space-padded ASCII
  uint32_t price;      // 4 implied decimals
  char attribution[4]; // Market Participant Identifier
};
static_assert(sizeof(MsgAddOrderMPID) == 39, "MsgAddOrderMPID size must be 39");

// 1.4.1 Order Executed Message
struct MsgOrderExecuted {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint8_t timestamp[6];
  uint64_t order_reference;
  uint32_t executed_shares;
  uint64_t match_number;
};
static_assert(sizeof(MsgOrderExecuted) == 30, "MsgOrderExecuted size must be 30 bytes");

// 1.4.2 Order Executed With Price Message
struct MsgOrderExecutedWithPrice {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint8_t timestamp[6];
  uint64_t order_reference;
  uint32_t executed_shares;
  uint64_t match_number;
  char printable;           // 'Y' = Printable, 'N' = Non-Printable
  uint32_t execution_price; // 4 implied decimals
};
static_assert(sizeof(MsgOrderExecutedWithPrice) == 35, "MsgOrderExecutedWithPrice size must be 35 bytes");

// 1.4.3 Order Cancel Message
struct MsgOrderCancel {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint8_t timestamp[6];
  uint64_t order_reference;
  uint32_t canceled_shares;
};
static_assert(sizeof(MsgOrderCancel) == 22, "MsgOrderCancel size must be 22 bytes");

// 1.4.4 Order Delete Message
struct MsgOrderDelete {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint8_t timestamp[6];
  uint64_t order_reference;
};
static_assert(sizeof(MsgOrderDelete) == 18, "MsgOrderDelete size must be 18 bytes");

// 1.4.5 Order Replace Message
struct MsgOrderReplace {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint8_t timestamp[6];
  uint64_t original_order_reference;
  uint64_t new_order_reference;
  uint32_t shares;
  uint32_t price; // 4 implied decimals
};
static_assert(sizeof(MsgOrderReplace) == 34, "MsgOrderReplace size must be 34 bytes");

struct ParsedMessage {
  char type;

  union {
    MsgAddOrder add;
    MsgAddOrderMPID add_mpid;
    MsgOrderExecuted executed;
    MsgOrderExecutedWithPrice exec_price;
    MsgOrderCancel cancel;
    MsgOrderDelete del;
    MsgOrderReplace replace;
  } payload;
};

static_assert(sizeof(ParsedMessage) <= 64, "Must fit in a single 64-byte cache line!");

#pragma pack(pop)

inline void process_fields(const void *payload_ptr, size_t size, MsgAddOrder &dst) {
  if (size < sizeof(MsgAddOrder))
    return;
  const auto *src = static_cast<const MsgAddOrder *>(payload_ptr);
  dst.stock_locate = __builtin_bswap16(src->stock_locate);
  dst.tracking_number = __builtin_bswap16(src->tracking_number);
  __builtin_memcpy(dst.timestamp, src->timestamp, 6);
  dst.order_reference = __builtin_bswap64(src->order_reference);
  dst.buy_sell_indicator = src->buy_sell_indicator;
  dst.shares = __builtin_bswap32(src->shares);
  __builtin_memcpy(dst.stock, src->stock, 8);
  dst.price = __builtin_bswap32(src->price);
}

inline void process_fields(const void *payload_ptr, size_t size, MsgAddOrderMPID &dst) {
  if (size < sizeof(MsgAddOrderMPID))
    return;
  const auto *src = static_cast<const MsgAddOrderMPID *>(payload_ptr);
  dst.stock_locate = __builtin_bswap16(src->stock_locate);
  dst.tracking_number = __builtin_bswap16(src->tracking_number);
  __builtin_memcpy(dst.timestamp, src->timestamp, 6);
  dst.order_reference = __builtin_bswap64(src->order_reference);
  dst.buy_sell_indicator = src->buy_sell_indicator;
  dst.shares = __builtin_bswap32(src->shares);
  __builtin_memcpy(dst.stock, src->stock, 8);
  dst.price = __builtin_bswap32(src->price);
  __builtin_memcpy(dst.attribution, src->attribution, 4);
}

inline void process_fields(const void *payload_ptr, size_t size, MsgOrderExecuted &dst) {
  if (size < sizeof(MsgOrderExecuted))
    return;
  const auto *src = static_cast<const MsgOrderExecuted *>(payload_ptr);
  dst.stock_locate = __builtin_bswap16(src->stock_locate);
  dst.tracking_number = __builtin_bswap16(src->tracking_number);
  __builtin_memcpy(dst.timestamp, src->timestamp, 6);
  dst.order_reference = __builtin_bswap64(src->order_reference);
  dst.executed_shares = __builtin_bswap32(src->executed_shares);
  dst.match_number = __builtin_bswap64(src->match_number);
}

inline void process_fields(const void *payload_ptr, size_t size, MsgOrderExecutedWithPrice &dst) {
  if (size < sizeof(MsgOrderExecutedWithPrice))
    return;
  const auto *src = static_cast<const MsgOrderExecutedWithPrice *>(payload_ptr);
  dst.stock_locate = __builtin_bswap16(src->stock_locate);
  dst.tracking_number = __builtin_bswap16(src->tracking_number);
  __builtin_memcpy(dst.timestamp, src->timestamp, 6);
  dst.order_reference = __builtin_bswap64(src->order_reference);
  dst.executed_shares = __builtin_bswap32(src->executed_shares);
  dst.match_number = __builtin_bswap64(src->match_number);
  dst.printable = src->printable;
  dst.execution_price = __builtin_bswap32(src->execution_price);
}

inline void process_fields(const void *payload_ptr, size_t size, MsgOrderCancel &dst) {
  if (size < sizeof(MsgOrderCancel))
    return;
  const auto *src = static_cast<const MsgOrderCancel *>(payload_ptr);
  dst.stock_locate = __builtin_bswap16(src->stock_locate);
  dst.tracking_number = __builtin_bswap16(src->tracking_number);
  __builtin_memcpy(dst.timestamp, src->timestamp, 6);
  dst.order_reference = __builtin_bswap64(src->order_reference);
  dst.canceled_shares = __builtin_bswap32(src->canceled_shares);
}

inline void process_fields(const void *payload_ptr, size_t size, MsgOrderDelete &dst) {
  if (size < sizeof(MsgOrderDelete))
    return;
  const auto *src = static_cast<const MsgOrderDelete *>(payload_ptr);
  dst.stock_locate = __builtin_bswap16(src->stock_locate);
  dst.tracking_number = __builtin_bswap16(src->tracking_number);
  __builtin_memcpy(dst.timestamp, src->timestamp, 6);
  dst.order_reference = __builtin_bswap64(src->order_reference);
}

inline void process_fields(const void *payload_ptr, size_t size, MsgOrderReplace &dst) {
  if (size < sizeof(MsgOrderReplace))
    return;
  const auto *src = static_cast<const MsgOrderReplace *>(payload_ptr);
  dst.stock_locate = __builtin_bswap16(src->stock_locate);
  dst.tracking_number = __builtin_bswap16(src->tracking_number);
  __builtin_memcpy(dst.timestamp, src->timestamp, 6);
  dst.original_order_reference = __builtin_bswap64(src->original_order_reference);
  dst.new_order_reference = __builtin_bswap64(src->new_order_reference);
  dst.shares = __builtin_bswap32(src->shares);
  dst.price = __builtin_bswap32(src->price);
}

class ItchParser {
public:
  ItchParser() {}

  template <typename QueueType>
  void parse_order(const void *payload_ptr, size_t size, char type, QueueType &q) {
    if (size == 0)
      return;

    ParsedMessage event;
    event.type = type;

    switch (type) {
    case 'A':
      process_fields(payload_ptr, size, event.payload.add);
      break;
    case 'F':
      process_fields(payload_ptr, size, event.payload.add_mpid);
      break;
    case 'E':
      process_fields(payload_ptr, size, event.payload.executed);
      break;
    case 'C':
      process_fields(payload_ptr, size, event.payload.exec_price);
      break;
    case 'X':
      process_fields(payload_ptr, size, event.payload.cancel);
      break;
    case 'D':
      process_fields(payload_ptr, size, event.payload.del);
      break;
    case 'U':
      process_fields(payload_ptr, size, event.payload.replace);
      break;
    default:
      // Unhandled or unknown message type
      return;
    }

    q.push(event);
  }
};
