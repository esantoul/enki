#ifndef BASE_ENGINE_HPP
#define BASE_ENGINE_HPP

#include "enki/impl/serdes_engine.hpp"

namespace enki
{
  class BaseEngine : public details::SerDesEngine<void>
  {
  private:
    using ParentType = details::SerDesEngine<void>;

  public:
    template <concepts::BasicSerializable T, concepts::ByteDataOutputIterator It>
    static constexpr Success<It> serialize(const T &val, It out)
    {
      return BaseEngine{}.serializeImpl(val, out);
    }

    template <concepts::BasicSerializable T, concepts::ByteDataInputIterator It>
    static constexpr Success<It> deserialize(T &val, It in)
    {
      return BaseEngine{}.deserializeImpl(val, in);
    }

    template <concepts::BasicSerializable T>
    static constexpr Success<void> NumBytes(const T &val)
    {
      return BaseEngine{}.numBytesImpl(val);
    }

  private:
    constexpr BaseEngine() = default;
    constexpr BaseEngine(const BaseEngine &) = delete;
    constexpr BaseEngine(BaseEngine &&) = delete;
    constexpr BaseEngine &operator=(const BaseEngine &) = delete;
    constexpr BaseEngine &operator=(BaseEngine &&) = delete;

    template <concepts::BasicSerializable T, concepts::ByteDataOutputIterator It>
    constexpr Success<It> serializeImpl(const T &val, It out) const
    {
      return ParentType::serialize(val, out);
    }

    template <concepts::BasicSerializable T, concepts::ByteDataInputIterator It>
    constexpr Success<It> deserializeImpl(T &val, It in) const
    {
      return ParentType::deserialize(val, in);
    }

    template <concepts::BasicSerializable T>
    constexpr Success<void> numBytesImpl(const T &val) const
    {
      return ParentType::numBytes(val);
    }
  };
} // namespace enki

#endif // BASE_ENGINE_HPP
