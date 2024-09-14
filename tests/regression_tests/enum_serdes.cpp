#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

enum class eMyEnum : char
{
  ONE,
  TWO
};

TEST_CASE("Enum SerDes", "[regression]")
{
  const eMyEnum e1 = eMyEnum::ONE;
  eMyEnum e2 = eMyEnum::TWO;

  enki::BinWriter writer;

  const auto serRes = enki::serialize(e1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == sizeof(e1));

  const auto desRes = enki::deserialize(e2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == sizeof(e1));

  REQUIRE(e1 == e2);
}
