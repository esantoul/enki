#ifndef ENKI_JSON_READER_HPP
#define ENKI_JSON_READER_HPP

#include <iomanip>
#include <sstream>
#include <string>

#include "enki/impl/concepts.hpp"
#include "enki/impl/success.hpp"

namespace enki
{
  template <typename SizeType = uint32_t>
  class JSONReader
  {
  public:
    JSONReader(std::string_view sv)
    {
      mStream << sv;
    }

    template <concepts::arithmetic_or_enum T>
    constexpr Success<void> read(T &v)
    {
      if constexpr (sizeof(T) == 1)
      {
        int32_t val{};
        mStream >> val;
        v = static_cast<T>(val);
      }
      else
      {
        mStream >> v;
      }
      return {};
    }

    constexpr Success<void> read(std::string &str)
    {
      mStream >> std::quoted(str);
      return {};
    }

    constexpr Success<void> arrayBegin()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

    constexpr Success<void> arrayEnd()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

    constexpr Success<void> nextArrayElement()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

    constexpr Success<void> rangeBegin(size_t &numElements)
    {
      char junk{};
      mStream >> junk >> numElements;
      if (numElements > 0)
      {
        mStream >> junk;
      }
      return {};
    }

    constexpr Success<void> rangeEnd()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

    constexpr Success<void> nextRangeElement()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

  private:
    std::stringstream mStream;
  };
} // namespace enki

#endif // ENKI_JSON_READER_HPP
