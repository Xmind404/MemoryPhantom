#include "MemoryPhantom.h"
#include <psapi.h>

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
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(PROCESSENTRY32W);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return std::nullopt;

    if (Process32FirstW(snapshot, &entry)) {
        do {
            char currentProcessName[MAX_PATH];
            WideCharToMultiByte(CP_UTF8, 0, entry.szExeFile, -1, currentProcessName, MAX_PATH, NULL, NULL);

            if (_stricmp(currentProcessName, processName) == 0) {
                CloseHandle(snapshot);
                MemoryPhantom phantom;
                if (phantom.Attach(entry.th32ProcessID, accessRights)) {
                    return phantom;
                }
                return std::nullopt;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return std::nullopt;
}

std::optional<uintptr_t> MemoryPhantom::FindModuleBase(const char* moduleName) const {
    if (!hProcess) return std::nullopt;

    HMODULE hMods[256];
    DWORD cbNeeded;

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        for (unsigned int i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            char szModName[MAX_PATH];
            if (GetModuleFileNameExA(hProcess, hMods[i], szModName, sizeof(szModName))) {
                const char* baseName = strrchr(szModName, '\\');
                if (baseName) baseName++;
                else baseName = szModName;

                if (_stricmp(baseName, moduleName) == 0) {
                    return reinterpret_cast<uintptr_t>(hMods[i]);
                }
            }
        }
    }

    return std::nullopt;
}

std::vector<uint8_t> MemoryPhantom::InternalReadBytes(uintptr_t addr, size_t sz) const {
    std::vector<uint8_t> buffer(sz);
    if (!hProcess || addr == 0 || sz == 0) return buffer;

    SIZE_T bytesRead;
    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(addr), buffer.data(), sz, &bytesRead) && bytesRead == sz) {
        return buffer;
    }
    return std::vector<uint8_t>();
}

int MemoryPhantom::ReadInt(uintptr_t addr) const {
    int value = 0;
    InternalRead(addr, value);
    return value;
}

int MemoryPhantom::ReadInt(uintptr_t addr, int offset) const {
    return ReadInt(addr + offset);
}

float MemoryPhantom::ReadFloat(uintptr_t addr) const {
    float value = 0;
    InternalRead(addr, value);
    return value;
}

float MemoryPhantom::ReadFloat(uintptr_t addr, int offset) const {
    return ReadFloat(addr + offset);
}

double MemoryPhantom::ReadDouble(uintptr_t addr) const {
    double value = 0;
    InternalRead(addr, value);
    return value;
}

double MemoryPhantom::ReadDouble(uintptr_t addr, int offset) const {
    return ReadDouble(addr + offset);
}

short MemoryPhantom::ReadShort(uintptr_t addr) const {
    short value = 0;
    InternalRead(addr, value);
    return value;
}

short MemoryPhantom::ReadShort(uintptr_t addr, int offset) const {
    return ReadShort(addr + offset);
}

unsigned short MemoryPhantom::ReadUShort(uintptr_t addr) const {
    unsigned short value = 0;
    InternalRead(addr, value);
    return value;
}

unsigned short MemoryPhantom::ReadUShort(uintptr_t addr, int offset) const {
    return ReadUShort(addr + offset);
}

unsigned int MemoryPhantom::ReadUInt(uintptr_t addr) const {
    unsigned int value = 0;
    InternalRead(addr, value);
    return value;
}

unsigned int MemoryPhantom::ReadUInt(uintptr_t addr, int offset) const {
    return ReadUInt(addr + offset);
}

uint64_t MemoryPhantom::ReadULong(uintptr_t addr) const {
    uint64_t value = 0;
    InternalRead(addr, value);
    return value;
}

uint64_t MemoryPhantom::ReadULong(uintptr_t addr, int offset) const {
    return ReadULong(addr + offset);
}

int64_t MemoryPhantom::ReadLong(uintptr_t addr) const {
    int64_t value = 0;
    InternalRead(addr, value);
    return value;
}

int64_t MemoryPhantom::ReadLong(uintptr_t addr, int offset) const {
    return ReadLong(addr + offset);
}

bool MemoryPhantom::ReadBool(uintptr_t addr) const {
    bool value = false;
    InternalRead(addr, value);
    return value;
}

