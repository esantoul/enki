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
  namespace
  {
    std::string readWord(std::istream &is)
    {
      std::string word;

      // Skip leading non-alphanumeric characters (if you want that)
      int ch;
      while ((ch = is.peek()) != EOF && !std::isalnum(static_cast<unsigned char>(ch)))
      {
        // If you want to *stop* on non-alnum instead of skipping, remove this loop
        is.get();
      }

      // Collect alphanumeric characters
      while ((ch = is.peek()) != EOF && std::isalnum(static_cast<unsigned char>(ch)))
      {
        word.push_back(static_cast<char>(is.get()));
      }

      return word;
    }
  } // namespace

  template <typename SizeType = uint32_t>
  class JSONReader
  {
  public:
    static constexpr bool serialize_custom_names = true; // NOLINT

    JSONReader(std::string_view sv)
    {
      mStream << sv;
    }

    constexpr Success read(bool &v)
    {
      std::string val = readWord(mStream);

      if (val == "true")
      {
        v = true;
      }
      else if (val == "false")
      {
        v = false;
      }
      else
      {
        return "Invalid boolean value";
      }
      return {};
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
