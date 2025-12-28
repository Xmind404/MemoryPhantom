# MemoryPhantom

**MemoryPhantom** is a lightweight C++ memory manipulation library inspired by the popular C# library [**Swed64**](https://github.com/Massivetwat/Swed64). It provides a clean, modern interface for reading and writing process memory on Windows systems, perfect for game hacking, debugging, and process monitoring applications.

---

## ✨ Features

- **Process Management**: Attach to processes by name or PID
- **Module Discovery**: Find base addresses of loaded modules (DLLs, EXEs)
- **Memory Operations**: Read/write all common data types with dedicated methods
- **Vector/Matrix Support**: Built-in Vec3 and 4x4 matrix operations
- **String Handling**: Read/write ANSI and Unicode strings
- **Error Safe**: Returns default values instead of throwing on errors
- **Lightweight**: Single header and source file, no external dependencies beyond Windows SDK

---

## 📦 Installation

1. Copy `MemoryPhantom.h` and `MemoryPhantom.cpp` to your project
2. Add to your source:
```cpp
#include "MemoryPhantom.h"
```
3. Link with `psapi.lib`

**CMake Example:**
```cmake
add_executable(YourApp main.cpp MemoryPhantom.cpp)
target_link_libraries(YourApp psapi)
```

---

## 🚀 Quick Start

```cpp
#include "MemoryPhantom.h"

int main() {
    // Create from process name
    auto phantom = MemoryPhantom::CreateFromName("notepad.exe");
    if (!phantom) return 1;
    
    // Find module base
    auto base = phantom->FindModuleBase("notepad.exe");
    if (!base) return 1;
    
    // Read values
    int value = phantom->ReadInt(*base + 0x1000);
    float posX = phantom->ReadFloat(*base + 0x2000);
    
    // Write values
    phantom->WriteInt(*base + 0x1000, 999);
    phantom->WriteFloat(*base + 0x2000, 123.45f);
    
    return 0;
}
```

---

## 📚 API Reference

### Process Management

```cpp
// Static creation
static std::optional<MemoryPhantom> CreateFromName(const char* processName, DWORD accessRights = PROCESS_ALL_ACCESS);

// Manual attachment
bool Attach(DWORD pid, DWORD accessRights = PROCESS_ALL_ACCESS);
void Detach();

// Status
bool IsActive() const;
DWORD GetPID() const;
HANDLE GetHandle() const;
```

### Module Operations

```cpp
std::optional<uintptr_t> FindModuleBase(const char* moduleName) const;
```

### Reading Memory

```cpp
// Basic types
int ReadInt(uintptr_t addr) const;
int ReadInt(uintptr_t addr, int offset) const;

float ReadFloat(uintptr_t addr) const;
float ReadFloat(uintptr_t addr, int offset) const;

double ReadDouble(uintptr_t addr) const;
double ReadDouble(uintptr_t addr, int offset) const;

short ReadShort(uintptr_t addr) const;
short ReadShort(uintptr_t addr, int offset) const;

unsigned short ReadUShort(uintptr_t addr) const;
unsigned short ReadUShort(uintptr_t addr, int offset) const;

unsigned int ReadUInt(uintptr_t addr) const;
unsigned int ReadUInt(uintptr_t addr, int offset) const;

uint64_t ReadULong(uintptr_t addr) const;
uint64_t ReadULong(uintptr_t addr, int offset) const;

int64_t ReadLong(uintptr_t addr) const;
int64_t ReadLong(uintptr_t addr, int offset) const;

bool ReadBool(uintptr_t addr) const;
bool ReadBool(uintptr_t addr, int offset) const;

char ReadChar(uintptr_t addr) const;
char ReadChar(uintptr_t addr, int offset) const;

uint8_t ReadByte(uintptr_t addr) const;
uint8_t ReadByte(uintptr_t addr, int offset) const;

// Strings
std::string ReadString(uintptr_t addr, size_t length) const;
std::string ReadString(uintptr_t addr, int offset, size_t length) const;

std::wstring ReadWString(uintptr_t addr, size_t length) const;
std::wstring ReadWString(uintptr_t addr, int offset, size_t length) const;

// Complex types
std::optional<Vec3> ReadVec3(uintptr_t addr) const;
std::optional<Vec3> ReadVec3(uintptr_t addr, int offset) const;

std::optional<Mat4x4> ReadMatrix(uintptr_t addr) const;
std::optional<Mat4x4> ReadMatrix(uintptr_t addr, int offset) const;

// Pointers and blocks
uintptr_t ReadPtr(uintptr_t addr) const;
uintptr_t ReadPtr(uintptr_t addr, int offset) const;

std::vector<uint8_t> ReadBytes(uintptr_t addr, size_t sz) const;
std::vector<uint8_t> ReadBytes(uintptr_t addr, int offset, size_t sz) const;
```

### Writing Memory

