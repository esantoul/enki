#ifndef ENKI_JSON_WRITER_HPP
#define ENKI_JSON_WRITER_HPP

#include <iomanip>
#include <sstream>
#include <string>

#include "enki/impl/concepts.hpp"
#include "enki/impl/success.hpp"

namespace enki
{
  template <typename SizeType = uint32_t>
  class JSONWriter
  {
  public:
    template <concepts::arithmetic_or_enum T>
    constexpr Success write(const T &v)
    {
      if constexpr (sizeof(T) == 1)
      {
        mStream << static_cast<int32_t>(v);
      }
      else
      {
        mStream << static_cast<std::underlying_type_t<T>>(v);
      }
      return {};
    }

    constexpr Success write(const std::string &s)
    {
      mStream << std::quoted(s);
      return {};
    }

    constexpr Success arrayBegin()
    {
      mStream << "[";
      return {};
    }

    constexpr Success arrayEnd()
    {
      mStream << "]";
      return {};
    }

    constexpr Success nextArrayElement()
    {
      mStream << ", ";
      return {};
    }

    constexpr Success rangeBegin(size_t numElements)
    {
      mStream << "[" << numElements;
      if (numElements > 0)
      {
        mStream << ", ";
      }
      return {};
    }

    constexpr Success rangeEnd()
    {
      mStream << "]";
      return {};
    }

    constexpr Success nextRangeElement()
    {
      mStream << ", ";
      return {};
    }

    const std::stringstream &data() const
    {
      return mStream;
    }

  private:
    std::stringstream mStream;
  };
} // namespace enki

#endif // ENKI_JSON_WRITER_HPP
