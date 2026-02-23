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
- **📐 Built-in Vector Types**: Native support for 2D (Vector2) and 3D (Vector3) vectors

---

## 📦 Installation

1. Copy `MemoryPhantom.h`, `MemoryPhantom.cpp`, and `Vectors.h` to your project
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
    
    // Read vector types
    Vector3 position = phantom->ReadVec3(base, 0x2000); // Read 3D vector
    
    // Multiple ways to write:
    phantom->WriteInt(base, 0x1000, 999);              // With optional
    phantom->Write(*base + 0x1000, 999);               // Template style
    phantom->Write<int>(base, 0x1000, 999);            // All together
    
    // Write vector
    phantom->WriteVec3(base, 0x2000, Vector3(1.0f, 2.0f, 3.0f));
    
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

### 📐 Built-in Vector Types

```cpp
// 2D Vector (8 bytes)
class Vector2 {
public:
    float x, y;
    Vector2();
    Vector2(float x, float y);
    std::string to_string() const;
    static Vector2 Add(const Vector2& a, const Vector2& b);
    static Vector2 Subtract(const Vector2& a, const Vector2& b);
    static float Distance(const Vector2& a, const Vector2& b);
};

// 3D Vector (12 bytes)
class Vector3 {
public:
    float x, y, z;
    Vector3();
    Vector3(float x, float y, float z);
    std::string to_string() const;
    static Vector3 Add(const Vector3& a, const Vector3& b);
    static Vector3 Subtract(const Vector3& a, const Vector3& b);
    static float Distance(const Vector3& a, const Vector3& b);
};

// 4x4 Matrix (64 bytes)
struct Mat4x4 {
    float data[16];
    Mat4x4() { memset(data, 0, sizeof(data)); }
};
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

// Vector types
Vector2 ReadVec2(uintptr_t addr) const;
Vector2 ReadVec2(uintptr_t addr, int offset) const;
Vector2 ReadVec2(const std::optional<uintptr_t>& addr) const;
Vector2 ReadVec2(const std::optional<uintptr_t>& addr, int offset) const;

Vector3 ReadVec3(uintptr_t addr) const;
Vector3 ReadVec3(uintptr_t addr, int offset) const;
Vector3 ReadVec3(const std::optional<uintptr_t>& addr) const;
Vector3 ReadVec3(const std::optional<uintptr_t>& addr, int offset) const;

// Matrix
std::optional<Mat4x4> ReadMatrix(uintptr_t addr) const;
std::optional<Mat4x4> ReadMatrix(uintptr_t addr, int offset) const;
std::optional<Mat4x4> ReadMatrix(const std::optional<uintptr_t>& addr) const;
std::optional<Mat4x4> ReadMatrix(const std::optional<uintptr_t>& addr, int offset) const;

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
auto position = phantom->Read<Vector3>(playerAddress + 0x138);
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

// Vector types
bool WriteVec2(uintptr_t addr, const Vector2& vec) const;
bool WriteVec2(uintptr_t addr, int offset, const Vector2& vec) const;
bool WriteVec2(const std::optional<uintptr_t>& addr, const Vector2& vec) const;
bool WriteVec2(const std::optional<uintptr_t>& addr, int offset, const Vector2& vec) const;

bool WriteVec3(uintptr_t addr, const Vector3& vec) const;
bool WriteVec3(uintptr_t addr, int offset, const Vector3& vec) const;
bool WriteVec3(const std::optional<uintptr_t>& addr, const Vector3& vec) const;
bool WriteVec3(const std::optional<uintptr_t>& addr, int offset, const Vector3& vec) const;

// Matrix
bool WriteMatrix(uintptr_t addr, const Mat4x4& matrix) const;
bool WriteMatrix(uintptr_t addr, int offset, const Mat4x4& matrix) const;
bool WriteMatrix(const std::optional<uintptr_t>& addr, const Mat4x4& matrix) const;
bool WriteMatrix(const std::optional<uintptr_t>& addr, int offset, const Mat4x4& matrix) const;

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
phantom->Write(playerHealth, 0x100, 1337);           // Type deduction
phantom->Write<Vector3>(positionAddress, Vector3(1, 2, 3));
```

---

## 🎯 Examples

