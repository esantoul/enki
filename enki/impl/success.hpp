#ifndef ENKI_IMPL_SUCCESS_HPP
#define ENKI_IMPL_SUCCESS_HPP

#include <cstddef>

#if __cpp_exceptions >= 199711
#include <stdexcept>
#endif

#include "enki/impl/concepts.hpp"

namespace enki
{
  namespace details
  {
    template <typename Derived>
    class BaseSuccess
    {
    public:
      constexpr BaseSuccess() noexcept = default;
      constexpr BaseSuccess(const char *error_description) noexcept : mError(error_description) {}
      constexpr BaseSuccess(size_t size) noexcept : mNumBytes(size) {}

#if __cpp_exceptions >= 199711
      constexpr const Derived &or_throw() const
      {
        if (mError) [[unlikely]]
          throw std::invalid_argument(mError);
          return static_cast<const Derived &>(*this);
      }

      constexpr Derived &or_throw()
      {
        if (mError) [[unlikely]]
          throw std::invalid_argument(mError);
          return static_cast<Derived &>(*this);
      }
#endif

      constexpr explicit operator bool() const noexcept { return mError == nullptr; }
      constexpr size_t size() const noexcept { return mNumBytes; }
      constexpr const char *error() const noexcept { return mError; }

      constexpr Derived &update(const BaseSuccess &other) noexcept
      {
        mError = other.mError;
        mNumBytes += other.mNumBytes;
        return static_cast<Derived &>(*this);
      }

    protected:
      const char *mError = nullptr;
      size_t mNumBytes = 0;
    };
  }

  template<typename It>
    requires (concepts::ByteDataIterator<It> || std::same_as<It, void>)
  class Success;

  template <>
  class Success<void> : public details::BaseSuccess<Success<void>>
  {
  private:
    using Base_t = details::BaseSuccess<Success<void>>;
  public:
    using Base_t::Base_t;
  };

  template <concepts::ByteDataIterator It>
  class Success<It> : public details::BaseSuccess<Success<It>>
  {
  private:
    using Base_t = details::BaseSuccess<Success<It>>;

  public:
    constexpr Success() noexcept = default;
    constexpr Success(const char *error_description, It iterator) noexcept : Base_t(error_description), mIt(iterator) {}
    constexpr Success(size_t size, It iterator) noexcept : Base_t(size), mIt(iterator) {}

    constexpr Success &update(const Success &other) noexcept
    {
      mIt = other.mIt;
      return Base_t::update(other);
    }

    constexpr const It &get_iterator() const
    {
      return mIt;
    }

  private:
    using Base_t::update;

    It mIt{};
  };
}

#endif // ENKI_IMPL_SUCCESS_HPP
