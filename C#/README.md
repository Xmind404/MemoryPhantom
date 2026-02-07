# MemoryPhantom.NET

**MemoryPhantom.NET** is a high-performance C# memory manipulation library inspired by the popular C# library [**Swed64**](https://github.com/Massivetwat/Swed64). Built from the ground up with modern C# optimizations, it provides faster execution, better type safety, and a cleaner API while maintaining the familiar interface that made Swed64 popular.

---

## ✨ Features

- **🚀 High Performance**: Uses `unsafe`, `Span<T>`, `stackalloc`, and aggressive inlining for C++-like performance
- **🔍 Pattern Scanning**: Advanced pattern scanning with wildcard support
- **📐 Built-in Vector Types**: Native support for Vector2, Vector3, and 4x4 matrices
- **🧠 Intelligent Caching**: Smart caching for frequently accessed addresses
- **⚡ Batch Operations**: Group operations for minimal overhead
- **🛡️ Type Safe**: Compile-time type checking with `unmanaged` constraints
- **📊 Architecture Detection**: Automatic 32/64-bit process detection
- **🎯 Multiple Interfaces**: Both explicit methods and generic methods for flexibility

---

## 📦 Installation

### Requirements
- .NET 6.0+ or .NET Framework 4.7.2+
- Windows 10/11
- Administrator privileges (for some operations)

### Adding to Project
1. Copy the `MemoryPhantom.cs` file to your project
2. Enable `unsafe` code in your project file:

```xml
<PropertyGroup>
    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>
</PropertyGroup>
```

3. Add required namespace references:

```csharp
using System;
using System.Runtime.InteropServices;
using System.Diagnostics;
```

---

## 🚀 Quick Start

```csharp
using System;

class Program
{
    static void Main()
    {
        // Auto-attach to process
        using var phantom = MemoryPhantom.CreateFromName("notepad.exe");
        if (phantom == null || !phantom.IsActive)
        {
            Console.WriteLine("Failed to attach to process");
            return;
        }
        
        // Find module base
        var baseAddress = phantom.FindModuleBase("notepad.exe");
        if (!baseAddress.HasValue)
        {
            Console.WriteLine("Module not found");
            return;
        }
        
        // Read various data types
        int health = phantom.ReadInt(baseAddress.Value + 0x1000);
        float positionX = phantom.ReadFloat(baseAddress.Value + 0x1010);
        string playerName = phantom.ReadString(baseAddress.Value + 0x1020, 32);
        Vector3 playerPos = phantom.ReadVec3(baseAddress.Value + 0x1030);
        
        Console.WriteLine($"Health: {health}, Position: {playerPos}");
        
        // Write data
        phantom.WriteInt(baseAddress.Value + 0x1000, 100);
        phantom.WriteVec3(baseAddress.Value + 0x1030, new Vector3(100, 200, 300));
        
        // Pattern scanning
        var patternAddress = phantom.ScanPatternModule(
            "client.dll", 
            "48 8B 05 ?? ?? ?? ?? 48 85 C0 74 05", 
            "xxx????xxxxx"
        );
        
        if (patternAddress.HasValue)
        {
            Console.WriteLine($"Found pattern at: 0x{patternAddress.Value:X}");
        }
    }
}
```

---

## 📚 API Reference

### 🏗️ Process Management

```csharp
// Smart creation - returns null for safety
public static MemoryPhantom? CreateFromName(
    string processName, 
    uint accessRights = 0x1F0FFF
);

// Manual control
public bool Attach(uint pid, uint accessRights = 0x1F0FFF);
public void Detach();

// Properties
public bool IsActive { get; }          // Check if process is still attached
public uint GetPID();                  // Get process ID
public IntPtr GetHandle();             // Get raw Windows handle
public bool Is64Bit { get; }           // Check process architecture
```

### 🔍 Module Operations

```csharp
// Find loaded module - returns nullable
public ulong? FindModuleBase(string moduleName);

// Get module information
public ModuleInfo? GetModuleInfo(string moduleName);
```

### 🔍 Pattern Scanning

```csharp
// Scan memory region
public ulong? ScanPattern(
    ulong start, 
    ulong end, 
    string pattern, 
    string mask
);

// Scan entire module
public ulong? ScanPatternModule(
    string moduleName, 
    string pattern, 
    string mask
);

// Scan with byte array
public ulong? AobScan(
    ulong start, 
    ulong end, 
    byte[] pattern, 
    string mask = null
);
```

