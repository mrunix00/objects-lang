#include "runtime.h"
#include <gtest/gtest.h>

TEST(runtime_tests, add_numbers)
{
    OLRuntime::OLRuntime runtime;
    runtime.run("1 + 1");
    ASSERT_EQ(runtime.getLastValue(), 2.0);
}

TEST(runtime_tests, subtract_numbers)
{
    OLRuntime::OLRuntime runtime;
    runtime.run("5 - 1");
    ASSERT_EQ(runtime.getLastValue(), 4.0);
}

TEST(runtime_tests, add_and_subtract_numbers)
{
    OLRuntime::OLRuntime runtime;
    runtime.run("1 + 1 - 1");
    ASSERT_EQ(runtime.getLastValue(), 1.0);
}

TEST(runtime_tests, multiply_numbers)
{
    OLRuntime::OLRuntime runtime;
    runtime.run("2 * 2");
    ASSERT_EQ(runtime.getLastValue(), 4.0);
}

TEST(runtime_tests, divide_numbers)
{
    OLRuntime::OLRuntime runtime;
    runtime.run("4 / 2");
    ASSERT_EQ(runtime.getLastValue(), 2.0);
}
