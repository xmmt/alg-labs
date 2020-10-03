#include "pch.h"

TEST(TestCaseName, TestName) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

TEST(Assertion, LastFirst) {
    int a[] = { 3, 2, 1 };
    ASSERT_DEBUG_DEATH(QuickSort::sort(a + 2, a + 1), "");
}