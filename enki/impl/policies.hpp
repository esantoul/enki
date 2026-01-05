#ifndef ENKI_POLICIES_HPP
#define ENKI_POLICIES_HPP

namespace enki
{
  /// Strict policy - error on unknown variant indices (current behavior)
  struct strict_t
  {
  };

  inline constexpr strict_t strict{};

  /// Forward compatibility policy - skip unknown variants, set to monostate
  struct forward_compatible_t
  {
  };

  inline constexpr forward_compatible_t forward_compatible{};

} // namespace enki

#endif // ENKI_POLICIES_HPP
