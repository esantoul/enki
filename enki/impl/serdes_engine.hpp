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
#include "enki/impl/success.hpp"
#include "enki/impl/utilities.hpp"

namespace enki
{
  namespace details
  {
    /**
     * @brief The CRTP Base class for custom SerDes
     * @tparam Child_t must be either void or the child class that inherits from SerDesEngine and
     * implements public methods serialize, Deserialize & numBytes
     */
    template <typename Child>
    class SerDesEngine
    {
    public:
      using size_type = uint32_t;

    protected:
      using This_t =
        std::conditional_t<std::same_as<Child, void>, const SerDesEngine<void> *, const Child *>;

      template <concepts::BasicSerializable T, concepts::ByteDataOutputIterator It>
      constexpr Success<It> serialize(const T &val, It out) const
      {
        if constexpr (concepts::arithmetic_or_enum<T>)
        {
          const auto &temp = std::bit_cast<std::array<std::byte, sizeof(T)>>(val);
          out = std::copy(temp.begin(), temp.end(), out);
          return {sizeof(T), out};
        }
        else if constexpr (concepts::array_like<T>)
        {
          Success<It> res(static_cast<size_t>(0), out);
          std::all_of(std::begin(val), std::end(val), [this, &out, &res](const auto &el) {
            auto r = static_cast<This_t>(this)->serialize(std::forward<decltype(el)>(el), out);
            out = r.get_iterator();
            return static_cast<bool>(res.update(r));
          });
          return res;
        }
        else if constexpr (concepts::range_constructible_container<T>)
        {
          Success<It> res(static_cast<size_t>(0), out);
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
            auto r = static_cast<This_t>(this)->serialize(numElements, out);
            out = r.get_iterator();
            res.update(r);
          }
          std::all_of(val.begin(), val.end(), [this, &out, &res](const auto &el) {
            auto r = static_cast<This_t>(this)->serialize(std::forward<decltype(el)>(el), out);
            out = r.get_iterator();
            return static_cast<bool>(res.update(r));
          });
          return res;
        }
        else if constexpr (concepts::tuple_like<T>)
        {
          return serializeTupleLike(val, out, std::make_index_sequence<std::tuple_size_v<T>>());
        }
        else if constexpr (concepts::custom_static_serializable<T>)
        {
          return serializeCustomSerializable(
            val,
            out,
            std::make_index_sequence<std::tuple_size_v<decltype(T::EnkiSerial::members)>>());
        }
      }

