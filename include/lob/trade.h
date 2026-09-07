#ifndef LOB_INCLUDE_LOB_TRADE_H_
#define LOB_INCLUDE_LOB_TRADE_H_

#include <cstdint>
#include <vector>

namespace lob {

struct Trade {
  std::uint64_t resting_order_id;
  std::uint64_t incoming_order_id;
  std::uint64_t price_ticks;
  std::uint32_t quantity;
};

struct AddOrderResult {
  std::uint64_t order_id;
  std::vector<Trade> trades;
  bool rested;
};

enum class AddOrderError {
  kPoolExhausted,
};

struct AddOrderFailure {
  AddOrderError error;
  std::vector<Trade> trades;
};

}  // namespace lob

#endif  // LOB_INCLUDE_LOB_TRADE_H_