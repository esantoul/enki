#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

TEST_CASE("Arithmetic SerDes", "[regression]")
{
  enki::BinWriter writer;

  static constexpr int32_t kValueToSerialize = 42;
  const auto serRes = enki::serialize(kValueToSerialize, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sizeof(kValueToSerialize));

  int32_t deserializedValue = 0;
  const auto desRes = enki::deserialize(deserializedValue, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == sizeof(kValueToSerialize));

  REQUIRE(deserializedValue == kValueToSerialize);
}
