# MemoryPhantom

MemoryPhantom is a lightweight C++ library for reading and writing process memory on Windows. It allows you to attach to running processes, read and write various data types, vectors, matrices, and strings.

**Inspired by the C# library [Swed64](https://github.com/Massivetwat/Swed64/blob/main/Swed64-master/swed64/Swed64.cs).**

## Features

* Attach/detach to processes by PID or process name
* Find module base addresses
* Read/write primitive data types (int, float, double, bool, etc.)
* Read/write vectors (`Vec3`) and matrices (`Mat4x4`)
* Read/write strings and wide strings
* Read/write memory blocks

## Installation

Include `MemoryPhantom.h` and `MemoryPhantom.cpp` in your project.

## Usage Example (CS2)

```cpp
#include "MemoryPhantom.h"
#include <iostream>

int main() {
    // Attach to CS2 process by name
    auto memOpt = MemoryPhantom::CreateFromName("cs2.exe");
    if (!memOpt) {
        std::cout << "CS2 is not running." << std::endl;
        return 1;
    }

    MemoryPhantom mem = std::move(memOpt.value());

    // Get module base (e.g., client.dll)
    auto clientBaseOpt = mem.FindModuleBase("client.dll");
    if (!clientBaseOpt) {
        std::cout << "client.dll not found." << std::endl;
        return 1;
    }

    uintptr_t clientBase = clientBaseOpt.value();
    std::cout << "client.dll base: 0x" << std::hex << clientBase << std::endl;

    // Read an integer from some offset
    uintptr_t someAddress = clientBase + 0x123456; // example offset
    auto valueOpt = mem.ReadData<int>(someAddress);
    if (valueOpt) {
        std::cout << "Value: " << valueOpt.value() << std::endl;
    }

    // Write an integer
    mem.WriteData<int>(someAddress, 1337);

    // Read a Vec3
    auto posOpt = mem.ReadVec3(clientBase + 0x654321); // example offset
    if (posOpt) {
        Vec3 pos = posOpt.value();
        std::cout << "Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
    }

    // Write a Vec3
    Vec3 newPos{1.0f, 2.0f, 3.0f};
    mem.WriteVec3(clientBase + 0x654321, newPos);

    return 0;
}
```

## License

This project is MIT licensed. You can freely use, modify, and distribute it.
