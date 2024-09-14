#ifndef ENKI_ENKI_DESERIALIZE_HPP
#define ENKI_ENKI_DESERIALIZE_HPP

#include <algorithm>
#include <vector>

#include "enki/impl/concepts.hpp"
#include "enki/impl/success.hpp"

namespace enki
{
  namespace details
  {
    template <typename T, typename Reader>
    concept immediately_readable = requires(Reader r, T &v) {
      { r.read(v) } -> std::same_as<enki::Success<void>>;
    };
  } // namespace details

  template <typename Reader, typename T>
  Success<void> deserialize(T &value, Reader &&r)
  {
    // All the logics for value decomposition is here

    // If the writer has a specialized `write` method
    // able to handle a value of type `T` it will be
    // used in priority
    if constexpr (details::immediately_readable<T, Reader>)
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
          r.nextArrayElement();
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
      std::vector<typename T::value_type> temp(numElements);
      for (size_t i = 0; i < numElements && isGood; ++i)
      {
        isGood = deserialize(temp[i], r);
        if (isGood && i != numElements)
        {
          r.nextRangeElement();
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
      return deserialize_tuple_like(value, r, std::make_index_sequence<std::tuple_size_v<T>>());
    }
    else if constexpr (concepts::custom_static_serializable<T>)
    {
      return deserialize_custom_serializable(
        value, r, std::make_index_sequence<std::tuple_size_v<decltype(T::EnkiSerial::members)>>());
    }
    else
    {
      return "Cannot deserialize value";
    }

    return {};
  }
} // namespace enki

#endif // ENKI_ENKI_DESERIALIZE_HPP
