/*
 * convertX.c - Data Conversion and String Manipulation Utilities
 *
 * This module provides essential utility functions for data conversion, 
 * string manipulation, and formatting throughout the Conquer game system.
 * Contains routines for:
 * - String formatting and escaping for display and storage
 * - Roman numeral conversion system
 * - Character encoding/decoding for special characters and control codes
 * - Key binding string conversion for user interface
 * - String completion matching for command parsing
 * - Nation name resolution and lookup
 * - Coordinate transformation for relative mapping
 * - Population-to-workers economic calculations
 * - Magic power list formatting for spell systems
 *
 * These utilities are used extensively throughout the game for data
 * presentation, user input processing, and internal data transformation.
 * Many functions handle edge cases and provide safety checking for
 * robust operation in the multi-user environment.
 */
/* various routines to change one thing into another */
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
#include "magicX.h"
#include "keyvalsX.h"

/*
 * form_str - Create formatted string representation with escaping and quoting
 *
 * Converts an input string into a properly formatted output string with
 * appropriate escaping, quoting, and control character handling. Used for
 * displaying strings safely in the user interface and for debugging output.
 * Handles both compact and full formatting modes.
 *
 * Parameters:
 *   out_str - Output buffer to store formatted string (must be pre-allocated)
 *   in_str  - Input string to format (null-terminated)
 *   full    - Flag for formatting mode: TRUE=full mode with char separators, FALSE=compact
 *
 * Returns:
 *   void (results stored in out_str)
 *
 * Side Effects:
 *   - Modifies out_str buffer with formatted string
 *   - Buffer overrun possible if out_str insufficient size
 *   - No bounds checking performed on output buffer
 *
 * Notes:
 *   - Control characters converted to ^X notation (e.g., ^A for ASCII 1)
 *   - Special characters '^', '\'', '"', '\' are escaped with backslash
 *   - Full mode adds quotes around each character and separates with dashes
 *   - Empty string becomes "'^@'" (null character representation)
 *   - Single character gets single quotes, multiple chars get double quotes
 *   - Used for key binding display and string debugging
 */
void
form_str PARM_3(char *, out_str, char *, in_str, int, full)
{
  int o_cnt = 1, i_cnt, ch;

  /* traverse the input string */
  for (i_cnt = 0; in_str[i_cnt] != '\0'; i_cnt++) {

    /* build it properly */
    ch = in_str[i_cnt];
    if (!isprint(ch)) {
      out_str[o_cnt++] = '^';
      out_str[o_cnt++] = non_cntrl(ch);
    } else {
      if (full) out_str[o_cnt++] = '\'';
      if ((ch == '^') || (ch == '\'') ||
	  (ch == '\"') || (ch == '\\')) {
	out_str[o_cnt++] = '\\';
      }
      out_str[o_cnt++] = ch;
      if (full) out_str[o_cnt++] = '\'';
    }

    /* add in spacing for full build */
    if (full) {
      out_str[o_cnt++] = '-';
    }
  }

  /* form the border of the string */
  if (i_cnt == 0) {
    strcpy(out_str, "'^@'");
  } else if (i_cnt == 1) {
    out_str[0] = '\'';
    out_str[o_cnt++] = '\'';
    out_str[o_cnt] = '\0';
  } else {
    out_str[0] = '"';
    out_str[o_cnt++] = '"';
    out_str[o_cnt] = '\0';
  }
}

/*
 * list_magics - Format magic power list into comma-separated string
 *
 * Converts a bitmask of magic powers into a human-readable string listing
 * the names of all active powers for a given magic class. Used for displaying
 * spell information, unit capabilities, and magic system status.
 *
 * Parameters:
 *   out_str - Output buffer for formatted power names (must be pre-allocated)
 *   mcls    - Magic class index (0 to MAG_NUMBER-1)
 *   powlist - Bitmask representing active powers (bit positions correspond to powers)
 *
 * Returns:
 *   Number of powers found and added to the string (0 if none or error)
 *
 * Side Effects:
 *   - Modifies out_str with comma-separated list of power names
 *   - Sets out_str to empty string if no powers or invalid input
 *   - No bounds checking on output buffer size
 *
 * Notes:
 *   - Powers separated by ", " for readable display
 *   - Validates magic class bounds before processing
 *   - Returns early with 0 if powlist is empty (0L)
 *   - Uses mclass_list global structure for power name lookup
 *   - Thread-safe if mclass_list is read-only during execution
 */
