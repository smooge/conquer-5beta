/*
 * Isolated Unit Tests for convertX.c Utility Functions
 *
 * Tests pure mathematical and string utility functions from convertX.c
 * using function extraction/isolation methodology.
 *
 * Functions tested:
 * - char_in_str() - String character search
 * - non_cntrl() - Control character to printable conversion
 * - to_cntrl() - Printable to control character conversion
 * - roman_value() - Roman numeral value calculation
 * - roman_base() - Roman numeral base finding
 * - completion() - String prefix matching
 * - poptoworkers() - Population to workers economic calculation
 */

#include "unity.h"
#include <string.h>
#include <ctype.h>

/* ============================================================================
 * MOCK CONSTANTS AND DEFINITIONS
 * ============================================================================ */

/* Constants from keyvalsX.h and header.h */
#define EXT_DEL '\177'          /* Delete character (ASCII 127) */
#define TOOMANYPEOPLE 5000L     /* Population efficiency threshold */
#define ABSMAXPEOPLE 50000L     /* Absolute maximum population per sector */

/* Boolean constants */
#define TRUE 1
#define FALSE 0

/* ============================================================================
 * EXTRACTED FUNCTIONS FROM convertX.c
 * ============================================================================ */

/*
 * EXTRACTED: char_in_str (from convertX.c:362-369)
 * Pure string search function - copied unchanged
 */
int char_in_str_extracted(char ch, char *str) {
    int i, l = strlen(str);

    for (i = 0; i < l; i++)
        if (ch == str[i]) return(TRUE);
    return(FALSE);
}

/*
 * EXTRACTED: non_cntrl (from convertX.c:398-412)
 * Pure character conversion function - copied unchanged
 */
int non_cntrl_extracted(int ch) {
    int hold;

    if (!isprint(ch)) {
        if (ch != EXT_DEL) {
            hold = ch + '@';
        } else {
            hold = '?';
        }
    } else {
        hold = ch;
    }
    return(hold);
}

/*
 * EXTRACTED: to_cntrl (from convertX.c:441-450)
 * Pure character conversion function - copied unchanged
 */
int to_cntrl_extracted(int ch) {
    int hold;

    if (ch == '?') {
        hold = EXT_DEL;
    } else {
        hold = ch - '@';
    }
    return(hold);
}

/*
 * EXTRACTED: roman_value (from convertX.c:199-216)
 * Pure mathematical function - copied unchanged
 */
long roman_value_extracted(int value) {
    long hold;
    int count;

    /* loop value times */
    hold = 1L;
    for (count = 0; count < value; count++) {
        if (count % 2 == 0) {
            hold *= 5L;
        } else {
            hold *= 2L;
        }
    }

    /* give back results */
    return(hold);
}

/*
 * EXTRACTED: roman_base (from convertX.c:243-264)
 * Pure mathematical function - copied unchanged
 */
int roman_base_extracted(int value) {
    long hold = 4L, base = 5L;
    int count = 0;

    while (TRUE) {
        if (value < hold) {
            break;
        }
        count++;
        if (count % 2 == 0) {
            hold -= base;
            hold *= 10L;
            base *= 10L;
        } else {
            hold += base;
        }
    }
    return(count);
}

/*
 * EXTRACTED: completion (from convertX.c:579-594)
 * Pure string matching function - copied unchanged
 */
int completion_extracted(char *s1, char *s2) {
    int ch1, ch2, hold = TRUE;

    if (strlen(s1) > strlen(s2)) return(FALSE);
    for (; *s1 != '\0'; s1++, s2++) {
        ch1 = (islower(*s1) ? toupper(*s1) : *s1);
        ch2 = (islower(*s2) ? toupper(*s2) : *s2);
        if (ch1 != ch2) {
            hold = FALSE;
            break;
        }
    }

    return(hold);
}

/*
 * EXTRACTED: poptoworkers (from convertX.c:832-851)
 * Pure economic calculation function - copied unchanged
 */
long poptoworkers_extracted(long population) {
    long workpop = 0L;
    int div_val = 1;

    /* check for maxing out */
    if (population > ABSMAXPEOPLE) {
        population = ABSMAXPEOPLE;
    }
    while (population > 0) {
        if (population > TOOMANYPEOPLE) {
            workpop += TOOMANYPEOPLE / div_val;
        } else {
            workpop += population / div_val;
        }
        population -= TOOMANYPEOPLE;
        div_val++;
    }
    return(workpop);
}

/* ============================================================================
 * UNITY TEST FRAMEWORK SETUP
 * ============================================================================ */

void setUp(void) {
    /* Set up test fixtures, if any */
}

void tearDown(void) {
    /* Clean up after each test */
}

