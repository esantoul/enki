#include <array>
#include <cstddef>

#include <catch2/catch_test_macros.hpp>

#include "enki/legacy/base_engine.hpp"

enum class eMyEnum : char
{
  ONE,
  TWO
};

TEST_CASE("Base Engine Enum SerDes", "[base_engine][regression]")
{
  const eMyEnum e1 = eMyEnum::ONE;
  eMyEnum e2 = eMyEnum::TWO;
  std::array<std::byte, 1> temp{};

  {
    const auto serRes = enki::BaseEngine::serialize(e1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == sizeof(e1));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(e2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == sizeof(e1));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(e1 == e2);
}
