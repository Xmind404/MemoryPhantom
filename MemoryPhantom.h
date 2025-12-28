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

    static std::optional<MemoryPhantom> CreateFromName(
        const char* processName,
        DWORD accessRights = PROCESS_ALL_ACCESS
    );

    std::optional<uintptr_t> FindModuleBase(const char* moduleName) const;

    template<typename T>
    std::optional<T> ReadData(uintptr_t addr) const;

    template<typename T>
    std::optional<T> ReadData(uintptr_t addr, int off) const;

    template<typename T>
    bool WriteData(uintptr_t addr, const T& value) const;

    template<typename T>
    bool WriteData(uintptr_t addr, int off, const T& value) const;

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
};

template std::optional<int> MemoryPhantom::ReadData<int>(uintptr_t) const;
template std::optional<float> MemoryPhantom::ReadData<float>(uintptr_t) const;
template std::optional<double> MemoryPhantom::ReadData<double>(uintptr_t) const;
template std::optional<short> MemoryPhantom::ReadData<short>(uintptr_t) const;
template std::optional<unsigned short> MemoryPhantom::ReadData<unsigned short>(uintptr_t) const;
template std::optional<unsigned int> MemoryPhantom::ReadData<unsigned int>(uintptr_t) const;
template std::optional<unsigned long long> MemoryPhantom::ReadData<unsigned long long>(uintptr_t) const;
template std::optional<bool> MemoryPhantom::ReadData<bool>(uintptr_t) const;
template std::optional<char> MemoryPhantom::ReadData<char>(uintptr_t) const;
template std::optional<uint8_t> MemoryPhantom::ReadData<uint8_t>(uintptr_t) const;
template std::optional<uint32_t> MemoryPhantom::ReadData<uint32_t>(uintptr_t) const;
template std::optional<int64_t> MemoryPhantom::ReadData<int64_t>(uintptr_t) const;

template bool MemoryPhantom::WriteData<int>(uintptr_t, const int&) const;
template bool MemoryPhantom::WriteData<float>(uintptr_t, const float&) const;
template bool MemoryPhantom::WriteData<double>(uintptr_t, const double&) const;
template bool MemoryPhantom::WriteData<short>(uintptr_t, const short&) const;
template bool MemoryPhantom::WriteData<unsigned short>(uintptr_t, const unsigned short&) const;
template bool MemoryPhantom::WriteData<unsigned int>(uintptr_t, const unsigned int&) const;
template bool MemoryPhantom::WriteData<unsigned long long>(uintptr_t, const unsigned long long&) const;
template bool MemoryPhantom::WriteData<bool>(uintptr_t, const bool&) const;
template bool MemoryPhantom::WriteData<char>(uintptr_t, const char&) const;
template bool MemoryPhantom::WriteData<uint8_t>(uintptr_t, const uint8_t&) const;
template bool MemoryPhantom::WriteData<uint32_t>(uintptr_t, const uint32_t&) const;
template bool MemoryPhantom::WriteData<int64_t>(uintptr_t, const int64_t&) const;

#endif
