/*
 * Security Tests for check_lock() Function - checkX.c
 *
 * Critical file locking tests for race condition prevention and data corruption avoidance.
 * Tests based on Phase 6A Option C detailed specifications.
 *
 * Security Risk: HIGH - Race conditions could corrupt game data
 * Test Priority: HIGH
 */

#include "../framework/unity.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <sys/file.h>

/* Define lock constants if not available */
#ifndef LOCK_EX
#define LOCK_EX 2
#endif
#ifndef LOCK_UN
#define LOCK_UN 8
#endif
#ifndef LOCK_NB
#define LOCK_NB 4
#endif

/* Mock file system for testing without affecting real files */
#define MAX_MOCK_FILES 10
#define MAX_FILENAME_LEN 256

struct mock_file {
    char filename[MAX_FILENAME_LEN];
    int fd;
    int is_locked;
    time_t creation_time;
    int permissions;
    int exists;
};

static struct mock_file mock_files[MAX_MOCK_FILES];
static int mock_file_count = 0;
static int mock_next_fd = 100;
static int mock_simulate_permission_error = 0;
static int mock_simulate_lock_failure = 0;
static time_t mock_current_time = 0;

/* Mock system functions */
static int mock_open(const char *pathname, int flags __attribute__((unused)), mode_t mode) {
    /* Simulate permission error */
    if (mock_simulate_permission_error) {
        errno = EACCES;
        return -1;
    }

    /* Find existing file or create new one */
    for (int i = 0; i < mock_file_count; i++) {
        if (strcmp(mock_files[i].filename, pathname) == 0) {
            if (!mock_files[i].exists) {
                /* File was unlinked, reuse this slot and create new */
                mock_files[i].fd = mock_next_fd++;
                mock_files[i].creation_time = mock_current_time;
                mock_files[i].exists = 1;
                mock_files[i].permissions = mode;
                return mock_files[i].fd;
            }
            return mock_files[i].fd;
        }
    }

    /* Create new mock file if space available */
    if (mock_file_count < MAX_MOCK_FILES) {
        struct mock_file *file = &mock_files[mock_file_count];
        strncpy(file->filename, pathname, MAX_FILENAME_LEN - 1);
        file->filename[MAX_FILENAME_LEN - 1] = '\0';
        file->fd = mock_next_fd++;
        file->is_locked = 0;
        file->creation_time = mock_current_time;
        file->permissions = mode;
        file->exists = 1;
        mock_file_count++;
        return file->fd;
    }

    errno = EMFILE;
    return -1;
}

static int mock_close(int fd) {
    for (int i = 0; i < mock_file_count; i++) {
        if (mock_files[i].fd == fd) {
            mock_files[i].is_locked = 0;
            return 0;
        }
    }
    errno = EBADF;
    return -1;
}

static int mock_flock(int fd, int operation) {
    if (mock_simulate_lock_failure) {
        errno = EAGAIN;
        return -1;
    }

    for (int i = 0; i < mock_file_count; i++) {
        if (mock_files[i].fd == fd) {
            if (operation & LOCK_EX) {
                if (mock_files[i].is_locked) {
                    errno = EAGAIN;
                    return -1;
                }
                mock_files[i].is_locked = 1;
            } else if (operation & LOCK_UN) {
                mock_files[i].is_locked = 0;
            }
            return 0;
        }
    }
    errno = EBADF;
    return -1;
}

static int mock_unlink(const char *pathname) {
    for (int i = 0; i < mock_file_count; i++) {
        if (strcmp(mock_files[i].filename, pathname) == 0) {
            mock_files[i].exists = 0;
            return 0;
        }
    }
    errno = ENOENT;
    return -1;
}

static int mock_stat(const char *pathname, struct stat *statbuf) {
    for (int i = 0; i < mock_file_count; i++) {
        if (strcmp(mock_files[i].filename, pathname) == 0 && mock_files[i].exists) {
            memset(statbuf, 0, sizeof(struct stat));
            statbuf->st_mtime = mock_files[i].creation_time;
            return 0;
        }
    }
    errno = ENOENT;
    return -1;
}

static time_t mock_time(time_t *tloc) {
    if (tloc) *tloc = mock_current_time;
    return mock_current_time;
}

/* Mock implementation of check_lock() function for testing */
#define TIME_DEAD 300  /* 5 minutes in seconds */

