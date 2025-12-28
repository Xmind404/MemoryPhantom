#ifndef MEMORYPHANTOM_H
#define MEMORYPHANTOM_H

#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <optional>
#include <cstdint>

class MemoryPhantom {
private:
    HANDLE hProcess;
    DWORD processId;

    template<typename T>
    bool InternalRead(uintptr_t addr, T& value) const {
        if (!hProcess || addr == 0) return false;
        SIZE_T bytesRead;
        return ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(addr), &value, sizeof(T), &bytesRead) &&
            bytesRead == sizeof(T);
    }

    template<typename T>
    bool InternalWrite(uintptr_t addr, const T& value) const {
        if (!hProcess || addr == 0) return false;
        SIZE_T bytesWritten;
        return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(addr), &value, sizeof(T), &bytesWritten) &&
            bytesWritten == sizeof(T);
    }

    std::vector<uint8_t> InternalReadBytes(uintptr_t addr, size_t sz) const;

public:
    struct Vec3 {
        float x, y, z;
        Vec3() : x(0), y(0), z(0) {}
        Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    };

    struct Mat4x4 {
        float data[16];
        Mat4x4() { memset(data, 0, sizeof(data)); }
    };

    MemoryPhantom();
    MemoryPhantom(DWORD pid, DWORD accessRights = PROCESS_ALL_ACCESS);
    ~MemoryPhantom();

    MemoryPhantom(const MemoryPhantom&) = delete;
    MemoryPhantom& operator=(const MemoryPhantom&) = delete;

    MemoryPhantom(MemoryPhantom&& other) noexcept;
    MemoryPhantom& operator=(MemoryPhantom&& other) noexcept;

    bool Attach(DWORD pid, DWORD accessRights = PROCESS_ALL_ACCESS);
    void Detach();
    bool IsActive() const;
    DWORD GetPID() const;
    HANDLE GetHandle() const;

    static std::optional<MemoryPhantom> CreateFromName(const char* processName, DWORD accessRights = PROCESS_ALL_ACCESS);

    std::optional<uintptr_t> FindModuleBase(const char* moduleName) const;

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

    std::string ReadString(uintptr_t addr, size_t length) const;
    std::string ReadString(uintptr_t addr, int offset, size_t length) const;

    std::wstring ReadWString(uintptr_t addr, size_t length) const;
    std::wstring ReadWString(uintptr_t addr, int offset, size_t length) const;

    std::optional<Vec3> ReadVec3(uintptr_t addr) const;
    std::optional<Vec3> ReadVec3(uintptr_t addr, int offset) const;

    std::optional<Mat4x4> ReadMatrix(uintptr_t addr) const;
    std::optional<Mat4x4> ReadMatrix(uintptr_t addr, int offset) const;

    uintptr_t ReadPtr(uintptr_t addr) const;
    uintptr_t ReadPtr(uintptr_t addr, int offset) const;

    std::vector<uint8_t> ReadBytes(uintptr_t addr, size_t sz) const;
    std::vector<uint8_t> ReadBytes(uintptr_t addr, int offset, size_t sz) const;

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

    bool WriteString(uintptr_t addr, const std::string& value) const;
    bool WriteString(uintptr_t addr, int offset, const std::string& value) const;

    bool WriteWString(uintptr_t addr, const std::wstring& value) const;
    bool WriteWString(uintptr_t addr, int offset, const std::wstring& value) const;

    bool WriteVec3(uintptr_t addr, const Vec3& vec) const;
    bool WriteVec3(uintptr_t addr, int offset, const Vec3& vec) const;

    bool WriteMatrix(uintptr_t addr, const Mat4x4& matrix) const;
    bool WriteMatrix(uintptr_t addr, int offset, const Mat4x4& matrix) const;

    bool WriteBytes(uintptr_t addr, const std::vector<uint8_t>& data) const;
    bool WriteBytes(uintptr_t addr, int offset, const std::vector<uint8_t>& data) const;
};

#endif
