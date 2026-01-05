#ifndef ENKI_POLICIES_HPP
#define ENKI_POLICIES_HPP

namespace enki
{
  /// Strict policy (default) - error on unknown variant indices
  /// Usage: enki::BinWriter writer; or enki::BinWriter writer(enki::strict);
  struct strict_t
  {
  };

  inline constexpr strict_t strict{};

  /// Forward compatibility policy - skip unknown variants, set to monostate
  /// Usage: enki::BinWriter writer(enki::forward_compatible);
  /// See docs/forward-compatibility.md for details
  struct forward_compatible_t
  {
  };

  inline constexpr forward_compatible_t forward_compatible{};

} // namespace enki

#endif // ENKI_POLICIES_HPP
