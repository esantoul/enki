#ifndef ENKI_ENKI_SERIALIZE_HPP
#define ENKI_ENKI_SERIALIZE_HPP

#include <algorithm>
#include <limits>

#include "enki/impl/concepts.hpp"
#include "enki/impl/policies.hpp"
#include "enki/impl/success.hpp"
#include "enki/impl/utilities.hpp"

namespace enki
{
  namespace detail
  {
    template <typename T, typename Writer>
    concept immediately_writeable = requires(Writer w, const T &v) {
      { w.write(v) } -> std::same_as<enki::Success>;
    };

    template <typename T, typename Writer, size_t... idx>
    constexpr Success serializeTupleLike(const T &value, Writer &&w, std::index_sequence<idx...>);

    template <typename T, typename Writer, size_t... idx>
    constexpr Success serializeCustom(const T &value, Writer &&w, std::index_sequence<idx...>);

    template <typename... Functors>
    struct Overloaded : Functors...
    {
      using Functors::operator()...;
    };

    template <typename... Functors>
    Overloaded(Functors...) -> Overloaded<Functors...>;
  } // namespace detail

  template <typename T, typename Writer>
  constexpr Success serialize(const T &value, Writer &&w)
  {
    // All the logics for value decomposition is here

    // If the writer has a specialized `write` method
    // able to handle a value of type `T` it will be
    // used in priority
    if constexpr (detail::immediately_writeable<T, Writer>)
    {
      return w.write(value);
    }
    else if constexpr (concepts::array_like<T>)
    {
      const size_t numElements = std::size(value);
      Success isGood = w.arrayBegin();
      if (!isGood)
      {
        return isGood;
      }
      size_t i = 0;
      static_cast<void>(std::all_of(
        std::begin(value), std::end(value), [numElements, &i, &w, &isGood](const auto &el) {
          ++i;
          if (isGood.update(serialize(std::forward<decltype(el)>(el), w)) && i != numElements)
          {
            if (!isGood.update(w.nextArrayElement()))
            {
              return false;
            }
          }
          return static_cast<bool>(isGood);
        }));
      if (isGood)
      {
        isGood.update(w.arrayEnd());
      }
      return isGood;
    }
    else if constexpr (concepts::range_constructible_container<T>)
    {
      Success isGood;
      const size_t numElements = [](const T &v) {
        if constexpr (requires { std::size(v); })
        {
          return std::size(v);
        }
        else
        {
          return std::distance(std::begin(v), std::end(v));
        }
      }(value);
      isGood = w.rangeBegin(numElements);
      if (!isGood)
      {
        return isGood;
      }
      size_t i = 0;
      static_cast<void>(std::all_of(
        std::begin(value), std::end(value), [&i, numElements, &w, &isGood](const auto &el) {
          ++i;
          if (isGood.update(serialize(std::forward<decltype(el)>(el), w)) && i != numElements)
          {
            if (!isGood.update(w.nextArrayElement()))
            {
              return false;
            }
          }
          return static_cast<bool>(isGood);
        }));
      if (isGood)
      {
        w.rangeEnd();
      }
      return isGood;
    }
    else if constexpr (concepts::tuple_like<T>)
    {
      return detail::serializeTupleLike(value, w, std::make_index_sequence<std::tuple_size_v<T>>());
    }
    else if constexpr (concepts::optional_like<T>)
    {
      Success isGood = serialize(static_cast<bool>(value), w);
      if (!isGood)
      {
        return isGood;
      }
      if (value)
      {
        isGood.update(serialize(*value, w));
      }
      return isGood;
    }
    else if constexpr (concepts::variant_like<T>)
    {
      using Policy = typename std::remove_cvref_t<Writer>::policy_type;
      using SizeType = typename std::remove_cvref_t<Writer>::size_type;

      if (value.index() > std::numeric_limits<SizeType>::max())
      {
        return "Variant index is too large to be serialized";
      }

      if (value.index() >= std::variant_size_v<T>)
      {
        return "Variant cannot be serialized because it is in invalid state";
      }

      Success isGood = serialize(static_cast<SizeType>(value.index()), w);
      if (!isGood)
      {
        return isGood;
      }

      // For forward_compat policy, use writeSkippable to handle size prefix
      // This enables skipping unknown variant alternatives during deserialization
      if constexpr (std::is_same_v<Policy, forward_compat_t>)
      {
        std::visit(
          detail::Overloaded{
            [&isGood, &w](const std::monostate &) {
              // Write size 0 for monostate (no data to follow)
              isGood.update(w.writeSkippable([](auto &&) { return Success{}; }));
            },
            [&isGood, &w](const auto &v) {
              isGood.update(w.writeSkippable([&v](auto &&writer) {
                return serialize(v, std::forward<decltype(writer)>(writer));
              }));
            }},
          value);
      }
      else
      {
        std::visit(
          detail::Overloaded{
            [](const std::monostate &) {},
            [&isGood, &w](const auto &v) { isGood.update(serialize(v, w)); }},
          value);
      }

      return isGood;
    }
    else if constexpr (concepts::custom_static_serializable<T>)
    {
      return detail::serializeCustom(
        value, w, std::make_index_sequence<T::EnkiSerial::Members::count>());
    }
    else
    {
      static_assert(
        !sizeof(T) /* Always evaluate to false but is only compiled if this branch is
                      effectively instantiated */
        ,
        "Cannot serialize value");
      return "Cannot serialize value";
    }

    return {};
  }

  namespace detail
  {
    template <typename T, typename Writer, size_t... idx>
    constexpr Success
    serializeTupleLike(const T &value, Writer &&writer, std::index_sequence<idx...>)
    {
      Success ret = writer.arrayBegin();
      if (!ret)
      {
        return ret;
      }
      size_t i = 0;
      const auto serializeOne = [](const auto &elem, Writer &&w, Success &isGood, size_t &ii) {
        ++ii;
        if (isGood.update(serialize(elem, w)) && ii != sizeof...(idx))
        {
          if (!isGood.update(w.nextArrayElement()))
          {
            return false;
          }
        }

        return static_cast<bool>(isGood);
      };

      static_cast<void>((serializeOne(std::get<idx>(value), writer, ret, i) && ...));

      if (ret)
      {
        ret.update(writer.arrayEnd());
      }

      return ret;
    }

    template <
      std::derived_from<RegisterBase> Reg,
      concepts::custom_static_serializable T,
      typename Writer>
    constexpr bool serializeOneCustom(const T &inst, Writer &&writer, Success &isGood, bool isLast)
    {
      if constexpr (std::remove_cvref_t<Writer>::serialize_custom_names)
      {
        if (!isGood.update(writer.objectName(Reg::name)))
        {
          return false;
        }
      }
      if (isGood.update(serialize(Reg::getter(inst), writer)) && !isLast)
      {
        if (!isGood.update(writer.nextObjectElement()))
        {
          return false;
        }
      }

      return static_cast<bool>(isGood);
    }

    template <typename T, typename Writer, size_t... idx>
    constexpr Success serializeCustom(const T &value, Writer &&writer, std::index_sequence<idx...>)
    {
      Success ret = writer.objectBegin();
      if (!ret)
      {
        return ret;
      }
      size_t i = 0;

      static_cast<void>(
        (serializeOneCustom<detail::get_nth_register_t<idx, typename T::EnkiSerial::Members>>(
           value, writer, ret, (++i) == sizeof...(idx)) &&
         ...));

      if (ret)
      {
        ret.update(writer.objectEnd());
      }

      return ret;
    }
  } // namespace detail
} // namespace enki

#endif // ENKI_ENKI_SERIALIZE_HPP
