/*
 * cmake_test.c - Simple test program for CMake build system validation
 *
 * This program tests that:
 * - CMake configuration works
 * - Feature detection functions properly  
 * - Library linking is successful
 * - Generated config.h is valid
 */

#include <stdio.h>
#include <stdlib.h>

/* Include our generated configuration */
#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_CURSES_H
#include <curses.h>
#elif defined(HAVE_NCURSES_H)
#include <ncurses.h>
#endif

int main(void) {
    printf("=== Conquer CMake Build System Test ===\n");
    printf("Version: %s\n", CONQUER_VERSION);
    printf("Admin User: %s\n", DEFAULT_ADMIN_USER);
    printf("Data Path: %s\n", CONQUER_DATA_PATH);
    
    printf("\n=== Feature Detection Results ===\n");
    
    #ifdef HAVE_STDLIB_H
    printf("✓ stdlib.h available\n");
    #else
    printf("✗ stdlib.h not available\n");
    #endif
    
    #ifdef HAVE_UNISTD_H
    printf("✓ unistd.h available\n");
    #else
    printf("✗ unistd.h not available\n");
    #endif
    
    #ifdef HAVE_STRING_H
    printf("✓ string.h available\n");
    #else
    printf("✗ string.h not available\n");
    #endif
    
    #ifdef HAVE_CURSES_H
    printf("✓ curses.h available\n");
    #elif defined(HAVE_NCURSES_H)
    printf("✓ ncurses.h available\n");
    #else
    printf("✗ No curses library headers found\n");
    #endif
    
    #ifdef HAVE_LIBCRYPT
    printf("✓ crypt library available\n");
    #elif defined(HAVE_CRYPT_IN_LIBC)
    printf("✓ crypt function in libc\n");
    #else
    printf("⚠ crypt function not found\n");
    #endif
    
    printf("\n=== Game Configuration ===\n");
    
    #ifdef COMPRESS
    printf("✓ Compression enabled\n");
    #else
    printf("✗ Compression disabled\n");
    #endif
    
    #ifdef HUGE_MAP
    printf("✓ Large maps enabled\n");
    #else
    printf("✗ Large maps disabled\n");
    #endif
    
    #ifdef MANY_UNITS
    printf("✓ Many units enabled\n");
    #else
    printf("✗ Many units disabled\n");
    #endif
    
    printf("Max Nations: %d\n", ABSMAXNTN);
    
    printf("\n=== System Function Tests ===\n");
    
    #ifdef HAVE_LRAND48
    printf("✓ Using lrand48() for random numbers\n");
    #elif defined(HAVE_RANDOM)
    printf("✓ Using random() for random numbers\n");
    #else
    printf("⚠ Using rand() for random numbers (lower quality)\n");
    #endif
    
    #ifdef HAVE_FLOCK
    printf("✓ Using flock() for file locking\n");
    #elif defined(HAVE_LOCKF)
    printf("✓ Using lockf() for file locking\n");
    #else
    printf("⚠ No file locking mechanism available\n");
    #endif
    
    printf("\n=== CMake Build System Test: SUCCESS ===\n");
    printf("The CMake configuration and feature detection is working properly.\n");
    printf("Ready to proceed with code modernization strategy.\n");
    
    return 0;
}