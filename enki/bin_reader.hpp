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
#include "enki/impl/success.hpp"

namespace enki
{
  template <typename SizeType = uint32_t>
  class BinReader
  {
  public:
    using size_type = SizeType; // NOLINT

    BinReader(std::span<const std::byte> data) :
      mData(std::begin(data), std::end(data))
    {
    }

    template <concepts::arithmetic_or_enum T>
    constexpr Success read(T &v)
    {
      if (mCurrentIndex + sizeof(T) > mData.size())
      {
#if __cpp_exceptions >= 199711
        throw std::out_of_range("BinReader out of range read");
#else
        std::abort();
#endif
      }
      std::memcpy(&v, mData.data() + mCurrentIndex, sizeof(T));
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

    const std::vector<std::byte> &data() const
    {
      return mData;
    }

  private:
    std::vector<std::byte> mData;
    size_t mCurrentIndex{};
  };
} // namespace enki

#endif // ENKI_BIN_READER_HPP
