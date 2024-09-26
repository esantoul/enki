#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_writer.hpp"
#include "enki/enki_serialize.hpp"

namespace
{
  bool gNewAllowed = true;
}

void *operator new(size_t count)
{
  if (count && gNewAllowed)
  {
    if (void *pRet = std::malloc(count))
    {
      return pRet;
    }
  }

  throw std::bad_alloc{}; // required by [new.delete.single]/3
}

namespace
{
  constexpr std::array<uint8_t, 1024> kValueToSerialize = [] {
    std::array<uint8_t, 1024> ret{};
    for (size_t i = 0; i < std::size(ret); ++i)
    {
      ret[i] = i;
    }

    return ret;
  }();
} // namespace

TEST_CASE("Bin Writer Basic Use", "[unit][BinSpanWriter]")
{
  enki::BinWriter<> writer;

  enki::Success serRes;
  REQUIRE_NOTHROW(serRes = enki::serialize(kValueToSerialize, writer));
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sizeof(kValueToSerialize));
  REQUIRE(writer.data().size() == sizeof(kValueToSerialize));

  for (size_t i = 0; i < std::size(writer.data()); ++i)
  {
    REQUIRE(static_cast<uint8_t>(writer.data()[i]) == (i & 0xFF));
  }
}

TEST_CASE("Bin Writer Reserve", "[unit][BinSpanWriter]")
{
  enki::BinWriter<> writer;

  gNewAllowed = false;

  // Check that our throwing new operator works properly
  REQUIRE_THROWS(writer.reserve(1024));

  gNewAllowed = true;

  // This should be the only memory allocation of this test
  REQUIRE_NOTHROW(writer.reserve(1024));

  // From here no new memory allocation is allowed
  gNewAllowed = false;

  enki::Success serRes;
  REQUIRE_NOTHROW(serRes = enki::serialize(kValueToSerialize, writer));

  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sizeof(kValueToSerialize));
  REQUIRE(writer.data().size() == sizeof(kValueToSerialize));

  for (size_t i = 0; i < std::size(writer.data()); ++i)
  {
    REQUIRE(static_cast<uint8_t>(writer.data()[i]) == (i & 0xFF));
  }

  // We allow memory allocation now that the test is done
  gNewAllowed = true;
}

TEST_CASE("Bin Writer Clear", "[unit][BinSpanWriter]")
{
  enki::BinWriter<> writer;

  gNewAllowed = false;

  // Check that our throwing new operator works properly
  REQUIRE_THROWS(writer.write(42));

  gNewAllowed = true;

  // First serialization, underlying vector should grow to adequate size
  {
    static constexpr std::array<uint8_t, 1024> kDummyValues{0xFF};

    STATIC_CHECK(sizeof(kDummyValues) == sizeof(kValueToSerialize));

    auto serRes = enki::serialize(kDummyValues, writer);
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == sizeof(kDummyValues));

    REQUIRE(writer.data()[0] == static_cast<std::byte>(0xFF));

    for (size_t i = 1; i < std::size(writer.data()); ++i)
    {
      REQUIRE(static_cast<uint8_t>(writer.data()[i]) == 0);
    }
  }

  // Data should be deleted but underlying vector capacity should not have changed
  writer.clear();

  // From here no new memory allocation is allowed
  gNewAllowed = false;

  enki::Success serRes;
  REQUIRE_NOTHROW(serRes = enki::serialize(kValueToSerialize, writer));

  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sizeof(kValueToSerialize));
  REQUIRE(writer.data().size() == sizeof(kValueToSerialize));

  for (size_t i = 0; i < std::size(writer.data()); ++i)
  {
    REQUIRE(static_cast<uint8_t>(writer.data()[i]) == (i & 0xFF));
  }

  // We allow memory allocation now that the test is done
  gNewAllowed = true;
}

TEST_CASE("Bin Span Writer Basic Use", "[unit][BinSpanWriter]")
{
  auto dataHolder = std::make_unique<std::array<std::byte, 1024>>();
  enki::BinSpanWriter<> writer(*dataHolder);

  enki::Success serRes;
  REQUIRE_NOTHROW(serRes = enki::serialize(kValueToSerialize, writer));
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sizeof(kValueToSerialize));

  for (size_t i = 0; i < std::size(*dataHolder); ++i)
  {
    REQUIRE(static_cast<uint8_t>((*dataHolder)[i]) == (i & 0xFF));
  }
}

TEST_CASE("Bin Span Writer Misuse Behaviour", "[unit][BinSpanWriter]")
{
  {
    enki::BinSpanWriter<> writer({});

    enki::Success serRes;
    REQUIRE_THROWS(serRes = enki::serialize(kValueToSerialize, writer));
  }

  {
    std::array<std::byte, 16> dataSpan{};
    enki::BinSpanWriter<> writer(dataSpan);

    enki::Success serRes;
    REQUIRE_THROWS(serRes = enki::serialize(kValueToSerialize, writer));

    for (size_t i = 0; i < std::size(dataSpan); ++i)
    {
      REQUIRE(static_cast<uint8_t>((dataSpan)[i]) == (i & 0xFF));
    }
  }
}
