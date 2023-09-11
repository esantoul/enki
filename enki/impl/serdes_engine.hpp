#ifndef SERDES_ENGINE_HPP
#define SERDES_ENGINE_HPP


#include <algorithm>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

#include "enki/impl/concepts.hpp"
#include "enki/impl/result.hpp"
#include "enki/impl/utilities.hpp"

namespace enki
{
  namespace details
  {
    /**
     * @brief The CRTP Base class for custom SerDes
     * @tparam Child_t must be either void or the child class that inherits from SerDesEngine and implements public methods Serialize, Deserialize & NumBytes
    */
    template <typename Child_t>
    class SerDesEngine
    {
    public:
      using size_type = uint32_t;

    protected:
      using This_t = std::conditional_t<std::same_as<Child_t, void>, const SerDesEngine<void> *, const Child_t *>;

      template <concepts::BasicSerializable T, concepts::ByteDataOutputIterator It>
      constexpr Result<It> Serialize(const T &val, It out) const
      {
        if constexpr (concepts::arithmetic_or_enum<T>)
        {
          const auto &temp = std::bit_cast<std::array<std::byte, sizeof(T)>>(val);
          out = std::copy(temp.begin(), temp.end(), out);
          return {sizeof(T), out};
        }
        else if constexpr (concepts::array_like<T>)
        {
          Result<It> res(static_cast<size_t>(0), out);
          std::all_of(std::begin(val), std::end(val), [this, &out, &res](const auto &el) {
            auto r = static_cast<This_t>(this)->Serialize(std::forward<decltype(el)>(el), out);
            out = r.get_iterator();
            return static_cast<bool>(res.update(r));
          });
          return res;
        }
        else if constexpr (concepts::range_constructible_container<T>)
        {
          Result<It> res(static_cast<size_t>(0), out);
          {
            const size_type numElements = [&val] {
              if constexpr (requires { std::size(val); })
              {
                return std::size(val);
              }
              else
              {
                return std::distance(std::begin(val), std::end(val));
              }
            }();
            auto r = static_cast<This_t>(this)->Serialize(numElements, out);
            out = r.get_iterator();
            res.update(r);
          }
          std::all_of(val.begin(), val.end(), [this, &out, &res](const auto &el) {
            auto r = static_cast<This_t>(this)->Serialize(std::forward<decltype(el)>(el), out);
            out = r.get_iterator();
            return static_cast<bool>(res.update(r));
          });
          return res;
        }
        else if constexpr (concepts::tuple_like<T>)
        {
          return serialize_tuple_like(val, out, std::make_index_sequence<std::tuple_size_v<T>>());
        }
        else if constexpr (concepts::custom_static_serializable<T>)
        {
          return serialize_custom_serializable(val, out, std::make_index_sequence<std::tuple_size_v<decltype(T::EnkiSerial::members)>>());
        }
      }

      template <concepts::BasicSerializable T, concepts::ByteDataInputIterator It>
      constexpr Result<It> Deserialize(T &val, It in) const
      {
        if constexpr (concepts::arithmetic_or_enum<T>)
        {
          std::array<std::byte, sizeof(T)> temp{};
          std::copy_n(in, temp.size(), temp.begin());
          val = std::bit_cast<T>(temp);
          return {sizeof(T), std::next(in, temp.size())};
        }
        else if constexpr (concepts::array_like<T>)
        {
          Result<It> res(static_cast<size_t>(0), in);
          std::all_of(std::begin(val), std::end(val), [this, &res, &in](auto &v) {
            auto r = static_cast<This_t>(this)->Deserialize(v, in);
            in = r.get_iterator();
            return static_cast<bool>(res.update(r));
          });
          return res;
        }
        else if constexpr (concepts::range_constructible_container<T>)
        {
          size_type numElements{};
          Result<It> res(static_cast<size_t>(0), in);
          {
            auto r = static_cast<This_t>(this)->Deserialize(numElements, in);
            in = r.get_iterator();
            res.update(r);
          }
          using value_type = details::assignable_value_t<T>;
          value_type *temp = new value_type[numElements];
          if (std::all_of(temp, temp + numElements, [this, &res, &in](auto &v) {
            auto r = static_cast<This_t>(this)->Deserialize(v, in);
            in = r.get_iterator();
            return static_cast<bool>(res.update(r));
          }))
          {
            val = T(temp, temp + numElements);
          }
          delete[] temp;
          return res;
        }
        else if constexpr (concepts::tuple_like<T>)
        {
          return deserialize_tuple_like(val, in, std::make_index_sequence<std::tuple_size_v<T>>());
        }
        else if constexpr (concepts::custom_static_serializable<T>)
        {
          return deserialize_custom_serializable(val, in, std::make_index_sequence<std::tuple_size_v<decltype(T::EnkiSerial::members)>>());
        }
      }

