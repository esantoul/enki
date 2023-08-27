#ifndef ARRAY_MAP_HPP
#define ARRAY_MAP_HPP

#include <cstddef>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <array>

template <typename K, typename T, size_t cty>
class ArrayMap
{
public:
  using key_type = K;
  using mapped_type = T;
  using value_type = std::pair<K, T>;
  using size_type = size_t;
  using iterator = typename std::array<value_type, 1>::iterator;
  using const_iterator = typename std::array<value_type, 1>::const_iterator;

  constexpr size_type size() const { return mSize; }

  constexpr iterator begin() { return mData.begin(); }
  constexpr iterator end() { return mData.begin() + mSize; }
  constexpr const_iterator begin() const { return mData.begin(); }
  constexpr const_iterator end() const { return mData.begin() + mSize; }

  constexpr iterator find(const key_type &k)
  {
    return std::find_if(mData.begin(), mData.begin() + mSize, [k](const value_type &dat) { return dat.first == k; });
  }

  constexpr const_iterator find(const key_type &k) const
  {
    return std::find_if(mData.begin(), mData.begin() + mSize, [k](const value_type &dat) { return dat.first == k; });
  }

  constexpr T &operator[](const key_type &k)
  {
    if (auto it = find(k); it != end())
      return it->second;
    // insert new key
    if (mSize >= cty) [[unlikely]]
      throw std::out_of_range("Trying to insert a new entry on full container");
    const auto idx = mSize;
    ++mSize;
    mData[idx].first = k;
    return mData[idx].second;
  }

public:
  size_type mSize = 0;
  std::array<value_type, cty> mData{};
};

#endif // ARRAY_MAP_HPP
