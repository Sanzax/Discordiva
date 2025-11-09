#include <sstream>
#include <chrono>
#include <iostream>

#include "discordManager.h"
#include "constants.h"
#include "logger.h"
#include "memoryReading.h"
#include "formatting.h"

discord::Core* core = nullptr;
discord::Activity activity;

constexpr float DISCORD_INIT_TIMER = 5.0f;

void InitDiscord()
{
    discord::Core* corePtr{};
    auto result = discord::Core::Create(DISCORD_APP_ID, DiscordCreateFlags_NoRequireDiscord, &corePtr);

    if(!corePtr || result != discord::Result::Ok)
    {
        LogWarning("Failed to initialize Discord Core...");
        return;
    }

    Log("Discord initialized successfully!");

    core = corePtr;
}

void CreateActivity()
{
    activity = {};
    activity.GetTimestamps().SetStart(std::time(nullptr));
    activity.SetType(discord::ActivityType::Playing);
}

bool IsDiscordInitialized()
{
    return core != nullptr;
}

void ReleaseCore()
{
    delete core;
    core = nullptr;
}

void ConstructActivityData()
{
    static float previousElapsedDuration = -1.0f;

    bool isPlaying = ReadNumberFromMemory<bool>(IS_PLAYING_ADDRESS);

    if(!isPlaying)
    {
        UpdateDiscordActivity("", STATE_NAMES[StateNames::InMenu], STATE_NAMES[StateNames::InMenu]);
        return;
    }

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

    bool isPractice = ReadNumberFromMemory<bool>(IS_PRACTICE_ADDRESS);
    bool isMV = ReadNumberFromMemory<bool>(IS_MV_ADDRESS);

    int modifierIndex = ReadNumberFromMemory<int>(MODIFIER_ADDRESS);

    if(elapsed > maxDuration)
        elapsed = maxDuration;

    std::ostringstream stateSs;
    stateSs << "Clear: " << FormatPercentage(clearPercentage)
        << "% | Score: " << score
        << " | Combo: " << combo;

    std::ostringstream difficultySs;
    difficultySs << "[" << difficultyName << " " << FormatStarNumber(starAmount) << u8"⭐] ";

    // Determine game mode
    const char* gameMode = "";
    if(isMV)
    {
        gameMode = "[Music Video] ";
        stateSs.str("");
        stateSs.clear();

        difficultySs.str("");
        difficultySs.clear();
    }
    else if(isPractice)
    {
        gameMode = "[Practice/No Fail] ";
    }
    bool isClear = clearPercentage >= border;

    // Determine state text
    const char* smallText;
    if(isPaused)
        smallText = STATE_NAMES[StateNames::Paused];
    else if(elapsed < maxDuration && previousElapsedDuration != elapsed)
        smallText = STATE_NAMES[StateNames::Playing];
    else if(elapsed == maxDuration)
        smallText = isClear ? STATE_NAMES[StateNames::Clear] : STATE_NAMES[StateNames::Notclear];
    else
        smallText = isMV ? STATE_NAMES[StateNames::Paused] : STATE_NAMES[StateNames::Notclear];

    // Modifier text
    const char* modifier = MODIFIER_TYPES[modifierIndex];

    // Build details
    std::ostringstream details;

    std::string artistSeparator = artist != "" ? u8" — " : " ";

    details << gameMode << modifier << difficultySs.str() << songName << artistSeparator << artist
        << " | [" << FormatTime(elapsed) << " / " << FormatTime(maxDuration) << "]";

    UpdateDiscordActivity(stateSs.str().c_str(), details.str().c_str(), smallText);
    previousElapsedDuration = elapsed;
}

void UpdateDiscordActivity(const char* state, const char* details, const char* smallText)
{
    activity.SetState(state);
    activity.SetDetails(details);

    // Handle small image
    discord::ActivityAssets& assets = activity.GetAssets();
    try
    {
        const char* smallImageName = SMALL_IMAGE_NAMES.at(smallText).c_str();
        assets.SetSmallImage(smallImageName);
    }
    catch(...)
    {
        assets.SetSmallImage(SMALL_IMAGE_NAMES.at(STATE_NAMES[StateNames::InMenu]).c_str());
    };
    assets.SetSmallText(smallText);

    // Send the activity to Discord
    core->ActivityManager().UpdateActivity(activity, [](discord::Result result)
    {
        if(result != discord::Result::Ok)
            LogWarning("Failed to update Discord activity!");
    });
}