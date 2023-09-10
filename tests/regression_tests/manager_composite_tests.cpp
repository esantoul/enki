#include <cstddef>
#include <cstdint>
#include <array>
#include <unordered_map>
#include <numbers>
#include <numeric>
#include <limits>
#include <vector>

#include "enki/manager.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Manager Composite Test (Custom type at top of data hierarchy)", "[manager]")
{
  struct S
  {
    std::vector<double> numbers;
    uint32_t timestamp;

    bool operator==(const S &) const = default;
  };

  S s1{{-2.0, 13.37}, std::numeric_limits<uint32_t>::max()};
  decltype(s1) s2;

  const size_t totalSerSize =
    sizeof(enki::Manager<>::size_type) + std::size(s1.numbers) * sizeof(decltype(s1.numbers)::value_type) +
    sizeof(decltype(s1.timestamp));

  std::vector<std::byte> temp(totalSerSize);

  auto mgr = enki::Manager{};
  mgr.Register<S, &S::timestamp, &S::numbers>();

  {
    const auto ser_res = mgr.Serialize(s1, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == std::size(temp));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = mgr.Deserialize(s2, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == std::size(temp));
    REQUIRE(des_res.get_iterator() == temp.end());
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
    Style style : 2{};
    uint8_t rating : 6{};

    constexpr bool operator==(const TrackInfo &) const = default;
  };
#pragma pack(pop)
}

TEST_CASE("Manager Composite Test (Custom type at bottom of data hierarchy)", "[manager]")
{
  std::unordered_map<std::string, TrackInfo> allSongs{
    std::make_pair("Electric anthem", TrackInfo{9999, TrackInfo::Style::Rock, 7}),
    std::make_pair("Mozenhelm no.3", TrackInfo{47, TrackInfo::Style::Classic, 12})
  };

  decltype(allSongs) allSongs2;

  auto mgr = enki::Manager{};
  mgr.Register<TrackInfo, ENKIWRAP(TrackInfo, style), ENKIWRAP(TrackInfo, rating), &TrackInfo::playCount>();

  const auto numBytesResult = mgr.NumBytes(allSongs);
  REQUIRE_NOTHROW(numBytesResult.or_throw());

  std::vector<std::byte> temp(numBytesResult.size());

  {
    const auto ser_res = mgr.Serialize(allSongs, temp.begin());
    REQUIRE_NOTHROW(ser_res.or_throw());
    REQUIRE(ser_res.size() == std::size(temp));
    REQUIRE(ser_res.get_iterator() == temp.end());
  }
  {
    const auto des_res = mgr.Deserialize(allSongs2, temp.begin());
    REQUIRE_NOTHROW(des_res.or_throw());
    REQUIRE(des_res.size() == std::size(temp));
    REQUIRE(des_res.get_iterator() == temp.end());
  }

  REQUIRE(allSongs == allSongs2);
}
