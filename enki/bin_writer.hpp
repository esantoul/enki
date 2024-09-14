#ifndef ENKI_BIN_WRITER_HPP
#define ENKI_BIN_WRITER_HPP

#include <algorithm>
#include <bit>
#include <vector>

#include "enki/impl/concepts.hpp"
#include "enki/impl/success.hpp"

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
      std::copy(std::begin(bytes), std::end(bytes), std::back_inserter(mData));
      return {sizeof(T)};
    }

    constexpr Success<void> arrayBegin() const
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
      return {sizeof(T)};
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
  };
} // namespace enki

#endif // ENKI_BIN_WRITER_HPP
