# Enki: a modern C++ Serialization Library

![License](https://img.shields.io/badge/license-BSD--3--Clause-blue.svg)

Enki is a C++20 serialization library designed to enable simple, clean, and efficient data serialization. It provides both **binary** and **JSON** serialization capabilities with a unified, extensible API.

## Requirements

- **C++20** compliant compiler (GCC 11+, Clang 13+, MSVC 2022+)
- **CMake 3.20** or higher

## Features

Enki provides a robust, flexible system for serializing both primitive types and custom structures with support for:

### Data Types
- ✅ Primitive arithmetic types (integers, floats, booleans)
- ✅ Enums (both scoped and unscoped)
- ✅ Fixed size arrays (`std::array` & C-style arrays)
- ✅ Range constructible containers (`std::vector`, `std::list`, etc.)
- ✅ Maps and associative containers (`std::map`, `std::unordered_map`, etc.)
- ✅ Tuples and tuple-like structures
- ✅ Optional values (`std::optional`)
- ✅ Variants (`std::variant`) with forward compatibility support
- ✅ Monostate (`std::monostate`) as null/empty type
- ✅ Custom structures with reflection-like capabilities

### Serialization Formats
- **Binary Serialization**: Compact, efficient binary format using `BinWriter`/`BinReader`
- **JSON Serialization**: Human-readable JSON format using `JSONWriter`/`JSONReader`
- **Span-based Writers**: Memory-efficient serialization with `BinSpanWriter`/`BinSpanReader`

### Advanced Features
- **Custom Structure Serialization**: Define serialization behavior using the `EnkiSerial` pattern
- **Bitfield Support**: Handle bitfields with `ENKIWRAP` macro
- **Range-based Serialization**: Automatic handling of range constructible containers
- **Template Specialization**: Extensible system for custom types
- **Error Handling**: Comprehensive error reporting with `or_throw()` and custom error types
- **Forward Compatibility**: Policy-based variant handling for schema evolution ([guide](docs/forward-compatibility.md))
- **CTAD Support**: Clean C++17 syntax with policy-first constructors

### Quick Start Examples

#### Binary Serialization
```cpp
#include "enki/enki.hpp"

int main() {
    // Simple binary serialization
    int value = 42;
    enki::BinWriter writer;
    enki::serialize(value, writer).or_throw();

    // Deserialize
    enki::BinReader reader(writer.data());
    int result;
    enki::deserialize(result, reader).or_throw();
    // result == 42
}
```

#### JSON Serialization
```cpp
#include "enki/enki.hpp"

int main() {
    // Simple JSON serialization
    int value = 42;
    enki::JSONWriter writer;
    enki::serialize(value, writer).or_throw();
    std::string json = writer.data().str();  // "42"

    // Deserialize
    enki::JSONReader reader(json);
    int result;
    enki::deserialize(result, reader).or_throw();
    // result == 42
}
```

#### Using Policies

```cpp
#include "enki/enki.hpp"

int main() {
    // Default strict policy
    enki::BinWriter strict_writer;

    // Forward-compatible policy (allows schema evolution for variants)
    enki::BinWriter compat_writer(enki::forward_compatible);
    enki::BinReader compat_reader(enki::forward_compatible, data);
}
```

See the [Forward Compatibility Guide](docs/forward-compatibility.md) for detailed usage.

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
    using Members = enki::Register<&Point::x, &Point::y>;
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
    using Members = enki::Register<
        &Person::name,
        &Person::age,
        ENKIWRAP(Person, preferences),  // Properly handle bitfield
        ENKIWRAP(Person, settings)      // Properly handle bitfield
    >;
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

- `ENKI_ENABLE_TESTS`: Build and run comprehensive test suite (default: OFF)
- `ENKI_ENABLE_EXAMPLES`: Build example program demonstrating library usage (default: OFF)

### Integration Examples

#### Using FetchContent
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

#### Using CPM.cmake
```cmake
include(cmake/CPM.cmake)

CPMAddPackage("gh:esantoul/enki@main") # or specify a version tag

target_link_libraries(your_target PRIVATE enki)
```

#### As a Subdirectory
```cmake
add_subdirectory(path/to/enki)
target_link_libraries(your_target PRIVATE enki)
```

## Testing & Examples

### Running Tests
Enki includes a comprehensive test suite built with Catch2:

```bash
mkdir build && cd build
cmake .. -DENKI_ENABLE_TESTS=ON
cmake --build .
ctest
```

The test suite covers:
- **Unit Tests**: Core serialization functionality
- **Regression Tests**: Data type serialization/deserialization
  - Arithmetic types (integers, floats, booleans)
  - Arrays and containers (`std::vector`, `std::array`)
  - Maps and associative containers
  - Custom types and structures
  - Optional values and variants
  - Enum serialization
  - Tuple-like structures
  - Range constructible containers
  - Binary and JSON serialization
  - Forward compatibility policies

### Running Examples
Build and run the example program to see Enki in action:

```bash
mkdir build && cd build
cmake .. -DENKI_ENABLE_EXAMPLES=ON
cmake --build .
./examples/example
```

The example demonstrate:
- Binary serialization with `BinWriter`/`BinReader`
- JSON serialization with `JSONWriter`/`JSONReader`
- Memory-efficient span-based serialization
- Custom structure serialization
- Complex nested data structures

## License

Enki is distributed under the BSD 3-Clause License. See the [LICENSE](LICENSE) file for more details.

## Contact

- Author: Etienne Santoul
- Project Homepage: [GitHub Repository](https://github.com/esantoul/enki)
