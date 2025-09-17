/*
 * Isolated Unit Tests for Simple sectorA.c Functions
 *
 * This test extracts and copies only the simplest functions from sectorA.c
 * to avoid the massive global dependency issues that prevent full file linking.
 *
 * Functions tested (copied and isolated):
 * - align_xdiff() - Coordinate wrapping mathematics (pure function)
 * - Custom versions of crt_milsum() and crt_sctsum() with minimal dependencies
 *
 * Testing Strategy:
 * - Copy function implementations to isolate from global dependencies
 * - Test core logic without complex world state requirements
 * - Validate that complex functions need integration testing approach
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "unity.h"

/* Minimal type definitions needed for testing */
typedef int maptype;
typedef int ntntype;

#define UNOWNED 0

/* Mock MAPX calculation */
static int mock_mapx = 99; /* This gives MAPX = 100 */
#define MAPX (mock_mapx + 1)

/* Structure definitions extracted from sectorA.c */
typedef struct s_milsum {
    ntntype owner;
    long size;
    struct s_milsum *next;
} MILSUM_STRUCT, *MILSUM_PTR;

typedef struct s_sctsum {
    maptype xloc, yloc;
    ntntype owner;
    MILSUM_STRUCT hold_troops;
    MILSUM_STRUCT siege_troops;
    MILSUM_PTR take_troops;
    MILSUM_PTR other_troops;
    struct s_sctsum *next;
} SCTSUM_STRUCT, *SCTSUM_PTR;

/* Mock error handling */
static int malloc_fail_next = 0;
static int abrt_called = 0;

void abrt(void) {
    abrt_called = 1;
}

/* Mock malloc to test error conditions */
void* test_malloc(size_t size) {
    if (malloc_fail_next) {
        malloc_fail_next = 0;
        return NULL;
    }
    return malloc(size);
}

/*
 * EXTRACTED FUNCTION 1: align_xdiff (copied from sectorA.c:688-699)
 * This is a pure function with no global dependencies
 */
int align_xdiff(int xd_val)
{
    /* check for edge wrapping */
    if (abs(xd_val) > MAPX - abs(xd_val)) {
        if (xd_val <= 0) {
            xd_val = MAPX - abs(xd_val);
        } else {
            xd_val = abs(xd_val) - MAPX;
        }
    }
    return(xd_val);
}

/*
 * EXTRACTED FUNCTION 2: crt_milsum (simplified from sectorA.c:69-84)
 * Removed fupdate dependency for isolated testing
 */
MILSUM_PTR crt_milsum(int who)
{
    MILSUM_PTR MS_ptr;

    /* get the memory */
    if ((MS_ptr = (MILSUM_PTR) test_malloc(sizeof(MILSUM_STRUCT))) == NULL) {
        abrt();
        return NULL;
    }

    /* now initialize values */
    MS_ptr->owner = who;
    MS_ptr->size = 0;
    MS_ptr->next = NULL;
    return(MS_ptr);
}

/*
 * EXTRACTED FUNCTION 3: crt_sctsum (simplified from sectorA.c:114-138)
 * Removed fupdate dependency for isolated testing
 */
SCTSUM_PTR crt_sctsum(int x, int y)
{
    SCTSUM_PTR SS_ptr;

    /* get the memory */
    if ((SS_ptr = (SCTSUM_PTR) test_malloc(sizeof(SCTSUM_STRUCT))) == NULL) {
        abrt();
        return NULL;
    }

    /* now initialize values */
    SS_ptr->xloc = x;
    SS_ptr->yloc = y;
    SS_ptr->owner = UNOWNED;
    SS_ptr->siege_troops.owner = UNOWNED;
    SS_ptr->siege_troops.size = 0L;
    SS_ptr->siege_troops.next = NULL;
    SS_ptr->hold_troops.owner = UNOWNED;
    SS_ptr->hold_troops.size = 0L;
    SS_ptr->hold_troops.next = NULL;
    SS_ptr->take_troops = NULL;
    SS_ptr->other_troops = NULL;
    SS_ptr->next = NULL;
    return(SS_ptr);
}

/* Unity test framework setup */
void setUp(void) {
    /* Reset test state before each test */
    abrt_called = 0;
    malloc_fail_next = 0;
    mock_mapx = 99; /* Default map size for testing */
}

void tearDown(void) {
    /* No cleanup needed for isolated tests */
}

/*
 * Test Suite 1: align_xdiff() - Coordinate wrapping mathematics
 */

void test_align_xdiff_no_wrapping_needed(void) {
    mock_mapx = 99; /* MAPX = 100 */

    TEST_ASSERT_EQUAL_INT(5, align_xdiff(5));
    TEST_ASSERT_EQUAL_INT(-10, align_xdiff(-10));
    TEST_ASSERT_EQUAL_INT(30, align_xdiff(30));
    TEST_ASSERT_EQUAL_INT(-45, align_xdiff(-45));
}

void test_align_xdiff_positive_wrapping(void) {
    mock_mapx = 99; /* MAPX = 100 */

    /* Large positive: 80 > (100-80=20), so wrap to -20 */
    TEST_ASSERT_EQUAL_INT(-20, align_xdiff(80));

    /* Edge case: 51 > (100-51=49), so wrap to -49 */
    TEST_ASSERT_EQUAL_INT(-49, align_xdiff(51));
}

