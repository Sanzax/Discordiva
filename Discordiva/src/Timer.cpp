#include "Timer.h"

Timer::Timer(double intervalSeconds) : _interval(intervalSeconds), _lastTime(Clock::now())
{
}

bool Timer::IsReady()
{
    auto now = Clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - _lastTime).count();
    if(elapsed < _interval)
        return false;

    _lastTime = now;
    return true;
}