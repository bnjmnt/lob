#include "lob/price_level.h"

#include <gtest/gtest.h>

#include "lob/order.h"

namespace lob {
namespace {

TEST(PriceLevelTest, ConstructorSetsPriceAndStartsEmpty) {
  PriceLevel level(10050);

  EXPECT_EQ(level.price_ticks(), 10050u);
  EXPECT_EQ(level.total_quantity(), 0u);
  EXPECT_TRUE(level.empty());
}

TEST(PriceLevelTest, PushBackOnEmptyLevelIsNotEmpty) {
  PriceLevel level(100);
  Order order;
  order.id = 1;
  order.remaining_quantity = 50;

  level.PushBack(&order);

  EXPECT_FALSE(level.empty());
}

TEST(PriceLevelTest, PushBackAccumulatesTotalQuantity) {
  PriceLevel level(100);
  Order first;
  first.remaining_quantity = 50;
  Order second;
  second.remaining_quantity = 30;

  level.PushBack(&first);
  level.PushBack(&second);

  EXPECT_EQ(level.total_quantity(), 80u);
}

TEST(PriceLevelTest, PopFrontReturnsOrdersInFifoOrder) {
  PriceLevel level(100);
  Order first;
  first.id = 1;
  first.remaining_quantity = 10;
  Order second;
  second.id = 2;
  second.remaining_quantity = 20;
  Order third;
  third.id = 3;
  third.remaining_quantity = 30;

  level.PushBack(&first);
  level.PushBack(&second);
  level.PushBack(&third);

  EXPECT_EQ(level.PopFront(), &first);
  EXPECT_EQ(level.PopFront(), &second);
  EXPECT_EQ(level.PopFront(), &third);
}

TEST(PriceLevelTest, PopFrontDecrementsTotalQuantity) {
  PriceLevel level(100);
  Order first;
  first.remaining_quantity = 10;
  Order second;
  second.remaining_quantity = 20;
  level.PushBack(&first);
  level.PushBack(&second);

  level.PopFront();

  EXPECT_EQ(level.total_quantity(), 20u);
}

TEST(PriceLevelTest, PopFrontOnSingleElementLevelEmptiesLevel) {
  PriceLevel level(100);
  Order order;
  order.remaining_quantity = 10;
  level.PushBack(&order);

  Order* popped = level.PopFront();

  EXPECT_EQ(popped, &order);
  EXPECT_TRUE(level.empty());
  EXPECT_EQ(level.total_quantity(), 0u);
}

TEST(PriceLevelTest, PopFrontThenPushBackRelinksTailCorrectly) {
  // Regression test: after popping down to empty, tail_ must not be left
  // dangling, or the next PushBack corrupts the list.
  PriceLevel level(100);
  Order first;
  first.remaining_quantity = 10;
  level.PushBack(&first);
  level.PopFront();

  Order second;
  second.remaining_quantity = 15;
  level.PushBack(&second);

  EXPECT_FALSE(level.empty());
  EXPECT_EQ(level.PopFront(), &second);
}

TEST(PriceLevelTest, RemoveHeadRelinksRemainingOrders) {
  PriceLevel level(100);
  Order first;
  first.remaining_quantity = 10;
  Order second;
  second.remaining_quantity = 20;
  level.PushBack(&first);
  level.PushBack(&second);

  bool now_empty = level.Remove(&first);

  EXPECT_FALSE(now_empty);
  EXPECT_EQ(level.PopFront(), &second);
}

TEST(PriceLevelTest, RemoveTailRelinksRemainingOrders) {
  PriceLevel level(100);
  Order first;
  first.remaining_quantity = 10;
  Order second;
  second.remaining_quantity = 20;
  level.PushBack(&first);
  level.PushBack(&second);

  level.Remove(&second);

  EXPECT_EQ(level.PopFront(), &first);
  EXPECT_TRUE(level.empty());
}

TEST(PriceLevelTest, RemoveMiddleRelinksNeighbors) {
  PriceLevel level(100);
  Order first;
  first.remaining_quantity = 10;
  Order second;
  second.remaining_quantity = 20;
  Order third;
  third.remaining_quantity = 30;
  level.PushBack(&first);
  level.PushBack(&second);
  level.PushBack(&third);

  level.Remove(&second);

  EXPECT_EQ(level.PopFront(), &first);
  EXPECT_EQ(level.PopFront(), &third);
}

TEST(PriceLevelTest, RemoveOnlyOrderEmptiesLevel) {
  PriceLevel level(100);
  Order order;
  order.remaining_quantity = 10;
  level.PushBack(&order);

  bool now_empty = level.Remove(&order);

  EXPECT_TRUE(now_empty);
  EXPECT_TRUE(level.empty());
}

TEST(PriceLevelTest, RemoveDecrementsTotalQuantity) {
  PriceLevel level(100);
  Order first;
  first.remaining_quantity = 10;
  Order second;
  second.remaining_quantity = 20;
  level.PushBack(&first);
  level.PushBack(&second);

  level.Remove(&first);

  EXPECT_EQ(level.total_quantity(), 20u);
}

}  // namespace
}  // namespace lob