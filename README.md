# MemoryPhantom

**MemoryPhantom** is a high-performance C++ memory manipulation library inspired by the popular C# library [**Swed64**](https://github.com/Massivetwat/Swed64). Built from the ground up in modern C++20, it provides faster execution, better type safety, and a cleaner API while maintaining the familiar interface that made Swed64 popular.

---

## ✨ Features

- **🚀 High Performance**: Compile-time type checking with `if constexpr`, minimal overhead
- **🛡️ Type Safe**: Dedicated methods for each data type, compile-time validation
- **🎯 Smart API**: Automatic optional handling, no manual dereferencing needed
- **📦 Modern C++**: C++20 features, move semantics, RAII, `std::optional` integration
- **🧠 Multiple Interfaces**: Both explicit methods (`ReadInt`, `ReadFloat`) and template methods (`Read<T>`)
- **⚡ Optimized Memory**: Buffer reuse, efficient string handling, minimal allocations

---

## 📦 Installation

1. Copy `MemoryPhantom.h` and `MemoryPhantom.cpp` to your project
2. Include the header:
```cpp
#include "MemoryPhantom.h"
```
3. Link with Windows libraries:
```bash
-lpsapi
```

**CMake Example:**
```cmake
cmake_minimum_required(VERSION 3.16)
project(MyApp)

set(CMAKE_CXX_STANDARD 20)

add_executable(MyApp main.cpp MemoryPhantom.cpp)
target_link_libraries(MyApp psapi)
```

---

## 🚀 Quick Start

```cpp
#include "MemoryPhantom.h"

int main() {
    // Auto-attach to process
    auto phantom = MemoryPhantom::CreateFromName("notepad.exe");
    if (!phantom) return 1;
    
    // Find module (no * operator needed!)
    auto base = phantom->FindModuleBase("notepad.exe");
    
    // Multiple ways to read - all valid:
    int value1 = phantom->ReadInt(base, 0x1000);       // With optional
    int value2 = phantom->Read<int>(*base + 0x1000);   // Template style
    int value3 = phantom->ReadInt(*base + 0x1000);     // Traditional
    
    // Multiple ways to write:
    phantom->WriteInt(base, 0x1000, 999);              // With optional
    phantom->Write(*base + 0x1000, 999);               // Template style
    phantom->Write<int>(base, 0x1000, 999);            // All together
    
    return 0;
}
```

---

## 📚 API Reference

### 🏗️ Process Management

```cpp
// Smart creation - returns std::optional for safety
static std::optional<MemoryPhantom> CreateFromName(
    const char* processName, 
    DWORD accessRights = PROCESS_ALL_ACCESS
);

// Manual control
bool Attach(DWORD pid, DWORD accessRights = PROCESS_ALL_ACCESS);
void Detach();
bool IsActive() const;      // Check if process is still attached
DWORD GetPID() const;       // Get process ID
HANDLE GetHandle() const;   // Get raw Windows handle
```

### 🔍 Module Operations

```cpp
// Find loaded module - returns optional
std::optional<uintptr_t> FindModuleBase(const char* moduleName) const;
```

### 📖 Reading Memory

#### Explicit Methods (Recommended)
```cpp
// Integer types
int ReadInt(uintptr_t addr) const;
int ReadInt(uintptr_t addr, int offset) const;
int ReadInt(const std::optional<uintptr_t>& addr) const;           // Auto-handles optional
int ReadInt(const std::optional<uintptr_t>& addr, int offset) const;

unsigned int ReadUInt(uintptr_t addr) const;
int64_t ReadLong(uintptr_t addr) const;
uint64_t ReadULong(uintptr_t addr) const;
short ReadShort(uintptr_t addr) const;
unsigned short ReadUShort(uintptr_t addr) const;

// Floating point
float ReadFloat(uintptr_t addr) const;
double ReadDouble(uintptr_t addr) const;

// Boolean and bytes
bool ReadBool(uintptr_t addr) const;
char ReadChar(uintptr_t addr) const;
uint8_t ReadByte(uintptr_t addr) const;

// Strings
std::string ReadString(uintptr_t addr, size_t length) const;
std::wstring ReadWString(uintptr_t addr, size_t length) const;

// Special types
std::optional<Vec3> ReadVec3(uintptr_t addr) const;
std::optional<Mat4x4> ReadMatrix(uintptr_t addr) const;

// Pointers and blocks
uintptr_t ReadPtr(uintptr_t addr) const;
std::vector<uint8_t> ReadBytes(uintptr_t addr, size_t size) const;
```

