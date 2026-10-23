#pragma once
#include <string>

struct ProcessInfo {
    std::string pid;
    std::string name;
    long memory_kb = 0;
    long utime = 0;
    long stime = 0;
    double cpu_percent = 0.0;
};
