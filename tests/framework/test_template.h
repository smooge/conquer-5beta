/*
 * Test Template for Conquer C2023 Modernization
 * 
 * Standard template for creating consistent tests across the modernization project
 */

#ifndef TEST_TEMPLATE_H
#define TEST_TEMPLATE_H

#include "unity.h"
#include "unity_config.h"

/* Standard test setup and teardown */
void setUp(void);
void tearDown(void);

/* Test helper macros for common patterns */
#define TEST_ASSERT_NOT_NULL_MSG(ptr, msg) \
    TEST_ASSERT_NOT_NULL_MESSAGE(ptr, msg)

#define TEST_ASSERT_VALID_POINTER(ptr) \
    TEST_ASSERT_NOT_NULL_MESSAGE(ptr, "Pointer should not be NULL")

#define TEST_ASSERT_SAFE_STRING(str, max_len) \
    do { \
        TEST_ASSERT_NOT_NULL_MESSAGE(str, "String should not be NULL"); \
        TEST_ASSERT_TRUE_MESSAGE((int)strlen(str) < (int)max_len, "String exceeds maximum length"); \
    } while(0)

/* Buffer overflow testing helpers */
#define TEST_BUFFER_SIZE 256
#define TEST_SMALL_BUFFER_SIZE 16

/* Memory testing helpers for security validation */
#define TEST_ASSERT_NO_MEMORY_LEAK(before, after) \
    TEST_ASSERT_EQUAL_MESSAGE(before, after, "Memory leak detected")

/* Test categories for organizing tests */
#define TEST_CATEGORY_UNIT "unit"
#define TEST_CATEGORY_INTEGRATION "integration"
#define TEST_CATEGORY_REGRESSION "regression"
#define TEST_CATEGORY_SECURITY "security"

#endif /* TEST_TEMPLATE_H */