#### Template Methods (Flexible)
```cpp
// Read any type - compile-time optimized
template<typename T>
T Read(uintptr_t addr) const;

template<typename T>
T Read(uintptr_t addr, int offset) const;

template<typename T>
T Read(const std::optional<uintptr_t>& addr) const;

template<typename T>
T Read(const std::optional<uintptr_t>& addr, int offset) const;

// Examples:
auto health = phantom->Read<int>(playerAddress + 0x100);
auto name = phantom->ReadString(moduleBase, 0x250, 32);     // Mix and match
```

### ✏️ Writing Memory

#### Explicit Methods
```cpp
// Integer types
bool WriteInt(uintptr_t addr, int value) const;
bool WriteUInt(uintptr_t addr, unsigned int value) const;
bool WriteLong(uintptr_t addr, int64_t value) const;
bool WriteULong(uintptr_t addr, uint64_t value) const;

// Floating point
bool WriteFloat(uintptr_t addr, float value) const;
bool WriteDouble(uintptr_t addr, double value) const;

// Boolean and bytes
bool WriteBool(uintptr_t addr, bool value) const;
bool WriteByte(uintptr_t addr, uint8_t value) const;

// Strings
bool WriteString(uintptr_t addr, const std::string& value) const;
bool WriteWString(uintptr_t addr, const std::wstring& value) const;

// Special types
bool WriteVec3(uintptr_t addr, const Vec3& vec) const;
bool WriteMatrix(uintptr_t addr, const Mat4x4& matrix) const;

// Bulk operations
bool WriteBytes(uintptr_t addr, const std::vector<uint8_t>& data) const;
```

#### Template Methods
```cpp
template<typename T>
bool Write(uintptr_t addr, const T& value) const;

template<typename T>
bool Write(uintptr_t addr, int offset, const T& value) const;

template<typename T>
bool Write(const std::optional<uintptr_t>& addr, const T& value) const;

// Examples:
phantom->Write<float>(playerPos, 100.5f);
phantom->Write(playerHealth, 0x100, 1337);  // Type deduction
```

### 📊 Data Structures

```cpp
// 3D Vector (12 bytes)
struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

// 4x4 Matrix (64 bytes)
struct Mat4x4 {
    float data[16];
    Mat4x4() { memset(data, 0, sizeof(data)); }
};
```

---

## 🎯 Examples

### Example 1: Modern CS2 FOV Changer
```cpp
#include "MemoryPhantom.h"

namespace Offsets {
    constexpr auto dwLocalPlayerPawn = 0x1BEEF28;
    constexpr auto m_pCameraServices = 0x1428;
    constexpr auto m_iFOV = 0x288;
    constexpr auto m_bIsScoped = 0x2718;
}

int main() {
    auto phantom = MemoryPhantom::CreateFromName("cs2.exe");
    auto client = phantom->FindModuleBase("client.dll");
    
    while (true) {
        // No dereferencing needed - optional auto-handled
        uintptr_t localPlayer = phantom->ReadPtr(client, Offsets::dwLocalPlayerPawn);
        uintptr_t cameraServices = phantom->ReadPtr(localPlayer, Offsets::m_pCameraServices);
        
        uint32_t currentFov = phantom->ReadUInt(cameraServices, Offsets::m_iFOV);
        bool isScoped = phantom->ReadBool(localPlayer, Offsets::m_bIsScoped);
        
        if (!isScoped && currentFov != 90) {
            phantom->WriteUInt(cameraServices + Offsets::m_iFOV, 90);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
```

