#ifndef ENKI_POLICIES_HPP
#define ENKI_POLICIES_HPP

#include <concepts>

namespace enki
{
  namespace detail
  {
    struct PolicyTag
    {
    };
  } // namespace detail

  template <typename T>
  concept policy = std::derived_from<T, detail::PolicyTag>;

  /// Strict policy (default) - error on unknown variant indices
  /// Usage: enki::BinWriter writer; or enki::BinWriter writer(enki::strict);
  struct strict_t : detail::PolicyTag // NOLINT
  {
  };

  inline constexpr strict_t strict{}; // NOLINT

  /// Forward compatibility policy - skip unknown variants, set to monostate
  /// Usage: enki::BinWriter writer(enki::forward_compatible);
  /// See docs/forward-compatibility.md for details
  struct forward_compatible_t : detail::PolicyTag // NOLINT
  {
  };

  inline constexpr forward_compatible_t forward_compatible{}; // NOLINT

} // namespace enki

#endif // ENKI_POLICIES_HPP