/* ============================================================================
 * CHARACTER SEARCH TESTS (char_in_str)
 * ============================================================================ */

void test_char_in_str_character_found_beginning(void) {
    TEST_ASSERT_EQUAL(TRUE, char_in_str_extracted('a', "abcdef"));
}

void test_char_in_str_character_found_middle(void) {
    TEST_ASSERT_EQUAL(TRUE, char_in_str_extracted('d', "abcdef"));
}

void test_char_in_str_character_found_end(void) {
    TEST_ASSERT_EQUAL(TRUE, char_in_str_extracted('f', "abcdef"));
}

void test_char_in_str_character_not_found(void) {
    TEST_ASSERT_EQUAL(FALSE, char_in_str_extracted('z', "abcdef"));
}

void test_char_in_str_empty_string(void) {
    TEST_ASSERT_EQUAL(FALSE, char_in_str_extracted('a', ""));
}

void test_char_in_str_single_character_match(void) {
    TEST_ASSERT_EQUAL(TRUE, char_in_str_extracted('x', "x"));
}

void test_char_in_str_single_character_no_match(void) {
    TEST_ASSERT_EQUAL(FALSE, char_in_str_extracted('y', "x"));
}

void test_char_in_str_special_characters(void) {
    TEST_ASSERT_EQUAL(TRUE, char_in_str_extracted('@', "hello@world"));
    TEST_ASSERT_EQUAL(TRUE, char_in_str_extracted(' ', "hello world"));
    TEST_ASSERT_EQUAL(TRUE, char_in_str_extracted('\t', "a\tb\tc"));
}

/* ============================================================================
 * CHARACTER CONVERSION TESTS (non_cntrl / to_cntrl)
 * ============================================================================ */

void test_non_cntrl_printable_characters_unchanged(void) {
    TEST_ASSERT_EQUAL('A', non_cntrl_extracted('A'));
    TEST_ASSERT_EQUAL('z', non_cntrl_extracted('z'));
    TEST_ASSERT_EQUAL('5', non_cntrl_extracted('5'));
    TEST_ASSERT_EQUAL('@', non_cntrl_extracted('@'));
    TEST_ASSERT_EQUAL(' ', non_cntrl_extracted(' ')); /* Space is printable */
}

void test_non_cntrl_control_characters_converted(void) {
    TEST_ASSERT_EQUAL('A', non_cntrl_extracted(0x01)); /* Ctrl-A -> 'A' */
    TEST_ASSERT_EQUAL('B', non_cntrl_extracted(0x02)); /* Ctrl-B -> 'B' */
    TEST_ASSERT_EQUAL('Z', non_cntrl_extracted(0x1A)); /* Ctrl-Z -> 'Z' */
}

void test_non_cntrl_delete_character_special_case(void) {
    TEST_ASSERT_EQUAL('?', non_cntrl_extracted(EXT_DEL)); /* DEL -> '?' */
}

void test_to_cntrl_normal_characters(void) {
    TEST_ASSERT_EQUAL(0x01, to_cntrl_extracted('A')); /* 'A' -> Ctrl-A */
    TEST_ASSERT_EQUAL(0x02, to_cntrl_extracted('B')); /* 'B' -> Ctrl-B */
    TEST_ASSERT_EQUAL(0x1A, to_cntrl_extracted('Z')); /* 'Z' -> Ctrl-Z */
}

void test_to_cntrl_question_mark_special_case(void) {
    TEST_ASSERT_EQUAL(EXT_DEL, to_cntrl_extracted('?')); /* '?' -> DEL */
}

void test_non_cntrl_to_cntrl_symmetry(void) {
    /* Test that the functions are inverses for control characters */
    for (int ch = 1; ch <= 26; ch++) {
        int converted = non_cntrl_extracted(ch);
        int reverted = to_cntrl_extracted(converted);
        TEST_ASSERT_EQUAL(ch, reverted);
    }
}

void test_to_cntrl_non_cntrl_symmetry_special_cases(void) {
    /* Test special cases work in both directions */
    TEST_ASSERT_EQUAL(EXT_DEL, to_cntrl_extracted(non_cntrl_extracted(EXT_DEL)));
}

/* ============================================================================
 * ROMAN NUMERAL CALCULATION TESTS
 * ============================================================================ */

void test_roman_value_base_cases(void) {
    TEST_ASSERT_EQUAL(1L, roman_value_extracted(0));    /* Base case: 1 */
    TEST_ASSERT_EQUAL(5L, roman_value_extracted(1));    /* 1 * 5 = 5 */
    TEST_ASSERT_EQUAL(10L, roman_value_extracted(2));   /* 5 * 2 = 10 */
    TEST_ASSERT_EQUAL(50L, roman_value_extracted(3));   /* 10 * 5 = 50 */
    TEST_ASSERT_EQUAL(100L, roman_value_extracted(4));  /* 50 * 2 = 100 */
}

