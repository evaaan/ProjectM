#pragma once
#include <chrono>
#include <cmath>
#include <thread>

class Timer
{
private:
    long long int startTime;                // start time (seconds)
    long long int currentTime;              // current time
    long long int pausedTime;               // time at the last pause
    long long int previousTime;             // time at the last inquiry before current
    long long int totalIdleTime;            // total paused time
    long long int numTicks;                 // number of ticks 

    double secondsPerCount;
    double deltaTime;
    bool isStopped;

public:
    Timer();
    ~Timer();

    double getTotalTime() const;
    double getDeltaTime() const;

    void start();
    void reset();
    void tick();
    void stop();
    void sleep_until(int period_microseconds);
};
