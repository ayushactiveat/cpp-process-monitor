#include <gtest/gtest.h>
#include "../include/Parser.hpp"

TEST(ParserTest, ParsesFakeStatData) {
    std::string fakeStat = "1234 (bash) S 1 1234 1234 0 -1 4194560 500 0 0 0 10 20 0 0 20 0 1 0 5000 10000 100";
    ProcessInfo p = ProcParser::parseStat("1234", "bash", fakeStat);
    EXPECT_EQ(p.pid, "1234");
    EXPECT_EQ(p.name, "bash");
    EXPECT_GT(p.memory_kb, 0);
}
