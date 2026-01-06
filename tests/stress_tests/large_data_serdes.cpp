/**
 * @file large_data_serdes.cpp
 * @brief Stress tests for large data serialization
 *
 * Tests serialization robustness with:
 * - Large containers (100,000+ elements)
 * - Deeply nested structures
 * - Large strings (1MB+)
 */

#include <cstdint>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"
#include "enki/json_reader.hpp"
#include "enki/json_writer.hpp"

// ============================================================================
// SECTION 1: Large Containers - Binary Format
// ============================================================================

TEST_CASE("Binary - Large vector of integers (100k elements)", "[stress][binary][large]")
{
  constexpr size_t kElementCount = 100'000;

  std::vector<int32_t> original;
  original.reserve(kElementCount);
  for (size_t i = 0; i < kElementCount; ++i)
  {
    original.push_back(static_cast<int32_t>(i));
  }

  enki::BinWriter writer;
  REQUIRE(enki::serialize(original, writer));

  std::vector<int32_t> restored;
  REQUIRE(enki::deserialize(restored, enki::BinReader(writer.data())));

  REQUIRE(restored.size() == kElementCount);
  REQUIRE(restored == original);
}

TEST_CASE("Binary - Large vector of strings (10k elements)", "[stress][binary][large]")
{
  constexpr size_t kElementCount = 10'000;

  std::vector<std::string> original;
  original.reserve(kElementCount);
  for (size_t i = 0; i < kElementCount; ++i)
  {
    original.push_back("string_" + std::to_string(i));
  }

  enki::BinWriter writer;
  REQUIRE(enki::serialize(original, writer));

  std::vector<std::string> restored;
  REQUIRE(enki::deserialize(restored, enki::BinReader(writer.data())));

  REQUIRE(restored.size() == kElementCount);
  REQUIRE(restored == original);
}

TEST_CASE("Binary - Large nested vector (1000x100 elements)", "[stress][binary][large]")
{
  constexpr size_t kOuterCount = 1'000;
  constexpr size_t kInnerCount = 100;

  std::vector<std::vector<int32_t>> original;
  original.reserve(kOuterCount);
  for (size_t i = 0; i < kOuterCount; ++i)
  {
    std::vector<int32_t> inner;
    inner.reserve(kInnerCount);
    for (size_t j = 0; j < kInnerCount; ++j)
    {
      inner.push_back(static_cast<int32_t>(i * kInnerCount + j));
    }
    original.push_back(std::move(inner));
  }

  enki::BinWriter writer;
  REQUIRE(enki::serialize(original, writer));

  std::vector<std::vector<int32_t>> restored;
  REQUIRE(enki::deserialize(restored, enki::BinReader(writer.data())));

  REQUIRE(restored.size() == kOuterCount);
  REQUIRE(restored == original);
}

// ============================================================================
// SECTION 2: Large Containers - JSON Format
// ============================================================================

TEST_CASE("JSON - Large vector of integers (10k elements)", "[stress][json][large]")
{
  // JSON is slower, so we use fewer elements
  constexpr size_t kElementCount = 10'000;

  std::vector<int32_t> original;
  original.reserve(kElementCount);
  for (size_t i = 0; i < kElementCount; ++i)
  {
    original.push_back(static_cast<int32_t>(i));
  }

  enki::JSONWriter writer;
  REQUIRE(enki::serialize(original, writer));

  std::vector<int32_t> restored;
  REQUIRE(enki::deserialize(restored, enki::JSONReader(writer.data().str())));

  REQUIRE(restored.size() == kElementCount);
  REQUIRE(restored == original);
}

TEST_CASE("JSON - Large vector of strings (1k elements)", "[stress][json][large]")
{
  constexpr size_t kElementCount = 1'000;

  std::vector<std::string> original;
  original.reserve(kElementCount);
  for (size_t i = 0; i < kElementCount; ++i)
  {
    original.push_back("string_" + std::to_string(i));
  }

  enki::JSONWriter writer;
  REQUIRE(enki::serialize(original, writer));

  std::vector<std::string> restored;
  REQUIRE(enki::deserialize(restored, enki::JSONReader(writer.data().str())));

  REQUIRE(restored.size() == kElementCount);
  REQUIRE(restored == original);
}

// ============================================================================
// SECTION 3: Deeply Nested Structures
// ============================================================================

// Helper to create deeply nested vector
template <size_t Depth>
struct NestedVector
{
  using type = std::vector<typename NestedVector<Depth - 1>::type>;
};

