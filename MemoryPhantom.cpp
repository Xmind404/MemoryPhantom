#include "MemoryPhantom.h"
#include <psapi.h>
#include <algorithm>

MemoryPhantom::MemoryPhantom() : hProcess(nullptr), processId(0) {}

MemoryPhantom::MemoryPhantom(DWORD pid, DWORD accessRights) : hProcess(nullptr), processId(0) {
    Attach(pid, accessRights);
}

MemoryPhantom::~MemoryPhantom() {
    Detach();
}

MemoryPhantom::MemoryPhantom(MemoryPhantom&& other) noexcept
    : hProcess(other.hProcess), processId(other.processId) {
    other.hProcess = nullptr;
    other.processId = 0;
}

MemoryPhantom& MemoryPhantom::operator=(MemoryPhantom&& other) noexcept {
    if (this != &other) {
        Detach();
        hProcess = other.hProcess;
        processId = other.processId;
        other.hProcess = nullptr;
        other.processId = 0;
    }
    return *this;
}

bool MemoryPhantom::Attach(DWORD pid, DWORD accessRights) {
    Detach();
    hProcess = OpenProcess(accessRights, FALSE, pid);
    if (hProcess) {
        processId = pid;
        return true;
    }
    return false;
}

void MemoryPhantom::Detach() {
    if (hProcess) {
        CloseHandle(hProcess);
        hProcess = nullptr;
        processId = 0;
    }
}

bool MemoryPhantom::IsActive() const {
    return hProcess != nullptr;
}

DWORD MemoryPhantom::GetPID() const {
    return processId;
}

HANDLE MemoryPhantom::GetHandle() const {
    return hProcess;
}

std::optional<MemoryPhantom> MemoryPhantom::CreateFromName(const char* processName, DWORD accessRights) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return std::nullopt;
    }

    if (Process32First(snapshot, &entry)) {
        while (Process32Next(snapshot, &entry)) {
            std::string currentProcessName = entry.szExeFile;
            if (_stricmp(currentProcessName.c_str(), processName) == 0) {
                CloseHandle(snapshot);
                MemoryPhantom phantom;
                if (phantom.Attach(entry.th32ProcessID, accessRights)) {
                    return phantom;
                }
                return std::nullopt;
            }
        }
    }

    CloseHandle(snapshot);
    return std::nullopt;
}

std::optional<uintptr_t> MemoryPhantom::FindModuleBase(const char* moduleName) const {
    if (!hProcess) return std::nullopt;

    HMODULE hMods[1024];
    DWORD cbNeeded;

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            char szModName[MAX_PATH];
            if (GetModuleFileNameExA(hProcess, hMods[i], szModName, sizeof(szModName))) {
                std::string fullPath = szModName;
                size_t pos = fullPath.find_last_of("\\/");
                std::string currentModuleName = (pos != std::string::npos) ?
                    fullPath.substr(pos + 1) : fullPath;

                if (_stricmp(currentModuleName.c_str(), moduleName) == 0) {
                    return reinterpret_cast<uintptr_t>(hMods[i]);
                }
            }
        }
    }

    return std::nullopt;
}

template<typename T>
std::optional<T> MemoryPhantom::ReadData(uintptr_t addr) const {
    if (!hProcess) return std::nullopt;

    T value;
    SIZE_T bytesRead;
    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(addr), &value, sizeof(T), &bytesRead) &&
        bytesRead == sizeof(T)) {
        return value;
    }
    return std::nullopt;
}

template<typename T>
std::optional<T> MemoryPhantom::ReadData(uintptr_t addr, int off) const {
    return ReadData<T>(addr + off);
}

template<typename T>
bool MemoryPhantom::WriteData(uintptr_t addr, const T& value) const {
    if (!hProcess) return false;

    SIZE_T bytesWritten;
    return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(addr), &value, sizeof(T), &bytesWritten) &&
        bytesWritten == sizeof(T);
}

template<typename T>
bool MemoryPhantom::WriteData(uintptr_t addr, int off, const T& value) const {
    return WriteData<T>(addr + off, value);
}

std::optional<uintptr_t> MemoryPhantom::ReadPtr(uintptr_t addr) const {
    return ReadData<uintptr_t>(addr);
}

std::optional<uintptr_t> MemoryPhantom::ReadPtr(uintptr_t addr, int off) const {
    return ReadPtr(addr + off);
}

std::optional<std::vector<uint8_t>> MemoryPhantom::ReadBlock(uintptr_t addr, size_t sz) const {
    if (!hProcess) return std::nullopt;

    std::vector<uint8_t> buffer(sz);
    SIZE_T bytesRead;
    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(addr), buffer.data(), sz, &bytesRead) &&
        bytesRead == sz) {
        return buffer;
    }
    return std::nullopt;
}

bool MemoryPhantom::WriteBlock(uintptr_t addr, const std::vector<uint8_t>& data) const {
    if (!hProcess || data.empty()) return false;

    SIZE_T bytesWritten;
    return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(addr), data.data(), data.size(), &bytesWritten) &&
        bytesWritten == data.size();
}

