#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>

#include "Parser.hpp"

namespace {

long pageSizeKb() { return sysconf(_SC_PAGESIZE) / 1024; }

// Fields after the name: state ppid pgrp session tty tpgid flags minflt
// cminflt majflt cmajflt utime(10) stime(20) ... vsize(10000) rss(100 pages)
const std::string kTail =
    " S 1 1234 1234 0 -1 4194560 500 0 0 0 10 20 0 0 20 0 1 0 5000 10000 100 0 0";

}  // namespace

// ---- parseStat ----

TEST(ParseStat, ParsesNormalLine) {
    ProcessInfo p = ProcParser::parseStat("1234", "bash", "1234 (bash)" + kTail);
    EXPECT_EQ(p.pid, "1234");
    EXPECT_EQ(p.name, "bash");
    EXPECT_EQ(p.utime, 10);
    EXPECT_EQ(p.stime, 20);
    EXPECT_EQ(p.memory_kb, 100 * pageSizeKb());
}

TEST(ParseStat, HandlesNameWithSpaces) {
    ProcessInfo p = ProcParser::parseStat("1234", "Web Content", "1234 (Web Content)" + kTail);
    EXPECT_EQ(p.utime, 10);
    EXPECT_EQ(p.stime, 20);
    EXPECT_EQ(p.memory_kb, 100 * pageSizeKb());
}

TEST(ParseStat, HandlesParenthesisInsideName) {
    ProcessInfo p = ProcParser::parseStat("1234", "a) b", "1234 (a) b)" + kTail);
    EXPECT_EQ(p.utime, 10);
    EXPECT_EQ(p.stime, 20);
}

TEST(ParseStat, LineEndingAtParenDoesNotThrow) {
    // Truncated read: the line stops right after the name
    ProcessInfo p;
    EXPECT_NO_THROW(p = ProcParser::parseStat("1", "x", "1 (x)"));
    EXPECT_EQ(p.utime, 0);
    EXPECT_EQ(p.memory_kb, 0);
}

TEST(ParseStat, ShortLineLeavesNumbersAtZero) {
    ProcessInfo p = ProcParser::parseStat("1", "x", "1 (x) S 1 1");
    EXPECT_EQ(p.pid, "1");
    EXPECT_EQ(p.name, "x");
    EXPECT_EQ(p.utime, 0);
    EXPECT_EQ(p.stime, 0);
    EXPECT_EQ(p.memory_kb, 0);
}

TEST(ParseStat, MissingParenOrEmptyLineLeavesNumbersAtZero) {
    ProcessInfo a = ProcParser::parseStat("1", "x", "no parentheses here");
    ProcessInfo b = ProcParser::parseStat("2", "y", "");
    EXPECT_EQ(a.utime, 0);
    EXPECT_EQ(a.memory_kb, 0);
    EXPECT_EQ(b.utime, 0);
    EXPECT_EQ(b.memory_kb, 0);
}

// ---- calculateCpuPercent ----

TEST(CpuPercent, OneFullCore) {
    // 100 ticks in 1 second at 100 ticks/sec
    EXPECT_DOUBLE_EQ(ProcParser::calculateCpuPercent(150, 50, 100, 0, 100, 1.0), 100.0);
}

TEST(CpuPercent, HalfACore) {
    EXPECT_DOUBLE_EQ(ProcParser::calculateCpuPercent(120, 30, 100, 0, 100, 1.0), 50.0);
}

TEST(CpuPercent, UsesRealElapsedTime) {
    // Same ticks over 2 seconds means half the usage
    EXPECT_DOUBLE_EQ(ProcParser::calculateCpuPercent(150, 50, 100, 0, 100, 2.0), 50.0);
}

TEST(CpuPercent, CanExceed100ForMultipleCores) {
    EXPECT_DOUBLE_EQ(ProcParser::calculateCpuPercent(200, 100, 100, 0, 100, 1.0), 200.0);
}

TEST(CpuPercent, NoChangeIsZero) {
    EXPECT_DOUBLE_EQ(ProcParser::calculateCpuPercent(100, 50, 100, 50, 100, 1.0), 0.0);
}

TEST(CpuPercent, BackwardsCounterIsClampedToZero) {
    EXPECT_DOUBLE_EQ(ProcParser::calculateCpuPercent(10, 10, 100, 100, 100, 1.0), 0.0);
}

TEST(CpuPercent, ZeroElapsedTimeIsZero) {
    EXPECT_DOUBLE_EQ(ProcParser::calculateCpuPercent(200, 0, 100, 0, 100, 0.0), 0.0);
}

// ---- readFile ----

TEST(ReadFile, MissingFileReturnsEmpty) {
    EXPECT_EQ(ProcParser::readFile("/proc/99999999/stat"), "");
}

TEST(ReadFile, ReturnsFirstLineOnly) {
    auto path = std::filesystem::temp_directory_path() / "procmon_test_first_line.txt";
    {
        std::ofstream out(path);
        out << "first line\nsecond line\n";
    }
    EXPECT_EQ(ProcParser::readFile(path.string()), "first line");
    std::filesystem::remove(path);
}

TEST(ReadFile, EmptyFileReturnsEmpty) {
    auto path = std::filesystem::temp_directory_path() / "procmon_test_empty.txt";
    { std::ofstream out(path); }
    EXPECT_EQ(ProcParser::readFile(path.string()), "");
    std::filesystem::remove(path);
}
