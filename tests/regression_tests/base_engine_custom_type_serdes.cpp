#include <array>
#include <bit>
#include <cstddef>
#include <tuple>

#include <catch2/catch_test_macros.hpp>

#include "enki/legacy/base_engine.hpp"

namespace
{
  class MyClass
  {
  public:
    constexpr MyClass() = default;

    constexpr MyClass(double dd, int ii) :
      mD(dd),
      mI(ii)
    {
    }

    constexpr bool operator==(const MyClass &) const = default;

    struct EnkiSerial;

    constexpr int getInt() const
    {
      return mI;
    }

    constexpr double getDouble() const
    {
      return mD;
    }

  private:
    friend struct SerialisationExpectedSuccess;

    double mD{};
    int mI{};
  };

  struct MyClass::EnkiSerial
  {
    static constexpr auto members = std::make_tuple(&MyClass::mI, &MyClass::mD); // NOLINT
  };

  struct SerialisationExpectedSuccess
  {
    std::array<std::byte, sizeof(decltype(MyClass::mI))> i; // MyClass::i should be serialized first
    std::array<std::byte, sizeof(decltype(MyClass::mD))>
      d; // MyClass::d should be serialized second
  };
} // namespace

TEST_CASE("Base Engine Custom Type SerDes", "[manager][regression]")
{
  const MyClass c1{3.14, 42};
  MyClass c2{};

  static constexpr size_t kNumBytesToSerialize = enki::BaseEngine::NumBytes(MyClass{}).size();
  std::array<std::byte, kNumBytesToSerialize> temp{};

  {
    const auto serRes = enki::BaseEngine::serialize(c1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(c2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(c1 == c2);

  // order of registration should be preserved
  {
    const auto &serializedLayout = std::bit_cast<SerialisationExpectedSuccess>(temp);
    REQUIRE(c1.getInt() == std::bit_cast<int>(serializedLayout.i));
    REQUIRE(c1.getDouble() == std::bit_cast<double>(serializedLayout.d));
  }
}
