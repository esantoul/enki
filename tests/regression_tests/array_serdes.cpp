#include <algorithm>
#include <array>
#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

TEST_CASE("C-Array SerDes", "[regression]")
{
  const int64_t carr1[] = {1337, -99};

  enki::BinWriter writer;

  const auto serRes = enki::serialize(carr1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sizeof(carr1));

  int64_t carr2[std::size(carr1)]{};

  const auto desRes = enki::deserialize(carr2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == sizeof(carr1));

  REQUIRE(std::equal(std::begin(carr1), std::end(carr1), std::begin(carr2)));
}

TEST_CASE("C++ Array (std::array) SerDes", "[regression]")
{
  const std::array<int64_t, 2> arr1 = {1337, -99};

  enki::BinWriter writer;

  const auto serRes = enki::serialize(arr1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sizeof(arr1));

  std::array<int64_t, 2> arr2{};

  const auto desRes = enki::deserialize(arr2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == sizeof(arr1));

  REQUIRE(arr1 == arr2);
}
