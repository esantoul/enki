#include <array>
#include <concepts>
#include <cstdint>
#include <cstddef>
#include <numbers>
#include <type_traits>
#include <variant>
#include <vector>

#include "enki/manager.hpp"

#include "catch2/catch_test_macros.hpp"

TEST_CASE("Manager copy/move behaves properly", "[manager][regression]")
{
#pragma pack(push, 1)
  struct S
  {
    double d{};
    int8_t i : 5{};

    constexpr bool operator==(const S &) const = default;
  };
#pragma pack(pop)

  const S s1{3.14, 0xF};
  const S s2{std::numbers::e, -2};
  const S s3{std::numbers::phi, 0x7};

  enki::Manager mgr;

  std::vector<std::byte> temp;

  REQUIRE_THROWS(mgr.Serialize(s1, std::back_inserter(temp)).or_throw());

  mgr.Register<S, ENKIWRAP(S, d), ENKIWRAP(S, i)>();
  REQUIRE_NOTHROW(mgr.Serialize(s1, std::back_inserter(temp)).or_throw());

  {
    auto mgr2 = mgr;

    S sd;

    const auto ser_res = mgr2.Serialize(s2, std::back_inserter(temp)).or_throw();
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == sizeof(S));

    const auto des_res = mgr2.Deserialize(sd, std::begin(temp)).or_throw();
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(sd == s1);
  }

  std::variant<std::monostate, enki::Manager<>> vmgr{};

  {
    auto mgr3 = std::move(mgr);

    S sd;

    const auto ser_res = mgr3.Serialize(s3, std::back_inserter(temp)).or_throw();
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == sizeof(S));

    const auto des_res = mgr3.Deserialize(sd, std::begin(temp)).or_throw();
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == sizeof(S));
    REQUIRE(sd == s1);

    vmgr = std::move(mgr3);
  }

  {
    std::array<S, 3> as;

    const auto des_res = std::get<enki::Manager<>>(vmgr).Deserialize(as, std::begin(temp));

    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(as == std::array{s1, s2, s3});
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