### 📖 Reading Memory

#### Explicit Methods (Recommended)
```csharp
// Integer types
int ReadInt(ulong addr);
int ReadInt(ulong addr, int offset);
int ReadInt(ulong? addr);
int ReadInt(ulong? addr, int offset);

uint ReadUInt(ulong addr);
long ReadLong(ulong addr);
ulong ReadULong(ulong addr);
short ReadShort(ulong addr);
ushort ReadUShort(ulong addr);

// Floating point
float ReadFloat(ulong addr);
double ReadDouble(ulong addr);

// Boolean and bytes
bool ReadBool(ulong addr);
char ReadChar(ulong addr);
byte ReadByte(ulong addr);

// Strings
string ReadString(ulong addr, uint length);
string ReadWString(ulong addr, uint length);  // Unicode

// Vector types
Vector2 ReadVec2(ulong addr);
Vector3 ReadVec3(ulong addr);
Mat4x4? ReadMatrix(ulong addr);

// Pointers and blocks
ulong ReadPtr(ulong addr);
byte[] ReadBytes(ulong addr, uint size);
```

#### Template Methods (Flexible)
```csharp
// Read any unmanaged type - compile-time optimized
T Read<T>(ulong addr) where T : unmanaged;
T Read<T>(ulong addr, int offset) where T : unmanaged;
T Read<T>(ulong? addr) where T : unmanaged;
T Read<T>(ulong? addr, int offset) where T : unmanaged;

// Examples:
var health = phantom.Read<int>(playerAddress + 0x100);
var position = phantom.Read<Vector3>(playerAddress + 0x138);
var matrix = phantom.Read<Mat4x4>(viewMatrixAddress);
```

### ✏️ Writing Memory

#### Explicit Methods
```csharp
// Integer types
bool WriteInt(ulong addr, int value);
bool WriteUInt(ulong addr, uint value);
bool WriteLong(ulong addr, long value);
bool WriteULong(ulong addr, ulong value);

// Floating point
bool WriteFloat(ulong addr, float value);
bool WriteDouble(ulong addr, double value);

// Boolean and bytes
bool WriteBool(ulong addr, bool value);
bool WriteByte(ulong addr, byte value);

// Strings
bool WriteString(ulong addr, string value);
bool WriteWString(ulong addr, string value);  // Unicode

// Vector types
bool WriteVec2(ulong addr, Vector2 vec);
bool WriteVec3(ulong addr, Vector3 vec);
bool WriteMatrix(ulong addr, Mat4x4 matrix);

// Bulk operations
bool WriteBytes(ulong addr, byte[] data);
```

#### Template Methods
```csharp
// Write any unmanaged type
bool Write<T>(ulong addr, T value) where T : unmanaged;
bool Write<T>(ulong addr, int offset, T value) where T : unmanaged;
bool Write<T>(ulong? addr, T value) where T : unmanaged;

// Examples:
phantom.Write<float>(playerAddress, 100.5f);
phantom.Write(playerAddress + 0x100, 1337);           // Type deduction
phantom.Write<Vector3>(positionAddress, new Vector3(1, 2, 3));
```

### ⚡ Advanced Operations

#### Batch Operations
```csharp
bool ReadBatch(params (ulong address, Span<byte> buffer)[] operations);
bool WriteBatch(params (ulong address, ReadOnlySpan<byte> data)[] operations);
```

#### Span Operations
```csharp
bool ReadSpan(ulong addr, Span<byte> buffer);
bool WriteSpan(ulong addr, ReadOnlySpan<byte> buffer);
```

#### Caching
```csharp
T ReadCached<T>(ulong addr) where T : unmanaged;
void ClearCache();
```

---

## 🎯 Examples

