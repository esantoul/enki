#ifndef ENKI_TYPE_INFO_HPP
#define ENKI_TYPE_INFO_HPP

#include <cstddef>
#include <string_view>

namespace enki
{
#if defined(__clang__) || defined(_MSC_VER)
  class Type
  {
    template <typename T>
    static constexpr bool getIdxImpl()
    {
      return false;
    }

  public:
    using Index = bool (*)();

    template <typename T>
    static constexpr Index getIndex()
    {
      return &getIdxImpl<T>;
    }
  };

  static_assert(
    Type::getIndex<char>() != Type::getIndex<unsigned char>(),
    "Your compiler does not support this library unique type signature feature");

#elif defined(__GNUC__) || defined(__GNUG__)

  class Type
  {
  public:
    using index_t = const char *;

    template <typename T>
    static constexpr index_t get_index()
    {
      return __PRETTY_FUNCTION__;
    }
  };

  static_assert(
    Type::get_index<char>() != Type::get_index<unsigned char>(),
    "Your compiler does not support this library unique type signature feature");

#else
  static_assert(false, "Your compiler does not support this library unique type signature feature");
#endif

  template <typename T>
  constexpr std::string_view getFullFunctionName()
  {
#if defined(__clang__) || defined(__GNUC__)
    return __PRETTY_FUNCTION__;
#elif defined(__MSC_VER)
    return __FUNCSIG__;
#else
#error Unsupported compiler
#endif
  }

  // Outside of the template so its computed once
  struct TypeNameInfo
  {
    static constexpr std::string_view kSentinel = getFullFunctionName<double>();
    static constexpr ptrdiff_t kTypeNameStart = kSentinel.find("double");
    static constexpr ptrdiff_t kSuffixLength = kSentinel.size() - kTypeNameStart - 6;
  };

  template <typename T>
  constexpr std::string_view getTypeName()
  {
    constexpr auto kSentinel = getFullFunctionName<T>();

    const auto start = TypeNameInfo::kTypeNameStart;
    const auto size = kSentinel.size() - start - TypeNameInfo::kSuffixLength;

    return kSentinel.substr(start, size);
  }
} // namespace enki

#endif // ENKI_TYPE_INFO_HPP
