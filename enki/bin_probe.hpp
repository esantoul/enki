#ifndef ENKI_BIN_PROBE_HPP
#define ENKI_BIN_PROBE_HPP

#include <cstdint>

#include "enki/impl/concepts.hpp"
#include "enki/impl/success.hpp"

namespace enki
{
  template <typename SizeType = uint32_t>
  class BinProbe
  {
  public:
    template <concepts::arithmetic_or_enum T>
    constexpr Success write(const T &)
    {
      return {sizeof(T)};
    }

    constexpr Success arrayBegin(size_t) const
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
  };
} // namespace enki

#endif // ENKI_BIN_PROBE_HPP