### Example 1: Modern CS2 ESP Reader with Vector3
```csharp
using System;
using System.Collections.Generic;

namespace CS2ESP
{
    public class ESPReader : IDisposable
    {
        private readonly MemoryPhantom phantom;
        private ulong clientBase;
        private ulong entityList;
        
        // Offsets
        private const ulong dwLocalPlayerPawn = 0x1BEEF28;
        private const ulong m_vOldOrigin = 0x138;
        private const ulong m_iHealth = 0x100;
        private const ulong m_iTeamNum = 0x3C;
        private const ulong m_iszPlayerName = 0x6E8;
        
        public class PlayerInfo
        {
            public Vector3 Position { get; set; }
            public int Health { get; set; }
            public int Team { get; set; }
            public string Name { get; set; }
            public bool IsValid { get; set; }
            
            public void Print()
            {
                Console.WriteLine($"{Name} | Health: {Health} | Position: {Position} | Team: {Team}");
            }
        }
        
        public ESPReader()
        {
            phantom = MemoryPhantom.CreateFromName("cs2.exe");
            if (phantom == null || !phantom.IsActive)
                throw new Exception("Failed to attach to CS2 process");
            
            var client = phantom.FindModuleBase("client.dll");
            if (!client.HasValue)
                throw new Exception("client.dll not found");
                
            clientBase = client.Value;
            entityList = clientBase + 0x1D13CE8;
        }
        
        public PlayerInfo ReadPlayer(int index)
        {
            var info = new PlayerInfo();
            
            // Read entity pointer
            ulong entity = phantom.ReadPtr(entityList + (ulong)(index * 0x70));
            if (entity == 0) return info;
            
            // Read all player data using Vector3
            info.Position = phantom.ReadVec3(entity + m_vOldOrigin);
            info.Health = phantom.ReadInt(entity + m_iHealth);
            info.Team = phantom.ReadInt(entity + m_iTeamNum);
            info.Name = phantom.ReadString(entity + m_iszPlayerName, 32);
            info.IsValid = true;
            
            return info;
        }
        
        public List<PlayerInfo> ReadAllPlayers(int maxPlayers = 64)
        {
            var players = new List<PlayerInfo>(maxPlayers);
            
            for (int i = 0; i < maxPlayers; i++)
            {
                var player = ReadPlayer(i);
                if (player.IsValid)
                {
                    players.Add(player);
                }
            }
            
            return players;
        }
        
        public void Dispose()
        {
            phantom?.Dispose();
        }
    }
    
    class Program
    {
        static void Main()
        {
            try
            {
                using var esp = new ESPReader();
                
                while (true)
                {
                    var players = esp.ReadAllPlayers();
                    Console.Clear();
                    Console.WriteLine($"=== Players Online: {players.Count} ===\n");
                    
                    foreach (var player in players)
                    {
                        player.Print();
                    }
                    
                    System.Threading.Thread.Sleep(1000);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error: {ex.Message}");
            }
        }
    }
}
```

### Example 2: 3D Aimbot with Vector Calculations
```csharp
using System;
using System.Linq;

namespace AimbotExample
{
    public class Aimbot : IDisposable
    {
        private readonly MemoryPhantom phantom;
        private ulong localPlayer;
        private ulong clientBase;
        
        private Vector3 CalculateAngle(Vector3 source, Vector3 destination)
        {
            Vector3 angles = new Vector3();
            Vector3 delta = destination - source;
            
            float hypotenuse = MathF.Sqrt(delta.X * delta.X + delta.Y * delta.Y);
            angles.X = MathF.Atan2(delta.Z, hypotenuse) * (180.0f / MathF.PI);
            angles.Y = MathF.Atan2(delta.Y, delta.X) * (180.0f / MathF.PI);
            angles.Z = 0.0f;
            
            return angles;
        }
        
        public Aimbot()
        {
            phantom = MemoryPhantom.CreateFromName("cs2.exe");
            if (phantom == null || !phantom.IsActive)
                throw new Exception("Failed to attach to process");
            
            var client = phantom.FindModuleBase("client.dll");
            if (!client.HasValue)
                throw new Exception("client.dll not found");
                
            clientBase = client.Value;
        }
        
        public void UpdateLocalPlayer()
        {
            localPlayer = phantom.ReadPtr(clientBase + 0x1BEEF28);
        }
        
        public Vector3 GetLocalPlayerPosition()
        {
            return phantom.ReadVec3(localPlayer + 0x138);
        }
        
        public Vector3 GetLocalPlayerViewAngles()
        {
            ulong viewAnglesPtr = clientBase + 0x1C1D0E0;
            return phantom.ReadVec3(viewAnglesPtr);
        }
        
        public void AimAt(Vector3 targetPosition)
        {
            Vector3 localPos = GetLocalPlayerPosition();
            Vector3 angles = CalculateAngle(localPos, targetPosition);
            
            // Write aim angles
            ulong viewAnglesPtr = clientBase + 0x1C1D0E0;
            phantom.WriteVec3(viewAnglesPtr, angles);
        }
        
        public Vector3? FindBestTarget()
        {
            Vector3 localPos = GetLocalPlayerPosition();
            float bestDistance = float.MaxValue;
            Vector3 bestTarget = new Vector3();
            bool found = false;
            
            // Iterate through entity list
            ulong entityList = clientBase + 0x1D13CE8;
            
            for (int i = 0; i < 64; i++)
            {
                ulong entity = phantom.ReadPtr(entityList + (ulong)(i * 0x70));
                if (entity == 0) continue;
                
                int health = phantom.ReadInt(entity + 0x100);
                int team = phantom.ReadInt(entity + 0x3C);
                int localTeam = phantom.ReadInt(localPlayer + 0x3C);
                
                if (health <= 0 || team == localTeam)
                {
                    continue;
                }
                
                Vector3 enemyPos = phantom.ReadVec3(entity + 0x138);
                float distance = Vector3.Distance(localPos, enemyPos);
                
                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    bestTarget = enemyPos;
                    found = true;
                }
            }
            
            return found ? bestTarget : null;
        }
        
        public void RunAimbot()
        {
            UpdateLocalPlayer();
            if (localPlayer == 0) return;
            
            var target = FindBestTarget();
            if (target.HasValue)
            {
                AimAt(target.Value);
            }
        }
        
        public void Dispose()
        {
            phantom?.Dispose();
        }
    }
}
```

