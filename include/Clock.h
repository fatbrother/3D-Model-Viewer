#pragma once

#include <chrono>

class Clock {
public:
    Clock();
    ~Clock();

    /**
     * @brief Reset the clock.
     * 
     * @note This function should be called before the rendering loop.
    */
    void Reset();

    /**
     * @brief Get the elapsed time since the clock was reset.
     * 
     * @return double
    */
    double GetElapsedTime();

private:
    // Clock Private Data.
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};