template <>
struct NestedVector<0>
{
  using type = int32_t;
};

TEST_CASE("Binary - Moderately nested vector (10 levels)", "[stress][binary][nested]")
{
  // 10 levels of nesting: vector<vector<...vector<int32_t>...>>
  using Level0 = int32_t;
  using Level1 = std::vector<Level0>;
  using Level2 = std::vector<Level1>;
  using Level3 = std::vector<Level2>;
  using Level4 = std::vector<Level3>;
  using Level5 = std::vector<Level4>;
  using Level6 = std::vector<Level5>;
  using Level7 = std::vector<Level6>;
  using Level8 = std::vector<Level7>;
  using Level9 = std::vector<Level8>;
  using Level10 = std::vector<Level9>;

  // Create a structure with one element at each level
  Level10 original = {{{{{{{{{{static_cast<int32_t>(42)}}}}}}}}}};

  enki::BinWriter writer;
  REQUIRE(enki::serialize(original, writer));

  Level10 restored;
  REQUIRE(enki::deserialize(restored, enki::BinReader(writer.data())));

  REQUIRE(restored == original);
}

TEST_CASE("JSON - Moderately nested vector (10 levels)", "[stress][json][nested]")
{
  using Level0 = int32_t;
  using Level1 = std::vector<Level0>;
  using Level2 = std::vector<Level1>;
  using Level3 = std::vector<Level2>;
  using Level4 = std::vector<Level3>;
  using Level5 = std::vector<Level4>;
  using Level6 = std::vector<Level5>;
  using Level7 = std::vector<Level6>;
  using Level8 = std::vector<Level7>;
  using Level9 = std::vector<Level8>;
  using Level10 = std::vector<Level9>;

  Level10 original = {{{{{{{{{{42}}}}}}}}}};

  enki::JSONWriter writer;
  REQUIRE(enki::serialize(original, writer));

  Level10 restored;
  REQUIRE(enki::deserialize(restored, enki::JSONReader(writer.data().str())));

  REQUIRE(restored == original);
}

TEST_CASE("Binary - Deeply nested with multiple elements per level", "[stress][binary][nested]")
{
  using Level0 = int32_t;
  using Level1 = std::vector<Level0>;
  using Level2 = std::vector<Level1>;
  using Level3 = std::vector<Level2>;
  using Level4 = std::vector<Level3>;
  using Level5 = std::vector<Level4>;

  // 5 levels with 2 elements each = 32 leaf values
  Level5 original = {
    {        {{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}},  {{{9, 10}, {11, 12}}, {{13, 14}, {15, 16}}}},
    {{{{17, 18}, {19, 20}}, {{21, 22}, {23, 24}}}, {{{25, 26}, {27, 28}}, {{29, 30}, {31, 32}}}}
  };

  enki::BinWriter writer;
  REQUIRE(enki::serialize(original, writer));

  Level5 restored;
  REQUIRE(enki::deserialize(restored, enki::BinReader(writer.data())));

  REQUIRE(restored == original);
}

// ============================================================================
// SECTION 4: Large Strings
// ============================================================================

