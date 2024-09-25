#ifndef ENKI_IMPL_SUCCESS_HPP
#define ENKI_IMPL_SUCCESS_HPP

#include <cstddef>

#if __cpp_exceptions >= 199711
#include <stdexcept>
#endif

namespace enki
{
  class Success
  {
  public:
    constexpr Success() noexcept = default;

    constexpr Success(const char *errorDescription) noexcept :
      mError(errorDescription)
    {
    }

    constexpr Success(size_t size) noexcept :
      mNumBytes(size)
    {
    }

#if __cpp_exceptions >= 199711
    constexpr const Success &or_throw() const // NOLINT
    {
      if (mError) [[unlikely]]
      {
        throw std::invalid_argument(mError);
      }
      return *this;
    }

    constexpr Success &or_throw() // NOLINT
    {
      if (mError) [[unlikely]]
      {
        throw std::invalid_argument(mError);
      }
      return *this;
    }
#endif

    constexpr explicit operator bool() const noexcept
    {
      return mError == nullptr;
    }

    constexpr size_t size() const noexcept
    {
      return mNumBytes;
    }

    constexpr const char *error() const noexcept
    {
      return mError;
    }

    constexpr Success &update(const Success &other) noexcept
    {
      mError = other.mError;
      mNumBytes += other.mNumBytes;
      return *this;
    }

  protected:
    const char *mError = nullptr;
    size_t mNumBytes = 0;
  };
} // namespace enki

#endif // ENKI_IMPL_SUCCESS_HPP