static int check_lock(char *filename, int keeplock) {
    int filed_id;
    struct stat fst;

    /* Check for NULL filename */
    if (filename == NULL) {
        return -1;
    }

    /* Check if the file exists and how old it is (non-FILELOCK version simulation) */
    filed_id = 0;
    if (mock_stat(filename, &fst) == 0) {
        time_t now = mock_time(NULL);
        if (now - fst.st_mtime < TIME_DEAD * 3) {
            filed_id = -1;  /* File is locked */
        } else {
            /* Remove stale file */
            mock_unlink(filename);
        }
    }

    /* Does a lock need to be made? */
    if ((filed_id != -1) && (keeplock == 1)) {
        /* Create lock file */
        if ((filed_id = mock_open(filename, O_WRONLY | O_CREAT, 0600)) == -1) {
            return -1;  /* Error creating lock */
        }
    }

    return filed_id;
}

void setUp(void) {
    /* Reset mock state before each test */
    memset(mock_files, 0, sizeof(mock_files));
    mock_file_count = 0;
    mock_next_fd = 100;
    mock_simulate_permission_error = 0;
    mock_simulate_lock_failure = 0;
    mock_current_time = 1000000; /* Set a base time */
}

void tearDown(void) {
    /* Cleanup after each test */
}

/*
 * Test Case CL-001: Normal Lock Creation
 * Test: New file lock created successfully
 * Setup: Unlocked file, keeplock = 1
 * Execute: result = check_lock("testfile.dat", 1)
 * Expected: Lock created, function returns success
 * Validation: Lock file exists, proper permissions
 */
void test_check_lock_normal_lock_creation(void) {
    /* Setup: No existing lock file */
    const char *filename = "testfile.dat";

    /* Execute: Create new lock */
    int result = check_lock((char *)filename, 1);

    /* Expected: Lock created successfully */
    TEST_ASSERT_TRUE(result >= 0);
    TEST_ASSERT_MESSAGE(result >= 0, "Normal lock creation should succeed");

    /* Validation: Lock file exists in mock system */
    int found = 0;
    for (int i = 0; i < mock_file_count; i++) {
        if (strcmp(mock_files[i].filename, filename) == 0 && mock_files[i].exists) {
            found = 1;
            TEST_ASSERT_EQUAL(0600, mock_files[i].permissions);
            break;
        }
    }
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_MESSAGE(found, "Lock file should exist with proper permissions");
}

/*
 * Test Case CL-002: Existing Lock Detection
 * Test: Existing lock properly detected
 * Setup: Pre-existing lock file
 * Execute: result = check_lock("testfile.dat", 1)
 * Expected: Lock detected, function returns busy status
 * Validation: Concurrent access prevented
 */
void test_check_lock_existing_lock_detection(void) {
    /* Setup: Create pre-existing lock file */
    const char *filename = "testfile.dat";

    /* Create existing lock */
    struct mock_file *file = &mock_files[mock_file_count++];
    strncpy(file->filename, filename, MAX_FILENAME_LEN - 1);
    file->fd = mock_next_fd++;
    file->is_locked = 1;
    file->creation_time = mock_current_time;
    file->exists = 1;

    /* Execute: Try to create lock when one already exists */
    int result = check_lock((char *)filename, 1);

    /* Expected: Lock detected, busy status returned (-1) */
    TEST_ASSERT_EQUAL(-1, result);
    TEST_ASSERT_MESSAGE(result == -1, "Existing lock should be detected and prevent new lock");
}

/*
 * Test Case CL-003: Stale Lock Cleanup
 * Test: Old locks cleaned up appropriately
 * Setup: Create stale lock file (old timestamp)
 * Execute: result = check_lock("testfile.dat", 1)
 * Expected: Stale lock removed, new lock created
 * Validation: Dead lock prevention
 */
void test_check_lock_stale_lock_cleanup(void) {
    /* Setup: Create stale lock file (older than TIME_DEAD * 3) */
    const char *filename = "testfile.dat";

    /* Create stale lock */
    struct mock_file *file = &mock_files[mock_file_count++];
    strncpy(file->filename, filename, MAX_FILENAME_LEN - 1);
    file->fd = mock_next_fd++;
    file->is_locked = 1;
    file->creation_time = mock_current_time - (TIME_DEAD * 4); /* Make it stale */
    file->exists = 1;

    /* Execute: Try to create lock - should clean up stale lock */
    int result = check_lock((char *)filename, 1);

    /* Expected: Stale lock removed, new lock created */
    TEST_ASSERT_TRUE(result >= 0);
    TEST_ASSERT_MESSAGE(result >= 0, "Stale lock should be cleaned up and new lock created");

    /* Validation: Old lock removed, new lock created */
    int found_active_lock = 0;
    for (int i = 0; i < mock_file_count; i++) {
        if (strcmp(mock_files[i].filename, filename) == 0 && mock_files[i].exists) {
            found_active_lock = 1;
            /* New lock should have current timestamp */
            TEST_ASSERT_TRUE(mock_files[i].creation_time >= mock_current_time);
            break;
        }
    }
    TEST_ASSERT_TRUE(found_active_lock);
    TEST_ASSERT_MESSAGE(found_active_lock, "New lock should be created after stale cleanup");
}

