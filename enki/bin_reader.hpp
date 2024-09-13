#ifndef ENKI_BIN_READER_HPP
#define ENKI_BIN_READER_HPP

#include <cstring>
#include <span>
#include <vector>

#include "enki/impl/concepts.hpp"
#include "enki/impl/success.hpp"

namespace enki
{
  template <typename SizeType = uint32_t>
  class BinReader
  {
  public:
    BinReader(std::span<const std::byte> data) :
      mData(std::begin(data), std::end(data))
    {
    }

    template <concepts::arithmetic_or_enum T>
    constexpr Success<void> read(T &v)
    {
      std::memcpy(&v, mData.data() + mCurrentIndex, sizeof(T));
      mCurrentIndex += sizeof(T);
      return {};
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

    constexpr Success<void> rangeBegin(size_t &numElements)
    {
      SizeType temp{};
      auto ret = read(temp);
      numElements = temp;
      return ret;
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
    size_t mCurrentIndex{};
  };
} // namespace enki

#endif // ENKI_BIN_READER_HPP
