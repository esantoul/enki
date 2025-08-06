#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

namespace
{
  class FixedPoint
  {
  public:
    FixedPoint() = default;

    FixedPoint(float val) :
      mValue(std::round(std::clamp(val, 0.f, 1.f) * std::numeric_limits<uint16_t>::max()))
    {
    }

    operator float() const
    {
      return mValue * 1.f / std::numeric_limits<uint16_t>::max();
    }

  private:
    uint16_t mValue{};

  public:
    struct EnkiSerial;
  };

  struct FixedPoint::EnkiSerial
  {
    // NOLINTNEXTLINE
    using members = enki::Register<ENKIWRAP_CAST(FixedPoint, mValue, float)>;
  };

  struct S
  {
    struct Nested
    {
      uint32_t a;
      float b;

      constexpr auto operator<=>(const Nested &) const noexcept = default;
    };

    Nested value;

    constexpr auto operator<=>(const S &) const noexcept = default;

    struct EnkiSerial;
  };

  struct S::EnkiSerial
  {
    // NOLINTNEXTLINE
    using members = enki::Register<ENKIWRAP_BITCAST(S, value)>;
  };
} // namespace

TEST_CASE("Conversion SerDes", "[regression]")
{
  // CAST
  {
    enki::BinWriter writer;

    const FixedPoint kValueToSerialize(0.42f);

    const auto serRes = enki::serialize(kValueToSerialize, writer);
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == sizeof(float));

    FixedPoint deserializedValue;
    const auto desRes = enki::deserialize(deserializedValue, enki::BinReader(writer.data()));
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == sizeof(float));

    REQUIRE(kValueToSerialize == deserializedValue);
  }

  // BITCAST
  {
    enki::BinWriter writer;

    static constexpr S kValueToSerialize({42, 0.42f});

    const auto serRes = enki::serialize(kValueToSerialize, writer);
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == sizeof(S));

    S deserializedValue;
    const auto desRes = enki::deserialize(deserializedValue, enki::BinReader(writer.data()));
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == sizeof(S));

    REQUIRE(kValueToSerialize == deserializedValue);
  }
}
