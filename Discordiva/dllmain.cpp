#include "pch.h"

#include "memoryReading.h"
#include "discordSetup.h"
#include "formatting.h"
#include "constants.h"


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
            UpdateDiscordActivity("", stateNames[StateNames::InMenu], stateNames[StateNames::InMenu], isNew);
        return;
    }

    // Game data
    std::string songName = ReadStringFromMemory(SONG_NAME_ADDRESS);
    std::string artist = ReadStringFromMemory(ARTIST_ADDRESS);

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

    bool isNoFail = ReadNumberFromMemory<bool>(IS_NO_FAIL_ADDRESS);
    bool isPractice = ReadNumberFromMemory<bool>(IS_PRACTICE_ADDRESS);
    bool isMV = ReadNumberFromMemory<bool>(IS_MV_ADDRESS);

    int modifierIndex = ReadNumberFromMemory<int>(MODIFIER_ADDRESS);

    if(elapsed > maxDuration)
        elapsed = maxDuration;

    std::ostringstream statess;
    statess << "Clear: " << FormatPercentage(clearPercentage)
        << "% | Score: " << score
        << " | Combo: " << combo;

    std::ostringstream difficulty;
    difficulty << "[" << difficultyName << " " << FormatStarNumber(starAmount) << u8"⭐] ";

    const char* gameMode = "";
    if(isMV)
    {
        gameMode = "[MV] ";
        statess.str("");
        statess.clear();

        difficulty.str("");
        difficulty.clear();
    }
    else if(isPractice)
    {
        gameMode = "[Practice] ";
        statess.str("");
        statess.clear();
    }
    else if(isNoFail)
    {
        gameMode = "[No Fail] ";
    }

    bool isClear = clearPercentage >= border;

    // Determine state text
    std::string smallText;
    if(isPaused)
        smallText = stateNames[StateNames::Paused];
    else if(elapsed < maxDuration && previousElapsedDuration != elapsed)
        smallText = stateNames[StateNames::Playing];
    else if(elapsed == maxDuration)
        smallText = isClear ? stateNames[StateNames::Clear] : stateNames[StateNames::Notclear];
    else
        smallText = isMV ? stateNames[StateNames::Paused] : stateNames[StateNames::Notclear];

    const char* modifier = MODIFIER_TYPES[modifierIndex];

    // Build details
    std::ostringstream details;
    details << gameMode << modifier << difficulty.str() << songName << u8" — " << artist
        << " | [" << FormatTime(elapsed) << " / " << FormatTime(maxDuration) << "]";

    UpdateDiscordActivity(statess.str().c_str(), details.str().c_str(), smallText.c_str(), isNew);
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

        if(elapsed >= ACTIVITY_UPDATE_DELAY)
        {
            ConstructActivityData();
            lastUpdate = now;
        }

        core->RunCallbacks();
    }
}