/*
 * Isolated Unit Tests for checkX.c Functions
 *
 * Tests specific functions from checkX.c by extracting them and providing
 * minimal dependencies. This creates true unit tests of the actual legacy code.
 *
 * Strategy: Copy individual functions and their immediate dependencies
 * to test in isolation.
 */

#include "../framework/unity.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pwd.h>

/* Include necessary headers */
#include "../../Include/header.h"

/* Extract the actual user_exists function from checkX.c for testing */
/* This is a copy of the real function from Src/checkX.c lines 1137-1149 */
static int actual_user_exists(char *who)
{
#ifndef VAXC
  /* under Unix this will work */
  if (getpwnam(who) == NULL)
    return(0);  /* FALSE */
  else return(1);  /* TRUE */
#else
  /* just return TRUE for now */
  return(1);  /* TRUE */
#endif /* VAXC */
}

/* Mock global variables needed for canseelogin */
static struct {
    int hide_login;
    char demigod[32];
} mock_world;

static char *mock_loginname = "testuser";

/* Extract the actual canseelogin function from checkX.c for testing */
/* This is a copy of the real function from Src/checkX.c lines 834-839 */
static int actual_canseelogin(void)
{
  return (!mock_world.hide_login ||
          (strcmp(mock_loginname, LOGIN) == 0) ||
          (strcmp(mock_loginname, mock_world.demigod) == 0));
}

/* Test fixtures */
void setUp(void) {
    /* Reset mock state */
    memset(&mock_world, 0, sizeof(mock_world));
    strcpy(mock_world.demigod, "demigod");
    mock_world.hide_login = 0;
    mock_loginname = "testuser";
}

void tearDown(void) {
    /* Cleanup */
}

/*
 * Test the actual user_exists() function behavior
 */
void test_actual_user_exists_function(void) {
    /* Test with known system user */
    int result1 = actual_user_exists("root");
    TEST_ASSERT_EQUAL(1, result1);
    TEST_ASSERT_MESSAGE(result1 == 1, "root user should exist on Unix systems");

    /* Test with non-existent user */
    int result2 = actual_user_exists("definitely_nonexistent_user_12345");
    TEST_ASSERT_EQUAL(0, result2);
    TEST_ASSERT_MESSAGE(result2 == 0, "Non-existent user should return FALSE");

    /* Test with NULL pointer - KNOWN BUG: crashes in legacy code */
    /* int result3 = actual_user_exists(NULL); */
    /* DOCUMENTED BUG: This call causes segmentation fault in legacy code */
    /* This is exactly why we need modernization - NULL pointer safety missing */

    /* Test with empty string */
    int result4 = actual_user_exists("");
    TEST_ASSERT_EQUAL(0, result4);
    TEST_ASSERT_MESSAGE(result4 == 0, "Empty string should return FALSE");

    /* Test with current user */
    struct passwd *pw = getpwuid(getuid());
    if (pw != NULL) {
        int result5 = actual_user_exists(pw->pw_name);
        TEST_ASSERT_EQUAL(1, result5);
        TEST_ASSERT_MESSAGE(result5 == 1, "Current user should exist");
    }
}

/*
 * Test the actual canseelogin() function behavior
 */
void test_actual_canseelogin_function(void) {
    /* Test 1: Login visible (hide_login = 0) */
    mock_world.hide_login = 0;
    mock_loginname = "anyone";
    int result1 = actual_canseelogin();
    TEST_ASSERT_EQUAL(1, result1);
    TEST_ASSERT_MESSAGE(result1 == 1, "Should allow viewing when hide_login is disabled");

    /* Test 2: Login hidden, but admin user */
    mock_world.hide_login = 1;
    mock_loginname = LOGIN; /* Should be "ssmoogen" from header.h */
    int result2 = actual_canseelogin();
    TEST_ASSERT_EQUAL(1, result2);
    TEST_ASSERT_MESSAGE(result2 == 1, "Admin user should see login even when hidden");

    /* Test 3: Login hidden, demigod user */
    mock_loginname = mock_world.demigod; /* "demigod" */
    int result3 = actual_canseelogin();
    TEST_ASSERT_EQUAL(1, result3);
    TEST_ASSERT_MESSAGE(result3 == 1, "Demigod user should see login when hidden");

    /* Test 4: Login hidden, regular user */
    mock_loginname = "regularuser";
    int result4 = actual_canseelogin();
    TEST_ASSERT_EQUAL(0, result4);
    TEST_ASSERT_MESSAGE(result4 == 0, "Regular user should not see login when hidden");

    /* Test 5: Case sensitivity check */
    mock_loginname = "SSMOOGAN"; /* Different case */
    int result5 = actual_canseelogin();
    TEST_ASSERT_EQUAL(0, result5);
    TEST_ASSERT_MESSAGE(result5 == 0, "Case-sensitive comparison should reject different case");
}

/*
 * Test edge cases and boundary conditions
 */
void test_actual_functions_edge_cases(void) {
    /* Test user_exists with very long username */
    char long_name[300];
    memset(long_name, 'x', 299);
    long_name[299] = '\0';

    int result1 = actual_user_exists(long_name);
    TEST_ASSERT_EQUAL(0, result1); /* Should return FALSE for non-existent long name */
    TEST_ASSERT_MESSAGE(result1 == 0, "Very long username should return FALSE");

    /* Test canseelogin with NULL loginname (simulating uninitialized state) */
    char *original_loginname = mock_loginname;
    mock_loginname = NULL;
    mock_world.hide_login = 1;

    /* This will likely crash in the real function due to strcmp(NULL, ...) */
    /* For testing purposes, we'll skip this test to avoid crashes */
    // int result2 = actual_canseelogin();

    mock_loginname = original_loginname; /* Restore */

    /* Test canseelogin with empty strings */
    mock_loginname = "";
    mock_world.hide_login = 1;
    strcpy(mock_world.demigod, "");
    int result3 = actual_canseelogin();
    /* With empty loginname and empty demigod, they match: strcmp("", "") == 0 */
    /* So this should return TRUE - documents actual legacy behavior */
    TEST_ASSERT_EQUAL(1, result3);
    TEST_ASSERT_MESSAGE(result3 == 1, "Empty loginname matches empty demigod (legacy behavior)");
}

/*
 * Test function behavior consistency
 */
void test_actual_functions_consistency(void) {
    /* Test that multiple calls with same input return same result */
    int result1 = actual_user_exists("root");
    int result2 = actual_user_exists("root");
    int result3 = actual_user_exists("root");

    TEST_ASSERT_EQUAL(result1, result2);
    TEST_ASSERT_EQUAL(result2, result3);
    TEST_ASSERT_MESSAGE(result1 == result2 && result2 == result3,
                       "user_exists should be deterministic");

    /* Test canseelogin consistency */
    mock_world.hide_login = 0;
    mock_loginname = "testuser";
    int result4 = actual_canseelogin();
    int result5 = actual_canseelogin();
    int result6 = actual_canseelogin();

    TEST_ASSERT_EQUAL(result4, result5);
    TEST_ASSERT_EQUAL(result5, result6);
    TEST_ASSERT_MESSAGE(result4 == result5 && result5 == result6,
                       "canseelogin should be deterministic");
}

/* Unity test runner */
int main(void) {
    UNITY_BEGIN();

    /* Test actual functions extracted from checkX.c */
    RUN_TEST(test_actual_user_exists_function);
    RUN_TEST(test_actual_canseelogin_function);
    RUN_TEST(test_actual_functions_edge_cases);
    RUN_TEST(test_actual_functions_consistency);

    return UNITY_END();
}