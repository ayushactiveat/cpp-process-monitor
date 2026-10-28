#include <algorithm>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <map>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>

#include "Parser.hpp"

namespace fs = std::filesystem;

namespace {

constexpr int kTopN = 15;
constexpr auto kRefreshInterval = std::chrono::seconds(1);

volatile std::sig_atomic_t g_running = 1;
void handleSigint(int) { g_running = 0; }

bool isNumeric(const std::string& s) {
    return !s.empty() && s.find_first_not_of("0123456789") == std::string::npos;
}

std::vector<ProcessInfo> scanProc() {
    std::vector<ProcessInfo> processes;
    try {
        for (const auto& entry : fs::directory_iterator("/proc")) {
            std::string pid = entry.path().filename().string();
            if (!isNumeric(pid)) continue;

            std::string name = ProcParser::readFile("/proc/" + pid + "/comm");
            std::string stat = ProcParser::readFile("/proc/" + pid + "/stat");

            // Empty means the process exited or we aren't allowed to read it
            if (name.empty() || stat.empty()) continue;

            processes.push_back(ProcParser::parseStat(pid, name, stat));
        }
    } catch (const fs::filesystem_error&) {
        // /proc changed while scanning; use what we have for this round
    }
    return processes;
}

}  // namespace

int main() {
    std::signal(SIGINT, handleSigint);

    const long ticks_per_sec = sysconf(_SC_CLK_TCK);
    std::map<std::string, std::pair<long, long>> prev_ticks;  // pid -> (utime, stime)
    auto prev_time = std::chrono::steady_clock::now();

    while (g_running) {
        auto now = std::chrono::steady_clock::now();
        double elapsed_sec = std::chrono::duration<double>(now - prev_time).count();
        prev_time = now;

        std::vector<ProcessInfo> processes = scanProc();

        // Only keep ticks for processes still alive, so dead PIDs don't pile up
        std::map<std::string, std::pair<long, long>> next_ticks;
        for (auto& p : processes) {
            auto it = prev_ticks.find(p.pid);
            if (it != prev_ticks.end()) {
                p.cpu_percent = ProcParser::calculateCpuPercent(
                    p.utime, p.stime, it->second.first, it->second.second,
                    ticks_per_sec, elapsed_sec);
            }
            next_ticks[p.pid] = {p.utime, p.stime};
        }
        prev_ticks = std::move(next_ticks);

        std::sort(processes.begin(), processes.end(),
                  [](const ProcessInfo& a, const ProcessInfo& b) {
                      return a.cpu_percent > b.cpu_percent;
                  });

        std::cout << "\033[2J\033[1;1H";
        std::cout << "=== Linux Process Monitor ===\n\n";
        std::cout << std::left << std::setw(10) << "PID"
                  << std::setw(10) << "CPU(%)"
                  << std::setw(15) << "MEMORY(KB)"
                  << "NAME" << "\n";
        std::cout << "------------------------------------------------------------\n";

        int count = 0;
        for (const auto& p : processes) {
            if (count++ >= kTopN) break;
            std::cout << std::left << std::setw(10) << p.pid
                      << std::setw(10) << std::fixed << std::setprecision(2) << p.cpu_percent
                      << std::setw(15) << p.memory_kb
                      << p.name << "\n";
        }
        std::cout << std::flush;

        std::this_thread::sleep_for(kRefreshInterval);
    }

    std::cout << "\n";
    return 0;
}
