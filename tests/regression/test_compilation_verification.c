/*
 * Compilation Verification Regression Tests for Conquer C2023 Modernization
 *
 * This test suite verifies that all source files compile successfully with
 * C2023 standards after Phase 7A.1 header modernization. This serves as a
 * critical regression test to ensure our modernization foundation is solid.
 *
 * SUCCESS CRITERIA:
 * - All 64 source files must compile without errors using C2023 standards
 * - Compilation must use strict flags: -std=c2x -Wall -Wextra -Wpedantic
 * - POSIX compliance maintained: -D_POSIX_C_SOURCE=200809L
 */

#include "../framework/unity.h"
#include "../framework/test_template.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>

/* Test configuration */
#define MAX_COMMAND_LENGTH 512
#define MAX_OUTPUT_LENGTH 2048
#define COMPILATION_TIMEOUT 30

/* Compilation flags used for C2023 compliance testing */
static const char* COMPILE_FLAGS =
    "gcc -std=c2x -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -g -O2 -c";


/* Global variables for dynamic paths */
static char PROJECT_ROOT[512];
static char SRC_PATH[512];
static char INCLUDE_PATH[512];

/*
 * Find the project root directory by looking for CMakeLists.txt
 * This function searches upward from the current directory until it finds
 * the project root containing CMakeLists.txt
 */
static int find_project_root(void) {
    char cwd[512];
    char test_path[512];
    struct stat st;

    /* Get current working directory */
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("ERROR: Cannot get current working directory\n");
        return -1;
    }

    printf("DEBUG: Current working directory: %s\n", cwd);

    /* Try current directory first */
    snprintf(test_path, sizeof(test_path), "%s/CMakeLists.txt", cwd);
    if (stat(test_path, &st) == 0) {
        strcpy(PROJECT_ROOT, cwd);
        printf("DEBUG: Found project root: %s\n", PROJECT_ROOT);
        return 0;
    }

    /* Try parent directory */
    snprintf(test_path, sizeof(test_path), "%s/../CMakeLists.txt", cwd);
    if (stat(test_path, &st) == 0) {
        snprintf(PROJECT_ROOT, sizeof(PROJECT_ROOT), "%s/..", cwd);
        printf("DEBUG: Found project root: %s\n", PROJECT_ROOT);
        return 0;
    }

    /* Try grandparent directory */
    snprintf(test_path, sizeof(test_path), "%s/../../CMakeLists.txt", cwd);
    if (stat(test_path, &st) == 0) {
        snprintf(PROJECT_ROOT, sizeof(PROJECT_ROOT), "%s/../..", cwd);
        printf("DEBUG: Found project root: %s\n", PROJECT_ROOT);
        return 0;
    }

    /* Try great-grandparent directory */
    snprintf(test_path, sizeof(test_path), "%s/../../../CMakeLists.txt", cwd);
    if (stat(test_path, &st) == 0) {
        snprintf(PROJECT_ROOT, sizeof(PROJECT_ROOT), "%s/../../..", cwd);
        printf("DEBUG: Found project root: %s\n", PROJECT_ROOT);
        return 0;
    }

    printf("ERROR: Cannot find project root (CMakeLists.txt not found)\n");
    return -1;
}

/*
 * Initialize paths based on detected project root
 */
static void initialize_paths(void) {
    if (find_project_root() != 0) {
        /* Fallback to relative paths if detection fails */
        strcpy(SRC_PATH, "../Src");
        strcpy(INCLUDE_PATH, "-I../Include");
        printf("WARNING: Using fallback paths\n");
    } else {
        /* Build paths relative to project root */
        snprintf(SRC_PATH, sizeof(SRC_PATH), "%s/Src", PROJECT_ROOT);
        snprintf(INCLUDE_PATH, sizeof(INCLUDE_PATH), "-I%s/Include", PROJECT_ROOT);
        printf("DEBUG: SRC_PATH: %s\n", SRC_PATH);
        printf("DEBUG: INCLUDE_PATH: %s\n", INCLUDE_PATH);
    }
}

void setUp(void) {
    /* Setup for each test */
}

void tearDown(void) {
    /* Cleanup after each test - remove any .o files created during testing */
    system("rm -f *.o");
}

/*
 * Helper function to test compilation of a single source file
 * Returns 0 on success, non-zero on compilation failure, -1 if file doesn't exist
 */
static int test_file_compilation(const char* source_file) {
    char command[MAX_COMMAND_LENGTH];
    char full_path[MAX_COMMAND_LENGTH];
    int result;
    FILE *check_file;

    /* Build full path to source file */
    snprintf(full_path, sizeof(full_path), "%s/%s", SRC_PATH, source_file);

    /* Check if file exists first */
    check_file = fopen(full_path, "r");
    if (check_file == NULL) {
        printf("SKIP: File %s does not exist\n", full_path);
        return -1; /* Indicate file doesn't exist */
    }
    fclose(check_file);

    /* Build compilation command */
    snprintf(command, sizeof(command), "%s %s %s",
             COMPILE_FLAGS, INCLUDE_PATH, full_path);

    /* Execute compilation test */
    result = system(command);

    /* Return success (0) or failure (non-zero) */
    return WEXITSTATUS(result);
}

/*
 * Test Core Game Engine Files (Priority 1)
 * These are the most critical files for game functionality
 */
