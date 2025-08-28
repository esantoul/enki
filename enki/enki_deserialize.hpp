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
      {
        r.read(v)
      } -> std::same_as<enki::Success>;
    };

    template <typename T, typename Reader, size_t... idx>
    constexpr Success deserializeTupleLike(T &value, Reader &&reader, std::index_sequence<idx...>);

    template <typename T, typename Reader, size_t... idx>
    constexpr Success deserializeCustom(T &value, Reader &&r, std::index_sequence<idx...>);

    template <typename T, typename Reader, size_t... idx>
    constexpr Success deserializeVariantLike(
      T &value,
      Reader &&r,
      size_t alternativeIndex,
      std::index_sequence<idx...>);
  } // namespace detail

  template <typename T, typename Reader>
  constexpr Success deserialize(T &value, Reader &&r)
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
      Success isGood = r.arrayBegin();
      if (!isGood)
      {
        return isGood;
      }
      size_t i = 0;
      static_cast<void>(
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
        }));
      if (isGood)
      {
        isGood.update(r.arrayEnd());
      }
      return isGood;
    }
    else if constexpr (concepts::range_constructible_container<T>)
    {
      Success isGood;
      size_t numElements = 0;
      isGood = r.rangeBegin(numElements);
      if (!isGood)
      {
        return isGood;
      }
      using value_type = detail::assignable_value_t<T>; // NOLINT

      std::vector<value_type> temp(numElements);
      for (size_t i = 0; (i < numElements) && isGood; ++i)
      {
        if (isGood.update(deserialize(temp[i], r)) && i != (numElements - 1))
        {
          if (!isGood.update(r.nextRangeElement()))
          {
            return isGood;
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
      Success isGood = deserialize(active, r);
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
    else if constexpr (concepts::variant_like<T>)
    {
      typename std::remove_cvref_t<Reader>::size_type index = -1;

      Success isGood = deserialize(index, r);
      if (!isGood)
      {
        return isGood;
      }

      if (index >= std::variant_size_v<T>)
      {
        return isGood.update("Deserialized variant index is out of range");
      }

      isGood.update(detail::deserializeVariantLike(
        value, r, index, std::make_index_sequence<std::variant_size_v<T>>()));

      return isGood;
    }
    else if constexpr (concepts::custom_static_serializable<T>)
    {
      return detail::deserializeCustom(
        value, r, std::make_index_sequence<T::EnkiSerial::Members::count>());
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
    constexpr Success deserializeTupleLike(T &value, Reader &&reader, std::index_sequence<idx...>)
    {
      Success ret = reader.arrayBegin();
      if (!ret)
      {
        return ret;
      }
      size_t i = 0;
      const auto deserializeOne = [](auto &elem, Reader &&w, Success &isGood, size_t &ii) {
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

      static_cast<void>((deserializeOne(std::get<idx>(value), reader, ret, i) && ...));

      if (ret)
      {
        ret.update(reader.arrayEnd());
      }

      return ret;
    }

    template <
      std::derived_from<detail::RegisterBase> Reg,
      concepts::custom_static_serializable T,
      typename Reader>
    constexpr bool deserializeOneCustom(T &inst, Reader &&reader, Success &isGood, bool isLast)
    {
      if constexpr (std::remove_cvref_t<Reader>::serialize_custom_names)
      {
        if (!isGood.update(reader.objectName(Reg::name)))
        {
          return false;
        }
      }
      typename Reg::value_type temp{};
      if (!isGood.update(deserialize(temp, reader)))
      {
        return false;
      }
      Reg::setter(inst, temp);

      if (!isLast)
      {
        if (!isGood.update(reader.nextObjectElement()))
        {
          return false;
        }
      }

      return static_cast<bool>(isGood);
    }

    template <typename T, typename Reader, size_t... idx>
    constexpr Success deserializeCustom(T &value, Reader &&reader, std::index_sequence<idx...>)
    {
      Success ret = reader.objectBegin();
      if (!ret)
      {
        return ret;
      }
      size_t i = 0;

      static_cast<void>(
        (deserializeOneCustom<detail::get_nth_register_t<idx, typename T::EnkiSerial::Members>>(
           value, reader, ret, (++i) == sizeof...(idx)) &&
         ...));

      if (ret)
      {
        ret.update(reader.objectEnd());
      }

      return ret;
    }

    template <typename Alternative>
    struct AlternativeDeserializer
    {
      template <typename T, typename Reader>
      constexpr bool operator()(T &value, Reader &&r, bool isCorrectType, Success &isGood)
      {
        if (!isCorrectType)
        {
          return false;
        }
        Alternative toDeserialize;
        if (isGood.update(deserialize(toDeserialize, r)))
        {
          value = std::move(toDeserialize);
        }
        return true;
      }
    };

    template <>
    struct AlternativeDeserializer<std::monostate>
    {
      template <typename T, typename Reader>
      constexpr bool operator()(T &value, Reader &&, bool isCorrectType, Success &)
      {
        if (!isCorrectType)
        {
          return false;
        }
        value = std::monostate{};
        return true;
      }
    };

    template <typename T, typename Reader, size_t... idx>
    constexpr Success deserializeVariantLike(
      T &value,
      Reader &&r,
      size_t alternativeIndex,
      std::index_sequence<idx...>)
    {
      Success isGood;
      static_cast<void>(
        (AlternativeDeserializer<std::variant_alternative_t<idx, T>>{}(
           value, r, idx == alternativeIndex, isGood) ||
         ...));
      return isGood;
    }
  } // namespace detail
} // namespace enki

#endif // ENKI_ENKI_DESERIALIZE_HPP
