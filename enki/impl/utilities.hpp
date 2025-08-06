#ifndef ENKI_UTILITIES_HPP
#define ENKI_UTILITIES_HPP

#include <bit>
#include <cstddef>
#include <type_traits>

#include "enki/impl/concepts.hpp"
#include "enki/impl/cstr.hpp"
#include "enki/impl/get_member_name.hpp"

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

    struct WrapperBase
    {
    };

    template <typename T, ::enki::CStr nm, auto getterFun, auto setterFun>
    struct MemberWrapper : public WrapperBase
    {
    public:
      using class_type = T;                                                             // NOLINT
      using value_type = typename std::invoke_result_t<decltype(getterFun), const T &>; // NOLINT

      static constexpr std::string_view name = nm; // NOLINT

      static constexpr value_type getter(const T &inst)
      {
        return getterFun(inst);
      }

      static constexpr void setter(T &inst, const value_type &val)
      {
        setterFun(inst, val);
      }
    };

    struct RegisterBase
    {
    };
  } // namespace detail

  template <auto... members>
  struct Register;

  template <auto member>
    requires std::is_member_object_pointer_v<decltype(member)>
  struct Register<member> : detail::RegisterBase
  {
    using parent = void;                                                             // NOLINT
    using class_type = typename concepts::detail::MemberPointer<member>::class_type; // NOLINT
    using value_type = typename concepts::detail::MemberPointer<member>::value_type; // NOLINT

    static constexpr std::string_view name = getMemberName<member>(); // NOLINT

    static constexpr value_type
    getter(const typename concepts::detail::MemberPointer<member>::class_type &inst)
    {
      return inst.*member;
    }

    static constexpr void
    setter(concepts::detail::MemberPointer<member>::class_type &inst, const auto &val)
    {
      inst.*member = val;
    }

    static constexpr size_t n = 0;     // NOLINT
    static constexpr size_t count = 1; // NOLINT
  };

  template <auto member, auto... members>
    requires std::is_member_object_pointer_v<decltype(member)>
  struct Register<member, members...> : detail::RegisterBase
  {
    using parent = Register<members...>; // NOLINT

    using class_type = typename concepts::detail::MemberPointer<member>::class_type; // NOLINT
    using value_type = typename concepts::detail::MemberPointer<member>::value_type; // NOLINT

    static constexpr std::string_view name = getMemberName<member>(); // NOLINT

    static constexpr value_type
    getter(const typename concepts::detail::MemberPointer<member>::class_type &inst)
    {
      return inst.*member;
    }

    static constexpr void
    setter(concepts::detail::MemberPointer<member>::class_type &inst, const auto &val)
    {
      inst.*member = val;
    }

    static constexpr size_t n = sizeof...(members);         // NOLINT
    static constexpr size_t count = 1 + sizeof...(members); // NOLINT
  };

  template <auto member>
    requires std::derived_from<decltype(member), detail::WrapperBase>
  struct Register<member> : detail::RegisterBase
  {
    using parent = void;                                      // NOLINT
    using class_type = typename decltype(member)::class_type; // NOLINT
    using value_type = typename decltype(member)::value_type; // NOLINT

    static constexpr std::string_view name = decltype(member)::name; // NOLINT

    static constexpr auto getter = decltype(member)::getter; // NOLINT
    static constexpr auto setter = decltype(member)::setter; // NOLINT

    static constexpr size_t n = 0;     // NOLINT
    static constexpr size_t count = 1; // NOLINT
  };

  template <auto member, auto... members>
    requires std::derived_from<decltype(member), detail::WrapperBase>
  struct Register<member, members...> : detail::RegisterBase
  {
    using parent = Register<members...>; // NOLINT

    using class_type = typename decltype(member)::class_type; // NOLINT
    using value_type = typename decltype(member)::value_type; // NOLINT

    static constexpr std::string_view name = decltype(member)::name; // NOLINT

    static constexpr auto getter = decltype(member)::getter; // NOLINT
    static constexpr auto setter = decltype(member)::setter; // NOLINT

    static constexpr size_t n = sizeof...(members);         // NOLINT
    static constexpr size_t count = 1 + sizeof...(members); // NOLINT
  };

  namespace detail
  {
    template <size_t i, typename T>
    struct nth // NOLINT
    {
      using type = // NOLINT
        std::conditional_t<T::n == i, T, typename nth<i, typename T::parent>::type>;
    };

    template <size_t i>
    struct nth<i, void>
    {
      using type = void; // NOLINT
    };

    template <size_t i, typename T>
    using get_nth_register_t = typename nth<T::count - 1 - i, T>::type; // NOLINT
  } // namespace detail
} // namespace enki

#define ENKIWRAP(Type, member)                                                                     \
  enki::detail::MemberWrapper<                                                                     \
    Type,                                                                                          \
    #member,                                                                                       \
    [](const Type &inst) { return inst.member; },                                                  \
    [](Type &inst, const auto &val) { inst.member = val; }>                                        \
  {                                                                                                \
  }

#define ENKIWRAP_CAST(Type, member, ToType)                                                        \
  enki::detail::MemberWrapper<                                                                     \
    Type,                                                                                          \
    #member,                                                                                       \
    [](const Type &inst) { return static_cast<ToType>(inst.member); },                             \
    [](Type &inst, const ToType &val) { inst.member = static_cast<decltype(Type::member)>(val); }> \
  {                                                                                                \
  }

#define ENKIWRAP_BITCAST(Type, member)                                                             \
  enki::detail::MemberWrapper<                                                                     \
    Type,                                                                                          \
    #member,                                                                                       \
    [](const Type &inst) {                                                                         \
      using TargetType = std::array<std::byte, sizeof(Type::member)>;                              \
      return std::bit_cast<TargetType>(inst.member);                                               \
    },                                                                                             \
    [](Type &inst, const auto &val) { inst.member = std::bit_cast<decltype(Type::member)>(val); }> \
  {                                                                                                \
  }
#endif // ENKI_UTILITIES_HPP