### Example 2: ESP Reader with Caching
```cpp
#include "MemoryPhantom.h"
#include <vector>

class ESPReader {
    MemoryPhantom phantom;
    uintptr_t clientBase;
    uintptr_t entityList;
    
public:
    struct PlayerInfo {
        MemoryPhantom::Vec3 position;
        int health;
        int team;
        std::string name;
        bool valid;
    };
    
    bool Initialize() {
        auto game = MemoryPhantom::CreateFromName("cs2.exe");
        if (!game) return false;
        
        phantom = std::move(*game);
        auto client = phantom.FindModuleBase("client.dll");
        if (!client) return false;
        
        clientBase = *client;
        entityList = clientBase + 0x1D13CE8;
        return true;
    }
    
    PlayerInfo ReadPlayer(int index) {
        PlayerInfo info{};
        
        // Single call with optional - clean and safe
        uintptr_t entity = phantom.ReadPtr(entityList, index * 0x70);
        if (!entity) return info;
        
        // Bulk read operations
        info.position = phantom.ReadVec3(entity + 0x138).value_or(MemoryPhantom::Vec3{});
        info.health = phantom.ReadInt(entity + 0x100);
        info.team = phantom.ReadInt(entity + 0x3C);
        info.name = phantom.ReadString(entity + 0x6E8, 32);
        info.valid = true;
        
        return info;
    }
    
    std::vector<PlayerInfo> ReadAllPlayers(int maxPlayers = 64) {
        std::vector<PlayerInfo> players;
        players.reserve(maxPlayers);
        
        for (int i = 0; i < maxPlayers; i++) {
            auto player = ReadPlayer(i);
            if (player.valid) {
                players.push_back(std::move(player));
            }
        }
        
        return players;
    }
};
```

### Example 3: Pattern Scanner (Performance Optimized)
```cpp
#include "MemoryPhantom.h"
#include <span>

class PatternScanner {
    MemoryPhantom& phantom;
    
public:
    PatternScanner(MemoryPhantom& phantom) : phantom(phantom) {}
    
    std::vector<uintptr_t> Scan(
        std::string_view pattern,
        uintptr_t start,
        uintptr_t end,
        size_t step = 1
    ) {
        std::vector<uintptr_t> results;
        auto patternBytes = ParsePattern(pattern);
        
        // Read large blocks for performance
        const size_t BLOCK_SIZE = 1024 * 1024; // 1MB
        
        for (uintptr_t addr = start; addr < end; addr += BLOCK_SIZE) {
            size_t readSize = std::min(BLOCK_SIZE, (size_t)(end - addr));
            auto block = phantom.ReadBytes(addr, readSize);
            
            if (block.size() < patternBytes.size()) continue;
            
            // Fast byte-by-byte scan
            for (size_t i = 0; i <= block.size() - patternBytes.size(); i += step) {
                if (ComparePattern(block.data() + i, patternBytes)) {
                    results.push_back(addr + i);
                }
            }
        }
        
        return results;
    }
    
private:
    struct PatternByte {
        uint8_t value;
        bool wildcard;
    };
    
    std::vector<PatternByte> ParsePattern(std::string_view pattern) {
        std::vector<PatternByte> bytes;
        std::stringstream ss(std::string(pattern));
        std::string token;
        
        while (ss >> token) {
            if (token == "??") {
                bytes.push_back({0, true});
            } else {
                bytes.push_back({(uint8_t)std::stoul(token, nullptr, 16), false});
            }
        }
        
        return bytes;
    }
    
    bool ComparePattern(const uint8_t* data, const std::vector<PatternByte>& pattern) {
        for (size_t i = 0; i < pattern.size(); i++) {
            if (!pattern[i].wildcard && data[i] != pattern[i].value) {
                return false;
            }
        }
        return true;
    }
};
```

