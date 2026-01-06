#ifndef ENKI_BIN_PROBE_HPP
#define ENKI_BIN_PROBE_HPP

#include <cstdint>
#include <string_view>
#include <variant>

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

    BinProbe() = default;
    explicit constexpr BinProbe(Policy) {}

    template <concepts::arithmetic_or_enum T>
    constexpr Success write(const T &)
    {
      return {sizeof(T)};
    }

    constexpr Success write(const std::monostate &)
    {
      return {};  // No bytes for monostate
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

    /// Probe a variant: index + value (with size prefix if forward_compatible)
    template <typename IndexFunc, typename ValueFunc>
    constexpr Success writeVariant(IndexFunc &&writeIndex, ValueFunc &&writeValue)
    {
      Success indexResult = writeIndex(*this);
      if (!indexResult)
      {
        return indexResult;
      }

      if constexpr (std::is_same_v<Policy, forward_compatible_t>)
      {
        // Forward compatible: add size prefix overhead
        Success valueResult = writeSkippable([&](auto &w) { return writeValue(w); });
        return {indexResult.size() + valueResult.size()};
      }
      else
      {
        // Strict: just probe value
        Success valueResult = writeValue(*this);
        return {indexResult.size() + valueResult.size()};
      }
    }
  };

  // Deduction guides for BinProbe
  BinProbe() -> BinProbe<strict_t, uint32_t>;
  BinProbe(strict_t) -> BinProbe<strict_t, uint32_t>;
  BinProbe(forward_compatible_t) -> BinProbe<forward_compatible_t, uint32_t>;
} // namespace enki

#endif // ENKI_BIN_PROBE_HPP
