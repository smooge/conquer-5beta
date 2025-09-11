/*
 * miscX.c - Miscellaneous utility functions for Conquer game system
 *
 * This file contains shared utility functions used by both the administrative
 * engine (A modules) and the graphical interface (G modules). It provides
 * essential support functions including string manipulation, random number
 * generation, time handling, nation validation, resource selection, bit
 * manipulation, and leader generation.
 *
 * Key Function Categories:
 * - String Utilities: Case-insensitive string comparison functions
 * - Random Number System: Deterministic and standard random number generation
 * - Time Functions: System time retrieval and formatting
 * - Nation Validation: Nation mark character validation and conflict checking
 * - Resource Selection: Weighted random tradegood selection algorithms
 * - Bit Operations: Bit counting and manipulation utilities
 * - Leader Generation: Automated spellcaster creation for nations
 *
 * The random number system supports both standard random generation and
 * deterministic "non-random" sequences for reproducible game scenarios.
 * Nation mark validation ensures unique character assignments across all
 * game display elements. Tradegood selection implements weighted probability
 * distributions for realistic economic simulation.
 */
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * A good deal of time and effort has gone into the writing of this
 * code and it is our hope that you respect this.  We give permission
 * to alter the code, but not to redistribute modified versions of the
 * code without our explicit permission.  If you do modify the code,
 * please document the changes made, and send us a copy, so that all
 * people may have it.  The code, to the best of our knowledge, works
 * well, but there will probably always be a need for bug fixes and
 * improvements.  We disclaim any responsibility for the codes'
 * actions.  [Use at your own risk].  This notice is just our way of
 * saying, "Happy gaming!", while making an effort to not get sued in
 * the process.
 *                           Ed Barlow, Adam Bryant
 */
#include "dataX.h"
#include "armyX.h"
#include "desigX.h"
#include "racesX.h"
#include "elevegX.h"
#include "nclassX.h"
#include "tgoodsX.h"
#include "displayX.h"
#include <time.h>

/*
 * str_test - Case-insensitive string comparison function
 *
 * Performs a case-insensitive comparison of two null-terminated strings,
 * similar to strcmp() but ignoring case differences. Converts lowercase
 * characters to uppercase for comparison while preserving the original
 * strings unchanged.
 *
 * Parameters:
 *   s1 - First string to compare (must not be NULL)
 *   s2 - Second string to compare (must not be NULL)
 *
 * Returns:
 *   0 if strings are equal (ignoring case)
 *   < 0 if s1 is lexicographically less than s2
 *   > 0 if s1 is lexicographically greater than s2
 *
 * Notes:
 *   - Uses toupper() conversion for case folding
 *   - Assumes valid C locale for character classification
 *   - Behavior is undefined if either string is NULL
 */
int
str_test PARM_2(char *, s1, char *, s2)
{
  int ch1, ch2;

  for (; *s1 != '\0'; s1++, s2++) {
    ch1 = (islower(*s1) ? toupper(*s1) : *s1);
    ch2 = (islower(*s2) ? toupper(*s2) : *s2);
    if (ch1 != ch2) return(ch1 - ch2);
  }
  return(*s1 - *s2);
}

/*
 * str_ntest - Case-insensitive bounded string comparison function
 *
 * Performs a case-insensitive comparison of up to 'num' characters from
 * two null-terminated strings, similar to strncmp() but ignoring case
 * differences. Stops comparison at null terminator or after 'num' characters,
 * whichever comes first.
 *
 * Parameters:
 *   s1 - First string to compare (must not be NULL)
 *   s2 - Second string to compare (must not be NULL)
 *   num - Maximum number of characters to compare
 *
 * Returns:
 *   0 if first 'num' characters are equal (ignoring case) or if num characters compared
 *   < 0 if s1 is lexicographically less than s2 within comparison range
 *   > 0 if s1 is lexicographically greater than s2 within comparison range
 *
 * Notes:
 *   - Uses toupper() conversion for case folding
 *   - Returns 0 if exactly 'num' characters compared without difference
 *   - Stops early if null terminator encountered in either string
 */
int
str_ntest PARM_3(char *, s1, char *, s2, int, num)
{
  int ch1, ch2, count = 0;

  for (; *s1 != '\0' && count < num; s1++, s2++, count++) {
    ch1 = (islower(*s1) ? toupper(*s1) : *s1);
    ch2 = (islower(*s2) ? toupper(*s2) : *s2);
    if (ch1 != ch2) return(ch1 - ch2);
  }
  if (count == num) return(0);
  return(*s1 - *s2);
}

/* the variable used for a "nonrandom" random value */
long nrand_number;

