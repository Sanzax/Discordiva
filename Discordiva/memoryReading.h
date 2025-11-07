#pragma once

extern uintptr_t baseAddress;

void InitializeBaseAddress();

std::string ReadStringFromMemory(uintptr_t offset);

template <typename T>
T ReadNumberFromMemory(uintptr_t offset)
{
    if(!baseAddress)
        InitializeBaseAddress();

    const uintptr_t address = baseAddress + offset;

    // Safety: verify pointer is valid before reading
    if(!address || IsBadReadPtr(reinterpret_cast<const void*>(address), sizeof(T)))
        return static_cast<T>(0);

    return *reinterpret_cast<const T*>(address);
}