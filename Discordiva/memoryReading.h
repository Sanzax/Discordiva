#pragma once

// ─────────────────────────────────────────────
// Game memory addresses (static offsets)
// ─────────────────────────────────────────────
constexpr uintptr_t SONG_NAME_ADDRESS = 0x0CC0B5F8;
constexpr uintptr_t STAR_AMOUNT_ADDRESS = 0x016E2BFC;
constexpr uintptr_t DIFFICULTY_TYPE_ADDRESS = 0x012B634C;
constexpr uintptr_t CLEAR_PERCENTAGE_ADDRESS = 0x012EF634;
constexpr uintptr_t BORDER_ADDRESS = 0x012EF63C;
constexpr uintptr_t SCORE_ADDRESS = 0x012EF568;
constexpr uintptr_t COMBO_ADDRESS = 0x012EF578;
constexpr uintptr_t MAX_DURATION_ADDRESS = 0x012EF668;
constexpr uintptr_t ELAPSED_DURATION_ADDRESS = 0x012EF66C;
constexpr uintptr_t PAUSED_ADDRESS = 0x0CC0CBF4;
constexpr uintptr_t FOCUS_LOST_ADDRESS = 0x00DC1401;
constexpr uintptr_t IS_PLAYING_ADDRESS = 0x012C2330;

// ─────────────────────────────────────────────
// Base application info
// ─────────────────────────────────────────────
constexpr const char* APP_NAME = "DivaMegaMix.exe";

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