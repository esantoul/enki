#include <cstddef>
#include <array>

#include "enki/base_engine.hpp"

enum class MyEnum : char
{
  ONE,
  TWO
};

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Base Engine Enum SerDes", "[base_engine][regression]")
{
  const MyEnum e1 = MyEnum::ONE;
  MyEnum e2 = MyEnum::TWO;
  std::array<std::byte, 1> temp{};

  {
    const auto ser_res = enki::BaseEngine::Serialize(e1, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == sizeof(e1));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = enki::BaseEngine::Deserialize(e2, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == sizeof(e1));
    REQUIRE(des_res.get_iterator() == temp.end());
  }

  REQUIRE(e1 == e2);
}