/*
 * nrand_seed - Initialize deterministic random number sequence
 *
 * Seeds the non-random number generator with a multi-digit value computed
 * from the standard random number generator. This creates a deterministic
 * sequence that can be reproduced given the same initial random seed.
 * Used for creating consistent game scenarios that can be replayed.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Nothing
 *
 * Side Effects:
 *   - Sets global variable nrand_number to a multi-digit deterministic value
 *   - Requires prior seeding of standard random number generator
 *
 * Notes:
 *   - NRAND_DIGITS determines the precision of the deterministic sequence
 *   - Must call rand_seed() before calling this function
 *   - Enables reproducible "random" sequences for game testing
 */
void
nrand_seed PARM_0(void)
{
  int i, digit = 1;

  /* must have already seeded the real randomizor */
  nrand_number = 0;

  /* compute a random value of NRAND_DIGITS */
  for (i = 0; i < NRAND_DIGITS; i++) {
    nrand_number += (rand_val(10) * digit);
    digit *= 10;
  }
}

/*
 * nrand_val - Generate deterministic pseudo-random value
 *
 * Produces a deterministic "random" value using the seeded nrand_number
 * combined with an adjustment factor. This function always returns the
 * same result for the same limit and adjustment values, enabling
 * reproducible game scenarios and testing.
 *
 * Parameters:
 *   limit - Upper bound for random value (exclusive), must be > 0
 *   adj - Adjustment factor to vary the sequence without reseeding
 *
 * Returns:
 *   Deterministic value in range [0, limit) based on nrand_number + adj
 *
 * Notes:
 *   - Requires prior call to nrand_seed() to initialize nrand_number
 *   - Integer overflow in (nrand_number + adj) may be intentional for distribution
 *   - Used for creating consistent random events in replayed scenarios
 *   - The adj parameter allows variation without changing the base seed
 */
long
nrand_val PARM_2(long, limit, long, adj)
{
  /* overflow here might be desired */
  return((nrand_number + adj) % limit);
}

/*
 * rand_seed - Initialize standard random number generator
 *
 * Seeds the system's random number generator with the current system time
 * to ensure unpredictable random sequences. Supports multiple random number
 * implementations through conditional compilation, prioritizing higher-quality
 * generators when available.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Nothing
 *
 * Side Effects:
 *   - Seeds the system random number generator with current time
 *   - Affects subsequent calls to rand_val() function
 *
 * Notes:
 *   - LRAND48: Uses srand48() for high-quality 48-bit linear congruential generator
 *   - RANDOM: Uses srandom() for better distribution than standard rand()
 *   - Default: Falls back to standard srand() if other options unavailable
 *   - Should be called once at program startup before any random generation
 */
void
rand_seed PARM_0(void)
{
#ifdef LRAND48
  srand48((long) time ((long *) 0));
#else
#ifdef RANDOM
  srandom((unsigned) time ((long *) 0));
#else
  srand((unsigned) time ((long *) 0));
#endif /* RANDOM */
#endif /* LRAND48 */
}

/*
 * rand_val - Generate random value in specified range
 *
 * Generates a random integer value in the range [0, limit) using the
 * system's random number generator. The implementation varies based on
 * available random number functions, prioritizing higher-quality generators
 * for better distribution properties.
 *
 * Parameters:
 *   limit - Upper bound for random value (exclusive), must be > 0
 *
 * Returns:
 *   Random integer value in range [0, limit)
 *
 * Notes:
 *   - LRAND48: Uses lrand48() for high-quality 48-bit generator
 *   - RANDOM: Uses random() for better distribution than standard rand()
 *   - Default: Uses rand() divided by 102 to improve distribution
 *   - The division by 102 in default case reduces correlation in lower bits
 *   - Requires prior call to rand_seed() for proper initialization
 */
long
rand_val PARM_1(int, limit)
{
#ifdef LRAND48
  return(lrand48() % limit);
#else
#ifdef RANDOM
  return(random() % limit);
#else
  return((rand() / 102)  % limit);
#endif /* RANDOM */
#endif /* LRAND48 */
}

/*
 * mach_time - Get current system time as formatted string
 *
 * Retrieves the current system time and returns it as a human-readable
 * string in the standard C library format. The returned string includes
 * day of week, month, day, time, and year.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to static string containing formatted current time
 *   Format: "Wed Jun 30 21:49:08 1993\n"
 *
 * Notes:
 *   - Returns pointer to static memory managed by ctime()
 *   - String is overwritten on subsequent calls to ctime()
 *   - Includes trailing newline character
 *   - Time is local time, not UTC
 *   - Used for timestamping game events and log entries
 */
char *
mach_time PARM_0(void)
{
  long timeval;

  /* first get the time */
  timeval = time(0);

  /* now find the string using ctime */
  return(ctime(&timeval));
}

