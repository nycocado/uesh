#include "common.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void test_die_exists(void) { TEST_ASSERT_NOT_NULL(die); }

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_die_exists);
    return UNITY_END();
}
