#pragma once
#include <string>

using namespace std;

struct ProcessInfo {
    string pid;
    string name;
    long memory_kb = 0;
    long utime = 0;
    long stime = 0;
    double cpu_percent = 0.0;
};
