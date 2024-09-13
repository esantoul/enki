#ifndef ENKI_MANAGER_HPP
#define ENKI_MANAGER_HPP

#include <iterator>
#include <unordered_map>

#include "enki/impl/any_byte_iterator.hpp"
#include "enki/impl/concepts.hpp"
#include "enki/impl/cstr.hpp"
#include "enki/impl/serdes_engine.hpp"
#include "enki/impl/type_info.hpp"

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
      requires(concepts::class_member<T, mem> && ...)
    constexpr void registerType()
    {
      mConverters[Type::getIndex<T>()] = &kSpecializedHandler<T, mem...>;
    }

    template <typename T>
    constexpr void unregisterType()
    {
      mConverters.erase(Type::getIndex<T>());
    }

    template <typename T, concepts::ByteDataOutputIterator It>
    constexpr Success<It> serialize(const T &val, It out) const
    {
      if constexpr (concepts::BasicSerializable<T>)
      {
        return ParentEngine::serialize(val, out);
      }
      else
      {
        if (auto it = mConverters.find(Type::getIndex<T>()); it != mConverters.end())
        {
          auto res = static_cast<const TypeSerDesHandlerInterface<T> *>(it->second)
                       ->serialize(this, val, AnyByteOutputIt::ref(out));
          if (res)
          {
            return {res.size(), out};
          }
          else
          {
            return {res.error(), out};
          }
        }
        else
        {
          return {kSerialError<T>.begin(), out};
        }
      }
    }

    template <typename T, concepts::ByteDataInputIterator It>
    constexpr Success<It> deserialize(T &val, It in) const
    {
      if constexpr (concepts::BasicSerializable<T>)
      {
        return ParentEngine::deserialize(val, in);
      }
      else
      {
        if (auto it = mConverters.find(Type::getIndex<T>()); it != mConverters.end())
        {
          auto res = static_cast<const TypeSerDesHandlerInterface<T> *>(it->second)
                       ->deserialize(this, val, in);
          std::advance(in, res.size());
          if (res)
          {
            return {res.size(), in};
          }
          else
          {
            return {res.error(), in};
          }
        }
        else
        {
          return {kDeserialError<T>.begin(), in};
        }
      }
    }

    template <typename T>
    constexpr Success<void> numBytes(const T &val) const
    {
      if constexpr (concepts::BasicSerializable<T>)
      {
        return ParentEngine::numBytes(val);
      }
      else
      {
        if (auto it = mConverters.find(Type::getIndex<T>()); it != mConverters.end())
        {
          return static_cast<const TypeSerDesHandlerInterface<T> *>(it->second)
            ->numBytes(this, val);
        }
        else
        {
          return kSerialError<T>.begin();
        }
      }
    }

  private:
    template <typename T>
    static constexpr auto kSerialError =
      cStrConcat<"Type ", getTypeName<T>(), " has not been registered for serialization">();
    template <typename T>
    static constexpr auto kDeserialError =
      cStrConcat<"Type ", getTypeName<T>(), " has not been registered for deserialization">();

    class BaseCustomSerDesHandler
    {
    public:
      constexpr virtual ~BaseCustomSerDesHandler() = default;
    };

    template <typename T>
    class TypeSerDesHandlerInterface : public BaseCustomSerDesHandler
    {
    public:
      constexpr virtual Success<AnyByteOutputIt>
      serialize(const Manager *, const T &, AnyByteOutputIt) const = 0;
      constexpr virtual Success<AnyByteInputIt>
      deserialize(const Manager *, T &, AnyByteInputIt) const = 0;
      constexpr virtual Success<void> numBytes(const Manager *, const T &) const = 0;
    };

    template <typename T, auto... mem>
    class SpecializedTypeSerDesHandler : public TypeSerDesHandlerInterface<T>
    {
    public:
      constexpr ~SpecializedTypeSerDesHandler() = default;

      constexpr Success<AnyByteOutputIt>
      serialize(const Manager *pMgr, const T &inst, AnyByteOutputIt out) const final
      {
        static_cast<void>(pMgr); // avoid unused variable warning
        Success<AnyByteOutputIt> res(static_cast<size_t>(0), out);
        // out underlying iterator is auto updated because out holds a reference to the original
        // output iterator see call to this very method
        static_cast<void>(
          (static_cast<bool>(res.update(serializeOne<mem>(pMgr, inst, out))) && ...));
        return res;
      }

      constexpr Success<AnyByteInputIt>
      deserialize(const Manager *pMgr, T &inst, AnyByteInputIt in) const final
      {
        static_cast<void>(pMgr); // avoid unused variable warning
        Success<AnyByteInputIt> res(static_cast<size_t>(0), in);
        static_cast<void>(([this, pMgr, &res, &in, &inst] {
          auto r = deserializeOne<mem>(pMgr, inst, in);
          std::advance(in, r.size());
          return static_cast<bool>(res.update(r));
        }() && ...));
        return res;
      }

      constexpr Success<void> numBytes(const Manager *pMgr, const T &inst) const final
      {
        static_cast<void>(pMgr); // avoid unused variable warning
        Success<void> res{};
        static_cast<void>((static_cast<bool>(res.update(numBytesOne<mem>(pMgr, inst))) && ...));
        return res;
      }

    private:
      template <auto onemem>
      constexpr Success<AnyByteOutputIt>
      serializeOne(const Manager *pMgr, const T &inst, AnyByteOutputIt out) const
      {
        return pMgr->serialize(inst.*onemem, out);
        // out underlying iterator is auto updated because out holds a reference to the original
        // output iterator
      }

      template <auto onemem>
        requires concepts::proper_member_wrapper<T, decltype(onemem)>
      constexpr Success<AnyByteOutputIt>
      serializeOne(const Manager *pMgr, const T &inst, AnyByteOutputIt out) const
      {
        return pMgr->serialize(onemem.getter(inst), out);
        // out underlying iterator is auto updated because out holds a reference to the original
        // output iterator
      }

      template <auto onemem>
      constexpr Success<AnyByteInputIt>
      deserializeOne(const Manager *pMgr, T &inst, AnyByteInputIt in) const
      {
        return pMgr->deserialize(inst.*onemem, in);
      }

      template <auto onemem>
        requires concepts::proper_member_wrapper<T, decltype(onemem)>
      constexpr Success<AnyByteInputIt>
      deserializeOne(const Manager *pMgr, T &inst, AnyByteInputIt in) const
      {
        typename decltype(onemem)::value_type temp{};
        auto r = pMgr->deserialize(temp, in);
        onemem.setter(inst, temp);
        return r;
      }

      template <auto onemem>
      constexpr Success<void> numBytesOne(const Manager *pMgr, const T &inst) const
      {
        return pMgr->numBytes(inst.*onemem);
      }

      template <auto onemem>
        requires concepts::proper_member_wrapper<T, decltype(onemem)>
      constexpr Success<void> numBytesOne(const Manager *pMgr, const T &inst) const
      {
        return pMgr->numBytes(onemem.getter(inst));
      }
    };

    template <typename T, auto... memptrs>
    static constexpr auto kSpecializedHandler = SpecializedTypeSerDesHandler<T, memptrs...>{};

    Container<Type::Index, const BaseCustomSerDesHandler *> mConverters{};
  };
} // namespace enki

#endif // ENKI_MANAGER_HPP
