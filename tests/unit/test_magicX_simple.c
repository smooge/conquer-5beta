/*
 * Simplified Unit Tests for Src/magicX.c Public Functions
 * Generated: 2025-09-17
 * Functions tested: 3 public functions (magic_ok, rand_magic, mgk_sctval)
 *
 * This file contains unit tests for the publicly accessible functions in magicX.c.
 * We focus on testing the functions that can actually be called directly when
 * linking with the magicX.c source file.
 *
 * Public Functions Tested:
 * - magic_ok: Magic power validation logic
 * - rand_magic: Random magic power generation with validation
 * - mgk_sctval: Magical combat bonus calculation for terrain
 *
 * Note: Static functions (mil_uppow, civ_uppow, etc.) are not directly testable
 * when linking with source. They would need integration tests through add_powers/kill_powers
 * or require copying their implementations (which creates maintenance issues).
 */

#include "../framework/unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Include/dataX.h"
#include "../../Include/magicX.h"
#include "../../Include/racesX.h"
#include "../../Include/elevegX.h"

/*
 * Test Infrastructure Setup
 */

/* Mock nation structure for testing */
static NTN_STRUCT test_nation;

/* Mock race information array */
static RACE_STRUCT test_race_info[10]; /* Support up to 10 races for testing */

/* Mock sector array for terrain testing */
static SCT_STRUCT test_sector_data[100][100]; /* 100x100 test map data */
static SCT_STRUCT *test_sector_ptrs[100]; /* Row pointers for 2D access */

/* Mock magic class list for validation testing */
static MCLASS_STRUCT test_mclass_list[MAG_NUMBER];

/* Mock magic power list for detailed testing */
static MAGIC_STRUCT test_power_list[20]; /* Support up to 20 powers per class */

/* Mock random value control */
static long mock_rand_val_return = 0;
static int mock_rand_val_call_count = 0;

/* Mock world structure */
static struct s_world test_world;

/* Global variable definitions that need to be defined for testing */
NTN_PTR ntn_ptr = &test_nation;
RACE_STRUCT race_info[10]; /* Global definition for code under test */
SCT_STRUCT **sct = test_sector_ptrs; /* Global definition for code under test */
MCLASS_STRUCT mclass_list[MAG_NUMBER]; /* Global definition for code under test */
struct s_world world = {0}; /* Global definition for magicX.c */

/*
 * Mock Functions for Controllable Testing
 */

/* Mock random value generator with controllable output */
long rand_val(int max) {
    (void)max; /* Suppress unused parameter warning */
    mock_rand_val_call_count++;
    return mock_rand_val_return;
}

/*
 * Test Utility Functions
 */

/* Initialize mock nation with clean state */
void init_test_nation(void) {
    memset(&test_nation, 0, sizeof(NTN_STRUCT));
    test_nation.aplus = 0;
    test_nation.dplus = 0;
    test_nation.maxmove = 10; /* Standard movement */
    test_nation.repro = 5;    /* Minimum reproduction rate */
    test_nation.race = 0;     /* First race */
    /* Clear all powers */
    test_nation.powers[MAG_MILITARY] = 0L;
    test_nation.powers[MAG_CIVILIAN] = 0L;
    test_nation.powers[MAG_WIZARDRY] = 0L;
}

/* Initialize mock race info for testing */
void init_test_race_info(void) {
    memset(test_race_info, 0, sizeof(test_race_info));
    memcpy(race_info, test_race_info, sizeof(test_race_info));

    /* Set up default race (index 0) */
    race_info[0].repro_limit = 15; /* Standard reproduction limit */
    race_info[0].pow_limit[MAG_MILITARY] = 0L;   /* No power restrictions */
    race_info[0].pow_limit[MAG_CIVILIAN] = 0L;   /* No power restrictions */
    race_info[0].pow_limit[MAG_WIZARDRY] = 0L;   /* No power restrictions */
}

/* Initialize mock sector array for terrain testing */
void init_test_sectors(void) {
    /* Initialize 2D sector array */
    memset(test_sector_data, 0, sizeof(test_sector_data));

    /* Set up row pointers for 2D access */
    for (int i = 0; i < 100; i++) {
        test_sector_ptrs[i] = test_sector_data[i];
    }

    /* Set up test sectors with known terrain */
    sct[50][50].vegetation = VEG_DESERT;
    sct[50][50].altitude = ELE_CLEAR;

    sct[51][51].vegetation = VEG_FOREST;
    sct[51][51].altitude = ELE_WATER;

    sct[52][52].vegetation = VEG_SWAMP;
    sct[52][52].altitude = ELE_MOUNTAIN;
}

