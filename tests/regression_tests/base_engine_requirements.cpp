
#include <concepts>
#include <type_traits>

#include "enki/base_engine.hpp"

#include "catch2/catch_test_macros.hpp"

TEST_CASE("Base Engine is a stateless shell", "[base_engine][regression]")
{
  static_assert(std::is_constructible_v<enki::BaseEngine> == false);
  static_assert(std::copyable<enki::BaseEngine> == false);
  static_assert(std::movable<enki::BaseEngine> == false);
  static_assert(std::assignable_from<enki::BaseEngine &, enki::BaseEngine> == false);

  struct S {};
  static_assert(sizeof(S) == sizeof(enki::BaseEngine));
  REQUIRE(true);
}

