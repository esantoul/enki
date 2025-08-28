#ifndef ENKI_CSTR_HPP
#define ENKI_CSTR_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

namespace enki
{
  struct CStr
  {
    static constexpr size_t cty = 128;

    template <size_t N>
    constexpr CStr(const char (&s)[N]) :
      mSize(N < cty ? N : cty)
    {
      std::copy_n(s, std::min<size_t>(N, 127), mStr);
    }

    constexpr CStr(std::string_view s) :
      mSize(s.size() < cty - 1 ? s.size() + 1 : cty)
    {
      std::copy_n(s.begin(), std::min<size_t>(s.size(), 127), mStr);
    }

    constexpr size_t capacity() const
    {
      return cty;
    }

    constexpr size_t size() const
    {
      return mSize;
    }

    constexpr const char *begin() const
    {
      return mStr;
    }

    constexpr const char *end() const
    {
      return mStr + size();
    }

    constexpr operator std::string_view() const
    {
      return {mStr, mStr + mSize};
    }

    size_t mSize;
    char mStr[cty]{};
  };

  template <CStr... strs>
  constexpr std::array<char, (strs.size() + ...) - sizeof...(strs) + 1> cStrConcat()
  {
    std::array<char, (strs.size() + ...) - sizeof...(strs) + 1> str{};
    size_t offset = 0;
    ((std::copy_n(strs.begin(), strs.size() - 1, str.begin() + offset), offset += strs.size() - 1),
     ...);
    return str;
  }
} // namespace enki

#endif // ENKI_CSTR_HPP