### Example 4: Memory Hook with Safety
```cpp
#include "MemoryPhantom.h"

class MemoryHook {
    MemoryPhantom& phantom;
    uintptr_t address;
    std::vector<uint8_t> original;
    std::vector<uint8_t> hook;
    bool installed;
    
public:
    MemoryHook(MemoryPhantom& phantom, uintptr_t addr, std::vector<uint8_t> hookBytes)
        : phantom(phantom), address(addr), hook(std::move(hookBytes)), installed(false) {
        
        // Backup original bytes
        original = phantom.ReadBytes(address, hook.size());
    }
    
    bool Install() {
        if (installed || !phantom.IsActive()) return false;
        
        if (phantom.WriteBytes(address, hook)) {
            installed = true;
            return true;
        }
        return false;
    }
    
    bool Remove() {
        if (!installed) return false;
        
        if (phantom.WriteBytes(address, original)) {
            installed = false;
            return true;
        }
        return false;
    }
    
    ~MemoryHook() {
        if (installed) {
            Remove();
        }
    }
    
    bool IsInstalled() const { return installed; }
    uintptr_t GetAddress() const { return address; }
};
```

---

## ⚡ Performance Features

### Why MemoryPhantom is Faster:

1. **Compile-Time Optimization**: Uses `if constexpr` to eliminate runtime type checks
2. **Buffer Reuse**: Internal buffers are reused to minimize allocations
3. **Optional Integration**: No manual `*` dereferencing, automatic null handling
4. **Type-Specific Methods**: Direct calls to optimized implementations
5. **Minimal Overhead**: Thin wrapper around Windows API with no extra layers

### Benchmark Comparison:
```cpp
// Traditional approach (slower)
auto base = phantom->FindModuleBase("client.dll");
if (base) {
    auto player = phantom->ReadPtr(*base + offset1);
    if (player) {
        auto health = phantom->ReadInt(*player + offset2);
    }
}

// MemoryPhantom approach (faster, cleaner)
auto health = phantom->ReadInt(phantom->FindModuleBase("client.dll"), 
                               offset1 + offset2);
```

---

## 🛡️ Error Handling

MemoryPhantom uses a defensive programming approach:

### Safe Defaults
```cpp
// Returns 0 for invalid reads
int health = phantom->ReadInt(0xDEADBEEF);  // Returns 0, no crash

// Returns empty string
auto name = phantom->ReadString(badAddress, 100);  // Returns ""

// Returns false for invalid writes
bool success = phantom->WriteInt(0xDEADBEEF, 100);  // Returns false
```

### Optional Safety
```cpp
// All these are safe:
auto value1 = phantom->ReadInt(std::nullopt);           // Returns 0
auto value2 = phantom->ReadInt(std::nullopt, 0x100);    // Returns 0
auto value3 = phantom->ReadString(std::nullopt, 50);    // Returns ""
```

### Process State Checking
```cpp
if (!phantom.IsActive()) {
    // Process terminated or detached
    // Re-attach or cleanup
}
```

---

## 🔧 Advanced Usage

### Chaining Operations
```cpp
// Read pointer chain in one line
auto weapon = phantom->ReadPtr(
    phantom->ReadPtr(
        phantom->ReadPtr(client, localPlayerOffset),
        weaponControllerOffset
    ),
    currentWeaponOffset
);
```

### Batch Operations
```cpp
// Read multiple values at once
struct PlayerState {
    int health, armor, ammo;
    Vec3 position;
};

PlayerState ReadPlayerState(uintptr_t player) {
    return {
        phantom.ReadInt(player + 0x100),
        phantom.ReadInt(player + 0x104),
        phantom.ReadInt(player + 0x150),
        phantom.ReadVec3(player + 0x138).value_or(Vec3{})
    };
}
```

### Custom Type Support
```cpp
struct MyCustomType {
    int id;
    float data[4];
    char name[32];
};

// Read custom type using template
MyCustomType data = phantom->Read<MyCustomType>(address);

// Or read individual fields
auto customId = phantom.ReadInt(address + offsetof(MyCustomType, id));
```

