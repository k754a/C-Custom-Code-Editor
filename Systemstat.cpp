
#include "Systemstat.h"
#include <Windows.h> 

float GetCPUUsage() {
    //this gets the app cpu usage
    static FILETIME prevIdleTime = { 0 }, prevKernelTime = { 0 }, prevUserTime = { 0 };

    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        ULONGLONG idleDiff = ((ULONGLONG)idleTime.dwLowDateTime | ((ULONGLONG)idleTime.dwHighDateTime << 32)) - ((ULONGLONG)prevIdleTime.dwLowDateTime | ((ULONGLONG)prevIdleTime.dwHighDateTime << 32));
        ULONGLONG kernelDiff = ((ULONGLONG)kernelTime.dwLowDateTime | ((ULONGLONG)kernelTime.dwHighDateTime << 32)) - ((ULONGLONG)prevKernelTime.dwLowDateTime | ((ULONGLONG)prevKernelTime.dwHighDateTime << 32));
        ULONGLONG userDiff = ((ULONGLONG)userTime.dwLowDateTime | ((ULONGLONG)userTime.dwHighDateTime << 32)) - ((ULONGLONG)prevUserTime.dwLowDateTime | ((ULONGLONG)prevUserTime.dwHighDateTime << 32));

        ULONGLONG totalDiff = kernelDiff + userDiff;
        float cpuUsage = (totalDiff > 0) ? (1.0f - (float)idleDiff / totalDiff) * 100.0f : 0.0f;

        prevIdleTime = idleTime;
        prevKernelTime = kernelTime;
        prevUserTime = userTime;

        return cpuUsage;
    }
    return 0.0f;
}

float GetRAMUsage() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (GlobalMemoryStatusEx(&status)) {
        return (1.0f - (float)status.ullAvailPhys / status.ullTotalPhys) * 100.0f;
    }
    return 0.0f;
}
