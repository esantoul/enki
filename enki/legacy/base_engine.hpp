#ifndef BASE_ENGINE_HPP
#define BASE_ENGINE_HPP

#include "enki/legacy/impl/serdes_engine.hpp"

namespace enki
{
  class BaseEngine : public details::SerDesEngine<void>
  {
  private:
    using Parent_t = details::SerDesEngine<void>;

  public:
    template <concepts::BasicSerializable T, concepts::ByteDataOutputIterator It>
    static constexpr Result<It> Serialize(const T &val, It out)
    {
      return BaseEngine{}.Serialize_impl(val, out);
    }

    template <concepts::BasicSerializable T, concepts::ByteDataInputIterator It>
    static constexpr Result<It> Deserialize(T &val, It in)
    {
      return BaseEngine{}.Deserialize_impl(val, in);
    }

    template <concepts::BasicSerializable T>
    static constexpr Result<void> NumBytes(const T &val)
    {
      return BaseEngine{}.NumBytes_impl(val);
    }

  private:
    constexpr BaseEngine() = default;
    constexpr BaseEngine(const BaseEngine &) = delete;
    constexpr BaseEngine(BaseEngine &&) = delete;
    constexpr BaseEngine &operator=(const BaseEngine &) = delete;
    constexpr BaseEngine &operator=(BaseEngine &&) = delete;

    template <concepts::BasicSerializable T, concepts::ByteDataOutputIterator It>
    constexpr Result<It> Serialize_impl(const T &val, It out) const
    {
      return Parent_t::Serialize(val, out);
    }

    template <concepts::BasicSerializable T, concepts::ByteDataInputIterator It>
    constexpr Result<It> Deserialize_impl(T &val, It in) const
    {
      return Parent_t::Deserialize(val, in);
    }

    template <concepts::BasicSerializable T>
    constexpr Result<void> NumBytes_impl(const T &val) const
    {
      return Parent_t::NumBytes(val);
    }
  };
}

#endif // BASE_ENGINE_HPP
