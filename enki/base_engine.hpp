#ifndef BASE_ENGINE_HPP
#define BASE_ENGINE_HPP

#include "enki/impl/serdes_engine.hpp"

namespace enki
{
  class BaseEngine : public details::SerDesEngine<void>
  {
  private:
    using Parent_t = details::SerDesEngine<void>;

  public:
    template <concepts::BasicSerializable T, concepts::ByteDataOutputIterator It>
    static constexpr std::pair<Result, It> Serialize(const T &val, It out)
    {
      return BaseEngine{}.Serialize_impl(val, out);
    }

    template <concepts::BasicSerializable T, concepts::ByteDataInputIterator It>
    static constexpr std::pair<Result, It> Deserialize(T &val, It in)
    {
      return BaseEngine{}.Deserialize_impl(val, in);
    }

    template <concepts::BasicSerializable T>
    static constexpr Result NumBytes(const T &val)
    {
      return BaseEngine{}.NumBytes_impl(val);
    }

  private:
    template <concepts::BasicSerializable T, concepts::ByteDataOutputIterator It>
    constexpr std::pair<Result, It> Serialize_impl(const T &val, It out) const
    {
      return Parent_t::Serialize(val, out);
    }

    template <concepts::BasicSerializable T, concepts::ByteDataInputIterator It>
    constexpr std::pair<Result, It> Deserialize_impl(T &val, It in) const
    {
      return Parent_t::Deserialize(val, in);
    }

    template <concepts::BasicSerializable T>
    constexpr Result NumBytes_impl(const T &val) const
    {
      return Parent_t::NumBytes(val);
    }
  };
}

#endif // BASE_ENGINE_HPP