/*
 * markok - Validate nation mark character for uniqueness and compatibility
 *
 * Validates that a proposed nation mark character is suitable for use by
 * checking that it is alphabetic and not already in use by game display
 * elements or other nations of the same race. Ensures visual distinctness
 * on the game map by preventing conflicts with terrain, elevation, and
 * designation symbols.
 *
 * Parameters:
 *   mark - Character to validate as nation mark
 *   racetype - Race type for uniqueness checking within race
 *   showwhy - If TRUE, display error message explaining rejection reason
 *
 * Returns:
 *   TRUE if mark is valid and available for use
 *   FALSE if mark conflicts with existing symbols or nations
 *
 * Side Effects:
 *   - May display error message via errormsg() if showwhy is TRUE
 *
 * Notes:
 *   - Checks against elevation symbols (ele_info[].symbol)
 *   - Checks against designation symbols (maj_dinfo[].symbol)
 *   - Checks against vegetation symbols (veg_info[].symbol)
 *   - Ensures uniqueness among nations of same race
 *   - Essential for map readability and player identification
 */
int
markok PARM_3( int, mark, int, racetype, int, showwhy )
{
  NTN_PTR n1_ptr;
  register int i;
  char tmpstr[LINELTH];

  /* only alphabet characters must be used */
  if (!isalpha(mark)) {
    if (showwhy) {
      sprintf(tmpstr, "%c is not an alpha character", mark);
      errormsg(tmpstr);
    }
    return(FALSE);
  }

  /* now make sure it is not used for other displays */
  for (i = 0; i < ELE_NUMBER; i++) {
    if (mark == ele_info[i].symbol) {
      if (showwhy) {
	sprintf(tmpstr, "%c is an elevation character", mark);
	errormsg(tmpstr);
      }
      return(FALSE);
    }
  }
  for (i = 0; i < MAJ_NUMBER; i++) {
    if (mark == maj_dinfo[i].symbol) {
      if (showwhy) {
	sprintf(tmpstr, "%c is a designation character", mark);
	errormsg(tmpstr);
      }
      return(FALSE);
    }
  }
  for (i = 0; i < VEG_NUMBER; i++) {
    if (mark == veg_info[i].symbol) {
      if (showwhy) {
	sprintf(tmpstr, "%c is a vegetation character", mark);
	errormsg(tmpstr);
      }
      return(FALSE);
    }
  }

  /* now check for those already in use */
  for (i = 0; i < MAXNTN; i++)
    if (i != country && ((n1_ptr = world.np[i]) != NULL)) {
      if ((n1_ptr->mark == mark) && (n1_ptr->race == racetype)) {
	if (showwhy) {
	  sprintf(tmpstr, "%c is already in use", mark);
	  errormsg(tmpstr);
	}
	return(FALSE);
      }
    }

  /* now it is okay */
  return(TRUE);
}

/*
 * rand_tgood - Select weighted random tradegood by class and minimum value
 *
 * Selects a random tradegood from a specified class with value at least
 * minval, using weighted probability based on tradegood rarity rates.
 * Caches selection data for efficiency when called repeatedly with the
 * same parameters. Implements realistic economic distribution where
 * common goods are selected more frequently than rare ones.
 *
 * Parameters:
 *   tg_class - Tradegood class to select from (food, luxury, etc.)
 *   minval - Minimum value threshold for tradegood selection
 *
 * Returns:
 *   Index of selected tradegood in tg_info[] array
 *   TG_NONE if no suitable tradegoods found
 *
 * Side Effects:
 *   - Allocates static memory for tradegood list on first call
 *   - Caches computation results for repeated calls with same parameters
 *   - May call errormsg() and abrt() on memory allocation failure
 *
 * Notes:
 *   - Uses tg_info[].rate for weighted probability distribution
 *   - Higher rate values increase selection probability
 *   - Optimization: maintains cache to avoid recomputation
 *   - Memory allocation failure is treated as fatal error
 */
int
rand_tgood PARM_2(int, tg_class, int, minval)
{
  static int sum_rates = 0, num_elems = 0;
  static int old_class = -1, old_minval = 0;
  static int *tg_list = NULL;
  int count, randnum, result = TG_NONE;

  /* check if it is initialized */
  if (tg_list == NULL) {
    if ((tg_list = (int *)malloc(sizeof(int) * tgoods_number)) == NULL) {
      errormsg("Serious Error: Memory allocation failure");
      abrt();
    }
  }

  /* keep the information around to speed up function */
  if ((old_class != tg_class) ||
      (old_minval != minval)) {
    /* fix settings */
    sum_rates = 0;
    num_elems = 0;
    old_class = tg_class;
    old_minval = minval;

    /* count 'em up and lay 'em down */
    for (count = 0; count < tgoods_number; count++) {
      /* check it */
      if (((int)tg_info[count].class == tg_class) &&
	  (tg_info[count].value >= minval)) {
	/* grab it */
	tg_list[num_elems++] = count;
	sum_rates += tg_info[count].rate;
      }
    }

  }

  /* was there anything that matched? */
  if ((num_elems == 0) ||
      (sum_rates == 0)) {
    return(result);
  }

  /* now get it */
  randnum = rand_val(sum_rates);
  for (count = 0; count < num_elems; count++) {
    /* search */
    randnum -= tg_info[tg_list[count]].rate;
    if (randnum <= 0) {
      result = tg_list[count];
      break;
    }
  }

  /* now leave */
  return(result);
}

