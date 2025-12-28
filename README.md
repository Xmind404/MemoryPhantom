# MemoryPhantom

**MemoryPhantom** is a lightweight C++ library for reading and writing memory of running processes on Windows.  
Provides a clean, modern C++ interface for memory manipulation in game hacking, process monitoring, and debugging applications.

## ✨ Features

- **Easy Process Attachment**: Attach to processes by name or PID
- **Module Discovery**: Locate base addresses of DLLs and executables
- **Type-Safe Memory Operations**: Read/write primitive types, strings, vectors, and matrices
- **Modern C++ Design**: Uses `std::optional` for safe error handling, move semantics, and RAII
- **Cross-Version Compatibility**: Works with both x86 and x64 processes
- **Two-File Library**: Only `MemoryPhantom.h` and `MemoryPhantom.cpp` for easy integration

## 📦 Installation

### Method 1: Direct Include (Recommended)
Copy both `MemoryPhantom.h` and `MemoryPhantom.cpp` into your project:

```cpp
#include "MemoryPhantom.h"
```

Add `MemoryPhantom.cpp` to your project compilation.

### Method 2: Using CMake
Add to your `CMakeLists.txt`:

```cmake
# Add MemoryPhantom to your project
add_library(MemoryPhantom MemoryPhantom.cpp MemoryPhantom.h)
target_include_directories(MemoryPhantom PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link with your executable
target_link_libraries(your_target MemoryPhantom psapi)
```

### Requirements
- C++17 or newer
- Windows SDK
- Visual Studio 2017+ or MinGW-w64
- Link with `psapi.lib` (Windows)

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

// No copy operations (process handles are unique)
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
// Generic read (pre-instantiated for common types)
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
// Generic write (pre-instantiated for common types)
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
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

std::optional<Vec3> ReadVec3(uintptr_t address) const;
bool WriteVec3(uintptr_t address, const Vec3& vec) const;

// Matrix 4x4 (64 bytes: 16 floats)
struct Mat4x4 {
    float data[16];
    Mat4x4() { memset(data, 0, sizeof(data)); }
};

std::optional<Mat4x4> ReadMatrix(uintptr_t address) const;
bool WriteMatrix(uintptr_t address, const Mat4x4& matrix) const;
```

## 🎯 Advanced Examples

### Example 1: CS2 TriggerBot

```cpp
#include "MemoryPhantom.h"
#include <iostream>
#include <thread>

