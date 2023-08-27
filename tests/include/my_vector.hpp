#ifndef MY_VECTOR_HPP
#define MY_VECTOR_HPP

template <typename T>
class MyVector
{
public:
  using value_type = T;

  constexpr MyVector() = default;
  constexpr MyVector(size_t n) : mSize(n), pData(new T[mSize]{}) {}

  template <std::forward_iterator It>
  constexpr MyVector(It b, It e) : mSize(std::distance(b, e)), pData(new T[mSize])
  {
    std::copy(b, e, pData);
  }

  constexpr MyVector(std::initializer_list<T> il) : mSize(il.size()), pData(new T[mSize])
  {
    std::copy(il.begin(), il.end(), pData);
  }

  constexpr MyVector(const MyVector &other)
    :
    MyVector(other.begin(), other.end())
  {
  }

  constexpr MyVector(MyVector &&other) : mSize(other.mSize), pData(other.pData)
  {
    other.mSize = 0;
    other.pData = nullptr;
  }

  constexpr MyVector &operator=(const MyVector &other)
  {
    if (pData)
      delete[] pData;
    mSize = other.mSize;
    pData = new T[mSize];
    std::copy(other.begin(), other.end(), pData);
    return *this;
  }

  constexpr MyVector &operator=(MyVector &&other)
  {
    if (pData)
      delete[] pData;
    mSize = other.mSize;
    pData = other.pData;
    other.mSize = 0;
    other.pData = nullptr;
    return *this;
  }

  constexpr ~MyVector() { if (pData) delete[] pData; }

  constexpr T *begin() { return pData; }
  constexpr T *end() { return pData + mSize; }

  constexpr T *begin() const { return pData; }
  constexpr T *end() const { return pData + mSize; }

private:
  size_t mSize = 0;
  T *pData = nullptr;
};

#endif // MY_VECTOR_HPP