/*
 * num_bits_on - Count number of set bits in a long integer
 *
 * Counts the number of bits set to 1 in a long integer value by examining
 * each bit position individually. Useful for analyzing bitmask flags and
 * determining the number of active options or features encoded in a
 * bit field.
 *
 * Parameters:
 *   lng_list - Long integer value to analyze for set bits
 *
 * Returns:
 *   Number of bits set to 1 in the input value (0-32)
 *
 * Notes:
 *   - Assumes 32-bit long integers (legacy assumption)
 *   - Uses bit shifting to test each position individually
 *   - Commonly used for flag analysis in game state management
 *   - May not be portable to 64-bit systems without modification
 */
int
num_bits_on PARM_1(long, lng_list)
{
  int i, count = 0;

  /* go through it */
  for (i = 0; i < 32; i++) {
    if ((1 << i) & lng_list) {
      count++;
    }
  }
  return(count);
}

/*
 * hl_targets - Determine if highlight style requires target parameter
 *
 * Checks whether a given highlight display style requires additional target
 * specification to function properly. Some highlighting methods need specific
 * values (like designation types or nation ownership) while others operate
 * on all applicable items without additional parameters.
 *
 * Parameters:
 *   style - Highlight style constant to check (HI_MINDESG, HI_OWN, etc.)
 *
 * Returns:
 *   TRUE if the highlight style requires a target value
 *   FALSE if the highlight style works without target specification
 *
 * Notes:
 *   - HI_MINDESG: Requires specific minor designation type
 *   - HI_MAJDESG: Requires specific major designation type
 *   - HI_OWN: Requires specific nation ownership
 *   - HI_TGOODS: Requires specific tradegood type
 *   - Used by display system to determine parameter requirements
 */
int
hl_targets PARM_1(int, style)
{
  switch (style) {
  case HI_MINDESG:
  case HI_MAJDESG:
  case HI_OWN:
  case HI_TGOODS:
    /* yep */
    return(TRUE);
    /*NOTREACHED*/
    break;
  default:
    /* nope */
    break;
  }
  return(FALSE);
}

/*
 * give_spellcasters - Generate magic-using leaders for the current nation
 *
 * Creates new magician units for the current nation based on racial
 * magical aptitude and leadership capacity. The generation probability
 * is modified by racial traits that affect magical ability, and reduced
 * if the nation already has spellcasting rulers. Magicians are placed
 * at the nation's capital with minimum strength.
 *
 * Parameters:
 *   None (operates on global ntn_ptr for current nation)
 *
 * Returns:
 *   Nothing
 *
 * Side Effects:
 *   - May create new army units at nation capital
 *   - Modifies global army_ptr if armies are created
 *   - Uses nation's total leader count (tleaders) for generation attempts
 *
 * Notes:
 *   - Base chance is 50% per leader slot
 *   - r_magicskill: +10% bonus for magically skilled races
 *   - r_wizardly: +15% bonus for naturally wizardly races
 *   - r_antimagic: -25% penalty for magic-resistant races
 *   - Existing spellcasting rulers: 50% chance reduction
 *   - Created magicians start with minimum strength and 75% movement
 */
void
give_spellcasters PARM_0(void)
{
  int count, chance = 50;

  /* determined chance for new leaders */
  if (r_magicskill(ntn_ptr->race)) {
    chance *= 110;
    chance /= 100;
  }
  if (r_wizardly(ntn_ptr->race)) {
    chance *= 115;
    chance /= 100;
  }
  if (r_antimagic(ntn_ptr->race)) {
    /* don't do well with magic */
    chance *= 75;
    chance /= 100;
  }
  if (a_castspells(unitbyname(nclass_list[ntn_ptr->class].rulertype))) {
    /* already have enough spell casters */
    chance /= 2;
  }

  /* add a few of them */
  for (count = 0; count < ntn_ptr->tleaders; count++) {
    if (rand_val(100) < chance) {
      if ((army_ptr = crt_army(unitbyname("Magician"))) != NULL) {
	ARMY_XLOC = ntn_ptr->capx;
	ARMY_YLOC = ntn_ptr->capy;
	ARMY_SIZE = ainfo_list[ARMY_TYPE].minsth;
	ARMY_MOVE = 75;
      }
    }
  }
}
