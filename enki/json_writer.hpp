#ifndef ENKI_JSON_WRITER_HPP
#define ENKI_JSON_WRITER_HPP

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

#include "enki/impl/concepts.hpp"
#include "enki/impl/policies.hpp"
#include "enki/impl/success.hpp"

namespace enki
{
  template <policy Policy = strict_t>
  class JSONWriter
  {
  public:
    using policy_type = Policy;                          // NOLINT
    using size_type = uint32_t;                          // NOLINT
    static constexpr bool serialize_custom_names = true; // NOLINT

    JSONWriter() = default;

    explicit constexpr JSONWriter(Policy)
    {
    }

    constexpr Success write(const bool v)
    {
      mStream << (v ? "true" : "false");
      return {};
    }

    template <concepts::arithmetic_or_enum T>
    constexpr Success write(const T &v)
    {
      if constexpr (std::is_floating_point_v<T>)
      {
        mStream << std::setprecision(std::numeric_limits<T>::max_digits10) << v;
      }
      else if constexpr (sizeof(T) < sizeof(int32_t))
      {
        mStream << static_cast<int32_t>(v);
      }
      else
      {
        if constexpr (std::is_enum_v<T>)
        {
          mStream << static_cast<int64_t>(v);
        }
        else
        {
          mStream << v;
        }
      }
      return {};
    }

    constexpr Success write(const std::string &s)
    {
      mStream << std::quoted(s);
      return {};
    }

    constexpr Success write(const std::monostate &)
    {
      mStream << "null";
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

    constexpr Success rangeBegin(size_t /* numElements */)
    {
      mStream << "[";
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

    constexpr Success objectBegin()
    {
      mStream << "{";
      return {};
    }

    constexpr Success objectEnd()
    {
      mStream << "}";
      return {};
    }

    constexpr Success nextObjectElement()
    {
      mStream << ", ";
      return {};
    }

    constexpr Success objectName(std::string_view name)
    {
      mStream << std::quoted(name) << ": ";
      return {};
    }

    /// Write a variant as {"index": value}
    template <typename IndexFunc, typename ValueFunc>
    constexpr Success writeVariant(IndexFunc &&writeIndex, ValueFunc &&writeValue)
    {
      mStream << "{\"";
      Success indexResult = writeIndex(*this);
      if (!indexResult)
      {
        return indexResult;
      }
      mStream << "\": ";
      Success valueResult = writeValue(*this);
      if (!valueResult)
      {
        return valueResult;
      }
      mStream << "}";
      return {indexResult.size() + valueResult.size()};
    }

    /// Write an optional as null (empty) or value directly (has value)
    template <typename ValueFunc>
    constexpr Success writeOptional(bool hasValue, ValueFunc &&writeValue)
    {
      if (!hasValue)
      {
        mStream << "null";
        return {};
      }
      return writeValue(*this);
    }

    const std::stringstream &data() const
    {
      return mStream;
    }

    /// Write skippable content - JSON is self-describing, just passes through
    template <typename WriteFunc>
    constexpr Success writeSkippable(WriteFunc &&writeContent)
    {
      return writeContent(*this);
    }

  private:
    std::stringstream mStream;
  };

  // Deduction guides for JSONWriter
  JSONWriter() -> JSONWriter<strict_t>;
  JSONWriter(strict_t) -> JSONWriter<strict_t>;
  JSONWriter(forward_compatible_t) -> JSONWriter<forward_compatible_t>;
} // namespace enki

#endif // ENKI_JSON_WRITER_HPP