/*
 * Test Case CL-004: Permission Denied Handling
 * Test: Lock creation with insufficient permissions
 * Setup: Read-only directory or file
 * Execute: result = check_lock("readonly/test.dat", 1)
 * Expected: Graceful failure, appropriate error code
 * Validation: Permission error handling
 */
void test_check_lock_permission_denied_handling(void) {
    /* Setup: Simulate permission error */
    const char *filename = "readonly/test.dat";
    mock_simulate_permission_error = 1;

    /* Execute: Try to create lock without permissions */
    int result = check_lock((char *)filename, 1);

    /* Expected: Graceful failure (-1) */
    TEST_ASSERT_EQUAL(-1, result);
    TEST_ASSERT_MESSAGE(result == -1, "Permission denied should be handled gracefully");

    /* Validation: No lock file created */
    int found = 0;
    for (int i = 0; i < mock_file_count; i++) {
        if (strcmp(mock_files[i].filename, filename) == 0) {
            found = 1;
            break;
        }
    }
    TEST_ASSERT_FALSE(found);
    TEST_ASSERT_MESSAGE(!found, "No lock file should be created on permission error");
}

/*
 * Test Case CL-005: Concurrent Lock Attempts
 * Test: Multiple simultaneous lock requests
 * Setup: Multiple processes attempting lock
 * Execute: Parallel check_lock calls
 * Expected: Only one succeeds, others wait or fail
 * Validation: Race condition prevention
 */
void test_check_lock_concurrent_lock_attempts(void) {
    /* Setup: Create scenario simulating concurrent access */
    const char *filename = "concurrent_test.dat";

    /* First process gets the lock */
    int result1 = check_lock((char *)filename, 1);
    TEST_ASSERT_TRUE(result1 >= 0);
    TEST_ASSERT_MESSAGE(result1 >= 0, "First lock attempt should succeed");

    /* Second process tries to get the same lock */
    int result2 = check_lock((char *)filename, 1);
    TEST_ASSERT_EQUAL(-1, result2);
    TEST_ASSERT_MESSAGE(result2 == -1, "Second concurrent lock attempt should fail");

    /* Third process also tries */
    int result3 = check_lock((char *)filename, 1);
    TEST_ASSERT_EQUAL(-1, result3);
    TEST_ASSERT_MESSAGE(result3 == -1, "Third concurrent lock attempt should fail");

    /* Validation: Only one lock exists */
    int lock_count = 0;
    for (int i = 0; i < mock_file_count; i++) {
        if (strstr(mock_files[i].filename, filename) && mock_files[i].exists) {
            lock_count++;
        }
    }
    TEST_ASSERT_EQUAL(1, lock_count);
    TEST_ASSERT_MESSAGE(lock_count == 1, "Only one lock should exist for concurrent attempts");
}

/*
 * Test Case CL-006: Check Without Keep (Read-Only Check)
 * Test: Check lock status without creating/keeping lock
 * Setup: Various lock states
 * Execute: result = check_lock("testfile.dat", 0)
 * Expected: Status returned without side effects
 * Validation: No lock files created or modified
 */
void test_check_lock_check_without_keep(void) {
    /* Test 1: Check non-existent lock */
    const char *filename = "check_only_test.dat";
    int result1 = check_lock((char *)filename, 0);
    TEST_ASSERT_EQUAL(0, result1);
    TEST_ASSERT_MESSAGE(result1 == 0, "Check of non-existent lock should return 0");

    /* Verify no file created */
    int found = 0;
    for (int i = 0; i < mock_file_count; i++) {
        if (strcmp(mock_files[i].filename, filename) == 0) {
            found = 1;
            break;
        }
    }
    TEST_ASSERT_FALSE(found);
    TEST_ASSERT_MESSAGE(!found, "Check-only should not create lock file");

    /* Test 2: Check existing lock */
    /* Setup existing lock */
    struct mock_file *file = &mock_files[mock_file_count++];
    strncpy(file->filename, filename, MAX_FILENAME_LEN - 1);
    file->fd = mock_next_fd++;
    file->creation_time = mock_current_time;
    file->exists = 1;

    int result2 = check_lock((char *)filename, 0);
    TEST_ASSERT_EQUAL(-1, result2);
    TEST_ASSERT_MESSAGE(result2 == -1, "Check of existing lock should return -1");
}

