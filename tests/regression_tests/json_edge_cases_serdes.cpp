/**
 * @file json_edge_cases_serdes.cpp
 * @brief Edge case tests for JSON serialization robustness
 *
 * Tests empty containers, nested structures, and variant edge cases for JSON format.
 */

#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <variant>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"
#include "enki/json_reader.hpp"
#include "enki/json_writer.hpp"

// ============================================================================
// SECTION 1: Empty Containers
// ============================================================================

TEST_CASE("Empty vector JSON roundtrip", "[edge_cases][empty][json]")
{
  const std::vector<int32_t> original;
  std::vector<int32_t> restored;

  enki::JSONWriter writer;
  const auto serRes = enki::serialize(original, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(writer.data().str() == "[]");

  const auto desRes = enki::deserialize(restored, enki::JSONReader(writer.data().str()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(restored.empty());
}

TEST_CASE("Empty string JSON roundtrip", "[edge_cases][empty][json]")
{
  const std::string original;
  std::string restored;

  enki::JSONWriter writer;
  const auto serRes = enki::serialize(original, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(writer.data().str() == "\"\"");

  const auto desRes = enki::deserialize(restored, enki::JSONReader(writer.data().str()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(restored.empty());
}

TEST_CASE("Empty set JSON roundtrip", "[edge_cases][empty][json]")
{
  const std::set<int32_t> original;
  std::set<int32_t> restored;

  enki::JSONWriter writer;
  const auto serRes = enki::serialize(original, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(writer.data().str() == "[]");

  const auto desRes = enki::deserialize(restored, enki::JSONReader(writer.data().str()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(restored.empty());
}

TEST_CASE("Empty map JSON roundtrip", "[edge_cases][empty][json]")
{
  const std::map<std::string, int32_t> original;
  std::map<std::string, int32_t> restored;

  enki::JSONWriter writer;
  const auto serRes = enki::serialize(original, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(writer.data().str() == "[]");

  const auto desRes = enki::deserialize(restored, enki::JSONReader(writer.data().str()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(restored.empty());
}

TEST_CASE("Empty optional JSON roundtrip", "[edge_cases][empty][json]")
{
  const std::optional<int32_t> original;
  std::optional<int32_t> restored = 42;

  enki::JSONWriter writer;
  const auto serRes = enki::serialize(original, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(writer.data().str() == "null");

  const auto desRes = enki::deserialize(restored, enki::JSONReader(writer.data().str()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE_FALSE(restored.has_value());
}

// ============================================================================
// SECTION 4: Nested Structures
// ============================================================================

TEST_CASE("Nested vectors JSON roundtrip", "[edge_cases][nested][json]")
{
  const std::vector<std::vector<int32_t>> original = {
    {1, 2, 3},
    {},
    {42},
    {-1, -2, -3, -4, -5}
  };
  std::vector<std::vector<int32_t>> restored;

  enki::JSONWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restored, enki::JSONReader(writer.data().str())).or_throw());
  REQUIRE(restored == original);
}

TEST_CASE("Map with vector values JSON roundtrip", "[edge_cases][nested][json]")
{
  const std::map<std::string, std::vector<int32_t>> original = {
    {   "empty",        {}},
    {  "single",      {42}},
    {"multiple", {1, 2, 3}}
  };
  std::map<std::string, std::vector<int32_t>> restored;

  enki::JSONWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restored, enki::JSONReader(writer.data().str())).or_throw());
  REQUIRE(restored == original);
}

TEST_CASE("Deeply nested structure (5 levels) JSON roundtrip", "[edge_cases][nested][json]")
{
  using Level5 = int32_t;
  using Level4 = std::vector<Level5>;
  using Level3 = std::vector<Level4>;
  using Level2 = std::vector<Level3>;
  using Level1 = std::vector<Level2>;

  const Level1 original = {{{{{{1, 2}}, {3}}}}, {{{42}}}};
  Level1 restored;

  enki::JSONWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restored, enki::JSONReader(writer.data().str())).or_throw());
  REQUIRE(restored == original);
}

TEST_CASE("Vector of optionals JSON roundtrip", "[edge_cases][nested][json]")
{
  const std::vector<std::optional<int32_t>> original = {std::nullopt, 42, std::nullopt, -1, 0};
  std::vector<std::optional<int32_t>> restored;

  enki::JSONWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restored, enki::JSONReader(writer.data().str())).or_throw());
  REQUIRE(restored == original);
}

// ============================================================================
// SECTION 5: Variant Edge Cases
// ============================================================================

TEST_CASE("Variant with many alternatives JSON roundtrip", "[edge_cases][variant][json]")
{
  using BigVariant = std::variant<
    int8_t,
    int16_t,
    int32_t,
    int64_t,
    uint8_t,
    uint16_t,
    uint32_t,
    uint64_t,
    float,
    double,
    std::string>;

  SECTION("First alternative")
  {
    const BigVariant original = int8_t{-42};
    BigVariant restored;

    enki::JSONWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
    REQUIRE_NOTHROW(enki::deserialize(restored, enki::JSONReader(writer.data().str())).or_throw());
    REQUIRE(std::get<int8_t>(restored) == -42);
  }

  SECTION("Last alternative")
  {
    const BigVariant original = std::string{"hello"};
    BigVariant restored;

    enki::JSONWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
    REQUIRE_NOTHROW(enki::deserialize(restored, enki::JSONReader(writer.data().str())).or_throw());
    REQUIRE(std::get<std::string>(restored) == "hello");
  }
}

TEST_CASE("Nested variant JSON roundtrip", "[edge_cases][variant][json]")
{
  using InnerVariant = std::variant<int32_t, std::string>;
  using OuterVariant = std::variant<InnerVariant, double>;

  SECTION("Inner int")
  {
    const OuterVariant original = InnerVariant{42};
    OuterVariant restored;

    enki::JSONWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
    REQUIRE_NOTHROW(enki::deserialize(restored, enki::JSONReader(writer.data().str())).or_throw());

    REQUIRE(std::holds_alternative<InnerVariant>(restored));
    REQUIRE(std::get<int32_t>(std::get<InnerVariant>(restored)) == 42);
  }

  SECTION("Outer double")
  {
    const OuterVariant original = 3.14159;
    OuterVariant restored;

    enki::JSONWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
    REQUIRE_NOTHROW(enki::deserialize(restored, enki::JSONReader(writer.data().str())).or_throw());

    REQUIRE(std::holds_alternative<double>(restored));
    REQUIRE(std::get<double>(restored) == 3.14159);
  }
}

// ============================================================================
// SECTION 6: Single Element Containers
// ============================================================================

TEST_CASE("Single element vector JSON roundtrip", "[edge_cases][single][json]")
{
  const std::vector<int32_t> original = {42};
  std::vector<int32_t> restored;

  enki::JSONWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
  REQUIRE(writer.data().str() == "[42]");

  REQUIRE_NOTHROW(enki::deserialize(restored, enki::JSONReader(writer.data().str())).or_throw());
  REQUIRE(restored.size() == 1);
  REQUIRE(restored[0] == 42);
}
