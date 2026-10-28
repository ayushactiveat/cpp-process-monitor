#include "../include/Parser.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>

using namespace std;

string ProcParser::readFile(const string& path) {
    ifstream file(path);
    
    // Handle permission denied (e.g., reading root processes without sudo)
    if (!file.is_open()) {
        return "";
    }

    string content;
    // Handle race condition: process terminates mid-read
    if (!getline(file, content)) {
        return "";
    }
    
    return content;
}

ProcessInfo ProcParser::parseStat(const string& pid, const string& name, const string& statData) {
    ProcessInfo p;
    p.pid = pid;
    p.name = name;

    // Find the last ')' to handle process names with spaces (e.g., "Web Content")
    size_t pos = statData.rfind(')');
    if (pos == string::npos) return p;

    string remaining = statData.substr(pos + 2);
    stringstream ss(remaining);
    string temp;

    for (int i = 0; i < 11; i++) ss >> temp;

    ss >> p.utime >> p.stime;

    for (int i = 0; i < 8; i++) ss >> temp;

    long rss_pages = 0;
    ss >> rss_pages;
    
    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    p.memory_kb = rss_pages * page_size_kb;

    return p;
}

double ProcParser::calculateCpuPercent(long utime, long stime, long prev_utime, long prev_stime, long ticks_per_sec, double elapsed_sec) {
    if (elapsed_sec <= 0.0) return 0.0;
    long total_ticks = utime + stime;
    long prev_total_ticks = prev_utime + prev_stime;
    
    long delta = total_ticks - prev_total_ticks;
    if (delta < 0) delta = 0;
    
    double delta_sec = (double)delta / ticks_per_sec;
    return (delta_sec / elapsed_sec) * 100.0;
}
