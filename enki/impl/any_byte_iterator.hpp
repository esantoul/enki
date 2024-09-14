#ifndef ANY_BYTE_ITERATOR_HPP
#define ANY_BYTE_ITERATOR_HPP

#include <cstddef>
#include <memory>
#include <optional>

#include "enki/impl/concepts.hpp"

namespace enki
{
  class AnyByteOutputIt;

  namespace detail
  {
    class ByteOutputItInterface
    {
    public:
      constexpr virtual ~ByteOutputItInterface() = default;
      constexpr virtual ByteOutputItInterface &operator*() = 0;
      constexpr virtual ByteOutputItInterface &operator=(std::byte) = 0;
      constexpr virtual ByteOutputItInterface &operator++() = 0;

    protected:
      friend AnyByteOutputIt;
      constexpr virtual ByteOutputItInterface *clone() = 0;
    };

    template <concepts::ByteDataOutputIterator It>
    class SpecializedByteOutputIt : public ByteOutputItInterface
    {
    public:
      constexpr ~SpecializedByteOutputIt() {}; // necessary because of a bug in gcc version <= 12

      constexpr SpecializedByteOutputIt(It it) :
        mIt(it)
      {
      }

      constexpr SpecializedByteOutputIt &operator*() final
      {
        return *this;
      }

      constexpr SpecializedByteOutputIt &operator=(std::byte val) final
      {
        *mIt = static_cast<concepts::detail::iterator_underlying_t<It>>(val);
        return *this;
      }

      constexpr SpecializedByteOutputIt &operator++() final
      {
        ++mIt;
        return *this;
      }

    private:
      constexpr ByteOutputItInterface *clone() final
      {
        return new SpecializedByteOutputIt(mIt);
      }

      It mIt;
    };

    template <concepts::ByteDataOutputIterator It>
    class RefByteOutputIt : public ByteOutputItInterface
    {
    public:
      constexpr ~RefByteOutputIt() {}; // necessary because of a bug in gcc version <= 12

      constexpr RefByteOutputIt(It &it) :
        mPIt(&it)
      {
      }

      constexpr RefByteOutputIt &operator*() final
      {
        return *this;
      }

      constexpr RefByteOutputIt &operator=(std::byte val) final
      {
        **mPIt = static_cast<concepts::detail::iterator_underlying_t<It>>(val);
        return *this;
      }

      constexpr RefByteOutputIt &operator++() final
      {
        ++*mPIt;
        return *this;
      }

    private:
      constexpr ByteOutputItInterface *clone() final
      {
        return new RefByteOutputIt(*mPIt);
      }

      It *mPIt;
    };
  } // namespace detail

  template <typename It>
  concept ByteDataOutputIteratorExceptSelf =
    (requires { typename It::AnyByteOutputIt_tag; } == false) &&
    concepts::ByteDataOutputIterator<It>;

  class AnyByteOutputIt
  {
  public:
    using AnyByteOutputIt_tag = void;  // NOLINT
    using difference_type = ptrdiff_t; // NOLINT
    using enki_value_type = std::byte; // NOLINT

    template <ByteDataOutputIteratorExceptSelf It>
    static constexpr AnyByteOutputIt copy(It it)
    {
      AnyByteOutputIt ret;
      ret.mPIt = new detail::SpecializedByteOutputIt<It>(std::move(it));
      return ret;
    }

    template <ByteDataOutputIteratorExceptSelf It>
    static constexpr AnyByteOutputIt ref(It &it)
    {
      AnyByteOutputIt ret;
      ret.mPIt = new detail::RefByteOutputIt<It>(it);
      return ret;
    }

    constexpr AnyByteOutputIt(const AnyByteOutputIt &other) :
      mPIt(other.mPIt->clone())
    {
    }

    constexpr AnyByteOutputIt &operator=(const AnyByteOutputIt &other)
    {
      auto pReviousIt = mPIt;
      mPIt = other.mPIt->clone();
      if (pReviousIt)
      {
        delete pReviousIt;
      }
      return *this;
    }