      template <concepts::BasicSerializable T>
      constexpr Result<void> NumBytes(const T &val) const
      {
        if constexpr (concepts::arithmetic_or_enum<T>)
        {
          return {sizeof(T)};
        }
        else if constexpr (concepts::array_like<T>)
        {
          Result<void> res = static_cast<This_t>(this)->NumBytes(val[0]);
          if (res)
          {
            res = res.size() * std::size(val);
          }
          return res;
        }
        else if constexpr (concepts::range_constructible_container<T>)
        {
          Result<void> res = static_cast<This_t>(this)->NumBytes(size_type{});
          std::all_of(val.begin(), val.end(), [this, &res](const auto &el) {
            auto r = static_cast<This_t>(this)->NumBytes(std::forward<decltype(el)>(el));
            return static_cast<bool>(res.update(r));
          });
          return res;
        }
        else if constexpr (concepts::tuple_like<T>)
        {
          return num_bytes_tuple_like(val, std::make_index_sequence<std::tuple_size_v<T>>());
        }
        else if constexpr (concepts::custom_static_serializable<T>)
        {
          return num_bytes_custom_serializable(val, std::make_index_sequence<std::tuple_size_v<decltype(T::EnkiSerial::members)>>());
        }
      }

    private:
      template <size_t i, concepts::BasicSerializable T, concepts::ByteDataOutputIterator It>
      constexpr bool serialize_one_in_tuple_like(Result<It> &res, const T &val, It &out) const
      {
        auto r = static_cast<This_t>(this)->Serialize(std::get<i>(val), out);
        out = r.get_iterator();
        return static_cast<bool>(res.update(r));
      }

      template <concepts::BasicSerializable T, concepts::ByteDataOutputIterator It, size_t... idx>
      constexpr Result<It> serialize_tuple_like(const T &val, It out, std::index_sequence<idx...>) const
      {
        Result<It> res(static_cast<size_t>(0), out);
        static_cast<void>((serialize_one_in_tuple_like<idx>(res, val, out) && ...));
        return res;
      }

      template <size_t i, concepts::BasicSerializable T, concepts::ByteDataInputIterator It>
      constexpr bool deserialize_one_in_tuple_like(Result<It> &res, T &val, It &in) const
      {
        auto r = static_cast<This_t>(this)->Deserialize(std::get<i>(val), in);
        in = r.get_iterator();
        return static_cast<bool>(res.update(r));
      }

      template <concepts::BasicSerializable T, concepts::ByteDataInputIterator It, size_t... idx>
      constexpr Result<It> deserialize_tuple_like(T &val, It in, std::index_sequence<idx...>) const
      {
        Result<It> res(static_cast<size_t>(0), in);
        static_cast<void>((deserialize_one_in_tuple_like<idx>(res, val, in) && ...));
        return res;
      }

      template <concepts::BasicSerializable T, size_t... idx>
      constexpr Result<void> num_bytes_tuple_like(const T &val, std::index_sequence<idx...>) const
      {
        Result<void> res(static_cast<size_t>(0));
        static_cast<void>((static_cast<bool>(res.update(static_cast<This_t>(this)->NumBytes(std::get<idx>(val)))) && ...));
        return res;
      }

