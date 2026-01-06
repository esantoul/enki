# Forward Compatibility Guide

This guide explains how to use Enki's forward compatibility features to safely evolve your serialization schemas over time.

## Introduction

When serializing `std::variant` types, you may need to add new alternatives in future versions of your software. The **forward compatibility policy** allows older code to safely read data containing unknown variant types by skipping them and falling back to a default state.

**When to use forward compatibility:**
- Your serialized data may be read by older software versions
- You plan to add new variant alternatives over time
- You need graceful degradation for unknown types

## Quick Start

Use `enki::forward_compatible` when creating writers and readers:

```cpp
#include "enki/enki.hpp"

// Writer with forward compatibility (adds size prefix to variants)
enki::BinWriter writer(enki::forward_compatible);

// Reader with forward compatibility (can skip unknown variants)
enki::BinReader reader(enki::forward_compatible, data);
```

The default policy is `enki::strict`, which errors on unknown variant indices.

## Using `std::monostate` as Fallback

For forward compatibility to work, your variant should include `std::monostate` as a fallback type. When an unknown variant index is encountered, the value is set to `monostate`.

```cpp
// Version 1: Original variant
using MessageV1 = std::variant<std::monostate, int, std::string>;

// Version 2: Added new type (double)
using MessageV2 = std::variant<std::monostate, int, std::string, double>;
```

**Example: Old reader handling new data**

```cpp
// New code serializes a double (index 3)
MessageV2 original = 3.14;
enki::BinWriter writer(enki::forward_compatible);
enki::serialize(original, writer).or_throw();

// Old code reads it - falls back to monostate
MessageV1 result;
enki::BinReader reader(enki::forward_compatible, writer.data());
enki::deserialize(result, reader).or_throw();

// result now holds std::monostate (index 0)
assert(std::holds_alternative<std::monostate>(result));
```

### `std::monostate` Serialization

`std::monostate` is a first-class serializable type:
- **Binary format**: Zero bytes (no data written)
- **JSON format**: `null`

```cpp
std::monostate empty;
enki::JSONWriter writer;
enki::serialize(empty, writer).or_throw();
// Output: null
```

## CTAD Syntax

Enki supports C++17 Class Template Argument Deduction with a policy-first constructor pattern (similar to `std::execution::par` in parallel algorithms):

```cpp
// Default strict policy
enki::BinWriter writer;
enki::BinReader reader(data);
enki::JSONWriter jwriter;
enki::JSONReader jreader(json);

// Forward compatible policy - policy comes first
enki::BinWriter writer(enki::forward_compatible);
enki::BinReader reader(enki::forward_compatible, data);
enki::JSONWriter jwriter(enki::forward_compatible);
enki::JSONReader jreader(enki::forward_compatible, json);

// Also works with span-based classes
enki::BinSpanWriter writer(enki::forward_compatible, buffer);
enki::BinSpanReader reader(enki::forward_compatible, data);
```

Explicit template parameters still work for custom size types:
```cpp
enki::BinWriter<enki::forward_compatible_t, uint16_t> writer;
```

## Policy Compatibility

### Binary Format: Policies Are NOT Wire-Compatible

**Important:** For binary serialization of variants, `strict` and `forward_compatible` produce different wire formats:

| Policy | Variant Binary Wire Format |
|--------|-------------------|
| `strict` | `[index][value]` |
| `forward_compatible` | `[index][size][value]` |

Because of this difference, **you must use the same policy for writing and reading**. Mixing policies will cause data corruption:

| Writer | Reader | Result |
|--------|--------|--------|
| `strict` | `strict` | Works (errors on unknown index) |
| `forward_compatible` | `forward_compatible` | Works (skips unknown, falls back to monostate) |
| `strict` | `forward_compatible` | **BROKEN** - reader tries to skip non-existent size field |
| `forward_compatible` | `strict` | **BROKEN** - reader interprets size field as value data |

### JSON Format: Policies Are Compatible

JSON is self-describing, so both policies produce identical output:

```json
{"1": 42}
```

The policy only affects **reading behavior** when encountering unknown variant indices:

| Reader Policy | Unknown Index Behavior |
|---------------|----------------------|
| `strict` | Returns error |
| `forward_compatible` | Skips value, falls back to monostate |

This means you can safely use any policy combination with JSON - the data format is always the same.

## Best Practices

### 1. Always include `std::monostate`

Place `monostate` at any index in your variant - Enki will find it:

```cpp
// Both work - monostate position doesn't matter
using Good1 = std::variant<std::monostate, int, std::string>;
using Good2 = std::variant<int, std::monostate, std::string>;
```

### 2. Only append new alternatives

Never remove or reorder existing variant alternatives. Only add new ones at the end:

```cpp
// Version 1
using MsgV1 = std::variant<std::monostate, int, std::string>;

// Version 2 - GOOD: append new type
using MsgV2 = std::variant<std::monostate, int, std::string, double>;

// Version 2 - BAD: reordering breaks compatibility
using MsgV2Bad = std::variant<std::monostate, std::string, int, double>;
```

### 3. Use forward_compatible for writers in production

If your data will be read by potentially older software, always use `forward_compatible` when writing:

```cpp
enki::BinWriter writer(enki::forward_compatible);
```

The overhead is minimal (one size field per variant value).

### 4. Handle monostate in application logic

After deserialization, check for monostate and handle gracefully:

```cpp
enki::deserialize(msg, reader).or_throw();

if (std::holds_alternative<std::monostate>(msg)) {
    // Unknown message type - log, skip, or use default behavior
    log_warning("Received unknown message type");
    return;
}

// Process known types
std::visit(overloaded{
    [](std::monostate) { /* already handled above */ },
    [](int i) { process_int(i); },
    [](const std::string& s) { process_string(s); }
}, msg);
```

### 5. Variants without monostate

If your variant doesn't have `monostate` and forward compatibility encounters an unknown index, deserialization returns an error:

```cpp
using NoFallback = std::variant<int, std::string>;  // No monostate

NoFallback result;
auto success = enki::deserialize(result, reader);
if (!success) {
    // Error: "Unknown variant index: no monostate alternative available"
}
```

## JSON Forward Compatibility

Forward compatibility also works with JSON serialization. JSON is self-describing, so no size prefix is needed - unknown values are parsed and skipped:

```cpp
// Variants serialize as {"index": value}
enki::JSONWriter writer(enki::forward_compatible);
std::variant<std::monostate, int> v = 42;
enki::serialize(v, writer).or_throw();
// Output: {"1": 42}

// Unknown index in JSON is skipped the same way
std::string json = R"({"5": {"nested": "object"}})";
enki::JSONReader reader(enki::forward_compatible, json);
std::variant<std::monostate, int> result;
enki::deserialize(result, reader).or_throw();
// result holds monostate
```
