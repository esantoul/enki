#ifndef ENKI_BIN_PROBE_HPP
#define ENKI_BIN_PROBE_HPP

#include <cstdint>
#include <string_view>

#include "enki/impl/concepts.hpp"
#include "enki/impl/policies.hpp"
#include "enki/impl/success.hpp"

namespace enki
{
  /// Probe writer that counts bytes without writing data
  /// Used for calculating serialized size before actual serialization
  template <typename Policy = strict_t, typename SizeType = uint32_t>
  class BinProbe
  {
  public:
    using policy_type = Policy;                           // NOLINT
    using size_type = SizeType;                           // NOLINT
    static constexpr bool serialize_custom_names = false; // NOLINT

    template <concepts::arithmetic_or_enum T>
    constexpr Success write(const T &)
    {
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

    constexpr Success objectBegin() const
    {
      return {};
    }

    constexpr Success objectEnd() const
    {
      return {};
    }

    constexpr Success nextObjectElement() const
    {
      return {};
    }

    constexpr Success objectName(std::string_view /* name */) const
    {
      return {};
    }

    /// Write skippable content - probes content and adds size field overhead
    template <typename WriteFunc>
    constexpr Success writeSkippable(WriteFunc &&writeContent)
    {
      Success probeResult = writeContent(*this);
      if (!probeResult)
      {
        return probeResult;
      }
      // Add size field overhead to total
      return {sizeof(size_type) + probeResult.size()};
    }
  };
} // namespace enki

#endif // ENKI_BIN_PROBE_HPP
