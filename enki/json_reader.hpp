#ifndef ENKI_JSON_READER_HPP
#define ENKI_JSON_READER_HPP

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

#include "enki/impl/concepts.hpp"
#include "enki/impl/success.hpp"

#if defined(__cpp_lib_stringstream_view) && __cpp_lib_stringstream_view >= 202'202L
#define HAS_STRINGSTREAM_VIEW 1
#else
#define HAS_STRINGSTREAM_VIEW 0
#endif

namespace enki
{
  namespace
  {
    [[maybe_unused]] std::string readWord(std::istream &is)
    {
      std::string word;

      // Skip leading non-alphanumeric characters (if you want that)
      int ch;
      while ((ch = is.peek()) != std::ios_base::eofbit &&
             !std::isalnum(static_cast<unsigned char>(ch)))
      {
        // If you want to *stop* on non-alnum instead of skipping, remove this loop
        is.get();
      }

      // Collect alphanumeric characters
      while ((ch = is.peek()) != std::ios_base::eofbit &&
             std::isalnum(static_cast<unsigned char>(ch)))
      {
        word.push_back(static_cast<char>(is.get()));
      }

      return word;
    }

    // Trim whitespace from both ends of a string
    [[maybe_unused]] std::string_view trim(std::string_view s)
    {
      const size_t start = s.find_first_not_of(" \t\n\r");
      if (start == std::string_view::npos)
      {
        return "";
      }
      const size_t end = s.find_last_not_of(" \t\n\r");
      return s.substr(start, end - start + 1);
    }

    [[maybe_unused]] int32_t countJsonArrayElements(std::string_view rawInput)
    {
      const std::string_view input = trim(rawInput);

      // Basic validation
      if (input.size() < 2 || input.front() != '[')
      {
        throw std::invalid_argument("Not a valid JSON array");
      }

      int depth = 0;
      bool inString = false;
      bool escape = false;
      int countCommas = 0;

      size_t i = 1;

      for (; i < (input.size() - 1) && (depth >= 0); ++i)
      {
        const char c = input[i];

        if (escape)
        {
          escape = false;
          continue;
        }

        if (c == '\\')
        {
          escape = true;
          continue;
        }

        if (c == '"')
        {
          inString = !inString;
          continue;
        }

        if (!inString)
        {
          if (c == '[' || c == '{')
          {
            depth++;
          }
          else if (c == ']' || c == '}')
          {
            depth--;
          }
          else if (c == ',' && depth == 0)
          {
            countCommas++;
          }
        }
      }

      // Check if array is empty
      const auto inside = input.substr(1, i - 2);
      if (inside.find_first_not_of(" \t\n\r") == std::string_view::npos)
      {
        return 0;
      }

      return countCommas + 1;
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
#if HAS_STRINGSTREAM_VIEW
      numElements = countJsonArrayElements(mStream.view().substr(mStream.tellg()));
#else
      numElements = countJsonArrayElements(mStream.str().substr(mStream.tellg()));
#endif

      char junk{};
      mStream >> junk; // Remove '['

      return {};
    }

    constexpr Success rangeEnd()
    {
      char junk{};
      mStream >> junk; // Remove ']'
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
