#ifndef ENKI_BIN_READER_HPP
#define ENKI_BIN_READER_HPP

#include <cstdint>
#include <cstring>
#include <span>
#include <variant>
#include <vector>

#if __cpp_exceptions >= 199711
#include <stdexcept>
#else
#include <cstdlib>
#endif

#include "enki/impl/concepts.hpp"
#include "enki/impl/policies.hpp"
#include "enki/impl/success.hpp"

namespace enki
{
  template <typename Policy = strict_t, typename SizeType = uint32_t>
  class BinSpanReader
  {
  public:
    using policy_type = Policy;                           // NOLINT
    using size_type = SizeType;                           // NOLINT
    static constexpr bool serialize_custom_names = false; // NOLINT

    BinSpanReader(std::span<const std::byte> data) :
      mSpan(data)
    {
    }

    template <concepts::arithmetic_or_enum T>
    constexpr Success read(T &v)
    {
      if (mCurrentIndex + sizeof(T) > mSpan.size())
      {
#if __cpp_exceptions >= 199711

        throw std::out_of_range("BinReader out of range read");
#else
        std::abort();
#endif
      }
      std::memcpy(&v, mSpan.data() + mCurrentIndex, sizeof(T));
      mCurrentIndex += sizeof(T);
      return {sizeof(T)};
    }

    constexpr Success read(std::monostate &)
    {
      return {};  // No bytes to read for monostate
    }

    /// Skip the size hint only - reads and discards the size prefix
    /// Used for forward compatibility when deserializing a known variant index
    constexpr Success skipHint()
    {
      SizeType size{};
      return read(size);
    }

    /// Skip the size hint and value - reads size then skips that many bytes
    /// Used for forward compatibility when encountering unknown variant types
    constexpr Success skipHintAndValue()
    {
      SizeType size{};
      Success result = read(size);
      if (!result)
      {
        return result;
      }

      if (mCurrentIndex + size > mSpan.size())
      {
#if __cpp_exceptions >= 199711
        throw std::out_of_range("BinReader skipHintAndValue out of range");
#else
        std::abort();
#endif
      }
      mCurrentIndex += size;
      return {sizeof(SizeType) + size};
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

    constexpr Success rangeBegin(size_t &numElements)
    {
      SizeType temp{};
      auto ret = read(temp);
      numElements = temp;
      return ret;
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

    const std::span<const std::byte> &data() const
    {
      return mSpan;
    }

    size_t remainingBytes() const noexcept
    {
      return mSpan.size() - mCurrentIndex;
    }

    /// Read variant index from binary format
    constexpr Success readVariantIndex(SizeType &index)
    {
      return read(index);
    }

    /// Finish reading a variant - no-op for binary format
    constexpr Success finishVariant()
    {
      return {};
    }

  private:
    std::span<const std::byte> mSpan;
    size_t mCurrentIndex{};
  };

  template <typename Policy = strict_t, typename SizeType = uint32_t>
  class BinReader : public BinSpanReader<Policy, SizeType>
  {
  public:
    using policy_type = typename BinSpanReader<Policy, SizeType>::policy_type; // NOLINT
    using size_type = typename BinSpanReader<Policy, SizeType>::size_type;     // NOLINT

    BinReader(std::span<const std::byte> data) :
      BinSpanReader<Policy, SizeType>({}),
      mData(std::begin(data), std::end(data))
    {
      static_cast<BinSpanReader<Policy, SizeType> &>(*this) = {mData};
    }

    using BinSpanReader<Policy, SizeType>::read;
    using BinSpanReader<Policy, SizeType>::skipHint;
    using BinSpanReader<Policy, SizeType>::skipHintAndValue;
    using BinSpanReader<Policy, SizeType>::readVariantIndex;
    using BinSpanReader<Policy, SizeType>::finishVariant;

  private:
    std::vector<std::byte> mData;
  };
} // namespace enki

#endif // ENKI_BIN_READER_HPP
