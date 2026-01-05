/**
 * @file json_error_handling_serdes.cpp
 * @brief Tests for JSON error handling and malformed data
 *
 * Tests behavior when deserializing:
 * - Invalid JSON syntax
 * - Type mismatches
 * - Overflow/underflow
 * - Skip functionality
 */

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"
#include "enki/json_reader.hpp"
#include "enki/json_writer.hpp"

// ============================================================================
// SECTION 1: Invalid JSON Syntax
// ============================================================================

TEST_CASE("Invalid JSON - malformed bool", "[error_handling][json]")
{
  bool restored{};

  // "tru" instead of "true"
  auto result = enki::deserialize(restored, enki::JSONReader("tru"));
  REQUIRE_FALSE(result); // Should fail
  REQUIRE(result.error() != nullptr);
}

TEST_CASE("Invalid JSON - variant with non-numeric index", "[error_handling][json]")
{
  using Var = std::variant<int32_t, std::string>;
  Var restored;

  // Index should be numeric, not string
  auto result = enki::deserialize(restored, enki::JSONReader("{\"abc\": 42}"));
  REQUIRE_FALSE(result); // Should fail parsing "abc" as index
  REQUIRE(result.error() != nullptr);
}

TEST_CASE("Invalid JSON - variant missing colon", "[error_handling][json]")
{
  using Var = std::variant<int32_t, std::string>;
  Var restored;

  // Missing colon after index
  auto result = enki::deserialize(restored, enki::JSONReader("{\"0\" 42}"));
  REQUIRE_FALSE(result);
  REQUIRE(result.error() != nullptr);
}

TEST_CASE("Invalid JSON - variant missing opening brace", "[error_handling][json]")
{
  using Var = std::variant<int32_t, std::string>;
  Var restored;

  auto result = enki::deserialize(restored, enki::JSONReader("\"0\": 42}"));
  REQUIRE_FALSE(result);
  REQUIRE(result.error() != nullptr);
}

TEST_CASE("Invalid JSON - monostate expects null", "[error_handling][json]")
{
  std::monostate restored;

  // Should be "null", not a number
  auto result = enki::deserialize(restored, enki::JSONReader("42"));
  REQUIRE_FALSE(result);
  REQUIRE(result.error() != nullptr);
}

TEST_CASE("Monostate reads null from quoted string (documents behavior)", "[error_handling][json]")
{
  std::monostate restored;

  // Note: The library reads "null" from inside quotes because readWord
  // skips non-alphanumeric characters including quotes.
  // This documents current behavior - the reader extracts "null" from "\"null\""
  auto result = enki::deserialize(restored, enki::JSONReader("\"null\""));
  REQUIRE(result); // Actually succeeds because readWord extracts "null"
}

TEST_CASE("Invalid JSON - optional expects null or value", "[error_handling][json]")
{
  std::optional<int32_t> restored;

  // "nul" instead of "null"
  auto result = enki::deserialize(restored, enki::JSONReader("nul"));
  REQUIRE_FALSE(result);
}

// ============================================================================
// SECTION 2: Type Mismatches
// ============================================================================

TEST_CASE("Type mismatch - wrong variant index (JSON strict)", "[error_handling][json]")
{
  using SmallVar = std::variant<int32_t, std::string>;
  SmallVar restored;

  // Index 99 is out of range for 2-alternative variant
  auto result = enki::deserialize(restored, enki::JSONReader("{\"99\": 42}"));
  REQUIRE_FALSE(result);
  REQUIRE(result.error() != nullptr);
}

// ============================================================================
// SECTION 3: Forward Compatibility Error Cases
// ============================================================================

TEST_CASE("JSON forward compat - unknown index without monostate fails", "[error_handling][json]")
{
  using NoFallbackVar = std::variant<int32_t, std::string>;
  NoFallbackVar restored;

  auto result = enki::deserialize(
    restored, enki::JSONReader<enki::forward_compatible_t>("{\"99\": 42}"));

  REQUIRE_FALSE(result);
  REQUIRE(result.error() != nullptr);
}

TEST_CASE("JSON forward compat - unknown index with monostate succeeds", "[error_handling][json]")
{
  using FallbackVar = std::variant<std::monostate, int32_t, std::string>;
  FallbackVar restored = int32_t{123}; // Pre-populate

  auto result = enki::deserialize(
    restored, enki::JSONReader<enki::forward_compatible_t>("{\"99\": \"unknown data\"}"));

  REQUIRE(result);
  REQUIRE(std::holds_alternative<std::monostate>(restored));
}

