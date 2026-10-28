#pragma once
#include <string>

struct ProcessInfo {
    std::string pid;
    std::string name;
    long memory_kb = 0;   // resident memory in KB
    long utime = 0;       // user-mode CPU time, in clock ticks
    long stime = 0;       // kernel-mode CPU time, in clock ticks
    double cpu_percent = 0.0;
};
