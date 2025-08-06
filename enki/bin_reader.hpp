#ifndef ENKI_BIN_READER_HPP
#define ENKI_BIN_READER_HPP

#include <cstdint>
#include <cstring>
#include <span>
#include <vector>

#if __cpp_exceptions >= 199'711
#include <stdexcept>
#else
#include <cstdlib>
#endif

#include "enki/impl/concepts.hpp"
#include "enki/impl/success.hpp"

namespace enki
{
  template <typename SizeType = uint32_t>
  class BinSpanReader
  {
  public:
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
#if __cpp_exceptions >= 199'711

        throw std::out_of_range("BinReader out of range read");
#else
        std::abort();
#endif
      }
      std::memcpy(&v, mSpan.data() + mCurrentIndex, sizeof(T));
      mCurrentIndex += sizeof(T);
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

  template <typename SizeType = uint32_t>
  class BinReader : public BinSpanReader<SizeType>
  {
  public:
    using size_type = typename BinSpanReader<SizeType>::size_type; // NOLINT

    BinReader(std::span<const std::byte> data) :
      BinSpanReader<>({}),
      mData(std::begin(data), std::end(data))
    {
      static_cast<BinSpanReader<> &>(*this) = {mData};
    }

    using BinSpanReader<>::read;

  private:
    std::vector<std::byte> mData;
  };
} // namespace enki

#endif // ENKI_BIN_READER_HPP