// ============================================================================
// SECTION 4: Overflow/Underflow Cases
// ============================================================================

TEST_CASE("JSON large number into small type", "[error_handling][json]")
{
  int8_t restored{};

  // 256 overflows int8_t
  auto result = enki::deserialize(restored, enki::JSONReader("256"));
  // JSON reader casts, so this documents overflow behavior
  // Value will be truncated (256 -> 0 for int8_t)
  REQUIRE(result); // Operation succeeds but value is truncated
}

TEST_CASE("JSON negative number into unsigned type", "[error_handling][json]")
{
  uint32_t restored{};

  auto result = enki::deserialize(restored, enki::JSONReader("-1"));
  // Documents behavior: casting negative to unsigned
  REQUIRE(result); // Operation succeeds, value wraps
}

// ============================================================================
// SECTION 5: Nested Error Propagation
// ============================================================================

TEST_CASE("Error in nested vector element", "[error_handling][json]")
{
  using Var = std::variant<int32_t, std::string>;
  std::vector<Var> restored;

  // Second element has invalid variant index
  auto result = enki::deserialize(
    restored, enki::JSONReader("[{\"0\": 42}, {\"99\": \"invalid\"}]"));

  REQUIRE_FALSE(result); // Should fail on second element
}

// ============================================================================
// SECTION 6: Skip functionality tests
// ============================================================================

TEST_CASE("JSONReader skipHintAndValue handles invalid JSON gracefully", "[error_handling][json]")
{
  enki::JSONReader reader("");
  auto result = reader.skipHintAndValue();
  REQUIRE_FALSE(result);
  REQUIRE(result.error() != nullptr);
}

TEST_CASE("JSONReader skip recognizes all JSON value types", "[error_handling][json]")
{
  // Test skipping object
  {
    enki::JSONReader reader("{\"key\": \"value\"}");
    auto result = reader.skipHintAndValue();
    REQUIRE(result);
  }

  // Test skipping array
  {
    enki::JSONReader reader("[1, 2, 3]");
    auto result = reader.skipHintAndValue();
    REQUIRE(result);
  }

  // Test skipping string
  {
    enki::JSONReader reader("\"hello\"");
    auto result = reader.skipHintAndValue();
    REQUIRE(result);
  }

  // Test skipping number
  {
    enki::JSONReader reader("42.5e-3");
    auto result = reader.skipHintAndValue();
    REQUIRE(result);
  }

  // Test skipping boolean true
  {
    enki::JSONReader reader("true");
    auto result = reader.skipHintAndValue();
    REQUIRE(result);
  }

  // Test skipping boolean false
  {
    enki::JSONReader reader("false");
    auto result = reader.skipHintAndValue();
    REQUIRE(result);
  }

  // Test skipping null
  {
    enki::JSONReader reader("null");
    auto result = reader.skipHintAndValue();
    REQUIRE(result);
  }
}

TEST_CASE("JSONReader skip handles nested structures", "[error_handling][json]")
{
  // Deeply nested object
  {
    enki::JSONReader reader("{\"a\": {\"b\": {\"c\": 42}}}");
    auto result = reader.skipHintAndValue();
    REQUIRE(result);
  }

  // Array containing objects
  {
    enki::JSONReader reader("[{\"x\": 1}, {\"y\": 2}]");
    auto result = reader.skipHintAndValue();
    REQUIRE(result);
  }

  // Mixed nesting
  {
    enki::JSONReader reader("{\"arr\": [1, [2, 3]], \"obj\": {\"nested\": true}}");
    auto result = reader.skipHintAndValue();
    REQUIRE(result);
  }
}

TEST_CASE("JSONReader skip handles strings with escapes", "[error_handling][json]")
{
  // String with quotes inside
  {
    enki::JSONReader reader("\"hello \\\"world\\\"\"");
    auto result = reader.skipHintAndValue();
    REQUIRE(result);
  }

  // String with backslashes
  {
    enki::JSONReader reader("\"path\\\\to\\\\file\"");
    auto result = reader.skipHintAndValue();
    REQUIRE(result);
  }
}

// ============================================================================
// SECTION 7: Variant error messages
// ============================================================================

TEST_CASE("Variant with invalid JSON closing brace", "[error_handling][json]")
{
  using Var = std::variant<int32_t, std::string>;
  Var restored;

  // Missing closing brace
  auto result = enki::deserialize(restored, enki::JSONReader("{\"0\": 42"));
  REQUIRE_FALSE(result);
}
