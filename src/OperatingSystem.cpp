#include "OperatingSystem.h"

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <time.h>
#endif

OperatingSystem::OperatingSystem(){}
OperatingSystem::~OperatingSystem(){}

double OperatingSystem::GetTickCount()
{
#ifdef _WIN32
    return ::GetTickCount();
#else
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now))
        return 0;
    return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
#endif
}
