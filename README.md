# MemoryPhantom

MemoryPhantom is a lightweight C++ library for reading and writing memory of running processes on Windows. It provides a convenient interface for attaching to processes, reading and writing primitive data types, vectors, matrices, strings, and memory blocks.

Inspired by the C# library [Swed64](https://github.com/Massivetwat/Swed64).

---

## Features

* Attach to and detach from processes by PID or process name
* Find module base addresses
* Read and write primitive types (`int`, `float`, `double`, `bool`, etc.)
* Read and write vectors (`Vec3`) and matrices (`Mat4x4`)
* Read and write strings (`std::string`) and wide strings (`std::wstring`)
* Read and write arbitrary memory blocks

---

## Installation

1. Include `MemoryPhantom.h` and `MemoryPhantom.cpp` in your project.
2. Compile your project with C++17 or newer.

---

## Getting Started

Include the header:

```cpp
#include "MemoryPhantom.h"
```

Create an instance of `MemoryPhantom`:

```cpp
auto memOpt = MemoryPhantom::CreateFromName("cs2.exe");
if (!memOpt) {
    // Process not found
}
MemoryPhantom mem = std::move(memOpt.value());
```

---

## API Documentation

### **Attach / Detach**

```cpp
bool Attach(DWORD pid, DWORD accessRights = PROCESS_ALL_ACCESS);
void Detach();
bool IsActive() const;
DWORD GetPID() const;
HANDLE GetHandle() const;
```

* **Attach**: Opens a handle to a process using its PID and specified access rights.
* **Detach**: Closes the handle and clears the PID.
* **IsActive**: Returns true if the handle is valid.
* **GetPID**: Returns the attached process ID.
* **GetHandle**: Returns the raw Windows process handle.

---

### **Process Lookup**

```cpp
static std::optional<MemoryPhantom> CreateFromName(const char* targetName, DWORD accessRights = PROCESS_ALL_ACCESS);
```

* Creates a `MemoryPhantom` instance by process name.
* Returns `std::nullopt` if the process is not running.

---

### **Module Base Address**

```cpp
std::optional<uintptr_t> FindModuleBase(const char* moduleName) const;
```

* Returns the base address of a module (DLL or EXE) in the process.
* Returns `std::nullopt` if the module is not found.

---

### **Read / Write Primitive Data**

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

* `ReadData<T>` reads a value of type `T` from memory.
* `WriteData<T>` writes a value of type `T` to memory.
* Overloads with an offset simplify pointer arithmetic.

---

### **Pointers and Memory Blocks**

```cpp
std::optional<uintptr_t> ReadPtr(uintptr_t addr) const;
std::optional<uintptr_t> ReadPtr(uintptr_t addr, int off) const;
std::optional<std::vector<uint8_t>> ReadBlock(uintptr_t addr, size_t sz) const;
bool WriteBlock(uintptr_t addr, const std::vector<uint8_t>& data) const;
```

* `ReadPtr` reads a pointer-sized value (uintptr\_t).
* `ReadBlock` reads a raw memory block into a `std::vector<uint8_t>`.
* `WriteBlock` writes a memory block from a vector.

---

### **Strings**

```cpp
std::optional<std::string> ReadText(uintptr_t addr, size_t maxLen, bool nullEnded = true) const;
std::optional<std::wstring> ReadWideText(uintptr_t addr, size_t maxLen, bool nullEnded = true) const;
bool WriteText(uintptr_t addr, const std::string& text, bool addNull = true) const;
bool WriteWideText(uintptr_t addr, const std::wstring& text, bool addNull = true) const;
```

* `ReadText` reads a string from memory.
* `ReadWideText` reads a UTF-16 wide string.
* `WriteText` and `WriteWideText` write strings to memory, optionally appending a null terminator.

---

### **Vectors and Matrices**

```cpp
std::optional<Vec3> ReadVec3(uintptr_t addr) const;
bool WriteVec3(uintptr_t addr, const Vec3& vec) const;
std::optional<Mat4x4> ReadMatrix(uintptr_t addr) const;
bool WriteMatrix(uintptr_t addr, const Mat4x4& matrix) const;
```

* `Vec3` contains three `float` values (`x`, `y`, `z`).
* `Mat4x4` contains 16 `float` values (4x4 matrix).
* These methods simplify reading and writing structured data.

---

## Usage Example (CS2)

```cpp
#include "MemoryPhantom.h"
#include <iostream>

int main() {
    auto memOpt = MemoryPhantom::CreateFromName("cs2.exe");
    if (!memOpt) return 1;

    MemoryPhantom mem = std::move(memOpt.value());

    auto clientBaseOpt = mem.FindModuleBase("client.dll");
    if (!clientBaseOpt) return 1;

    uintptr_t clientBase = clientBaseOpt.value();

    auto valueOpt = mem.ReadData<int>(clientBase + 0x123456);
    if (valueOpt) {
        mem.WriteData<int>(clientBase + 0x123456, valueOpt.value() + 1);
    }

    Vec3 pos{1.0f, 2.0f, 3.0f};
    mem.WriteVec3(clientBase + 0x654321, pos);

    return 0;
}
```

---

## License

MIT License. Free to use, modify, and distribute.
