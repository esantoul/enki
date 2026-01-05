/**
 * @file serdes_benchmarks.cpp
 * @brief Performance benchmarks for enki serialization library
 *
 * Measures throughput, latency, and memory efficiency for binary and JSON formats.
 *
 * Run with: ./enki_benchmarks --benchmark-samples 100
 */

#include <cstdint>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "enki/bin_probe.hpp"
#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"
#include "enki/json_reader.hpp"
#include "enki/json_writer.hpp"

// ============================================================================
// Test Data Structures
// ============================================================================

struct SmallStruct
{
  int32_t x;
  int32_t y;
  int32_t z;

  constexpr auto operator<=>(const SmallStruct&) const noexcept = default;

  struct EnkiSerial
  {
    using Members = enki::Register<&SmallStruct::x, &SmallStruct::y, &SmallStruct::z>;
  };
};

// ============================================================================
// SECTION 1: Small Object Latency
// ============================================================================

TEST_CASE("Latency - Small Objects", "[benchmark][latency]")
{
  BENCHMARK("Binary: serialize int32_t")
  {
    enki::BinWriter writer;
    int32_t value = 42;
    return enki::serialize(value, writer);
  };

  BENCHMARK("Binary: roundtrip int32_t")
  {
    enki::BinWriter writer;
    int32_t value = 42;
    enki::serialize(value, writer);
    int32_t result;
    return enki::deserialize(result, enki::BinReader(writer.data()));
  };

  BENCHMARK("JSON: serialize int32_t")
  {
    enki::JSONWriter writer;
    int32_t value = 42;
    return enki::serialize(value, writer);
  };

  BENCHMARK("JSON: roundtrip int32_t")
  {
    enki::JSONWriter writer;
    int32_t value = 42;
    enki::serialize(value, writer);
    int32_t result;
    return enki::deserialize(result, enki::JSONReader(writer.data().str()));
  };

  BENCHMARK("Binary: roundtrip SmallStruct")
  {
    enki::BinWriter writer;
    SmallStruct value{1, 2, 3};
    enki::serialize(value, writer);
    SmallStruct result;
    return enki::deserialize(result, enki::BinReader(writer.data()));
  };

  BENCHMARK("JSON: roundtrip SmallStruct")
  {
    enki::JSONWriter writer;
    SmallStruct value{1, 2, 3};
    enki::serialize(value, writer);
    SmallStruct result;
    return enki::deserialize(result, enki::JSONReader(writer.data().str()));
  };
}

// ============================================================================
// SECTION 2: Vector Throughput (Binary)
// ============================================================================