int main() {
    auto phantom = MemoryPhantom::CreateFromName("cs2.exe");
    if (!phantom) {
        std::cerr << "Failed to find cs2.exe!" << std::endl;
        return 1;
    }

    auto client = phantom->FindModuleBase("client.dll");
    if(!client) {
        std::cerr << "Failed to find client.dll!" << std::endl;
        return 1;
    }
    
    uintptr_t dwForceAttack = *client + 0x1BE83A0;
    uintptr_t dwLocalPlayerPtr = *client + 0x1BEEF28;
    uintptr_t m_iCrosshairIdOffset = 0x3ECC;

    while (true) {
        auto dwLocalPlayer = phantom->ReadData<uintptr_t>(dwLocalPlayerPtr);
        if (!dwLocalPlayer) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        auto entityIndex = phantom->ReadData<int32_t>(*dwLocalPlayer + m_iCrosshairIdOffset);
        if (!entityIndex) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        if (GetAsyncKeyState(0x56) & 0x8000) {
            if (*entityIndex > 0) {
                phantom->WriteData<uint32_t>(dwForceAttack, 65537);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                phantom->WriteData<uint32_t>(dwForceAttack, 256);
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
```

### Example 2: Player Information Reader

```cpp
#include "MemoryPhantom.h"
#include <iostream>

struct PlayerInfo {
    MemoryPhantom::Vec3 position;
    int health;
    int armor;
    std::string name;
};

class GameReader {
    MemoryPhantom phantom;
    uintptr_t clientBase;
    
public:
    bool Initialize(const char* processName) {
        auto game = MemoryPhantom::CreateFromName(processName);
        if (!game) return false;
        
        phantom = std::move(*game);
        
        auto client = phantom.FindModuleBase("client.dll");
        if (!client) return false;
        
        clientBase = *client;
        return true;
    }
    
    std::optional<PlayerInfo> ReadLocalPlayer() {
        uintptr_t localPlayer = *clientBase + 0x1BEEF28;
        auto playerPtr = phantom.ReadData<uintptr_t>(localPlayer);
        if (!playerPtr) return std::nullopt;
        
        PlayerInfo info;
        
        auto pos = phantom.ReadVec3(*playerPtr + 0x138);
        if (pos) info.position = *pos;
        
        auto health = phantom.ReadData<int>(*playerPtr + 0x100);
        if (health) info.health = *health;
        
        auto armor = phantom.ReadData<int>(*playerPtr + 0x104);
        if (armor) info.armor = *armor;
        
        auto name = phantom.ReadText(*playerPtr + 0x250, 32);
        if (name) info.name = *name;
        
        return info;
    }
};
```

### Example 3: Pattern Scanner with Memory Block

```cpp
#include "MemoryPhantom.h"
#include <vector>
#include <algorithm>

class PatternScanner {
    MemoryPhantom& phantom;
    
public:
    PatternScanner(MemoryPhantom& phantom) : phantom(phantom) {}
    
    std::vector<uintptr_t> ScanPattern(
        const std::string& pattern,  // "FF 74 ?? 8B 45"
        uintptr_t startAddr,
        uintptr_t endAddr
    ) {
        std::vector<uintptr_t> results;
        std::vector<uint8_t> patternBytes;
        std::vector<bool> wildcards;
        
        ParsePattern(pattern, patternBytes, wildcards);
        
        size_t patternSize = patternBytes.size();
        size_t blockSize = 1024 * 1024; // 1MB blocks
        
        for (uintptr_t addr = startAddr; addr < endAddr - patternSize; addr += blockSize - patternSize) {
            size_t readSize = std::min(blockSize, (size_t)(endAddr - addr));
            
            auto block = phantom.ReadBlock(addr, readSize);
            if (!block) continue;
            
            for (size_t i = 0; i <= block->size() - patternSize; i++) {
                bool match = true;
                
                for (size_t j = 0; j < patternSize; j++) {
                    if (!wildcards[j] && (*block)[i + j] != patternBytes[j]) {
                        match = false;
                        break;
                    }
                }
                
                if (match) {
                    results.push_back(addr + i);
                }
            }
        }
        
        return results;
    }
    
private:
    void ParsePattern(const std::string& pattern, std::vector<uint8_t>& bytes, std::vector<bool>& wildcards) {
        std::istringstream stream(pattern);
        std::string token;
        
        while (stream >> token) {
            if (token == "??") {
                bytes.push_back(0);
                wildcards.push_back(true);
            } else {
                bytes.push_back((uint8_t)std::stoul(token, nullptr, 16));
                wildcards.push_back(false);
            }
        }
    }
};
```

### Example 4: Hook Installation

```cpp
#include "MemoryPhantom.h"
#include <vector>

class MemoryHook {
    MemoryPhantom& phantom;
    uintptr_t targetAddress;
    std::vector<uint8_t> originalBytes;
    std::vector<uint8_t> hookBytes;
    
public:
    MemoryHook(MemoryPhantom& phantom, uintptr_t address, const std::vector<uint8_t>& hook)
        : phantom(phantom), targetAddress(address), hookBytes(hook) {}
    
    bool Install() {
        originalBytes = phantom.ReadBlock(targetAddress, hookBytes.size()).value_or(std::vector<uint8_t>());
        if (originalBytes.size() != hookBytes.size()) return false;
        
        return phantom.WriteBlock(targetAddress, hookBytes);
    }
    
    bool Remove() {
        return phantom.WriteBlock(targetAddress, originalBytes);
    }
    
    bool IsInstalled() const {
        auto currentBytes = phantom.ReadBlock(targetAddress, hookBytes.size());
        return currentBytes && (*currentBytes) == hookBytes;
    }
};

// Usage example:
// std::vector<uint8_t> jmpHook = {0xE9, 0x00, 0x00, 0x00, 0x00}; // JMP instruction
// MemoryHook hook(phantom, 0x401000, jmpHook);
// hook.Install();
```

## 📊 Pre-Instantiated Template Types

The library includes explicit template instantiations for common types:

### Integer Types
- `int8_t`, `uint8_t` (char, byte)
- `int16_t`, `uint16_t` (short)
- `int32_t`, `uint32_t` (int, DWORD)
- `int64_t`, `uint64_t` (long long, QWORD)
- `unsigned long long`

### Floating Point Types
- `float` (32-bit)
- `double` (64-bit)

### Boolean and Character
- `bool`
- `char`

### Special Types
- `Vec3` (custom 3D vector)
- `Mat4x4` (custom 4x4 matrix)

## ⚠️ Important Notes

### Memory Safety
- Always check return values of `std::optional` operations
- Use `IsActive()` to verify process attachment is valid
- Handle invalid memory access gracefully with error checking
- Process handles are automatically cleaned up via RAII

### Performance Considerations
- Cache frequently accessed addresses to reduce overhead
- Use `ReadBlock()` for bulk operations instead of many small reads
- Consider using static offsets when possible (faster than pointer chains)
- Close handles promptly when done

### Common Pitfalls
1. **Process Permissions**: Ensure your application has sufficient privileges (run as admin if needed)
2. **ASLR (Address Space Layout Randomization)**: Module base addresses change between executions
3. **32-bit vs 64-bit Differences**: Be aware of pointer size differences (4 vs 8 bytes)
4. **Thread Safety**: Library is not thread-safe; synchronize access if using from multiple threads
5. **Anti-Cheat Software**: Many games have anti-cheat that detects memory reading/writing

## 🔧 Building

### Visual Studio (Manual Setup)
1. Create a new C++ project
2. Add `MemoryPhantom.h` and `MemoryPhantom.cpp` to your project
3. Enable C++17: `Project Properties → C/C++ → Language → C++ Language Standard`
4. Add `psapi.lib` to linker: `Project Properties → Linker → Input → Additional Dependencies`
5. Build and run

### Visual Studio (Command Line)
```batch
cl /std:c++17 /EHsc main.cpp MemoryPhantom.cpp /link psapi.lib
```

### MinGW/GCC
```bash
g++ -std=c++17 -o program main.cpp MemoryPhantom.cpp -lpsapi
```

### CMake (Full Example)
```cmake
cmake_minimum_required(VERSION 3.10)
project(GameHack)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add MemoryPhantom library
add_library(MemoryPhantom STATIC MemoryPhantom.cpp MemoryPhantom.h)

# Your executable
add_executable(GameHack main.cpp)

# Link with MemoryPhantom and Windows libraries
target_link_libraries(GameHack MemoryPhantom psapi)
```

## 🛠️ Debugging Tips

### Checking Process Attachment
```cpp
auto phantom = MemoryPhantom::CreateFromName("target.exe");
if (!phantom) {
    std::cerr << "Process not found or insufficient permissions!" << std::endl;
    return;
}

if (!phantom->IsActive()) {
    std::cerr << "Failed to attach to process!" << std::endl;
    return;
}

std::cout << "Successfully attached to PID: " << phantom->GetPID() << std::endl;
```

### Validating Memory Reads
```cpp
auto value = phantom.ReadData<int>(address);
if (!value) {
    // Read failed - possible reasons:
    // 1. Invalid address
    // 2. Process terminated
    // 3. Insufficient permissions
    // 4. Page is not readable
    std::cerr << "Failed to read memory at 0x" << std::hex << address << std::dec << std::endl;
} else {
    std::cout << "Successfully read value: " << *value << std::endl;
}
```

### Error Recovery
```cpp
void SafeMemoryOperation(MemoryPhantom& phantom, uintptr_t address) {
    if (!phantom.IsActive()) {
        // Try to reattach
        if (!phantom.Attach(phantom.GetPID())) {
            std::cerr << "Process terminated!" << std::endl;
            return;
        }
    }
    
    // Proceed with operation...
}
```

## 🔍 Troubleshooting

### Common Issues and Solutions

| Issue | Possible Cause | Solution |
|-------|---------------|----------|
| **Process not found** | Wrong process name, process not running | Check process name in Task Manager |
| **Access denied** | Insufficient permissions | Run as Administrator |
| **Invalid address** | Wrong offset, module not loaded | Verify offsets, check if module is loaded |
| **Memory read fails** | Page protection, invalid memory | Use valid addresses, check memory regions |
| **Slow performance** | Too many small reads | Use ReadBlock() for bulk operations |

### Getting Debug Information
```cpp
void DebugMemoryInfo(MemoryPhantom& phantom, const char* moduleName) {
    auto base = phantom.FindModuleBase(moduleName);
    if (base) {
        std::cout << moduleName << " base: 0x" << std::hex << *base << std::dec << std::endl;
        
        // Test read at base to verify module is accessible
        auto test = phantom.ReadData<uint32_t>(*base);
        if (test) {
            std::cout << "First DWORD: 0x" << std::hex << *test << std::dec << std::endl;
        }
    } else {
        std::cout << moduleName << " not found!" << std::endl;
    }
}
```

## 🤝 Contributing

We welcome contributions! Please follow these guidelines:

### How to Contribute
1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature`
3. Commit your changes: `git commit -m 'Add your feature'`
4. Push to the branch: `git push origin feature/your-feature`
5. Open a Pull Request

### Coding Standards
- Follow the existing code style (snake_case for methods, PascalCase for classes)
- Add explicit template instantiations for new types in both `.h` and `.cpp`
- Include comprehensive error handling
- Add comments for complex operations
- Ensure backward compatibility

### Testing
- Test with both x86 and x64 processes
- Verify edge cases (invalid addresses, terminated processes)
- Test on different Windows versions if possible

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Inspired by [Swed64](https://github.com/Massivetwat/Swed64)
- Thanks to the game hacking and reverse engineering community
- Special thanks to contributors who help improve the library

## 📞 Support

### Getting Help
- **GitHub Issues**: For bug reports and feature requests
- **Documentation**: This README and code comments
- **Community**: Game hacking forums and Discord servers

### Reporting Issues
When reporting issues, please include:
1. Windows version
2. Compiler and version
3. Target process architecture (x86/x64)
4. Error messages
5. Minimal code to reproduce

## 🚨 Disclaimer

### Intended Use Cases
- Educational purposes and learning
- Legitimate single-player game modding
- Debugging and reverse engineering of your own software
- Security research (with proper authorization)

### Prohibited Uses
- **NEVER** use for cheating in multiplayer games
- **NEVER** use for malicious software
- **NEVER** use to violate software terms of service
- **NEVER** use for illegal activities

### Legal Warning
- Using this library on software you don't own may violate laws
- Multiplayer game cheating can result in account bans
- Unauthorized memory access may be illegal in your jurisdiction

The authors are not responsible for any misuse of this library. Use responsibly and ethically.

---

⭐ **If you find this library useful, please consider giving it a star on GitHub!** ⭐

### Version Information
- **Current Version**: 1.0.0
- **Last Updated**: December 2024
- **Minimum C++**: C++17
- **Platform**: Windows (x86/x64)

Happy coding and remember: with great power comes great responsibility!
