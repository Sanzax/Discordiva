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

// Simple UTF-8 validation function
bool isValidUtf8(const char* data, size_t len)
{
    size_t i = 0;
    while(i < len)
    {
        unsigned char c = static_cast<unsigned char>(data[i]);
        if(c <= 0x7F) // 1-byte ASCII
        {
            i += 1;
        }
        else if((c & 0xE0) == 0xC0) // 2-byte sequence
        {
            if(i + 1 >= len) return false;
            if((static_cast<unsigned char>(data[i + 1]) & 0xC0) != 0x80) return false;
            i += 2;
        }
        else if((c & 0xF0) == 0xE0) // 3-byte sequence
        {
            if(i + 2 >= len) return false;
            if((static_cast<unsigned char>(data[i + 1]) & 0xC0) != 0x80) return false;
            if((static_cast<unsigned char>(data[i + 2]) & 0xC0) != 0x80) return false;
            i += 3;
        }
        else if((c & 0xF8) == 0xF0) // 4-byte sequence
        {
            if(i + 3 >= len) return false;
            if((static_cast<unsigned char>(data[i + 1]) & 0xC0) != 0x80) return false;
            if((static_cast<unsigned char>(data[i + 2]) & 0xC0) != 0x80) return false;
            if((static_cast<unsigned char>(data[i + 3]) & 0xC0) != 0x80) return false;
            i += 4;
        }
        else
        {
            return false; // invalid UTF-8 byte
        }
    }
    return true;
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

    auto looksLikeUtf8String = [](const char* data, size_t len) -> bool
    {
        // Must have at least one character before null
        if(len < 2) return false;
        return isValidUtf8(data, len - 1);
    };

    // 1. Attempt to interpret memory as a pointer
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
            char buffer[maxLen + 1] = {};
            memcpy(buffer, reinterpret_cast<const void*>(potentialPtr), readLen);
            buffer[maxLen] = '\0';

            const char* nullp = reinterpret_cast<const char*>(memchr(buffer, '\0', readLen));
            if(nullp && looksLikeUtf8String(buffer, nullp - buffer + 1))
            {
                return std::string(buffer);
            }
        }
    }

    // 2. Fallback: interpret memory as inline UTF-8 string
    {
        const size_t readLen = std::min<size_t>(maxLen, mbi.RegionSize);
        char buffer[maxLen + 1] = {};
        memcpy(buffer, reinterpret_cast<const void*>(address), readLen);
        buffer[maxLen] = '\0';

        const char* nullp = reinterpret_cast<const char*>(memchr(buffer, '\0', readLen));
        if(nullp && looksLikeUtf8String(buffer, nullp - buffer + 1))
        {
            return std::string(buffer);
        }
    }

    return {};
}
