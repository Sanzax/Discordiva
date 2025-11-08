#pragma once
#include <string>

constexpr const char* MOD_NAME_PREFIX = "[Discordiva]";
constexpr const char* WARNING_PREFIX = "[Warning]";

void Log(const std::string& message);

void LogWarning(const std::string& message);