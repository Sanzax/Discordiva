#include "pch.h"

#include "memoryReading.h"
#include "DiscordSetup.h"

// ─────────────────────────────────────────────
// State tracking
// ─────────────────────────────────────────────

constexpr const char* DIFFICULTY_TYPES[] =
{
    "Easy",
    "Normal",
    "Hard",
    "Extreme",
    "Ex Extreme"
};


// ─────────────────────────────────────────────
// Helper functions
// ─────────────────────────────────────────────
inline std::string FormatTwoDigits(int number)
{
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << number;
    return oss.str();
}

inline std::string FormatStarNumber(float value)
{
    std::ostringstream oss;
    if(std::fabs(value - std::round(value)) < 1e-9)
        oss << static_cast<int>(std::round(value));
    else
        oss << std::fixed << std::setprecision(1) << value;
    return oss.str();
}

inline std::string FormatPercentage(float value)
{
    float truncated = std::trunc(value * 100.0f) / 100.0f;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << truncated;
    return oss.str();
}

inline std::string FormatTime(float seconds)
{
    int total = static_cast<int>(seconds);
    int minutes = total / 60;
    int secs = total % 60;

    std::ostringstream oss;
    oss << FormatTwoDigits(minutes) << ":" << FormatTwoDigits(secs);
    return oss.str();
}

// ─────────────────────────────────────────────
// Main logic
// ─────────────────────────────────────────────
void ConstructActivityData()
{
    static bool previousState = -1;
    static float previousElapsedDuration = -1.0f;

    bool isPlaying = ReadNumberFromMemory<bool>(IS_PLAYING_ADDRESS);
    bool isNew = (previousState != isPlaying);
    previousState = isPlaying;

    if(!isPlaying)
    {
        if(isNew)
            UpdateDiscordActivity(u8"🏠 In menu", "", isNew);
        return;
    }

    // Game data
    std::string songName = ReadStringFromMemory(SONG_NAME_ADDRESS);

    int difficultyIndex = ReadNumberFromMemory<int>(DIFFICULTY_TYPE_ADDRESS);
    const char* difficultyName = DIFFICULTY_TYPES[difficultyIndex];

    int starAmountTimesTwo = ReadNumberFromMemory<int>(STAR_AMOUNT_ADDRESS);
    float starAmount = starAmountTimesTwo / 2.0f;

    float border = ReadNumberFromMemory<float>(BORDER_ADDRESS);
    float clearPercentage = ReadNumberFromMemory<float>(CLEAR_PERCENTAGE_ADDRESS);
    int score = ReadNumberFromMemory<int>(SCORE_ADDRESS);
    int combo = ReadNumberFromMemory<int>(COMBO_ADDRESS);

    float elapsed = ReadNumberFromMemory<float>(ELAPSED_DURATION_ADDRESS);
    float maxDuration = ReadNumberFromMemory<float>(MAX_DURATION_ADDRESS);

    bool isPaused = ReadNumberFromMemory<bool>(PAUSED_ADDRESS);

    if(elapsed > maxDuration)
        elapsed = maxDuration;

    // Determine clear status
    bool isClear = clearPercentage >= border;
    const char* clearEmoji = isClear ? u8"✅" : u8"❌";

    // Determine state text
    std::string state;
    if(isPaused)
        state = "Paused";
    else if(elapsed < maxDuration && previousElapsedDuration != elapsed)
        state = "Playing";
    else if(elapsed == maxDuration)
        state = isClear ? "Clear!" : "Not clear...";
    else
        state = "Drop out...";

    // Build details
    std::ostringstream details;
    details << "[" << difficultyName << " " << FormatStarNumber(starAmount) << u8"⭐] "
        << songName
        << " | [" << FormatTime(elapsed) << " / " << FormatTime(maxDuration) << "]"
        << " | Clear(" << clearEmoji << "): " << FormatPercentage(clearPercentage)
        << "% | Score: " << score
        << " | Combo: " << combo;

    UpdateDiscordActivity(state.c_str(), details.str().c_str(), isNew);
    previousElapsedDuration = elapsed;
}

extern "C"
{
    __declspec(dllexport) void Init()
    {
        InitializeBaseAddress();
        InitDiscord();
    }

    __declspec(dllexport) void OnFrame(IDXGISwapChain*)
    {
        using Clock = std::chrono::steady_clock;

        static auto lastUpdate = Clock::now();

        auto now = Clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate).count();

        if(elapsed >= 5)
        {
            ConstructActivityData();
            lastUpdate = now;
        }

        core->RunCallbacks();
    }
}