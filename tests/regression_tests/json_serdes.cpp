// =============================================================================
// JSON Serialization/Deserialization Tests
// =============================================================================
// This file tests JSON roundtrip serialization for all supported types.
// Binary serialization is tested in other files; this focuses on JSON format.

#include <array>
#include <cstdint>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "enki/enki.hpp"

// =============================================================================
// Section A: Primitive Types
// =============================================================================

TEST_CASE("JSON - bool serialization", "[json][primitive]")
{
  SECTION("true value")
  {
    bool value = true;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    bool result = false;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == true);
  }

  SECTION("false value")
  {
    bool value = false;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    bool result = true;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == false);
  }
}

TEST_CASE("JSON - integer serialization", "[json][primitive]")
{
  SECTION("int8_t")
  {
    int8_t value = -42;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    int8_t result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("int16_t")
  {
    int16_t value = -1'234;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    int16_t result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("int32_t")
  {
    int32_t value = -123'456;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    int32_t result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("int64_t")
  {
    int64_t value = -123'456'789'012'345LL;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    int64_t result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("uint8_t")
  {
    uint8_t value = 255;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    uint8_t result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("uint16_t")
  {
    uint16_t value = 65'535;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    uint16_t result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("uint32_t")
  {
    uint32_t value = 4'294'967'295U;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    uint32_t result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("uint64_t")
  {
    uint64_t value = 123'456'789'012'345ULL;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    uint64_t result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }
}

TEST_CASE("JSON - floating point serialization", "[json][primitive]")
{
  SECTION("float")
  {
    float value = 3.14159f;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    float result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("double")
  {
    double value = 3.141592653589793;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    double result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("negative float")
  {
    float value = -123.456f;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    float result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }
}

TEST_CASE("JSON - char serialization", "[json][primitive]")
{
  char value = 'A';
  enki::JSONWriter writer;
  REQUIRE(enki::serialize(value, writer));

  char result{};
  enki::JSONReader reader(writer.data().str());
  REQUIRE(enki::deserialize(result, reader));
  REQUIRE(result == value);
}

// =============================================================================
// Section B: String Types
// =============================================================================

TEST_CASE("JSON - string serialization", "[json][string]")
{
  SECTION("simple string")
  {
    std::string value = "Hello, World!";
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    std::string result;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("empty string")
  {
    std::string value;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    std::string result = "not empty";
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result.empty());
  }

  SECTION("string with spaces")
  {
    std::string value = "Hello World with spaces";
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    std::string result;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }
}

// =============================================================================
// Section C: Enum Types
// =============================================================================

namespace
{
  enum class ScopedColor
  {
    Red,
    Green,
    Blue
  };

  enum UnscopedStatus
  {
    Idle = 0,
    Running = 1,
    Complete = 2
  };
} // namespace

TEST_CASE("JSON - enum serialization", "[json][enum]")
{
  SECTION("scoped enum")
  {
    ScopedColor value = ScopedColor::Green;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    ScopedColor result = ScopedColor::Red;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == ScopedColor::Green);
  }

  SECTION("unscoped enum")
  {
    UnscopedStatus value = Running;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    UnscopedStatus result = Idle;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == Running);
  }
}

// =============================================================================
// Section D: Container Types
// =============================================================================

TEST_CASE("JSON - std::array serialization", "[json][container]")
{
  std::array<int, 4> value = {1, 2, 3, 4};
  enki::JSONWriter writer;
  REQUIRE(enki::serialize(value, writer));

  std::array<int, 4> result{};
  enki::JSONReader reader(writer.data().str());
  REQUIRE(enki::deserialize(result, reader));
  REQUIRE(result == value);
}

TEST_CASE("JSON - std::vector serialization", "[json][container]")
{
  SECTION("non-empty vector")
  {
    std::vector<int> value = {10, 20, 30, 40, 50};
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    std::vector<int> result;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("empty vector")
  {
    std::vector<int> value;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    std::vector<int> result = {1, 2, 3};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));

    INFO(writer.data().str());

    REQUIRE(result.empty());
  }

  SECTION("vector of strings")
  {
    std::vector<std::string> value = {"apple", "banana", "cherry"};
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    std::vector<std::string> result;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }
}

TEST_CASE("JSON - std::set serialization", "[json][container]")
{
  std::set<int> value = {5, 3, 8, 1, 9};
  enki::JSONWriter writer;
  REQUIRE(enki::serialize(value, writer));

  std::set<int> result;
  enki::JSONReader reader(writer.data().str());
  REQUIRE(enki::deserialize(result, reader));
  REQUIRE(result == value);
}

TEST_CASE("JSON - std::map serialization", "[json][container]")
{
  std::map<int, std::string> value = {
    {1,   "one"},
    {2,   "two"},
    {3, "three"}
  };
  enki::JSONWriter writer;
  REQUIRE(enki::serialize(value, writer));

  std::map<int, std::string> result;
  enki::JSONReader reader(writer.data().str());
  REQUIRE(enki::deserialize(result, reader));
  REQUIRE(result == value);
}

// =============================================================================
// Section E: Optional Types
// =============================================================================

TEST_CASE("JSON - std::optional serialization", "[json][optional]")
{
  SECTION("optional with value - roundtrip")
  {
    std::optional<int> value = 42;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    std::optional<int> result;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result.has_value());
    REQUIRE(result.value() == 42);
  }

  SECTION("optional without value - roundtrip")
  {
    std::optional<int> value;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    std::optional<int> result = 999;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE_FALSE(result.has_value());
  }

  SECTION("optional string with value - roundtrip")
  {
    std::optional<std::string> value = "hello";
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    std::optional<std::string> result;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result.has_value());
    REQUIRE(result.value() == "hello");
  }
}

TEST_CASE("JSON - std::optional format verification", "[json][optional][format]")
{
  SECTION("empty optional serializes to null")
  {
    std::optional<int> value;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));
    REQUIRE(writer.data().str() == "null");
  }

  SECTION("optional<int> with value serializes directly (not wrapped)")
  {
    std::optional<int> value = 42;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));
    REQUIRE(writer.data().str() == "42");
  }

  SECTION("optional<string> with value serializes directly")
  {
    std::optional<std::string> value = "hello";
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));
    REQUIRE(writer.data().str() == "\"hello\"");
  }

  SECTION("optional<vector> with value serializes directly")
  {
    std::optional<std::vector<int>> value = std::vector<int>{1, 2, 3};
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));
    REQUIRE(writer.data().str() == "[1, 2, 3]");
  }

  SECTION("optional<bool> true serializes directly")
  {
    std::optional<bool> value = true;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));
    REQUIRE(writer.data().str() == "true");
  }

  SECTION("optional<bool> false serializes directly")
  {
    std::optional<bool> value = false;
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));
    REQUIRE(writer.data().str() == "false");
  }
}