bool MemoryPhantom::ReadBool(uintptr_t addr, int offset) const {
    return ReadBool(addr + offset);
}

char MemoryPhantom::ReadChar(uintptr_t addr) const {
    char value = 0;
    InternalRead(addr, value);
    return value;
}

char MemoryPhantom::ReadChar(uintptr_t addr, int offset) const {
    return ReadChar(addr + offset);
}

uint8_t MemoryPhantom::ReadByte(uintptr_t addr) const {
    uint8_t value = 0;
    InternalRead(addr, value);
    return value;
}

uint8_t MemoryPhantom::ReadByte(uintptr_t addr, int offset) const {
    return ReadByte(addr + offset);
}

std::string MemoryPhantom::ReadString(uintptr_t addr, size_t length) const {
    if (!hProcess || addr == 0 || length == 0) return "";

    std::vector<char> buffer(length + 1);
    SIZE_T bytesRead;
    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(addr), buffer.data(), length, &bytesRead)) {
        buffer[length] = '\0';
        return std::string(buffer.data());
    }
    return "";
}

std::string MemoryPhantom::ReadString(uintptr_t addr, int offset, size_t length) const {
    return ReadString(addr + offset, length);
}

std::wstring MemoryPhantom::ReadWString(uintptr_t addr, size_t length) const {
    if (!hProcess || addr == 0 || length == 0) return L"";

    size_t byteLength = length * sizeof(wchar_t);
    std::vector<wchar_t> buffer(length + 1);
    SIZE_T bytesRead;
    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(addr), buffer.data(), byteLength, &bytesRead)) {
        buffer[length] = L'\0';
        return std::wstring(buffer.data());
    }
    return L"";
}

std::wstring MemoryPhantom::ReadWString(uintptr_t addr, int offset, size_t length) const {
    return ReadWString(addr + offset, length);
}

std::optional<MemoryPhantom::Vec3> MemoryPhantom::ReadVec3(uintptr_t addr) const {
    Vec3 vec;
    if (InternalRead(addr, vec)) return vec;
    return std::nullopt;
}

std::optional<MemoryPhantom::Vec3> MemoryPhantom::ReadVec3(uintptr_t addr, int offset) const {
    return ReadVec3(addr + offset);
}

std::optional<MemoryPhantom::Mat4x4> MemoryPhantom::ReadMatrix(uintptr_t addr) const {
    Mat4x4 matrix;
    if (InternalRead(addr, matrix)) return matrix;
    return std::nullopt;
}

std::optional<MemoryPhantom::Mat4x4> MemoryPhantom::ReadMatrix(uintptr_t addr, int offset) const {
    return ReadMatrix(addr + offset);
}

uintptr_t MemoryPhantom::ReadPtr(uintptr_t addr) const {
    uintptr_t value = 0;
    InternalRead(addr, value);
    return value;
}

uintptr_t MemoryPhantom::ReadPtr(uintptr_t addr, int offset) const {
    return ReadPtr(addr + offset);
}

std::vector<uint8_t> MemoryPhantom::ReadBytes(uintptr_t addr, size_t sz) const {
    return InternalReadBytes(addr, sz);
}

std::vector<uint8_t> MemoryPhantom::ReadBytes(uintptr_t addr, int offset, size_t sz) const {
    return InternalReadBytes(addr + offset, sz);
}

bool MemoryPhantom::WriteInt(uintptr_t addr, int value) const {
    return InternalWrite(addr, value);
}

bool MemoryPhantom::WriteInt(uintptr_t addr, int offset, int value) const {
    return WriteInt(addr + offset, value);
}

bool MemoryPhantom::WriteFloat(uintptr_t addr, float value) const {
    return InternalWrite(addr, value);
}

bool MemoryPhantom::WriteFloat(uintptr_t addr, int offset, float value) const {
    return WriteFloat(addr + offset, value);
}

bool MemoryPhantom::WriteDouble(uintptr_t addr, double value) const {
    return InternalWrite(addr, value);
}

bool MemoryPhantom::WriteDouble(uintptr_t addr, int offset, double value) const {
    return WriteDouble(addr + offset, value);
}

bool MemoryPhantom::WriteShort(uintptr_t addr, short value) const {
    return InternalWrite(addr, value);
}

