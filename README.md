# json – A header-only C++ JSON library

`json` is a simple, header-only C++ library for reading and writing JSON data.
It requires C++17 and uses only the standard library.
The library provides a `JsonElement` class that can represent any JSON value (null, boolean, number, string, array, object)
and supports streaming input/output.

## Features

- Header-only: just include `json.h` and start using it.
- No external dependencies.
- Full JSON grammar support (numbers, strings, objects, arrays, literals).
- Preserves insertion order of object keys (via `OrderedDict`).
- Custom number type that retains original string representation and can be converted to long long / double.
- Streaming operators (`<<` and `>>`) for easy serialization / deserialization.
- Detailed error messages via custom exception classes.

## Usage

### Including the library

```cpp
#include "json.h"
```

### Parsing JSON from a stream

```cpp
std::stringstream ss(R"({"name": "John", "age": 30, "cars": ["Ford", "BMW"]})");
json::JsonElement elem;
ss >> elem;                     // parse from stream
std::cout << elem << std::endl; // pretty-print
```

### Building a JSON value programmatically

```cpp
json::JsonElement obj;
json::Object dict;
dict.insert("name", json::JsonElement("Alice"));
dict.insert("age", json::JsonElement(25));
obj = dict;
std::cout << obj << std::endl;
```

### Accessing data

```cpp
if (elem.type == json::JsonElementTyping::OBJECT) {
    const auto& obj = elem.get_Object();
    std::string name = obj.at("name").get_String();
    int age = obj.at("age").get_Number().to_int();
}
```

### Number handling

Numbers are stored in a `json::Number` class that preserves the exact string representation.
You can check if it is an integer or float:

```cpp
json::Number num("123", false);               // integer 123
bool is_int = num.is_int();                   // true
long long i = num.to_int();                   // 123

json::Number num2("3.14", "0", false, false); // 3.14
double d = num2.to_float();                   // 3.14
```

### Exceptions

The library throws exceptions derived from `json::JsonException`:

- JsonIstreamFailure – on input format errors.
- JsonTypeError – on type mismatches.

### Building

Since it is header-only, no compilation is needed.
Just copy the `include/` folder and `json.h` into your project and include the main header.

### Requirements

- C++17 compatible compiler (tested with GCC).


