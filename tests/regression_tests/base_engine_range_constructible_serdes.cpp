#include <cstddef>
#include <cstdint>

#include <deque>
#include <forward_list>
#include <list>
#include <set>
#include <unordered_set>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "enki/legacy/base_engine.hpp"

TEST_CASE("Base Engine Range Constructible (std::vector) SerDes", "[base_engine][regression]")
{
  const std::vector<int32_t> rc1{1337, -99};
  std::vector<int32_t> rc2;
  std::vector<std::byte> temp(
    sizeof(enki::BaseEngine::size_type) + sizeof(decltype(rc1)::value_type) * std::size(rc1));

  {
    const auto serRes = enki::BaseEngine::serialize(rc1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(rc2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(rc1 == rc2);
}

TEST_CASE("Base Engine Range Constructible (std::set) SerDes", "[base_engine][regression]")
{
  const std::set<int32_t> rc1{1337, -99, 1337, 42};
  std::set<int32_t> rc2;
  std::vector<std::byte> temp(
    sizeof(enki::BaseEngine::size_type) + sizeof(decltype(rc1)::value_type) * std::size(rc1));

  {
    const auto serRes = enki::BaseEngine::serialize(rc1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(rc2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(rc1 == rc2);
}

TEST_CASE("Base Engine Range Constructible (std::multiset) SerDes", "[base_engine][regression]")
{
  const std::multiset<int32_t> rc1{1337, -99, 1337, 42};
  std::multiset<int32_t> rc2;
  std::vector<std::byte> temp(
    sizeof(enki::BaseEngine::size_type) + sizeof(decltype(rc1)::value_type) * std::size(rc1));

  {
    const auto serRes = enki::BaseEngine::serialize(rc1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(rc2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(rc1 == rc2);
}

TEST_CASE(
  "Base Engine Range Constructible (std::unordered_set) SerDes",
  "[base_engine][regression]")
{
  const std::unordered_set<int32_t> rc1{1337, -99, 1337, 42};
  std::unordered_set<int32_t> rc2;
  std::vector<std::byte> temp(
    sizeof(enki::BaseEngine::size_type) + sizeof(decltype(rc1)::value_type) * std::size(rc1));

  {
    const auto serRes = enki::BaseEngine::serialize(rc1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(rc2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(rc1 == rc2);
}

TEST_CASE(
  "Base Engine Range Constructible (std::unordered_multiset) SerDes",
  "[base_engine][regression]")
{
  const std::unordered_multiset<int32_t> rc1{1337, -99, 1337, 42};
  std::unordered_multiset<int32_t> rc2;
  std::vector<std::byte> temp(
    sizeof(enki::BaseEngine::size_type) + sizeof(decltype(rc1)::value_type) * std::size(rc1));

  {
    const auto serRes = enki::BaseEngine::serialize(rc1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(rc2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(rc1 == rc2);
}

TEST_CASE("Base Engine Range Constructible (std::deque) SerDes", "[base_engine][regression]")
{
  const std::deque<int32_t> rc1{1337, -99, 1337, 42};
  std::deque<int32_t> rc2;
  std::vector<std::byte> temp(
    sizeof(enki::BaseEngine::size_type) + sizeof(decltype(rc1)::value_type) * std::size(rc1));

  {
    const auto serRes = enki::BaseEngine::serialize(rc1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(rc2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(rc1 == rc2);
}

TEST_CASE("Base Engine Range Constructible (std::forward_list) SerDes", "[base_engine][regression]")
{
  const std::forward_list<int32_t> rc1{1337, -99, 1337, 42};
  std::forward_list<int32_t> rc2;

  const auto rc1_size = std::distance(std::begin(rc1), std::end(rc1));
  std::vector<std::byte> temp(
    sizeof(enki::BaseEngine::size_type) + sizeof(decltype(rc1)::value_type) * rc1_size);

  {
    const auto serRes = enki::BaseEngine::serialize(rc1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(rc2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(rc1 == rc2);
}

TEST_CASE("Base Engine Range Constructible (std::list) SerDes", "[base_engine][regression]")
{
  const std::list<int32_t> rc1{1337, -99, 1337, 42};
  std::list<int32_t> rc2;
  std::vector<std::byte> temp(
    sizeof(enki::BaseEngine::size_type) + sizeof(decltype(rc1)::value_type) * std::size(rc1));

  {
    const auto serRes = enki::BaseEngine::serialize(rc1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = enki::BaseEngine::deserialize(rc2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(rc1 == rc2);
}