      template <concepts::BasicSerializable T, concepts::ByteDataInputIterator It>
      constexpr Success<It> deserialize(T &val, It in) const
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
          Success<It> res(static_cast<size_t>(0), in);
          std::all_of(std::begin(val), std::end(val), [this, &res, &in](auto &v) {
            auto r = static_cast<This_t>(this)->deserialize(v, in);
            in = r.get_iterator();
            return static_cast<bool>(res.update(r));
          });
          return res;
        }
        else if constexpr (concepts::range_constructible_container<T>)
        {
          size_type numElements{};
          Success<It> res(static_cast<size_t>(0), in);
          {
            auto r = static_cast<This_t>(this)->deserialize(numElements, in);
            in = r.get_iterator();
            res.update(r);
          }
          using value_type = details::assignable_value_t<T>; // NOLINT
          value_type *temp = new value_type[numElements];
          if (std::all_of(temp, temp + numElements, [this, &res, &in](auto &v) {
                auto r = static_cast<This_t>(this)->deserialize(v, in);
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
          return deserializeTupleLike(val, in, std::make_index_sequence<std::tuple_size_v<T>>());
        }
        else if constexpr (concepts::custom_static_serializable<T>)
        {
          return deserializeCustomSerializable(
            val,
            in,
            std::make_index_sequence<std::tuple_size_v<decltype(T::EnkiSerial::members)>>());
        }
      }

      template <concepts::BasicSerializable T>
      constexpr Success<void> numBytes(const T &val) const
      {
        if constexpr (concepts::arithmetic_or_enum<T>)
        {
          return {sizeof(T)};
        }
        else if constexpr (concepts::array_like<T>)
        {
          Success<void> res = static_cast<This_t>(this)->numBytes(val[0]);
          if (res)
          {
            res = res.size() * std::size(val);
          }
          return res;
        }
        else if constexpr (concepts::range_constructible_container<T>)
        {
          Success<void> res = static_cast<This_t>(this)->numBytes(size_type{});
          std::all_of(val.begin(), val.end(), [this, &res](const auto &el) {
            auto r = static_cast<This_t>(this)->numBytes(std::forward<decltype(el)>(el));
            return static_cast<bool>(res.update(r));
          });
          return res;
        }
        else if constexpr (concepts::tuple_like<T>)
        {
          return numBytesTupleLike(val, std::make_index_sequence<std::tuple_size_v<T>>());
        }
        else if constexpr (concepts::custom_static_serializable<T>)
        {
          return numBytesCustomSerializable(
            val, std::make_index_sequence<std::tuple_size_v<decltype(T::EnkiSerial::members)>>());
        }
      }

    private:
      template <size_t i, concepts::BasicSerializable T, concepts::ByteDataOutputIterator It>
      constexpr bool serializeOneInTupleLike(Success<It> &res, const T &val, It &out) const
      {
        auto r = static_cast<This_t>(this)->serialize(std::get<i>(val), out);
        out = r.get_iterator();
        return static_cast<bool>(res.update(r));
      }

      template <concepts::BasicSerializable T, concepts::ByteDataOutputIterator It, size_t... idx>
      constexpr Success<It>
      serializeTupleLike(const T &val, It out, std::index_sequence<idx...>) const
      {
        Success<It> res(static_cast<size_t>(0), out);
        static_cast<void>((serializeOneInTupleLike<idx>(res, val, out) && ...));
        return res;
      }

      template <size_t i, concepts::BasicSerializable T, concepts::ByteDataInputIterator It>
      constexpr bool deserializeOneInTupleLike(Success<It> &res, T &val, It &in) const
      {
        auto r = static_cast<This_t>(this)->deserialize(std::get<i>(val), in);
        in = r.get_iterator();
        return static_cast<bool>(res.update(r));
      }

      template <concepts::BasicSerializable T, concepts::ByteDataInputIterator It, size_t... idx>
      constexpr Success<It> deserializeTupleLike(T &val, It in, std::index_sequence<idx...>) const
      {
        Success<It> res(static_cast<size_t>(0), in);
        static_cast<void>((deserializeOneInTupleLike<idx>(res, val, in) && ...));
        return res;
      }

      template <concepts::BasicSerializable T, size_t... idx>
      constexpr Success<void> numBytesTupleLike(const T &val, std::index_sequence<idx...>) const
      {
        Success<void> res(static_cast<size_t>(0));
        static_cast<void>(
          (static_cast<bool>(res.update(static_cast<This_t>(this)->numBytes(std::get<idx>(val)))) &&
           ...));
        return res;
      }

      template <
        auto onemem,
        concepts::custom_static_serializable T,
        concepts::ByteDataOutputIterator It>
      constexpr Success<It> serializeOneCustomSerializable(const T &inst, It &out) const
      {
        auto r = static_cast<This_t>(this)->serialize(inst.*onemem, out);
        out = r.get_iterator();
        return r;
      }

      template <
        auto onemem,
        concepts::custom_static_serializable T,
        concepts::ByteDataOutputIterator It>
        requires concepts::proper_member_wrapper<T, decltype(onemem)>
      constexpr Success<It> serializeOneCustomSerializable(const T &inst, It &out) const
      {
        auto r = static_cast<This_t>(this)->serialize(onemem.getter(inst), out);
        out = r.get_iterator();
        return r;
      }

      template <
        concepts::custom_static_serializable T,
        concepts::ByteDataOutputIterator It,
        size_t... idx>
      constexpr Success<It>
      serializeCustomSerializable(const T &inst, It out, std::index_sequence<idx...>) const
      {
        Success<It> res(static_cast<size_t>(0), out);
        // out iterator is auto in calls to serialize_one_custom_serializable
        static_cast<void>(
          (static_cast<bool>(res.update(
             serializeOneCustomSerializable<std::get<idx>(T::EnkiSerial::members)>(inst, out))) &&
           ...));
        return res;
      }

      template <
        auto onemem,
        concepts::custom_static_serializable T,
        concepts::ByteDataInputIterator It>
      constexpr Success<It> deserializeOneCustomSerializable(T &inst, It &in) const
      {
        auto r = static_cast<This_t>(this)->deserialize(inst.*onemem, in);
        in = r.get_iterator();
        return r;
      }

      template <
        auto onemem,
        concepts::custom_static_serializable T,
        concepts::ByteDataInputIterator It>
        requires concepts::proper_member_wrapper<T, decltype(onemem)>
      constexpr Success<It> deserializeOneCustomSerializable(T &inst, It &in) const
      {
        typename decltype(onemem)::value_type temp{};
        auto r = static_cast<This_t>(this)->Deserialize(temp, in);
        onemem.setter(inst, temp);
        in = r.get_iterator();
        return r;
      }

      template <
        concepts::custom_static_serializable T,
        concepts::ByteDataInputIterator It,
        size_t... idx>
      constexpr Success<It>
      deserializeCustomSerializable(T &inst, It in, std::index_sequence<idx...>) const
      {
        Success<It> res(static_cast<size_t>(0), in);
        // in iterator is auto in calls to deserialize_one_custom_serializable
        static_cast<void>(
          (static_cast<bool>(res.update(
             deserializeOneCustomSerializable<std::get<idx>(T::EnkiSerial::members)>(inst, in))) &&
           ...));
        return res;
      }

      template <auto onemem, concepts::custom_static_serializable T>
      constexpr Success<void> numBytesOneCustomSerializable(const T &inst) const
      {
        return static_cast<This_t>(this)->numBytes(inst.*onemem);
      }

      template <auto onemem, concepts::custom_static_serializable T>
        requires concepts::proper_member_wrapper<T, decltype(onemem)>
      constexpr Success<void> numBytesOneCustomSerializable(const T &inst) const
      {
        return static_cast<This_t>(this)->numBytes(onemem.getter(inst));
      }

      template <concepts::custom_static_serializable T, size_t... idx>
      constexpr Success<void>
      numBytesCustomSerializable(const T &inst, std::index_sequence<idx...>) const
      {
        Success<void> res(static_cast<size_t>(0));
        static_cast<void>(
          (static_cast<bool>(res.update(
             numBytesOneCustomSerializable<std::get<idx>(T::EnkiSerial::members)>(inst))) &&
           ...));
        return res;
      }
    };
  } // namespace details
} // namespace enki

#endif // SERDES_ENGINE_HPP
