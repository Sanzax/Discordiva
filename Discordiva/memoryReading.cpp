#include "pch.h"
#include "memoryReading.h"

uintptr_t baseAddress = 0;

void InitializeBaseAddress()
{
    if(baseAddress)
        return;

    if(HMODULE hModule = GetModuleHandleA(APP_NAME))
        baseAddress = reinterpret_cast<uintptr_t>(hModule);
}

std::string ReadStringFromMemory(uintptr_t offset)
{
    size_t maxLen = 256;
    const uintptr_t address = baseAddress + offset;
    if(!address)
        return std::string();

    MEMORY_BASIC_INFORMATION mbi = {};
    if(VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi)) == 0 || mbi.State != MEM_COMMIT)
        return std::string();

    // 1. Check if memory holds a pointer to a heap string
    uintptr_t potentialHeapPtr = 0;
    memcpy(&potentialHeapPtr, reinterpret_cast<const void*>(address), sizeof(uintptr_t));

    if(potentialHeapPtr &&
       VirtualQuery(reinterpret_cast<LPCVOID>(potentialHeapPtr), &mbi, sizeof(mbi)) != 0 &&
       mbi.State == MEM_COMMIT)
    {
        // Read a limited portion to avoid buffer overruns
        const SIZE_T readLen = (mbi.RegionSize < maxLen) ? mbi.RegionSize : maxLen;

        std::string result(readLen, '\0');
        memcpy(&result[0], reinterpret_cast<const void*>(potentialHeapPtr), readLen);

        // Trim at null terminator if found
        const size_t nullPos = result.find('\0');
        if(nullPos != std::string::npos)
            result.resize(nullPos);

        return result;
    }

    // 2. Fallback: assume inline or short string stored directly
    char buffer[16] = { 0 };
    const SIZE_T readLen = (mbi.RegionSize < sizeof(buffer)) ? mbi.RegionSize : sizeof(buffer);
    memcpy(buffer, reinterpret_cast<const void*>(address), readLen);

    const void* nullPtr = memchr(buffer, 0, readLen);
    if(nullPtr)
        return std::string(buffer, static_cast<const char*>(nullPtr) - buffer);

    // No null terminator found; return the full buffer content
    return std::string(buffer, readLen);
}