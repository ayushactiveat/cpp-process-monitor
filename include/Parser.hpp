#pragma once
#include <string>
#include "Process.hpp"

class ProcParser {
public:
    // Returns the first line of the file, or "" if it can't be opened or read
    // (process exited, permission denied, empty file).
    static std::string readFile(const std::string& path);

    // statData is one line from /proc/[pid]/stat. If the line is missing or
    // malformed, the numeric fields are left at 0.
    static ProcessInfo parseStat(const std::string& pid,
                                 const std::string& name,
                                 const std::string& statData);

    // CPU usage over one sampling interval, as a percentage of one core.
    // A process using two full cores gives about 200.
    static double calculateCpuPercent(long utime, long stime,
                                      long prev_utime, long prev_stime,
                                      long ticks_per_sec, double elapsed_sec);
};
