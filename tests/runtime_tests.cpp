#include "runtime.h"
#include <gtest/gtest.h>

TEST(runtime_tests, add_numbers)
{
    OLRuntime::OLRuntime runtime;
    runtime.run("1 + 1");
    ASSERT_EQ(runtime.getLastValue(), 2.0);
}