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
    using members = enki::Register<&MyClass::mI, &MyClass::mD>; // NOLINT
  };

  struct SerialisationExpectedSuccess
  {
    std::array<std::byte, sizeof(decltype(MyClass::mI))> i; // MyClass::i should be serialized first
    std::array<std::byte, sizeof(decltype(MyClass::mD))>
      d; // MyClass::d should be serialized second
  };
} // namespace

TEST_CASE("Custom Tagged Type With Member Pointers SerDes", "[regression]")
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

namespace
{
  struct BitStruct
  {
    uint8_t a : 1;
    uint8_t b : 2;
    uint8_t c : 3;
    uint8_t d : 2;

    constexpr auto operator<=>(const BitStruct &) const noexcept = default;

    struct EnkiSerial;
  };

  struct BitStruct::EnkiSerial
  {
    // NOLINTNEXTLINE
    using members = enki::Register<
      ENKIWRAP(BitStruct, b),
      ENKIWRAP(BitStruct, c),
      ENKIWRAP(BitStruct, d),
      ENKIWRAP(BitStruct, a)>;
  };
} // namespace

TEST_CASE("Custom Tagged Type With Bitfields SerDes", "[regression]")
{
  static_assert(sizeof(BitStruct) == 1);

  const BitStruct c1{1, 2, 7, 3};
  BitStruct c2{};

  enki::BinWriter writer;

  const auto serRes = enki::serialize(c1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == 4);

  const auto desRes = enki::deserialize(c2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == 4);

  REQUIRE(c1 == c2);

  // order of registration should be preserved
  {
    std::array<std::byte, 4> serializationOutput;
    std::copy(std::begin(writer.data()), std::end(writer.data()), serializationOutput.begin());
    static constexpr uint32_t kExpectedSerializationData = 0x01'03'07'02;
    REQUIRE(std::bit_cast<uint32_t>(serializationOutput) == kExpectedSerializationData);
  }
}

namespace
{
  struct MyStruct
  {
    uint8_t a : 1;
    uint8_t b : 2;
    uint8_t c : 3;
    uint8_t d : 2;
    uint8_t value;

    constexpr auto operator<=>(const MyStruct &) const noexcept = default;
  };

  struct MyBinWriter : public enki::BinWriter<>
  {
    using enki::BinWriter<>::write;

    constexpr enki::Success write(const MyStruct &v)
    {
      const auto &[a, b, c, d, value] = v;
      return enki::serialize(std::tie(a, b, c, d, value), *this);
    }
  };

  struct MyBinReader : public enki::BinReader<>
  {
    using enki::BinReader<>::BinReader;
    using enki::BinReader<>::read;

    constexpr enki::Success read(MyStruct &v)
    {
      std::tuple<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t> data;
      auto ret = enki::deserialize(data, *this);
      v.a = std::get<0>(data);
      v.b = std::get<1>(data);
      v.c = std::get<2>(data);
      v.d = std::get<3>(data);
      v.value = std::get<4>(data);

      return ret;
    }
  };
} // namespace

TEST_CASE("Custom Type With Custom Writer/Reader SerDes", "[regression]")
{
  const MyStruct c1{1, 2, 7, 3, 0x42};
  MyStruct c2{};

  MyBinWriter writer;

  const auto serRes = enki::serialize(c1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == 5);

  const auto desRes = enki::deserialize(c2, MyBinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == 5);

  REQUIRE(c1 == c2);

  // order of registration should be preserved
  {
    std::array<std::byte, 5> serializationOutput;
    std::copy(std::begin(writer.data()), std::end(writer.data()), serializationOutput.begin());
    static constexpr std::array<uint8_t, 5> kExpectedSerializationData{
      0x01, 0x02, 0x07, 0x03, 0x42};
    REQUIRE(
      std::bit_cast<std::array<uint8_t, 5>>(serializationOutput) == kExpectedSerializationData);
  }
}
