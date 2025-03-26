# Enki: a modern C++ Serialization Library

![License](https://img.shields.io/badge/license-BSD--3--Clause-blue.svg)

Enki is a C++20 serialization library designed to enable simple to use and clean data serialization.

## Requirements

- C++20 compliant compiler
- CMake 3.20 or higher

The library is compatible with GCC11 and above, as well as Clang13 and above.

## Features

Enki provides a robust, flexible system for serializing both primitive types and custom structures. This section explores how serialization and deserialization work within the library.

### Core Serialization Logic

At its heart, Enki uses template specialization to handle different types in an extensible way. The library can serialize:

- Primitive arithmetic types
- Arrays and containers
- Tuples and tuple-like structures
- Optional values
- Variants
- Custom structures using the `EnkiSerial` pattern or by defining custom serializer/deserializer classes

### Basic Usage Example

```cpp
#include <array>
#include <string_view>

#include "enki/bin_reader.hpp"
#include "enki/bin_writer.hpp"
#include "enki/serialize.hpp"

// Values to serialize
constexpr int32_t integer = 42;
constexpr double floating = 3.14159;
const std::string text = "Hello Enki!";

std::vector<std::byte> basic_serialization() {
  enki::BinWriter writer;

  // Serialize values
  enki::serialize(integer, writer).or_throw();
  enki::serialize(floating, writer).or_throw();
  enki::serialize(text, writer).or_throw();

  return writer.data();
}

bool basic_deserialization(const std::vector<std::byte>& buffer)
{
  enki::BinReader reader(buffer);
  // Deserialize values
  int32_t out_int{};
  double out_double{};
  std::string out_text;

  enki::deserialize(out_int, reader).or_throw();
  enki::deserialize(out_double, reader).or_throw();
  enki::deserialize(out_text, reader).or_throw();

  // Verify correctness
  return out_int == integer &&
          out_double == floating &&
          out_text == text;
}
```

### Custom Structure Serialization Example (Using `EnkiSerial` Tag)

Enki provides flexible ways to serialize custom structures and classes.

The simplest approach is to define an `EnkiSerial` nested struct within your type:

```cpp
struct Point {
    int x;
    int y;

    struct EnkiSerial;  // This forward declaration enables you to define the EnkiSerial struct later in a .cpp file
};

struct Point::EnkiSerial {
    // Define which members to serialize and in what order
    static constexpr auto members = std::make_tuple(&Point::x, &Point::y);
};
```

### Using `ENKIWRAP` for Bitfields and Custom Members

```cpp
struct Person {
    std::string name;
    int age;
    uint8_t preferences : 4;  // Bitfield member
    uint8_t settings : 4;     // Another bitfield

    struct EnkiSerial;  // Forward declaration
};

struct Person::EnkiSerial {
    // Using regular member pointers for standard types
    // Using ENKIWRAP for bitfields to ensure proper serialization
    static constexpr auto members = std::make_tuple(
        &Person::name,
        &Person::age,
        ENKIWRAP(Person, preferences),  // Properly handle bitfield
        ENKIWRAP(Person, settings)      // Properly handle bitfield
    );
};
```

## Building the Library

```bash
# Clone the repository
git clone https://github.com/esantoul/enki.git
cd enki

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build .
```

### Build Options

- `ENKI_ENABLE_TESTS`: Build and run tests (default: OFF)

## Testing

Enki uses Catch2 for testing. To build and run the tests:

```bash
mkdir build && cd build
cmake .. -DENKI_ENABLE_TESTS=ON
cmake --build .
ctest
```

## Integration

### Using With CMake

#### Option 1: Using FetchContent
```cmake
include(FetchContent)
FetchContent_Declare(
  enki
  GIT_REPOSITORY https://github.com/esantoul/enki.git
  GIT_TAG main  # Or specify a version tag
)
FetchContent_MakeAvailable(enki)

target_link_libraries(your_target PRIVATE enki)
```

#### Option 2: Using CPM.cmake
```cmake
include(cmake/CPM.cmake)

CPMAddPackage("gh:esantoul/enki@main") # or specify a version tag

target_link_libraries(your_target PRIVATE enki)
```

#### Option 3: As a subdirectory
```cmake
add_subdirectory(path/to/enki)
target_link_libraries(your_target PRIVATE enki)
```

## License

Enki is distributed under the BSD 3-Clause License. See the [LICENSE](LICENSE) file for more details.

## Contributing

Contributions are welcome! Please ensure your code:

- Follows modern C++ best practices
- Includes appropriate tests
- Maintains a clean, readable style
- Is properly documented where necessary

## Contact

- Author: Etienne Santoul
- Project Homepage: [GitHub Repository](https://github.com/esantoul/enki)
