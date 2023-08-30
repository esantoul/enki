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
  template <template <typename, typename, typename...> typename Container = std::unordered_map>
  class Manager : public details::SerDesEngine<Manager<Container>>
  {
  private:
    using ParentEngine = details::SerDesEngine<Manager<Container>>;

  public:
    constexpr Manager() = default;

    template <typename T, auto... mem>
      requires (concepts::class_member<T, mem> && ...)
    constexpr void Register()
    {
      mConverters[Type::get_index<T>()] = &kSpecializedHandler<T, mem...>;
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

    template <typename T, auto ... mem>
    class SpecializedTypeSerDesHandler : public TypeSerDesHandler<T>
    {
    public:
      constexpr ~SpecializedTypeSerDesHandler() = default;

      constexpr Result Serialize(const Manager *pMgr, const T &inst, AnyByteOutputIt out) const final
      {
        Result res{};
        // out underlying iterator is auto updated because out holds a reference to the original output iterator
        // see call to this very method
        static_cast<void>((static_cast<bool>(res.update(Serialize_one<mem>(pMgr, inst, out))) && ...));
        return res;
      }

      constexpr Result Deserialize(const Manager *pMgr, T &inst, AnyByteInputIt in) const final
      {
        Result res{};
        static_cast<void>(([this, pMgr, &res, &in, &inst] {
          auto r = Deserialize_one<mem>(pMgr, inst, in);
          std::advance(in, r.size());
          return static_cast<bool>(res.update(r));
        }() && ...));
        return res;
      }

      constexpr Result NumBytes(const Manager *pMgr, const T &inst) const final
      {
        Result res{};
        static_cast<void>((static_cast<bool>(res.update(NumBytes_one<mem>(pMgr, inst))) && ...));
        return res;
      }

    private:
      template <auto onemem>
      constexpr Result Serialize_one(const Manager *pMgr, const T &inst, AnyByteOutputIt out) const
      {
        auto [r, _] = pMgr->Serialize(inst.*onemem, out);
        // out underlying iterator is auto updated because out holds a reference to the original output iterator
        return r;
      }

      template <auto onemem>
        requires concepts::proper_member_wrapper<T, decltype(onemem)>
      constexpr Result Serialize_one(const Manager *pMgr, const T &inst, AnyByteOutputIt out) const
      {
        auto [r, _] = pMgr->Serialize(onemem.getter(inst), out);
        // out underlying iterator is auto updated because out holds a reference to the original output iterator
        return r;
      }

      template <auto onemem>
      constexpr Result Deserialize_one(const Manager *pMgr, T &inst, AnyByteInputIt in) const
      {
        auto [r, _] = pMgr->Deserialize(inst.*onemem, in);
        return r;
      }

      template <auto onemem>
        requires concepts::proper_member_wrapper<T, decltype(onemem)>
      constexpr Result Deserialize_one(const Manager *pMgr, T &inst, AnyByteInputIt in) const
      {
        typename decltype(onemem)::value_type temp{};
        auto [r, _] = pMgr->Deserialize(temp, in);
        onemem.setter(inst, temp);
        return r;
      }

      template <auto onemem>
      constexpr Result NumBytes_one(const Manager *pMgr, const T &inst) const
      {
        return pMgr->NumBytes(inst.*onemem);
      }

      template <auto onemem>
        requires concepts::proper_member_wrapper<T, decltype(onemem)>
      constexpr Result NumBytes_one(const Manager *pMgr, const T &inst) const
      {
        return pMgr->NumBytes(onemem.getter(inst));
      }
    };

    template <typename T, auto... memptrs>
    static constexpr auto kSpecializedHandler = SpecializedTypeSerDesHandler<T, memptrs...>{};

    Container<Type::index_t, const BaseCustomSerDesHandler *> mConverters{};
  };
} // namespace enki

#endif // ENKI_MANAGER_HPP