int
list_magics PARM_3(char *, out_str, int, mcls, long, powlist)
{
  int i, num = 0;

  /* check input */
  if ((out_str == NULL) ||
      (mcls < 0) ||
      (mcls >= MAG_NUMBER)) {
    return(num);
  }

  /* check for a quick out */
  out_str[0] = '\0';
  if (powlist == 0L) {
    return(num);
  }

  /* scan all of the powers of the given class */
  for (i = 0; i < mclass_list[mcls].maxval; i++) {
    if ((1L << i) & powlist) {
      num++;
      if (num > 1) {
	strcat(out_str, ", ");
      }
      strcat(out_str, mclass_list[mcls].pow_list[i].name);
    }
  }

  /* done */
  return(num);
}

/*
 * roman_value - Calculate decimal value for roman numeral base position
 *
 * Internal helper function that computes the decimal value corresponding
 * to a specific position in the roman numeral system. Uses alternating
 * multiplication by 5 and 2 to generate the sequence: 1, 5, 10, 50, 100, 500...
 * This corresponds to roman numerals I, V, X, L, C, D, M...
 *
 * Parameters:
 *   value - Base position in roman numeral system (0=I, 1=V, 2=X, etc.)
 *
 * Returns:
 *   Decimal value for the specified roman numeral base position
 *
 * Side Effects:
 *   None (pure calculation function)
 *
 * Notes:
 *   - Static function, only used internally by roman numeral functions
 *   - Algorithm: start with 1, alternate multiply by 5 then 2
 *   - Position 0=1(I), 1=5(V), 2=10(X), 3=50(L), 4=100(C), 5=500(D), 6=1000(M)
 *   - No overflow checking for large values
 *   - Thread-safe (no global state)
 */
