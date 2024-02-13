#ifndef ENKI_ENKI_SERIALIZE_HPP
#define ENKI_ENKI_SERIALIZE_HPP

#include "success.hpp"
#include "concepts.hpp"

#include <algorithm>

namespace enki
{
  namespace details
  {
    template <typename T, typename Writer>
    concept immediately_writeable = requires (Writer w, const T & v)
    {
      { w.write(v) } -> std::same_as<enki::Success<void>>;
    };
  }

  template <typename Writer, typename T>
  Success<void> serialize(const T &value, Writer &w)
  {
    // All the logics for value decomposition is here

    // If the writer has a specialized `write` method
    // able to handle a value of type `T` it will be
    // used in priority
    if constexpr (details::immediately_writeable<T, Writer>)
    {
      return w.write(value);
    }
    else if constexpr (concepts::array_like<T>)
    {
      const size_t numElements = std::size(value);
      Success<void> isGood = w.arrayBegin(numElements);
      if (!isGood)
      {
        return isGood;
      }
      size_t i = 0;
      std::all_of(std::begin(value), std::end(value), [numElements, &i, &w, &isGood](const auto &el) {
        ++i;
        isGood = serialize(std::forward<decltype(el)>(el), w);
        if (isGood && i != numElements)
        {
          w.nextArrayElement();
        }
        return static_cast<bool>(isGood);
      });
      if (isGood)
      {
        isGood = w.arrayEnd();
      }
      return isGood;
    }
    else if constexpr (concepts::range_constructible_container<T>)
    {
      Success<void> isGood;
      const size_t numElements = [&value] {
        if constexpr (requires { std::size(value); })
        {
          return std::size(value);
        }
        else
        {
          return std::distance(std::begin(value), std::end(value));
        }
      }();
      isGood = w.rangeBegin(numElements);
      if (!isGood)
      {
        return isGood;
      }
      size_t i = 0;
      std::all_of(std::begin(value), std::end(value), [&i, numElements, &w, &isGood](const auto &el) {
        ++i;
        isGood = serialize(std::forward<decltype(el)>(el), w);
        if (isGood && i != numElements)
        {
          w.nextRangeElement();
        }
        return static_cast<bool>(isGood);
      });
      if (isGood)
      {
        w.rangeEnd();
      }
      return isGood;
    }
    else if constexpr (concepts::tuple_like<T>)
    {
      return serialize_tuple_like(value, w, std::make_index_sequence<std::tuple_size_v<T>>());
    }
    else if constexpr (concepts::custom_static_serializable<T>)
    {
      return serialize_custom_serializable(value, w, std::make_index_sequence<std::tuple_size_v<decltype(T::EnkiSerial::members)>>());
    }
    else
    {
      return "Cannot serialize value";
    }

    return {};
  }
} // namespace enki

#endif // ENKI_ENKI_SERIALIZE_HPP