### Example 3: World-to-Screen Converter
```csharp
using System;

namespace WorldToScreenExample
{
    public class W2SConverter : IDisposable
    {
        private readonly MemoryPhantom phantom;
        private ulong clientBase;
        
        public struct ScreenSize
        {
            public int Width, Height;
        }
        
        public W2SConverter()
        {
            phantom = MemoryPhantom.CreateFromName("cs2.exe");
            if (phantom == null || !phantom.IsActive)
                throw new Exception("Failed to attach to process");
            
            var client = phantom.FindModuleBase("client.dll");
            if (!client.HasValue)
                throw new Exception("client.dll not found");
                
            clientBase = client.Value;
        }
        
        public ScreenSize GetScreenSize()
        {
            return new ScreenSize { Width = 1920, Height = 1080 };
        }
        
        public Vector2? WorldToScreen(Vector3 worldPos)
        {
            // Read view matrix
            var viewMatrix = phantom.ReadMatrix(clientBase + 0x1C1D0E0);
            if (!viewMatrix.HasValue) return null;
            
            var matrix = viewMatrix.Value.Data;
            ScreenSize screen = GetScreenSize();
            
            // Apply view matrix transformation
            float w = matrix[3] * worldPos.X + matrix[7] * worldPos.Y + 
                      matrix[11] * worldPos.Z + matrix[15];
            
            if (w < 0.01f) return null;
            
            float inv_w = 1.0f / w;
            float x = matrix[0] * worldPos.X + matrix[4] * worldPos.Y + 
                      matrix[8] * worldPos.Z + matrix[12];
            float y = matrix[1] * worldPos.X + matrix[5] * worldPos.Y + 
                      matrix[9] * worldPos.Z + matrix[13];
            
            // Convert to screen coordinates
            float screenX = (screen.Width / 2) * (1 + x * inv_w);
            float screenY = (screen.Height / 2) * (1 - y * inv_w);
            
            return new Vector2(screenX, screenY);
        }
        
        public bool IsOnScreen(Vector2 screenPos, float margin = 10.0f)
        {
            ScreenSize screen = GetScreenSize();
            return screenPos.X >= -margin && screenPos.X <= screen.Width + margin &&
                   screenPos.Y >= -margin && screenPos.Y <= screen.Height + margin;
        }
        
        public void Dispose()
        {
            phantom?.Dispose();
        }
    }
}
```

