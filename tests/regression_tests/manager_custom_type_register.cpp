#include <cstddef>
#include <cstdint>
#include <array>
#include <bit>

#include "enki/manager.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Manager Custom Type Register/Unregister", "[manager][regression]")
{
  struct S {};

  auto mgr = enki::Manager{};

  std::array<std::byte, 0> temp;

  // Non registered type should not be serializable
  REQUIRE_THROWS(mgr.Serialize(S{}, temp.begin()).or_throw());

  mgr.Register<S>();

  REQUIRE_NOTHROW(mgr.Serialize(S{}, temp.begin()).or_throw());

  mgr.Unregister<S>();

  // Unregistered type should be correctly unregistered
  REQUIRE_THROWS(mgr.Serialize(S{}, temp.begin()).or_throw());
}

TEST_CASE("Manager Custom Type Dynamic SerDes", "[manager][regression]")
{
  struct S
  {
    double d;
    int i;

    constexpr bool operator==(const S &) const = default;
  };

  const S s1{3.14, 42};
  S s2{};
  std::array<std::byte, sizeof(decltype(S::i)) + sizeof(decltype(S::d))> temp{};

  auto mgr = enki::Manager{};

  mgr.Register<S, &S::i, &S::d>(); // you can register members in any order
  {
    const auto ser_res = mgr.Serialize(s1, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == std::size(temp));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = mgr.Deserialize(s2, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == std::size(temp));
    REQUIRE(des_res.get_iterator() == temp.end());
  }

  REQUIRE(s1 == s2);

  // order of registration should be preserved
  {
    struct SerLayout
    {
      std::array<std::byte, sizeof(decltype(S::i))> i;  // S::i should be serialized first
      std::array<std::byte, sizeof(decltype(S::d))> d;  // S::d should be serialized second
    };
    const auto &serializedLayout = std::bit_cast<SerLayout>(temp);
    REQUIRE(s1.i == std::bit_cast<int>(serializedLayout.i));
    REQUIRE(s1.d == std::bit_cast<double>(serializedLayout.d));
  }
}

TEST_CASE("Manager Custom Type With Bit Field Dynamic Serdes", "[manager][regression]")
{
  struct S
  {
    uint8_t val : 3;

    constexpr bool operator==(const S &) const = default;
  };

  S s1{7};
  s1.val += 8;
  REQUIRE(s1.val == 7);

  S s2{};
  std::array<std::byte, sizeof(decltype(S::val))> temp{};

  auto mgr = enki::Manager{};

  mgr.Register<S, ENKIWRAP(S, val)>(); // use ENKIWRAP to register bit field
  {
    const auto ser_res = mgr.Serialize(s1, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == std::size(temp));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = mgr.Deserialize(s2, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == std::size(temp));
    REQUIRE(des_res.get_iterator() == temp.end());
  }

  REQUIRE(s1 == s2);
}

TEST_CASE("Manager Custom Type With Inherited Member Dynamic Serdes", "[manager][regression]")
{
  struct SBase
  {
    int val;

    constexpr bool operator==(const SBase &) const = default;
  };

  struct S : SBase
  {
    uint8_t val;

    constexpr bool operator==(const S &) const = default;
  };

  const S s1{{42}, 7};
  S s2{};
  std::array<std::byte, sizeof(decltype(SBase::val)) + sizeof(decltype(S::val))> temp{};

  auto mgr = enki::Manager{};

  mgr.Register<S, &SBase::val, &S::val>();
  {
    const auto ser_res = mgr.Serialize(s1, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == std::size(temp));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = mgr.Deserialize(s2, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == std::size(temp));
    REQUIRE(des_res.get_iterator() == temp.end());
  }

  REQUIRE(s1 == s2);
}
