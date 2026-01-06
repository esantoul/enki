#include <string>
#include <variant>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

// =============================================================================
// Binary Format - Basic Variant Tests
// =============================================================================

TEST_CASE("Variant SerDes", "[regression]")
{
  enki::BinWriter writer;

  static constexpr std::variant<char, double, float, int> kValueToSerialize = 3.14;
  const auto serRes = enki::serialize(kValueToSerialize, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sizeof(enki::BinWriter<>::size_type) + sizeof(double));

  std::remove_cvref_t<decltype(kValueToSerialize)> deserializedValue;
  const auto desRes = enki::deserialize(deserializedValue, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == sizeof(enki::BinWriter<>::size_type) + sizeof(double));

  REQUIRE(deserializedValue == kValueToSerialize);
}

TEST_CASE("Variant (std::monostate) SerDes", "[regression]")
{
  enki::BinWriter writer;

  static constexpr std::variant<char, double, float, std::monostate> kValueToSerialize =
    std::monostate{};
  const auto serRes = enki::serialize(kValueToSerialize, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sizeof(enki::BinWriter<>::size_type));

  std::remove_cvref_t<decltype(kValueToSerialize)> deserializedValue;
  const auto desRes = enki::deserialize(deserializedValue, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == sizeof(enki::BinWriter<>::size_type));

  REQUIRE(deserializedValue == kValueToSerialize);
}

TEST_CASE("Binary strict - known variant index roundtrip", "[variant][strict][binary]")
{
  enki::BinWriter<enki::strict_t> writer;

  std::variant<int, double, char> value = 3.14;
  const auto serRes = enki::serialize(value, writer);
  REQUIRE(serRes);
  // Strict: index (4 bytes) + data (8 bytes for double)
  REQUIRE(serRes.size() == sizeof(uint32_t) + sizeof(double));

  std::variant<int, double, char> deserialized;
  const auto desRes =
    enki::deserialize(deserialized, enki::BinSpanReader<enki::strict_t>(writer.data()));
  REQUIRE(desRes);
  REQUIRE(std::get<double>(deserialized) == 3.14);
}

TEST_CASE("Binary strict - unknown variant index returns error", "[variant][strict][binary]")
{
  // Serialize with a variant that has more alternatives
  using NewVariant = std::variant<int, double, std::string>;
  enki::BinWriter<enki::strict_t> writer;

  NewVariant newValue = std::string("unknown"); // index 2
  const auto serRes = enki::serialize(newValue, writer);
  REQUIRE(serRes);

  // Try to deserialize into a variant with fewer alternatives
  using OldVariant = std::variant<int, double>;
  OldVariant oldValue;
  const auto desRes =
    enki::deserialize(oldValue, enki::BinSpanReader<enki::strict_t>(writer.data()));
  REQUIRE_FALSE(desRes);
}

// =============================================================================
// Binary Format - Nested Variant Tests
// =============================================================================

TEST_CASE("Strict - nested variant roundtrip", "[variant][strict][nested_variant]")
{
  enki::BinWriter<enki::strict_t> writer;

  using InnerVariant = std::variant<char, double>;
  using OuterVariant = std::variant<int, InnerVariant>;

  OuterVariant value = InnerVariant{'X'};
  const auto serRes = enki::serialize(value, writer);
  REQUIRE(serRes);

  OuterVariant deserialized;
  const auto desRes =
    enki::deserialize(deserialized, enki::BinSpanReader<enki::strict_t>(writer.data()));
  REQUIRE(desRes);
  REQUIRE(std::holds_alternative<InnerVariant>(deserialized));
  REQUIRE(std::get<char>(std::get<InnerVariant>(deserialized)) == 'X');
}

TEST_CASE(
  "Strict - nested variant unknown outer index returns error",
  "[variant][strict][nested_variant]")
{
  // Serialize with a variant that has more outer alternatives
  using NewInner = std::variant<char, double>;
  using NewOuter = std::variant<int, NewInner, std::string>;

  enki::BinWriter<enki::strict_t> writer;
  NewOuter newValue = std::string("unknown"); // index 2
  const auto serRes = enki::serialize(newValue, writer);
  REQUIRE(serRes);

  // Old version has fewer outer alternatives
  using OldInner = std::variant<char, double>;
  using OldOuter = std::variant<int, OldInner>;

  OldOuter oldValue;
  const auto desRes =
    enki::deserialize(oldValue, enki::BinSpanReader<enki::strict_t>(writer.data()));
  REQUIRE_FALSE(desRes);
}
