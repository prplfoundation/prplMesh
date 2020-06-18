// Main.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    int rc = RUN_ALL_TESTS();

    return rc;
}
