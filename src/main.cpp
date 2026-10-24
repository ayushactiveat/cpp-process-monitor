#include <algorithm>
#include <iostream>
#include <filesystem>
#include <vector>
#include <iomanip> // For formatting the output nicely
#include "../include/Parser.hpp"

using namespace std;
namespace fs = std::filesystem;

int main() {
    cout << "=== Linux Process Monitor (Modular) ===\n";
    vector<ProcessInfo> processes;

    for (const auto& entry : fs::directory_iterator("/proc")) {
        string pid = entry.path().filename().string();
        if (pid.find_first_not_of("0123456789") == string::npos) {
            string name = ProcParser::readFile("/proc/" + pid + "/comm");
            string stat = ProcParser::readFile("/proc/" + pid + "/stat");

            if (!name.empty() && !stat.empty()) {
                processes.push_back(ProcParser::parseStat(pid, name, stat));
            }
        }
    }

    // 1. THE SORTING LOGIC
    // Sort the vector so the process with the highest memory is first.
    sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        return a.memory_kb > b.memory_kb;
    });

    cout << "Found " << processes.size() << " active processes.\n\n";

    // 2. PRINTING THE TOP 10 PROCESSES
    cout << left << setw(10) << "PID" << setw(15) << "MEMORY(KB)" << "NAME" << "\n";
    cout << "--------------------------------------------------\n";

    // We only print the top 10 to keep the terminal clean
    int count = 0;
    for (const auto& p : processes) {
        if (count >= 10) break; // Stop after 10

        cout << left << setw(10) << p.pid
             << setw(15) << p.memory_kb
             << p.name << "\n";
        count++;
    }

    return 0;
}
