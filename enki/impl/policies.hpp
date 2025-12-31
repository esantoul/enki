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
  struct forward_compat_t
  {
  };

  inline constexpr forward_compat_t forward_compat{};

} // namespace enki

#endif // ENKI_POLICIES_HPP
