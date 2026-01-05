#ifndef ENKI_JSON_READER_HPP
#define ENKI_JSON_READER_HPP

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>

#include "enki/impl/concepts.hpp"
#include "enki/impl/policies.hpp"
#include "enki/impl/success.hpp"

// clang-format off
#if defined(__cpp_lib_stringstream_view) && __cpp_lib_stringstream_view >= 202202L
#define HAS_STRINGSTREAM_VIEW 1
#else
#define HAS_STRINGSTREAM_VIEW 0
#endif
// clang-format on

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

  template <typename Policy = strict_t>
  class JSONReader
  {
  public:
    using policy_type = Policy;                          // NOLINT
    using size_type = uint32_t;                          // NOLINT
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

    constexpr Success read(std::monostate &)
    {
      std::string word = readWord(mStream);
      if (word != "null")
      {
        return "Expected null for monostate";
      }
      return {};
    }

    /// Skip the size hint only - JSON has no size hints, so this is a no-op
    /// Used for forward compatibility when deserializing a known variant index
    Success skipHint()
    {
      return {};
    }

    /// Skip a JSON value - parses and discards current value
    /// Used for forward compatibility when encountering unknown variant types
    Success skipHintAndValue()
    {
      // Skip whitespace
      mStream >> std::ws;

      int ch = mStream.peek();
      if (ch == std::char_traits<char>::eof())
      {
        return "Unexpected end of JSON";
      }

      char c = static_cast<char>(ch);

      if (c == '{' || c == '[')
      {
        // Object or array - need to match braces/brackets
        char openBrace = c;
        char closeBrace = (c == '{') ? '}' : ']';
        mStream.get(); // consume opening brace

        int depth = 1;
        bool inString = false;
        bool escape = false;

        while (depth > 0 && mStream.peek() != std::char_traits<char>::eof())
        {
          char cur = static_cast<char>(mStream.get());

          if (escape)
          {
            escape = false;
            continue;
          }

          if (cur == '\\' && inString)
          {
            escape = true;
            continue;
          }

          if (cur == '"')
          {
            inString = !inString;
            continue;
          }

          if (!inString)
          {
            if (cur == openBrace)
            {
              depth++;
            }
            else if (cur == closeBrace)
            {
              depth--;
            }
          }
        }
      }
      else if (c == '"')
      {
        // String - read until closing quote
        std::string dummy;
        mStream >> std::quoted(dummy);
      }
      else if (c == 't' || c == 'f')
      {
        // Boolean
        readWord(mStream);
      }
      else if (c == 'n')
      {
        // null
        readWord(mStream);
      }
      else if (c == '-' || std::isdigit(static_cast<unsigned char>(c)))
      {
        // Number - read until non-numeric character
        while (mStream.peek() != std::char_traits<char>::eof())
        {
          int next = mStream.peek();
          char nc = static_cast<char>(next);
          if (
            nc == '-' || nc == '+' || nc == '.' || nc == 'e' || nc == 'E' ||
            std::isdigit(static_cast<unsigned char>(nc)))
          {
            mStream.get();
          }
          else
          {
            break;
          }
        }
      }
      else
      {
        return "Invalid JSON value";
      }

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

    /// Read variant index from {"index": value} format
    /// Reads the opening brace, key (index), and colon
    Success readVariantIndex(size_type &index)
    {
      char brace{};
      mStream >> brace;
      if (brace != '{')
      {
        return "Expected '{' at start of variant";
      }

      // Read the key (index as string) - we need to parse it as a number
      std::string indexStr;
      mStream >> std::quoted(indexStr);

      // Parse index from string
      try
      {
        index = static_cast<size_type>(std::stoul(indexStr));
      }
      catch (...)
      {
        return "Invalid variant index in JSON";
      }

      // Read the colon
      char colon{};
      mStream >> colon;
      if (colon != ':')
      {
        return "Expected ':' after variant index";
      }

      return {};
    }

    /// Finish reading a variant - reads the closing brace
    Success finishVariant()
    {
      char brace{};
      mStream >> brace;
      if (brace != '}')
      {
        return "Expected '}' at end of variant";
      }
      return {};
    }

  private:
    std::stringstream mStream;
  };
} // namespace enki

#endif // ENKI_JSON_READER_HPP
