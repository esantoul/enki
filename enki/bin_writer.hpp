#ifndef ENKI_BIN_WRITER_HPP
#define ENKI_BIN_WRITER_HPP

#include <algorithm>
#include <bit>
#include <cstdint>
#include <iterator>
#include <span>
#include <variant>
#include <vector>

#if __cpp_exceptions >= 199711
#include <stdexcept>
#else
#include <cstdlib>
#endif

#include "enki/bin_probe.hpp"
#include "enki/impl/concepts.hpp"
#include "enki/impl/policies.hpp"
#include "enki/impl/success.hpp"

namespace enki
{
  template <typename Policy = strict_t, typename SizeType = uint32_t>
  class BinWriter
  {
  public:
    using policy_type = Policy;                           // NOLINT
    using size_type = SizeType;                           // NOLINT
    static constexpr bool serialize_custom_names = false; // NOLINT

    BinWriter() = default;
    explicit constexpr BinWriter(Policy) {}

    template <concepts::arithmetic_or_enum T>
    constexpr Success write(const T &v)
    {
      const auto bytes = std::bit_cast<std::array<std::byte, sizeof(T)>>(v);
      std::copy(std::begin(bytes), std::end(bytes), std::back_inserter(mData));
      return {sizeof(T)};
    }

    constexpr Success write(const std::monostate &)
    {
      return {};  // No bytes written for monostate in binary format
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

    const std::vector<std::byte> &data() const
    {
      return mData;
    }

    std::vector<std::byte> &data()
    {
      return mData;
    }

    void reserve(size_t capacity)
    {
      mData.reserve(capacity);
    }

    void clear()
    {
      mData.clear();
    }

    /// Write skippable content with size prefix for forward compatibility
    template <typename WriteFunc>
    constexpr Success writeSkippable(WriteFunc &&writeContent)
    {
      // Probe phase to calculate size
      BinProbe<policy_type, size_type> probe;
      Success probeResult = writeContent(probe);
      if (!probeResult)
      {
        return probeResult;
      }

      // Write size prefix
      Success result = write(static_cast<size_type>(probeResult.size()));
      if (!result)
      {
        return result;
      }

      // Write actual data
      return result.update(writeContent(*this));
    }

    /// Write a variant: index + value (with size prefix if forward_compatible)
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
        // Forward compatible: wrap value with size prefix
        Success valueResult = writeSkippable([&](auto &w) { return writeValue(w); });
        return {indexResult.size() + valueResult.size()};
      }
      else
      {
        // Strict: write value directly
        Success valueResult = writeValue(*this);
        return {indexResult.size() + valueResult.size()};
      }
    }

    /// Write an optional: bool flag + value if present
    template <typename ValueFunc>
    constexpr Success writeOptional(bool hasValue, ValueFunc &&writeValue)
    {
      Success flagResult = write(hasValue);
      if (!flagResult)
      {
        return flagResult;
      }
      if (hasValue)
      {
        Success valueResult = writeValue(*this);
        return {flagResult.size() + valueResult.size()};
      }
      return flagResult;
    }

  private:
    std::vector<std::byte> mData;
  };

  template <typename Policy = strict_t, typename SizeType = uint32_t>
  class BinSpanWriter
  {
  public:
    using policy_type = Policy;                           // NOLINT
    using size_type = SizeType;                           // NOLINT
    static constexpr bool serialize_custom_names = false; // NOLINT

    BinSpanWriter(std::span<std::byte> byteSpan) :
      mDataSpan(byteSpan)
    {
    }

    explicit BinSpanWriter(Policy, std::span<std::byte> byteSpan) :
      mDataSpan(byteSpan)
    {
    }

    template <concepts::arithmetic_or_enum T>
    constexpr Success write(const T &v)
    {
      if (mCurrentSize + sizeof(T) > mDataSpan.size())
      {
#if __cpp_exceptions >= 199711
        throw std::out_of_range("BinSpanWriter out of range write");
#else
        std::abort();
#endif
      }

      const auto bytes = std::bit_cast<std::array<std::byte, sizeof(T)>>(v);
      std::copy(std::begin(bytes), std::end(bytes), mDataSpan.data() + mCurrentSize);

      mCurrentSize += sizeof(T);

      return {sizeof(T)};
    }

    constexpr Success write(const std::monostate &)
    {
      return {};  // No bytes written for monostate in binary format
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

    std::span<const std::byte> data() const
    {
      return mDataSpan.subspan(0, mCurrentSize);
    }

    /// Write skippable content with size prefix for forward compatibility
    template <typename WriteFunc>
    constexpr Success writeSkippable(WriteFunc &&writeContent)
    {
      // Probe phase to calculate size
      BinProbe<policy_type, size_type> probe;
      Success probeResult = writeContent(probe);
      if (!probeResult)
      {
        return probeResult;
      }

      // Write size prefix
      Success result = write(static_cast<size_type>(probeResult.size()));
      if (!result)
      {
        return result;
      }

      // Write actual data
      return result.update(writeContent(*this));
    }

    /// Write a variant: index + value (with size prefix if forward_compatible)
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
        // Forward compatible: wrap value with size prefix
        Success valueResult = writeSkippable([&](auto &w) { return writeValue(w); });
        return {indexResult.size() + valueResult.size()};
      }
      else
      {
        // Strict: write value directly
        Success valueResult = writeValue(*this);
        return {indexResult.size() + valueResult.size()};
      }
    }

    /// Write an optional: bool flag + value if present
    template <typename ValueFunc>
    constexpr Success writeOptional(bool hasValue, ValueFunc &&writeValue)
    {
      Success flagResult = write(hasValue);
      if (!flagResult)
      {
        return flagResult;
      }
      if (hasValue)
      {
        Success valueResult = writeValue(*this);
        return {flagResult.size() + valueResult.size()};
      }
      return flagResult;
    }

  private:
    std::span<std::byte> mDataSpan;
    size_t mCurrentSize = 0;
  };

  // Deduction guides for BinWriter
  BinWriter() -> BinWriter<strict_t, uint32_t>;
  BinWriter(strict_t) -> BinWriter<strict_t, uint32_t>;
  BinWriter(forward_compatible_t) -> BinWriter<forward_compatible_t, uint32_t>;

  // Deduction guides for BinSpanWriter
  BinSpanWriter(std::span<std::byte>) -> BinSpanWriter<strict_t, uint32_t>;
  BinSpanWriter(strict_t, std::span<std::byte>) -> BinSpanWriter<strict_t, uint32_t>;
  BinSpanWriter(forward_compatible_t, std::span<std::byte>) -> BinSpanWriter<forward_compatible_t, uint32_t>;
} // namespace enki

#endif // ENKI_BIN_WRITER_HPP
