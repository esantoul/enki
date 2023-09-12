
#include <concepts>
#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <vector>

#include "enki/manager.hpp"

#include "catch2/catch_test_macros.hpp"

TEST_CASE("Manager copy/move behaves properly", "[manager][regression]")
{
#pragma pack(push, 1)
  struct S
  {
    double d{};
    uint8_t i : 4{};

    constexpr bool operator==(const S &) const = default;
  };
#pragma pack(pop)

  const S s1{3.14, 0xF};

  enki::Manager mgr;

  std::vector<std::byte> temp;

  REQUIRE_THROWS(mgr.Serialize(s1, std::back_inserter(temp)).or_throw());

  mgr.Register<S, ENKIWRAP(S, d), ENKIWRAP(S, i)>();
  REQUIRE_NOTHROW(mgr.Serialize(s1, std::back_inserter(temp)).or_throw());

  {
    auto mgr2 = mgr;

    S s2;

    const auto ser_res = mgr2.Serialize(S{}, std::back_inserter(temp)).or_throw();
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == sizeof(S));

    const auto des_res = mgr2.Deserialize(s2, std::begin(temp)).or_throw();
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(s2 == s1);
  }

  {
    auto mgr3 = std::move(mgr);

    S s3;

    const auto ser_res = mgr3.Serialize(S{}, std::back_inserter(temp)).or_throw();
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == sizeof(S));

    const auto des_res = mgr3.Deserialize(s3, std::begin(temp)).or_throw();
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == sizeof(S));
    REQUIRE(s3 == s1);
  }

  REQUIRE_THROWS(mgr.Serialize(S{}, std::back_inserter(temp)).or_throw());
}

TEST_CASE("Manager type can be instantiated, copied and moved", "[manager][regression]")
{
  static_assert(std::is_constructible_v<enki::Manager<>>);
  static_assert(std::copyable<enki::Manager<>>);
  static_assert(std::movable<enki::Manager<>>);
  static_assert(std::assignable_from<enki::Manager<> &, enki::Manager<>>);

  REQUIRE(true);
}