/* Initialize mock magic class list */
void init_test_mclass_list(void) {
    memset(test_mclass_list, 0, sizeof(test_mclass_list));
    memcpy(mclass_list, test_mclass_list, sizeof(test_mclass_list));

    /* Set up magic class maximums */
    mclass_list[MAG_MILITARY].maxval = 11; /* 12 military powers (0-11) */
    mclass_list[MAG_CIVILIAN].maxval = 17; /* 18 civilian powers (0-17) */
    mclass_list[MAG_WIZARDRY].maxval = 17; /* 18 wizardry powers (0-17) */

    /* Set up power lists */
    mclass_list[MAG_MILITARY].pow_list = test_power_list;
    mclass_list[MAG_CIVILIAN].pow_list = test_power_list;
    mclass_list[MAG_WIZARDRY].pow_list = test_power_list;

    /* Initialize test power prerequisites (all clear for basic testing) */
    for (int i = 0; i < 20; i++) {
        test_power_list[i].pow_need[MAG_MILITARY] = 0L;
        test_power_list[i].pow_need[MAG_CIVILIAN] = 0L;
        test_power_list[i].pow_need[MAG_WIZARDRY] = 0L;
    }
}

/* Initialize mock world structure */
void init_test_world(void) {
    memset(&test_world, 0, sizeof(struct s_world));
    memcpy(&world, &test_world, sizeof(struct s_world));

    /* Set up basic world parameters that mgk_sctval might need */
    world.mapx = 100;
    world.mapy = 100;
}

/*
 * Unity Test Framework Setup/Teardown
 */

void setUp(void) {
    /* Initialize all mock data for each test */
    init_test_nation();
    init_test_race_info();
    init_test_sectors();
    init_test_mclass_list();
    init_test_world();

    /* Reset mock function call tracking */
    mock_rand_val_call_count = 0;
    mock_rand_val_return = 0;
}

void tearDown(void) {
    /* Clean up after each test if needed */
}

/*
 * ============================================================================
 * MAGIC VALIDATION TESTS (magic_ok)
 * ============================================================================
 */

void test_magic_ok_already_possessed(void) {
    /* Test rejection when power is already possessed */
    ntn_ptr->powers[MAG_MILITARY] = MM_WARRIOR; /* Already has warrior */

    int result = magic_ok(MAG_MILITARY, 11); /* MM_WARRIOR is bit 11 (0x800) */
    TEST_ASSERT_EQUAL_INT(FALSE, result);
}

void test_magic_ok_race_restriction(void) {
    /* Test rejection due to race restrictions */
    ntn_ptr->powers[MAG_CIVILIAN] = 0L; /* No powers */
    ntn_ptr->race = 0;
    race_info[0].pow_limit[MAG_CIVILIAN] = MC_RELIGION; /* Religion restricted */

    int result = magic_ok(MAG_CIVILIAN, 12); /* MC_RELIGION is bit 12 (0x1000) */
    TEST_ASSERT_EQUAL_INT(FALSE, result);
}

void test_magic_ok_valid_power(void) {
    /* Test acceptance of valid power */
    ntn_ptr->powers[MAG_WIZARDRY] = 0L; /* No powers */
    ntn_ptr->race = 0;
    race_info[0].pow_limit[MAG_WIZARDRY] = 0L; /* No restrictions */

    /* Set up prerequisites as met (all clear) */
    test_power_list[3].pow_need[MAG_MILITARY] = 0L;
    test_power_list[3].pow_need[MAG_CIVILIAN] = 0L;
    test_power_list[3].pow_need[MAG_WIZARDRY] = 0L;

    int result = magic_ok(MAG_WIZARDRY, 3); /* MW_DRUIDISM is bit 3 (0x8) */
    TEST_ASSERT_EQUAL_INT(TRUE, result);
}

/*
 * ============================================================================
 * RANDOM MAGIC GENERATION TESTS (rand_magic)
 * ============================================================================
 */

void test_rand_magic_successful_generation(void) {
    /* Test successful random magic power generation */

    /* Set up mocks for success case */
    mock_rand_val_return = 5; /* Will generate power bit 5 */
    ntn_ptr->powers[MAG_MILITARY] = 0L; /* No existing powers */
    race_info[0].pow_limit[MAG_MILITARY] = 0L; /* No restrictions */

    /* Set up prerequisites as met */
    test_power_list[5].pow_need[MAG_MILITARY] = 0L;
    test_power_list[5].pow_need[MAG_CIVILIAN] = 0L;
    test_power_list[5].pow_need[MAG_WIZARDRY] = 0L;

    long result = rand_magic(MAG_MILITARY);
    TEST_ASSERT_EQUAL_INT64(1L << 5, result); /* Should return power bit 5 */
    TEST_ASSERT_EQUAL_INT(1, mock_rand_val_call_count); /* Called once */
}

