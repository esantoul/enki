#include <array>
#include <cstddef>
#include <numbers>
#include <numeric>
#include <type_traits>
#include <unordered_map>

#include <catch2/catch_test_macros.hpp>

#include "enki/legacy/base_engine.hpp"

TEST_CASE("Base Engine Composite Test", "[base_engine][regression]")
{
  enum class eMaths
  {
    Pythagora,
    Euler
  };

  const std::unordered_map<eMaths, std::vector<double>> m1{
    std::make_pair(eMaths::Pythagora, std::vector<double>{std::numbers::pi}),
    std::make_pair(eMaths::Euler, std::vector<double>{std::numbers::e, 1.0 / std::numbers::e})};

  std::remove_cvref_t<decltype(m1)> m2;

  const size_t totalSerSize = std::accumulate(
    m1.begin(), m1.end(), sizeof(enki::BaseEngine::size_type), [](size_t prev, auto kv) {
      return prev + sizeof(kv.first) + sizeof(enki::BaseEngine::size_type) +
             std::size(kv.second) * sizeof(typename decltype(kv.second)::value_type);
    });

  std::vector<std::byte> temp(totalSerSize);

  {
    const auto serRes = enki::BaseEngine::serialize(m1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(m2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(m1 == m2);
}