std::optional<std::string> MemoryPhantom::ReadText(uintptr_t addr, size_t maxLen, bool nullEnded) const {
    if (!hProcess || maxLen == 0) return std::nullopt;

    std::vector<char> buffer(maxLen + 1);
    SIZE_T bytesRead;
    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(addr), buffer.data(), maxLen, &bytesRead)) {
        buffer[maxLen] = '\0';
        if (nullEnded) {
            return std::string(buffer.data());
        }
        else {
            return std::string(buffer.data(), maxLen);
        }
    }
    return std::nullopt;
}

std::optional<std::wstring> MemoryPhantom::ReadWideText(uintptr_t addr, size_t maxLen, bool nullEnded) const {
    if (!hProcess || maxLen == 0) return std::nullopt;

    std::vector<wchar_t> buffer(maxLen + 1);
    SIZE_T bytesRead;
    size_t wchars = maxLen / sizeof(wchar_t);
    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(addr), buffer.data(), maxLen, &bytesRead)) {
        buffer[wchars] = L'\0';
        if (nullEnded) {
            return std::wstring(buffer.data());
        }
        else {
            return std::wstring(buffer.data(), wchars);
        }
    }
    return std::nullopt;
}

bool MemoryPhantom::WriteText(uintptr_t addr, const std::string& text, bool addNull) const {
    if (!hProcess) return false;

    size_t len = text.length() + (addNull ? 1 : 0);
    SIZE_T bytesWritten;
    if (addNull) {
        return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(addr), text.c_str(), len, &bytesWritten) &&
            bytesWritten == len;
    }
    else {
        return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(addr), text.data(), len, &bytesWritten) &&
            bytesWritten == len;
    }
}

bool MemoryPhantom::WriteWideText(uintptr_t addr, const std::wstring& text, bool addNull) const {
    if (!hProcess) return false;

    size_t len = (text.length() + (addNull ? 1 : 0)) * sizeof(wchar_t);
    SIZE_T bytesWritten;
    if (addNull) {
        return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(addr), text.c_str(), len, &bytesWritten) &&
            bytesWritten == len;
    }
    else {
        return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(addr), text.data(), len, &bytesWritten) &&
            bytesWritten == len;
    }
}

std::optional<MemoryPhantom::Vec3> MemoryPhantom::ReadVec3(uintptr_t addr) const {
    static_assert(sizeof(Vec3) == 12, "Vec3 should be 12 bytes");
    return ReadData<Vec3>(addr);
}

bool MemoryPhantom::WriteVec3(uintptr_t addr, const Vec3& vec) const {
    static_assert(sizeof(Vec3) == 12, "Vec3 should be 12 bytes");
    return WriteData<Vec3>(addr, vec);
}

std::optional<MemoryPhantom::Mat4x4> MemoryPhantom::ReadMatrix(uintptr_t addr) const {
    static_assert(sizeof(Mat4x4) == 64, "Mat4x4 should be 64 bytes");
    return ReadData<Mat4x4>(addr);
}

bool MemoryPhantom::WriteMatrix(uintptr_t addr, const Mat4x4& matrix) const {
    static_assert(sizeof(Mat4x4) == 64, "Mat4x4 should be 64 bytes");
    return WriteData<Mat4x4>(addr, matrix);
}

template std::optional<int> MemoryPhantom::ReadData<int>(uintptr_t) const;
template std::optional<float> MemoryPhantom::ReadData<float>(uintptr_t) const;
template std::optional<double> MemoryPhantom::ReadData<double>(uintptr_t) const;
template std::optional<short> MemoryPhantom::ReadData<short>(uintptr_t) const;
template std::optional<unsigned short> MemoryPhantom::ReadData<unsigned short>(uintptr_t) const;
template std::optional<unsigned int> MemoryPhantom::ReadData<unsigned int>(uintptr_t) const;
template std::optional<unsigned long long> MemoryPhantom::ReadData<unsigned long long>(uintptr_t) const;
template std::optional<bool> MemoryPhantom::ReadData<bool>(uintptr_t) const;
template std::optional<char> MemoryPhantom::ReadData<char>(uintptr_t) const;

template bool MemoryPhantom::WriteData<int>(uintptr_t, const int&) const;
template bool MemoryPhantom::WriteData<float>(uintptr_t, const float&) const;
template bool MemoryPhantom::WriteData<double>(uintptr_t, const double&) const;
template bool MemoryPhantom::WriteData<short>(uintptr_t, const short&) const;
template bool MemoryPhantom::WriteData<unsigned short>(uintptr_t, const unsigned short&) const;
template bool MemoryPhantom::WriteData<unsigned int>(uintptr_t, const unsigned int&) const;
template bool MemoryPhantom::WriteData<unsigned long long>(uintptr_t, const unsigned long long&) const;
template bool MemoryPhantom::WriteData<bool>(uintptr_t, const bool&) const;
template bool MemoryPhantom::WriteData<char>(uintptr_t, const char&) const;
