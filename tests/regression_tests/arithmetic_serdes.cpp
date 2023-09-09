#include <cstddef>
#include <cstdint>
#include <array>

#include "enki/base_engine.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Arithmetic Type Serialization and Deserialization")
{
  int32_t i = 42;
  int32_t j = 0;
  std::array<std::byte, 4> temp{};

  const auto ser_res = enki::BaseEngine::Serialize(i, temp.begin()).first;
  REQUIRE_NOTHROW(ser_res.or_throw());
  REQUIRE(ser_res.size() == sizeof(i));

  const auto des_res = enki::BaseEngine::Deserialize(j, temp.begin()).first;
  REQUIRE_NOTHROW(des_res.or_throw());
  REQUIRE(des_res.size() == sizeof(i));

  REQUIRE(j == i);
}
