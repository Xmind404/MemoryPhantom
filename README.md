# MemoryPhantom

**MemoryPhantom** is a lightweight, header-only C++ library for reading and writing memory of running processes on Windows.  
Inspired by the C# project [Swed64](https://github.com/Massivetwat/Swed64), it provides a clean, modern C++ interface for memory manipulation in game hacking, process monitoring, and debugging applications.

## ✨ Features

- **Easy Process Attachment**: Attach to processes by name or PID
- **Module Discovery**: Locate base addresses of DLLs and executables
- **Type-Safe Memory Operations**: Read/write primitive types, strings, vectors, and matrices
- **Modern C++ Design**: Uses `std::optional` for safe error handling, move semantics, and RAII
- **Cross-Version Compatibility**: Works with both x86 and x64 processes
- **Header-Only**: Single-header implementation for easy integration

## 📦 Installation

### Method 1: Direct Include (Recommended)
Simply copy `MemoryPhantom.h` into your project and include it:

```cpp
#include "MemoryPhantom.h"
```

### Method 2: Using CMake
Add to your `CMakeLists.txt`:

```cmake
# Add MemoryPhantom to your project
include_directories(path/to/MemoryPhantom)

# Or add as a subdirectory
add_subdirectory(path/to/MemoryPhantom)
```

### Requirements
- C++17 or newer
- Windows SDK
- Visual Studio 2017+ or MinGW-w64

## 🚀 Quick Start

```cpp
#include "MemoryPhantom.h"
#include <iostream>

int main() {
    // Attach to a process by name
    auto phantom = MemoryPhantom::CreateFromName("notepad.exe");
    if (!phantom) {
        std::cerr << "Failed to find notepad.exe!" << std::endl;
        return 1;
    }

    std::cout << "Attached to process PID: " << phantom->GetPID() << std::endl;

    // Find a module base address
    auto moduleBase = phantom->FindModuleBase("notepad.exe");
    if (moduleBase) {
        std::cout << "Module base: 0x" << std::hex << *moduleBase << std::dec << std::endl;
        
        // Read an integer value
        auto value = phantom->ReadData<int>(*moduleBase + 0x1000);
        if (value) {
            std::cout << "Read value: " << *value << std::endl;
            
            // Write a new value
            phantom->WriteData<int>(*moduleBase + 0x1000, 1337);
        }
    }

    return 0;
}
```

## 📚 API Reference

### Core Management

```cpp
// Construction
MemoryPhantom();  // Empty object
MemoryPhantom(DWORD pid, DWORD accessRights = PROCESS_ALL_ACCESS);
~MemoryPhantom(); // Automatically detaches

// Move operations
MemoryPhantom(MemoryPhantom&& other) noexcept;
MemoryPhantom& operator=(MemoryPhantom&& other) noexcept;

// No copy operations (singleton pattern for process handles)
MemoryPhantom(const MemoryPhantom&) = delete;
MemoryPhantom& operator=(const MemoryPhantom&) = delete;
```

### Process Control

```cpp
// Attach/detach
bool Attach(DWORD pid, DWORD accessRights = PROCESS_ALL_ACCESS);
void Detach();
bool IsActive() const;

// Process information
DWORD GetPID() const;
HANDLE GetHandle() const;

// Static factory method
static std::optional<MemoryPhantom> CreateFromName(
    const char* processName, 
    DWORD accessRights = PROCESS_ALL_ACCESS
);
```

### Module Operations

```cpp
// Find module base address
std::optional<uintptr_t> FindModuleBase(const char* moduleName) const;
```

### Memory Reading

```cpp
// Generic read (supports any trivially copyable type)
template<typename T>
std::optional<T> ReadData(uintptr_t address) const;

template<typename T>
std::optional<T> ReadData(uintptr_t address, int offset) const;

// Pointer reading
std::optional<uintptr_t> ReadPtr(uintptr_t address) const;
std::optional<uintptr_t> ReadPtr(uintptr_t address, int offset) const;

// Bulk memory operations
std::optional<std::vector<uint8_t>> ReadBlock(uintptr_t address, size_t size) const;
```

