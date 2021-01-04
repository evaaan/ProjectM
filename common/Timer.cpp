#include <Windows.h>
#include <stdexcept>
#include "Timer.h"
#include "Utilities.h"

Timer::Timer() :
    startTime(0),
    currentTime(0),
    pausedTime(0),
    previousTime(0),
    totalIdleTime(0),
    numTicks(0),
    secondsPerCount(0.0),
    deltaTime(0.0),
    isStopped(true)
{
    // Get frequency of performance counter
    long long int frequency = 0;
    if (QueryPerformanceFrequency((LARGE_INTEGER*)&frequency))
    {
        secondsPerCount = 1.0 / (double)frequency;
    }
    else
        throw std::runtime_error("Hardware does not support a high-precision timer!");
}

Timer::~Timer()
{
}

/* Returns the time elapsed between two frames in seconds. This is regularly updated during the game loop. */
double Timer::getDeltaTime() const
{
    return deltaTime;
}

/* Returns the time elapsed since starting the Timer in seconds. */
double Timer::getTotalTime() const
{
    if (isStopped)
        return (pausedTime - startTime - totalIdleTime) * secondsPerCount;
    else
        return (currentTime - startTime - totalIdleTime) * secondsPerCount;
}

/* Returns */
double Timer::getCurrentTime() const
{
    if (QueryPerformanceCounter((LARGE_INTEGER*)&currentTime))
    {
        return currentTime * secondsPerCount;
    }
    else
        throw std::runtime_error("Unable to query performance counter!");

}

/* Start the timer (if not already running) */
void Timer::start()
{
    if (isStopped)
    {
        long long int now = 0;

        if (QueryPerformanceCounter((LARGE_INTEGER*)&now))
        {
            // Set current time
            currentTime = now;

            // add time idle since stop() was last called
            totalIdleTime += (now - pausedTime);

            // set previous time to now
            previousTime = now;
            pausedTime = 0;
            isStopped = false;

        }
    }

}

void Timer::reset()
{
    long long int now = 0;
    if (QueryPerformanceCounter((LARGE_INTEGER*)&now))
    {
        startTime = now;
        previousTime = now;
        pausedTime = 0;
        isStopped = false;
        numTicks = 0;
    }
    else
        throw std::runtime_error("Unable to query performance counter!");

}

/* Compute the time since the last tick. This is called every cycle. */
void Timer::tick()
{
    // Update current time
    if (QueryPerformanceCounter((LARGE_INTEGER*)&currentTime))
    {
        // compute frame time
        deltaTime = (currentTime - previousTime) * secondsPerCount;

        previousTime = currentTime;

        //printf("deltaTime: ( %f ) sec\n", deltaTime);

        // deltaTime can be negative if processor goes idle, apparently
        if (deltaTime < 0.0)
            deltaTime = 0.0;

        // increment numTicks
        numTicks++;
    }
    else
        throw std::runtime_error("Unable to query performance counter!");
}

void Timer::stop()
{
    if (!isStopped)
    {
        long long int now = 0;

        if (QueryPerformanceCounter((LARGE_INTEGER*)&now))
        {
            // Set the time the timer was stopped to now
            pausedTime = now;
            isStopped = true;

        }
        else
            throw std::runtime_error("Unable to query performance counter!");
    }
}

/* Sleep until the given time has passed since the last tick. */
void Timer::sleep_until(int period_ms)
{
    if (QueryPerformanceCounter((LARGE_INTEGER*)&currentTime))
    {
        int sleep_ms = period_ms - (int)std::round((currentTime - previousTime) * secondsPerCount * 1000);
        //printf("sleep_ms: ( %d )ms\n", sleep_ms);

        if (sleep_ms > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
    }
    else
        throw std::runtime_error("Unable to query performance counter!");


}