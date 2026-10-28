#include "Parser.hpp"

#include <fstream>
#include <sstream>
#include <unistd.h>

namespace {
// See `man 5 proc`, /proc/[pid]/stat. Counting from the field after the
// closing ')' (field 3, the state), utime is field 14 and rss is field 24.
constexpr int kFieldsBeforeUtime = 11;      // fields 3..13
constexpr int kFieldsBetweenStimeAndRss = 8; // fields 16..23
}

std::string ProcParser::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }

    std::string content;
    if (!std::getline(file, content)) {
        return "";
    }
    return content;
}

ProcessInfo ProcParser::parseStat(const std::string& pid,
                                  const std::string& name,
                                  const std::string& statData) {
    ProcessInfo p;
    p.pid = pid;
    p.name = name;

    // The process name sits in parentheses and can contain spaces or even
    // ')', so find the LAST ')' and parse everything after it.
    size_t pos = statData.rfind(')');
    if (pos == std::string::npos || pos + 2 > statData.size()) {
        return p;
    }

    std::stringstream ss(statData.substr(pos + 2));
    std::string skipped;

    for (int i = 0; i < kFieldsBeforeUtime; i++) ss >> skipped;

    long utime = 0;
    long stime = 0;
    ss >> utime >> stime;

    for (int i = 0; i < kFieldsBetweenStimeAndRss; i++) ss >> skipped;

    long rss_pages = 0;
    ss >> rss_pages;

    // Line was too short or garbled: leave everything at 0
    if (ss.fail()) {
        return p;
    }

    p.utime = utime;
    p.stime = stime;
    // rss in /proc/[pid]/stat is counted in pages, not KB
    p.memory_kb = rss_pages * (sysconf(_SC_PAGESIZE) / 1024);
    return p;
}

double ProcParser::calculateCpuPercent(long utime, long stime,
                                       long prev_utime, long prev_stime,
                                       long ticks_per_sec, double elapsed_sec) {
    if (elapsed_sec <= 0.0 || ticks_per_sec <= 0) {
        return 0.0;
    }

    long delta = (utime + stime) - (prev_utime + prev_stime);
    // A reused PID can make the counter go backwards
    if (delta < 0) {
        delta = 0;
    }

    double cpu_seconds = static_cast<double>(delta) / ticks_per_sec;
    return cpu_seconds / elapsed_sec * 100.0;
}
