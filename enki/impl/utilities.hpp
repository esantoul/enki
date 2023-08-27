#ifndef ENKI_UTILITIES_HPP
#define ENKI_UTILITIES_HPP

#include "enki/impl/concepts.hpp"

namespace enki::details
{
  template <concepts::range_constructible_container T>
  struct assignable_value
  {
    using type = typename T::value_type;
  };

  template <concepts::map_range_constructible_container T>
  struct assignable_value<T>
  {
    using type = std::pair<typename T::key_type, typename T::mapped_type>;
  };

  template <concepts::range_constructible_container T>
  using assignable_value_t = typename assignable_value<T>::type;
} // namespace enki::details

#endif // ENKI_UTILITIES_HPP
