#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include <type_traits>
#include <variant>
#include <vector>

#include "catch2/catch_test_macros.hpp"

#include "enki/legacy/manager.hpp"

TEST_CASE("Manager copy/move behaves properly", "[manager][regression]")
{
#pragma pack(push, 1)

  struct S
  {
    double d{};
    int8_t i : 5 {};

    constexpr bool operator==(const S &) const = default;
  };

#pragma pack(pop)

  const S s1{3.14, 0xF};
  const S s2{std::numbers::e, -2};
  const S s3{std::numbers::phi, 0x7};

  enki::Manager mgr;

  std::vector<std::byte> temp;

  REQUIRE_THROWS(mgr.serialize(s1, std::back_inserter(temp)).or_throw());

  mgr.registerType<S, ENKIWRAP(S, d), ENKIWRAP(S, i)>();
  REQUIRE_NOTHROW(mgr.serialize(s1, std::back_inserter(temp)).or_throw());

  {
    auto mgr2 = mgr;

    S sd;

    const auto serRes = mgr2.serialize(s2, std::back_inserter(temp)).or_throw();
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == sizeof(S));

    const auto desRes = mgr2.deserialize(sd, std::begin(temp)).or_throw();
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(sd == s1);
  }

  std::variant<std::monostate, enki::Manager<>> vmgr{};

  {
    auto mgr3 = std::move(mgr);

    S sd;

    const auto serRes = mgr3.serialize(s3, std::back_inserter(temp)).or_throw();
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == sizeof(S));

    const auto desRes = mgr3.deserialize(sd, std::begin(temp)).or_throw();
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == sizeof(S));
    REQUIRE(sd == s1);

    vmgr = std::move(mgr3);
  }

  {
    std::array<S, 3> as;

    const auto desRes = std::get<enki::Manager<>>(vmgr).deserialize(as, std::begin(temp));

    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(as == std::array{s1, s2, s3});
  }

  REQUIRE_THROWS(mgr.serialize(S{}, std::back_inserter(temp)).or_throw());
}

TEST_CASE("Manager type can be instantiated, copied and moved", "[manager][regression]")
{
  static_assert(std::is_constructible_v<enki::Manager<>>);
  static_assert(std::copyable<enki::Manager<>>);
  static_assert(std::movable<enki::Manager<>>);
  static_assert(std::assignable_from<enki::Manager<> &, enki::Manager<>>);

  REQUIRE(true);
}
