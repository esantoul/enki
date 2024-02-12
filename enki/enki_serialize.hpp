#ifndef ENKI_ENKI_SERIALIZE_HPP
#define ENKI_ENKI_SERIALIZE_HPP

#include "enki/impl/success.hpp"
#include "enki/impl/concepts.hpp"

namespace enki
{
  template <typename T, typename Writer>
  Success serialize(const T &value, Writer &w)
  {
    // All the logics for value decomposition is here

    if constexpr (requires (Writer & w) { w.write(value)->Success; })
    {

    }
  }
} // namespace enki

#endif // ENKI_ENKI_SERIALIZE_HPP
