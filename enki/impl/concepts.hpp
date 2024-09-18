#ifndef ENKI_CONCEPTS_HPP
#define ENKI_CONCEPTS_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace enki::concepts
{
  namespace detail
  {
    template <typename T>
    struct is_array_like : std::false_type // NOLINT
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
  } // namespace detail

  template <typename T>
  concept arithmetic_or_enum = std::integral<T> || std::floating_point<T> || std::is_enum_v<T>;

  template <typename T>
  concept tuple_like = requires { typename std::tuple_size<T>::type; };

  template <typename T>
  concept range_constructible_container = requires(T t) {
    T(std::move(t));
    T(t.begin(), t.end());
    typename T::value_type;
    t = {};
  };

  template <typename T>
  concept map_range_constructible_container =
    range_constructible_container<T> &&
    requires {
      typename T::key_type;
      typename T::mapped_type;
    } &&
    std::convertible_to<
      std::pair<typename T::key_type, typename T::mapped_type>,
      typename T::value_type>;

  template <typename T>
  concept array_like = detail::is_array_like<T>::value;

  template <typename T>
  concept optional_like = std::convertible_to<typename T::value_type, T> &&
                          std::default_initializable<T> && requires(T t) {
                            typename T::value_type;
                            t.operator bool();
                            { t.value() } -> std::convertible_to<typename T::value_type>;
                            { *t } -> std::convertible_to<typename T::value_type>;
                          };

  namespace detail
  {
    template <typename T, size_t... idx>
    constexpr bool allVariantIndexesGood(std::index_sequence<idx...>)
    {
      return (std::convertible_to<std::variant_alternative_t<idx, T>, T> && ...);
    }
  } // namespace detail

  template <typename T>
  concept variant_like = requires(T t) {
    { t.index() } -> std::convertible_to<size_t>;
    std::visit([](const auto &) {}, t);
    std::variant_size_v<T>;
    detail::allVariantIndexesGood<T>(std::make_index_sequence<std::variant_size_v<T>>());
  } && detail::allVariantIndexesGood<T>(std::make_index_sequence<std::variant_size_v<T>>());

  namespace detail
  {
    template <typename It>
    struct IteratorUnderlying
    {
      using value_type = typename std::iterator_traits<It>::value_type; // NOLINT
    };

    template <typename It>
      requires requires { typename It::container_type; }
    struct IteratorUnderlying<It>
    {
      using value_type = typename It::container_type::value_type; // NOLINT
    };

    template <typename It>
      requires requires { typename It::enki_value_type; }
    struct IteratorUnderlying<It>
    {
      using value_type = typename It::enki_value_type; // NOLINT
    };

    template <typename It>
    using iterator_underlying_t = typename IteratorUnderlying<It>::value_type; // NOLINT
  } // namespace detail

  template <typename It>
  concept ByteDataOutputIterator =
    std::input_or_output_iterator<It> && (sizeof(detail::iterator_underlying_t<It>) == 1) &&
    requires(It it) { *it = static_cast<detail::iterator_underlying_t<It>>(std::byte{}); } &&
    requires(It it, It it2) { it2 = it; };

  template <typename It>
  concept ByteDataInputIterator =
    std::input_iterator<It> && (sizeof(detail::iterator_underlying_t<It>) == 1) &&
    requires(It it) { static_cast<std::byte>(*it); } && requires(It it, It it2) { it2 = it; };

  template <typename It>
  concept ByteDataIterator = ByteDataInputIterator<It> || ByteDataOutputIterator<It>;
} // namespace enki::concepts

namespace enki
{
  struct WrapperBase;
}

namespace enki::concepts
{
  template <typename T, typename MW>
  concept proper_member_wrapper = std::derived_from<MW, ::enki::WrapperBase> &&
                                  std::derived_from<T, typename MW::class_type> && requires {
                                    typename MW::value_type;
                                    MW::getter;
                                    MW::setter;
                                  };

  namespace detail
  {
    template <auto p>
    struct MemberPointer : std::false_type
    {
    };

    template <typename T, typename M, M T::*p>
    struct MemberPointer<p> : std::true_type
    {
      using class_type = T; // NOLINT
      using value_type = M; // NOLINT
    };
  } // namespace detail

  template <typename T, auto p>
  concept class_member = proper_member_wrapper<T, decltype(p)> ||
                         (detail::MemberPointer<p>::value &&
                          std::derived_from<T, typename detail::MemberPointer<p>::class_type>);

  namespace detail
  {
    template <typename T, size_t... idx>
    constexpr bool isTupleOfClassMembers(std::index_sequence<idx...>)
    {
      return (class_member<T, std::get<idx>(T::EnkiSerial::members)> && ...);
    }
  } // namespace detail

  template <typename T>
  concept custom_static_serializable =
    requires(T inst) { std::tuple_size<decltype(T::EnkiSerial::members)>::value; } &&
    (std::tuple_size_v<decltype(T::EnkiSerial::members)> >= 1) &&
    detail::isTupleOfClassMembers<T>(
      std::make_index_sequence<std::tuple_size_v<decltype(T::EnkiSerial::members)>>());

  template <typename T>
  concept BasicSerializable =
    arithmetic_or_enum<T> || array_like<T> || range_constructible_container<T> || tuple_like<T> ||
    custom_static_serializable<T>;
} // namespace enki::concepts

#endif // ENKI_CONCEPTS_HPP
