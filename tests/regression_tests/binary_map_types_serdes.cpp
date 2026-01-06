#include <array>
#include <cstddef>
#include <numbers>
#include <numeric>
#include <type_traits>

#include <map>
#include <unordered_map>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

namespace
{
  enum class eMaths
  {
    Pythagora,
    Euler
  };
}

TEST_CASE("Map Type (std::map) SerDes", "[regression]")
{
  const std::map<eMaths, std::vector<double>> m1{
    std::make_pair(eMaths::Euler, std::vector<double>{std::numbers::e, 1.0 / std::numbers::e}),
    std::make_pair(eMaths::Pythagora, std::vector<double>{std::numbers::pi}),
    std::make_pair(eMaths::Euler, std::vector<double>{std::numbers::e, 2.0 * std::numbers::e})};

  std::remove_cvref_t<decltype(m1)> m2;

  enki::BinWriter writer;

  const size_t totalSerSize = std::accumulate(
    m1.begin(), m1.end(), sizeof(decltype(writer)::size_type), [](size_t prev, auto kv) {
      return prev + sizeof(kv.first) + sizeof(decltype(writer)::size_type) +
             std::size(kv.second) * sizeof(typename decltype(kv.second)::value_type);
    });

  const auto serRes = enki::serialize(m1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == totalSerSize);

  const auto desRes = enki::deserialize(m2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == totalSerSize);

  REQUIRE(m1 == m2);
}

TEST_CASE("Map Type (std::multimap) SerDes", "[regression]")
{
  const std::multimap<eMaths, std::vector<double>> m1{
    std::make_pair(eMaths::Euler, std::vector<double>{std::numbers::e, 1.0 / std::numbers::e}),
    std::make_pair(eMaths::Pythagora, std::vector<double>{std::numbers::pi}),
    std::make_pair(eMaths::Euler, std::vector<double>{std::numbers::e, 2.0 * std::numbers::e})};

  std::remove_cvref_t<decltype(m1)> m2;

  enki::BinWriter writer;

  const size_t totalSerSize = std::accumulate(
    m1.begin(), m1.end(), sizeof(decltype(writer)::size_type), [](size_t prev, auto kv) {
      return prev + sizeof(kv.first) + sizeof(decltype(writer)::size_type) +
             std::size(kv.second) * sizeof(typename decltype(kv.second)::value_type);
    });

  const auto serRes = enki::serialize(m1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == totalSerSize);

  const auto desRes = enki::deserialize(m2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == totalSerSize);

  REQUIRE(m1 == m2);
}

TEST_CASE("Map Type (std::unordered_map) SerDes", "[regression]")
{
  const std::unordered_map<eMaths, std::vector<double>> m1{
    std::make_pair(eMaths::Euler, std::vector<double>{std::numbers::e, 1.0 / std::numbers::e}),
    std::make_pair(eMaths::Pythagora, std::vector<double>{std::numbers::pi}),
    std::make_pair(eMaths::Euler, std::vector<double>{std::numbers::e, 2.0 * std::numbers::e})};

  std::remove_cvref_t<decltype(m1)> m2;

  enki::BinWriter writer;

  const size_t totalSerSize = std::accumulate(
    m1.begin(), m1.end(), sizeof(decltype(writer)::size_type), [](size_t prev, auto kv) {
      return prev + sizeof(kv.first) + sizeof(decltype(writer)::size_type) +
             std::size(kv.second) * sizeof(typename decltype(kv.second)::value_type);
    });

  const auto serRes = enki::serialize(m1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == totalSerSize);

  const auto desRes = enki::deserialize(m2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == totalSerSize);

  REQUIRE(m1 == m2);
}

TEST_CASE("Map Type (std::unordered_multimap) SerDes", "[regression]")
{
  const std::unordered_multimap<eMaths, std::vector<double>> m1{
    std::make_pair(eMaths::Euler, std::vector<double>{std::numbers::e, 1.0 / std::numbers::e}),
    std::make_pair(eMaths::Pythagora, std::vector<double>{std::numbers::pi}),
    std::make_pair(eMaths::Euler, std::vector<double>{std::numbers::e, 2.0 * std::numbers::e})};

  std::remove_cvref_t<decltype(m1)> m2;

  enki::BinWriter writer;

  const size_t totalSerSize = std::accumulate(
    m1.begin(), m1.end(), sizeof(decltype(writer)::size_type), [](size_t prev, auto kv) {
      return prev + sizeof(kv.first) + sizeof(decltype(writer)::size_type) +
             std::size(kv.second) * sizeof(typename decltype(kv.second)::value_type);
    });

  const auto serRes = enki::serialize(m1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == totalSerSize);

  const auto desRes = enki::deserialize(m2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == totalSerSize);

  REQUIRE(m1 == m2);
}