### Example 1: Modern CS2 ESP Reader with Vectors
```cpp
#include "MemoryPhantom.h"

namespace Offsets {
    constexpr auto dwLocalPlayerPawn = 0x1BEEF28;
    constexpr auto m_vOldOrigin = 0x138;        // Player position
    constexpr auto m_iHealth = 0x100;           // Player health
    constexpr auto m_iTeamNum = 0x3C;           // Team ID
    constexpr auto m_iszPlayerName = 0x6E8;     // Player name
}

class ESPReader {
    MemoryPhantom phantom;
    uintptr_t clientBase;
    uintptr_t entityList;
    
public:
    struct PlayerInfo {
        Vector3 position;    // Using built-in Vector3
        int health;
        int team;
        std::string name;
        bool valid;
        
        void print() const {
            std::cout << name << " | Health: " << health 
                      << " | Position: " << position.to_string() 
                      << " | Team: " << team << std::endl;
        }
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
        
        // Read entity pointer
        uintptr_t entity = phantom.ReadPtr(entityList, index * 0x70);
        if (!entity) return info;
        
        // Read all player data using Vector3
        info.position = phantom.ReadVec3(entity + Offsets::m_vOldOrigin);
        info.health = phantom.ReadInt(entity + Offsets::m_iHealth);
        info.team = phantom.ReadInt(entity + Offsets::m_iTeamNum);
        info.name = phantom.ReadString(entity + Offsets::m_iszPlayerName, 32);
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

int main() {
    ESPReader esp;
    if (!esp.Initialize()) {
        std::cerr << "Failed to initialize ESP reader!" << std::endl;
        return 1;
    }
    
    while (true) {
        auto players = esp.ReadAllPlayers();
        std::cout << "\n=== Players Online: " << players.size() << " ===\n";
        
        for (const auto& player : players) {
            player.print();
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
```

### Example 2: 3D Aimbot with Vector Calculations
```cpp
#include "MemoryPhantom.h"
#include <cmath>

class Aimbot {
    MemoryPhantom phantom;
    uintptr_t localPlayer;
    uintptr_t clientBase;
    
    Vector3 CalculateAngle(const Vector3& source, const Vector3& destination) {
        Vector3 angles;
        Vector3 delta = Vector3::Subtract(destination, source);
        
        float hypotenuse = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        angles.x = std::atan2(delta.z, hypotenuse) * 180.0f / 3.14159265358979323846f;
        angles.y = std::atan2(delta.y, delta.x) * 180.0f / 3.14159265358979323846f;
        angles.z = 0.0f;
        
        return angles;
    }
    
public:
    bool Initialize() {
        auto game = MemoryPhantom::CreateFromName("cs2.exe");
        if (!game) return false;
        
        phantom = std::move(*game);
        auto client = phantom.FindModuleBase("client.dll");
        if (!client) return false;
        
        clientBase = *client;
        return true;
    }
    
    void UpdateLocalPlayer() {
        localPlayer = phantom.ReadPtr(clientBase + 0x1BEEF28);
    }
    
    Vector3 GetLocalPlayerPosition() {
        return phantom.ReadVec3(localPlayer + 0x138);
    }
    
    Vector3 GetLocalPlayerViewAngles() {
        uintptr_t viewAnglesPtr = clientBase + 0x1C1D0E0;
        return phantom.ReadVec3(viewAnglesPtr);
    }
    
    void AimAt(const Vector3& targetPosition) {
        Vector3 localPos = GetLocalPlayerPosition();
        Vector3 angles = CalculateAngle(localPos, targetPosition);
        
        // Write aim angles
        uintptr_t viewAnglesPtr = clientBase + 0x1C1D0E0;
        phantom.WriteVec3(viewAnglesPtr, angles);
    }
    
    // Find closest enemy
    std::optional<Vector3> FindBestTarget() {
        Vector3 localPos = GetLocalPlayerPosition();
        float bestDistance = FLT_MAX;
        Vector3 bestTarget;
        bool found = false;
        
        // Iterate through entity list
        uintptr_t entityList = clientBase + 0x1D13CE8;
        
        for (int i = 0; i < 64; i++) {
            uintptr_t entity = phantom.ReadPtr(entityList, i * 0x70);
            if (!entity) continue;
            
            int health = phantom.ReadInt(entity + 0x100);
            int team = phantom.ReadInt(entity + 0x3C);
            
            if (health <= 0 || team == phantom.ReadInt(localPlayer + 0x3C)) {
                continue;
            }
            
            Vector3 enemyPos = phantom.ReadVec3(entity + 0x138);
            float distance = Vector3::Distance(localPos, enemyPos);
            
            if (distance < bestDistance) {
                bestDistance = distance;
                bestTarget = enemyPos;
                found = true;
            }
        }
        
        return found ? std::optional<Vector3>(bestTarget) : std::nullopt;
    }
    
    void RunAimbot() {
        UpdateLocalPlayer();
        if (!localPlayer) return;
        
        auto target = FindBestTarget();
        if (target) {
            AimAt(*target);
        }
    }
};
```