### Example 4: Teleport Hack with Vector3
```csharp
using System;
using System.Collections.Generic;

namespace TeleportHackExample
{
    public class TeleportHack : IDisposable
    {
        private readonly MemoryPhantom phantom;
        private ulong localPlayer;
        
        public class SavedPosition
        {
            public Vector3 Position { get; set; }
            public Vector3 ViewAngles { get; set; }
            public string Name { get; set; }
        }
        
        public List<SavedPosition> SavedPositions { get; } = new List<SavedPosition>();
        
        public TeleportHack()
        {
            phantom = MemoryPhantom.CreateFromName("cs2.exe");
            if (phantom == null || !phantom.IsActive)
                throw new Exception("Failed to attach to process");
        }
        
        public bool Initialize()
        {
            var client = phantom.FindModuleBase("client.dll");
            if (!client.HasValue) return false;
            
            ulong clientBase = client.Value;
            localPlayer = phantom.ReadPtr(clientBase + 0x1BEEF28);
            return localPlayer != 0;
        }
        
        public SavedPosition GetCurrentPosition()
        {
            return new SavedPosition
            {
                Position = phantom.ReadVec3(localPlayer + 0x138),
                ViewAngles = phantom.ReadVec3(localPlayer + 0x140),
                Name = $"Position {SavedPositions.Count + 1}"
            };
        }
        
        public bool SavePosition(string name = "")
        {
            var pos = GetCurrentPosition();
            if (!string.IsNullOrEmpty(name))
            {
                pos.Name = name;
            }
            SavedPositions.Add(pos);
            return true;
        }
        
        public bool TeleportTo(Vector3 position)
        {
            return phantom.WriteVec3(localPlayer + 0x138, position);
        }
        
        public bool TeleportToSaved(int index)
        {
            if (index < 0 || index >= SavedPositions.Count)
                return false;
            
            var pos = SavedPositions[index];
            bool posSuccess = phantom.WriteVec3(localPlayer + 0x138, pos.Position);
            bool angleSuccess = phantom.WriteVec3(localPlayer + 0x140, pos.ViewAngles);
            
            return posSuccess && angleSuccess;
        }
        
        public void ListSavedPositions()
        {
            Console.WriteLine($"\n=== Saved Positions ({SavedPositions.Count}) ===");
            for (int i = 0; i < SavedPositions.Count; i++)
            {
                var pos = SavedPositions[i];
                Console.WriteLine($"{i}. {pos.Name} | Pos: {pos.Position} | Angles: {pos.ViewAngles}");
            }
        }
        
        public void Dispose()
        {
            phantom?.Dispose();
        }
    }
    
    class Program
    {
        static void Main()
        {
            using var teleport = new TeleportHack();
            if (!teleport.Initialize())
            {
                Console.WriteLine("Failed to initialize teleport hack!");
                return;
            }
            
            // Example usage
            teleport.SavePosition("Spawn");
            
            // Move somewhere
            System.Threading.Thread.Sleep(2000);
            
            teleport.SavePosition("First Location");
            teleport.ListSavedPositions();
            
            // Teleport back to spawn
            teleport.TeleportToSaved(0);
        }
    }
}
```

### Example 5: Pattern Scanning for Dynamic Offsets
```csharp
using System;

namespace PatternScanExample
{
    public class OffsetScanner : IDisposable
    {
        private readonly MemoryPhantom phantom;
        
        public OffsetScanner()
        {
            phantom = MemoryPhantom.CreateFromName("game.exe");
        }
        
        public ulong? FindLocalPlayer()
        {
            // Pattern for local player in many games
            var pattern = "48 8B 0D ?? ?? ?? ?? 48 85 C9 74 07";
            var mask = "xxx????xxxxx";
            
            return phantom.ScanPatternModule("client.dll", pattern, mask);
        }
        
        public ulong? FindEntityList()
        {
            // Pattern for entity list
            var pattern = "48 8B 15 ?? ?? ?? ?? 45 33 C9 4C 8B C2";
            var mask = "xxx????xxxxxx";
            
            return phantom.ScanPatternModule("client.dll", pattern, mask);
        }
        
        public void ScanAllOffsets()
        {
            Console.WriteLine("=== Scanning Offsets ===");
            
            var localPlayer = FindLocalPlayer();
            if (localPlayer.HasValue)
            {
                Console.WriteLine($"LocalPlayer: 0x{localPlayer.Value:X}");
                
                // Read pointer with dereferencing
                ulong localPlayerPtr = phantom.ReadPtr(localPlayer.Value + 3);
                localPlayerPtr = localPlayerPtr + 7 + phantom.ReadInt(localPlayer.Value + 3);
                Console.WriteLine($"LocalPlayer Ptr: 0x{localPlayerPtr:X}");
            }
            
            var entityList = FindEntityList();
            if (entityList.HasValue)
            {
                Console.WriteLine($"EntityList: 0x{entityList.Value:X}");
            }
        }
        
        public void Dispose()
        {
            phantom?.Dispose();
        }
    }
}
```

