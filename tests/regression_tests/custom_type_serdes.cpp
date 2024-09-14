#include <array>
#include <bit>
#include <cstddef>
#include <tuple>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

namespace
{
  class MyClass
  {
  public:
    static constexpr size_t kDataSize = sizeof(double) + sizeof(int);

    constexpr MyClass() = default;

    constexpr MyClass(double dd, int ii) :
      mD(dd),
      mI(ii)
    {
    }

    constexpr auto operator<=>(const MyClass &) const noexcept = default;

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

TEST_CASE("Custom Type SerDes", "[manager][regression]")
{
  const MyClass c1{3.14, 42};
  MyClass c2{};

  enki::BinWriter writer;

  const auto serRes = enki::serialize(c1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == MyClass::kDataSize);

  const auto desRes = enki::deserialize(c2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == MyClass::kDataSize);

  REQUIRE(c1 == c2);

  // order of registration should be preserved
  {
    std::array<std::byte, MyClass::kDataSize> serializationOutput;
    std::copy(std::begin(writer.data()), std::end(writer.data()), serializationOutput.begin());
    const auto &serializedLayout = std::bit_cast<SerialisationExpectedSuccess>(serializationOutput);
    REQUIRE(c1.getInt() == std::bit_cast<int>(serializedLayout.i));
    REQUIRE(c1.getDouble() == std::bit_cast<double>(serializedLayout.d));
  }
}