### Example 3: World-to-Screen Converter
```cpp
#include "MemoryPhantom.h"

class W2SConverter {
    MemoryPhantom phantom;
    uintptr_t clientBase;
    
public:
    struct ScreenSize {
        int width, height;
    };
    
    bool Initialize() {
        auto game = MemoryPhantom::CreateFromName("cs2.exe");
        if (!game) return false;
        
        phantom = std::move(*game);
        auto client = phantom.FindModuleBase("client.dll");
        if (!client) return false;
        
        clientBase = *client;
        return true;
    }
    
    ScreenSize GetScreenSize() {
        return { 1920, 1080 }; // Example resolution
    }
    
    std::optional<Vector2> WorldToScreen(const Vector3& worldPos) {
        // Read view matrix
        auto viewMatrix = phantom.ReadMatrix(clientBase + 0x1C1D0E0);
        if (!viewMatrix) return std::nullopt;
        
        const auto& matrix = viewMatrix->data;
        ScreenSize screen = GetScreenSize();
        
        // Apply view matrix transformation
        float w = matrix[3] * worldPos.x + matrix[7] * worldPos.y + 
                  matrix[11] * worldPos.z + matrix[15];
        
        if (w < 0.01f) return std::nullopt;
        
        float inv_w = 1.0f / w;
        float x = matrix[0] * worldPos.x + matrix[4] * worldPos.y + 
                  matrix[8] * worldPos.z + matrix[12];
        float y = matrix[1] * worldPos.x + matrix[5] * worldPos.y + 
                  matrix[9] * worldPos.z + matrix[13];
        
        // Convert to screen coordinates
        float screen_x = (screen.width / 2) * (1 + x * inv_w);
        float screen_y = (screen.height / 2) * (1 - y * inv_w);
        
        return Vector2(screen_x, screen_y);
    }
    
    bool IsOnScreen(const Vector2& screenPos, float margin = 10.0f) {
        ScreenSize screen = GetScreenSize();
        return screenPos.x >= -margin && screenPos.x <= screen.width + margin &&
               screenPos.y >= -margin && screenPos.y <= screen.height + margin;
    }
};
```

### Example 4: Teleport Hack with Vector3
```cpp
#include "MemoryPhantom.h"

class TeleportHack {
    MemoryPhantom phantom;
    uintptr_t localPlayer;
    
public:
    struct SavedPosition {
        Vector3 position;
        Vector3 viewAngles;
        std::string name;
    };
    
    std::vector<SavedPosition> savedPositions;
    
    bool Initialize() {
        auto game = MemoryPhantom::CreateFromName("cs2.exe");
        if (!game) return false;
        
        phantom = std::move(*game);
        auto client = phantom.FindModuleBase("client.dll");
        if (!client) return false;
        
        uintptr_t clientBase = *client;
        localPlayer = phantom.ReadPtr(clientBase + 0x1BEEF28);
        return localPlayer != 0;
    }
    
    SavedPosition GetCurrentPosition() {
        SavedPosition pos;
        pos.position = phantom.ReadVec3(localPlayer + 0x138);
        pos.viewAngles = phantom.ReadVec3(localPlayer + 0x140);
        pos.name = "Position " + std::to_string(savedPositions.size() + 1);
        return pos;
    }
    
    bool SavePosition(const std::string& name = "") {
        auto pos = GetCurrentPosition();
        if (!name.empty()) {
            pos.name = name;
        }
        savedPositions.push_back(pos);
        return true;
    }
    
    bool TeleportTo(const Vector3& position) {
        return phantom.WriteVec3(localPlayer + 0x138, position);
    }
    
    bool TeleportToSaved(int index) {
        if (index < 0 || index >= savedPositions.size()) {
            return false;
        }
        
        const auto& pos = savedPositions[index];
        bool posSuccess = phantom.WriteVec3(localPlayer + 0x138, pos.position);
        bool angleSuccess = phantom.WriteVec3(localPlayer + 0x140, pos.viewAngles);
        
        return posSuccess && angleSuccess;
    }
    
    void ListSavedPositions() {
        std::cout << "\n=== Saved Positions (" << savedPositions.size() << ") ===" << std::endl;
        for (size_t i = 0; i < savedPositions.size(); i++) {
            const auto& pos = savedPositions[i];
            std::cout << i << ". " << pos.name 
                      << " | Pos: " << pos.position.to_string()
                      << " | Angles: " << pos.viewAngles.to_string() << std::endl;
        }
    }
};

int main() {
    TeleportHack teleport;
    if (!teleport.Initialize()) {
        std::cerr << "Failed to initialize teleport hack!" << std::endl;
        return 1;
    }
    
    // Example usage
    teleport.SavePosition("Spawn");
    
    // Move somewhere
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    teleport.SavePosition("First Location");
    teleport.ListSavedPositions();
    
    // Teleport back to spawn
    teleport.TeleportToSaved(0);
    
    return 0;
}
```

