#include "TimeTools.h"

#include "gtest/gtest.h"

using namespace u::reactor;

namespace {

TEST(TimeToolsTest, getElapsedTime_ShouldSucceed_WithSeconds)
{
    struct timespec start {
        0, 0
    };
    struct timespec end {
        1, 0
    };

    ASSERT_EQ(1000, TimeTools::getElapsedTime(&start, &end));
}

TEST(TimeToolsTest, getElapsedTime_ShouldSucceed_WithNanoseconds)
{
    struct timespec start {
        0, 0
    };
    struct timespec end {
        1, 1000000
    };

    ASSERT_EQ(1001, TimeTools::getElapsedTime(&start, &end));
}

TEST(TimeToolsTest, getElapsedTime_ShouldSucceed_WithStartNSecGTEndNSec)
{
    struct timespec start {
        0, 1000000
    };
    struct timespec end {
        1, 0
    };

    ASSERT_EQ(999, TimeTools::getElapsedTime(&start, &end));
}

} // namespace