### Memory Writing

```cpp
// Generic write
template<typename T>
bool WriteData(uintptr_t address, const T& value) const;

template<typename T>
bool WriteData(uintptr_t address, int offset, const T& value) const;

// Bulk write
bool WriteBlock(uintptr_t address, const std::vector<uint8_t>& data) const;
```

### String Operations

```cpp
// ANSI strings
std::optional<std::string> ReadText(
    uintptr_t address, 
    size_t maxLength, 
    bool nullTerminated = true
) const;

bool WriteText(
    uintptr_t address, 
    const std::string& text, 
    bool addNullTerminator = true
) const;

// Wide strings (Unicode)
std::optional<std::wstring> ReadWideText(
    uintptr_t address, 
    size_t maxLength, 
    bool nullTerminated = true
) const;

bool WriteWideText(
    uintptr_t address, 
    const std::wstring& text, 
    bool addNullTerminator = true
) const;
```

### Specialized Types

```cpp
// Vector3 (12 bytes: float x, y, z)
struct Vec3 {
    float x, y, z;
};

std::optional<Vec3> ReadVec3(uintptr_t address) const;
bool WriteVec3(uintptr_t address, const Vec3& vec) const;

// Matrix 4x4 (64 bytes: 16 floats)
struct Mat4x4 {
    float data[16]; // Column-major: [0] [1] [2] [3] ... [15]
};

std::optional<Mat4x4> ReadMatrix(uintptr_t address) const;
bool WriteMatrix(uintptr_t address, const Mat4x4& matrix) const;
```

## 🎯 Advanced Examples

### Example 1: Game Hacking (Health Manipulation)

```cpp
#include "MemoryPhantom.h"

class GameHack {
    MemoryPhantom phantom;
    uintptr_t playerBase;
    uintptr_t healthOffset = 0x100;
    
public:
    bool Initialize(const char* gameName) {
        auto game = MemoryPhantom::CreateFromName(gameName);
        if (!game) return false;
        
        phantom = std::move(*game);
        
        auto module = phantom.FindModuleBase("game.dll");
        if (!module) return false;
        
        playerBase = *module + 0x123456; // Player structure base
        return true;
    }
    
    std::optional<int> GetHealth() {
        return phantom.ReadData<int>(playerBase + healthOffset);
    }
    
    bool SetHealth(int health) {
        return phantom.WriteData<int>(playerBase + healthOffset, health);
    }
    
    std::optional<MemoryPhantom::Vec3> GetPosition() {
        uintptr_t positionOffset = 0x200;
        return phantom.ReadVec3(playerBase + positionOffset);
    }
};
```

### Example 2: Process Memory Scanner

```cpp
#include "MemoryPhantom.h"
#include <iostream>
#include <vector>

class MemoryScanner {
    MemoryPhantom& phantom;
    
public:
    MemoryScanner(MemoryPhantom& phantom) : phantom(phantom) {}
    
    std::vector<uintptr_t> ScanForValue(int value, uintptr_t start, uintptr_t end, size_t step = 4) {
        std::vector<uintptr_t> results;
        
        for (uintptr_t addr = start; addr < end; addr += step) {
            auto readValue = phantom.ReadData<int>(addr);
            if (readValue && *readValue == value) {
                results.push_back(addr);
            }
        }
        
        return results;
    }
    
    std::vector<uintptr_t> ScanForPattern(
        const std::vector<uint8_t>& pattern,
        uintptr_t start,
        uintptr_t end
    ) {
        std::vector<uintptr_t> results;
        size_t patternSize = pattern.size();
        
        for (uintptr_t addr = start; addr < end - patternSize; ++addr) {
            auto block = phantom.ReadBlock(addr, patternSize);
            if (block && std::equal(pattern.begin(), pattern.end(), block->begin())) {
                results.push_back(addr);
            }
        }
        
        return results;
    }
};
```

### Example 3: Multi-level Pointer Resolution

