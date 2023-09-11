#include <cstddef>
#include <cstdint>
#include <array>
#include <bit>
#include <tuple>

#include "enki/base_engine.hpp"

#include <catch2/catch_test_macros.hpp>

namespace
{
  class MyClass
  {
  public:
    constexpr MyClass() = default;
    constexpr MyClass(double dd, int ii)
      :
      d(dd),
      i(ii)
    {
    }

    constexpr bool operator==(const MyClass &) const = default;

    struct EnkiSerial;
    struct SerLayout;

    constexpr int get_int() const { return i; }
    constexpr double get_double() const { return d; }

  private:
    double d{};
    int i{};
  };

  struct MyClass::EnkiSerial
  {
    static constexpr auto members = std::make_tuple(&MyClass::i, &MyClass::d);
  };

  struct MyClass::SerLayout
  {
    std::array<std::byte, sizeof(decltype(MyClass::i))> i;  // MyClass::i should be serialized first
    std::array<std::byte, sizeof(decltype(MyClass::d))> d;  // MyClass::d should be serialized second
  };
}

TEST_CASE("Base Engine Custom Type SerDes", "[manager]")
{
  MyClass c1{3.14, 42};
  MyClass c2{};

  static constexpr size_t numBytesS = enki::BaseEngine::NumBytes(MyClass{}).size();
  std::array<std::byte, numBytesS> temp{};

  {
    const auto ser_res = enki::BaseEngine::Serialize(c1, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == std::size(temp));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = enki::BaseEngine::Deserialize(c2, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == std::size(temp));
    REQUIRE(des_res.get_iterator() == temp.end());
  }

  REQUIRE(c1 == c2);

  // order of registration should be preserved
  {
    const auto &serializedLayout = std::bit_cast<MyClass::SerLayout>(temp);
    REQUIRE(c1.get_int() == std::bit_cast<int>(serializedLayout.i));
    REQUIRE(c1.get_double() == std::bit_cast<double>(serializedLayout.d));
  }
}
