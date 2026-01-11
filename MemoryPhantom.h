#ifndef MEMORYPHANTOM_H
#define MEMORYPHANTOM_H

#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <optional>
#include <cstdint>
#include <type_traits>
#include "Vectors.h"

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
    int ReadInt(const std::optional<uintptr_t>& addr) const;
    int ReadInt(const std::optional<uintptr_t>& addr, int offset) const;

    float ReadFloat(uintptr_t addr) const;
    float ReadFloat(uintptr_t addr, int offset) const;
    float ReadFloat(const std::optional<uintptr_t>& addr) const;
    float ReadFloat(const std::optional<uintptr_t>& addr, int offset) const;

    double ReadDouble(uintptr_t addr) const;
    double ReadDouble(uintptr_t addr, int offset) const;
    double ReadDouble(const std::optional<uintptr_t>& addr) const;
    double ReadDouble(const std::optional<uintptr_t>& addr, int offset) const;

    short ReadShort(uintptr_t addr) const;
    short ReadShort(uintptr_t addr, int offset) const;
    short ReadShort(const std::optional<uintptr_t>& addr) const;
    short ReadShort(const std::optional<uintptr_t>& addr, int offset) const;

    unsigned short ReadUShort(uintptr_t addr) const;
    unsigned short ReadUShort(uintptr_t addr, int offset) const;
    unsigned short ReadUShort(const std::optional<uintptr_t>& addr) const;
    unsigned short ReadUShort(const std::optional<uintptr_t>& addr, int offset) const;

    unsigned int ReadUInt(uintptr_t addr) const;
    unsigned int ReadUInt(uintptr_t addr, int offset) const;
    unsigned int ReadUInt(const std::optional<uintptr_t>& addr) const;
    unsigned int ReadUInt(const std::optional<uintptr_t>& addr, int offset) const;

    uint64_t ReadULong(uintptr_t addr) const;
    uint64_t ReadULong(uintptr_t addr, int offset) const;
    uint64_t ReadULong(const std::optional<uintptr_t>& addr) const;
    uint64_t ReadULong(const std::optional<uintptr_t>& addr, int offset) const;

    int64_t ReadLong(uintptr_t addr) const;
    int64_t ReadLong(uintptr_t addr, int offset) const;
    int64_t ReadLong(const std::optional<uintptr_t>& addr) const;
    int64_t ReadLong(const std::optional<uintptr_t>& addr, int offset) const;

    bool ReadBool(uintptr_t addr) const;
    bool ReadBool(uintptr_t addr, int offset) const;
    bool ReadBool(const std::optional<uintptr_t>& addr) const;
    bool ReadBool(const std::optional<uintptr_t>& addr, int offset) const;

    char ReadChar(uintptr_t addr) const;
    char ReadChar(uintptr_t addr, int offset) const;
    char ReadChar(const std::optional<uintptr_t>& addr) const;
    char ReadChar(const std::optional<uintptr_t>& addr, int offset) const;

    uint8_t ReadByte(uintptr_t addr) const;
    uint8_t ReadByte(uintptr_t addr, int offset) const;
    uint8_t ReadByte(const std::optional<uintptr_t>& addr) const;
    uint8_t ReadByte(const std::optional<uintptr_t>& addr, int offset) const;

    std::string ReadString(uintptr_t addr, size_t length) const;
    std::string ReadString(uintptr_t addr, int offset, size_t length) const;
    std::string ReadString(const std::optional<uintptr_t>& addr, size_t length) const;
    std::string ReadString(const std::optional<uintptr_t>& addr, int offset, size_t length) const;

    std::wstring ReadWString(uintptr_t addr, size_t length) const;
    std::wstring ReadWString(uintptr_t addr, int offset, size_t length) const;
    std::wstring ReadWString(const std::optional<uintptr_t>& addr, size_t length) const;
    std::wstring ReadWString(const std::optional<uintptr_t>& addr, int offset, size_t length) const;

    Vector3 ReadVec3(uintptr_t addr) const;
    Vector3 ReadVec3(uintptr_t addr, int offset) const;
    Vector3 ReadVec3(const std::optional<uintptr_t>& addr) const;
    Vector3 ReadVec3(const std::optional<uintptr_t>& addr, int offset) const;

    std::optional<Mat4x4> ReadMatrix(uintptr_t addr) const;
    std::optional<Mat4x4> ReadMatrix(uintptr_t addr, int offset) const;
    std::optional<Mat4x4> ReadMatrix(const std::optional<uintptr_t>& addr) const;
    std::optional<Mat4x4> ReadMatrix(const std::optional<uintptr_t>& addr, int offset) const;

    uintptr_t ReadPtr(uintptr_t addr) const;
    uintptr_t ReadPtr(uintptr_t addr, int offset) const;
    uintptr_t ReadPtr(const std::optional<uintptr_t>& addr) const;
    uintptr_t ReadPtr(const std::optional<uintptr_t>& addr, int offset) const;

    std::vector<uint8_t> ReadBytes(uintptr_t addr, size_t sz) const;
    std::vector<uint8_t> ReadBytes(uintptr_t addr, int offset, size_t sz) const;
    std::vector<uint8_t> ReadBytes(const std::optional<uintptr_t>& addr, size_t sz) const;
    std::vector<uint8_t> ReadBytes(const std::optional<uintptr_t>& addr, int offset, size_t sz) const;

    bool WriteInt(uintptr_t addr, int value) const;
    bool WriteInt(uintptr_t addr, int offset, int value) const;
    bool WriteInt(const std::optional<uintptr_t>& addr, int value) const;
    bool WriteInt(const std::optional<uintptr_t>& addr, int offset, int value) const;

    bool WriteFloat(uintptr_t addr, float value) const;
    bool WriteFloat(uintptr_t addr, int offset, float value) const;
    bool WriteFloat(const std::optional<uintptr_t>& addr, float value) const;
    bool WriteFloat(const std::optional<uintptr_t>& addr, int offset, float value) const;

    bool WriteDouble(uintptr_t addr, double value) const;
    bool WriteDouble(uintptr_t addr, int offset, double value) const;
    bool WriteDouble(const std::optional<uintptr_t>& addr, double value) const;
    bool WriteDouble(const std::optional<uintptr_t>& addr, int offset, double value) const;

    bool WriteShort(uintptr_t addr, short value) const;
    bool WriteShort(uintptr_t addr, int offset, short value) const;
    bool WriteShort(const std::optional<uintptr_t>& addr, short value) const;
    bool WriteShort(const std::optional<uintptr_t>& addr, int offset, short value) const;

    bool WriteUShort(uintptr_t addr, unsigned short value) const;
    bool WriteUShort(uintptr_t addr, int offset, unsigned short value) const;
    bool WriteUShort(const std::optional<uintptr_t>& addr, unsigned short value) const;
    bool WriteUShort(const std::optional<uintptr_t>& addr, int offset, unsigned short value) const;

    bool WriteUInt(uintptr_t addr, unsigned int value) const;
    bool WriteUInt(uintptr_t addr, int offset, unsigned int value) const;
    bool WriteUInt(const std::optional<uintptr_t>& addr, unsigned int value) const;
    bool WriteUInt(const std::optional<uintptr_t>& addr, int offset, unsigned int value) const;

    bool WriteULong(uintptr_t addr, uint64_t value) const;
    bool WriteULong(uintptr_t addr, int offset, uint64_t value) const;
    bool WriteULong(const std::optional<uintptr_t>& addr, uint64_t value) const;
    bool WriteULong(const std::optional<uintptr_t>& addr, int offset, uint64_t value) const;

    bool WriteLong(uintptr_t addr, int64_t value) const;
    bool WriteLong(uintptr_t addr, int offset, int64_t value) const;
    bool WriteLong(const std::optional<uintptr_t>& addr, int64_t value) const;
    bool WriteLong(const std::optional<uintptr_t>& addr, int offset, int64_t value) const;

    bool WriteBool(uintptr_t addr, bool value) const;
    bool WriteBool(uintptr_t addr, int offset, bool value) const;
    bool WriteBool(const std::optional<uintptr_t>& addr, bool value) const;
    bool WriteBool(const std::optional<uintptr_t>& addr, int offset, bool value) const;

    bool WriteChar(uintptr_t addr, char value) const;
    bool WriteChar(uintptr_t addr, int offset, char value) const;
    bool WriteChar(const std::optional<uintptr_t>& addr, char value) const;
    bool WriteChar(const std::optional<uintptr_t>& addr, int offset, char value) const;

    bool WriteByte(uintptr_t addr, uint8_t value) const;
    bool WriteByte(uintptr_t addr, int offset, uint8_t value) const;
    bool WriteByte(const std::optional<uintptr_t>& addr, uint8_t value) const;
    bool WriteByte(const std::optional<uintptr_t>& addr, int offset, uint8_t value) const;

    bool WriteString(uintptr_t addr, const std::string& value) const;
    bool WriteString(uintptr_t addr, int offset, const std::string& value) const;
    bool WriteString(const std::optional<uintptr_t>& addr, const std::string& value) const;
    bool WriteString(const std::optional<uintptr_t>& addr, int offset, const std::string& value) const;

    bool WriteWString(uintptr_t addr, const std::wstring& value) const;
    bool WriteWString(uintptr_t addr, int offset, const std::wstring& value) const;
    bool WriteWString(const std::optional<uintptr_t>& addr, const std::wstring& value) const;
    bool WriteWString(const std::optional<uintptr_t>& addr, int offset, const std::wstring& value) const;

    bool WriteVec3(uintptr_t addr, const Vector3& vec) const;
    bool WriteVec3(uintptr_t addr, int offset, const Vector3& vec) const;
    bool WriteVec3(const std::optional<uintptr_t>& addr, const Vector3& vec) const;
    bool WriteVec3(const std::optional<uintptr_t>& addr, int offset, const Vector3& vec) const;

    bool WriteMatrix(uintptr_t addr, const Mat4x4& matrix) const;
    bool WriteMatrix(uintptr_t addr, int offset, const Mat4x4& matrix) const;
    bool WriteMatrix(const std::optional<uintptr_t>& addr, const Mat4x4& matrix) const;
    bool WriteMatrix(const std::optional<uintptr_t>& addr, int offset, const Mat4x4& matrix) const;

    bool WriteBytes(uintptr_t addr, const std::vector<uint8_t>& data) const;
    bool WriteBytes(uintptr_t addr, int offset, const std::vector<uint8_t>& data) const;
    bool WriteBytes(const std::optional<uintptr_t>& addr, const std::vector<uint8_t>& data) const;
    bool WriteBytes(const std::optional<uintptr_t>& addr, int offset, const std::vector<uint8_t>& data) const;

    template<typename T>
    T Read(uintptr_t addr) const {
        if constexpr (std::is_same_v<T, int>) return ReadInt(addr);
        else if constexpr (std::is_same_v<T, float>) return ReadFloat(addr);
        else if constexpr (std::is_same_v<T, double>) return ReadDouble(addr);
        else if constexpr (std::is_same_v<T, short>) return ReadShort(addr);
        else if constexpr (std::is_same_v<T, unsigned short>) return ReadUShort(addr);
        else if constexpr (std::is_same_v<T, unsigned int>) return ReadUInt(addr);
        else if constexpr (std::is_same_v<T, uint64_t>) return ReadULong(addr);
        else if constexpr (std::is_same_v<T, int64_t>) return ReadLong(addr);
        else if constexpr (std::is_same_v<T, bool>) return ReadBool(addr);
        else if constexpr (std::is_same_v<T, char>) return ReadChar(addr);
        else if constexpr (std::is_same_v<T, uint8_t>) return ReadByte(addr);
        else if constexpr (std::is_same_v<T, uintptr_t>) return ReadPtr(addr);
        else {
            T value;
            InternalRead(addr, value);
            return value;
        }
    }

    template<typename T>
    T Read(uintptr_t addr, int offset) const {
        return Read<T>(addr + offset);
    }

    template<typename T>
    T Read(const std::optional<uintptr_t>& addr) const {
        return addr ? Read<T>(*addr) : T();
    }

    template<typename T>
    T Read(const std::optional<uintptr_t>& addr, int offset) const {
        return addr ? Read<T>(*addr + offset) : T();
    }

    template<typename T>
    bool Write(uintptr_t addr, const T& value) const {
        if constexpr (std::is_same_v<T, int>) return WriteInt(addr, value);
        else if constexpr (std::is_same_v<T, float>) return WriteFloat(addr, value);
        else if constexpr (std::is_same_v<T, double>) return WriteDouble(addr, value);
        else if constexpr (std::is_same_v<T, short>) return WriteShort(addr, value);
        else if constexpr (std::is_same_v<T, unsigned short>) return WriteUShort(addr, value);
        else if constexpr (std::is_same_v<T, unsigned int>) return WriteUInt(addr, value);
        else if constexpr (std::is_same_v<T, uint64_t>) return WriteULong(addr, value);
        else if constexpr (std::is_same_v<T, int64_t>) return WriteLong(addr, value);
        else if constexpr (std::is_same_v<T, bool>) return WriteBool(addr, value);
        else if constexpr (std::is_same_v<T, char>) return WriteChar(addr, value);
        else if constexpr (std::is_same_v<T, uint8_t>) return WriteByte(addr, value);
        else return InternalWrite(addr, value);
    }

    template<typename T>
    bool Write(uintptr_t addr, int offset, const T& value) const {
        return Write<T>(addr + offset, value);
    }

    template<typename T>
    bool Write(const std::optional<uintptr_t>& addr, const T& value) const {
        return addr ? Write<T>(*addr, value) : false;
    }

    template<typename T>
    bool Write(const std::optional<uintptr_t>& addr, int offset, const T& value) const {
        return addr ? Write<T>(*addr + offset, value) : false;
    }
};

#endif