#include <cstddef>
#include <array>

#include "enki/base_engine.hpp"

enum class MyEnum : char
{
  ONE,
  TWO
};

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Enumeration Serialization And Deserialization")
{
  MyEnum e1 = MyEnum::ONE;
  MyEnum e2 = MyEnum::TWO;
  std::array<std::byte, 1> temp{};

  const auto ser_res = enki::BaseEngine::Serialize(e1, temp.begin()).first;
  REQUIRE_NOTHROW(ser_res.or_throw());
  REQUIRE(ser_res.size() == sizeof(e1));


  const auto des_res = enki::BaseEngine::Deserialize(e2, temp.begin()).first;
  REQUIRE_NOTHROW(des_res.or_throw());
  REQUIRE(des_res.size() == sizeof(e1));

  REQUIRE(e1 == e2);
}