---

## ⚠️ Important Notes

### Performance Tips
- ✅ **Cache addresses** - Don't re-calculate offsets every frame
- ✅ **Use explicit methods** (`ReadInt`, `ReadFloat`) for known types
- ✅ **Batch reads** when accessing multiple nearby values
- ✅ **Check `IsActive()`** before loops
- ❌ **Avoid** frequent string reads in hot loops
- ❌ **Don't** create/destroy MemoryPhantom objects frequently

### Common Patterns
```cpp
// Good: Cache module base
auto client = phantom.FindModuleBase("client.dll");
uintptr_t localPlayerBase = *client + 0x1BEEF28;

// Bad: Re-calculate every time
for (int i = 0; i < 100; i++) {
    auto player = phantom.ReadPtr(phantom.FindModuleBase("client.dll").value() + 0x1BEEF28);
}

// Good: Batch reads
auto ReadPlayerInfo(uintptr_t player) {
    return std::make_tuple(
        phantom.ReadInt(player + 0x100),  // health
        phantom.ReadInt(player + 0x104),  // armor
        phantom.ReadVec3(player + 0x138)  // position
    );
}

// Bad: Multiple separate reads
auto health = phantom.ReadInt(player + 0x100);
auto armor = phantom.ReadInt(player + 0x104);
auto pos = phantom.ReadVec3(player + 0x138);
```

---

## 🚨 Safety & Legal

### Intended Use
- ✅ Game modding (single-player)
- ✅ Reverse engineering your own software
- ✅ Educational purposes
- ✅ Debugging and testing

### Prohibited Use
- ❌ Multiplayer game cheating
- ❌ Malicious software
- ❌ Privacy invasion
- ❌ Copyright violation

### Anti-Cheat Warning
Many games have sophisticated anti-cheat systems that:
- Detect memory reading/writing
- Ban accounts permanently
- May have legal consequences
- Can damage your system

**Use at your own risk!**

---

## 🔧 Building

### Visual Studio 2022+
1. Create new C++ Console App
2. Set C++ Language Standard: **C++20**
3. Add `MemoryPhantom.h` and `MemoryPhantom.cpp`
4. Linker → Input → Additional Dependencies: `psapi.lib`

### Command Line (MSVC)
```bash
cl /std:c++20 /EHsc /O2 main.cpp MemoryPhantom.cpp /link psapi.lib
```

### Command Line (GCC/MinGW)
```bash
g++ -std=c++20 -O3 main.cpp MemoryPhantom.cpp -o app.exe -lpsapi
```

### CMake (Cross-Platform Windows)
```cmake
cmake_minimum_required(VERSION 3.20)
project(GameTool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_executable(GameTool main.cpp MemoryPhantom.cpp)
    target_link_libraries(GameTool psapi)
else()
    message(FATAL_ERROR "MemoryPhantom requires Windows")
endif()
```

---

## 📞 Support & Contributing

### Getting Help
- Check the examples in this README
- Review inline documentation in header file
- Create GitHub issue for bugs

### Reporting Issues
Include:
1. Windows version
2. Compiler and version
3. Target process (32/64-bit)
4. Error or unexpected behavior
5. Minimal reproducible code

### Contributing
We welcome contributions! Please:
1. Follow existing code style
2. Add tests if possible
3. Update documentation
4. Ensure backward compatibility

---

## 📄 License

MIT License - See LICENSE file for details.

**Disclaimer**: The authors are not responsible for any misuse of this library. Use responsibly and ethically.

---

## 🔗 Links & Inspiration

- **[Swed64](https://github.com/Massivetwat/Swed64)** - Original C# inspiration
- **[GameReverse](https://guidedhacking.com/)** - Learning resource

---

**MemoryPhantom** - Because C++ should be fast, safe, and elegant. 🚀

*Performance. Safety. Simplicity.*