void test_align_xdiff_negative_wrapping(void) {
    mock_mapx = 99; /* MAPX = 100 */

    /* Large negative: |-75| > (100-75=25), so wrap to +25 */
    TEST_ASSERT_EQUAL_INT(25, align_xdiff(-75));

    /* Edge case: |-50| = (100-50=50), no wrap */
    TEST_ASSERT_EQUAL_INT(-50, align_xdiff(-50));
}

void test_align_xdiff_different_map_sizes(void) {
    /* Test with different map sizes */
    mock_mapx = 49; /* MAPX = 50 */

    TEST_ASSERT_EQUAL_INT(10, align_xdiff(10));
    TEST_ASSERT_EQUAL_INT(-15, align_xdiff(35)); /* 35 > (50-35=15), wrap to -15 */
    TEST_ASSERT_EQUAL_INT(20, align_xdiff(-30)); /* |-30| > (50-30=20), wrap to +20 */
}

/*
 * Test Suite 2: crt_milsum() - Memory allocation with error handling
 */

void test_crt_milsum_successful_allocation(void) {
    MILSUM_PTR result = crt_milsum(5);

    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(5, result->owner);
    TEST_ASSERT_EQUAL_INT(0, result->size);
    TEST_ASSERT_NULL(result->next);
    TEST_ASSERT_EQUAL_INT(0, abrt_called);

    free(result);
}

void test_crt_milsum_memory_allocation_failure(void) {
    /* Force malloc to fail */
    malloc_fail_next = 1;

    MILSUM_PTR result = crt_milsum(10);

    TEST_ASSERT_NULL(result);
    TEST_ASSERT_EQUAL_INT(1, abrt_called);
}

void test_crt_milsum_different_owners(void) {
    MILSUM_PTR result1 = crt_milsum(0);
    MILSUM_PTR result2 = crt_milsum(-1);
    MILSUM_PTR result3 = crt_milsum(999);

    TEST_ASSERT_EQUAL_INT(0, result1->owner);
    TEST_ASSERT_EQUAL_INT(-1, result2->owner);
    TEST_ASSERT_EQUAL_INT(999, result3->owner);

    /* All should have same initialization pattern */
    TEST_ASSERT_EQUAL_INT(0, result1->size);
    TEST_ASSERT_EQUAL_INT(0, result2->size);
    TEST_ASSERT_EQUAL_INT(0, result3->size);

    free(result1);
    free(result2);
    free(result3);
}

/*
 * Test Suite 3: crt_sctsum() - Structure allocation and initialization
 */

void test_crt_sctsum_successful_allocation(void) {
    SCTSUM_PTR result = crt_sctsum(25, 30);

    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(25, result->xloc);
    TEST_ASSERT_EQUAL_INT(30, result->yloc);
    TEST_ASSERT_EQUAL_INT(UNOWNED, result->owner);
    TEST_ASSERT_EQUAL_INT(0, abrt_called);

    /* Verify embedded structure initialization */
    TEST_ASSERT_EQUAL_INT(UNOWNED, result->siege_troops.owner);
    TEST_ASSERT_EQUAL_INT(0, result->siege_troops.size);
    TEST_ASSERT_NULL(result->siege_troops.next);

    TEST_ASSERT_EQUAL_INT(UNOWNED, result->hold_troops.owner);
    TEST_ASSERT_EQUAL_INT(0, result->hold_troops.size);
    TEST_ASSERT_NULL(result->hold_troops.next);

    TEST_ASSERT_NULL(result->take_troops);
    TEST_ASSERT_NULL(result->other_troops);
    TEST_ASSERT_NULL(result->next);

    free(result);
}

void test_crt_sctsum_memory_allocation_failure(void) {
    /* Force malloc to fail */
    malloc_fail_next = 1;

    SCTSUM_PTR result = crt_sctsum(10, 20);

    TEST_ASSERT_NULL(result);
    TEST_ASSERT_EQUAL_INT(1, abrt_called);
}

void test_crt_sctsum_coordinate_range(void) {
    SCTSUM_PTR result1 = crt_sctsum(0, 0);
    SCTSUM_PTR result2 = crt_sctsum(-5, 999);
    SCTSUM_PTR result3 = crt_sctsum(100, -10);

    TEST_ASSERT_EQUAL_INT(0, result1->xloc);
    TEST_ASSERT_EQUAL_INT(0, result1->yloc);
    TEST_ASSERT_EQUAL_INT(-5, result2->xloc);
    TEST_ASSERT_EQUAL_INT(999, result2->yloc);
    TEST_ASSERT_EQUAL_INT(100, result3->xloc);
    TEST_ASSERT_EQUAL_INT(-10, result3->yloc);

    free(result1);
    free(result2);
    free(result3);
}

/*
 * Test runner function
 */
int run_tests(void) {
    UNITY_BEGIN();

    /* align_xdiff() tests */
    RUN_TEST(test_align_xdiff_no_wrapping_needed);
    RUN_TEST(test_align_xdiff_positive_wrapping);
    RUN_TEST(test_align_xdiff_negative_wrapping);
    RUN_TEST(test_align_xdiff_different_map_sizes);

    /* crt_milsum() tests */
    RUN_TEST(test_crt_milsum_successful_allocation);
    RUN_TEST(test_crt_milsum_memory_allocation_failure);
    RUN_TEST(test_crt_milsum_different_owners);

    /* crt_sctsum() tests */
    RUN_TEST(test_crt_sctsum_successful_allocation);
    RUN_TEST(test_crt_sctsum_memory_allocation_failure);
    RUN_TEST(test_crt_sctsum_coordinate_range);

    return UNITY_END();
}

/*
 * Main function with correct signature
 */
int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    return run_tests();
}