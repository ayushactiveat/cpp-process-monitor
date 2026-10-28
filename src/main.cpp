#include <algorithm>
#include <iostream>
#include <filesystem>
#include <vector>
#include <iomanip>
#include <thread>
#include <chrono>
#include <map>
#include <unistd.h>
#include "../include/Parser.hpp"

using namespace std;
namespace fs = std::filesystem;

int main() {
    long ticks_per_sec = sysconf(_SC_CLK_TCK);
    map<string, pair<long, long>> prev_ticks;
    auto prev_time = chrono::steady_clock::now();

    while (true) {
        auto current_time = chrono::steady_clock::now();
        double elapsed_sec = chrono::duration<double>(current_time - prev_time).count();
        prev_time = current_time;

        vector<ProcessInfo> processes;
        for (const auto& entry : fs::directory_iterator("/proc")) {
            string pid = entry.path().filename().string();
            if (pid.find_first_not_of("0123456789") == string::npos) {
                string name = ProcParser::readFile("/proc/" + pid + "/comm");
                string stat = ProcParser::readFile("/proc/" + pid + "/stat");
                
                // Skips processes that died mid-read or lacked permissions (returns empty string)
                if (!name.empty() && !stat.empty()) {
                    processes.push_back(ProcParser::parseStat(pid, name, stat));
                }
            }
        }

        map<string, pair<long, long>> next_prev_ticks;

        for (auto& p : processes) {
            if (prev_ticks.find(p.pid) != prev_ticks.end()) {
                long prev_ut = prev_ticks[p.pid].first;
                long prev_st = prev_ticks[p.pid].second;
                p.cpu_percent = ProcParser::calculateCpuPercent(p.utime, p.stime, prev_ut, prev_st, ticks_per_sec, elapsed_sec);
            }
            next_prev_ticks[p.pid] = {p.utime, p.stime};
        }
        // Rebuild map to prevent dead PIDs from lingering and causing negative deltas
        prev_ticks = next_prev_ticks;

        sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
            return a.cpu_percent > b.cpu_percent;
        });

        cout << "\033[2J\033[1;1H";
        cout << "=== Linux Process Monitor ===\n\n";
        cout << left << setw(10) << "PID" << setw(10) << "CPU(%)" << setw(15) << "MEMORY(KB)" << "NAME" << "\n";
        cout << "------------------------------------------------------------\n";
        
        int count = 0;
        for (const auto& p : processes) {
            if (count >= 15) break;
            cout << left << setw(10) << p.pid 
                 << setw(10) << fixed << setprecision(2) << p.cpu_percent 
                 << setw(15) << p.memory_kb 
                 << p.name << "\n";
            count++;
        }
        this_thread::sleep_for(chrono::seconds(1));
    }
    return 0;
}
