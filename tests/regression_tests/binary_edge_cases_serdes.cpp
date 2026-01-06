/**
 * @file binary_edge_cases_serdes.cpp
 * @brief Edge case tests for binary serialization robustness
 *
 * Tests empty containers, boundary values, nested structures,
 * and variant edge cases for binary format.
 */

#include <cmath>
#include <cstdint>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <variant>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

// ============================================================================
// SECTION 1: Empty Containers
// ============================================================================

TEST_CASE("Empty vector binary roundtrip", "[edge_cases][empty]")
{
  const std::vector<int32_t> original;
  std::vector<int32_t> restored;

  enki::BinWriter writer;
  const auto serRes = enki::serialize(original, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sizeof(uint32_t)); // Just the size field

  const auto desRes = enki::deserialize(restored, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(restored.empty());
}

TEST_CASE("Empty string binary roundtrip", "[edge_cases][empty]")
{
  const std::string original;
  std::string restored;

  enki::BinWriter writer;
  const auto serRes = enki::serialize(original, writer);
  REQUIRE_NOTHROW(serRes.or_throw());

  const auto desRes = enki::deserialize(restored, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(restored.empty());
}

TEST_CASE("Empty set binary roundtrip", "[edge_cases][empty]")
{
  const std::set<int32_t> original;
  std::set<int32_t> restored;

  enki::BinWriter writer;
  const auto serRes = enki::serialize(original, writer);
  REQUIRE_NOTHROW(serRes.or_throw());

  const auto desRes = enki::deserialize(restored, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(restored.empty());
}

TEST_CASE("Empty map binary roundtrip", "[edge_cases][empty]")
{
  const std::map<std::string, int32_t> original;
  std::map<std::string, int32_t> restored;

  enki::BinWriter writer;
  const auto serRes = enki::serialize(original, writer);
  REQUIRE_NOTHROW(serRes.or_throw());

  const auto desRes = enki::deserialize(restored, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(restored.empty());
}

TEST_CASE("Empty optional binary roundtrip", "[edge_cases][empty]")
{
  const std::optional<int32_t> original;
  std::optional<int32_t> restored = 42; // Pre-populate to verify it gets cleared

  enki::BinWriter writer;
  const auto serRes = enki::serialize(original, writer);
  REQUIRE_NOTHROW(serRes.or_throw());

  const auto desRes = enki::deserialize(restored, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE_FALSE(restored.has_value());
}

// ============================================================================
// SECTION 2: Boundary Values - Integers
// ============================================================================

TEST_CASE("int8_t boundary values binary roundtrip", "[edge_cases][boundary]")
{
  constexpr int8_t minVal = std::numeric_limits<int8_t>::min();
  constexpr int8_t maxVal = std::numeric_limits<int8_t>::max();

  int8_t restoredMin{}, restoredMax{};

  enki::BinWriter writerMin, writerMax;
  REQUIRE_NOTHROW(enki::serialize(minVal, writerMin).or_throw());
  REQUIRE_NOTHROW(enki::serialize(maxVal, writerMax).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restoredMin, enki::BinReader(writerMin.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredMax, enki::BinReader(writerMax.data())).or_throw());

  REQUIRE(restoredMin == minVal);
  REQUIRE(restoredMax == maxVal);
}

TEST_CASE("int16_t boundary values binary roundtrip", "[edge_cases][boundary]")
{
  constexpr int16_t minVal = std::numeric_limits<int16_t>::min();
  constexpr int16_t maxVal = std::numeric_limits<int16_t>::max();

  int16_t restoredMin{}, restoredMax{};

  enki::BinWriter writerMin, writerMax;
  REQUIRE_NOTHROW(enki::serialize(minVal, writerMin).or_throw());
  REQUIRE_NOTHROW(enki::serialize(maxVal, writerMax).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restoredMin, enki::BinReader(writerMin.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredMax, enki::BinReader(writerMax.data())).or_throw());

  REQUIRE(restoredMin == minVal);
  REQUIRE(restoredMax == maxVal);
}

TEST_CASE("int32_t boundary values binary roundtrip", "[edge_cases][boundary]")
{
  constexpr int32_t minVal = std::numeric_limits<int32_t>::min();
  constexpr int32_t maxVal = std::numeric_limits<int32_t>::max();

  int32_t restoredMin{}, restoredMax{};

  enki::BinWriter writerMin, writerMax;
  REQUIRE_NOTHROW(enki::serialize(minVal, writerMin).or_throw());
  REQUIRE_NOTHROW(enki::serialize(maxVal, writerMax).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restoredMin, enki::BinReader(writerMin.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredMax, enki::BinReader(writerMax.data())).or_throw());

  REQUIRE(restoredMin == minVal);
  REQUIRE(restoredMax == maxVal);
}

TEST_CASE("int64_t boundary values binary roundtrip", "[edge_cases][boundary]")
{
  constexpr int64_t minVal = std::numeric_limits<int64_t>::min();
  constexpr int64_t maxVal = std::numeric_limits<int64_t>::max();

  int64_t restoredMin{}, restoredMax{};

  enki::BinWriter writerMin, writerMax;
  REQUIRE_NOTHROW(enki::serialize(minVal, writerMin).or_throw());
  REQUIRE_NOTHROW(enki::serialize(maxVal, writerMax).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restoredMin, enki::BinReader(writerMin.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredMax, enki::BinReader(writerMax.data())).or_throw());

  REQUIRE(restoredMin == minVal);
  REQUIRE(restoredMax == maxVal);
}

TEST_CASE("uint8_t boundary values binary roundtrip", "[edge_cases][boundary]")
{
  constexpr uint8_t minVal = std::numeric_limits<uint8_t>::min();
  constexpr uint8_t maxVal = std::numeric_limits<uint8_t>::max();

  uint8_t restoredMin{}, restoredMax{};

  enki::BinWriter writerMin, writerMax;
  REQUIRE_NOTHROW(enki::serialize(minVal, writerMin).or_throw());
  REQUIRE_NOTHROW(enki::serialize(maxVal, writerMax).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restoredMin, enki::BinReader(writerMin.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredMax, enki::BinReader(writerMax.data())).or_throw());

  REQUIRE(restoredMin == minVal);
  REQUIRE(restoredMax == maxVal);
}

TEST_CASE("uint64_t boundary values binary roundtrip", "[edge_cases][boundary]")
{
  constexpr uint64_t minVal = std::numeric_limits<uint64_t>::min();
  constexpr uint64_t maxVal = std::numeric_limits<uint64_t>::max();

  uint64_t restoredMin{}, restoredMax{};

  enki::BinWriter writerMin, writerMax;
  REQUIRE_NOTHROW(enki::serialize(minVal, writerMin).or_throw());
  REQUIRE_NOTHROW(enki::serialize(maxVal, writerMax).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restoredMin, enki::BinReader(writerMin.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredMax, enki::BinReader(writerMax.data())).or_throw());

  REQUIRE(restoredMin == minVal);
  REQUIRE(restoredMax == maxVal);
}

// ============================================================================
// SECTION 3: Boundary Values - Floating Point
// ============================================================================

TEST_CASE("float special values binary roundtrip", "[edge_cases][boundary][float]")
{
  const float posInf = std::numeric_limits<float>::infinity();
  const float negInf = -std::numeric_limits<float>::infinity();
  const float nan = std::numeric_limits<float>::quiet_NaN();
  const float negZero = -0.0f;
  const float denorm = std::numeric_limits<float>::denorm_min();
  const float maxVal = std::numeric_limits<float>::max();
  const float minVal = std::numeric_limits<float>::lowest();

  float restoredPosInf{}, restoredNegInf{}, restoredNaN{};
  float restoredNegZero{}, restoredDenorm{}, restoredMax{}, restoredMin{};

  enki::BinWriter w1, w2, w3, w4, w5, w6, w7;
  REQUIRE_NOTHROW(enki::serialize(posInf, w1).or_throw());
  REQUIRE_NOTHROW(enki::serialize(negInf, w2).or_throw());
  REQUIRE_NOTHROW(enki::serialize(nan, w3).or_throw());
  REQUIRE_NOTHROW(enki::serialize(negZero, w4).or_throw());
  REQUIRE_NOTHROW(enki::serialize(denorm, w5).or_throw());
  REQUIRE_NOTHROW(enki::serialize(maxVal, w6).or_throw());
  REQUIRE_NOTHROW(enki::serialize(minVal, w7).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restoredPosInf, enki::BinReader(w1.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredNegInf, enki::BinReader(w2.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredNaN, enki::BinReader(w3.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredNegZero, enki::BinReader(w4.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredDenorm, enki::BinReader(w5.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredMax, enki::BinReader(w6.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredMin, enki::BinReader(w7.data())).or_throw());

  REQUIRE(std::isinf(restoredPosInf));
  REQUIRE(restoredPosInf > 0);
  REQUIRE(std::isinf(restoredNegInf));
  REQUIRE(restoredNegInf < 0);
  REQUIRE(std::isnan(restoredNaN));
  REQUIRE(restoredNegZero == 0.0f);
  REQUIRE(std::signbit(restoredNegZero)); // Verify negative zero
  REQUIRE(restoredDenorm == denorm);
  REQUIRE(restoredMax == maxVal);
  REQUIRE(restoredMin == minVal);
}

TEST_CASE("double special values binary roundtrip", "[edge_cases][boundary][float]")
{
  const double posInf = std::numeric_limits<double>::infinity();
  const double negInf = -std::numeric_limits<double>::infinity();
  const double nan = std::numeric_limits<double>::quiet_NaN();
  const double negZero = -0.0;
  const double denorm = std::numeric_limits<double>::denorm_min();

  double restoredPosInf{}, restoredNegInf{}, restoredNaN{};
  double restoredNegZero{}, restoredDenorm{};

  enki::BinWriter w1, w2, w3, w4, w5;
  REQUIRE_NOTHROW(enki::serialize(posInf, w1).or_throw());
  REQUIRE_NOTHROW(enki::serialize(negInf, w2).or_throw());
  REQUIRE_NOTHROW(enki::serialize(nan, w3).or_throw());
  REQUIRE_NOTHROW(enki::serialize(negZero, w4).or_throw());
  REQUIRE_NOTHROW(enki::serialize(denorm, w5).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restoredPosInf, enki::BinReader(w1.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredNegInf, enki::BinReader(w2.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredNaN, enki::BinReader(w3.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredNegZero, enki::BinReader(w4.data())).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restoredDenorm, enki::BinReader(w5.data())).or_throw());

  REQUIRE(std::isinf(restoredPosInf));
  REQUIRE(restoredPosInf > 0);
  REQUIRE(std::isinf(restoredNegInf));
  REQUIRE(restoredNegInf < 0);
  REQUIRE(std::isnan(restoredNaN));
  REQUIRE(restoredNegZero == 0.0);
  REQUIRE(std::signbit(restoredNegZero));
  REQUIRE(restoredDenorm == denorm);
}

// ============================================================================
// SECTION 4: Nested Structures
// ============================================================================

TEST_CASE("Nested vectors binary roundtrip", "[edge_cases][nested]")
{
  const std::vector<std::vector<int32_t>> original = {
    {1, 2, 3},
    {},
    {42},
    {-1, -2, -3, -4, -5}
  };
  std::vector<std::vector<int32_t>> restored;

  enki::BinWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());
  REQUIRE(restored == original);
}

TEST_CASE("Map with vector values binary roundtrip", "[edge_cases][nested]")
{
  const std::map<std::string, std::vector<int32_t>> original = {
    {   "empty",        {}},
    {  "single",      {42}},
    {"multiple", {1, 2, 3}}
  };
  std::map<std::string, std::vector<int32_t>> restored;

  enki::BinWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());
  REQUIRE(restored == original);
}

TEST_CASE("Deeply nested structure (5 levels) binary roundtrip", "[edge_cases][nested]")
{
  using Level5 = int32_t;
  using Level4 = std::vector<Level5>;
  using Level3 = std::vector<Level4>;
  using Level2 = std::vector<Level3>;
  using Level1 = std::vector<Level2>;

  const Level1 original = {{{{{{1, 2}}, {3}}}}, {{{42}}}};
  Level1 restored;

  enki::BinWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());
  REQUIRE(restored == original);
}

TEST_CASE("Vector of optionals binary roundtrip", "[edge_cases][nested]")
{
  const std::vector<std::optional<int32_t>> original = {std::nullopt, 42, std::nullopt, -1, 0};
  std::vector<std::optional<int32_t>> restored;

  enki::BinWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());

  REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());
  REQUIRE(restored == original);
}

TEST_CASE("Optional of vector binary roundtrip", "[edge_cases][nested]")
{
  SECTION("With value")
  {
    const std::optional<std::vector<int32_t>> original = std::vector<int32_t>{1, 2, 3};
    std::optional<std::vector<int32_t>> restored;

    enki::BinWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());

    REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());
    REQUIRE(restored == original);
  }

  SECTION("Empty optional")
  {
    const std::optional<std::vector<int32_t>> original;
    std::optional<std::vector<int32_t>> restored = std::vector<int32_t>{99};

    enki::BinWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());

    REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());
    REQUIRE_FALSE(restored.has_value());
  }

  SECTION("Optional with empty vector")
  {
    const std::optional<std::vector<int32_t>> original = std::vector<int32_t>{};
    std::optional<std::vector<int32_t>> restored;

    enki::BinWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());

    REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());
    REQUIRE(restored.has_value());
    REQUIRE(restored->empty());
  }
}

