#ifndef ANY_BYTE_ITERATOR_HPP
#define ANY_BYTE_ITERATOR_HPP

#include <cstddef>
#include <memory>

#include "enki/impl/concepts.hpp"

namespace enki
{
  class AnyByteOutputIt;

  namespace details
  {
    class BaseByteOutputIt
    {
    public:
      constexpr virtual ~BaseByteOutputIt() = default;
      constexpr virtual BaseByteOutputIt &operator*() = 0;
      constexpr virtual BaseByteOutputIt &operator=(std::byte) = 0;
      constexpr virtual BaseByteOutputIt &operator++() = 0;

    protected:
      friend AnyByteOutputIt;
      constexpr virtual BaseByteOutputIt *clone() = 0;
    };

    template <concepts::ByteDataOutputIterator It>
    class SpecializedByteOutputIt : public BaseByteOutputIt
    {
    public:
      constexpr ~SpecializedByteOutputIt() {};  // necessary because of a bug in gcc version <= 12
      constexpr SpecializedByteOutputIt(It it)
        :
        mIt(it)
      {
      }

      constexpr SpecializedByteOutputIt &operator*() final { return *this; }
      constexpr SpecializedByteOutputIt &operator=(std::byte val) final { *mIt = static_cast<concepts::details::iterator_underlying_t<It>>(val); return *this; }
      constexpr SpecializedByteOutputIt &operator++() final { ++mIt; return *this; }

    private:
      constexpr BaseByteOutputIt *clone() final { return new SpecializedByteOutputIt(mIt); }
      It mIt;
    };

    template <concepts::ByteDataOutputIterator It>
    class RefByteOutputIt : public BaseByteOutputIt
    {
    public:
      constexpr ~RefByteOutputIt() {};  // necessary because of a bug in gcc version <= 12
      constexpr RefByteOutputIt(It &it)
        :
        pIt(&it)
      {
      }

      constexpr RefByteOutputIt &operator*() final { return *this; }
      constexpr RefByteOutputIt &operator=(std::byte val) final { **pIt = static_cast<concepts::details::iterator_underlying_t<It>>(val); return *this; }
      constexpr RefByteOutputIt &operator++() final { ++*pIt; return *this; }

    private:
      constexpr BaseByteOutputIt *clone() final { return new RefByteOutputIt(*pIt); }
      It *pIt;
    };
  }

  template <typename It>
  concept ByteDataOutputIteratorExceptSelf = (requires { typename It::AnyByteOutputIt_tag; } == false) && concepts::ByteDataOutputIterator<It>;

  class AnyByteOutputIt
  {
  public:
    using AnyByteOutputIt_tag = void;
    using difference_type = ptrdiff_t;
    using enki_value_type = std::byte;

    template <ByteDataOutputIteratorExceptSelf It>
    static constexpr AnyByteOutputIt Copy(It it)
    {
      AnyByteOutputIt ret;
      ret.pIt = new details::SpecializedByteOutputIt<It>(std::move(it));
      return ret;
    }

    template <ByteDataOutputIteratorExceptSelf It>
    static constexpr AnyByteOutputIt Ref(It &it)
    {
      AnyByteOutputIt ret;
      ret.pIt = new details::RefByteOutputIt<It>(it);
      return ret;
    }

    constexpr AnyByteOutputIt(const AnyByteOutputIt &other)
      :
      pIt(other.pIt->clone())
    {
    }

    constexpr AnyByteOutputIt &operator=(const AnyByteOutputIt &other)
    {
      auto previousItPtr = pIt;
      pIt = other.pIt->clone();
      if (previousItPtr) delete previousItPtr;
      return *this;
    }

    constexpr ~AnyByteOutputIt() { if (pIt) delete pIt; }
    constexpr AnyByteOutputIt &operator*() { return *this; };
    constexpr void operator=(std::byte val) { (*pIt) = val; };
    constexpr AnyByteOutputIt &operator++() { ++*pIt; return *this; };
    constexpr AnyByteOutputIt operator++(int) { AnyByteOutputIt cp(*this); ++*this; return cp; }

  private:
    constexpr AnyByteOutputIt() = default;

    details::BaseByteOutputIt *pIt = nullptr;
  };

  class AnyByteInputIt;

  namespace details
  {
    class BaseByteInputIt
    {
    public:
      constexpr virtual ~BaseByteInputIt() = default;
      constexpr virtual const std::byte &operator*() const = 0;
      constexpr virtual BaseByteInputIt &operator++() = 0;

    protected:
      friend AnyByteInputIt;
      constexpr virtual BaseByteInputIt *clone() = 0;
      constexpr virtual const void *address() const = 0;
    };

    template <concepts::ByteDataInputIterator It>
    class SpecializedByteInputIt : public BaseByteInputIt
    {
    public:
      constexpr ~SpecializedByteInputIt() {};  // necessary because of a bug in gcc version <= 12
      constexpr SpecializedByteInputIt(It it)
        :
        mIt(it),
        mCurrentValue(static_cast<std::byte>(*mIt))
      {
      }
      constexpr const std::byte &operator*() const final { return mCurrentValue; }
      constexpr SpecializedByteInputIt &operator++() final { ++mIt; mCurrentValue = static_cast<std::byte>(*mIt); return *this; }

    private:
      constexpr SpecializedByteInputIt *clone() final { return new SpecializedByteInputIt(mIt); }
      constexpr const void *address() const { return &*mIt; }

      It mIt;
      std::byte mCurrentValue{};
    };
  }

  template <typename It>
  concept ByteDataInputIteratorExceptSelf = (requires { typename It::AnyByteInputIt_tag; } == false) && concepts::ByteDataInputIterator<It>;

  class AnyByteInputIt
  {
  public:
    using AnyByteInputIt_tag = void;
    using value_type = std::byte;
    using difference_type = ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    template <ByteDataInputIteratorExceptSelf It>
    constexpr AnyByteInputIt(It it)
      :
      pIt(new details::SpecializedByteInputIt<It>(it))
    {
    }

    constexpr AnyByteInputIt(const AnyByteInputIt &other)
      :
      pIt(other.pIt->clone())
    {
    }

    constexpr AnyByteInputIt &operator=(const AnyByteInputIt &other)
    {
      auto previousItPtr = pIt;
      pIt = other.pIt->clone();
      if (previousItPtr) delete previousItPtr;
      return *this;
    }

    constexpr ~AnyByteInputIt() { if (pIt) delete pIt; }
    constexpr const std::byte &operator*() const { return **pIt; };
    constexpr AnyByteInputIt &operator++() { ++*pIt; return *this; };
    constexpr AnyByteInputIt operator++(int) { AnyByteInputIt cp(*this); ++*this; return cp; }
    constexpr bool operator==(const AnyByteInputIt &other) const { return pIt->address() == other.pIt->address(); }

  private:
    details::BaseByteInputIt *pIt = nullptr;
  };
}

#endif // ANY_BYTE_ITERATOR_HPP
