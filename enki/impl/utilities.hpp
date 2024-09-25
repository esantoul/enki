#ifndef ENKI_UTILITIES_HPP
#define ENKI_UTILITIES_HPP

#include <bit>

#include "enki/impl/concepts.hpp"

namespace enki
{
  namespace detail
  {
    template <concepts::range_constructible_container T>
    struct AssignableValue
    {
      using type = typename T::value_type; // NOLINT
    };

    template <concepts::map_range_constructible_container T>
    struct AssignableValue<T>
    {
      using type = std::pair<typename T::key_type, typename T::mapped_type>; // NOLINT
    };

    template <concepts::range_constructible_container T>
    using assignable_value_t = typename AssignableValue<T>::type; // NOLINT
  } // namespace detail

  struct WrapperBase
  {
  };

  template <typename T, auto getterFun, auto setterFun>
  struct MemberWrapper : WrapperBase
  {
  public:
    using class_type = T;                                                             // NOLINT
    using value_type = typename std::invoke_result_t<decltype(getterFun), const T &>; // NOLINT

    static constexpr value_type getter(const T &inst)
    {
      return getterFun(inst);
    }

    static constexpr void setter(T &inst, const value_type &val)
    {
      setterFun(inst, val);
    }
  };
} // namespace enki

#define ENKIWRAP(Type, member)                                                                     \
  enki::MemberWrapper<                                                                             \
    Type,                                                                                          \
    [](const Type &inst) { return inst.member; },                                                  \
    [](Type &inst, const auto &val) { inst.member = val; }>                                        \
  {                                                                                                \
  }

#define ENKIWRAP_CAST(Type, member, ToType)                                                        \
  enki::MemberWrapper<                                                                             \
    Type,                                                                                          \
    [](const Type &inst) { return static_cast<ToType>(inst.member); },                             \
    [](Type &inst, const ToType &val) { inst.member = static_cast<decltype(Type::member)>(val); }> \
  {                                                                                                \
  }

#define ENKIWRAP_BITCAST(Type, member)                                                             \
  enki::MemberWrapper<                                                                             \
    Type,                                                                                          \
    [](const Type &inst) {                                                                         \
      using TargetType = std::array<std::byte, sizeof(Type::member)>;                              \
      return std::bit_cast<TargetType>(inst.member);                                               \
    },                                                                                             \
    [](Type &inst, const auto &val) { inst.member = std::bit_cast<decltype(Type::member)>(val); }> \
  {                                                                                                \
  }
#endif // ENKI_UTILITIES_HPP
