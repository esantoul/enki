#ifndef ENKI_UTILITIES_HPP
#define ENKI_UTILITIES_HPP

#include "enki/impl/concepts.hpp"

namespace enki
{
  namespace details
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
  }

  struct WrapperBase {};

  template <typename T, auto getter_fun, auto setter_fun>
  struct MemberWrapper : WrapperBase
  {
  public:
    using class_type = T;
    using value_type = typename std::invoke_result_t<decltype(getter_fun), const T &>;

    static constexpr value_type getter(const T &inst) { return getter_fun(inst); }
    static constexpr void setter(T &inst, const value_type &val) { setter_fun(inst, val); }
  };
} // namespace enki::details

#define ENKIWRAP(T, m)                                \
  enki::MemberWrapper<T,                                    \
                [](const T& inst) { return inst.m; }, \
                [](T& inst, auto& val) { inst.m = val; }>

#define ENKIREG(T, m) ENKIWRAP(T, m){}

#endif // ENKI_UTILITIES_HPP
