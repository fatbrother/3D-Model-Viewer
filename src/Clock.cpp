#include "Clock.h"

Clock::Clock() {
    startTime = std::chrono::steady_clock::now();
}

Clock::~Clock() {}

void Clock::Reset() {
    startTime = std::chrono::steady_clock::now();
}

double Clock::GetElapsedTime() {
    std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = currentTime - startTime;
    return elapsedTime.count();
}