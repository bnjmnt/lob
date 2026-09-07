#ifndef LOB_INCLUDE_LOB_ORDER_BOOK_H_
#define LOB_INCLUDE_LOB_ORDER_BOOK_H_

#include <cstdint>
#include <expected>
#include <flat_map>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>

#include "lob/order.h"
#include "lob/order_pool.h"
#include "lob/price_level.h"

namespace lob {

enum class AddOrderError {
  kPoolExhausted,
};

class OrderBook {
 public:
  explicit OrderBook(std::uint64_t max_orders);

  std::expected<std::uint64_t, AddOrderError> AddOrder(
      std::uint64_t price_ticks, std::uint32_t quantity, Side side);
  bool CancelOrder(std::uint64_t order_id);

  std::optional<std::uint64_t> BestBid() const;
  std::optional<std::uint64_t> BestAsk() const;

 private:
  std::uint64_t NextOrderId();

  std::flat_map<std::uint64_t, std::unique_ptr<PriceLevel>, std::greater<>>
      bids_;
  std::flat_map<std::uint64_t, std::unique_ptr<PriceLevel>> asks_;
  std::unordered_map<std::uint64_t, Order*> order_lookup_;
  OrderPool pool_;
  std::uint64_t next_order_id_ = 1;
};

}  // namespace lob

#endif  // LOB_INCLUDE_LOB_ORDER_BOOK_H_