void test_roman_value_pattern_validation(void) {
    /* Test the alternating 5, 2, 5, 2 pattern */
    TEST_ASSERT_EQUAL(500L, roman_value_extracted(5));    /* 100 * 5 = 500 */
    TEST_ASSERT_EQUAL(1000L, roman_value_extracted(6));   /* 500 * 2 = 1000 */
    TEST_ASSERT_EQUAL(5000L, roman_value_extracted(7));   /* 1000 * 5 = 5000 */
}

void test_roman_base_small_values(void) {
    TEST_ASSERT_EQUAL(0, roman_base_extracted(1));  /* Value 1: base 0 (I) */
    TEST_ASSERT_EQUAL(0, roman_base_extracted(3));  /* Value 3: base 0 (III) */
    TEST_ASSERT_EQUAL(1, roman_base_extracted(4));  /* Value 4: base 1 (IV) */
    TEST_ASSERT_EQUAL(1, roman_base_extracted(5));  /* Value 5: base 1 (V) */
    TEST_ASSERT_EQUAL(1, roman_base_extracted(8));  /* Value 8: base 1 (VIII) */
}

void test_roman_base_larger_values(void) {
    TEST_ASSERT_EQUAL(2, roman_base_extracted(9));   /* Value 9: base 2 (IX) */
    TEST_ASSERT_EQUAL(2, roman_base_extracted(10));  /* Value 10: base 2 (X) */
    TEST_ASSERT_EQUAL(3, roman_base_extracted(40));  /* Value 40: base 3 (XL) */
    TEST_ASSERT_EQUAL(3, roman_base_extracted(50));  /* Value 50: base 3 (L) */
    TEST_ASSERT_EQUAL(4, roman_base_extracted(90));  /* Value 90: base 4 (XC) */
    TEST_ASSERT_EQUAL(4, roman_base_extracted(100)); /* Value 100: base 4 (C) */
}

void test_roman_base_edge_cases(void) {
    TEST_ASSERT_EQUAL(0, roman_base_extracted(0));   /* Zero case */
    TEST_ASSERT_EQUAL(5, roman_base_extracted(400)); /* Value 400: base 5 (CD) */
    TEST_ASSERT_EQUAL(5, roman_base_extracted(500)); /* Value 500: base 5 (D) */
}

/* ============================================================================
 * STRING COMPLETION TESTS
 * ============================================================================ */

void test_completion_exact_match(void) {
    TEST_ASSERT_EQUAL(TRUE, completion_extracted("test", "test"));
}

void test_completion_prefix_match(void) {
    TEST_ASSERT_EQUAL(TRUE, completion_extracted("hel", "hello"));
    TEST_ASSERT_EQUAL(TRUE, completion_extracted("t", "test"));
    TEST_ASSERT_EQUAL(TRUE, completion_extracted("", "anything")); /* Empty prefix matches all */
}

void test_completion_case_insensitive(void) {
    TEST_ASSERT_EQUAL(TRUE, completion_extracted("hel", "HELLO"));
    TEST_ASSERT_EQUAL(TRUE, completion_extracted("HEL", "hello"));
    TEST_ASSERT_EQUAL(TRUE, completion_extracted("MiX", "MixedCase"));
}

void test_completion_no_match(void) {
    TEST_ASSERT_EQUAL(FALSE, completion_extracted("hello", "help"));
    TEST_ASSERT_EQUAL(FALSE, completion_extracted("xyz", "abc"));
}

void test_completion_prefix_longer_than_target(void) {
    TEST_ASSERT_EQUAL(FALSE, completion_extracted("longer", "short"));
}

void test_completion_empty_strings(void) {
    TEST_ASSERT_EQUAL(TRUE, completion_extracted("", ""));    /* Both empty */
    TEST_ASSERT_EQUAL(TRUE, completion_extracted("", "test")); /* Empty prefix */
}

void test_completion_identical_length_mismatch(void) {
    TEST_ASSERT_EQUAL(FALSE, completion_extracted("abcd", "abce"));
}

/* ============================================================================
 * ECONOMIC CALCULATION TESTS (poptoworkers)
 * ============================================================================ */

void test_poptoworkers_small_populations(void) {
    TEST_ASSERT_EQUAL(1000L, poptoworkers_extracted(1000L));  /* Under threshold: 1:1 ratio */
    TEST_ASSERT_EQUAL(4999L, poptoworkers_extracted(4999L));  /* Just under threshold */
    TEST_ASSERT_EQUAL(0L, poptoworkers_extracted(0L));        /* Zero population */
}

