#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "enki/legacy/base_engine.hpp"

namespace
{
  template <enki::concepts::tuple_like T, size_t... idx>
  constexpr size_t sumTupleElementSizesImpl(const T &, std::index_sequence<idx...>)
  {
    return (sizeof(std::tuple_element_t<idx, T>) + ...);
  }

  template <enki::concepts::tuple_like T>
  constexpr size_t sumTupleElementSizes(const T &tpl)
  {
    return sumTupleElementSizesImpl(tpl, std::make_index_sequence<std::tuple_size_v<T>>());
  }
} // namespace

TEST_CASE("Base Engine Tuple SerDes", "[base_engine][regression]")
{
  const auto t1 = std::make_tuple(3.14, 42, std::numeric_limits<uint64_t>::max());
  std::remove_cvref_t<decltype(t1)> t2{};
  std::array<std::byte, sumTupleElementSizes(t1)> temp{};

  {
    const auto serRes = enki::BaseEngine::serialize(t1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == sizeof(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(t2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == sizeof(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(t1 == t2);
}

TEST_CASE("Base Engine Pair SerDes", "[base_engine][regression]")
{
  const std::pair<int16_t, float> p1{-22, std::numeric_limits<float>::min()};
  std::remove_cvref_t<decltype(p1)> p2{};
  std::array<std::byte, sumTupleElementSizes(p1)> temp{};

  {
    const auto serRes = enki::BaseEngine::serialize(p1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == sizeof(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(p2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == sizeof(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(p1 == p2);
}
