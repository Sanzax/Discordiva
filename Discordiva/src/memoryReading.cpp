#include "logger.h"
#include "memoryReading.h"
#include "constants.h"

uintptr_t baseAddress = 0;

void InitializeBaseAddress()
{
    if(baseAddress)
        return;

    if(HMODULE hModule = GetModuleHandleA(MODULE_NAME))
    {
        baseAddress = reinterpret_cast<uintptr_t>(hModule);
        return;
    }

    LogWarning("Failed to initialize base address...");
}

std::string ReadStringFromMemory(uintptr_t offset)
{
    if(!baseAddress)
        InitializeBaseAddress();

    constexpr size_t maxLen = 64;
    const uintptr_t address = baseAddress + offset;

    if(!address)
        return {};

    MEMORY_BASIC_INFORMATION mbi = {};
    if(VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi)) == 0 ||
       mbi.State != MEM_COMMIT)
    {
        return {};
    }

    // Helper lambda: validate ASCII-ish strings
    auto looksLikeString = [](const char* data, size_t len) -> bool
    {
        size_t printableCount = 0;

        for(size_t i = 0; i < len; ++i)
        {
            unsigned char c = data[i];
            if(c == '\0')
                return printableCount >= 1;  // at least 1 printable char before null

            if(c >= 32 && c <= 126) // normal ASCII printable
                printableCount++;
            else
                return false; // non-printable → not a string
        }

        return false; // no null terminator within len
    };

    // 1. Attempt to interpret the memory as a pointer first
    uintptr_t potentialPtr = 0;
    memcpy(&potentialPtr, reinterpret_cast<const void*>(address), sizeof(uintptr_t));

    if(potentialPtr)
    {
        MEMORY_BASIC_INFORMATION mbi2 = {};
        if(VirtualQuery(reinterpret_cast<LPCVOID>(potentialPtr), &mbi2, sizeof(mbi2)) != 0 &&
           mbi2.State == MEM_COMMIT &&
           mbi2.RegionSize > 1)
        {
            const size_t readLen = std::min<size_t>(maxLen, mbi2.RegionSize);

            // Temporary read buffer
            char buffer[maxLen + 1] = {};
            memcpy(buffer, reinterpret_cast<const void*>(potentialPtr), readLen);
            buffer[maxLen] = '\0';

            // Check for null terminator within readLen
            const char* nullp = reinterpret_cast<const char*>(memchr(buffer, '\0', readLen));

            if(nullp && looksLikeString(buffer, nullp - buffer + 1))
            {
                return std::string(buffer);
            }
        }
    }

    // 2. Fallback: interpret memory as an inline string
    {
        const size_t readLen = std::min<size_t>(maxLen, mbi.RegionSize);

        char buffer[maxLen + 1] = {};
        memcpy(buffer, reinterpret_cast<const void*>(address), readLen);
        buffer[maxLen] = '\0';

        const char* nullp = reinterpret_cast<const char*>(memchr(buffer, '\0', readLen));

        if(nullp && looksLikeString(buffer, nullp - buffer + 1))
        {
            return std::string(buffer);
        }
    }

    // If neither case looks like a string, return empty string
    return {};
}