---

## ⚡ Vector Operations

### Basic Vector Usage
```cpp
// Creating vectors
Vector2 vec2d(10.5f, 20.3f);
Vector3 vec3d(1.0f, 2.0f, 3.0f);

// String representation
std::string str2d = vec2d.to_string();  // "(10.500000, 20.300000)"
std::string str3d = vec3d.to_string();  // "(1.000000, 2.000000, 3.000000)"

// Vector operations
Vector3 a(1, 2, 3);
Vector3 b(4, 5, 6);

Vector3 sum = Vector3::Add(a, b);        // (5, 7, 9)
Vector3 diff = Vector3::Subtract(a, b);  // (-3, -3, -3)
float distance = Vector3::Distance(a, b); // ~5.196

// Reading/writing vectors from memory
Vector3 playerPos = phantom.ReadVec3(playerAddress + 0x138);
phantom.WriteVec3(playerAddress + 0x138, Vector3(100, 200, 300));
```

---

## 🛡️ Error Handling

MemoryPhantom uses a defensive programming approach:

### Safe Defaults
```cpp
// Returns default vector for invalid reads
Vector3 pos = phantom.ReadVec3(0xDEADBEEF);  // Returns Vector3(0, 0, 0)

// Returns optional with nullopt for matrices
auto matrix = phantom.ReadMatrix(badAddress);  // Returns std::nullopt

// Returns false for invalid writes
bool success = phantom.WriteVec3(0xDEADBEEF, Vector3());  // Returns false
```

### Optional Safety with Vectors
```cpp
// Safe operations with optionals
auto pos = phantom.ReadVec3(std::nullopt);           // Returns Vector3(0, 0, 0)
auto pos2 = phantom.ReadVec3(std::nullopt, 0x100);   // Returns Vector3(0, 0, 0)

// Check if vector is valid
Vector3 position = phantom.ReadVec3(address);
if (position.x != 0 || position.y != 0 || position.z != 0) {
    // Valid position
}
```

---

## 🔧 Building with Vectors

### Project Structure
```
YourProject/
├── MemoryPhantom.h
├── MemoryPhantom.cpp
├── Vectors.h        # Added vector classes
└── main.cpp
```

### Required Headers
```cpp
// main.cpp
#include "MemoryPhantom.h"  // Automatically includes Vectors.h

// Or directly include both
#include "Vectors.h"
#include "MemoryPhantom.h"
```

### Compilation
```bash
# All files are required
g++ -std=c++20 -O3 main.cpp MemoryPhantom.cpp -o app.exe -lpsapi
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

## 📄 License

MIT License - See LICENSE file for details.

**Disclaimer**: The authors are not responsible for any misuse of this library. Use responsibly and ethically.

---

## 🔗 Credits

- **MemoryPhantom Framework**: Original implementation
- **Vector Classes**: Contributed by [Mikolaj0524](https://github.com/Mikolaj0524)
- **Inspiration**: [Swed64](https://github.com/Massivetwat/Swed64)

---

**MemoryPhantom** - Complete memory manipulation with built-in vector support. 🚀

*Performance. Safety. Simplicity.*
