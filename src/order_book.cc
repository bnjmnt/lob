#include "lob/order_book.h"

#include <cstdint>
#include <expected>
#include <memory>
#include <optional>

#include "lob/order.h"
#include "lob/order_pool.h"
#include "lob/price_level.h"

namespace lob {

OrderBook::OrderBook(std::uint64_t max_orders) : pool_(max_orders) {}

std::expected<std::uint64_t, AddOrderError> OrderBook::AddOrder(
    std::uint64_t price_ticks, std::uint32_t quantity, Side side) {
  if (!pool_.free_count()) {
    return std::unexpected(AddOrderError::kPoolExhausted);
  }

  Order* order = pool_.Acquire();
  order->id = NextOrderId();
  order->price_ticks = price_ticks;
  order->remaining_quantity = quantity;
  order->side = side;

  if (side == Side::kAsk) {
    auto [it, is_inserted] = asks_.try_emplace(price_ticks, nullptr);
    if (is_inserted) {
      it->second = std::make_unique<PriceLevel>(price_ticks);
    }
    it->second->PushBack(order);
  } else {
    auto [it, is_inserted] = bids_.try_emplace(price_ticks, nullptr);
    if (is_inserted) {
      it->second = std::make_unique<PriceLevel>(price_ticks);
    }
    it->second->PushBack(order);
  }

  order_lookup_.emplace(order->id, order);

  return order->id;
}

bool OrderBook::CancelOrder(std::uint64_t order_id) {
  auto it = order_lookup_.find(order_id);
  if (it == order_lookup_.end()) {
    return false;
  }
  Order* order = it->second;
  bool is_level_empty = order->level->Remove(order);
  if (is_level_empty) {
    if (order->side == Side::kAsk) {
      asks_.erase(order->price_ticks);
    } else {
      bids_.erase(order->price_ticks);
    }
  }
  order_lookup_.erase(it);
  pool_.Release(order);
  return true;
}

std::optional<std::uint64_t> OrderBook::BestBid() const {
  if (bids_.empty()) {
    return std::nullopt;
  }
  return bids_.begin()->first;
}

std::optional<std::uint64_t> OrderBook::BestAsk() const {
  if (asks_.empty()) {
    return std::nullopt;
  }
  return asks_.begin()->first;
}

std::uint64_t OrderBook::NextOrderId() { return next_order_id_++; }

}  // namespace lob