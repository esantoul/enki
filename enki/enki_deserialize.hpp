#ifndef ENKI_ENKI_DESERIALIZE_HPP
#define ENKI_ENKI_DESERIALIZE_HPP

#include <algorithm>
#include <vector>

#include "enki/impl/concepts.hpp"
#include "enki/impl/success.hpp"
#include "enki/impl/utilities.hpp"

namespace enki
{
  namespace detail
  {
    template <typename T, typename Reader>
    concept immediately_readable = requires(Reader r, T &v) {
      { r.read(v) } -> std::same_as<enki::Success<void>>;
    };

    template <typename T, typename Reader, size_t... idx>
    constexpr Success<void>
    deserializeTupleLike(T &value, Reader &&reader, std::index_sequence<idx...>);

    template <typename T, typename Reader, size_t... idx>
    constexpr Success<void> deserializeCustom(T &value, Reader &&w, std::index_sequence<idx...>);
  } // namespace detail

  template <typename T, typename Reader>
  constexpr Success<void> deserialize(T &value, Reader &&r)
  {
    // All the logics for value decomposition is here

    // If the reader has a specialized `reader` method
    // able to handle a value of type `T` it will be
    // used in priority
    if constexpr (detail::immediately_readable<T, Reader>)
    {
      return r.read(value);
    }
    else if constexpr (concepts::array_like<T>)
    {
      const size_t numElements = std::size(value);
      Success<void> isGood = r.arrayBegin();
      if (!isGood)
      {
        return isGood;
      }
      size_t i = 0;
      std::all_of(std::begin(value), std::end(value), [numElements, &i, &r, &isGood](auto &el) {
        ++i;
        if (isGood.update(deserialize(std::forward<decltype(el)>(el), r)) && i != numElements)
        {
          if (!isGood.update(r.nextArrayElement()))
          {
            return false;
          }
        }
        return static_cast<bool>(isGood);
      });
      if (isGood)
      {
        isGood.update(r.arrayEnd());
      }
      return isGood;
    }
    else if constexpr (concepts::range_constructible_container<T>)
    {
      Success<void> isGood;
      size_t numElements = 0;
      isGood = r.rangeBegin(numElements);
      if (!isGood)
      {
        return isGood;
      }
      using value_type = detail::assignable_value_t<T>; // NOLINT

      std::vector<value_type> temp(numElements);
      for (size_t i = 0; i < numElements && isGood; ++i)
      {
        if (isGood.update(deserialize(temp[i], r)) && i != numElements)
        {
          if (!isGood.update(r.nextRangeElement()))
          {
            return false;
          }
        }
      }
      value = {std::begin(temp), std::end(temp)};
      if (isGood)
      {
        r.rangeEnd();
      }
      return isGood;
    }
    else if constexpr (concepts::tuple_like<T>)
    {
      return detail::deserializeTupleLike(
        value, r, std::make_index_sequence<std::tuple_size_v<T>>());
    }
    else if constexpr (concepts::optional_like<T>)
    {
      bool active = false;
      Success<void> isGood = deserialize(active, r);
      if (!isGood)
      {
        return isGood;
      }
      if (active)
      {
        typename T::value_type deserializedValue;
        if (isGood.update(deserialize(deserializedValue, r)))
        {
          value = std::move(deserializedValue);
        }
      }
      else
      {
        value.reset();
      }
      return isGood;
    }
    else if constexpr (concepts::custom_static_serializable<T>)
    {
      return detail::deserializeCustom(
        value, r, std::make_index_sequence<std::tuple_size_v<decltype(T::EnkiSerial::members)>>());
    }
    else
    {
      static_assert(
        !sizeof(T) /* Always evaluate to false but is only compiled if this branch is
                      effectively instantiated */
        ,
        "Cannot deserialize value");
      return "Cannot deserialize value";
    }

    return {};
  }

  namespace detail
  {
    template <typename T, typename Reader, size_t... idx>
    constexpr Success<void>
    deserializeTupleLike(T &value, Reader &&reader, std::index_sequence<idx...>)
    {
      Success<void> ret = reader.arrayBegin();
      if (!ret)
      {
        return ret;
      }
      size_t i = 0;
      const auto deserializeOne = [](auto &elem, Reader &&w, Success<void> &isGood, size_t &ii) {
        ++ii;
        if (isGood.update(deserialize(elem, w)) && ii != sizeof...(idx))
        {
          if (!isGood.update(w.nextArrayElement()))
          {
            return false;
          }
        }

        return static_cast<bool>(isGood);
      };

      (deserializeOne(std::get<idx>(value), reader, ret, i) && ...);

      if (ret)
      {
        ret.update(reader.arrayEnd());
      }

      return ret;
    }

    template <auto member, concepts::custom_static_serializable T, typename Reader>
    constexpr bool
    deserializeOneCustom(T &inst, Reader &&reader, Success<void> &isGood, bool isLast)
    {
      if (isGood.update(deserialize(inst.*member, reader)) && !isLast)
      {
        if (!isGood.update(reader.nextArrayElement()))
        {
          return false;
        }
      }

      return static_cast<bool>(isGood);
    }

    template <auto member, concepts::custom_static_serializable T, typename Reader>
      requires concepts::proper_member_wrapper<T, decltype(member)>
    constexpr bool
    deserializeOneCustom(T &inst, Reader &&reader, Success<void> &isGood, bool isLast)
    {
      typename decltype(member)::value_type temp{};
      if (!isGood.update(deserialize(temp, reader)))
      {
        return false;
      }
      member.setter(inst, temp);

      if (!isLast)
      {
        if (!isGood.update(reader.nextArrayElement()))
        {
          return false;
        }
      }

      return static_cast<bool>(isGood);
    }

    template <typename T, typename Reader, size_t... idx>
    constexpr Success<void>
    deserializeCustom(T &value, Reader &&reader, std::index_sequence<idx...>)
    {
      Success<void> ret = reader.arrayBegin();
      if (!ret)
      {
        return ret;
      }
      size_t i = 0;

      (deserializeOneCustom<std::get<idx>(T::EnkiSerial::members)>(
         value, reader, ret, (++i) == sizeof...(idx)) &&
       ...);

      if (ret)
      {
        ret.update(reader.arrayEnd());
      }

      return ret;
    }
  } // namespace detail
} // namespace enki

#endif // ENKI_ENKI_DESERIALIZE_HPP
