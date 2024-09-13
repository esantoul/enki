#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "enki/legacy/manager.hpp"

TEST_CASE("Manager Custom Type Register/Unregister", "[manager][regression]")
{
  struct S
  {
  };

  auto mgr = enki::Manager{};

  std::array<std::byte, 0> temp;

  // Non registered type should not be serializable
  REQUIRE_THROWS(mgr.serialize(S{}, temp.begin()).or_throw());

  mgr.registerType<S>();

  REQUIRE_NOTHROW(mgr.serialize(S{}, temp.begin()).or_throw());

  mgr.unregisterType<S>();

  // Unregistered type should be correctly unregistered
  REQUIRE_THROWS(mgr.serialize(S{}, temp.begin()).or_throw());
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

  mgr.registerType<S, &S::i, &S::d>(); // you can register members in any order
  {
    const auto serRes = mgr.serialize(s1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = mgr.deserialize(s2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(s1 == s2);

  // order of registration should be preserved
  {
    struct SerLayout
    {
      std::array<std::byte, sizeof(decltype(S::i))> i; // S::i should be serialized first
      std::array<std::byte, sizeof(decltype(S::d))> d; // S::d should be serialized second
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

  mgr.registerType<S, ENKIWRAP(S, val)>(); // use ENKIWRAP to register bit field
  {
    const auto serRes = mgr.serialize(s1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = mgr.deserialize(s2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
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

  mgr.registerType<S, &SBase::val, &S::val>();
  {
    const auto serRes = mgr.serialize(s1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = mgr.deserialize(s2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(s1 == s2);
}
