
#include "enki/manager.hpp"
#include "enki/base_engine.hpp"

#include "array_map.hpp"
#include "my_vector.hpp"

#include <unordered_map>
#include <iostream>
#include <numeric>
#include <initializer_list>
#include <cstdint>

struct SBase
{
  int32_t a : 16 = 0;
};

struct S : SBase
{
  float b = 0.f;
};

template <typename K, typename T>
using MyArrayMap = ArrayMap<K, T, 10>;

static constexpr auto mgr = []() {
  enki::Manager<MyArrayMap> ret;
  ret.Register<S, &S::b, ENKIREG(SBase, a)>();
  return ret;
}();

static const auto kResult = []() {
  std::array<std::byte, 128> workbuf{};

  S in{{20}, 30.f};
  mgr.Serialize(in, workbuf.begin()).first.or_throw();

  S ret;
  mgr.Deserialize(ret, workbuf.begin()).first.or_throw();

  return ret;
}();

static constexpr std::array<int, 10> container_test = []() {
  MyVector ctn = {1, 2, 9, 42};

  std::array<std::byte, 64> temp{};

  auto [res, pastEndIt] = enki::BaseEngine::Serialize(ctn, temp.begin());
  res.or_throw();

  MyVector<int> ctn2;

  enki::BaseEngine::Deserialize(ctn2, temp.begin()).first.or_throw();

  std::array<int, 10> ret{};
  std::copy(ctn2.begin(), ctn2.end(), ret.begin());

  return ret;
}();

static constexpr std::array<int, 10> c_array_test = []() {
  int arr[10]{6, 7, -2, 4, -12};

  std::array<std::byte, 64> temp{};

  enki::BaseEngine::Serialize(arr, temp.begin()).first.or_throw();

  int arr2[10]{};

  enki::BaseEngine::Deserialize(arr2, temp.begin()).first.or_throw();

  std::array<int, 10> ret{};
  std::copy(std::begin(arr2), std::end(arr2), ret.begin());

  return ret;
}();

static constexpr size_t kNumBytesTest = enki::BaseEngine::NumBytes(std::tuple<char, std::array<std::byte, 9>, MyVector<int32_t>>{0, {}, (3)}).size();
static_assert(kNumBytesTest == 1 + 9 + 4 + 3 * 4);

struct PSPRawData
{
  using intensity_map = std::unordered_map<int, std::array<std::array<uint16_t, 40>, 40>>;
  intensity_map intensities;
};

#include <vector>
#include <memory>
#include <random>

int get_random()
{
  static thread_local std::random_device dev;
  static thread_local std::mt19937 rng(dev());
  return rng();
}

int main()
{
  std::cout << "Compile time computed value: " << kResult.a << '\n';
  std::array<int, 8> arr{};
  std::iota(arr.begin(), arr.end(), 1);

  for (auto el : arr)
    std::cout << el << ' ';
  std::cout << std::endl;

  decltype(arr) arr2{};

  std::array<std::byte, 32> serialized{};

  mgr.Serialize(arr, serialized.begin()).first.or_throw();
  mgr.Deserialize(arr2, serialized.begin()).first.or_throw();

  for (auto el : arr2)
    std::cout << el << ' ';
  std::cout << std::endl;

  for (auto el : container_test)
    std::cout << el << ' ';
  std::cout << '\n';

  for (auto el : c_array_test)
    std::cout << el << ' ';
  std::cout << '\n';

  enki::Manager mgr2;

  std::array<std::pair<int, std::string>, 2> inits{
    std::pair<int, std::string>{1, "Bonjour"},
    std::pair<int, std::string>{2, "Coucou"}
  };

  std::unordered_map<int, std::string> test(inits.begin(), inits.end());
  auto big_array = std::make_unique<std::array<int, 1024>>();
  std::for_each(big_array.get()->begin(), big_array.get()->end(), [](auto &el) { el = get_random(); });

  std::vector<std::byte> temp(1024 * 16);
  enki::Result serialization_result = mgr2.Serialize(test, temp.data()).first.or_throw();

  serialization_result.update(mgr2.Serialize(*big_array.get(), temp.data() + serialization_result.size()).first.or_throw());

  decltype(test) cp{};
  auto big_array_2 = std::make_unique<decltype(big_array)::element_type>();

  enki::Result deserialization_result = mgr2.Deserialize(cp, temp.data()).first.or_throw();

  deserialization_result.update(mgr2.Deserialize(*big_array_2.get(), temp.data() + deserialization_result.size()).first.or_throw());

  if (serialization_result.size() != deserialization_result.size())
    std::cerr << "Serialize and Deserialize sizes not equal!\n";
  else
    std::cout << "Serialized into " << serialization_result.size() << " bytes\n";

  for (auto el : cp)
    std::cout << el.first << ": " << el.second << '\n';

  if (std::equal(big_array.get()->begin(), big_array.get()->end(), big_array_2.get()->begin()))
    std::cout << "Big array correctly handled!";
  else
    std::cerr << "Big array serialization/deserialization error!";

  std::cout << std::endl;
  std::cerr << std::endl;
  return 0;
}