TEST_CASE("Throughput - Binary Vector<int32_t>", "[benchmark][throughput][binary]")
{
  SECTION("1K elements")
  {
    std::vector<int32_t> data(1'000);
    for (size_t i = 0; i < data.size(); ++i)
      data[i] = static_cast<int32_t>(i);

    BENCHMARK("serialize 1K ints")
    {
      enki::BinWriter writer;
      return enki::serialize(data, writer);
    };

    enki::BinWriter writer;
    enki::serialize(data, writer);

    BENCHMARK("deserialize 1K ints")
    {
      std::vector<int32_t> result;
      return enki::deserialize(result, enki::BinReader(writer.data()));
    };
  }

  SECTION("10K elements")
  {
    std::vector<int32_t> data(10'000);
    for (size_t i = 0; i < data.size(); ++i)
      data[i] = static_cast<int32_t>(i);

    BENCHMARK("serialize 10K ints")
    {
      enki::BinWriter writer;
      return enki::serialize(data, writer);
    };

    enki::BinWriter writer;
    enki::serialize(data, writer);

    BENCHMARK("deserialize 10K ints")
    {
      std::vector<int32_t> result;
      return enki::deserialize(result, enki::BinReader(writer.data()));
    };
  }

  SECTION("100K elements")
  {
    std::vector<int32_t> data(100'000);
    for (size_t i = 0; i < data.size(); ++i)
      data[i] = static_cast<int32_t>(i);

    BENCHMARK("serialize 100K ints")
    {
      enki::BinWriter writer;
      return enki::serialize(data, writer);
    };

    enki::BinWriter writer;
    enki::serialize(data, writer);

    BENCHMARK("deserialize 100K ints")
    {
      std::vector<int32_t> result;
      return enki::deserialize(result, enki::BinReader(writer.data()));
    };
  }
}

TEST_CASE("Throughput - Binary Vector<string>", "[benchmark][throughput][binary]")
{
  std::vector<std::string> data;
  data.reserve(1'000);
  for (size_t i = 0; i < 1'000; ++i)
  {
    data.push_back("string_" + std::to_string(i));
  }

  BENCHMARK("serialize 1K strings")
  {
    enki::BinWriter writer;
    return enki::serialize(data, writer);
  };

  enki::BinWriter writer;
  enki::serialize(data, writer);

  BENCHMARK("deserialize 1K strings")
  {
    std::vector<std::string> result;
    return enki::deserialize(result, enki::BinReader(writer.data()));
  };
}

// ============================================================================
// SECTION 3: Vector Throughput (JSON)
// ============================================================================

TEST_CASE("Throughput - JSON Vector<int32_t>", "[benchmark][throughput][json]")
{
  SECTION("1K elements")
  {
    std::vector<int32_t> data(1'000);
    for (size_t i = 0; i < data.size(); ++i)
      data[i] = static_cast<int32_t>(i);

    BENCHMARK("serialize 1K ints")
    {
      enki::JSONWriter writer;
      return enki::serialize(data, writer);
    };

    enki::JSONWriter writer;
    enki::serialize(data, writer);
    std::string json = writer.data().str();

    BENCHMARK("deserialize 1K ints")
    {
      std::vector<int32_t> result;
      return enki::deserialize(result, enki::JSONReader(json));
    };
  }

  SECTION("10K elements")
  {
    std::vector<int32_t> data(10'000);
    for (size_t i = 0; i < data.size(); ++i)
      data[i] = static_cast<int32_t>(i);

    BENCHMARK("serialize 10K ints")
    {
      enki::JSONWriter writer;
      return enki::serialize(data, writer);
    };

    enki::JSONWriter writer;
    enki::serialize(data, writer);
    std::string json = writer.data().str();

    BENCHMARK("deserialize 10K ints")
    {
      std::vector<int32_t> result;
      return enki::deserialize(result, enki::JSONReader(json));
    };
  }
}

// ============================================================================
// SECTION 4: String Throughput
// ============================================================================

TEST_CASE("Throughput - String Sizes", "[benchmark][throughput][string]")
{
  SECTION("1KB string")
  {
    std::string data(1024, 'x');

    BENCHMARK("Binary: serialize 1KB string")
    {
      enki::BinWriter writer;
      return enki::serialize(data, writer);
    };

    BENCHMARK("JSON: serialize 1KB string")
    {
      enki::JSONWriter writer;
      return enki::serialize(data, writer);
    };

    enki::BinWriter binWriter;
    enki::serialize(data, binWriter);

    BENCHMARK("Binary: deserialize 1KB string")
    {
      std::string result;
      return enki::deserialize(result, enki::BinReader(binWriter.data()));
    };
  }

  SECTION("10KB string")
  {
    std::string data(10 * 1024, 'y');

    BENCHMARK("Binary: serialize 10KB string")
    {
      enki::BinWriter writer;
      return enki::serialize(data, writer);
    };

    BENCHMARK("JSON: serialize 10KB string")
    {
      enki::JSONWriter writer;
      return enki::serialize(data, writer);
    };
  }

  SECTION("100KB string")
  {
    std::string data(100 * 1024, 'z');

    BENCHMARK("Binary: serialize 100KB string")
    {
      enki::BinWriter writer;
      return enki::serialize(data, writer);
    };

    BENCHMARK("JSON: serialize 100KB string")
    {
      enki::JSONWriter writer;
      return enki::serialize(data, writer);
    };
  }
}

// ============================================================================
// SECTION 5: Nested Structure Performance
// ============================================================================

TEST_CASE("Throughput - Nested Structures", "[benchmark][throughput][nested]")
{
  SECTION("100x100 nested vector")
  {
    std::vector<std::vector<int32_t>> data;
    data.reserve(100);
    for (size_t i = 0; i < 100; ++i)
    {
      std::vector<int32_t> inner(100);
      for (size_t j = 0; j < 100; ++j)
        inner[j] = static_cast<int32_t>(i * 100 + j);
      data.push_back(std::move(inner));
    }

    BENCHMARK("Binary: serialize 100x100")
    {
      enki::BinWriter writer;
      return enki::serialize(data, writer);
    };

    enki::BinWriter writer;
    enki::serialize(data, writer);

    BENCHMARK("Binary: deserialize 100x100")
    {
      std::vector<std::vector<int32_t>> result;
      return enki::deserialize(result, enki::BinReader(writer.data()));
    };
  }

  SECTION("5-level deep nesting")
  {
    using Level0 = int32_t;
    using Level1 = std::vector<Level0>;
    using Level2 = std::vector<Level1>;
    using Level3 = std::vector<Level2>;
    using Level4 = std::vector<Level3>;
    using Level5 = std::vector<Level4>;

    // 2 elements at each level = 32 leaf values
    Level5 data = {
      {{{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}}, {{{9, 10}, {11, 12}}, {{13, 14}, {15, 16}}}},
      {{{{17, 18}, {19, 20}}, {{21, 22}, {23, 24}}},
       {{{25, 26}, {27, 28}}, {{29, 30}, {31, 32}}}}};

    BENCHMARK("Binary: serialize 5-level nested")
    {
      enki::BinWriter writer;
      return enki::serialize(data, writer);
    };

    BENCHMARK("JSON: serialize 5-level nested")
    {
      enki::JSONWriter writer;
      return enki::serialize(data, writer);
    };
  }
}

// ============================================================================
// SECTION 6: Memory Optimization - reserve() and Writer Reuse
// ============================================================================

TEST_CASE("Memory - reserve() benefit for vectors", "[benchmark][memory]")
{
  SECTION("100K int32_t")
  {
    std::vector<int32_t> data(100'000);
    for (size_t i = 0; i < data.size(); ++i)
      data[i] = static_cast<int32_t>(i);

    // Use BinProbe to calculate exact serialized size
    enki::BinProbe probe;
    const size_t expectedSize = enki::serialize(data, probe).size();

    BENCHMARK("Without reserve()")
    {
      enki::BinWriter writer;
      return enki::serialize(data, writer);
    };

    BENCHMARK("With reserve() (size from BinProbe)")
    {
      enki::BinWriter writer;
      writer.reserve(expectedSize);
      return enki::serialize(data, writer);
    };
  }

  SECTION("10K int32_t")
  {
    std::vector<int32_t> data(10'000);
    for (size_t i = 0; i < data.size(); ++i)
      data[i] = static_cast<int32_t>(i);

    // Use BinProbe to calculate exact serialized size
    enki::BinProbe probe;
    const size_t expectedSize = enki::serialize(data, probe).size();

    BENCHMARK("Without reserve()")
    {
      enki::BinWriter writer;
      return enki::serialize(data, writer);
    };

    BENCHMARK("With reserve() (size from BinProbe)")
    {
      enki::BinWriter writer;
      writer.reserve(expectedSize);
      return enki::serialize(data, writer);
    };
  }
}

TEST_CASE("Memory - reserve() benefit for strings", "[benchmark][memory]")
{
  SECTION("100KB string")
  {
    std::string data(100 * 1024, 'x');

    // Use BinProbe to calculate exact serialized size
    enki::BinProbe probe;
    const size_t expectedSize = enki::serialize(data, probe).size();

    BENCHMARK("Without reserve()")
    {
      enki::BinWriter writer;
      return enki::serialize(data, writer);
    };

    BENCHMARK("With reserve() (size from BinProbe)")
    {
      enki::BinWriter writer;
      writer.reserve(expectedSize);
      return enki::serialize(data, writer);
    };
  }
}

TEST_CASE("Memory - Writer reuse with clear()", "[benchmark][memory]")
{
  std::vector<int32_t> data(10'000);
  for (size_t i = 0; i < data.size(); ++i)
    data[i] = static_cast<int32_t>(i);

  // Use BinProbe to calculate exact serialized size
  enki::BinProbe probe;
  const size_t expectedSize = enki::serialize(data, probe).size();

  BENCHMARK("New writer each time")
  {
    enki::BinWriter writer;
    return enki::serialize(data, writer);
  };

  // Pre-allocated writer that gets reused
  enki::BinWriter reusableWriter;
  reusableWriter.reserve(expectedSize);

  BENCHMARK("Reuse writer with clear()")
  {
    reusableWriter.clear();
    return enki::serialize(data, reusableWriter);
  };
}

TEST_CASE("Memory - Pre-reserved throughput comparison", "[benchmark][memory][throughput]")
{
  SECTION("1K ints - measuring allocation overhead")
  {
    std::vector<int32_t> data(1'000);
    for (size_t i = 0; i < data.size(); ++i)
      data[i] = static_cast<int32_t>(i);

    // Use BinProbe to calculate exact serialized size
    enki::BinProbe probe;
    const size_t expectedSize = enki::serialize(data, probe).size();

    // Pre-create reserved writer for reuse benchmark
    enki::BinWriter reservedWriter;
    reservedWriter.reserve(expectedSize);

    BENCHMARK("Fresh writer (allocations)")
    {
      enki::BinWriter writer;
      return enki::serialize(data, writer);
    };

    BENCHMARK("Pre-reserved writer (no allocations)")
    {
      reservedWriter.clear();
      return enki::serialize(data, reservedWriter);
    };
  }

  SECTION("Full roundtrip with reserved writer")
  {
    std::vector<int32_t> data(10'000);
    for (size_t i = 0; i < data.size(); ++i)
      data[i] = static_cast<int32_t>(i);

    // Use BinProbe to calculate exact serialized size
    enki::BinProbe probe;
    const size_t expectedSize = enki::serialize(data, probe).size();

    BENCHMARK("Roundtrip - fresh writer")
    {
      enki::BinWriter writer;
      enki::serialize(data, writer);
      std::vector<int32_t> result;
      return enki::deserialize(result, enki::BinReader(writer.data()));
    };

    enki::BinWriter reservedWriter;
    reservedWriter.reserve(expectedSize);

    BENCHMARK("Roundtrip - pre-reserved writer")
    {
      reservedWriter.clear();
      enki::serialize(data, reservedWriter);
      std::vector<int32_t> result;
      return enki::deserialize(result, enki::BinReader(reservedWriter.data()));
    };
  }
}

TEST_CASE("Memory - BinProbe overhead", "[benchmark][memory]")
{
  std::vector<int32_t> data(10'000);
  for (size_t i = 0; i < data.size(); ++i)
    data[i] = static_cast<int32_t>(i);

  BENCHMARK("BinProbe size calculation")
  {
    enki::BinProbe probe;
    return enki::serialize(data, probe);
  };

  BENCHMARK("BinWriter serialize")
  {
    enki::BinWriter writer;
    return enki::serialize(data, writer);
  };

  // Combined: probe + reserve + serialize
  BENCHMARK("Probe + reserve + serialize")
  {
    enki::BinProbe probe;
    const size_t size = enki::serialize(data, probe).size();
    enki::BinWriter writer;
    writer.reserve(size);
    return enki::serialize(data, writer);
  };
}

// ============================================================================
// SECTION 7: Format Comparison (Same Data)
// ============================================================================

TEST_CASE("Comparison - Binary vs JSON", "[benchmark][comparison]")
{
  std::vector<int32_t> intData(1'000);
  for (size_t i = 0; i < intData.size(); ++i)
    intData[i] = static_cast<int32_t>(i);

  BENCHMARK("Binary: 1K int roundtrip")
  {
    enki::BinWriter writer;
    enki::serialize(intData, writer);
    std::vector<int32_t> result;
    return enki::deserialize(result, enki::BinReader(writer.data()));
  };

  BENCHMARK("JSON: 1K int roundtrip")
  {
    enki::JSONWriter writer;
    enki::serialize(intData, writer);
    std::vector<int32_t> result;
    return enki::deserialize(result, enki::JSONReader(writer.data().str()));
  };

  std::vector<std::string> strData;
  strData.reserve(100);
  for (size_t i = 0; i < 100; ++i)
    strData.push_back("item_" + std::to_string(i));

  BENCHMARK("Binary: 100 string roundtrip")
  {
    enki::BinWriter writer;
    enki::serialize(strData, writer);
    std::vector<std::string> result;
    return enki::deserialize(result, enki::BinReader(writer.data()));
  };

  BENCHMARK("JSON: 100 string roundtrip")
  {
    enki::JSONWriter writer;
    enki::serialize(strData, writer);
    std::vector<std::string> result;
    return enki::deserialize(result, enki::JSONReader(writer.data().str()));
  };
}
