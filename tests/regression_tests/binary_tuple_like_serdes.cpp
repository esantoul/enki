#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

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

TEST_CASE("Tuple SerDes", "[regression]")
{
  const auto t1 = std::make_tuple(3.14, 42, std::numeric_limits<uint64_t>::max());
  std::remove_cvref_t<decltype(t1)> t2{};

  enki::BinWriter writer;

  const auto serRes = enki::serialize(t1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sumTupleElementSizes(t1));

  const auto desRes = enki::deserialize(t2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == sumTupleElementSizes(t1));

  REQUIRE(t1 == t2);
}

TEST_CASE("Pair SerDes", "[regression]")
{
  const std::pair<int16_t, float> p1{-22, std::numeric_limits<float>::min()};
  std::remove_cvref_t<decltype(p1)> p2{};

  enki::BinWriter writer;

  const auto serRes = enki::serialize(p1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sumTupleElementSizes(p1));

  const auto desRes = enki::deserialize(p2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == sumTupleElementSizes(p1));

  REQUIRE(p1 == p2);
}
