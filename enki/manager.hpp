#ifndef ENKI_MANAGER_HPP
#define ENKI_MANAGER_HPP

#include <algorithm>
#include <array>
#include <unordered_map>
#include <iterator>

#include "enki/impl/type_info.hpp"
#include "enki/impl/concepts.hpp"
#include "enki/impl/cstr.hpp"
#include "enki/impl/serdes_engine.hpp"
#include "enki/impl/any_byte_iterator.hpp"

namespace enki
{
  template <typename T, typename M>
  struct MemberWrapper
  {
    using parent_type = T;
    constexpr MemberWrapper(M T:: *p) : pmem(p) {}
    M T:: *pmem;
  };

  template <template <typename, typename, typename...> typename Container = std::unordered_map>
  class Manager : public details::SerDesEngine<Manager<Container>>
  {
  private:
    using ParentEngine = details::SerDesEngine<Manager<Container>>;

  public:
    constexpr Manager() = default;

    template <typename T, MemberWrapper... mem>
    constexpr void Register()
    {
      static_assert((std::same_as<T, typename decltype(mem)::parent_type> && ...), "Members 'mem' must be members from type T");
      mConverters[Type::get_index<T>()] = &kSpecializedHandler<T, mem.pmem...>;
    }

    template <typename T, concepts::ByteDataOutputIterator It>
    constexpr std::pair<Result, It> Serialize(const T &val, It out) const
    {
      if constexpr (concepts::BasicSerializable<T>)
      {
        return ParentEngine::Serialize(val, out);
      }
      else
      {
        if (auto it = mConverters.find(Type::get_index<T>()); it != mConverters.end())
        {
          return {static_cast<const TypeSerDesHandler<T>*>(it->second)->Serialize(this, val, AnyByteOutputIt::Ref(out)), out};
        }
        else
        {
          return {serial_error<T>.begin(), out};
        }
      }
    }

    template <typename T, concepts::ByteDataInputIterator It>
    constexpr std::pair<Result, It> Deserialize(T &val, It in) const
    {
      if constexpr (concepts::BasicSerializable<T>)
      {
        return ParentEngine::Deserialize(val, in);
      }
      else
      {
        if (auto it = mConverters.find(Type::get_index<T>()); it != mConverters.end())
        {
          auto res = static_cast<const TypeSerDesHandler<T>*>(it->second)->Deserialize(this, val, in);
          std::advance(in, res.size());
          return {res, in};
        }
        else
        {
          return {deserial_error<T>.begin(), in};
        }
      }
    }

    template <typename T>
    constexpr Result NumBytes(const T &val) const
    {
      if constexpr (concepts::BasicSerializable<T>)
      {
        return ParentEngine::NumBytes(val);
      }
      else
      {
        if (auto it = mConverters.find(Type::get_index<T>()); it != mConverters.end())
        {
          return static_cast<const TypeSerDesHandler<T>*>(it->second)->NumBytes(this, val);
        }
        else
        {
          return serial_error<T>.begin();
        }
      }
    }

  private:
    template <typename T>
    static constexpr auto serial_error = CStrConcat<"Type ", get_type_name<T>(), " has not been registered for serialization">();
    template <typename T>
    static constexpr auto deserial_error = CStrConcat<"Type ", get_type_name<T>(), " has not been registered for deserialization">();

    class BaseCustomSerDesHandler
    {
    public:
      constexpr virtual ~BaseCustomSerDesHandler() = default;
    };

    template <typename T>
    class TypeSerDesHandler : public BaseCustomSerDesHandler
    {
    public:
      constexpr virtual Result Serialize(const Manager *, const T &, AnyByteOutputIt) const = 0;
      constexpr virtual Result Deserialize(const Manager *, T &, AnyByteInputIt) const = 0;
      constexpr virtual Result NumBytes(const Manager *, const T &) const = 0;
    };

    template <typename T, auto ... memptrs>
    class SpecializedTypeSerDesHandler : public TypeSerDesHandler<T>
    {
    public:
      constexpr ~SpecializedTypeSerDesHandler() = default;

      constexpr Result Serialize(const Manager *pMgr, const T &val, AnyByteOutputIt out) const final
      {
        Result res{};
        ([pMgr, &res, &out, &val] {
          auto [r, _] = pMgr->Serialize(val.*memptrs, out);
          // out underlying iterator is auto updated because out holds a reference to the original output iterator
          return static_cast<bool>(res.update(r));
        }() && ...);
        return res;
      }

      constexpr Result Deserialize(const Manager *pMgr, T &val, AnyByteInputIt in) const final
      {
        Result res{};
        ([pMgr, &res, &in, &val] {
          auto [r, _] = pMgr->Deserialize(val.*memptrs, in);
          std::advance(in, r.size());
          return static_cast<bool>(res.update(r));
        }() && ...);
        return res;
      }

      constexpr virtual Result NumBytes(const Manager *pMgr, const T &val) const final
      {
        Result res{};
        static_cast<void>((static_cast<bool>(res.update(pMgr->NumBytes(val.*memptrs))) && ...));
        return res;
      }
    };

    template <typename T, auto... memptrs>
    static constexpr auto kSpecializedHandler = SpecializedTypeSerDesHandler<T, memptrs...>{};

    Container<Type::index_t, const BaseCustomSerDesHandler *> mConverters{};
  };
} // namespace enki

#endif // ENKI_MANAGER_HPP
