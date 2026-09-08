#ifndef LOB_MATCHING_ENGINE_INCLUDE_LOB_ORDER_H_
#define LOB_MATCHING_ENGINE_INCLUDE_LOB_ORDER_H_

#include <cstdint>

namespace lob {

class PriceLevel;

enum class Side : uint8_t {
  kBid,
  kAsk,
};

struct Order {
  std::uint64_t id = 0;
  std::uint64_t price_ticks = 0;
  Order* next = nullptr;
  Order* prev = nullptr;
  PriceLevel* level = nullptr;
  std::uint32_t remaining_quantity = 0;
  Side side = Side::kBid;
};

}  // namespace lob

#endif  // LOB_MATCHING_ENGINE_INCLUDE_LOB_ORDER_H_
