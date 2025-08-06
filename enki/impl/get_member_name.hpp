#ifndef ENKI_GET_MEMBER_NAME_HPP
#define ENKI_GET_MEMBER_NAME_HPP

// Inspired by https://github.com/Neargye/nameof

#include <cstddef>
#include <string_view>

namespace enki
{
  namespace detail
  {
    constexpr std::string_view prettyName(std::string_view name) noexcept
    {
      if (name.size() >= 1 && (name[0] == '"' || name[0] == '\''))
      {
        return {}; // Narrow multibyte string literal.
      }
      else if (name.size() >= 2 && name[0] == 'R' && (name[1] == '"' || name[1] == '\''))
      {
        return {}; // Raw string literal.
      }
      else if (name.size() >= 2 && name[0] == 'L' && (name[1] == '"' || name[1] == '\''))
      {
        return {}; // Wide string literal.
      }
      else if (name.size() >= 2 && name[0] == 'U' && (name[1] == '"' || name[1] == '\''))
      {
        return {}; // UTF-32 encoded string literal.
      }
      else if (name.size() >= 2 && name[0] == 'u' && (name[1] == '"' || name[1] == '\''))
      {
        return {}; // UTF-16 encoded string literal.
      }
      else if (
        name.size() >= 3 && name[0] == 'u' && name[1] == '8' && (name[2] == '"' || name[2] == '\''))
      {
        return {}; // UTF-8 encoded string literal.
      }
      else if (name.size() >= 1 && (name[0] >= '0' && name[0] <= '9'))
      {
        return {}; // Invalid name.
      }

      for (std::size_t i = name.size(), h = 0, s = 0; i > 0; --i)
      {
        if (name[i - 1] == ')')
        {
          ++h;
          ++s;
          continue;
        }
        else if (name[i - 1] == '(')
        {
          --h;
          ++s;
          continue;
        }

        if (h == 0)
        {
          name.remove_suffix(s);
          break;
        }
        else
        {
          ++s;
          continue;
        }
      }

      std::size_t s = 0;
      for (std::size_t i = name.size(), h = 0; i > 0; --i)
      {
        if (name[i - 1] == '>')
        {
          ++h;
          ++s;
          continue;
        }
        else if (name[i - 1] == '<')
        {
          --h;
          ++s;
          continue;
        }

        if (h == 0)
        {
          break;
        }
        else
        {
          ++s;
          continue;
        }
      }

      for (std::size_t i = name.size() - s; i > 0; --i)
      {
        if (!((name[i - 1] >= '0' && name[i - 1] <= '9') ||
              (name[i - 1] >= 'a' && name[i - 1] <= 'z') ||
              (name[i - 1] >= 'A' && name[i - 1] <= 'Z') || (name[i - 1] == '_')))
        {
          name.remove_prefix(i);
          break;
        }
      }

      name.remove_suffix(s);

      if (
        name.size() > 0 && ((name[0] >= 'a' && name[0] <= 'z') ||
                            (name[0] >= 'A' && name[0] <= 'Z') || (name[0] == '_')))
      {
        return name;
      }

      return {}; // Invalid name.
    }
  } // namespace detail

  template <auto v, auto u = v>
  constexpr auto getMemberName() noexcept
  {
#if defined(__clang__) || defined(__GNUC__)
    return detail::prettyName({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER) && defined(_MSVC_LANG) && _MSVC_LANG >= 202'002L
    return detail::prettyName(
      {__FUNCSIG__, sizeof(__FUNCSIG__) - 18 + std::is_member_function_pointer_v<decltype(u)>});
#else
#error `getMemberName` is not supported on this platform
    return std::string_view{};
#endif
  }
} // namespace enki

#endif // ENKI_GET_MEMBER_NAME_HPP
