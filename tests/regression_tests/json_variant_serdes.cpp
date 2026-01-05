#include <string>
#include <variant>

#include <catch2/catch_test_macros.hpp>

#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"
#include "enki/json_reader.hpp"
#include "enki/json_writer.hpp"

// =============================================================================
// JSON Format - Basic Variant Tests
// =============================================================================

TEST_CASE("JSON strict - known variant index roundtrip", "[variant][strict][json]")
{
  enki::JSONWriter<enki::strict_t> writer;

  // Use 1.5 instead of 3.14 because 1.5 can be represented exactly in binary floating-point
  std::variant<int, double, char> value = 1.5;
  const auto serRes = enki::serialize(value, writer);
  REQUIRE(serRes);

  // Verify JSON format is {"index": value}
  std::string json = writer.data().str();
  REQUIRE(json == R"({"1": 1.5})");

  std::variant<int, double, char> deserialized;
  const auto desRes = enki::deserialize(deserialized, enki::JSONReader<enki::strict_t>(json));
  REQUIRE(desRes);
  REQUIRE(std::get<double>(deserialized) == 1.5);
}

TEST_CASE("JSON strict - variant with int value roundtrip", "[variant][strict][json]")
{
  enki::JSONWriter<enki::strict_t> writer;

  std::variant<int, double> value = 42;
  const auto serRes = enki::serialize(value, writer);
  REQUIRE(serRes);

  std::string json = writer.data().str();
  REQUIRE(json == R"({"0": 42})");

  std::variant<int, double> deserialized;
  const auto desRes = enki::deserialize(deserialized, enki::JSONReader<enki::strict_t>(json));
  REQUIRE(desRes);
  REQUIRE(std::get<int>(deserialized) == 42);
}

TEST_CASE("JSON strict - variant with string value roundtrip", "[variant][strict][json]")
{
  enki::JSONWriter<enki::strict_t> writer;

  std::variant<int, std::string> value = std::string("hello");
  const auto serRes = enki::serialize(value, writer);
  REQUIRE(serRes);

  std::string json = writer.data().str();
  REQUIRE(json == R"({"1": "hello"})");

  std::variant<int, std::string> deserialized;
  const auto desRes = enki::deserialize(deserialized, enki::JSONReader<enki::strict_t>(json));
  REQUIRE(desRes);
  REQUIRE(std::get<std::string>(deserialized) == "hello");
}

TEST_CASE("JSON strict - unknown variant index returns error", "[variant][strict][json]")
{
  // Manually create JSON with unknown index
  std::string json = R"({"5": 42})";

  std::variant<int, double> value;
  const auto desRes = enki::deserialize(value, enki::JSONReader<enki::strict_t>(json));
  REQUIRE_FALSE(desRes);
}

TEST_CASE("JSON - monostate serialization roundtrip", "[variant][json]")
{
  enki::JSONWriter<enki::strict_t> writer;

  std::variant<int, std::monostate> value = std::monostate{};
  const auto serRes = enki::serialize(value, writer);
  REQUIRE(serRes);

  std::string json = writer.data().str();
  REQUIRE(json == R"({"1": null})");

  // Test deserialization roundtrip
  std::variant<int, std::monostate> deserialized = 42;
  const auto desRes = enki::deserialize(deserialized, enki::JSONReader<enki::strict_t>(json));
  REQUIRE(desRes);
  REQUIRE(std::holds_alternative<std::monostate>(deserialized));
}

TEST_CASE("JSON - nested variant roundtrip", "[variant][json][nested_variant]")
{
  enki::JSONWriter<enki::strict_t> writer;

  using InnerVariant = std::variant<char, double>;
  using OuterVariant = std::variant<int, InnerVariant>;

  OuterVariant value = InnerVariant{'X'};
  const auto serRes = enki::serialize(value, writer);
  REQUIRE(serRes);

  std::string json = writer.data().str();
  // Outer index 1 (InnerVariant), inner index 0 (char 'X' = 88)
  REQUIRE(json == R"({"1": {"0": 88}})");

  OuterVariant deserialized;
  const auto desRes = enki::deserialize(deserialized, enki::JSONReader<enki::strict_t>(json));
  REQUIRE(desRes);
  REQUIRE(std::holds_alternative<InnerVariant>(deserialized));
  REQUIRE(std::get<char>(std::get<InnerVariant>(deserialized)) == 'X');
}
