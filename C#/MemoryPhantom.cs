using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

public sealed class MemoryPhantom : IDisposable
{
    private IntPtr hProcess = IntPtr.Zero;
    private uint processId = 0;
    private Dictionary<string, ulong> moduleCache = new Dictionary<string, ulong>();
    private bool is64Bit = false;

    [StructLayout(LayoutKind.Sequential)]
    public struct Mat4x4
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
        public float[] Data;

        public Mat4x4()
        {
            Data = new float[16];
        }
    }

    public MemoryPhantom() { }

    public MemoryPhantom(uint pid, uint accessRights = 0x1F0FFF)
    {
        Attach(pid, accessRights);
    }

    ~MemoryPhantom()
    {
        Dispose(false);
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    private void Dispose(bool disposing)
    {
        Detach();
        if (disposing)
        {
            moduleCache.Clear();
        }
    }

    public bool Attach(uint pid, uint accessRights = 0x1F0FFF)
    {
        Detach();
        hProcess = Native.OpenProcess(accessRights, false, pid);
        if (hProcess != IntPtr.Zero)
        {
            processId = pid;
            Native.IsWow64Process(hProcess, out bool wow64);
            is64Bit = !wow64;
            moduleCache.Clear();
            return true;
        }
        return false;
    }

    public void Detach()
    {
        if (hProcess != IntPtr.Zero)
        {
            Native.CloseHandle(hProcess);
            hProcess = IntPtr.Zero;
            processId = 0;
            moduleCache.Clear();
        }
    }

    public bool IsActive => hProcess != IntPtr.Zero;
    public uint GetPID() => processId;
    public IntPtr GetHandle() => hProcess;
    public bool Is64Bit => is64Bit;

    public static MemoryPhantom? CreateFromName(string processName, uint accessRights = 0x1F0FFF)
    {
        var process = Process.GetProcessesByName(processName.Replace(".exe", ""))
            .FirstOrDefault();
        if (process == null) return null;

        var phantom = new MemoryPhantom();
        return phantom.Attach((uint)process.Id, accessRights) ? phantom : null;
    }

    public ulong? FindModuleBase(string moduleName)
    {
        if (!IsActive) return null;

        if (moduleCache.TryGetValue(moduleName, out ulong cachedBase))
            return cachedBase;

        IntPtr[] modules = new IntPtr[512];
        uint cbNeeded;
        if (Native.EnumProcessModulesEx(hProcess, modules, (uint)(modules.Length * IntPtr.Size), 
            out cbNeeded, is64Bit ? 0x01 : 0x00))
        {
            uint moduleCount = Math.Min(cbNeeded / (uint)IntPtr.Size, (uint)modules.Length);
            for (int i = 0; i < moduleCount; i++)
            {
                var sb = new ValueStringBuilder(stackalloc char[260]);
                if (Native.GetModuleBaseName(hProcess, modules[i], ref sb, (uint)sb.Capacity) > 0)
                {
                    if (sb.AsSpan().Equals(moduleName, StringComparison.OrdinalIgnoreCase))
                    {
                        ulong baseAddr = (ulong)modules[i];
                        moduleCache[moduleName] = baseAddr;
                        return baseAddr;
                    }
                }
            }
        }
        return null;
    }

    public ulong? ScanPattern(ulong start, ulong end, string pattern, string mask)
    {
        if (!IsActive || start >= end || pattern.Length != mask.Length) 
            return null;

        const uint CHUNK_SIZE = 0x10000;
        byte[] buffer = new byte[CHUNK_SIZE + pattern.Length - 1];
        
        for (ulong addr = start; addr < end; addr += CHUNK_SIZE)
        {
            uint readSize = (uint)Math.Min((ulong)CHUNK_SIZE + (ulong)(pattern.Length - 1), end - addr);
            byte[] chunk = ReadBytes(addr, readSize);
            if (chunk.Length == 0) continue;

            int result = ScanChunk(chunk, pattern, mask, readSize);
            if (result != -1)
                return addr + (ulong)result;
        }
        
        return null;
    }

    public ulong? ScanPatternModule(string moduleName, string pattern, string mask)
    {
        var moduleBase = FindModuleBase(moduleName);
        if (!moduleBase.HasValue) return null;

        var moduleInfo = GetModuleInfo(moduleName);
        if (!moduleInfo.HasValue) return null;

        return ScanPattern(moduleBase.Value, moduleBase.Value + moduleInfo.Value.SizeOfImage, pattern, mask);
    }

    public ulong? AobScan(ulong start, ulong end, byte[] pattern, string mask = null)
    {
        if (mask == null)
        {
            mask = new string('x', pattern.Length);
        }
        
        string hexPattern = BitConverter.ToString(pattern).Replace("-", "");
        return ScanPattern(start, end, hexPattern, mask);
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private int ScanChunk(byte[] buffer, string pattern, string mask, uint bufferSize)
    {
        int patternLen = pattern.Length / 2;
        byte[] patternBytes = new byte[patternLen];
        
        for (int i = 0; i < patternLen; i++)
        {
            patternBytes[i] = Convert.ToByte(pattern.Substring(i * 2, 2), 16);
        }

        for (int i = 0; i <= bufferSize - patternLen; i++)
        {
            bool found = true;
            for (int j = 0; j < patternLen; j++)
            {
                if (mask[j] == 'x' && buffer[i + j] != patternBytes[j])
                {
                    found = false;
                    break;
                }
            }
            if (found) return i;
        }
        return -1;
    }

    private ModuleInfo? GetModuleInfo(string moduleName)
    {
        var moduleBase = FindModuleBase(moduleName);
        if (!moduleBase.HasValue) return null;

        var moduleInfo = new Native.MODULEINFO();
        if (Native.GetModuleInformation(hProcess, (IntPtr)moduleBase.Value, out moduleInfo, (uint)Marshal.SizeOf<Native.MODULEINFO>()))
        {
            return new ModuleInfo
            {
                BaseAddress = (ulong)moduleInfo.lpBaseOfDll,
                SizeOfImage = moduleInfo.SizeOfImage,
                EntryPoint = (ulong)moduleInfo.EntryPoint
            };
        }
        return null;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private unsafe byte[] InternalReadBytes(ulong addr, uint sz)
    {
        if (!IsActive || addr == 0 || sz == 0) 
            return Array.Empty<byte>();

        byte[] buffer = new byte[sz];
        uint bytesRead;
        
        fixed (byte* pBuffer = buffer)
        {
            if (Native.ReadProcessMemory(hProcess, (IntPtr)addr, pBuffer, sz, out bytesRead) && bytesRead == sz)
            {
                return buffer;
            }
        }
        return Array.Empty<byte>();
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private T InternalRead<T>(ulong addr) where T : unmanaged
    {
        if (!IsActive || addr == 0) 
            return default;

        T value = default;
        uint bytesRead;
        
        unsafe
        {
            if (Native.ReadProcessMemory(hProcess, (IntPtr)addr, &value, (uint)sizeof(T), out bytesRead) && 
                bytesRead == sizeof(T))
            {
                return value;
            }
        }
        return default;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private bool InternalWrite<T>(ulong addr, T value) where T : unmanaged
    {
        if (!IsActive || addr == 0) 
            return false;

        uint bytesWritten;
        
        unsafe
        {
            return Native.WriteProcessMemory(hProcess, (IntPtr)addr, &value, (uint)sizeof(T), out bytesWritten) &&
                   bytesWritten == sizeof(T);
        }
    }

    // Metody Read/Write z optymalizacją inline
    [MethodImpl(MethodImplOptions.AggressiveInlining)] public int ReadInt(ulong addr) => InternalRead<int>(addr);
    [MethodImpl(MethodImplOptions.AggressiveInlining)] public float ReadFloat(ulong addr) => InternalRead<float>(addr);
    [MethodImpl(MethodImplOptions.AggressiveInlining)] public ulong ReadULong(ulong addr) => InternalRead<ulong>(addr);
    [MethodImpl(MethodImplOptions.AggressiveInlining)] public bool ReadBool(ulong addr) => InternalRead<byte>(addr) != 0;
    [MethodImpl(MethodImplOptions.AggressiveInlining)] public byte ReadByte(ulong addr) => InternalRead<byte>(addr);
    
    [MethodImpl(MethodImplOptions.AggressiveInlining)] public bool WriteInt(ulong addr, int value) => InternalWrite(addr, value);
    [MethodImpl(MethodImplOptions.AggressiveInlining)] public bool WriteFloat(ulong addr, float value) => InternalWrite(addr, value);
    [MethodImpl(MethodImplOptions.AggressiveInlining)] public bool WriteULong(ulong addr, ulong value) => InternalWrite(addr, value);

    public unsafe string ReadString(ulong addr, uint length)
    {
        if (!IsActive || addr == 0 || length == 0) 
            return string.Empty;

        byte[] buffer = new byte[length];
        uint bytesRead;
        
        fixed (byte* pBuffer = buffer)
        {
            if (!Native.ReadProcessMemory(hProcess, (IntPtr)addr, pBuffer, length, out bytesRead) || bytesRead != length)
                return string.Empty;
        }

        int nullIndex = Array.IndexOf(buffer, (byte)0);
        int strLength = nullIndex == -1 ? buffer.Length : nullIndex;
        
        return Encoding.UTF8.GetString(buffer, 0, strLength);
    }

    public unsafe string ReadWString(ulong addr, uint length)
    {
        if (!IsActive || addr == 0 || length == 0) 
            return string.Empty;

        byte[] buffer = new byte[length * 2];
        uint bytesRead;
        
        fixed (byte* pBuffer = buffer)
        {
            if (!Native.ReadProcessMemory(hProcess, (IntPtr)addr, pBuffer, (uint)buffer.Length, out bytesRead) || 
                bytesRead != buffer.Length)
                return string.Empty;
        }

        int nullIndex = Array.IndexOf(buffer, (byte)0);
        int strLength = nullIndex == -1 ? buffer.Length : nullIndex;
        
        return Encoding.Unicode.GetString(buffer, 0, strLength & ~1); // Zaokrąglenie do parzystej liczby bajtów
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public Vector3 ReadVec3(ulong addr)
    {
        Span<byte> data = stackalloc byte[12];
        ReadSpan(addr, data);
        return new Vector3(
            BitConverter.ToSingle(data),
            BitConverter.ToSingle(data.Slice(4)),
            BitConverter.ToSingle(data.Slice(8))
        );
    }

    public bool ReadSpan(ulong addr, Span<byte> buffer)
    {
        if (!IsActive || addr == 0 || buffer.Length == 0) 
            return false;

        uint bytesRead;
        
        unsafe
        {
            fixed (byte* pBuffer = buffer)
            {
                return Native.ReadProcessMemory(hProcess, (IntPtr)addr, pBuffer, (uint)buffer.Length, out bytesRead) &&
                       bytesRead == buffer.Length;
            }
        }
    }

    public bool WriteSpan(ulong addr, ReadOnlySpan<byte> buffer)
    {
        if (!IsActive || addr == 0 || buffer.Length == 0) 
            return false;

        uint bytesWritten;
        
        unsafe
        {
            fixed (byte* pBuffer = buffer)
            {
                return Native.WriteProcessMemory(hProcess, (IntPtr)addr, pBuffer, (uint)buffer.Length, out bytesWritten) &&
                       bytesWritten == buffer.Length;
            }
        }
    }

    // Batch operations
    public bool ReadBatch(params (ulong address, Span<byte> buffer)[] operations)
    {
        bool allSuccess = true;
        foreach (var op in operations)
        {
            if (!ReadSpan(op.address, op.buffer))
                allSuccess = false;
        }
        return allSuccess;
    }

    public bool WriteBatch(params (ulong address, ReadOnlySpan<byte> data)[] operations)
    {
        bool allSuccess = true;
        foreach (var op in operations)
        {
            if (!WriteSpan(op.address, op.data))
                allSuccess = false;
        }
        return allSuccess;
    }

    // Cache dla często używanych wartości
    private readonly Dictionary<ulong, object> readCache = new Dictionary<ulong, object>();
    private const int CACHE_SIZE_LIMIT = 1000;

    public T ReadCached<T>(ulong addr) where T : unmanaged
    {
        if (readCache.TryGetValue(addr, out object cached))
        {
            if (cached is T value)
                return value;
        }

        T result = Read<T>(addr);
        
        if (readCache.Count >= CACHE_SIZE_LIMIT)
            readCache.Clear();
        
        readCache[addr] = result;
        return result;
    }

    public void ClearCache() => readCache.Clear();

    private static class Native
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr OpenProcess(uint dwDesiredAccess, [MarshalAs(UnmanagedType.Bool)] bool bInheritHandle, uint dwProcessId);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool CloseHandle(IntPtr hObject);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern unsafe bool ReadProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, void* lpBuffer, uint nSize, out uint lpNumberOfBytesRead);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern unsafe bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, void* lpBuffer, uint nSize, out uint lpNumberOfBytesWritten);

        [DllImport("psapi.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool EnumProcessModulesEx(IntPtr hProcess, [Out] IntPtr[] lphModule, uint cb, out uint lpcbNeeded, uint dwFilterFlag);

        [DllImport("psapi.dll", SetLastError = true)]
        public static extern uint GetModuleBaseName(IntPtr hProcess, IntPtr hModule, ref ValueStringBuilder lpBaseName, uint nSize);

        [DllImport("psapi.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool GetModuleInformation(IntPtr hProcess, IntPtr hModule, out MODULEINFO lpmodinfo, uint cb);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool IsWow64Process(IntPtr hProcess, [MarshalAs(UnmanagedType.Bool)] out bool wow64Process);

        [StructLayout(LayoutKind.Sequential)]
        public struct MODULEINFO
        {
            public IntPtr lpBaseOfDll;
            public uint SizeOfImage;
            public IntPtr EntryPoint;
        }
    }

    public struct ModuleInfo
    {
        public ulong BaseAddress;
        public uint SizeOfImage;
        public ulong EntryPoint;
    }
}

// Optymalizowane struktury
[StructLayout(LayoutKind.Sequential)]
public struct Vector3
{
    public float X, Y, Z;

    public Vector3(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public readonly override string ToString() => $"({X}, {Y}, {Z})";

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public static Vector3 operator +(Vector3 a, Vector3 b) => new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public static Vector3 operator -(Vector3 a, Vector3 b) => new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public static float Distance(Vector3 a, Vector3 b)
    {
        float dx = b.X - a.X;
        float dy = b.Y - a.Y;
        float dz = b.Z - a.Z;
        return MathF.Sqrt(dx * dx + dy * dy + dz * dz);
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public readonly float Length() => MathF.Sqrt(X * X + Y * Y + Z * Z);
}

[StructLayout(LayoutKind.Sequential)]
public struct Vector2
{
    public float X, Y;

    public Vector2(float x, float y)
    {
        X = x;
        Y = y;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public readonly override string ToString() => $"({X}, {Y})";

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public static Vector2 operator +(Vector2 a, Vector2 b) => new Vector2(a.X + b.X, a.Y + b.Y);

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public static Vector2 operator -(Vector2 a, Vector2 b) => new Vector2(a.X - b.X, a.Y - b.Y);

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public static float Distance(Vector2 a, Vector2 b)
    {
        float dx = b.X - a.X;
        float dy = b.Y - a.Y;
        return MathF.Sqrt(dx * dx + dy * dy);
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public readonly float Length() => MathF.Sqrt(X * X + Y * Y);
}

// Helper dla string builder bez alokacji
internal ref struct ValueStringBuilder
{
    private Span<char> _chars;
    private int _pos;

    public ValueStringBuilder(Span<char> buffer)
    {
        _chars = buffer;
        _pos = 0;
    }

    public int Capacity => _chars.Length;
    public Span<char> RawChars => _chars;
    public Span<char> AsSpan() => _chars.Slice(0, _pos);

    public void Append(char c)
    {
        if (_pos < _chars.Length)
        {
            _chars[_pos++] = c;
        }
    }

    public override string ToString() => AsSpan().ToString();
}