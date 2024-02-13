#ifndef ENKI_BIN_WRITER_HPP
#define ENKI_BIN_WRITER_HPP

#include "enki/impl/concepts.hpp"
#include "enki/impl/success.hpp"

#include <bit>
#include <vector>

namespace enki
{
  template <typename SizeType = uint32_t>
  class BinWriter
  {
  public:
    template <concepts::arithmetic_or_enum T>
    constexpr Success<void> write(const T &v)
    {
      const auto bytes = std::bit_cast<std::array<std::byte, sizeof(T)>>(v);
      std::ranges::copy(bytes, std::back_inserter(mData));
      return {};
    }

    constexpr Success<void> arrayBegin(size_t) const
    {
      return {};
    }

    constexpr Success<void> arrayEnd() const
    {
      return {};
    }

    constexpr Success<void> nextArrayElement() const
    {
      return {};
    }

    constexpr Success<void> rangeBegin(size_t numElements)
    {
      return write(static_cast<SizeType>(numElements));
    }

    constexpr Success<void> rangeEnd() const
    {
      return {};
    }

    constexpr Success<void> nextRangeElement() const
    {
      return {};
    }

    const std::vector<std::byte> &data() const
    {
      return mData;
    }

  private:
    std::vector<std::byte> mData;
  };

  template <typename SizeType = uint32_t>
  class BinSizeProbe
  {
  public:
    template <concepts::arithmetic_or_enum T>
    constexpr Success<void> write(const T &)
    {
      mSize += sizeof(T);
      return {};
    }

    constexpr Success<void> arrayBegin(size_t) const
    {
      return {};
    }

    constexpr Success<void> arrayEnd() const
    {
      return {};
    }

    constexpr Success<void> nextArrayElement() const
    {
      return {};
    }

    constexpr Success<void> rangeBegin(size_t numElements)
    {
      return write(static_cast<SizeType>(numElements));
    }

    constexpr Success<void> rangeEnd() const
    {
      return {};
    }

    constexpr Success<void> nextRangeElement() const
    {
      return {};
    }

    size_t size() const
    {
      return mSize;
    }

  private:
    size_t mSize{};
  };
} // namespace enki

#endif // ENKI_BIN_WRITER_HPP