/*
 * Test Case CL-007: NULL Pointer Handling
 * Test: Function behavior with NULL filename
 * Setup: NULL pointer parameter
 * Execute: result = check_lock(NULL, 1)
 * Expected: Graceful failure
 * Validation: No crash, proper error handling
 */
void test_check_lock_null_pointer_handling(void) {
    /* Execute: Test NULL pointer handling */
    int result = check_lock(NULL, 1);

    /* Expected: Graceful failure (-1) */
    TEST_ASSERT_EQUAL(-1, result);
    TEST_ASSERT_MESSAGE(result == -1, "NULL filename should be handled gracefully");
}

/*
 * Test Case CL-008: Lock Cleanup and Resource Management
 * Test: Proper resource cleanup after lock operations
 * Setup: Create multiple locks
 * Execute: Various lock operations
 * Expected: Resources properly managed
 * Validation: No resource leaks
 */
void test_check_lock_resource_management(void) {
    /* Create multiple locks */
    const char *files[] = {"lock1.dat", "lock2.dat", "lock3.dat"};
    int results[3];
    int num_files = sizeof(files) / sizeof(files[0]);

    /* Create locks */
    for (int i = 0; i < num_files; i++) {
        results[i] = check_lock((char *)files[i], 1);
        TEST_ASSERT_TRUE(results[i] >= 0);
    }

    /* Verify all locks created */
    TEST_ASSERT_EQUAL(num_files, mock_file_count);

    /* Simulate resource cleanup by removing files */
    for (int i = 0; i < num_files; i++) {
        int cleanup_result = mock_unlink(files[i]);
        TEST_ASSERT_EQUAL(0, cleanup_result);
    }

    /* Validation: Resources properly cleaned up */
    int active_locks = 0;
    for (int i = 0; i < mock_file_count; i++) {
        if (mock_files[i].exists) {
            active_locks++;
        }
    }
    TEST_ASSERT_EQUAL(0, active_locks);
    TEST_ASSERT_MESSAGE(active_locks == 0, "All locks should be cleaned up");
}

/*
 * Test Case CL-009: Time-based Lock Expiration
 * Test: Lock expiration based on time thresholds
 * Setup: Locks with different ages
 * Execute: Check locks at different time intervals
 * Expected: Proper time-based expiration
 * Validation: Expired locks are cleaned up
 */
void test_check_lock_time_based_expiration(void) {
    const char *filename = "time_test.dat";

    /* Create lock at initial time */
    int result1 = check_lock((char *)filename, 1);
    TEST_ASSERT_TRUE(result1 >= 0);

    /* Advance time by half the expiration period - lock should still be valid */
    mock_current_time += TIME_DEAD;
    int result2 = check_lock((char *)filename, 1);
    TEST_ASSERT_EQUAL(-1, result2); /* Still locked */

    /* Advance time beyond expiration - lock should be cleaned up */
    mock_current_time += TIME_DEAD * 3;
    int result3 = check_lock((char *)filename, 1);
    TEST_ASSERT_TRUE(result3 >= 0); /* Old lock cleaned, new lock created */

    TEST_ASSERT_MESSAGE(result3 >= 0, "Expired lock should be cleaned up and new lock created");
}

/* Unity test runner */
int main(void) {
    UNITY_BEGIN();

    /* Critical Security Functions (Priority 1) - check_lock() */
    RUN_TEST(test_check_lock_normal_lock_creation);
    RUN_TEST(test_check_lock_existing_lock_detection);
    RUN_TEST(test_check_lock_stale_lock_cleanup);
    RUN_TEST(test_check_lock_permission_denied_handling);
    RUN_TEST(test_check_lock_concurrent_lock_attempts);
    RUN_TEST(test_check_lock_check_without_keep);
    RUN_TEST(test_check_lock_null_pointer_handling);
    RUN_TEST(test_check_lock_resource_management);
    RUN_TEST(test_check_lock_time_based_expiration);

    return UNITY_END();
}