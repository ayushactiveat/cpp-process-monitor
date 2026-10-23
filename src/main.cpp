#include <iostream>
#include <filesystem>
#include <vector>
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
    cout << "Found " << processes.size() << " active processes.\n";
    return 0;
}