// ============================================================================
// SECTION 5: Variant Edge Cases
// ============================================================================

TEST_CASE("Variant with many alternatives binary roundtrip", "[edge_cases][variant]")
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

    enki::BinWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
    REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());
    REQUIRE(std::get<int8_t>(restored) == -42);
  }

  SECTION("Last alternative")
  {
    const BigVariant original = std::string{"hello"};
    BigVariant restored;

    enki::BinWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
    REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());
    REQUIRE(std::get<std::string>(restored) == "hello");
  }

  SECTION("Middle alternative (uint32_t)")
  {
    const BigVariant original = uint32_t{1'234'567'890};
    BigVariant restored;

    enki::BinWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
    REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());
    REQUIRE(std::get<uint32_t>(restored) == 1'234'567'890);
  }
}

TEST_CASE("Nested variant binary roundtrip", "[edge_cases][variant]")
{
  using InnerVariant = std::variant<int32_t, std::string>;
  using OuterVariant = std::variant<InnerVariant, double>;

  SECTION("Inner int")
  {
    const OuterVariant original = InnerVariant{42};
    OuterVariant restored;

    enki::BinWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
    REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());

    REQUIRE(std::holds_alternative<InnerVariant>(restored));
    REQUIRE(std::get<int32_t>(std::get<InnerVariant>(restored)) == 42);
  }

  SECTION("Inner string")
  {
    const OuterVariant original = InnerVariant{std::string{"nested"}};
    OuterVariant restored;

    enki::BinWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
    REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());

    REQUIRE(std::holds_alternative<InnerVariant>(restored));
    REQUIRE(std::get<std::string>(std::get<InnerVariant>(restored)) == "nested");
  }

  SECTION("Outer double")
  {
    const OuterVariant original = 3.14159;
    OuterVariant restored;

    enki::BinWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
    REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());

    REQUIRE(std::holds_alternative<double>(restored));
    REQUIRE(std::get<double>(restored) == 3.14159);
  }
}

