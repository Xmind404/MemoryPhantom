#ifndef MEMORYPHANTOM_H
#define MEMORYPHANTOM_H

#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <cstring>

struct Vec3
{
    float x, y, z;
};

struct Mat4x4
{
    float data[16];
};

class MemoryPhantom
{
private:
    HANDLE processHandle = nullptr;
    DWORD procID = 0;

    void VerifyHandle() const;
    bool MemoryRead(LPCVOID addr, LPVOID buf, SIZE_T sz) const;
    bool MemoryWrite(LPVOID addr, LPCVOID buf, SIZE_T sz) const;

public:
    MemoryPhantom() = default;
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

    static std::optional<MemoryPhantom> CreateFromName(const char* targetName, DWORD accessRights = PROCESS_ALL_ACCESS);
    std::optional<uintptr_t> FindModuleBase(const char* moduleName) const;

    std::optional<uintptr_t> ReadPtr(uintptr_t addr) const;
    std::optional<uintptr_t> ReadPtr(uintptr_t addr, int off) const;
    std::optional<std::vector<uint8_t>> ReadBlock(uintptr_t addr, size_t sz) const;
    bool WriteBlock(uintptr_t addr, const std::vector<uint8_t>& data) const;
    std::optional<std::string> ReadText(uintptr_t addr, size_t maxLen, bool nullEnded = true) const;
    std::optional<std::wstring> ReadWideText(uintptr_t addr, size_t maxLen, bool nullEnded = true) const;
    bool WriteText(uintptr_t addr, const std::string& text, bool addNull = true) const;
    bool WriteWideText(uintptr_t addr, const std::wstring& text, bool addNull = true) const;
    std::optional<Vec3> ReadVec3(uintptr_t addr) const;
    bool WriteVec3(uintptr_t addr, const Vec3& vec) const;
    std::optional<Mat4x4> ReadMatrix(uintptr_t addr) const;
    bool WriteMatrix(uintptr_t addr, const Mat4x4& matrix) const;

    template<typename T>
    std::optional<T> ReadData(uintptr_t addr) const
    {
        T value;
        if (MemoryRead(reinterpret_cast<LPCVOID>(addr), &value, sizeof(T)))
            return value;
        return std::nullopt;
    }

    template<typename T>
    std::optional<T> ReadData(uintptr_t addr, int off) const
    {
        return ReadData<T>(addr + off);
    }

    template<typename T>
    bool WriteData(uintptr_t addr, const T& value) const
    {
        return MemoryWrite(reinterpret_cast<LPVOID>(addr), &value, sizeof(T));
    }

    template<typename T>
    bool WriteData(uintptr_t addr, int off, const T& value) const
    {
        return WriteData<T>(addr + off, value);
    }
};

static_assert(sizeof(Vec3) == 12, "Vec3 size incorrect");
static_assert(sizeof(Mat4x4) == 64, "Mat4x4 size incorrect");

#endif
