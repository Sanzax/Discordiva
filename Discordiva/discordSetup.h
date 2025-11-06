#pragma once
#include "pch.h"
#include "discord/discord.h"

const long long DISCORD_APP_ID = 1434669921664503892;

extern discord::Core* core;
extern discord::Activity activity;

void InitDiscord();

void UpdateDiscordActivity(const char* state, const char* details, bool isNew);