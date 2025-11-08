#include "logger.h"

#include <iostream>

void Log(const std::string& message)
{
	std::cout << MOD_NAME_PREFIX << ": " << message << "\n";
}

void LogWarning(const std::string& message)
{
	std::cout << MOD_NAME_PREFIX << WARNING_PREFIX << ": " << message << "\n";
}