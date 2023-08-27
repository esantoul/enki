#ifndef ENKI_TYPE_INFO_HPP
#define ENKI_TYPE_INFO_HPP

#include <string_view>
#include <cstddef>

namespace enki
{
#if defined(__clang__) || defined(_MSC_VER)
  class Type
  {
    template <typename T>
    static constexpr bool get_idx_impl() { return false; }

  public:
    using index_t = bool (*)();

    template <typename T>
    static constexpr index_t get_index()
    {
      return &get_idx_impl<T>;
    }
  };

  static_assert(Type::get_index<char>() != Type::get_index<unsigned char>(), "Your compiler does not support this library unique type signature feature");

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

  static_assert(Type::get_index<char>() != Type::get_index<unsigned char>(), "Your compiler does not support this library unique type signature feature");

#else
  static_assert(false, "Your compiler does not support this library unique type signature feature");
#endif

  template <typename T>
  constexpr std::string_view full_function_name()
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
  struct type_name_info
  {
    static constexpr std::string_view sentinel = full_function_name<double>();
    static constexpr ptrdiff_t type_name_start = sentinel.find("double");
    static constexpr ptrdiff_t suffix_length = sentinel.size() - type_name_start - 6;
  };

  template <typename T>
  constexpr std::string_view get_type_name()
  {
    constexpr auto sentinel = full_function_name<T>();

    const auto start = type_name_info::type_name_start;
    const auto size = sentinel.size() - start - type_name_info::suffix_length;

    return sentinel.substr(start, size);
  }
}

#endif // ENKI_TYPE_INFO_HPP