void test_core_engine_compilation(void) {
    int result;

    result = test_file_compilation("mainA.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "mainA.c should compile successfully");
    }

    result = test_file_compilation("dataA.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "dataA.c should compile successfully");
    }

    result = test_file_compilation("combatA.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "combatA.c should compile successfully");
    }

    result = test_file_compilation("moveA.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "moveA.c should compile successfully");
    }
}

/*
 * Test I/O and Data Management Files (Priority 2)
 * Critical for data integrity and file operations
 */
void test_io_data_compilation(void) {
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("ioX.c"),
                              "ioX.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("iodataX.c"),
                              "iodataX.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("memoryX.c"),
                              "memoryX.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("dataX.c"),
                              "dataX.c should compile successfully");
}

/*
 * Test User Interface Files (Priority 3)
 * Essential for player interaction
 */
void test_interface_compilation(void) {
    int result;

    result = test_file_compilation("mainG.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "mainG.c should compile successfully");
    }

    result = test_file_compilation("displayG.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "displayG.c should compile successfully");
    }

    result = test_file_compilation("infoG.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "infoG.c should compile successfully");
    }

    result = test_file_compilation("inputG.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "inputG.c should compile successfully");
    }
}

/*
 * Test Game Content Files (Priority 4)
 * Game features and content systems
 */
void test_game_content_compilation(void) {
    int result;

    result = test_file_compilation("magicA.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "magicA.c should compile successfully");
    }

    result = test_file_compilation("monsterA.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "monsterA.c should compile successfully");
    }

    result = test_file_compilation("npcA.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "npcA.c should compile successfully");
    }

    result = test_file_compilation("clanA.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "clanA.c should compile successfully");
    }
}

/*
 * Test Utility and Support Files (Priority 5)
 * Support functions and utilities
 */
void test_utility_compilation(void) {
    int result;

    result = test_file_compilation("miscA.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "miscA.c should compile successfully");
    }

    result = test_file_compilation("miscX.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "miscX.c should compile successfully");
    }

    result = test_file_compilation("miscG.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "miscG.c should compile successfully");
    }

    result = test_file_compilation("utilityX.c");
    if (result != -1) {
        TEST_ASSERT_EQUAL_MESSAGE(0, result, "utilityX.c should compile successfully");
    }
}

/*
 * Test Additional Critical Files
 * Other important system components
 */
void test_additional_files_compilation(void) {
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("jointA.c"),
                              "jointA.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("jointG.c"),
                              "jointG.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("createA.c"),
                              "createA.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("configA.c"),
                              "configA.c should compile successfully");
}

/*
 * Comprehensive compilation test for all remaining source files
 * This validates our complete Phase 7A.1 success
 */
void test_remaining_files_compilation(void) {
    /* Array of remaining source files to test */
    const char* remaining_files[] = {
	    "adduserA.c", "armyG.c", "caravanG.c", "checkX.c", "combatA.c",
	    "computeX.c", "configA.c", "convertX.c", "createA.c", "customG.c",
	    "customX.c", "dataA.c", "dataG.c", "dataX.c", "datamagX.c",
	    "datamilX.c", "displayG.c", "economyA.c", "emailG.c", "enlistG.c",
	    "executeX.c", "hexmapG.c", "hexmapX.c", "ieditG.c", "infoG.c",
	    "ioG.c", "ioX.c", "iodataG.c", "iodataX.c", "jointA.c", "jointG.c",
	    "keybindG.c", "magicA.c", "magicG.c", "magicX.c", "mailA.c",
	    "mailG.c", "mailX.c", "mainA.c", "mainG.c", "memoryX.c", "miscA.c",
	    "miscG.c", "miscX.c", "monsterA.c", "moveA.c", "moveG.c", "moveX.c",
	    "navyG.c", "npcA.c", "ntninfoG.c", "pagerG.c", "regionG.c",
	    "sectorA.c", "sectorG.c", "sectorX.c", "selectG.c", "selectX.c",
	    "time_ckG.c", "unitsX.c", "updateA.c", "vms.c", "xferG.c"
    };

    int num_files = sizeof(remaining_files) / sizeof(remaining_files[0]);
    int i;

    /* Test each remaining file */
    for (i = 0; i < num_files; i++) {
        char message[256];
        int result;

        result = test_file_compilation(remaining_files[i]);
        if (result != -1) {
            snprintf(message, sizeof(message), "%s should compile successfully",
                     remaining_files[i]);
            TEST_ASSERT_EQUAL_MESSAGE(0, result, message);
        }
    }
}

/* Unity test runner */
int main(void) {
    UNITY_BEGIN();

    /* Initialize dynamic paths based on current working directory */
    initialize_paths();

    printf("=== Conquer C2023 Compilation Verification Tests ===\n");
    printf("Testing Phase 7A.1 success: All source files compile with C2023 standards\n");
    printf("Compiler flags: %s %s\n", COMPILE_FLAGS, INCLUDE_PATH);
    printf("Expected files: 63 C source files \n\n");

    RUN_TEST(test_core_engine_compilation);
    RUN_TEST(test_io_data_compilation);
    RUN_TEST(test_interface_compilation);
    RUN_TEST(test_game_content_compilation);
    RUN_TEST(test_utility_compilation);
    RUN_TEST(test_additional_files_compilation);
    RUN_TEST(test_remaining_files_compilation);

    printf("\n=== Compilation Verification Complete ===\n");

    return UNITY_END();
}