```cpp
#include "MemoryPhantom.h"
#include <vector>

class PointerChainResolver {
    MemoryPhantom& phantom;
    
public:
    PointerChainResolver(MemoryPhantom& phantom) : phantom(phantom) {}
    
    std::optional<uintptr_t> ResolvePointerChain(
        uintptr_t baseAddress,
        const std::vector<int>& offsets
    ) {
        uintptr_t current = baseAddress;
        
        for (size_t i = 0; i < offsets.size(); ++i) {
            auto pointer = phantom.ReadPtr(current);
            if (!pointer) return std::nullopt;
            
            current = *pointer;
            
            // Add offset for all but the last iteration
            if (i < offsets.size() - 1) {
                current += offsets[i];
            }
        }
        
        // Add final offset
        if (!offsets.empty()) {
            current += offsets.back();
        }
        
        return current;
    }
};

// Usage:
// PointerChainResolver resolver(phantom);
// auto finalAddress = resolver.ResolvePointerChain(
//     moduleBase + 0x123456,
//     {0x10, 0x20, 0x30, 0x40}
// );
```

## ⚠️ Important Notes

### Memory Safety
- Always check the return value of `std::optional` operations
- Use `IsActive()` to verify process attachment
- Handle invalid memory access gracefully

### Performance Considerations
- Cache frequently accessed addresses
- Use `ReadBlock()` for bulk operations instead of multiple small reads
- Consider using static offsets when possible

### Common Pitfalls
1. **Process Permissions**: Ensure your application has sufficient privileges
2. **Address Space Layout Randomization (ASLR)**: Module addresses change between executions
3. **32-bit vs 64-bit**: Be aware of pointer size differences
4. **Thread Safety**: This library is not thread-safe by default

## 🔧 Building from Source

### Visual Studio
1. Create a new C++ project
2. Add `MemoryPhantom.h` to your project
3. Enable C++17: `Project Properties → C/C++ → Language → C++ Language Standard`
4. Build and run

### MinGW/GCC
```bash
g++ -std=c++17 -o myapp main.cpp -lpsapi
```

### CMake
```cmake
cmake_minimum_required(VERSION 3.10)
project(MyApp)

set(CMAKE_CXX_STANDARD 17)

add_executable(myapp main.cpp)
target_link_libraries(myapp psapi)
```

## 📊 Supported Data Types

| Type | Size | Read Method | Write Method |
|------|------|-------------|--------------|
| `int` | 4 bytes | `ReadData<int>()` | `WriteData<int>()` |
| `float` | 4 bytes | `ReadData<float>()` | `WriteData<float>()` |
| `double` | 8 bytes | `ReadData<double>()` | `WriteData<double>()` |
| `bool` | 1 byte | `ReadData<bool>()` | `WriteData<bool>()` |
| `uintptr_t` | 4/8 bytes | `ReadPtr()` | `WriteData<uintptr_t>()` |
| `Vec3` | 12 bytes | `ReadVec3()` | `WriteVec3()` |
| `Mat4x4` | 64 bytes | `ReadMatrix()` | `WriteMatrix()` |
| `std::string` | Variable | `ReadText()` | `WriteText()` |
| `std::wstring` | Variable | `ReadWideText()` | `WriteWideText()` |

## 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Development Guidelines
- Follow the existing code style
- Add tests for new features
- Update documentation as needed
- Ensure backward compatibility

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Inspired by [Swed64](https://github.com/Massivetwat/Swed64)
- Thanks to all contributors and the game hacking community
- Special thanks to Microsoft for the Windows API documentation

## 📞 Support

- **GitHub Issues**: For bug reports and feature requests
- **Discussions**: For questions and community support
- **Wiki**: For detailed documentation and tutorials

## 🚨 Disclaimer

This library is intended for:
- Educational purposes
- Legitimate game modding
- Debugging and reverse engineering
- Security research

**Never use this library for:**
- Cheating in multiplayer games
- Malicious software
- Violating terms of service
- Illegal activities

The authors are not responsible for any misuse of this library.

---

⭐ **If you find this library useful, please give it a star on GitHub!** ⭐
