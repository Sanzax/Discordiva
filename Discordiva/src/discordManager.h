#pragma once
#include "discord/discord.h"

extern discord::Core* core;
extern discord::Activity activity;

void InitDiscord();

void CreateActivity();

bool IsDiscordInitialized();

void ConstructActivityData();

void UpdateDiscordActivity(const char* state, const char* details, const char* smallText, const char* songName);

void ReleaseCore();