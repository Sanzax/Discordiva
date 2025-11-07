#include "pch.h"
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
        printf("Failed to initialize Discord Core!\n");
        return;
    }

    printf("Discord initialized!\n");

    core = corePtr;

    activity.SetType(discord::ActivityType::Playing);
}

void UpdateDiscordActivity(const char* state, const char* details, const char* smallText, bool isNew)
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
        assets.SetSmallImage(SMALL_IMAGE_NAMES.at(smallText).c_str());
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
            printf("Failed to update Discord activity!\n");
    });
}