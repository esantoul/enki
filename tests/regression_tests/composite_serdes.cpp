#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"

namespace
{
  struct S
  {
    std::vector<double> numbers;
    uint32_t timestamp;

    constexpr auto operator<=>(const S &) const noexcept = default;

    struct EnkiSerial
    {
      // NOLINTNEXTLINE
      static constexpr auto members = std::make_tuple(&S::numbers, &S::timestamp);
    };
  };
} // namespace

TEST_CASE("Composite Test (Custom type at top of data hierarchy)", "[regression]")
{
  const S s1{
    {-2.0, 13.37},
    std::numeric_limits<uint32_t>::max()
  };
  std::remove_cvref_t<decltype(s1)> s2;

  const size_t totalSerSize = sizeof(enki::BinWriter<>::size_type) +
                              std::size(s1.numbers) * sizeof(decltype(s1.numbers)::value_type) +
                              sizeof(decltype(s1.timestamp));

  enki::BinWriter writer;

  const auto serRes = enki::serialize(s1, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  REQUIRE(serRes.size() == totalSerSize);

  const auto desRes = enki::deserialize(s2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == totalSerSize);

  REQUIRE(s1 == s2);
}

namespace
{
#pragma pack(push, 1)

  struct TrackInfo
  {
    enum class eStyle : uint8_t
    {
      Unknown = 0,
      Classic,
      Jazz,
      Rock
    };

    uint64_t playCount;
    eStyle style : 2 {};
    uint8_t rating : 6 {};

    constexpr auto operator<=>(const TrackInfo &) const noexcept = default;

    struct EnkiSerial;
  };

  struct TrackInfo::EnkiSerial
  {
    // NOLINTNEXTLINE
    static constexpr auto members = std::make_tuple(
      ENKIWRAP(TrackInfo, style),
      ENKIWRAP(TrackInfo, rating),
      &TrackInfo::playCount);
  };

#pragma pack(pop)
} // namespace

TEST_CASE("Composite Test (Custom type at bottom of data hierarchy)", "[regression]")
{
  const std::unordered_map<std::string, TrackInfo> allSongs{
    std::make_pair("Electric anthem", TrackInfo{9999, TrackInfo::eStyle::Rock, 7}),
    std::make_pair("Mozenhelm no.3", TrackInfo{47, TrackInfo::eStyle::Classic, 12})};

  std::remove_cvref_t<decltype(allSongs)> allSongs2;

  // const auto numBytesSuccess = mgr.numBytes(allSongs);
  // REQUIRE_NOTHROW(numBytesSuccess.or_throw());

  enki::BinWriter writer;

  const auto serRes = enki::serialize(allSongs, writer);
  REQUIRE_NOTHROW(serRes.or_throw());
  // REQUIRE(serRes.size() == std::size(temp));

  const auto desRes = enki::deserialize(allSongs2, enki::BinReader(writer.data()));
  REQUIRE_NOTHROW(desRes.or_throw());
  REQUIRE(desRes.size() == serRes.size());

  REQUIRE(allSongs == allSongs2);
}
