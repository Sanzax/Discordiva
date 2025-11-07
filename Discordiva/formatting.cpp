#include "pch.h"
#include "formatting.h"

std::string FormatStarNumber(float value)
{
    std::ostringstream oss;
    if(std::fabs(value - std::round(value)) < 1e-9)
        oss << static_cast<int>(std::round(value));
    else
        oss << std::fixed << std::setprecision(1) << value;
    return oss.str();
}

std::string FormatPercentage(float value)
{
    float truncated = std::trunc(value * 100.0f) / 100.0f;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << truncated;
    return oss.str();
}

std::string FormatTwoDigits(int number)
{
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << number;
    return oss.str();
}

std::string FormatTime(float seconds)
{
    int total = static_cast<int>(seconds);
    int minutes = total / 60;
    int secs = total % 60;

    std::ostringstream oss;
    oss << FormatTwoDigits(minutes) << ":" << FormatTwoDigits(secs);
    return oss.str();
}