void test_poptoworkers_threshold_boundary(void) {
    TEST_ASSERT_EQUAL(5000L, poptoworkers_extracted(5000L));  /* Exactly at threshold */
    TEST_ASSERT_EQUAL(5000L, poptoworkers_extracted(5001L));  /* Just over: 5000 + 1/2 = 5000 (integer division) */
}

void test_poptoworkers_efficiency_degradation(void) {
    /* Test diminishing returns algorithm */
    TEST_ASSERT_EQUAL(7500L, poptoworkers_extracted(10000L)); /* 5000 + 5000/2 = 7500 */
    TEST_ASSERT_EQUAL(9166L, poptoworkers_extracted(15000L)); /* 5000 + 2500 + 1666 = 9166 */
    TEST_ASSERT_EQUAL(10416L, poptoworkers_extracted(20000L)); /* 5000 + 2500 + 1666 + 1250 = 10416 */
}

void test_poptoworkers_maximum_population_cap(void) {
    /* Test that populations over ABSMAXPEOPLE are capped */
    long result_max = poptoworkers_extracted(ABSMAXPEOPLE);
    long result_over = poptoworkers_extracted(ABSMAXPEOPLE + 10000L);
    TEST_ASSERT_EQUAL(result_max, result_over); /* Should be identical due to capping */
}

void test_poptoworkers_large_population_calculation(void) {
    /* Test calculation for maximum allowed population */
    long expected = 0L;
    long pop = ABSMAXPEOPLE;
    int div = 1;

    /* Calculate expected result manually */
    while (pop > 0) {
        if (pop > TOOMANYPEOPLE) {
            expected += TOOMANYPEOPLE / div;
        } else {
            expected += pop / div;
        }
        pop -= TOOMANYPEOPLE;
        div++;
    }

    TEST_ASSERT_EQUAL(expected, poptoworkers_extracted(ABSMAXPEOPLE));
}

void test_poptoworkers_efficiency_pattern(void) {
    /* Verify the 1, 1/2, 1/3, 1/4 efficiency pattern */
    long pop_25k = poptoworkers_extracted(25000L); /* 5 segments */

    /* Manual calculation: 5000 + 2500 + 1666 + 1250 + 1000 = 11416 */
    long expected = 5000L + 2500L + 1666L + 1250L + 1000L;
    TEST_ASSERT_EQUAL(expected, pop_25k);
}

/* ============================================================================
 * MAIN TEST RUNNER
 * ============================================================================ */

int main(void) {
    UNITY_BEGIN();

    /* Character search tests */
    RUN_TEST(test_char_in_str_character_found_beginning);
    RUN_TEST(test_char_in_str_character_found_middle);
    RUN_TEST(test_char_in_str_character_found_end);
    RUN_TEST(test_char_in_str_character_not_found);
    RUN_TEST(test_char_in_str_empty_string);
    RUN_TEST(test_char_in_str_single_character_match);
    RUN_TEST(test_char_in_str_single_character_no_match);
    RUN_TEST(test_char_in_str_special_characters);

    /* Character conversion tests */
    RUN_TEST(test_non_cntrl_printable_characters_unchanged);
    RUN_TEST(test_non_cntrl_control_characters_converted);
    RUN_TEST(test_non_cntrl_delete_character_special_case);
    RUN_TEST(test_to_cntrl_normal_characters);
    RUN_TEST(test_to_cntrl_question_mark_special_case);
    RUN_TEST(test_non_cntrl_to_cntrl_symmetry);
    RUN_TEST(test_to_cntrl_non_cntrl_symmetry_special_cases);

    /* Roman numeral calculation tests */
    RUN_TEST(test_roman_value_base_cases);
    RUN_TEST(test_roman_value_pattern_validation);
    RUN_TEST(test_roman_base_small_values);
    RUN_TEST(test_roman_base_larger_values);
    RUN_TEST(test_roman_base_edge_cases);

    /* String completion tests */
    RUN_TEST(test_completion_exact_match);
    RUN_TEST(test_completion_prefix_match);
    RUN_TEST(test_completion_case_insensitive);
    RUN_TEST(test_completion_no_match);
    RUN_TEST(test_completion_prefix_longer_than_target);
    RUN_TEST(test_completion_empty_strings);
    RUN_TEST(test_completion_identical_length_mismatch);

    /* Economic calculation tests */
    RUN_TEST(test_poptoworkers_small_populations);
    RUN_TEST(test_poptoworkers_threshold_boundary);
    RUN_TEST(test_poptoworkers_efficiency_degradation);
    RUN_TEST(test_poptoworkers_maximum_population_cap);
    RUN_TEST(test_poptoworkers_large_population_calculation);
    RUN_TEST(test_poptoworkers_efficiency_pattern);

    return UNITY_END();
}