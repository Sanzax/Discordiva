#include "logger.h"
#include "memoryReading.h"
#include "discordManager.h"
#include "constants.h"
#include "Timer.h"


extern "C"
{
    __declspec(dllexport) void Init()
    {
        Log("Initialized!");
        InitializeBaseAddress();
        InitDiscord();
        CreateActivity();
    }

    __declspec(dllexport) void OnFrame(IDXGISwapChain*)
    {
        static Timer discordActivityUpdateTimer(ACTIVITY_UPDATE_INTERVAL);

        // If discord is uninitialized, try to initialize it
        if(!IsDiscordInitialized())
        {
            static Timer discordInitTimer(DISCORD_INIT_INTERVAL);

            // Try to initialize on intervals
            if(discordInitTimer.IsReady())
                InitDiscord();

            return;
        }

        discord::Result callbacksResult = core->RunCallbacks();

        // Handle issues with discord client
        if(callbacksResult != discord::Result::Ok)
        {
            if(callbacksResult == discord::Result::NotRunning)
            {
                LogWarning("Discord client closed...");
            }
            else
            {
                LogWarning(std::string("Discord callback failed with result: ") + std::to_string(static_cast<int>(callbacksResult)));
            }

            // Release Core so we can re-init later
            ReleaseCore();
            return;
        }

        // Update activity on intervals
        if(discordActivityUpdateTimer.IsReady())
        {
            ConstructActivityData();
        }
    }
}