static long
roman_value PARM_1 (int, value)
{
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
 * roman_base - Find appropriate roman numeral base for given decimal value
 *
 * Determines the correct roman numeral base position to use when converting
 * a decimal number to roman numerals. Handles special cases like 4=>5, 9=>10,
 * 40=>50 where subtractive notation is used (IV, IX, XL).
 *
 * Parameters:
 *   value - Decimal number to find roman base for
 *
 * Returns:
 *   Base position index for roman numeral conversion (0=I, 1=V, 2=X, etc.)
 *
 * Side Effects:
 *   None (pure calculation function)
 *
 * Notes:
 *   - Algorithm finds largest roman base that doesn't exceed input value
 *   - Handles subtractive cases (4->5, 9->10, 40->50, 90->100, etc.)
 *   - Uses alternating pattern: base values are 4,5 then 9,10 then 40,50, etc.
 *   - Works with roman_value() to implement complete roman numeral system
 *   - Thread-safe (no global state)
 *   - No bounds checking for extremely large values
 */
int
roman_base PARM_1 (int, value)
{
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
 * roman_number - Convert decimal integer to roman numeral string
 *
 * Converts a decimal integer into its roman numeral representation using
 * standard roman numeral rules including subtractive notation (IV, IX, XL, etc.).
 * Handles negative numbers by appending " BC" suffix, and zero as "0".
 *
 * Parameters:
 *   str   - Output buffer for roman numeral string (must be pre-allocated)
 *   value - Decimal integer to convert (positive, negative, or zero)
 *
 * Returns:
 *   void (result stored in str)
 *
 * Side Effects:
 *   - Modifies str buffer with roman numeral representation
 *   - No bounds checking on output buffer size
 *   - Assumes str has sufficient space for result
 *
 * Notes:
 *   - Zero becomes "0" (not a true roman numeral)
 *   - Negative numbers get " BC" suffix (e.g., "V BC" for -5)
 *   - Uses rnumerals global array for character mapping
 *   - Implements subtractive notation correctly (IV not IIII)
 *   - Algorithm repeatedly finds largest base and subtracts
 *   - Works with roman_base() and roman_value() helper functions
 *   - Thread-safe if rnumerals array is read-only
 */
void
roman_number PARM_2 ( char *, str, int, value )
{
  int neg = FALSE, count = 0, base;
  long bval;

  if (value == 0) {
    strcpy(str, "0");
    return;
  }

  if (value < 0) {
    neg = TRUE;
    value = -value;
  }

  while (value > 0) {

    /* obtain the nearest base */
    base = roman_base( value );

    /* place the next numeral */
    if ((bval = roman_value(base)) <= value) {
      str[count++] = rnumerals[base];
      value -= bval;
    } else {
      str[count++] = rnumerals[base - ((base + 1) % 2 + 1)];
      str[count++] = rnumerals[base];
      value -= (bval - roman_value(base - ((base + 1) % 2 + 1)));
    }

  }

  /* end the string */
  str[count] = '\0';
  if (neg == TRUE) {
    /* why not? */
    strcat( str, " BC");
  }

}

/*
 * char_in_str - Check if character exists in string
 *
 * Simple utility function that searches for a specific character within
 * a null-terminated string. Returns TRUE if found, FALSE otherwise.
 * Used for character validation and string parsing operations.
 *
 * Parameters:
 *   ch  - Character to search for
 *   str - Null-terminated string to search in
 *
 * Returns:
 *   TRUE if character found in string, FALSE otherwise
 *
 * Side Effects:
 *   None (read-only operation)
 *
 * Notes:
 *   - Linear search through string characters
 *   - Case-sensitive matching
 *   - Performance: O(n) where n is string length
 *   - Thread-safe (no global state modification)
 *   - No bounds checking beyond null terminator
 *   - Commonly used for input validation and parsing
 */
int
char_in_str PARM_2( char, ch, char *, str)
{
  int i, l = strlen(str);
  
  for (i = 0; i < l; i++)
    if (ch == str[i]) return(TRUE);
  return(FALSE);
}

/*
 * non_cntrl - Convert control character to printable representation
 *
 * Converts control characters to their printable equivalents for display
 * purposes. Control characters (ASCII 0-31) are converted to their corresponding
 * printable characters by adding '@' (e.g., Ctrl-A becomes 'A'). Special
 * handling for DEL character (ASCII 127).
 *
 * Parameters:
 *   ch - Character to convert (may be control or printable)
 *
 * Returns:
 *   Printable character representation (original char if already printable)
 *
 * Side Effects:
 *   None (pure conversion function)
 *
 * Notes:
 *   - Maps control chars: 0x01->A, 0x02->B, etc. (ch + '@')
 *   - DEL character (0x7F) becomes '?' for display
 *   - Printable characters returned unchanged
 *   - Uses isprint() to determine if character needs conversion
 *   - Complementary function to to_cntrl()
 *   - Used in key binding display and string formatting
 *   - Thread-safe (no global state)
 */
int
non_cntrl PARM_1(int, ch)
{
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
 * to_cntrl - Convert printable character to control character
 *
 * Converts printable characters to their corresponding control character
 * equivalents. This is the inverse operation of non_cntrl(). Used for
 * processing key binding strings and control character input.
 *
 * Parameters:
 *   ch - Printable character to convert to control character
 *
 * Returns:
 *   Control character equivalent (ch - '@')
 *
 * Side Effects:
 *   None (pure conversion function)
 *
 * Notes:
 *   - Maps printable chars: 'A'->0x01, 'B'->0x02, etc. (ch - '@')
 *   - '?' character specifically maps to EXT_DEL (ASCII 127)
 *   - Assumes input is valid printable character
 *   - No validation of input character range
 *   - Complementary function to non_cntrl()
 *   - Used in key binding processing and command input
 *   - Thread-safe (no global state)
 *   - Results may not be valid control characters for all inputs
 */
int
to_cntrl PARM_1(int, ch)
{
  int hold;

  if (ch == '?') {
    hold = EXT_DEL;
  } else {
    hold = ch - '@';
  }
  return(hold);
}

/*
 * convert_kbind - Convert textual key binding string to actual key codes
 *
 * Parses and converts a textual key binding string containing escape sequences,
 * control characters, and special notations into the actual key codes they
 * represent. Handles C-style escape sequences, hexadecimal codes, octal codes,
 * and control character notation. Modifies the string in-place.
 *
 * Parameters:
 *   str - Key binding string to convert (modified in-place)
 *
 * Returns:
 *   void (string modified in-place)
 *
 * Side Effects:
 *   - Modifies input string in-place with converted key codes
 *   - String may become shorter after escape sequence processing
 *   - No bounds checking (safe since output <= input length)
 *
 * Notes:
 *   - Escape sequences: \n (newline), \r (CR), \t (tab), \b (backspace)
 *   - Hex sequences: \x## (two hex digits)
 *   - Octal sequences: \### (up to three octal digits)
 *   - Control notation: ^X becomes control character
 *   - Direct escaping: \c becomes character c
 *   - Used for processing user key binding configuration
 *   - Complex parsing with multiple format support
 *   - Thread-safe (operates only on input parameter)
 */
void
convert_kbind PARM_1(char *, str)
{
  int ch, out_cnt = 0, in_cnt = 0, num;

  /* traverse the string, and clean it up */
  do {
    if ((ch = str[in_cnt]) == '\\') {
      /* handle the normal C characters */
      switch (ch = str[++in_cnt]) {
      case 'n':
	/* newline */
	ch = '\n';
	break;
      case 'r':
	/* carriage return */
	ch = '\r';
	break;
      case 't':
	/* tab key */
	ch = '\t';
	break;
      case 'b':
	/* backspace */
	ch = '\b';
	break;
      case 'x':
	/* hexidecimal condition */
	ch = 0;
	num = 2;
	for (in_cnt++;
	     (num > 0) &&
	     (isalpha(str[in_cnt]) ||
	      isdigit(str[in_cnt]));
	     in_cnt++, num--) {
	  /* provide conversion */
	  if (isdigit(str[in_cnt])) {
	    ch = ch * 16 + (str[in_cnt] - '0');
	  } else {
	    ch = ch * 16 + (str[in_cnt] - 'a' + 10);
	  }
	}
	break;
      case '0':
	/* octal conversion */
	ch = 0;
	num = 3;
	do {
	  ch = ch * 8 + (str[in_cnt] - '0');
	} while (isdigit(str[in_cnt++]) && (--num > 0));

	/* don't skip the character */
	in_cnt--;
	break;
      default:
	/* direct quote */
	break;
      }
      str[out_cnt++] = ch;
    } else if (ch == '^') {
      /* control key */
      if (str[++in_cnt] != '\0') {
	str[out_cnt++] = to_cntrl(str[in_cnt]);
      }
    } else {
      str[out_cnt++] = ch;
    }
  } while (str[in_cnt++] != '\0');
}

/*
 * completion - Check if second string is valid completion of first
 *
 * Determines if the second string could be a completion of the first string,
 * similar to strncmp(s1, s2, strlen(s1)) but with case-insensitive matching.
 * Used for command completion and partial string matching in user interface.
 *
 * Parameters:
 *   s1 - Partial string (prefix to match)
 *   s2 - Complete string (potential completion)
 *
 * Returns:
 *   TRUE if s2 starts with s1 (case-insensitive), FALSE otherwise
 *
 * Side Effects:
 *   None (read-only comparison)
 *
 * Notes:
 *   - Case-insensitive comparison using toupper()
 *   - Returns FALSE immediately if s1 is longer than s2
 *   - Only compares characters up to length of s1
 *   - Used for command parsing and auto-completion features
 *   - Equivalent to strncasecmp(s1, s2, strlen(s1)) == 0
 *   - Thread-safe (no global state modification)
 *   - Performance: O(n) where n is length of s1
 */
int
completion PARM_2 (char *, s1, char *, s2)
{
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
 * ntn_realname - Resolve nation ID to proper nation name string
 *
 * Converts a nation identifier to the appropriate name string for display
 * or file operations. Handles special cases like newspaper, unowned territories,
 * and validates nation IDs. Provides error handling for invalid nation references.
 *
 * Parameters:
 *   outstr - Output buffer for nation name (must be pre-allocated)
 *   who    - Nation ID to resolve (or special constant like NEWSPAPER, UNOWNED)
 *
 * Returns:
 *   FALSE on success, TRUE on error (invalid nation ID)
 *
 * Side Effects:
 *   - Modifies outstr with resolved nation name
 *   - May call errormsg() for invalid nation IDs
 *   - No bounds checking on output buffer
 *
 * Notes:
 *   - NEWSPAPER constant -> newsfile global variable
 *   - UNOWNED constant -> "god" (deity/admin)
 *   - Current player uses nationname global if not in update mode
 *   - Validates nation ID bounds (0 to ABSMAXNTN)
 *   - Uses world.np[] array for nation lookup
 *   - Sets "bad-name" and returns TRUE for invalid IDs
 *   - Thread-safe if global variables are properly protected
 *   - Used throughout system for nation name display
 */
int
ntn_realname PARM_2(char *, outstr, int, who)
{
  switch (who) {
  case NEWSPAPER:
    /* sending to the newspaper */
    strcpy(outstr, newsfile);
    break;
  case UNOWNED:
    /* sending to the almighty one */
    strcpy(outstr, "god");
    break;
  default:
    /* a normal nation... */
    if (!is_update && (country == who)) {
      strcpy(outstr, nationname);
    } else if ((who < 0) || (who > ABSMAXNTN) ||
	       (ntn_tptr = world.np[who]) == NULL) {
      strcpy(outstr, "bad-name");
      errormsg("Error in ntn_realname(): Unknown nation asked for");
      return(TRUE);
    } else {
      strcpy(outstr, ntn_tptr->name);
    }
    break;
  }
  return(FALSE);
}

/*
 * trim_str - Remove leading/trailing whitespace and optional comments
 *
 * Removes leading and trailing whitespace from a string, with optional
 * comment removal. Comments are marked by '#' character and removed if
 * killpound flag is set. Modifies string in-place and returns final length.
 *
 * Parameters:
 *   str       - String to trim (modified in-place)
 *   killpound - Flag: TRUE to remove '#' comments, FALSE to preserve them
 *
 * Returns:
 *   Final length of trimmed string
 *
 * Side Effects:
 *   - Modifies input string in-place
 *   - String may become shorter after trimming
 *   - Null terminates the trimmed result
 *
 * Notes:
 *   - Removes leading whitespace (spaces, tabs, newlines, etc.)
 *   - Removes trailing whitespace from end of string
 *   - If killpound=TRUE, '#' at start (after leading spaces) terminates string
 *   - Algorithm: skip leading spaces, shift string, track last non-space
 *   - Used for configuration file parsing and input sanitization
 *   - Thread-safe (operates only on input parameter)
 *   - Efficient in-place processing with single pass
 */
int
trim_str PARM_2(char *, str, int, killpound)
{
  register int i = 0, j = 0;
  int last_space = -1;

  /* trim leading spaces */
  while (isspace(str[i])) i++;
  if ((str[i] == '#') &&
      (killpound == TRUE)) {
    str[i] = '\0';
  }

  /* shift string */
  while (1) {
    if ((str[j] = str[i++]) == '\0') {
      break;
    } else if (isspace(str[j])) {
      last_space = j;
    } else {
      last_space = -1;
    }
    j++;
  }

  /* now remove any trailing spaces */
  if (last_space != -1) {
    str[last_space] = '\0';
    j = last_space;
  }
  return(j);
}

/*
 * xloc_relative - Convert absolute X coordinate to relative coordinate
 *
 * Transforms an absolute X coordinate to a coordinate relative to the current
 * nation's center position. Handles world wrapping for cylindrical map topology.
 * Used for displaying coordinates relative to player's viewpoint.
 *
 * Parameters:
 *   given_xloc - Absolute X coordinate on world map
 *
 * Returns:
 *   X coordinate relative to current nation's center (or absolute if relative mode off)
 *
 * Side Effects:
 *   None (read-only calculation)
 *
 * Notes:
 *   - Only applies transformation if world.relative_map is TRUE
 *   - God mode (is_god=TRUE) bypasses relative transformation
 *   - Requires valid ntn_ptr for current nation
 *   - Handles world wrapping: if difference > MAPX/2, wraps around
 *   - World is cylindrical in X direction (wraps at edges)
 *   - Used for display coordinates in user interface
 *   - Thread-safe if global variables are properly protected
 *   - Returns unchanged coordinate if relative mode disabled
 */
int
xloc_relative PARM_1(int, given_xloc)
{
  int hold;

  if ((world.relative_map) &&
      (is_god == FALSE) &&
      (ntn_ptr != NULL)) {
    hold = given_xloc - ntn_ptr->centerx;
    if (hold > MAPX / 2) {
      hold -= MAPX;
    }
    if (hold <= - (MAPX / 2) ) {
      hold += MAPX;
    }
  } else {
    hold = given_xloc;
  }
  return(hold);
}

/*
 * yloc_relative - Convert absolute Y coordinate to relative coordinate
 *
 * Transforms an absolute Y coordinate to a coordinate relative to the current
 * nation's center position. Unlike X coordinates, Y coordinates do not wrap
 * since the world has finite north/south boundaries.
 *
 * Parameters:
 *   given_yloc - Absolute Y coordinate on world map
 *
 * Returns:
 *   Y coordinate relative to current nation's center (or absolute if relative mode off)
 *
 * Side Effects:
 *   None (read-only calculation)
 *
 * Notes:
 *   - Only applies transformation if world.relative_map is TRUE
 *   - God mode (is_god=TRUE) bypasses relative transformation
 *   - Requires valid ntn_ptr for current nation
 *   - No wrapping logic needed for Y coordinates (finite world height)
 *   - Simple subtraction: given_yloc - ntn_ptr->centery
 *   - Used for display coordinates in user interface
 *   - Companion function to xloc_relative()
 *   - Thread-safe if global variables are properly protected
 *   - Returns unchanged coordinate if relative mode disabled
 */
int
yloc_relative PARM_1(int, given_yloc)
{
  int hold;

  if ((world.relative_map) &&
      (is_god == FALSE) &&
      (ntn_ptr != NULL)) {
    hold = given_yloc - ntn_ptr->centery;
  } else {
    hold = given_yloc;
  }
  return(hold);
}

/*
 * poptoworkers - Calculate effective workforce from total population
 *
 * Converts total population into effective workforce using diminishing returns
 * algorithm. Large populations become less efficient due to overcrowding and
 * resource limitations. Implements game balance mechanic to prevent runaway
 * population growth from dominating gameplay.
 *
 * Parameters:
 *   population - Total population count to convert
 *
 * Returns:
 *   Effective workforce count (always <= population)
 *
 * Side Effects:
 *   None (pure calculation function)
 *
 * Notes:
 *   - Caps input at ABSMAXPEOPLE to prevent overflow
 *   - Uses TOOMANYPEOPLE threshold for diminishing returns
 *   - Algorithm: full efficiency up to TOOMANYPEOPLE, then 1/2, 1/3, 1/4, etc.
 *   - Each successive TOOMANYPEOPLE chunk has progressively lower efficiency
 *   - Game balance: prevents large nations from becoming overpowered
 *   - Used in economic calculations and production systems
 *   - Thread-safe (no global state modification)
 *   - Performance: O(population/TOOMANYPEOPLE) complexity
 */
long
poptoworkers PARM_1(long, population)
{
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
