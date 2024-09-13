#include <array>
#include <cstddef>
#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "enki/legacy/base_engine.hpp"

TEST_CASE("Base Engine Arithmetic SerDes", "[base_engine][regression]")
{
  const int32_t i = 42;
  int32_t j = 0;
  std::array<std::byte, 4> temp{};

  {
    const auto serRes = enki::BaseEngine::serialize(i, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == sizeof(i));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(j, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == sizeof(i));
    REQUIRE(desRes.get_iterator() == temp.end());
  }
  REQUIRE(j == i);
}
