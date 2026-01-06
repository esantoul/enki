#include <cstdint>
#include <span>
#include <string_view>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_probe.hpp"
#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/json_reader.hpp"
#include "enki/json_writer.hpp"

// Test that CTAD works correctly and produces the expected types

TEST_CASE("BinWriter CTAD - default constructor", "[unit][CTAD]")
{
  enki::BinWriter writer;
  STATIC_CHECK(std::is_same_v<decltype(writer), enki::BinWriter<enki::strict_t, uint32_t>>);
}

TEST_CASE("BinWriter CTAD - strict policy", "[unit][CTAD]")
{
  enki::BinWriter writer(enki::strict);
  STATIC_CHECK(std::is_same_v<decltype(writer), enki::BinWriter<enki::strict_t, uint32_t>>);
}

TEST_CASE("BinWriter CTAD - forward_compatible policy", "[unit][CTAD]")
{
  enki::BinWriter writer(enki::forward_compatible);
  STATIC_CHECK(
    std::is_same_v<decltype(writer), enki::BinWriter<enki::forward_compatible_t, uint32_t>>);
}

TEST_CASE("BinSpanWriter CTAD - span only", "[unit][CTAD]")
{
  std::array<std::byte, 64> buffer{};
  std::span span{buffer};
  enki::BinSpanWriter writer(span);
  STATIC_CHECK(std::is_same_v<decltype(writer), enki::BinSpanWriter<enki::strict_t, uint32_t>>);
}

TEST_CASE("BinSpanWriter CTAD - strict policy first", "[unit][CTAD]")
{
  std::array<std::byte, 64> buffer{};
  std::span span{buffer};
  enki::BinSpanWriter writer(enki::strict, span);
  STATIC_CHECK(std::is_same_v<decltype(writer), enki::BinSpanWriter<enki::strict_t, uint32_t>>);
}

TEST_CASE("BinSpanWriter CTAD - forward_compatible policy first", "[unit][CTAD]")
{
  std::array<std::byte, 64> buffer{};
  std::span span{buffer};
  enki::BinSpanWriter writer(enki::forward_compatible, span);
  STATIC_CHECK(
    std::is_same_v<decltype(writer), enki::BinSpanWriter<enki::forward_compatible_t, uint32_t>>);
}

TEST_CASE("BinProbe CTAD - default constructor", "[unit][CTAD]")
{
  enki::BinProbe probe;
  STATIC_CHECK(std::is_same_v<decltype(probe), enki::BinProbe<enki::strict_t, uint32_t>>);
}

TEST_CASE("BinProbe CTAD - strict policy", "[unit][CTAD]")
{
  enki::BinProbe probe(enki::strict);
  STATIC_CHECK(std::is_same_v<decltype(probe), enki::BinProbe<enki::strict_t, uint32_t>>);
}

TEST_CASE("BinProbe CTAD - forward_compatible policy", "[unit][CTAD]")
{
  enki::BinProbe probe(enki::forward_compatible);
  STATIC_CHECK(
    std::is_same_v<decltype(probe), enki::BinProbe<enki::forward_compatible_t, uint32_t>>);
}

TEST_CASE("BinSpanReader CTAD - span only", "[unit][CTAD]")
{
  const std::array<std::byte, 64> buffer{};
  std::span span{buffer};
  enki::BinSpanReader reader(span);
  STATIC_CHECK(std::is_same_v<decltype(reader), enki::BinSpanReader<enki::strict_t, uint32_t>>);
}

TEST_CASE("BinSpanReader CTAD - strict policy first", "[unit][CTAD]")
{
  const std::array<std::byte, 64> buffer{};
  std::span span{buffer};
  enki::BinSpanReader reader(enki::strict, span);
  STATIC_CHECK(std::is_same_v<decltype(reader), enki::BinSpanReader<enki::strict_t, uint32_t>>);
}

TEST_CASE("BinSpanReader CTAD - forward_compatible policy first", "[unit][CTAD]")
{
  const std::array<std::byte, 64> buffer{};
  std::span span{buffer};
  enki::BinSpanReader reader(enki::forward_compatible, span);
  STATIC_CHECK(
    std::is_same_v<decltype(reader), enki::BinSpanReader<enki::forward_compatible_t, uint32_t>>);
}

TEST_CASE("BinReader CTAD - span only", "[unit][CTAD]")
{
  const std::array<std::byte, 64> buffer{};
  std::span span{buffer};
  enki::BinReader reader(span);
  STATIC_CHECK(std::is_same_v<decltype(reader), enki::BinReader<enki::strict_t, uint32_t>>);
}

TEST_CASE("BinReader CTAD - strict policy first", "[unit][CTAD]")
{
  const std::array<std::byte, 64> buffer{};
  std::span span{buffer};
  enki::BinReader reader(enki::strict, span);
  STATIC_CHECK(std::is_same_v<decltype(reader), enki::BinReader<enki::strict_t, uint32_t>>);
}

TEST_CASE("BinReader CTAD - forward_compatible policy first", "[unit][CTAD]")
{
  const std::array<std::byte, 64> buffer{};
  std::span span{buffer};
  enki::BinReader reader(enki::forward_compatible, span);
  STATIC_CHECK(
    std::is_same_v<decltype(reader), enki::BinReader<enki::forward_compatible_t, uint32_t>>);
}

TEST_CASE("JSONWriter CTAD - default constructor", "[unit][CTAD]")
{
  enki::JSONWriter writer;
  STATIC_CHECK(std::is_same_v<decltype(writer), enki::JSONWriter<enki::strict_t>>);
}

TEST_CASE("JSONWriter CTAD - strict policy", "[unit][CTAD]")
{
  enki::JSONWriter writer(enki::strict);
  STATIC_CHECK(std::is_same_v<decltype(writer), enki::JSONWriter<enki::strict_t>>);
}

TEST_CASE("JSONWriter CTAD - forward_compatible policy", "[unit][CTAD]")
{
  enki::JSONWriter writer(enki::forward_compatible);
  STATIC_CHECK(std::is_same_v<decltype(writer), enki::JSONWriter<enki::forward_compatible_t>>);
}

TEST_CASE("JSONReader CTAD - string_view only", "[unit][CTAD]")
{
  enki::JSONReader reader("{}");
  STATIC_CHECK(std::is_same_v<decltype(reader), enki::JSONReader<enki::strict_t>>);
}

TEST_CASE("JSONReader CTAD - strict policy first", "[unit][CTAD]")
{
  enki::JSONReader reader(enki::strict, "{}");
  STATIC_CHECK(std::is_same_v<decltype(reader), enki::JSONReader<enki::strict_t>>);
}

TEST_CASE("JSONReader CTAD - forward_compatible policy first", "[unit][CTAD]")
{
  enki::JSONReader reader(enki::forward_compatible, "{}");
  STATIC_CHECK(std::is_same_v<decltype(reader), enki::JSONReader<enki::forward_compatible_t>>);
}
