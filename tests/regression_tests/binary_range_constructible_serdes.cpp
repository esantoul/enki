#include <cstdint>

#include <deque>
#include <forward_list>
#include <list>
#include <set>
#include <unordered_set>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

TEST_CASE("Range Constructible (std::vector) SerDes", "[regression]")
{
  const std::vector<int32_t> rc1{1337, -99};
  std::vector<int32_t> rc2;

  enki::BinWriter writer;

  const size_t expectedSerDesSize =
    sizeof(decltype(writer)::size_type) + std::size(rc1) * sizeof(decltype(rc1)::value_type);

  const auto serRes = enki::serialize(rc1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == expectedSerDesSize);

  const auto desRes = enki::deserialize(rc2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == expectedSerDesSize);

  REQUIRE(rc1 == rc2);
}

TEST_CASE("Range Constructible (std::set) SerDes", "[regression]")
{
  const std::set<int32_t> rc1{1337, -99, 1337, 42};
  std::set<int32_t> rc2;

  enki::BinWriter writer;

  const size_t expectedSerDesSize =
    sizeof(decltype(writer)::size_type) + std::size(rc1) * sizeof(decltype(rc1)::value_type);

  const auto serRes = enki::serialize(rc1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == expectedSerDesSize);

  const auto desRes = enki::deserialize(rc2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == expectedSerDesSize);

  REQUIRE(rc1 == rc2);
}

TEST_CASE("Range Constructible (std::multiset) SerDes", "[regression]")
{
  const std::multiset<int32_t> rc1{1337, -99, 1337, 42};
  std::multiset<int32_t> rc2;

  enki::BinWriter writer;

  const size_t expectedSerDesSize =
    sizeof(decltype(writer)::size_type) + std::size(rc1) * sizeof(decltype(rc1)::value_type);

  const auto serRes = enki::serialize(rc1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == expectedSerDesSize);

  const auto desRes = enki::deserialize(rc2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == expectedSerDesSize);

  REQUIRE(rc1 == rc2);
}

TEST_CASE("Range Constructible (std::unordered_set) SerDes", "[regression]")
{
  const std::unordered_set<int32_t> rc1{1337, -99, 1337, 42};
  std::unordered_set<int32_t> rc2;

  enki::BinWriter writer;

  const size_t expectedSerDesSize =
    sizeof(decltype(writer)::size_type) + std::size(rc1) * sizeof(decltype(rc1)::value_type);

  const auto serRes = enki::serialize(rc1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == expectedSerDesSize);

  const auto desRes = enki::deserialize(rc2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == expectedSerDesSize);

  REQUIRE(rc1 == rc2);
}

TEST_CASE("Range Constructible (std::unordered_multiset) SerDes", "[regression]")
{
  const std::unordered_multiset<int32_t> rc1{1337, -99, 1337, 42};
  std::unordered_multiset<int32_t> rc2;

  enki::BinWriter writer;

  const size_t expectedSerDesSize =
    sizeof(decltype(writer)::size_type) + std::size(rc1) * sizeof(decltype(rc1)::value_type);

  const auto serRes = enki::serialize(rc1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == expectedSerDesSize);

  const auto desRes = enki::deserialize(rc2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == expectedSerDesSize);

  REQUIRE(rc1 == rc2);
}

TEST_CASE("Range Constructible (std::deque) SerDes", "[regression]")
{
  const std::deque<int32_t> rc1{1337, -99, 1337, 42};
  std::deque<int32_t> rc2;

  enki::BinWriter writer;

  const size_t expectedSerDesSize =
    sizeof(decltype(writer)::size_type) + std::size(rc1) * sizeof(decltype(rc1)::value_type);

  const auto serRes = enki::serialize(rc1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == expectedSerDesSize);

  const auto desRes = enki::deserialize(rc2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == expectedSerDesSize);

  REQUIRE(rc1 == rc2);
}

TEST_CASE("Range Constructible (std::forward_list) SerDes", "[regression]")
{
  const std::forward_list<int32_t> rc1{1337, -99, 1337, 42};
  std::forward_list<int32_t> rc2;

  const auto rc1Size = std::distance(std::begin(rc1), std::end(rc1));

  enki::BinWriter writer;

  const size_t expectedSerDesSize =
    sizeof(decltype(writer)::size_type) + rc1Size * sizeof(decltype(rc1)::value_type);

  const auto serRes = enki::serialize(rc1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == expectedSerDesSize);

  const auto desRes = enki::deserialize(rc2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == expectedSerDesSize);

  REQUIRE(rc1 == rc2);
}

TEST_CASE("Range Constructible (std::list) SerDes", "[regression]")
{
  const std::list<int32_t> rc1{1337, -99, 1337, 42};
  std::list<int32_t> rc2;

  enki::BinWriter writer;

  const size_t expectedSerDesSize =
    sizeof(decltype(writer)::size_type) + std::size(rc1) * sizeof(decltype(rc1)::value_type);

  const auto serRes = enki::serialize(rc1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == expectedSerDesSize);

  const auto desRes = enki::deserialize(rc2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == expectedSerDesSize);

  REQUIRE(rc1 == rc2);
}