    constexpr ~AnyByteOutputIt()
    {
      if (mPIt)
      {
        delete mPIt;
      }
    }

    constexpr AnyByteOutputIt &operator*()
    {
      return *this;
    };

    constexpr void operator=(std::byte val)
    {
      (*mPIt) = val;
    };

    constexpr AnyByteOutputIt &operator++()
    {
      ++*mPIt;
      return *this;
    };

    constexpr AnyByteOutputIt operator++(int)
    {
      AnyByteOutputIt cp(*this);
      ++*this;
      return cp;
    }

  private:
    constexpr AnyByteOutputIt() = default;

    detail::ByteOutputItInterface *mPIt = nullptr;
  };

  class AnyByteInputIt;

  namespace detail
  {
    class ByteInputItInterface
    {
    public:
      constexpr virtual ~ByteInputItInterface() = default;
      constexpr virtual const std::byte &operator*() const = 0;
      constexpr virtual ByteInputItInterface &operator++() = 0;

    protected:
      friend AnyByteInputIt;
      constexpr virtual ByteInputItInterface *clone() = 0;
      constexpr virtual const void *address() const = 0;
    };

    template <concepts::ByteDataInputIterator It>
    class SpecializedByteInputIt : public ByteInputItInterface
    {
    public:
      constexpr ~SpecializedByteInputIt() {}; // necessary because of a bug in gcc version <= 12

      constexpr SpecializedByteInputIt(It it) :
        mIt(it)
      {
      }

      constexpr const std::byte &operator*() const final
      {
        if (!mCurrentValue)
        {
          mCurrentValue = static_cast<std::byte>(*mIt);
        }
        return mCurrentValue.value();
      }

      constexpr SpecializedByteInputIt &operator++() final
      {
        ++mIt;
        mCurrentValue.reset();
        return *this;
      }

    private:
      constexpr SpecializedByteInputIt *clone() final
      {
        return new SpecializedByteInputIt(mIt);
      }

      constexpr const void *address() const final
      {
        return &*mIt;
      }

      It mIt;
      mutable std::optional<std::byte> mCurrentValue{};
    };
  } // namespace detail

  template <typename It>
  concept ByteDataInputIteratorExceptSelf =
    (requires { typename It::AnyByteInputIt_tag; } == false) && concepts::ByteDataInputIterator<It>;

  class AnyByteInputIt
  {
  public:
    using AnyByteInputIt_tag = void;                   // NOLINT
    using value_type = std::byte;                      // NOLINT
    using difference_type = ptrdiff_t;                 // NOLINT
    using iterator_category = std::input_iterator_tag; // NOLINT

    template <ByteDataInputIteratorExceptSelf It>
    constexpr AnyByteInputIt(It it) :
      mPIt(new detail::SpecializedByteInputIt<It>(it))
    {
    }

    constexpr AnyByteInputIt(const AnyByteInputIt &other) :
      mPIt(other.mPIt->clone())
    {
    }

    constexpr AnyByteInputIt &operator=(const AnyByteInputIt &other)
    {
      auto pPreviousIt = mPIt;
      mPIt = other.mPIt->clone();
      if (pPreviousIt)
      {
        delete pPreviousIt;
      }
      return *this;
    }

    constexpr ~AnyByteInputIt()
    {
      if (mPIt)
      {
        delete mPIt;
      }
    }

    constexpr const std::byte &operator*() const
    {
      return **mPIt;
    };

    constexpr AnyByteInputIt &operator++()
    {
      ++*mPIt;
      return *this;
    };

    constexpr AnyByteInputIt operator++(int)
    {
      AnyByteInputIt cp(*this);
      ++*this;
      return cp;
    }

    constexpr bool operator==(const AnyByteInputIt &other) const
    {
      return mPIt->address() == other.mPIt->address();
    }

  private:
    detail::ByteInputItInterface *mPIt = nullptr;
  };
} // namespace enki

#endif // ANY_BYTE_ITERATOR_HPP