void test_rand_magic_failure_after_attempts(void) {
    /* Test failure after maximum attempts */

    /* Set up mocks for repeated failure */
    mock_rand_val_return = 3; /* Always generate same power */
    ntn_ptr->powers[MAG_CIVILIAN] = (1L << 3); /* Already has that power */

    long result = rand_magic(MAG_CIVILIAN);
    TEST_ASSERT_EQUAL_INT64(0L, result); /* Should return 0 (failure) */
    TEST_ASSERT_EQUAL_INT(500, mock_rand_val_call_count); /* Max attempts */
}

/*
 * ============================================================================
 * TERRAIN COMBAT BONUS TESTS (mgk_sctval)
 * ============================================================================
 */

void test_mgk_sctval_null_nation(void) {
    /* Test null nation pointer handling */
    int result = mgk_sctval(NULL, 50, 50);
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_mgk_sctval_off_map_coordinates(void) {
    /* Test off-map coordinate handling */
    int result = mgk_sctval(&test_nation, -1, 50);
    TEST_ASSERT_EQUAL_INT(0, result);

    result = mgk_sctval(&test_nation, 50, 150);
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_mgk_sctval_desert_bonuses(void) {
    /* Test desert/ice vegetation bonuses */

    /* Set up desert terrain */
    sct[25][25].vegetation = VEG_DESERT;
    sct[25][25].altitude = ELE_CLEAR;

    /* Test dervish bonus */
    ntn_ptr->powers[MAG_CIVILIAN] = MC_DERVISH;
    ntn_ptr->powers[MAG_WIZARDRY] = 0L;
    int result = mgk_sctval(ntn_ptr, 25, 25);
    TEST_ASSERT_EQUAL_INT(20, result); /* +20 for dervish */

    /* Test destroyer bonus */
    ntn_ptr->powers[MAG_CIVILIAN] = 0L;
    ntn_ptr->powers[MAG_WIZARDRY] = MW_DESTROYER;
    result = mgk_sctval(ntn_ptr, 25, 25);
    TEST_ASSERT_EQUAL_INT(20, result); /* +20 for destroyer */

    /* Test stacking bonuses */
    ntn_ptr->powers[MAG_CIVILIAN] = MC_DERVISH;
    ntn_ptr->powers[MAG_WIZARDRY] = MW_DESTROYER;
    result = mgk_sctval(ntn_ptr, 25, 25);
    TEST_ASSERT_EQUAL_INT(40, result); /* +20 + 20 stacking */
}

void test_mgk_sctval_water_altitude_bonuses(void) {
    /* Test water altitude bonuses and penalties */

    /* Set up water terrain */
    sct[35][35].vegetation = VEG_GOOD;
    sct[35][35].altitude = ELE_WATER;

    /* Test water magic bonus */
    ntn_ptr->powers[MAG_WIZARDRY] = MW_WATER;
    ntn_ptr->powers[MAG_CIVILIAN] = 0L;
    int result = mgk_sctval(ntn_ptr, 35, 35);
    TEST_ASSERT_EQUAL_INT(30, result); /* +30 for water magic */

    /* Test earth magic penalty */
    ntn_ptr->powers[MAG_WIZARDRY] = MW_EARTH;
    result = mgk_sctval(ntn_ptr, 35, 35);
    TEST_ASSERT_EQUAL_INT(-10, result); /* -10 for earth in water */

    /* Test naval bonuses */
    ntn_ptr->powers[MAG_WIZARDRY] = 0L;
    ntn_ptr->powers[MAG_CIVILIAN] = MC_MARINE;
    result = mgk_sctval(ntn_ptr, 35, 35);
    TEST_ASSERT_EQUAL_INT(20, result); /* +20 for marine */
}

/*
 * ============================================================================
 * UNIT TEST MAIN FUNCTION
 * ============================================================================
 */

int main(void) {
    UNITY_BEGIN();

    /* Magic Validation Tests */
    RUN_TEST(test_magic_ok_already_possessed);
    RUN_TEST(test_magic_ok_race_restriction);
    RUN_TEST(test_magic_ok_valid_power);

    /* Random Magic Generation Tests */
    RUN_TEST(test_rand_magic_successful_generation);
    RUN_TEST(test_rand_magic_failure_after_attempts);

    /* Terrain Combat Bonus Tests */
    RUN_TEST(test_mgk_sctval_null_nation);
    RUN_TEST(test_mgk_sctval_off_map_coordinates);
    RUN_TEST(test_mgk_sctval_desert_bonuses);
    RUN_TEST(test_mgk_sctval_water_altitude_bonuses);

    return UNITY_END();
}