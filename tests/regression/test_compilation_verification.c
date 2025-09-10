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

/* Test configuration */
#define MAX_COMMAND_LENGTH 512
#define MAX_OUTPUT_LENGTH 2048
#define COMPILATION_TIMEOUT 30

/* Compilation flags used for C2023 compliance testing */
static const char* COMPILE_FLAGS = 
    "gcc -std=c2x -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -g -O2 -c";

/* Include path for Conquer headers */
static const char* INCLUDE_PATH = "-I../../Include";

void setUp(void) {
    /* Setup for each test */
}

void tearDown(void) {
    /* Cleanup after each test - remove any .o files created during testing */
    system("rm -f *.o");
}

/*
 * Helper function to test compilation of a single source file
 * Returns 0 on success, non-zero on compilation failure
 */
static int test_file_compilation(const char* source_file) {
    char command[MAX_COMMAND_LENGTH];
    char full_path[MAX_COMMAND_LENGTH];
    int result;
    
    /* Build full path to source file */
    snprintf(full_path, sizeof(full_path), "../../Src/%s", source_file);
    
    /* Build compilation command */
    snprintf(command, sizeof(command), "%s %s %s 2>/dev/null", 
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
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("mainA.c"), 
                              "mainA.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("dataA.c"), 
                              "dataA.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("combatA.c"), 
                              "combatA.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("moveA.c"), 
                              "moveA.c should compile successfully");
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
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("mainG.c"), 
                              "mainG.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("displayG.c"), 
                              "displayG.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("infoG.c"), 
                              "infoG.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("inputG.c"), 
                              "inputG.c should compile successfully");
}

/*
 * Test Game Content Files (Priority 4)
 * Game features and content systems
 */
void test_game_content_compilation(void) {
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("magicA.c"), 
                              "magicA.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("monsterA.c"), 
                              "monsterA.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("npcA.c"), 
                              "npcA.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("clanA.c"), 
                              "clanA.c should compile successfully");
}

/*
 * Test Utility and Support Files (Priority 5)
 * Support functions and utilities
 */
void test_utility_compilation(void) {
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("miscA.c"), 
                              "miscA.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("miscX.c"), 
                              "miscX.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("miscG.c"), 
                              "miscG.c should compile successfully");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation("utilityX.c"), 
                              "utilityX.c should compile successfully");
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
        "adduserA.c", "armyG.c", "caravanG.c", "checkX.c", "computeX.c",
        "convertX.c", "customG.c", "customX.c", "dataG.c", "diplomacyA.c",
        "diplomacyG.c", "editG.c", "executeA.c", "fileX.c", "forumA.c",
        "forumG.c", "guildA.c", "guildG.c", "healthA.c", "helpG.c",
        "industryA.c", "industryG.c", "kingdomA.c", "kingdomG.c", "listG.c",
        "logonA.c", "logonG.c", "mailA.c", "mailG.c", "mapG.c",
        "marketA.c", "marketG.c", "mvtpntsA.c", "mvtpntsG.c", "navyA.c",
        "navyG.c", "nclassA.c", "nclassG.c", "newsA.c", "newsG.c",
        "parseX.c", "passwordG.c", "populatnA.c", "populatnG.c", "randomX.c",
        "reportA.c", "reportG.c", "researchA.c", "researchG.c", "sectG.c",
        "spellA.c", "spellG.c", "statusG.c", "statisticX.c", "systemX.c",
        "terrorA.c", "timeA.c", "titleA.c", "titleG.c", "updateA.c",
        "voteA.c", "voteG.c", "wizardA.c", "wizardG.c"
    };
    
    int num_files = sizeof(remaining_files) / sizeof(remaining_files[0]);
    int i;
    
    /* Test each remaining file */
    for (i = 0; i < num_files; i++) {
        char message[256];
        snprintf(message, sizeof(message), "%s should compile successfully", 
                 remaining_files[i]);
        TEST_ASSERT_EQUAL_MESSAGE(0, test_file_compilation(remaining_files[i]), 
                                  message);
    }
}

/* Unity test runner */
int main(void) {
    UNITY_BEGIN();
    
    printf("=== Conquer C2023 Compilation Verification Tests ===\n");
    printf("Testing Phase 7A.1 success: All source files compile with C2023 standards\n");
    printf("Compiler flags: %s %s\n", COMPILE_FLAGS, INCLUDE_PATH);
    printf("Expected files: 64 C source files\n\n");
    
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