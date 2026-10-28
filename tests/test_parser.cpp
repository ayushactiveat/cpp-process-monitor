#include <gtest/gtest.h>
#include "../include/Parser.hpp"

using namespace std;

TEST(ParserTest, ParsesMockedProcDataWithSpaces) {
    // Mocked /proc/[pid]/stat line with spaces in the process name
    string mocked_proc_data = "1234 (Web Content) S 1 1234 1234 0 -1 4194560 500 0 0 0 10 20 0 0 20 0 1 0 5000 10000 100 0 0";
    ProcessInfo p = ProcParser::parseStat("1234", "Web Content", mocked_proc_data);
    
    EXPECT_EQ(p.pid, "1234");
    EXPECT_EQ(p.name, "Web Content");
    EXPECT_EQ(p.utime, 10);
    EXPECT_EQ(p.stime, 20);
    
    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    EXPECT_EQ(p.memory_kb, 100 * page_size_kb);
}

TEST(ParserTest, CalculatesCpuPercentCorrectly) {
    double cpu = ProcParser::calculateCpuPercent(150, 50, 100, 0, 100, 1.0);
    EXPECT_DOUBLE_EQ(cpu, 100.0);
    
    cpu = ProcParser::calculateCpuPercent(120, 30, 100, 0, 100, 1.0);
    EXPECT_DOUBLE_EQ(cpu, 50.0);
}
