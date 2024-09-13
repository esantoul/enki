#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "enki/legacy/base_engine.hpp"

TEST_CASE("Base Engine C-Array SerDes", "[base_engine][regression]")
{
  const int64_t carr1[] = {1337, -99};
  int64_t carr2[std::size(carr1)]{};
  std::array<std::byte, sizeof(carr1)> temp{};

  {
    const auto serRes = enki::BaseEngine::serialize(carr1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == sizeof(carr1));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(carr2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == sizeof(carr1));
    REQUIRE(desRes.get_iterator() == temp.end());
  }
  REQUIRE(std::equal(std::begin(carr1), std::end(carr1), std::begin(carr2)));
}

TEST_CASE("Base Engine C++ Array (std::array) SerDes", "[base_engine][regression]")
{
  const std::array<int64_t, 2> arr1 = {1337, -99};
  std::array<int64_t, 2> arr2{};
  std::array<std::byte, sizeof(arr1)> temp{};
  {
    const auto serRes = enki::BaseEngine::serialize(arr1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == sizeof(arr1));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(arr2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == sizeof(arr1));
    REQUIRE(desRes.get_iterator() == temp.end());
  }
  REQUIRE(arr1 == arr2);
}
