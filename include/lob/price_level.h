#ifndef LOB_INCLUDE_LOB_PRICE_LEVEL_H_
#define LOB_INCLUDE_LOB_PRICE_LEVEL_H_

#include <cstdint>

#include "lob/order.h"

namespace lob {

class PriceLevel {
 public:
  explicit PriceLevel(std::uint64_t price_ticks);

  void PushBack(Order* order);
  Order* PopFront();
  bool Remove(Order* order);

  bool empty() const;
  std::uint64_t price_ticks() const;
  std::uint64_t total_quantity() const;

 private:
  std::uint64_t price_ticks_;
  std::uint64_t total_quantity_;
  Order* head_;
  Order* tail_;
};

}  // namespace lob

#endif  // LOB_INCLUDE_LOB_PRICE_LEVEL_H_
