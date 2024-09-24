#include <optional>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

TEST_CASE("Optional (No Value) SerDes", "[regression]")
{
  enki::BinWriter writer;

  static constexpr std::optional<char> kValueToSerialize;
  const auto serRes = enki::serialize(kValueToSerialize, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == 1);

  std::optional<char> deserializedValue = 'z';
  const auto desRes = enki::deserialize(deserializedValue, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == 1);

  REQUIRE(deserializedValue == kValueToSerialize);
}

TEST_CASE("Optional (With Value) SerDes", "[regression]")
{
  enki::BinWriter writer;

  static constexpr std::optional<char> kValueToSerialize = 'a';
  const auto serRes = enki::serialize(kValueToSerialize, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == 2);

  std::optional<char> deserializedValue;
  const auto desRes = enki::deserialize(deserializedValue, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == 2);

  REQUIRE(deserializedValue == kValueToSerialize);
}