      template <auto onemem, concepts::custom_static_serializable T, concepts::ByteDataOutputIterator It>
      constexpr Result<It> serialize_one_custom_serializable(const T &inst, It &out) const
      {
        auto r = static_cast<This_t>(this)->Serialize(inst.*onemem, out);
        out = r.get_iterator();
        return r;
      }

      template <auto onemem, concepts::custom_static_serializable T, concepts::ByteDataOutputIterator It>
        requires concepts::proper_member_wrapper<T, decltype(onemem)>
      constexpr Result<It> serialize_one_custom_serializable(const T &inst, It &out) const
      {
        auto r = static_cast<This_t>(this)->Serialize(onemem.getter(inst), out);
        out = r.get_iterator();
        return r;
      }

      template <concepts::custom_static_serializable T, concepts::ByteDataOutputIterator It, size_t... idx>
      constexpr Result<It> serialize_custom_serializable(const T &inst, It out, std::index_sequence<idx...>) const
      {
        Result<It> res(static_cast<size_t>(0), out);
        // out iterator is auto in calls to serialize_one_custom_serializable
        static_cast<void>((
          static_cast<bool>(res.update(serialize_one_custom_serializable<std::get<idx>(T::EnkiSerial::members)>(inst, out)))
          && ...));
        return res;
      }

      template <auto onemem, concepts::custom_static_serializable T, concepts::ByteDataInputIterator It>
      constexpr Result<It> deserialize_one_custom_serializable(T &inst, It &in) const
      {
        auto r = static_cast<This_t>(this)->Deserialize(inst.*onemem, in);
        in = r.get_iterator();
        return r;
      }

      template <auto onemem, concepts::custom_static_serializable T, concepts::ByteDataInputIterator It>
        requires concepts::proper_member_wrapper<T, decltype(onemem)>
      constexpr Result<It> deserialize_one_custom_serializable(T &inst, It &in) const
      {
        typename decltype(onemem)::value_type temp{};
        auto r = static_cast<This_t>(this)->Deserialize(temp, in);
        onemem.setter(inst, temp);
        in = r.get_iterator();
        return r;
      }

      template <concepts::custom_static_serializable T, concepts::ByteDataInputIterator It, size_t... idx>
      constexpr Result<It> deserialize_custom_serializable(T &inst, It in, std::index_sequence<idx...>) const
      {
        Result<It> res(static_cast<size_t>(0), in);
        // in iterator is auto in calls to deserialize_one_custom_serializable
        static_cast<void>((
          static_cast<bool>(res.update(deserialize_one_custom_serializable<std::get<idx>(T::EnkiSerial::members)>(inst, in)))
          && ...));
        return res;
      }

      template <auto onemem, concepts::custom_static_serializable T>
      constexpr Result<void> num_bytes_one_custom_serializable(const T &inst) const
      {
        return static_cast<This_t>(this)->NumBytes(inst.*onemem);
      }

      template <auto onemem, concepts::custom_static_serializable T>
        requires concepts::proper_member_wrapper<T, decltype(onemem)>
      constexpr Result<void> num_bytes_one_custom_serializable(const T &inst) const
      {
        return static_cast<This_t>(this)->NumBytes(onemem.getter(inst));
      }

      template <concepts::custom_static_serializable T, size_t... idx>
      constexpr Result<void> num_bytes_custom_serializable(const T &inst, std::index_sequence<idx...>) const
      {
        Result<void> res(static_cast<size_t>(0));
        static_cast<void>((
          static_cast<bool>(res.update(num_bytes_one_custom_serializable<std::get<idx>(T::EnkiSerial::members)>(inst)))
          && ...));
        return res;
      }
    };
  }
}

#endif // SERDES_ENGINE_HPP
