#include <array>
#include <cstdint>
#include <cstddef>
#include <algorithm>

#include "enki/base_engine.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("C-Array Serialization and Deserialization")
{
  int64_t carr1[] = {1337, -99};
  int64_t carr2[std::size(carr1)]{};
  std::array<std::byte, sizeof(carr1)> temp{};

  {
    const auto ser_res = enki::BaseEngine::Serialize(carr1, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == sizeof(carr1));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = enki::BaseEngine::Deserialize(carr2, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == sizeof(carr1));
    REQUIRE(des_res.get_iterator() == temp.end());
  }
  REQUIRE(std::equal(std::begin(carr1), std::end(carr1), std::begin(carr2)));
}

TEST_CASE("C++ Array (std::array) Serialization and Deserialization")
{
  std::array<int64_t, 2> arr1 = {1337, -99};
  std::array<int64_t, 2> arr2{};
  std::array<std::byte, sizeof(arr1)> temp{};
  {
    const auto ser_res = enki::BaseEngine::Serialize(arr1, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == sizeof(arr1));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = enki::BaseEngine::Deserialize(arr2, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == sizeof(arr1));
    REQUIRE(des_res.get_iterator() == temp.end());
  }
  REQUIRE(arr1 == arr2);
}

