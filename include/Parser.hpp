#pragma once
#include "Process.hpp"

class ProcParser {
public:
    static std::string readFile(const std::string& path);
    static ProcessInfo parseStat(const std::string& pid, const std::string& name, const std::string& statData);
};
