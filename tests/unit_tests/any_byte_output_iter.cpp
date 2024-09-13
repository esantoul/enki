#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <numeric>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "enki/impl/any_byte_iterator.hpp"

TEST_CASE("Any Byte Output Iterator Works With Any Byte Type", "[any_byte_iterator][unit]")
{
  static constexpr size_t kNumElements = 16;

  std::array<uint8_t, kNumElements> v1;
  std::array<char, kNumElements> v2;
  std::array<std::byte, kNumElements> v3;

  std::iota(v1.begin(), v1.end(), 0);

  auto v2It = v2.begin(); // NOLINT

  std::transform(v1.begin(), v1.end(), enki::AnyByteOutputIt::ref(v2It), [](auto el) {
    return static_cast<std::byte>(el);
  });

  REQUIRE(v2It == v2.end());
  REQUIRE(std::equal(v1.begin(), v1.end(), v2.begin(), [](auto lh, auto rh) {
    return static_cast<std::byte>(lh) == static_cast<std::byte>(rh);
  }));

  std::transform(v2.begin(), v2.end(), enki::AnyByteOutputIt::copy(std::begin(v3)), [](auto el) {
    return static_cast<std::byte>(el);
  });

  REQUIRE(std::size(v1) == std::size(v3));
  REQUIRE(std::equal(v1.begin(), v1.end(), v3.begin(), [](auto lh, auto rh) {
    return static_cast<std::byte>(lh) == static_cast<std::byte>(rh);
  }));
}

TEST_CASE("Any Byte Output Iterator Properly Mirrors Behaviour", "[any_byte_iterator][unit]")
{
  std::vector<uint8_t> v1(16);
  std::vector<char> v2;
  std::deque<std::byte> v3;

  std::iota(v1.begin(), v1.end(), 0);

  auto v2BackInserter = std::back_inserter(v2);

  std::transform(v1.begin(), v1.end(), enki::AnyByteOutputIt::ref(v2BackInserter), [](auto el) {
    return static_cast<std::byte>(el);
  });

  REQUIRE(std::size(v1) == std::size(v2));
  REQUIRE(std::equal(v1.begin(), v1.end(), v2.begin(), [](auto lh, auto rh) {
    return static_cast<std::byte>(lh) == static_cast<std::byte>(rh);
  }));

  std::transform(
    v2.begin(), v2.end(), enki::AnyByteOutputIt::copy(std::front_inserter(v3)), [](auto el) {
      return static_cast<std::byte>(el);
    });

  REQUIRE(std::size(v1) == std::size(v3));
  REQUIRE(std::equal(v1.begin(), v1.end(), v3.rbegin(), [](auto lh, auto rh) {
    return static_cast<std::byte>(lh) == static_cast<std::byte>(rh);
  }));
}
