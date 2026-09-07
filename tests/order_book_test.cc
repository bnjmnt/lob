#include "lob/order_book.h"

#include <gtest/gtest.h>

#include "lob/order.h"

namespace lob {
namespace {

TEST(OrderBookTest, NewBookHasNoBestBidOrAsk) {
  OrderBook book(16);

  EXPECT_FALSE(book.BestBid().has_value());
  EXPECT_FALSE(book.BestAsk().has_value());
}

TEST(OrderBookTest, AddOrderReturnsAnId) {
  OrderBook book(16);

  auto result = book.AddOrder(10000, 100, Side::kBid);

  ASSERT_TRUE(result.has_value());
}

TEST(OrderBookTest, AddOrderIdsAreUniqueAndIncreasing) {
  OrderBook book(16);

  auto first = book.AddOrder(10000, 100, Side::kBid);
  auto second = book.AddOrder(10000, 100, Side::kBid);

  ASSERT_TRUE(first.has_value());
  ASSERT_TRUE(second.has_value());
  EXPECT_NE(*first, *second);
  EXPECT_GT(*second, *first);
}

TEST(OrderBookTest, AddOrderNeverIssuesIdZero) {
  OrderBook book(16);

  auto result = book.AddOrder(10000, 100, Side::kBid);

  ASSERT_TRUE(result.has_value());
  EXPECT_NE(*result, 0u);
}

TEST(OrderBookTest, AddingBidSetsBestBid) {
  OrderBook book(16);

  book.AddOrder(10000, 100, Side::kBid);

  ASSERT_TRUE(book.BestBid().has_value());
  EXPECT_EQ(*book.BestBid(), 10000u);
}

TEST(OrderBookTest, AddingAskSetsBestAsk) {
  OrderBook book(16);

  book.AddOrder(10050, 100, Side::kAsk);

  ASSERT_TRUE(book.BestAsk().has_value());
  EXPECT_EQ(*book.BestAsk(), 10050u);
}

TEST(OrderBookTest, BestBidIsHighestBidPrice) {
  OrderBook book(16);

  book.AddOrder(9900, 100, Side::kBid);
  book.AddOrder(10000, 100, Side::kBid);
  book.AddOrder(9950, 100, Side::kBid);

  ASSERT_TRUE(book.BestBid().has_value());
  EXPECT_EQ(*book.BestBid(), 10000u);
}

TEST(OrderBookTest, BestAskIsLowestAskPrice) {
  OrderBook book(16);

  book.AddOrder(10100, 100, Side::kAsk);
  book.AddOrder(10050, 100, Side::kAsk);
  book.AddOrder(10075, 100, Side::kAsk);

  ASSERT_TRUE(book.BestAsk().has_value());
  EXPECT_EQ(*book.BestAsk(), 10050u);
}

TEST(OrderBookTest, BidsAndAsksDoNotInterfereWithEachOther) {
  OrderBook book(16);

  book.AddOrder(9900, 100, Side::kBid);
  book.AddOrder(10100, 100, Side::kAsk);

  ASSERT_TRUE(book.BestBid().has_value());
  ASSERT_TRUE(book.BestAsk().has_value());
  EXPECT_EQ(*book.BestBid(), 9900u);
  EXPECT_EQ(*book.BestAsk(), 10100u);
}

TEST(OrderBookTest, MultipleOrdersAtSamePriceStayAtThatBestPrice) {
  OrderBook book(16);

  book.AddOrder(10000, 100, Side::kBid);
  book.AddOrder(10000, 200, Side::kBid);

  ASSERT_TRUE(book.BestBid().has_value());
  EXPECT_EQ(*book.BestBid(), 10000u);
}

TEST(OrderBookTest, CancelOrderOnUnknownIdReturnsFalse) {
  OrderBook book(16);

  EXPECT_FALSE(book.CancelOrder(999));
}

TEST(OrderBookTest, CancelOrderOnKnownIdReturnsTrue) {
  OrderBook book(16);
  auto id = book.AddOrder(10000, 100, Side::kBid);
  ASSERT_TRUE(id.has_value());

  EXPECT_TRUE(book.CancelOrder(*id));
}

TEST(OrderBookTest, CancelOrderTwiceReturnsFalseOnSecondCall) {
  OrderBook book(16);
  auto id = book.AddOrder(10000, 100, Side::kBid);
  ASSERT_TRUE(id.has_value());

  book.CancelOrder(*id);

  EXPECT_FALSE(book.CancelOrder(*id));
}

TEST(OrderBookTest, CancelingOnlyOrderAtPriceRemovesThatBestPrice) {
  OrderBook book(16);
  auto id = book.AddOrder(10000, 100, Side::kBid);
  ASSERT_TRUE(id.has_value());

  book.CancelOrder(*id);

  EXPECT_FALSE(book.BestBid().has_value());
}

TEST(OrderBookTest, CancelingBestPriceRevealsNextBestPrice) {
  OrderBook book(16);
  auto first = book.AddOrder(10000, 100, Side::kBid);
  book.AddOrder(9900, 100, Side::kBid);
  ASSERT_TRUE(first.has_value());

  book.CancelOrder(*first);

  ASSERT_TRUE(book.BestBid().has_value());
  EXPECT_EQ(*book.BestBid(), 9900u);
}

TEST(OrderBookTest, CancelingNonBestPriceLeavesBestPriceUnchanged) {
  OrderBook book(16);
  book.AddOrder(10000, 100, Side::kBid);
  auto worse = book.AddOrder(9900, 100, Side::kBid);
  ASSERT_TRUE(worse.has_value());

  book.CancelOrder(*worse);

  ASSERT_TRUE(book.BestBid().has_value());
  EXPECT_EQ(*book.BestBid(), 10000u);
}

TEST(OrderBookTest, CancelingOneOfTwoOrdersAtSamePriceKeepsPriceLevel) {
  OrderBook book(16);
  auto first = book.AddOrder(10000, 100, Side::kBid);
  book.AddOrder(10000, 200, Side::kBid);
  ASSERT_TRUE(first.has_value());

  book.CancelOrder(*first);

  ASSERT_TRUE(book.BestBid().has_value());
  EXPECT_EQ(*book.BestBid(), 10000u);
}

TEST(OrderBookTest, AddOrderFailsWhenPoolIsExhausted) {
  OrderBook book(2);
  book.AddOrder(10000, 100, Side::kBid);
  book.AddOrder(10001, 100, Side::kBid);

  auto result = book.AddOrder(10002, 100, Side::kBid);

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), AddOrderError::kPoolExhausted);
}

TEST(OrderBookTest, CancelFreesPoolCapacityForReuse) {
  OrderBook book(1);
  auto id = book.AddOrder(10000, 100, Side::kBid);
  ASSERT_TRUE(id.has_value());
  ASSERT_FALSE(book.AddOrder(10001, 100, Side::kBid).has_value());

  book.CancelOrder(*id);
  auto reused = book.AddOrder(10002, 100, Side::kBid);

  EXPECT_TRUE(reused.has_value());
}

}  // namespace
}  // namespace lob