bool MemoryPhantom::WriteShort(uintptr_t addr, int offset, short value) const {
    return WriteShort(addr + offset, value);
}

bool MemoryPhantom::WriteUShort(uintptr_t addr, unsigned short value) const {
    return InternalWrite(addr, value);
}

bool MemoryPhantom::WriteUShort(uintptr_t addr, int offset, unsigned short value) const {
    return WriteUShort(addr + offset, value);
}

bool MemoryPhantom::WriteUInt(uintptr_t addr, unsigned int value) const {
    return InternalWrite(addr, value);
}

bool MemoryPhantom::WriteUInt(uintptr_t addr, int offset, unsigned int value) const {
    return WriteUInt(addr + offset, value);
}

bool MemoryPhantom::WriteULong(uintptr_t addr, uint64_t value) const {
    return InternalWrite(addr, value);
}

bool MemoryPhantom::WriteULong(uintptr_t addr, int offset, uint64_t value) const {
    return WriteULong(addr + offset, value);
}

bool MemoryPhantom::WriteLong(uintptr_t addr, int64_t value) const {
    return InternalWrite(addr, value);
}

bool MemoryPhantom::WriteLong(uintptr_t addr, int offset, int64_t value) const {
    return WriteLong(addr + offset, value);
}

bool MemoryPhantom::WriteBool(uintptr_t addr, bool value) const {
    return InternalWrite(addr, value);
}

bool MemoryPhantom::WriteBool(uintptr_t addr, int offset, bool value) const {
    return WriteBool(addr + offset, value);
}

bool MemoryPhantom::WriteChar(uintptr_t addr, char value) const {
    return InternalWrite(addr, value);
}

bool MemoryPhantom::WriteChar(uintptr_t addr, int offset, char value) const {
    return WriteChar(addr + offset, value);
}

bool MemoryPhantom::WriteByte(uintptr_t addr, uint8_t value) const {
    return InternalWrite(addr, value);
}

bool MemoryPhantom::WriteByte(uintptr_t addr, int offset, uint8_t value) const {
    return WriteByte(addr + offset, value);
}

bool MemoryPhantom::WriteString(uintptr_t addr, const std::string& value) const {
    if (!hProcess || addr == 0 || value.empty()) return false;

    SIZE_T bytesWritten;
    return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(addr), value.c_str(), value.length(), &bytesWritten) &&
        bytesWritten == value.length();
}

bool MemoryPhantom::WriteString(uintptr_t addr, int offset, const std::string& value) const {
    return WriteString(addr + offset, value);
}

bool MemoryPhantom::WriteWString(uintptr_t addr, const std::wstring& value) const {
    if (!hProcess || addr == 0 || value.empty()) return false;

    SIZE_T bytesWritten;
    size_t byteLength = value.length() * sizeof(wchar_t);
    return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(addr), value.c_str(), byteLength, &bytesWritten) &&
        bytesWritten == byteLength;
}

bool MemoryPhantom::WriteWString(uintptr_t addr, int offset, const std::wstring& value) const {
    return WriteWString(addr + offset, value);
}

bool MemoryPhantom::WriteVec3(uintptr_t addr, const Vec3& vec) const {
    return InternalWrite(addr, vec);
}

bool MemoryPhantom::WriteVec3(uintptr_t addr, int offset, const Vec3& vec) const {
    return WriteVec3(addr + offset, vec);
}

bool MemoryPhantom::WriteMatrix(uintptr_t addr, const Mat4x4& matrix) const {
    return InternalWrite(addr, matrix);
}

bool MemoryPhantom::WriteMatrix(uintptr_t addr, int offset, const Mat4x4& matrix) const {
    return WriteMatrix(addr + offset, matrix);
}

bool MemoryPhantom::WriteBytes(uintptr_t addr, const std::vector<uint8_t>& data) const {
    if (!hProcess || addr == 0 || data.empty()) return false;

    SIZE_T bytesWritten;
    return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(addr), data.data(), data.size(), &bytesWritten) &&
        bytesWritten == data.size();
}

bool MemoryPhantom::WriteBytes(uintptr_t addr, int offset, const std::vector<uint8_t>& data) const {
    return WriteBytes(addr + offset, data);
}
