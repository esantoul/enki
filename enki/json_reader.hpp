#ifndef ENKI_JSON_READER_HPP
#define ENKI_JSON_READER_HPP

#include <cstdint>
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
    static constexpr bool serialize_custom_names = true; // NOLINT

    JSONReader(std::string_view sv)
    {
      mStream << sv;
    }

    template <concepts::arithmetic_or_enum T>
    constexpr Success read(T &v)
    {
      if constexpr (std::is_floating_point_v<T>)
      {
        T val{};
        mStream >> val;
        v = static_cast<T>(val);
      }
      else
      {
        int64_t val{};
        mStream >> val;
        v = static_cast<T>(val);
      }
      return {};
    }

    constexpr Success read(std::string &str)
    {
      mStream >> std::quoted(str);
      return {};
    }

    constexpr Success arrayBegin()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

    constexpr Success arrayEnd()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

    constexpr Success nextArrayElement()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

    constexpr Success rangeBegin(size_t &numElements)
    {
      char junk{};
      mStream >> junk >> numElements;
      if (numElements > 0)
      {
        mStream >> junk;
      }
      return {};
    }

    constexpr Success rangeEnd()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

    constexpr Success nextRangeElement()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

    constexpr Success objectBegin()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

    constexpr Success objectEnd()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

    constexpr Success nextObjectElement()
    {
      char junk{};
      mStream >> junk;
      return {};
    }

    constexpr Success objectName(std::string_view name)
    {
      std::string readName;
      mStream >> std::quoted(readName);

      if (readName != name)
      {
        return "Encountered invalid variable name";
      }

      char junk{};

      mStream >> junk;

      return {};
    }

  private:
    std::stringstream mStream;
  };
} // namespace enki

#endif // ENKI_JSON_READER_HPP
