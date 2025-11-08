#pragma once
#include <chrono>

class Timer
{
public:
    using Clock = std::chrono::steady_clock;

    Timer(double intervalSeconds);

    bool IsReady();

private:
    double _interval;
    Clock::time_point _lastTime;
};
