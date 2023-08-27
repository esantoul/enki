#ifndef ENKI_RESULT_HPP
#define ENKI_RESULT_HPP

#include <cstddef>

#if __cpp_exceptions >= 199711
#include <stdexcept>
#endif

namespace enki
{
  class Result
  {
  public:
    constexpr Result() noexcept = default;
    constexpr Result(const char *error_description) noexcept : error(error_description) {}
    constexpr Result(size_t size) noexcept : error(nullptr), num_bytes(size) {}

#if __cpp_exceptions >= 199711
    constexpr const Result &or_throw() const
    {
      if (error) [[unlikely]]
        throw std::invalid_argument(error);
        return *this;
    }

    constexpr Result &or_throw()
    {
      if (error) [[unlikely]]
        throw std::invalid_argument(error);
        return *this;
    }
#endif

    constexpr explicit operator bool() const noexcept { return error == nullptr; }
    constexpr size_t size() const noexcept { return num_bytes; }

    constexpr Result &update(const Result &other) noexcept
    {
      error = other.error;
      num_bytes += other.num_bytes;
      return *this;
    }

  private:
    const char *error = nullptr;
    size_t num_bytes = 0;
  };
}

#endif // ENKI_RESULT_HPP
