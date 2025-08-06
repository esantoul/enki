#include <array>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/enki_deserialize.hpp"
#include "enki/enki_serialize.hpp"
#include "enki/json_reader.hpp"
#include "enki/json_writer.hpp"

// Custom type for demonstrating serialization
struct Person
{
  std::string name;
  int age;
  double height;

  bool operator==(const Person &other) const
  {
    return name == other.name && age == other.age && height == other.height;
  }

  struct EnkiSerial;
};

// Define a complex structure with an array, string, and map
struct ComplexData
{
  std::array<int, 5> numbers;
  std::string description;
  std::map<std::string, double> metrics;

  bool operator==(const ComplexData &other) const
  {
    return numbers == other.numbers && description == other.description && metrics == other.metrics;
  }

  struct EnkiSerial;
};

struct Person::EnkiSerial
{
  using members =
    enki::Register<ENKIWRAP(Person, name), ENKIWRAP(Person, age), ENKIWRAP(Person, height)>;
};

struct ComplexData::EnkiSerial
{
  using members =
    enki::Register<&ComplexData::numbers, &ComplexData::description, &ComplexData::metrics>;
};

// Binary serialization examples
void binarySerializationExamples()
{
  std::cout << "\n=== Binary Serialization Examples ===\n";

  // Arithmetic type with BinWriter/BinReader
  {
    int value = 42;
    int result;

    enki::BinWriter writer;
    enki::serialize(value, writer).or_throw();
    enki::BinReader reader(writer.data());
    enki::deserialize(result, reader).or_throw();
    std::cout << "Arithmetic: Original=" << value << ", Deserialized=" << result << "\n";
  }

  // Array with BinSpanWriter/BinSpanReader
  {
    std::array<float, 3> arr = {1.1f, 2.2f, 3.3f};
    std::array<float, 3> result;

    std::array<std::byte, 100> buffer{}; // Preallocated buffer

    enki::BinSpanWriter writer(buffer);
    auto serRes = enki::serialize(arr, writer).or_throw();

    enki::BinSpanReader reader(std::span<const std::byte>(buffer.data(), serRes.size()));
    enki::deserialize(result, reader).or_throw();
    std::cout << "Array: Original=[" << arr[0] << "," << arr[1] << "," << arr[2]
              << "], Deserialized=[" << result[0] << "," << result[1] << "," << result[2] << "]\n";
  }

  // Vector (range-based) with BinWriter/BinReader
  {
    std::vector<double> vec = {3.14, 2.71, 1.41};
    std::vector<double> result;

    enki::BinWriter writer;
    enki::serialize(vec, writer).or_throw();

    enki::BinReader reader(writer.data());
    enki::deserialize(result, reader).or_throw();
    std::cout << "Vector: Original size=" << vec.size() << ", Deserialized size=" << result.size()
              << "\n";
  }

  // Map with BinWriter/BinReader
  {
    std::map<std::string, int> map = {
      {  "one", 1},
      {  "two", 2},
      {"three", 3}
    };
    std::map<std::string, int> result;

    enki::BinWriter writer;
    enki::serialize(map, writer).or_throw();

    enki::BinReader reader(writer.data());
    enki::deserialize(result, reader).or_throw();
    std::cout << "Map: Original size=" << map.size() << ", Deserialized size=" << result.size()
              << "\n";
  }

  // Custom struct with BinSpanWriter/BinSpanReader
  {
    Person person = {"Alice", 30, 1.65};
    Person result;

    std::array<std::byte, 100> buffer{};

    enki::BinSpanWriter writer(buffer);
    auto serRes = enki::serialize(person, writer).or_throw();

    enki::BinSpanReader reader(std::span<const std::byte>(buffer.data(), serRes.size()));
    enki::deserialize(result, reader).or_throw();
    std::cout << "Custom type: Original={" << person.name << "," << person.age << ","
              << person.height << "}, Deserialized={" << result.name << "," << result.age << ","
              << result.height << "}\n";
  }
}

