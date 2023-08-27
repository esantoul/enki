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
  namespace concepts
  {
    template <typename T>
    concept SerDes = requires (T sd)
    {
      {sd.Serialize(std::declval<int>(), std::declval<std::byte *>())} -> std::same_as<std::pair<enki::Result, std::byte *>>;
      {sd.Deserialize(std::declval<int &>(), std::declval<const std::byte *>())} -> std::same_as<std::pair<enki::Result, const std::byte *>>;
      {sd.NumBytes(std::declval<int>())} -> std::same_as<enki::Result>;
    };
  }

  namespace details
  {
    /**
     * @brief The CRTP Base class for custom SerDes
     * @tparam Child_t must be either void or the child class that inherits from SerDesEngine and implements public methods Serialize, Deserialize & NumBytes
    */
    template <typename Child_t>
    class SerDesEngine
    {
    protected:
      using This_t = std::conditional_t<std::same_as<Child_t, void>, const SerDesEngine<void> *, const Child_t *>;

      template <concepts::BasicSerializable T, concepts::ByteDataOutputIterator It>
      constexpr std::pair<Result, It> Serialize(const T &val, It out) const
      {
        if constexpr (concepts::arithmetic_or_enum<T>)
        {
          const auto &temp = std::bit_cast<std::array<std::byte, sizeof(T)>>(val);
          out = std::copy(temp.begin(), temp.end(), out);
          return {sizeof(T), out};
        }
        else if constexpr (concepts::array_like<T>)
        {
          Result res{};
          std::all_of(std::begin(val), std::end(val), [this, &out, &res](const auto &el) {
            auto [r, it] = static_cast<This_t>(this)->Serialize(std::forward<decltype(el)>(el), out);
            out = it;
            return static_cast<bool>(res.update(r));
          });
          return {res, out};
        }
        else if constexpr (concepts::range_constructible_container<T>)
        {
          Result res{};
          {
            const uint32_t numElements = [&val] {
              if constexpr (requires { std::size(val); })
              {
                return std::size(val);
              }
              else
              {
                return std::distance(std::begin(val), std::end(val));
              }
            }();
            auto [r, it] = static_cast<This_t>(this)->Serialize(numElements, out);
            out = it;
            res.update(r);
          }
          std::all_of(val.begin(), val.end(), [this, &out, &res](const auto &el) {
            auto [r, it] = static_cast<This_t>(this)->Serialize(std::forward<decltype(el)>(el), out);
            out = it;
            return static_cast<bool>(res.update(r));
          });
          return {res, out};
        }
        else if constexpr (concepts::tuple_like<T>)
        {
          return serialize_tuple_like(val, out, std::make_index_sequence<std::tuple_size_v<T>>());
        }
      }

      template <concepts::BasicSerializable T, concepts::ByteDataInputIterator It>
      constexpr std::pair<Result, It> Deserialize(T &val, It in) const
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
          Result res{};
          std::all_of(std::begin(val), std::end(val), [this, &res, &in](auto &v) {
            auto [r, it] = static_cast<This_t>(this)->Deserialize(v, in);
            in = it;
            return static_cast<bool>(res.update(r));
          });
          return {res, in};
        }
        else if constexpr (concepts::range_constructible_container<T>)
        {
          uint32_t sz{};
          Result res;
          {
            auto [r, it] = static_cast<This_t>(this)->Deserialize(sz, in);
            in = it;
            res.update(r);
          }
          using value_type = details::assignable_value_t<T>;
          value_type *temp = new value_type[sz];
          if (std::all_of(temp, temp + sz, [this, &res, &in](auto &v) {
            auto [r, it] = static_cast<This_t>(this)->Deserialize(v, in);
            in = it;
            return static_cast<bool>(res.update(r));
          }))
          {
            val = T(temp, temp + sz);
          }
          delete[] temp;
          return {res, in};
        }
        else if constexpr (concepts::tuple_like<T>)
        {
          return deserialize_tuple_like(val, in, std::make_index_sequence<std::tuple_size_v<T>>());
        }
      }

      template <concepts::BasicSerializable T>
      constexpr Result NumBytes(const T &val) const
      {
        if constexpr (concepts::arithmetic_or_enum<T>)
        {
          return sizeof(T);
        }
        else if constexpr (concepts::array_like<T>)
        {
          Result res = static_cast<This_t>(this)->NumBytes(val[0]);
          if (res)
          {
            res = res.size() * std::size(val);
          }
          return res;
        }
        else if constexpr (concepts::range_constructible_container<T>)
        {
          const uint32_t numElements = [&val] {
            if constexpr (requires { std::size(val); })
            {
              return std::size(val);
            }
            else
            {
              return std::distance(std::begin(val), std::end(val));
            }
          }();
          if (numElements == 0)
          {
            return Result{sizeof(uint32_t)};
          }

          Result res = static_cast<This_t>(this)->NumBytes(*val.begin());
          if (res)
          {
            res = res.size() * numElements + sizeof(uint32_t);
          }
          return res;
        }
        else if constexpr (concepts::tuple_like<T>)
        {
          return num_bytes_tuple_like(val, std::make_index_sequence<std::tuple_size_v<T>>());
        }
      }

    private:
      template <size_t i, concepts::BasicSerializable T, concepts::ByteDataOutputIterator It>
      constexpr bool serialize_one_in_tuple_like(Result &res, const T &val, It &out) const
      {
        auto [r, it] = static_cast<This_t>(this)->Serialize(std::get<i>(val), out);
        out = it;
        return static_cast<bool>(res.update(r));
      }

      template <size_t i, concepts::BasicSerializable T, concepts::ByteDataInputIterator It>
      constexpr bool deserialize_one_in_tuple_like(Result &res, T &val, It &in) const
      {
        auto [r, it] = static_cast<This_t>(this)->Deserialize(std::get<i>(val), in);
        in = it;
        return static_cast<bool>(res.update(r));
      }

      template <concepts::BasicSerializable T, concepts::ByteDataOutputIterator It, size_t... idx>
      constexpr std::pair<Result, It> serialize_tuple_like(const T &val, It out, std::index_sequence<idx...>) const
      {
        Result res{};
        static_cast<void>((serialize_one_in_tuple_like<idx>(res, val, out) && ...));
        return {res, out};
      }

      template <concepts::BasicSerializable T, concepts::ByteDataInputIterator It, size_t... idx>
      constexpr std::pair<Result, It> deserialize_tuple_like(T &val, It in, std::index_sequence<idx...>) const
      {
        Result res{};
        static_cast<void>((deserialize_one_in_tuple_like<idx>(res, val, in) && ...));
        return {res, in};
      }

      template <concepts::BasicSerializable T, size_t... idx>
      constexpr Result num_bytes_tuple_like(const T &val, std::index_sequence<idx...>) const
      {
        Result res{};
        static_cast<void>((static_cast<bool>(res.update(static_cast<This_t>(this)->NumBytes(std::get<idx>(val)))) && ...));
        return res;
      }
    };
  }
}

#endif // SERDES_ENGINE_HPP
