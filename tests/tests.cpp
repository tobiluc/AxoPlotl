
#include "gtest/gtest.h"

#include <AxoPlotl/parsing/parsing.h>

namespace AxoPlotl
{

TEST(HelloWorldTest, OnePlusOneIsTwo)
{
    ASSERT_EQ(1+1, 2);
}

TEST(HelloWorldTest, ds)
{
    // auto text = "a = 1 + 1";
    // auto tokens = Parsing::tokenize(text);
    // auto root = Parsing::Parser(tokens).parse();
    // ASSERT_EQ(1+1, 2);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
}
