#pragma once
#include "discord/discord.h"

extern discord::Core* core;
extern discord::Activity activity;

void InitDiscord();

void UpdateDiscordActivity(const char* state, const char* details, const char* smallText, bool isNew, const char* songName);