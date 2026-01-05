/**
 * @file binary_error_handling_serdes.cpp
 * @brief Tests for binary error handling and malformed data
 *
 * Tests behavior when deserializing:
 * - Type mismatches
 * - Forward compatibility error cases
 */

#include <cstdint>
#include <string>
#include <variant>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

// ============================================================================
// SECTION 2: Type Mismatches
// ============================================================================

TEST_CASE("Type mismatch - wrong variant index (binary strict)", "[error_handling][binary]")
{
  using SmallVar = std::variant<int32_t, std::string>;

  // Serialize with index 1 (string)
  const SmallVar original = std::string{"hello"};

  enki::BinWriter writer;
  REQUIRE_NOTHROW(enki::serialize(original, writer).or_throw());

  // Manually corrupt index to 99 (out of range)
  auto data = writer.data();
  data[0] = std::byte{99}; // First byte is the index (uint32_t)

  SmallVar restored;
  auto result = enki::deserialize(restored, enki::BinReader(data));
  REQUIRE_FALSE(result); // Should fail - index out of range
  REQUIRE(result.error() != nullptr);
}

// ============================================================================
// SECTION 3: Forward Compatibility Error Cases
// ============================================================================

TEST_CASE("Forward compat - unknown index without monostate", "[error_handling][forward_compat]")
{
  using NoFallbackVar = std::variant<int32_t, std::string>; // No monostate

  // Write a variant with index 99
  enki::BinWriter<enki::forward_compatible_t> writer;

  // Manually construct: index=99, size=4, value=42
  writer.write(uint32_t{99});
  writer.write(uint32_t{4}); // size of value
  writer.write(int32_t{42}); // value

  NoFallbackVar restored;
  auto result = enki::deserialize(
    restored, enki::BinReader<enki::forward_compatible_t>(writer.data()));

  REQUIRE_FALSE(result); // Should fail - no monostate fallback
  REQUIRE(result.error() != nullptr);
}

TEST_CASE("Forward compat - unknown index with monostate succeeds", "[error_handling][forward_compat]")
{
  using FallbackVar = std::variant<std::monostate, int32_t, std::string>;

  // Write a variant with unknown index
  enki::BinWriter<enki::forward_compatible_t> writer;
  writer.write(uint32_t{99}); // unknown index
  writer.write(uint32_t{4});  // size of value
  writer.write(int32_t{42});  // value to skip

  FallbackVar restored = int32_t{0}; // Pre-populate with non-monostate
  auto result = enki::deserialize(
    restored, enki::BinReader<enki::forward_compatible_t>(writer.data()));

  REQUIRE(result); // Should succeed - falls back to monostate
  REQUIRE(std::holds_alternative<std::monostate>(restored));
}
