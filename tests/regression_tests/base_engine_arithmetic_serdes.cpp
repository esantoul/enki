#include <cstddef>
#include <cstdint>
#include <array>

#include "enki/legacy/base_engine.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Base Engine Arithmetic SerDes", "[base_engine][regression]")
{
  const int32_t i = 42;
  int32_t j = 0;
  std::array<std::byte, 4> temp{};

  {
    const auto ser_res = enki::BaseEngine::Serialize(i, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == sizeof(i));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = enki::BaseEngine::Deserialize(j, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == sizeof(i));
    REQUIRE(des_res.get_iterator() == temp.end());
  }
  REQUIRE(j == i);
}
