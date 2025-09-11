# MemoryPhantom

**MemoryPhantom** is a lightweight C++ library for reading and writing memory of running processes on Windows.  
It allows easy process attachment, module locating, and data manipulation — from basic types, through strings, to vectors and matrices.

Inspired by the C# project: [Swed64](https://github.com/Massivetwat/Swed64).

---

## ✨ Features

- Attach/detach from processes (by PID or process name).
- Locate module base addresses (DLL/EXE).
- Read/write primitive types (`int`, `float`, `double`, `bool`, ...).
- Support for vectors (`Vec3`) and matrices (`Mat4x4`).
- Support for strings (`std::string`, `std::wstring`).
- Read/write memory blocks.
- Convenient interface using `std::optional` for safer error handling.

---

## 🔧 Installation

1. Copy `MemoryPhantom.h` and `MemoryPhantom.cpp` into your project.
2. Compile your project using **C++17** or newer.

---

## 🚀 Quick Start

```cpp
#include "MemoryPhantom.h"

int main() {
    auto phantom = MemoryPhantom::CreateFromName("cs2.exe");
    if (!phantom) {
        return -1; // Process not found
    }

    MemoryPhantom mem = std::move(*phantom);

    auto base = mem.FindModuleBase("client.dll");
    if (!base) return -1;

    auto health = mem.ReadData<int>(*base + 0x123456);
    if (health) {
        mem.WriteData<int>(*base + 0x123456, *health + 10);
    }

    return 0;
}
```

---

## 📚 API

### Construction and Lifecycle

```cpp
MemoryPhantom();                                     // empty object
MemoryPhantom(DWORD pid, DWORD rights = PROCESS_ALL_ACCESS);
~MemoryPhantom();                                    // auto Detach()

MemoryPhantom(MemoryPhantom&& other);                // move ctor
MemoryPhantom& operator=(MemoryPhantom&& other);     // move assign
```

---

### Process Management

```cpp
bool Attach(DWORD pid, DWORD rights = PROCESS_ALL_ACCESS);
void Detach();
bool IsActive() const;
DWORD GetPID() const;
HANDLE GetHandle() const;

static std::optional<MemoryPhantom> CreateFromName(
    const char* processName, 
    DWORD rights = PROCESS_ALL_ACCESS
);
```

---

### Modules

```cpp
std::optional<uintptr_t> FindModuleBase(const char* moduleName) const;
```

---

### Read/Write Primitive Types

```cpp
template<typename T>
std::optional<T> ReadData(uintptr_t addr) const;

template<typename T>
std::optional<T> ReadData(uintptr_t addr, int off) const;

template<typename T>
bool WriteData(uintptr_t addr, const T& value) const;

template<typename T>
bool WriteData(uintptr_t addr, int off, const T& value) const;
```

---

### Pointers and Memory Blocks

```cpp
std::optional<uintptr_t> ReadPtr(uintptr_t addr) const;
std::optional<uintptr_t> ReadPtr(uintptr_t addr, int off) const;

std::optional<std::vector<uint8_t>> ReadBlock(uintptr_t addr, size_t sz) const;
bool WriteBlock(uintptr_t addr, const std::vector<uint8_t>& data) const;
```

---

### Strings

```cpp
std::optional<std::string> ReadText(uintptr_t addr, size_t maxLen, bool nullEnded = true) const;
std::optional<std::wstring> ReadWideText(uintptr_t addr, size_t maxLen, bool nullEnded = true) const;

bool WriteText(uintptr_t addr, const std::string& text, bool addNull = true) const;
bool WriteWideText(uintptr_t addr, const std::wstring& text, bool addNull = true) const;
```

---

### Math Structures

```cpp
struct Vec3 {
    float x, y, z;
};

struct Mat4x4 {
    float data[16];
};

std::optional<Vec3> ReadVec3(uintptr_t addr) const;
bool WriteVec3(uintptr_t addr, const Vec3& vec) const;

std::optional<Mat4x4> ReadMatrix(uintptr_t addr) const;
bool WriteMatrix(uintptr_t addr, const Mat4x4& matrix) const;
```

---

## 🕹️ Example Usage (CS2)

```cpp
#include "MemoryPhantom.h"
#include <iostream>

int main() {
    auto phantom = MemoryPhantom::CreateFromName("cs2.exe");
    if (!phantom) {
        std::cerr << "cs2.exe process not found!" << std::endl;
        return -1;
    }

    std::cout << "Connected to cs2.exe (PID: " << phantom->GetPID() << ")" << std::endl;

    auto clientBase = phantom->FindModuleBase("client.dll");
    if (!clientBase) {
        std::cerr << "client.dll not found!" << std::endl;
        return -1;
    }

    uintptr_t healthAddr = *clientBase + 0x123456; // example offset

    auto health = phantom->ReadData<int>(healthAddr);
    if (health) {
        std::cout << "Health: " << *health << std::endl;

        if (phantom->WriteData<int>(healthAddr, 1337)) {
            std::cout << "Health set to 1337" << std::endl;
        }
    }

    return 0;
}
```

⚠️ **Note:** offset `0x123456` is just an example.  
Use actual CS2 offsets in practice (e.g., `dwLocalPlayer`, `m_iHealth`, etc.).

---

## 📜 License

MIT License — free to use, modify, and distribute.
