#pragma once
#include <string>
#include "Process.hpp"

using namespace std;

class ProcParser {
public:
    static string readFile(const string& path);
    static ProcessInfo parseStat(const string& pid, const string& name, const string& statData);
    static double calculateCpuPercent(long utime, long stime, long prev_utime, long prev_stime, long ticks_per_sec, double elapsed_sec);
};
