#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "enki/legacy/manager.hpp"

TEST_CASE("Manager Composite Test (Custom type at top of data hierarchy)", "[manager][regression]")
{
  struct S
  {
    std::vector<double> numbers;
    uint32_t timestamp;

    bool operator==(const S &) const = default;
  };

  const S s1{
    {-2.0, 13.37},
    std::numeric_limits<uint32_t>::max()
  };
  std::remove_cvref_t<decltype(s1)> s2;

  const size_t totalSerSize = sizeof(enki::Manager<>::size_type) +
                              std::size(s1.numbers) * sizeof(decltype(s1.numbers)::value_type) +
                              sizeof(decltype(s1.timestamp));

  std::vector<std::byte> temp(totalSerSize);

  auto mgr = enki::Manager{};
  mgr.registerType<S, &S::timestamp, &S::numbers>();

  {
    const auto serRes = mgr.serialize(s1, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = mgr.deserialize(s2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(s1 == s2);
}

namespace
{
#pragma pack(push, 1)

  struct TrackInfo
  {
    enum class Style : uint8_t
    {
      Unknown = 0,
      Classic,
      Jazz,
      Rock
    };

    uint64_t playCount;
    Style style : 2 {};
    uint8_t rating : 6 {};

    constexpr bool operator==(const TrackInfo &) const = default;
  };

#pragma pack(pop)
} // namespace

TEST_CASE(
  "Manager Composite Test (Custom type at bottom of data hierarchy)",
  "[manager][regression]")
{
  const std::unordered_map<std::string, TrackInfo> allSongs{
    std::make_pair("Electric anthem", TrackInfo{9999, TrackInfo::Style::Rock, 7}),
    std::make_pair("Mozenhelm no.3", TrackInfo{47, TrackInfo::Style::Classic, 12})};

  std::remove_cvref_t<decltype(allSongs)> allSongs2;

  auto mgr = enki::Manager{};
  mgr.registerType<
    TrackInfo,
    ENKIWRAP(TrackInfo, style),
    ENKIWRAP(TrackInfo, rating),
    &TrackInfo::playCount>();

  const auto numBytesSuccess = mgr.numBytes(allSongs);
  REQUIRE_NOTHROW(numBytesSuccess.or_throw());

  std::vector<std::byte> temp(numBytesSuccess.size());

  {
    const auto serRes = mgr.serialize(allSongs, temp.begin());
    REQUIRE_NOTHROW(serRes.or_throw());
    REQUIRE(serRes.size() == std::size(temp));
    REQUIRE(serRes.get_iterator() == temp.end());
  }
  {
    const auto desRes = mgr.deserialize(allSongs2, temp.begin());
    REQUIRE_NOTHROW(desRes.or_throw());
    REQUIRE(desRes.size() == std::size(temp));
    REQUIRE(desRes.get_iterator() == temp.end());
  }

  REQUIRE(allSongs == allSongs2);
}
