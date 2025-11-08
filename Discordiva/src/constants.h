#pragma once
#include <Windows.h>
#include <unordered_map>

// Address offsets
constexpr uintptr_t SONG_NAME_ADDRESS = 0x0CC0B5F8;
constexpr uintptr_t ARTIST_ADDRESS = 0x0CC0B618;
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
constexpr uintptr_t IS_MV_ADDRESS = 0x0CC6E428;
constexpr uintptr_t IS_PRACTICE_ADDRESS = 0x012EF6E8; 


constexpr uintptr_t IS_NO_FAIL_ADDRESS = 0x0114982A;
constexpr uintptr_t MODIFIER_ADDRESS = 0x016E2BB8;

constexpr const char* MODULE_NAME = "DivaMegaMix.exe";

constexpr long long DISCORD_APP_ID = 1434669921664503892;

constexpr float ACTIVITY_UPDATE_INTERVAL= 5.0f;
constexpr float DISCORD_INIT_INTERVAL = 5.0f;



constexpr const char* DIFFICULTY_TYPES[] =
{
    "Easy",
    "Normal",
    "Hard",
    "Extreme",
    "Ex Extreme"
};

constexpr const char* MODIFIER_TYPES[] =
{
    "",
    "[HI SPEED] ",
    "[HIDDEN] ",
    "[SUDDEN] ",
};

enum StateNames
{
    InMenu,
    Playing,
    Paused,
    Clear,
    Notclear,
};

constexpr const char* STATE_NAMES[] =
{
    "In Menu",
    "Playing",
    "Paused",
    "Clear!",
    "Not clear...",
};

const std::unordered_map<std::string, std::string> SMALL_IMAGE_NAMES = {
    {STATE_NAMES[StateNames::InMenu], "home"},
    {STATE_NAMES[StateNames::Playing], "playing"},
    {STATE_NAMES[StateNames::Paused], "paused"},
    {STATE_NAMES[StateNames::Clear], "clear"},
    {STATE_NAMES[StateNames::Notclear], "not_clear"},
};