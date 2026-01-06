/// Tests for the forward_compatible_t policy - JSON Format
/// Focuses on policy-specific behavior: unknown index handling, monostate fallback,
/// and skipHintAndValue() functionality

#include <string>
#include <variant>

#include <catch2/catch_test_macros.hpp>

#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"
#include "enki/json_reader.hpp"
#include "enki/json_writer.hpp"

// =============================================================================
// Section B: JSON Format - Forward Compat Policy
// =============================================================================

TEST_CASE("JSON forward_compat - known variant index roundtrip", "[policy][forward_compat][json]")
{
  enki::JSONWriter<enki::forward_compatible_t> writer;

  std::variant<int, double, char> value = 42;
  const auto serRes = enki::serialize(value, writer);
  REQUIRE(serRes);

  std::string json = writer.data().str();
  REQUIRE(json == R"({"0": 42})");

  std::variant<int, double, char> deserialized;
  const auto desRes =
    enki::deserialize(deserialized, enki::JSONReader<enki::forward_compatible_t>(json));
  REQUIRE(desRes);
  REQUIRE(std::get<int>(deserialized) == 42);
}

TEST_CASE(
  "JSON forward_compat - unknown index with monostate falls back",
  "[policy][forward_compat][json]")
{
  // Manually create JSON with unknown index (index 3 is unknown for a 3-element variant)
  std::string json = R"({"3": "unknown_value"})";

  // Variant with monostate fallback
  std::variant<int, double, std::monostate> value = 42;
  const auto desRes =
    enki::deserialize(value, enki::JSONReader<enki::forward_compatible_t>(json));
  REQUIRE(desRes);
  REQUIRE(std::holds_alternative<std::monostate>(value));
}

TEST_CASE(
  "JSON forward_compat - unknown index without monostate returns error",
  "[policy][forward_compat][json]")
{
  // Manually create JSON with unknown index
  std::string json = R"({"5": 42})";

  // Variant without monostate
  std::variant<int, double> value;
  const auto desRes =
    enki::deserialize(value, enki::JSONReader<enki::forward_compatible_t>(json));
  REQUIRE_FALSE(desRes);
}

TEST_CASE("JSON forward_compat - skip complex JSON structures", "[policy][forward_compat][json]")
{
  // Unknown index with complex nested value
  std::string json = R"({"5": {"nested": [1, 2, {"deep": "value"}], "other": true}})";

  std::variant<int, double, std::monostate> value = 42;
  const auto desRes =
    enki::deserialize(value, enki::JSONReader<enki::forward_compatible_t>(json));
  REQUIRE(desRes);
  REQUIRE(std::holds_alternative<std::monostate>(value));
}

// =============================================================================
// Section C: skipHintAndValue() Unit Tests - JSON
// =============================================================================

TEST_CASE("JSONReader skipHintAndValue - skips objects", "[policy][skipHintAndValue][json]")
{
  std::string json = R"({"key": "value", "nested": {"a": 1}}, 42)";
  enki::JSONReader<enki::forward_compatible_t> reader(json);

  const auto skipRes = reader.skipHintAndValue();
  REQUIRE(skipRes);

  // Should be able to read the trailing number
  // Note: need to skip comma first
  reader.nextArrayElement(); // consume comma

  int value{};
  const auto readRes = reader.read(value);
  REQUIRE(readRes);
  REQUIRE(value == 42);
}

TEST_CASE("JSONReader skipHintAndValue - skips arrays", "[policy][skipHintAndValue][json]")
{
  std::string json = R"([1, 2, [3, 4], 5], true)";
  enki::JSONReader<enki::forward_compatible_t> reader(json);

  const auto skipRes = reader.skipHintAndValue();
  REQUIRE(skipRes);

  reader.nextArrayElement(); // consume comma

  bool value{};
  const auto readRes = reader.read(value);
  REQUIRE(readRes);
  REQUIRE(value == true);
}

TEST_CASE(
  "JSONReader skipHintAndValue - skips strings with escapes",
  "[policy][skipHintAndValue][json]")
{
  std::string json = R"("hello \"world\" \n\t", 123)";
  enki::JSONReader<enki::forward_compatible_t> reader(json);

  const auto skipRes = reader.skipHintAndValue();
  REQUIRE(skipRes);

  reader.nextArrayElement();

  int value{};
  const auto readRes = reader.read(value);
  REQUIRE(readRes);
  REQUIRE(value == 123);
}

TEST_CASE("JSONReader skipHintAndValue - skips numbers", "[policy][skipHintAndValue][json]")
{
  std::string json = "-123.456e+10, true";
  enki::JSONReader<enki::forward_compatible_t> reader(json);

  const auto skipRes = reader.skipHintAndValue();
  REQUIRE(skipRes);

  reader.nextArrayElement();

  bool value{};
  const auto readRes = reader.read(value);
  REQUIRE(readRes);
  REQUIRE(value == true);
}

TEST_CASE("JSONReader skipHintAndValue - skips null", "[policy][skipHintAndValue][json]")
{
  std::string json = "null, 42";
  enki::JSONReader<enki::forward_compatible_t> reader(json);

  const auto skipRes = reader.skipHintAndValue();
  REQUIRE(skipRes);

  reader.nextArrayElement();

  int value{};
  const auto readRes = reader.read(value);
  REQUIRE(readRes);
  REQUIRE(value == 42);
}