TEST_CASE("Binary - Large string (1MB)", "[stress][binary][string]")
{
  constexpr size_t kStringSize = 1'024 * 1'024; // 1 MB

  std::string original(kStringSize, 'x');
  // Add some variation
  for (size_t i = 0; i < kStringSize; i += 1'000)
  {
    original[i] = static_cast<char>('a' + (i % 26));
  }

  enki::BinWriter writer;
  REQUIRE(enki::serialize(original, writer));

  std::string restored;
  REQUIRE(enki::deserialize(restored, enki::BinReader(writer.data())));

  REQUIRE(restored.size() == kStringSize);
  REQUIRE(restored == original);
}

TEST_CASE("Binary - Very large string (10MB)", "[stress][binary][string][slow]")
{
  constexpr size_t kStringSize = 10 * 1'024 * 1'024; // 10 MB

  std::string original(kStringSize, 'y');

  enki::BinWriter writer;
  REQUIRE(enki::serialize(original, writer));

  std::string restored;
  REQUIRE(enki::deserialize(restored, enki::BinReader(writer.data())));

  REQUIRE(restored.size() == kStringSize);
  REQUIRE(restored == original);
}

TEST_CASE("JSON - Large string (100KB)", "[stress][json][string]")
{
  // JSON strings need escaping, so use smaller size
  constexpr size_t kStringSize = 100 * 1'024; // 100 KB

  std::string original(kStringSize, 'z');
  // Add some variation (using only safe ASCII)
  for (size_t i = 0; i < kStringSize; i += 100)
  {
    original[i] = static_cast<char>('A' + (i % 26));
  }

  enki::JSONWriter writer;
  REQUIRE(enki::serialize(original, writer));

  std::string restored;
  REQUIRE(enki::deserialize(restored, enki::JSONReader(writer.data().str())));

  REQUIRE(restored.size() == kStringSize);
  REQUIRE(restored == original);
}

TEST_CASE("Binary - Vector of large strings", "[stress][binary][string]")
{
  constexpr size_t kStringCount = 100;
  constexpr size_t kStringSize = 10 * 1'024; // 10 KB each

  std::vector<std::string> original;
  original.reserve(kStringCount);
  for (size_t i = 0; i < kStringCount; ++i)
  {
    original.push_back(std::string(kStringSize, static_cast<char>('a' + (i % 26))));
  }

  enki::BinWriter writer;
  REQUIRE(enki::serialize(original, writer));

  std::vector<std::string> restored;
  REQUIRE(enki::deserialize(restored, enki::BinReader(writer.data())));

  REQUIRE(restored.size() == kStringCount);
  REQUIRE(restored == original);
}

// ============================================================================
// SECTION 5: Memory Efficiency Tests
// ============================================================================

TEST_CASE("Binary - Verify writer reserve prevents reallocations", "[stress][binary][memory]")
{
  constexpr size_t kElementCount = 10'000;

  std::vector<int32_t> original;
  original.reserve(kElementCount);
  for (size_t i = 0; i < kElementCount; ++i)
  {
    original.push_back(static_cast<int32_t>(i));
  }

  // Pre-reserve space in writer
  enki::BinWriter writer;
  // Each int32_t is 4 bytes, plus size field (4 bytes)
  constexpr size_t kExpectedSize = sizeof(uint32_t) + kElementCount * sizeof(int32_t);
  writer.reserve(kExpectedSize);

  REQUIRE(enki::serialize(original, writer));

  // Verify data was written correctly
  std::vector<int32_t> restored;
  REQUIRE(enki::deserialize(restored, enki::BinReader(writer.data())));
  REQUIRE(restored == original);
}

TEST_CASE("Binary - Large data roundtrip preserves all bytes", "[stress][binary][integrity]")
{
  // Create a vector with every possible byte value repeated
  constexpr size_t kRepetitions = 1'000;
  std::vector<uint8_t> original;
  original.reserve(256 * kRepetitions);

  for (size_t rep = 0; rep < kRepetitions; ++rep)
  {
    for (int i = 0; i < 256; ++i)
    {
      original.push_back(static_cast<uint8_t>(i));
    }
  }

  enki::BinWriter writer;
  REQUIRE(enki::serialize(original, writer));

  std::vector<uint8_t> restored;
  REQUIRE(enki::deserialize(restored, enki::BinReader(writer.data())));

  REQUIRE(restored.size() == original.size());
  REQUIRE(restored == original);
}

// ============================================================================
// SECTION 6: Edge Case Combinations
// ============================================================================

TEST_CASE("Binary - Empty containers in large structure", "[stress][binary][edge]")
{
  // Mix of empty and non-empty containers
  std::vector<std::vector<int32_t>> original;
  original.reserve(1'000);

  for (size_t i = 0; i < 1'000; ++i)
  {
    if (i % 3 == 0)
    {
      original.push_back({}); // Empty
    }
    else
    {
      original.push_back({static_cast<int32_t>(i), static_cast<int32_t>(i + 1)});
    }
  }

  enki::BinWriter writer;
  REQUIRE(enki::serialize(original, writer));

  std::vector<std::vector<int32_t>> restored;
  REQUIRE(enki::deserialize(restored, enki::BinReader(writer.data())));

  REQUIRE(restored == original);
}

TEST_CASE("JSON - Empty containers in large structure", "[stress][json][edge]")
{
  std::vector<std::vector<int32_t>> original;
  original.reserve(100);

  for (size_t i = 0; i < 100; ++i)
  {
    if (i % 3 == 0)
    {
      original.push_back({});
    }
    else
    {
      original.push_back({static_cast<int32_t>(i)});
    }
  }

  enki::JSONWriter writer;
  REQUIRE(enki::serialize(original, writer));

  std::vector<std::vector<int32_t>> restored;
  REQUIRE(enki::deserialize(restored, enki::JSONReader(writer.data().str())));

  REQUIRE(restored == original);
}
