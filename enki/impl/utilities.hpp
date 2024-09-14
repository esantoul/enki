#ifndef ENKI_UTILITIES_HPP
#define ENKI_UTILITIES_HPP

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

#define ENKIWRAP(T, m)                                                                             \
  enki::MemberWrapper<                                                                             \
    T,                                                                                             \
    [](const T &inst) { return inst.m; },                                                          \
    [](T &inst, auto &val) { inst.m = val; }>                                                      \
  {                                                                                                \
  }

#endif // ENKI_UTILITIES_HPP
