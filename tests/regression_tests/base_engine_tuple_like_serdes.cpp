#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include "enki/base_engine.hpp"

#include <catch2/catch_test_macros.hpp>

namespace
{
  template <enki::concepts::tuple_like T, size_t ... idx>
  constexpr size_t sum_tuple_element_sizes_impl(const T &, std::index_sequence<idx...>)
  {
    return (sizeof(std::tuple_element_t<idx, T>) + ...);
  }

  template <enki::concepts::tuple_like T>
  constexpr size_t sum_tuple_element_sizes(const T &tpl)
  {
    return sum_tuple_element_sizes_impl(tpl, std::make_index_sequence<std::tuple_size_v<T>>());
  }
}

TEST_CASE("Base Engine Tuple SerDes", "[base_engine][regression]")
{
  const auto t1 = std::make_tuple(3.14, 42, std::numeric_limits<uint64_t>::max());
  std::remove_cvref_t<decltype(t1)> t2{};
  std::array<std::byte, sum_tuple_element_sizes(t1)> temp{};

  {
    const auto ser_res = enki::BaseEngine::Serialize(t1, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == sizeof(temp));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = enki::BaseEngine::Deserialize(t2, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == sizeof(temp));
    REQUIRE(des_res.get_iterator() == temp.end());
  }

  REQUIRE(t1 == t2);
}

TEST_CASE("Base Engine Pair SerDes", "[base_engine][regression]")
{
  const std::pair<int16_t, float> p1{-22, std::numeric_limits<float>::min()};
  std::remove_cvref_t<decltype(p1)> p2{};
  std::array<std::byte, sum_tuple_element_sizes(p1)> temp{};

  {
    const auto ser_res = enki::BaseEngine::Serialize(p1, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == sizeof(temp));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = enki::BaseEngine::Deserialize(p2, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == sizeof(temp));
    REQUIRE(des_res.get_iterator() == temp.end());
  }

  REQUIRE(p1 == p2);
}
