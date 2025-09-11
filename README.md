# MemoryPhantom

MemoryPhantom is a C++ library for reading and writing memory of external processes. It allows you to attach to a process, read/write primitive types, vectors, matrices, strings, and handle pointers safely.

## Features

* Attach/detach from external processes
* Read and write primitive data types (int, float, double, bool, etc.)
* Read and write strings (ASCII and wide)
* Read and write vectors (`Vec3`) and matrices (`Mat4x4`)
* Read and write memory blocks
* Find module base addresses
* Safe memory handling with optional return values

## Installation

1. Clone or download this repository.
2. Include `MemoryPhantom.h` in your project.
3. Link against `Kernel32.lib` (Windows API).

```cpp
#include "MemoryPhantom.h"
```

## Example Usage (CS2 / Counter-Strike 2)

### Attach to Process

```cpp
#include "MemoryPhantom.h"
#include <iostream>

int main() {
    auto mpOpt = MemoryPhantom::CreateFromName("cs2.exe");
    if (!mpOpt.has_value()) {
        std::cout << "Process not found!" << std::endl;
        return 1;
    }

    MemoryPhantom mp = std::move(mpOpt.value());
    if (!mp.IsActive()) {
        std::cout << "Failed to attach to process." << std::endl;
        return 1;
    }

    std::cout << "Attached to CS2 (PID: " << mp.GetPID() << ")" << std::endl;
}
```

### Get Module Base Address (client.dll)

```cpp
auto clientBaseOpt = mp.FindModuleBase("client.dll");
if (clientBaseOpt.has_value()) {
    uintptr_t clientBase = clientBaseOpt.value();
    std::cout << "client.dll base: 0x" << std::hex << clientBase << std::endl;
} else {
    std::cout << "Module not found." << std::endl;
}
```

### Reading and Writing Data

```cpp
uintptr_t playerHealthAddr = clientBase + 0x123456; // Example offset

// Read int
auto healthOpt = mp.ReadData<int>(playerHealthAddr);
if (healthOpt.has_value()) {
    int health = healthOpt.value();
    std::cout << "Player health: " << health << std::endl;
}

// Write int
mp.WriteData<int>(playerHealthAddr, 100);
```

### Reading Vectors (Vec3)

```cpp
uintptr_t playerPosAddr = clientBase + 0x654321; // Example offset
auto posOpt = mp.ReadVec3(playerPosAddr);
if (posOpt.has_value()) {
    Vec3 pos = posOpt.value();
    std::cout << "Player position: x=" << pos.x << " y=" << pos.y << " z=" << pos.z << std::endl;
}

// Write new position
Vec3 newPos{ 10.0f, 20.0f, 30.0f };
mp.WriteVec3(playerPosAddr, newPos);
```

### Reading Strings

```cpp
uintptr_t nameAddr = clientBase + 0xABCDEF;
auto nameOpt = mp.ReadText(nameAddr, 32);
if (nameOpt.has_value()) {
    std::cout << "Player name: " << nameOpt.value() << std::endl;
}
```

## License

This project is licensed under the MIT License. You are free to use, modify, and distribute it.
