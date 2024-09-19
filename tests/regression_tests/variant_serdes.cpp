#include <variant>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

TEST_CASE("Variant SerDes", "[regression]")
{
  enki::BinWriter writer;

  static constexpr std::variant<char, double, float, int> kValueToSerialize = 3.14;
  const auto serRes = enki::serialize(kValueToSerialize, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sizeof(enki::BinWriter<>::size_type) + sizeof(double));

  std::remove_cvref_t<decltype(kValueToSerialize)> deserializedValue;
  const auto desRes = enki::deserialize(deserializedValue, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == sizeof(enki::BinWriter<>::size_type) + sizeof(double));

  REQUIRE(deserializedValue == kValueToSerialize);
}
