#include "MemoryPhantom.h"

void MemoryPhantom::VerifyHandle() const
{
    if (processHandle == nullptr || processHandle == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Process handle is invalid");
    }
}

bool MemoryPhantom::MemoryRead(LPCVOID addr, LPVOID buf, SIZE_T sz) const
{
    VerifyHandle();
    SIZE_T read;
    return ReadProcessMemory(processHandle, addr, buf, sz, &read) && read == sz;
}

bool MemoryPhantom::MemoryWrite(LPVOID addr, LPCVOID buf, SIZE_T sz) const
{
    VerifyHandle();
    SIZE_T written;
    return WriteProcessMemory(processHandle, addr, buf, sz, &written) && written == sz;
}

MemoryPhantom::MemoryPhantom(DWORD pid, DWORD accessRights)
{
    Attach(pid, accessRights);
}

MemoryPhantom::~MemoryPhantom()
{
    Detach();
}

MemoryPhantom::MemoryPhantom(MemoryPhantom&& other) noexcept
    : processHandle(other.processHandle), procID(other.procID)
{
    other.processHandle = nullptr;
    other.procID = 0;
}

MemoryPhantom& MemoryPhantom::operator=(MemoryPhantom&& other) noexcept
{
    if (this != &other)
    {
        Detach();
        processHandle = other.processHandle;
        procID = other.procID;
        other.processHandle = nullptr;
        other.procID = 0;
    }
    return *this;
}

bool MemoryPhantom::Attach(DWORD pid, DWORD accessRights)
{
    Detach();
    processHandle = ::OpenProcess(accessRights, FALSE, pid);
    if (processHandle != nullptr)
    {
        procID = pid;
        return true;
    }
    return false;
}

void MemoryPhantom::Detach()
{
    if (processHandle != nullptr)
    {
        ::CloseHandle(processHandle);
        processHandle = nullptr;
    }
    procID = 0;
}

bool MemoryPhantom::IsActive() const
{
    return processHandle != nullptr && processHandle != INVALID_HANDLE_VALUE;
}

DWORD MemoryPhantom::GetPID() const
{
    return procID;
}

HANDLE MemoryPhantom::GetHandle() const
{
    return processHandle;
}

std::optional<MemoryPhantom> MemoryPhantom::CreateFromName(const char* targetName, DWORD accessRights)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return std::nullopt;

    std::optional<MemoryPhantom> result = std::nullopt;

    if (Process32First(snapshot, &entry))
    {
        do
        {
            if (strcmp(entry.szExeFile, targetName) == 0)
            {
                MemoryPhantom mp;
                if (mp.Attach(entry.th32ProcessID, accessRights))
                {
                    result = std::move(mp);
                }
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return result;
}

std::optional<uintptr_t> MemoryPhantom::FindModuleBase(const char* moduleName) const
{
    VerifyHandle();

    MODULEENTRY32 entry;
    entry.dwSize = sizeof(MODULEENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
    if (snapshot == INVALID_HANDLE_VALUE)
        return std::nullopt;

    std::optional<uintptr_t> result = std::nullopt;

    if (Module32First(snapshot, &entry))
    {
        do
        {
            if (strcmp(entry.szModule, moduleName) == 0)
            {
                result = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
                break;
            }
        } while (Module32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return result;
}

template<typename T>
std::optional<T> MemoryPhantom::ReadData(uintptr_t addr) const
{
    T value;
    if (MemoryRead(reinterpret_cast<LPCVOID>(addr), &value, sizeof(T)))
    {
        return value;
    }
    return std::nullopt;
}

template<typename T>
std::optional<T> MemoryPhantom::ReadData(uintptr_t addr, int off) const
{
    return ReadData<T>(addr + off);
}

template<typename T>
bool MemoryPhantom::WriteData(uintptr_t addr, const T& value) const
{
    return MemoryWrite(reinterpret_cast<LPVOID>(addr), &value, sizeof(T));
}

template<typename T>
bool MemoryPhantom::WriteData(uintptr_t addr, int off, const T& value) const
{
    return WriteData<T>(addr + off, value);
}

std::optional<uintptr_t> MemoryPhantom::ReadPtr(uintptr_t addr) const
{
    return ReadData<uintptr_t>(addr);
}

std::optional<uintptr_t> MemoryPhantom::ReadPtr(uintptr_t addr, int off) const
{
    return ReadPtr(addr + off);
}

std::optional<std::vector<uint8_t>> MemoryPhantom::ReadBlock(uintptr_t addr, size_t sz) const
{
    std::vector<uint8_t> buffer(sz);
    if (MemoryRead(reinterpret_cast<LPCVOID>(addr), buffer.data(), sz))
    {
        return buffer;
    }
    return std::nullopt;
}

bool MemoryPhantom::WriteBlock(uintptr_t addr, const std::vector<uint8_t>& data) const
{
    return MemoryWrite(reinterpret_cast<LPVOID>(addr), data.data(), data.size());
}

std::optional<std::string> MemoryPhantom::ReadText(uintptr_t addr, size_t maxLen, bool nullEnded) const
{
    std::vector<char> buffer(maxLen + 1);
    if (MemoryRead(reinterpret_cast<LPCVOID>(addr), buffer.data(), maxLen))
    {
        if (nullEnded)
        {
            buffer[maxLen] = '\0';
        }
        return std::string(buffer.data());
    }
    return std::nullopt;
}

std::optional<std::wstring> MemoryPhantom::ReadWideText(uintptr_t addr, size_t maxLen, bool nullEnded) const
{
    std::vector<wchar_t> buffer(maxLen + 1);
    if (MemoryRead(reinterpret_cast<LPCVOID>(addr), buffer.data(), maxLen * sizeof(wchar_t)))
    {
        if (nullEnded)
        {
            buffer[maxLen] = L'\0';
        }
        return std::wstring(buffer.data());
    }
    return std::nullopt;
}

bool MemoryPhantom::WriteText(uintptr_t addr, const std::string& text, bool addNull) const
{
    size_t len = addNull ? text.size() + 1 : text.size();
    return MemoryWrite(reinterpret_cast<LPVOID>(addr), text.c_str(), len);
}

bool MemoryPhantom::WriteWideText(uintptr_t addr, const std::wstring& text, bool addNull) const
{
    size_t len = addNull ? (text.size() + 1) * sizeof(wchar_t) : text.size() * sizeof(wchar_t);
    return MemoryWrite(reinterpret_cast<LPVOID>(addr), text.c_str(), len);
}

std::optional<Vec3> MemoryPhantom::ReadVec3(uintptr_t addr) const
{
    return ReadData<Vec3>(addr);
}

bool MemoryPhantom::WriteVec3(uintptr_t addr, const Vec3& vec) const
{
    return WriteData<Vec3>(addr, vec);
}

std::optional<Mat4x4> MemoryPhantom::ReadMatrix(uintptr_t addr) const
{
    return ReadData<Mat4x4>(addr);
}

bool MemoryPhantom::WriteMatrix(uintptr_t addr, const Mat4x4& matrix) const
{
    return WriteData<Mat4x4>(addr, matrix);
}
