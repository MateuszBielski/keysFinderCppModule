#include <gtest/gtest.h>
#include "TeethGenerator.h"

TEST(TeethGenerator,Success)
{
    ASSERT_TRUE(true);
}

TEST(TeethGenerator,SizeOfGeneratedSet)
{
    TeethGenerator tg;
    tg.SizeOfGenerated(5);
    auto generatedSet = tg.Generate();
    ASSERT_EQ(5,generatedSet.size());
}