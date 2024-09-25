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
    using size_type = SizeType; // NOLINT

    template <concepts::arithmetic_or_enum T>
    constexpr Success write(const T &v)
    {
      const auto bytes = std::bit_cast<std::array<std::byte, sizeof(T)>>(v);
      std::copy(std::begin(bytes), std::end(bytes), std::back_inserter(mData));
      return {sizeof(T)};
    }

    constexpr Success arrayBegin() const
    {
      return {};
    }

    constexpr Success arrayEnd() const
    {
      return {};
    }

    constexpr Success nextArrayElement() const
    {
      return {};
    }

    constexpr Success rangeBegin(size_t numElements)
    {
      return write(static_cast<SizeType>(numElements));
    }

    constexpr Success rangeEnd() const
    {
      return {};
    }

    constexpr Success nextRangeElement() const
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
} // namespace enki

#endif // ENKI_BIN_WRITER_HPP
