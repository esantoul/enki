
#include <concepts>
#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <vector>

#include "enki/manager.hpp"

#include "catch2/catch_test_macros.hpp"

TEST_CASE("Manager type can be instantiated, copied and moved", "[manager][regression]")
{
  static_assert(std::is_constructible_v<enki::Manager<>>);
  static_assert(std::copyable<enki::Manager<>>);
  static_assert(std::movable<enki::Manager<>>);
  static_assert(std::assignable_from<enki::Manager<> &, enki::Manager<>>);

  REQUIRE(true);
}

TEST_CASE("Manager copy/move behaves properly", "[manager][regression]")
{
  struct S
  {
    double d;
    uint8_t i : 4;
  };

  enki::Manager mgr;

  std::vector<std::byte> temp;

  REQUIRE_THROWS(mgr.Serialize(S{}, std::back_inserter(temp)).or_throw());

  mgr.Register<S, &S::d/* , ENKIWRAP(S, i) */>(); // bug in clang-17 when trying to register S::i
  REQUIRE_NOTHROW(mgr.Serialize(S{}, std::back_inserter(temp)).or_throw());

  auto mgr2 = mgr;
  REQUIRE_NOTHROW(mgr2.Serialize(S{}, std::back_inserter(temp)).or_throw());

  auto mgr3 = std::move(mgr);
  REQUIRE_NOTHROW(mgr3.Serialize(S{}, std::back_inserter(temp)).or_throw());
  REQUIRE_THROWS(mgr.Serialize(S{}, std::back_inserter(temp)).or_throw());
}
