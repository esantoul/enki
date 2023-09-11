#include <array>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include <numeric>
#include <type_traits>
#include <unordered_map>

#include "enki/base_engine.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Base Engine Composite Test", "[base_engine][regression]")
{
  enum class Maths
  {
    pythagora,
    euler
  };

  const std::unordered_map<Maths, std::vector<double>> m1{
    std::make_pair(Maths::pythagora, std::vector<double>{std::numbers::pi}),
    std::make_pair(Maths::euler, std::vector<double>{std::numbers::e, 1.0 / std::numbers::e})};

  std::remove_cvref_t<decltype(m1)> m2;

  const size_t totalSerSize =
    std::accumulate(
      m1.begin(),
      m1.end(),
      sizeof(enki::BaseEngine::size_type),
      [](size_t prev, auto kv) {
    return prev +
      sizeof(kv.first) + sizeof(enki::BaseEngine::size_type) + std::size(kv.second) * sizeof(typename decltype(kv.second)::value_type);
  });

  std::vector<std::byte> temp(totalSerSize);

  {
    const auto ser_res = enki::BaseEngine::Serialize(m1, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == std::size(temp));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = enki::BaseEngine::Deserialize(m2, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == std::size(temp));
    REQUIRE(des_res.get_iterator() == temp.end());
  }

  REQUIRE(m1 == m2);
}