---

## ⚡ Vector Operations

### Basic Vector Usage
```csharp
// Creating vectors
Vector2 vec2d = new Vector2(10.5f, 20.3f);
Vector3 vec3d = new Vector3(1.0f, 2.0f, 3.0f);

// String representation
string str2d = vec2d.ToString();  // "(10.5, 20.3)"
string str3d = vec3d.ToString();  // "(1, 2, 3)"

// Vector operations
Vector3 a = new Vector3(1, 2, 3);
Vector3 b = new Vector3(4, 5, 6);

Vector3 sum = a + b;              // (5, 7, 9)
Vector3 diff = a - b;             // (-3, -3, -3)
float distance = Vector3.Distance(a, b); // ~5.196
float length = a.Length();        // Length of vector a

// Reading/writing vectors from memory
Vector3 playerPos = phantom.ReadVec3(playerAddress + 0x138);
phantom.WriteVec3(playerAddress + 0x138, new Vector3(100, 200, 300));
```

### Matrix 4x4 Operations
```csharp
// Reading view matrix
var viewMatrix = phantom.ReadMatrix(viewMatrixAddress);
if (viewMatrix.HasValue)
{
    // World point transformation
    var matrix = viewMatrix.Value;
    
    // Example usage in World-to-Screen
    Vector3 worldPos = new Vector3(100, 200, 300);
    // ... matrix transformations
}

// Writing matrix
Mat4x4 newMatrix = new Mat4x4();
// ... fill matrix with data
phantom.WriteMatrix(matrixAddress, newMatrix);
```

---

## 🛡️ Error Handling

MemoryPhantom uses a defensive programming approach:

### Safe Defaults
```csharp
// Returns default vector for invalid reads
Vector3 pos = phantom.ReadVec3(0xDEADBEEF);  // Returns Vector3(0, 0, 0)

// Returns null for matrices
var matrix = phantom.ReadMatrix(badAddress);  // Returns null

// Returns false for invalid writes
bool success = phantom.WriteVec3(0xDEADBEEF, new Vector3());  // Returns false
```

### Null Safety
```csharp
// Safe operations with nullables
Vector3 pos = phantom.ReadVec3(null);           // Returns Vector3(0, 0, 0)
Vector3 pos2 = phantom.ReadVec3(null, 0x100);   // Returns Vector3(0, 0, 0)

// Check if vector is valid
Vector3 position = phantom.ReadVec3(address);
if (position.X != 0 || position.Y != 0 || position.Z != 0)
{
    // Valid position
}
```

---

## 🔧 Performance Optimizations

### 1. Using `unsafe` and Pointers
```csharp
// Instead of this:
byte[] buffer = new byte[size];
// ... copying through Marshal

// Use this:
T value = default;
unsafe
{
    phantom.ReadProcessMemory(addr, &value, size);
}
```

### 2. `Span<T>` Instead of Arrays
```csharp
// Instead of this:
byte[] buffer = new byte[1024];

// Use this:
Span<byte> buffer = stackalloc byte[1024];  // Stack allocation
```

### 3. Aggressive Inlining
```csharp
[MethodImpl(MethodImplOptions.AggressiveInlining)]
public int ReadInt(ulong addr)
{
    return InternalRead<int>(addr);
}
```

### 4. Batch Operations
```csharp
// Perform multiple operations at once
phantom.ReadBatch(
    (addr1, buffer1),
    (addr2, buffer2),
    (addr3, buffer3)
);
```

### 5. Caching
```csharp
// For frequently read values
int health = phantom.ReadCached<int>(playerAddress + 0x100);
```

### 6. Minimal Allocation
```csharp
// Using ValueStringBuilder for string operations
var sb = new ValueStringBuilder(stackalloc char[260]);
// ... string operations without heap allocation
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

- **MemoryPhantom.NET**: Original C# implementation
- **Inspiration**: [Swed64](https://github.com/Massivetwat/Swed64) by MassiveTwat
- **Performance Optimizations**: Community contributors

---

**MemoryPhantom.NET** - Complete memory manipulation with built-in vector support. 🚀

*Performance. Safety. Simplicity.*