TEST_CASE("Variant containing optional binary roundtrip", "[edge_cases][variant]")
{
  using VariantOpt = std::variant<std::optional<int32_t>, std::string>;

  SECTION("Optional with value")
  {
    const VariantOpt original = std::optional<int32_t>{42};
    VariantOpt restored;

    enki::BinWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
    REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());

    REQUIRE(std::holds_alternative<std::optional<int32_t>>(restored));
    REQUIRE(std::get<std::optional<int32_t>>(restored).has_value());
    REQUIRE(*std::get<std::optional<int32_t>>(restored) == 42);
  }

  SECTION("Optional empty")
  {
    const VariantOpt original = std::optional<int32_t>{};
    VariantOpt restored = std::string{"should be replaced"};

    enki::BinWriter writer;
    REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
    REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());

    REQUIRE(std::holds_alternative<std::optional<int32_t>>(restored));
    REQUIRE_FALSE(std::get<std::optional<int32_t>>(restored).has_value());
  }
}

// ============================================================================
// SECTION 6: Single Element Containers
// ============================================================================

TEST_CASE("Single element vector binary roundtrip", "[edge_cases][single]")
{
  const std::vector<int32_t> original = {42};
  std::vector<int32_t> restored;

  enki::BinWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());

  REQUIRE(restored.size() == 1);
  REQUIRE(restored[0] == 42);
}

TEST_CASE("Single char string binary roundtrip", "[edge_cases][single]")
{
  const std::string original = "X";
  std::string restored;

  enki::BinWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());

  REQUIRE(restored == "X");
}

TEST_CASE("Single pair map binary roundtrip", "[edge_cases][single]")
{
  const std::map<std::string, int32_t> original = {
    {"key", 42}
  };
  std::map<std::string, int32_t> restored;

  enki::BinWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());
  REQUIRE_NOTHROW(enki::deserialize(restored, enki::BinReader(writer.data())).or_throw());

  REQUIRE(restored.size() == 1);
  REQUIRE(restored["key"] == 42);
}
