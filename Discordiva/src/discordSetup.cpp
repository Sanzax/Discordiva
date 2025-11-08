#include <sstream>
#include <chrono>
#include <iostream>
#include "discordSetup.h"
#include "constants.h"

discord::Core* core = nullptr;
discord::Activity activity{};

void InitDiscord()
{
    discord::Core* corePtr{};
    auto result = discord::Core::Create(DISCORD_APP_ID, DiscordCreateFlags_Default, &corePtr);

    if(!corePtr || result != discord::Result::Ok)
    {
        std::cout << "Failed to initialize Discord Core!\n";
        return;
    }

    std::cout << "Discord initialized successfully!\n";

    core = corePtr;

    activity.SetType(discord::ActivityType::Playing);
}

void UpdateDiscordActivity(const char* state, const char* details, const char* smallText, bool isNew, const char* songName)
{
    if(isNew)
    {
        activity = {};
        activity.GetTimestamps().SetStart(std::time(nullptr));
    }

    activity.SetState(state);
    activity.SetDetails(details);

    discord::ActivityAssets& assets = activity.GetAssets();
    try
    {
        const char* imageName = SMALL_IMAGE_NAMES.at(smallText).c_str();
        assets.SetSmallImage(imageName);
    }
    catch(const std::out_of_range&)
    {
        assets.SetSmallImage("home");
    };
    assets.SetSmallText(smallText);

    // Send the activity to Discord
    core->ActivityManager().UpdateActivity(activity, [](discord::Result result)
    {
        if(result != discord::Result::Ok)
            std::cout << "Failed to update Discord activity!\n";
    });
}