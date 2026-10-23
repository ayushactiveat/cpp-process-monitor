#include "../include/Parser.hpp"
#include <fstream>
#include <sstream>

std::string ProcParser::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return ""; 
    std::string content;
    std::getline(file, content);
    return content;
}

ProcessInfo ProcParser::parseStat(const std::string& pid, const std::string& name, const std::string& statData) {
    ProcessInfo p;
    p.pid = pid;
    p.name = name;

    std::stringstream ss(statData);
    std::string temp;

    for (int i = 0; i < 13; i++) ss >> temp;

    ss >> p.utime >> p.stime;

    for (int i = 0; i < 6; i++) ss >> temp;

    ss >> p.memory_kb;

    return p;
}

