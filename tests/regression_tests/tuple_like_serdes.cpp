#include <cstddef>
#include <cstdint>
#include <array>
#include <tuple>
#include <utility>
#include <limits>

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

TEST_CASE("Tuple Serialization And Deserialization")
{
  std::tuple<double, uint8_t, uint64_t> t1{3.14, 42, std::numeric_limits<uint64_t>::max()};
  decltype(t1) t2{};
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

TEST_CASE("Pair Serialization And Deserialization")
{
  std::pair<int16_t, float> p1{-22, std::numeric_limits<float>::min()};
  decltype(p1) p2{};
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
