#ifndef ENKI_BIN_READER_HPP
#define ENKI_BIN_READER_HPP

#include <cstdint>
#include <cstring>
#include <span>
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
    using policy_type = Policy;                                                    // NOLINT
    using size_type = SizeType;                                                    // NOLINT
    static constexpr bool serialize_custom_names = false;                          // NOLINT
    static constexpr bool requires_size_prefix_for_forward_compatibility = true;   // NOLINT

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

    /// Skip a value - reads embedded size then skips that many bytes
    /// Used for forward compatibility when encountering unknown variant types
    constexpr Success skipValue()
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
        throw std::out_of_range("BinReader skipValue out of range");
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
    using BinSpanReader<Policy, SizeType>::skipValue;

  private:
    std::vector<std::byte> mData;
  };
} // namespace enki

#endif // ENKI_BIN_READER_HPP
