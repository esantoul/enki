#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <vector>
#include <deque>

#include "enki/impl/any_byte_iterator.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Any Byte Input Iterator Works With Any Byte Type", "[any_byte_iterator][unit]")
{
  static constexpr size_t kNumElements = 16;

  std::array<uint8_t, kNumElements> v1;
  std::array<char, kNumElements> v2;

  std::iota(v1.begin(), v1.end(), 0);
  std::iota(v2.begin(), v2.end(), 0);

  REQUIRE(std::equal(
    enki::AnyByteInputIt(v2.begin()),
    enki::AnyByteInputIt(v2.end()),
    enki::AnyByteInputIt(v1.begin()),
    [](std::byte lh, std::byte rh) {
    return lh == rh;
  }));
}
