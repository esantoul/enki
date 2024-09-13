#ifndef MY_VECTOR_HPP
#define MY_VECTOR_HPP

#include <cstddef>
#include <iterator>

template <typename T>
class MyVector
{
public:
  using value_type = T; // NOLINT

  constexpr MyVector() = default;

  constexpr MyVector(size_t n) :
    mSize(n),
    mPData(new T[mSize]{})
  {
  }

  template <std::forward_iterator It>
  constexpr MyVector(It b, It e) :
    mSize(std::distance(b, e)),
    mPData(new T[mSize])
  {
    std::copy(b, e, mPData);
  }

  constexpr MyVector(std::initializer_list<T> il) :
    mSize(il.size()),
    mPData(new T[mSize])
  {
    std::copy(il.begin(), il.end(), mPData);
  }

  constexpr MyVector(const MyVector &other) :
    MyVector(other.begin(), other.end())
  {
  }

  constexpr MyVector(MyVector &&other) :
    mSize(other.mSize),
    mPData(other.mPData)
  {
    other.mSize = 0;
    other.mPData = nullptr;
  }

  constexpr MyVector &operator=(const MyVector &other)
  {
    if (mPData)
    {
      delete[] mPData;
    }
    mSize = other.mSize;
    mPData = new T[mSize];
    std::copy(other.begin(), other.end(), mPData);
    return *this;
  }

  constexpr MyVector &operator=(MyVector &&other)
  {
    if (mPData)
    {
      delete[] mPData;
    }
    mSize = other.mSize;
    mPData = other.mPData;
    other.mSize = 0;
    other.mPData = nullptr;
    return *this;
  }

  constexpr ~MyVector()
  {
    if (mPData)
    {
      delete[] mPData;
    }
  }

  constexpr T *begin()
  {
    return mPData;
  }

  constexpr T *end()
  {
    return mPData + mSize;
  }

  constexpr T *begin() const
  {
    return mPData;
  }

  constexpr T *end() const
  {
    return mPData + mSize;
  }

private:
  size_t mSize = 0;
  T *mPData = nullptr;
};

#endif // MY_VECTOR_HPP
