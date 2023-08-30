#ifndef ENKI_CONCEPTS_HPP
#define ENKI_CONCEPTS_HPP

#include <concepts>
#include <type_traits>
#include <tuple>
#include <utility>
#include <array>
#include <variant>
#include <cstddef>

namespace enki::concepts
{
  namespace details
  {
    template <typename T>
    struct is_array_like : std::false_type
    {
    };

    template <typename T, size_t cty>
    struct is_array_like<std::array<T, cty>> : std::true_type
    {
    };

    template <typename T, size_t cty>
    struct is_array_like<T[cty]> : std::true_type
    {
    };
  } // namespace details

  template <typename T>
  concept arithmetic_or_enum = std::integral<T> || std::floating_point<T> || std::is_enum_v<T>;

  template <typename T>
  concept tuple_like = requires { typename std::tuple_size<T>::type; };

  template <typename T>
  concept range_constructible_container = requires(T t) { T(std::move(t)); T(t.begin(), t.end()); typename T::value_type; t = {}; };

  template <typename T>
  concept map_range_constructible_container =
    range_constructible_container<T> &&
    requires { typename T::key_type; typename T::mapped_type; } &&
  std::convertible_to<std::pair<typename T::key_type, typename T::mapped_type>, typename T::value_type>;

  template <typename T>
  concept array_like = details::is_array_like<T>::value;

  template <typename T>
  concept optional_like =
    std::convertible_to<typename T::value_type, T> &&std::default_initializable<T> &&requires(T t)
  {
    typename T::value_type;
    t.operator bool();
    { t.value() } -> std::convertible_to<typename T::value_type>;
  };

  namespace details
  {
    template <typename T, size_t ... idx>
    constexpr bool all_variant_indexes_good(std::index_sequence<idx...>)
    {
      return (std::convertible_to<std::variant_alternative_t<idx, T>, T> && ...);
    }
  }

  template <typename T>
  concept variant_like = requires(T t)
  {
    { t.index() } -> std::convertible_to<size_t>;
    std::visit([](const auto &) {}, t);
    std::variant_size_v<T>;
    details::all_variant_indexes_good<T>(std::make_index_sequence<std::variant_size_v<T>>());
  } &&
    details::all_variant_indexes_good<T>(std::make_index_sequence<std::variant_size_v<T>>());

  template <typename T>
  concept BasicSerializable = arithmetic_or_enum<T> || array_like<T> || range_constructible_container<T> || tuple_like<T>;

  namespace details
  {
    template <typename It>
    struct iterator_underlying
    {
      using value_type = typename std::iterator_traits<It>::value_type;
    };

    template <typename It>
      requires requires { typename It::container_type; }
    struct iterator_underlying<It>
    {
      using value_type = typename It::container_type::value_type;
    };

    template <typename It>
      requires requires { typename It::enki_value_type; }
    struct iterator_underlying<It>
    {
      using value_type = typename It::enki_value_type;
    };

    template <typename It>
    using iterator_underlying_t = typename iterator_underlying<It>::value_type;
  }

  template <typename It>
  concept ByteDataOutputIterator =
    std::input_or_output_iterator<It> &&
    (sizeof(details::iterator_underlying_t<It>) == 1) &&
    requires (It it) { *it = static_cast<details::iterator_underlying_t<It>>(std::byte{}); } &&
    requires (It it, It it2) { it2 = it; };

  template <typename It>
  concept ByteDataInputIterator =
    std::input_iterator<It> &&
    (sizeof(details::iterator_underlying_t<It>) == 1) &&
    requires (It it) { static_cast<std::byte>(*it); } &&
    requires (It it, It it2) { it2 = it; };
}

namespace enki
{
  struct WrapperBase;
}

namespace enki::concepts
{
  template <typename T, typename MW>
  concept proper_member_wrapper =
    std::derived_from<MW, ::enki::WrapperBase> &&
    std::derived_from<T, typename MW::class_type> &&
    requires { typename MW::value_type; MW::getter; MW::setter; };

  namespace details
  {
    template <auto p>
    struct member_pointer : std::false_type {};

    template <typename T, typename M, M T:: *p>
    struct member_pointer<p> : std::true_type
    {
      using class_type = T;
      using value_type = M;
    };
  }

  template <typename T, auto p>
  concept class_member =
    proper_member_wrapper<T, decltype(p)> ||
    (details::member_pointer<p>::value &&
      std::same_as<typename details::member_pointer<p>::class_type, T>);
}

#endif // ENKI_CONCEPTS_HPP