// JSON serialization examples
void jsonSerializationExamples()
{
  std::cout << "\n=== JSON Serialization Examples ===\n";

  // Arithmetic type
  {
    int value = 42;
    int result;

    enki::JSONWriter writer;
    enki::serialize(value, writer).or_throw();
    std::string jsonStr = writer.data().str();

    std::cout << "Arithmetic JSON: " << jsonStr << "\n";

    enki::JSONReader reader(jsonStr);
    enki::deserialize(result, reader).or_throw();
    std::cout << "Deserialized: " << result << "\n";
  }

  // Array
  {
    std::array<float, 3> arr = {1.1f, 2.2f, 3.3f};
    std::array<float, 3> result;

    enki::JSONWriter writer;
    enki::serialize(arr, writer).or_throw();
    std::string jsonStr = writer.data().str();

    std::cout << "Array JSON: " << jsonStr << "\n";

    enki::JSONReader reader(jsonStr);
    enki::deserialize(result, reader).or_throw();
    std::cout << "Deserialized: [" << result[0] << "," << result[1] << "," << result[2] << "]\n";
  }

  // Vector (range-based)
  {
    std::vector<double> vec = {3.14, 2.71, 1.41};
    std::vector<double> result;

    enki::JSONWriter writer;
    enki::serialize(vec, writer).or_throw();
    std::string jsonStr = writer.data().str();

    std::cout << "Vector JSON: " << jsonStr << "\n";

    enki::JSONReader reader(jsonStr);
    enki::deserialize(result, reader).or_throw();
    std::cout << "Deserialized: size=" << result.size() << "\n";
  }

  // Map
  {
    std::map<std::string, int> map = {
      {  "one", 1},
      {  "two", 2},
      {"three", 3}
    };
    std::map<std::string, int> result;

    enki::JSONWriter writer;
    enki::serialize(map, writer).or_throw();
    std::string jsonStr = writer.data().str();

    std::cout << "Map JSON: " << jsonStr << "\n";

    enki::JSONReader reader(jsonStr);
    enki::deserialize(result, reader).or_throw();
    std::cout << "Deserialized: size=" << result.size() << "\n";
  }

  // Custom struct
  {
    Person person = {"Alice", 30, 1.65};
    Person result;

    enki::JSONWriter writer;
    enki::serialize(person, writer).or_throw();
    std::string jsonStr = writer.data().str();

    std::cout << "Custom type JSON: " << jsonStr << "\n";

    enki::JSONReader reader(jsonStr);
    enki::deserialize(result, reader).or_throw();
    std::cout << "Deserialized: {" << result.name << "," << result.age << "," << result.height
              << "}\n";
  }

  // Complex nested structure example
  {
    // Create an instance with test data
    ComplexData complex{
      {1, 2, 3, 4, 5},
      "This is a complex nested structure",
      {{"accuracy", 0.98}, {"precision", 0.87}, {"recall", 0.92}, {"f1_score", 0.895}}
    };
    ComplexData result;

    // Serialize to JSON
    enki::JSONWriter writer;
    enki::serialize(complex, writer).or_throw();
    std::string jsonStr = writer.data().str();

    // Print the JSON representation
    std::cout << "Complex nested structure JSON:\n" << jsonStr << "\n";

    // Deserialize from JSON
    enki::JSONReader reader(jsonStr);
    enki::deserialize(result, reader).or_throw();

    // Verify the result
    bool identical = complex == result;
    std::cout << "Successful deserialization: " << (identical ? "Yes" : "No") << "\n";
    std::cout << "Deserialized structure has:\n"
              << "- " << result.numbers.size() << " numbers\n"
              << "- Description length: " << result.description.length() << " characters\n"
              << "- " << result.metrics.size() << " metrics\n";
  }
}

int main()
{
  binarySerializationExamples();
  jsonSerializationExamples();

  return 0;
}