```cpp
// Basic types
bool WriteInt(uintptr_t addr, int value) const;
bool WriteInt(uintptr_t addr, int offset, int value) const;

bool WriteFloat(uintptr_t addr, float value) const;
bool WriteFloat(uintptr_t addr, int offset, float value) const;

bool WriteDouble(uintptr_t addr, double value) const;
bool WriteDouble(uintptr_t addr, int offset, double value) const;

bool WriteShort(uintptr_t addr, short value) const;
bool WriteShort(uintptr_t addr, int offset, short value) const;

bool WriteUShort(uintptr_t addr, unsigned short value) const;
bool WriteUShort(uintptr_t addr, int offset, unsigned short value) const;

bool WriteUInt(uintptr_t addr, unsigned int value) const;
bool WriteUInt(uintptr_t addr, int offset, unsigned int value) const;

bool WriteULong(uintptr_t addr, uint64_t value) const;
bool WriteULong(uintptr_t addr, int offset, uint64_t value) const;

bool WriteLong(uintptr_t addr, int64_t value) const;
bool WriteLong(uintptr_t addr, int offset, int64_t value) const;

bool WriteBool(uintptr_t addr, bool value) const;
bool WriteBool(uintptr_t addr, int offset, bool value) const;

bool WriteChar(uintptr_t addr, char value) const;
bool WriteChar(uintptr_t addr, int offset, char value) const;

bool WriteByte(uintptr_t addr, uint8_t value) const;
bool WriteByte(uintptr_t addr, int offset, uint8_t value) const;

// Strings
bool WriteString(uintptr_t addr, const std::string& value) const;
bool WriteString(uintptr_t addr, int offset, const std::string& value) const;

bool WriteWString(uintptr_t addr, const std::wstring& value) const;
bool WriteWString(uintptr_t addr, int offset, const std::wstring& value) const;

// Complex types
bool WriteVec3(uintptr_t addr, const Vec3& vec) const;
bool WriteVec3(uintptr_t addr, int offset, const Vec3& vec) const;

bool WriteMatrix(uintptr_t addr, const Mat4x4& matrix) const;
bool WriteMatrix(uintptr_t addr, int offset, const Mat4x4& matrix) const;

// Blocks
bool WriteBytes(uintptr_t addr, const std::vector<uint8_t>& data) const;
bool WriteBytes(uintptr_t addr, int offset, const std::vector<uint8_t>& data) const;
```

### Data Structures

```cpp
struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Mat4x4 {
    float data[16];
    Mat4x4() { memset(data, 0, sizeof(data)); }
};
```

---

## 🎯 Examples

### Example 1: CS2 Wallhack (Radar)
```cpp
#include "MemoryPhantom.h"
#include <thread>

int main() {
    auto phantom = MemoryPhantom::CreateFromName("cs2.exe");
    if (!phantom) return 1;

    auto client = phantom->FindModuleBase("client.dll");
    if (!client) return 1;

    uintptr_t entityList = *client + 0x1D13CE8;
    uintptr_t localPlayer = phantom->ReadPtr(*client + 0x1BEEF28);
    
    for (int i = 0; i < 64; i++) {
        uintptr_t entity = phantom->ReadPtr(entityList, i * 0x70);
        if (entity == 0) continue;
        
        uintptr_t pawnHandle = phantom->ReadUInt(entity + 0x8FC);
        uintptr_t listEntry = phantom->ReadPtr(entityList, 0x8 * ((pawnHandle & 0x7FFF) >> 9) + 0x10);
        uintptr_t currentPawn = phantom->ReadPtr(listEntry, 0x70 * (pawnHandle & 0x1FF));
        
        phantom->WriteBool(currentPawn + 0x2708, true);
    }
    
    return 0;
}
```

### Example 2: Player Info Reader
```cpp
#include "MemoryPhantom.h"
#include <iostream>

struct Player {
    MemoryPhantom::Vec3 position;
    int health;
    std::string name;
};

class GameHack {
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
    
    Player ReadPlayer(int index) {
        Player player;
        uintptr_t entityList = clientBase + 0x1D13CE8;
        uintptr_t entity = phantom.ReadPtr(entityList, index * 0x70);
        
        if (entity != 0) {
            auto pos = phantom.ReadVec3(entity + 0x138);
            if (pos) player.position = *pos;
            
            player.health = phantom.ReadInt(entity + 0x100);
            player.name = phantom.ReadString(entity + 0x6E8, 32);
        }
        
        return player;
    }
};
```

---

## ⚠️ Important Notes

### Performance Tips
- Cache frequently accessed addresses
- Use appropriate types (ReadInt vs ReadFloat)
- Batch reads when possible
- Check `IsActive()` before operations

### Error Handling
- All methods return safe defaults on failure
- Check `IsActive()` after creation
- Invalid addresses return zeros/empty values
- Process termination is automatically detected

### Permissions
- Run as Administrator for full access
- Some processes require elevated privileges
- Anti-cheat software may block access

### Safety
- **For educational purposes only**
- Don't use on software you don't own
- Multiplayer cheating is unethical and often illegal
- Single-player games only!

---

## 🔧 Building

**Visual Studio:**
- Add `MemoryPhantom.cpp` to project
- Set C++17 or higher
- Link with `psapi.lib`

**Command Line:**
```bash
g++ -std=c++17 main.cpp MemoryPhantom.cpp -o app.exe -lpsapi
cl /std:c++17 main.cpp MemoryPhantom.cpp /link psapi.lib
```

---

## 📞 Support

- **Issues**: GitHub repository
- **Questions**: Check examples first
- **Contributions**: Pull requests welcome

---

## 📄 License

MIT License - Use responsibly!

---

[**Inspired by Swed64**](https://github.com/Massivetwat/Swed64) - Bringing C#-style memory operations to C++!
