# MySTL

MySTL is a modern C++20 library that implements commonly used Standard Template Library (STL) components from scratch. The project is designed to explore the internal working of STL containers, algorithms, iterators, function objects, and memory management while following modern C++ programming practices.

## Features

### Containers

- Vector
- List
- Deque
- Stack
- Queue
- Map
- Set
- Unordered Map

### Algorithms

- sort
- find
- copy
- reverse
- transform
- binary_search
- lower_bound
- upper_bound

### Iterators

- Forward Iterator
- Bidirectional Iterator
- Random Access Iterator
- Reverse Iterator
- Iterator Traits

### Function Objects

- less
- greater
- equal_to
- not_equal_to
- plus
- minus
- multiplies
- divides

### Memory Management

- Custom Allocator
- Dynamic Memory Allocation
- Object Construction
- Object Destruction

---

## Project Structure

```
mystl
│
├── include/
│   └── mystl/
│       ├── vector.hpp
│       ├── list.hpp
│       ├── deque.hpp
│       ├── stack.hpp
│       ├── queue.hpp
│       ├── map.hpp
│       ├── set.hpp
│       ├── unordered_map.hpp
│       ├── algorithm.hpp
│       ├── iterator.hpp
│       ├── functional.hpp
│       ├── allocator.hpp
│       └── utility.hpp
│
├── examples/
│
├── tests/
│
├── benchmark/
│
├── docs/
│
├── CMakeLists.txt
├── README.md
└── .gitignore
```

---

## Requirements

- C++20 or later
- CMake 3.16+

Supported compilers:

- GCC
- Clang
- MSVC

---

## Building the Project

```bash
git clone <repository-url>

cd mystl

mkdir build
cd build

cmake ..
cmake --build .
```

---

## Example

```cpp
#include <iostream>
#include "mystl/vector.hpp"

int main()
{
    mystl::vector<int> numbers;

    numbers.push_back(10);
    numbers.push_back(20);
    numbers.push_back(30);

    for (const auto& value : numbers)
    {
        std::cout << value << " ";
    }

    return 0;
}
```

Output:

```
10 20 30
```

---

## Objectives

This project focuses on implementing and understanding:

- Generic programming with templates
- Dynamic memory management
- Container design
- Iterator implementation
- Generic algorithms
- Function objects
- Copy and move semantics
- Exception safety
- Modern C++ programming practices

---