TEST_CASE("JSON - std::optional deserialization from standard JSON", "[json][optional][interop]")
{
  SECTION("deserialize null to empty optional")
  {
    std::optional<int> result = 999;
    enki::JSONReader reader("null");
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE_FALSE(result.has_value());
  }

  SECTION("deserialize direct value to optional<int>")
  {
    std::optional<int> result;
    enki::JSONReader reader("42");
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result.has_value());
    REQUIRE(result.value() == 42);
  }

  SECTION("deserialize direct string to optional<string>")
  {
    std::optional<std::string> result;
    enki::JSONReader reader("\"hello world\"");
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result.has_value());
    REQUIRE(result.value() == "hello world");
  }

  SECTION("deserialize direct array to optional<vector>")
  {
    std::optional<std::vector<int>> result;
    enki::JSONReader reader("[1, 2, 3]");
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result.has_value());
    REQUIRE(result.value() == std::vector<int>{1, 2, 3});
  }
}

// =============================================================================
// Section F: Tuple Types
// =============================================================================

TEST_CASE("JSON - std::tuple serialization", "[json][tuple]")
{
  std::tuple<int, double, std::string> value = {42, 3.14, "test"};
  enki::JSONWriter writer;
  REQUIRE(enki::serialize(value, writer));

  std::tuple<int, double, std::string> result;
  enki::JSONReader reader(writer.data().str());
  REQUIRE(enki::deserialize(result, reader));
  REQUIRE(std::get<0>(result) == 42);
  REQUIRE(std::get<1>(result) == 3.14);
  REQUIRE(std::get<2>(result) == "test");
}

TEST_CASE("JSON - std::pair serialization", "[json][tuple]")
{
  std::pair<std::string, int> value = {"count", 100};
  enki::JSONWriter writer;
  REQUIRE(enki::serialize(value, writer));

  std::pair<std::string, int> result;
  enki::JSONReader reader(writer.data().str());
  REQUIRE(enki::deserialize(result, reader));
  REQUIRE(result.first == "count");
  REQUIRE(result.second == 100);
}

// =============================================================================
// Section G: Custom Types
// =============================================================================

namespace
{
  struct Point
  {
    int x;
    int y;

    bool operator==(const Point &other) const
    {
      return x == other.x && y == other.y;
    }

    struct EnkiSerial
    {
      using Members = enki::Register<&Point::x, &Point::y>;
    };
  };

  struct Rectangle
  {
    Point topLeft;
    Point bottomRight;
    std::string name;

    bool operator==(const Rectangle &other) const
    {
      return topLeft == other.topLeft && bottomRight == other.bottomRight && name == other.name;
    }

    struct EnkiSerial
    {
      using Members =
        enki::Register<&Rectangle::topLeft, &Rectangle::bottomRight, &Rectangle::name>;
    };
  };
} // namespace

TEST_CASE("JSON - custom type serialization", "[json][custom]")
{
  SECTION("simple struct")
  {
    Point value = {10, 20};
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    Point result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("nested struct")
  {
    Rectangle value = {
      {  0,  0},
      {100, 50},
      "my_rect"
    };
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    Rectangle result{};
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }
}

// =============================================================================
// Section H: Complex/Nested Structures
// =============================================================================

TEST_CASE("JSON - nested containers", "[json][complex]")
{
  SECTION("vector of vectors")
  {
    std::vector<std::vector<int>> value = {
      {1, 2},
      {3, 4, 5},
      {6}
    };
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    std::vector<std::vector<int>> result;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("map with vector values")
  {
    std::map<std::string, std::vector<int>> value = {
      { "odds", {1, 3, 5}},
      {"evens", {2, 4, 6}}
    };
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    std::map<std::string, std::vector<int>> result;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }

  SECTION("vector of custom types")
  {
    std::vector<Point> value = {
      {1, 2},
      {3, 4},
      {5, 6}
    };
    enki::JSONWriter writer;
    REQUIRE(enki::serialize(value, writer));

    std::vector<Point> result;
    enki::JSONReader reader(writer.data().str());
    REQUIRE(enki::deserialize(result, reader));
    REQUIRE(result == value);
  }
}
