/* file perusal program especially written for conquer */
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
#include "dataG.h"
#include "stringX.h"
#include "keyvalsX.h"

/* local variables */
#ifdef REGEXP
static int use_regexp;
#endif /* REGEXP */
static int exp_len, cur_line, marked_line;
static int max_line, old_line;

/* the search string */
static char search_str[BIGLTH];

/* the structure for the file */
LINE_STRUCT file_line[MAX_FILE_LINES];

#ifdef REGEXP
/* characters in a regular expression */
static char *regexp_list = ".*[]\\^$+?|";
#endif /* REGEXP */

/* The list of commands for the pager */
static char *pager_help[MAX_PAGERHELP] = {
  " 'q' - exit pager",
  " ' ' - scroll file forward N lines",
  " 'b' - scroll file backward N lines",
  " RET - scroll file forward 1 line",
  " DEL - scroll file backward 1 line",
  " 'g' - go to beginning of file",
  " 'G' - go to end of file",
  " '/' - search forward for regexp/string",
  " '?' - search backward for regexp/string",
  " 'n' - repeat last search forward",
  " 'N' - repeat last search backward",
  " 'p' - search for next page header",
  " 'P' - search for prior pager header",
  " 'x' - return to last marked position",
  " 'm' - mark the current screen position",
  " 's' - set scrolling distance N",
  " 'S' - set search displacement from top",
  " 't' - set tab width when displaying",
  " '=' - display pager status information",
  "  ^L - redraw screen after clearing",
  "  ^R - redraw screen without clearing"
};

/*
 * check_regexp - Analyze search string for regular expression patterns
 *
 * Examines the provided search string to determine if it contains regular
 * expression metacharacters. Sets the global regexp usage flag and stores
 * the expression length. This function enables the pager to automatically
 * switch between string matching and regex matching based on content.
 *
 * Parameters:
 *   str - Search string to analyze for regex metacharacters (must not be NULL)
 *
 * Returns:
 *   TRUE if string contains regex metacharacters, FALSE for literal strings
 *
 * Side Effects:
 *   - Sets global exp_len to string length
 *   - Sets global use_regexp flag when REGEXP is enabled
 *   - Prepares search engine for appropriate matching method
 *
 * Notes:
 *   - Regex support is conditional on REGEXP compilation flag
 *   - Metacharacters checked: . * [ ] \ ^ $ + ? |
 *   - Used by forward/backward search commands in main pager loop
 */
static int
check_regexp PARM_1( char *, str )
{
#ifdef REGEXP
  int i, l = strlen(regexp_list);
#endif /* REGEXP */

  /* store length of expression */
  exp_len = strlen(str);

#ifdef REGEXP
  /* now check if it is a regular expression */
  for (i = 0; i < l; i++) {
    if (char_in_str(regexp_list[i], str) == TRUE) {
      use_regexp = TRUE;
      return(TRUE);
    }
  }
  use_regexp = FALSE;
#endif /* REGEXP */
  return(FALSE);
}

/*
 * line_match - Test if a line matches the current search pattern
 *
 * Performs pattern matching against a single line using either regular
 * expression matching or optimized string searching based on the current
 * search mode. The function automatically selects the appropriate matching
 * algorithm based on the use_regexp flag set by check_regexp().
 *
 * Parameters:
 *   str - Line content to test against current search pattern (must not be NULL)
 *
 * Returns:
 *   TRUE if line matches current search pattern, FALSE otherwise
 *
 * Side Effects:
 *   - None (read-only operation)
 *
 * Notes:
 *   - Uses re_exec() for regex matching when REGEXP enabled and use_regexp is TRUE
 *   - Falls back to optimized str_ntest() for literal string matching
 *   - Searches from end of string backward for efficiency in string mode
 *   - Core matching engine used by pg_search() for forward/backward searches
 */
static int
line_match PARM_1(char *, str)
{
  register int i;
  int len_str;

#ifdef REGEXP
  /* determine searching method */
  if (use_regexp == TRUE) {
    /* use the standard regular expression functions */
    return(re_exec(str));
  }
#endif /* REGEXP */

  /* use quicker string comparison method */
  len_str = strlen(str);
  for (i = len_str - exp_len; i >= 0; i--) {
    if (str_ntest(search_str, &(str[i]), exp_len) == 0) {
      return(TRUE);
    }
  }
  return(FALSE);
}

/*
 * pg_cleanup - Release all allocated memory and reset pager state
 *
 * Performs comprehensive cleanup of the pager's memory allocations and state.
 * Frees all dynamically allocated line buffers from the file_line array and
 * resets highlight flags. This function is essential for preventing memory
 * leaks when exiting the pager.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Frees all allocated memory in file_line[].line arrays
 *   - Sets all file_line[].line pointers to NULL
 *   - Resets all file_line[].highlight flags to FALSE
 *   - Prepares pager for safe shutdown or reinitialization
 *
 * Notes:
 *   - Must be called before pager exit to prevent memory leaks
 *   - Safe to call multiple times (checks for NULL before freeing)
 *   - Processes entire MAX_FILE_LINES array for thorough cleanup
 */
static void
pg_cleanup PARM_0(void)
{
  register int i;

  /* clear out the "file" memory */
  for (i = 0; i < MAX_FILE_LINES; i++) {
    if (file_line[i].line != NULL) {
      free(file_line[i].line);
      file_line[i].line = NULL;
    }
    file_line[i].highlight = FALSE;
  }
}

/*
 * pg_init - Initialize pager state and validate configuration settings
 *
 * Performs comprehensive initialization of the pager system, setting up
 * default values for all state variables and validating user configuration
 * settings. Ensures the pager starts in a consistent, safe state with
 * reasonable defaults for all operational parameters.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Clears search_str and sets exp_len to 0
 *   - Initializes file_line array (NULL pointers, FALSE highlights)
 *   - Sets cur_line, max_line, old_line to 0
 *   - Sets marked_line to -1 (no mark)
 *   - Validates and constrains pager_offset (0 to 3/4 screen height)
 *   - Validates and constrains pager_scroll (1 to screen height - 1)
 *   - Resets use_regexp to FALSE when REGEXP enabled
 *
 * Notes:
 *   - Called once per file before reading and display
 *   - Ensures configuration values are within safe operational ranges
 *   - Prevents buffer overflows and display corruption from bad settings
 */
static void
pg_init PARM_0(void)
{
  register int i;

  /* no search string as yet */
  exp_len = 0;
  strcpy(search_str, "");

  /* clear out the "file" */
  for (i = 0; i < MAX_FILE_LINES; i++) {
    file_line[i].line = NULL;
    file_line[i].highlight = FALSE;
  }

  /* at the beginning of the file */
  cur_line = max_line = old_line = 0;
  marked_line = (-1);
  if (pager_offset < 0) {
    /* be sure of lower limit */
    pager_offset = 0;
  } else if (pager_offset > (LINES * 3) / 4) {
    /* keep the upper limit reasonable */
    pager_offset = (LINES * 3) / 4;
  }

  /* check the scroll settings */
  if ((pager_scroll <= 0) || (pager_scroll > LINES - 1)) {
    pager_scroll = LINES - 1;
  }

#ifdef REGEXP
  /* if we have it, use it */
  use_regexp = FALSE;
#endif /* REGEXP */
}

/*
 * pg_status - Display comprehensive pager status information
 *
 * Shows detailed status information about the current pager session including
 * current position, search state, and configuration settings. Provides users
 * with complete visibility into pager state for debugging and navigation.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Displays status line in standout mode at bottom of screen
 *   - Truncates display at screen width to prevent wrapping
 *   - Waits for user keypress before returning
 *   - Clears to end of line for clean display
 *
 * Notes:
 *   - Shows: bottom line number/total, search string, mark position, tab width, scroll distance
 *   - Bottom line calculated as: max(0, cur_line - pager_offset) + LINES - 1
 *   - Mark displayed as 0 when no mark set (marked_line == -1)
 *   - Activated by '=' command in main pager loop
 */
static void
pg_status PARM_0(void)
{
  int i;

  /* show what the bottom line is */
  i = max(0, cur_line - pager_offset) + LINES - 1;
  sprintf(string,
	  "bottom = %d/%d; search = \"%s\"; mark = %d; tab = %d; scroll = %d",
	  i, max_line, search_str,
	  (marked_line == -1) ? 0 : marked_line,
	  pager_tab, pager_scroll);
  string[COLS - 1] = '\0';
  standout();
  mvaddstr(LINES - 1, 0, string);
  standend();
  clrtoeol();

  /* now ask for input */
  mvaddstr(LINES - 1, COLS - 11, "-ANY KEY-");
  refresh();

  next_char();
}

/*
 * pg_bottom - Display command prompt and help summary at screen bottom
 *
 * Provides a concise command summary and navigation prompt at the bottom of
 * the screen during normal pager operation. Shows the most essential commands
 * for quick reference without requiring the full help display.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Clears bottom line of screen
 *   - Displays centered standout help text
 *   - Positions cursor at pager_offset for continued display
 *   - Refreshes screen to show changes
 *
 * Notes:
 *   - Shows essential commands: 'q' exit, ' ' advance, '/' '?' search, 'h' help
 *   - Text is centered horizontally for better visual appeal
 *   - Called after each command to maintain consistent interface
 *   - Cursor positioned at pager_offset for proper text display
 */
static void
pg_bottom PARM_0(void)
{
  /* provide a quick help listing */
  clear_bottom(1);
  sprintf(string,
	  "'q' to exit; ' ' to advance; '/' or '?' to search; 'h' for help");
  standout();
  mvaddstr(LINES - 1, (COLS - strlen(string)) / 2, string);
  standend();
  move(pager_offset, 0);
  refresh();
}

/*
 * pg_setcurrent - Set current line position with bounds checking
 *
 * Updates the current line position with comprehensive bounds checking to ensure
 * the display remains valid. Handles edge cases for small files and maintains
 * proper relationship between current line and display offset.
 *
 * Parameters:
 *   to_what - Target line number to set as current position
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Updates cur_line with bounds-checked value
 *   - Ensures cur_line stays within valid display range
 *   - Maintains proper offset relationship for scrolling
 *
 * Notes:
 *   - For small files (< LINES-1): sets cur_line to 0
 *   - Enforces minimum: cur_line >= pager_offset
 *   - Enforces maximum: cur_line <= max_line + pager_offset - LINES + 1
 *   - Core positioning function used by all navigation commands
 */
static void
pg_setcurrent PARM_1(int, to_what)
{
  /* simply do it */
  cur_line = to_what;
  if (max_line < LINES - 1) {
    cur_line = 0;
  } else {
    if (cur_line < pager_offset) {
      cur_line = pager_offset;
    } else if ((cur_line > pager_offset) &&
	       (cur_line - pager_offset > max_line - LINES + 1)) {
      cur_line = max_line + pager_offset - LINES + 1;
    }
  }
}

/*
 * pg_saveline - Save current line position for restoration
 *
 * Stores the current line position in old_line for later restoration.
 * Used to implement position memory for operations that might fail or
 * be cancelled, allowing the pager to return to the previous position.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Sets old_line to current cur_line value
 *
 * Notes:
 *   - Used before search operations that might fail
 *   - Paired with pg_restore() for rollback functionality
 *   - Simple state preservation for navigation safety
 */
static void
pg_saveline PARM_0(void)
{
  old_line = cur_line;
}

/*
 * pg_restore - Restore previously saved line position
 *
 * Restores the line position saved by pg_saveline(), effectively undoing
 * the last position change. Used when search operations fail or when the
 * user wants to return to a previous position.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Sets cur_line to previously saved old_line value
 *
 * Notes:
 *   - Paired with pg_saveline() for position rollback
 *   - Used when searches fail to return to starting position
 *   - Simple undo mechanism for navigation operations
 */
static void
pg_restore PARM_0(void)
{
  cur_line = old_line;
}

/*
 * pg_lineout - Display a single line with appropriate formatting
 *
 * Renders a single line from the file buffer to the screen at the specified
 * location, applying appropriate highlighting and formatting based on line
 * type and user preferences. Handles headers, gaudy mode, and normal text.
 *
 * Parameters:
 *   d_line - Index into file_line array for source line
 *   loc - Screen line number where content should be displayed
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Displays line content at specified screen location
 *   - Applies standout mode for highlighted header lines
 *   - Uses gaudy_lineout() for nation name highlighting when enabled
 *   - Clears to end of line for clean display
 *
 * Notes:
 *   - Headers centered horizontally and displayed in standout mode
 *   - Gaudy mode highlights nation names within text
 *   - Normal mode uses standard text display
 *   - Automatically cleans up line edges with clrtoeol()
 */
static void
pg_lineout PARM_2(int, d_line, int, loc)
{
  if (file_line[d_line].highlight) {
    /* easiest to assume headers are properly formatted */
    strcpy(string, file_line[d_line].line);
    move(loc, 0);
    clrtoeol();
    standout();
    mvaddstr(loc, (COLS - strlen(string)) / 2, string);
    standend();
  } else if (conq_gaudy == TRUE) {
    /* highlight nation name occurances */
    gaudy_lineout(loc, file_line[d_line].line);
  } else {
    /* just display the string */
    normal_lineout(loc, file_line[d_line].line);
  }

  /* clean up edges */
  clrtoeol();
}

/*
 * pg_redraw - Redraw entire screen based on current line position
 *
 * Redraws the complete screen display based on the current line position
 * and pager offset. Calculates the visible range and renders all lines
 * within that range, filling remaining space with tilde markers.
 *
 * Parameters:
 *   full_clr - TRUE for complete screen clear, FALSE for partial refresh
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Clears screen (full or partial based on full_clr parameter)
 *   - Redraws all visible lines in current display window
 *   - Fills empty lines with '~' characters (vi-style)
 *   - Updates entire screen display
 *
 * Notes:
 *   - Display range: max(0, cur_line - pager_offset) to min(start + LINES - 1, max_line)
 *   - Used for major navigation changes and screen refresh commands
 *   - Tilde markers indicate end of file or empty space
 *   - More expensive than scrolling for small position changes
 */
static void
pg_redraw PARM_1(int, full_clr)
{
  int count, start, finish;

  /* redraw entire screen? */
  if (full_clr == TRUE) {
    clear();
  } else {
    move(0, 0);
    clrtobot();
  }

  /* find range of drawing */
  start = max(0, cur_line - pager_offset);
  finish = min(start + LINES - 1, max_line);

  /* go to it */
  for (count = start; count < finish; count++) {
    pg_lineout(count, count - start);
  }
  for (count -= start; count < LINES - 1; count++) {
    mvaddch(count, 0, '~');
  }
}

/*
 * pg_scroll - Efficiently scroll display to current line position
 *
 * Performs optimized scrolling to move the display from old_line to cur_line
 * position. Uses hardware scrolling when possible for smooth movement and
 * falls back to redraw for major position changes.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Enables scrollok() for hardware scrolling support
 *   - Scrolls screen up/down as needed to reach target position
 *   - Updates old_line to track actual position during scrolling
 *   - May fall back to pg_redraw() if scrolling causes display issues
 *
 * Notes:
 *   - Scrolls up: adds new lines at bottom while moving window up
 *   - Scrolls down: inserts lines at top for backward movement
 *   - More efficient than redraw for small position changes
 *   - Automatically handles both forward and backward scrolling
 */
static void
pg_scroll PARM_0(void)
{
  int did_scroll = FALSE, next_line = 0;

  /* scroll up */
  scrollok(curscr, TRUE);
  while (old_line < cur_line) {

    /* scroll up next line */
    next_line = max(0, old_line - pager_offset) + LINES - 1;
    pg_lineout(next_line, LINES - 1);
    refresh();

    /* move window up one line */
    scroll(curscr);
    refresh();

    /* keep going */
    did_scroll = TRUE;
    old_line++;

  }
  scrollok(curscr, FALSE);
  if (did_scroll == TRUE) {
    /* fix any stupid glitches */
    pg_redraw(FALSE);
  }

  /* scroll down */
  while (old_line > cur_line) {

    /* provide space */
    move(0, 0);
    insertln();

    /* add the next line */
    next_line = --old_line - pager_offset;
    pg_lineout(next_line, 0);

  }
}

/*
 * pg_help - Display comprehensive command help screen
 *
 * Shows the complete list of pager commands with descriptions, providing
 * users with full documentation of available functionality. Displays help
 * with scrolling effect and waits for user acknowledgment.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Overwrites current screen with help text
 *   - Displays all commands from pager_help array
 *   - Shows commands with nice scrolling visual effect
 *   - Waits for keypress before returning to normal display
 *   - Restores original screen content after help viewing
 *
 * Notes:
 *   - Shows 21 different pager commands and their functions
 *   - Help includes navigation, search, marking, and configuration commands
 *   - Scrolling effect provides visual feedback during display
 *   - Automatically restores previous display with pg_redraw()
 */
static void
pg_help PARM_0(void)
{
  int i, j, len;

  /* show all of the help lines */
  for (i = 0; i < MAX_PAGERHELP; i++) {
    mvaddstr(i, 0, pager_help[i]);
    len = strlen(pager_help[i]);
    for (j = len; j < WIDTH_PAGERHELP; j++) {
      addch(' ');
    }

    /* nice scrolling effect */
    refresh();
  }
  standout();
  mvaddstr(i, 0, "-- Hit Any Key --");
  standend();
  for (j = 17; j < WIDTH_PAGERHELP; j++) {
    addch(' ');
  }
  move(i, 17);
  refresh();
  next_char();

  /* replace screen properly */
  pg_redraw(FALSE);
}

/*
 * pg_search - Search for pattern in specified direction
 *
 * Performs forward or backward search through the file content using the
 * current search pattern. Searches line by line from current position until
 * a match is found or file boundaries are reached.
 *
 * Parameters:
 *   dir - Search direction: 1 for forward, -1 for backward
 *
 * Returns:
 *   Line number of match if found, -1 if no match or error
 *
 * Side Effects:
 *   - None (read-only search operation)
 *
 * Notes:
 *   - Validates direction parameter for safety
 *   - Uses line_match() for actual pattern matching
 *   - Searches from cur_line + dir to avoid matching current line
 *   - Respects file boundaries (0 to max_line)
 *   - Core search engine used by 'n', 'N', '/', and '?' commands
 */
static int
pg_search PARM_1(int, dir)
{
  register int lnum;

  /* check the input */
  if ((dir != (-1)) && (dir != 1)) {
    errormsg("Internal search error... please help me");
    return(-1);
  }

  /* now perform the search */
  lnum = cur_line + dir;
  while ((lnum >= 0) && (lnum < max_line)) {

    /* I've been searchin... */
    if (line_match(file_line[lnum].line) == TRUE) {
      return(lnum);
    }
    lnum += dir;

  }

  /* no value found */
  return(-1);
}

/*
 * pg_header - Search for highlighted header lines in specified direction
 *
 * Searches for lines marked with the highlight flag, which typically
 * represent page headers or section dividers. Provides quick navigation
 * between major sections of the document.
 *
 * Parameters:
 *   dir - Search direction: 1 for forward, -1 for backward
 *
 * Returns:
 *   Line number of next header if found, -1 if no header found or error
 *
 * Side Effects:
 *   - None (read-only search operation)
 *
 * Notes:
 *   - Validates direction parameter for safety
 *   - Searches for file_line[].highlight == TRUE
 *   - Used by 'p' (forward) and 'P' (backward) commands
 *   - Enables quick navigation between document sections
 *   - Headers are typically centered and displayed in standout mode
 */
static int
pg_header PARM_1(int, dir)
{
  register int lnum;

  /* check the input */
  if ((dir != (-1)) && (dir != 1)) {
    errormsg("Internal search error... please help me");
    return(-1);
  }

  /* now perform the search */
  lnum = cur_line + dir;
  while ((lnum >= 0) && (lnum < max_line)) {

    /* I've been searchin... */
    if (file_line[lnum].highlight) {
      return(lnum);
    }
    lnum += dir;

  }

  /* no value found */
  return(-1);
}

/*
 * pg_convert - Process input string removing backspace characters
 *
 * Processes an input string to handle embedded backspace characters,
 * effectively removing characters that would be "erased" by backspaces.
 * This handles text formatting artifacts that might appear in input files.
 *
 * Parameters:
 *   o_str - Output buffer for processed string (must be large enough)
 *   i_str - Input string to process (must not be NULL)
 *
 * Returns:
 *   Length of processed output string
 *
 * Side Effects:
 *   - Modifies o_str with processed content
 *   - Null-terminates output string
 *
 * Notes:
 *   - Processes backspaces by reducing output count (removing previous char)
 *   - Handles embedded control characters that affect text display
 *   - Used during file reading to clean up line content
 *   - Essential for proper display of formatted text files
 */
static int
pg_convert PARM_2(char *, o_str, char *, i_str)
{
  register int i;
  int o_cnt = 0, len = strlen(i_str);

  /* go through all of the input string */
  for (i = 0; i < len; i++) {

    /* get the input character and copy it */
    if ((i_str[i] == '\b') && (o_cnt > 0)) {
      /* remove characters prior to backspaces */
      o_cnt--;
    } else {
      /* just keep them otherwise */
      o_str[o_cnt++] = i_str[i];
    }

  }
  o_str[o_cnt] = '\0';
  return(o_cnt);
}

/*
 * pg_readfile - Read and parse file content into pager data structure
 *
 * Reads the entire input file and parses it into the internal file_line
 * array, handling special formatting for news files and header detection.
 * Supports page separation, header highlighting, and content preprocessing.
 *
 * Parameters:
 *   fpin - Open file pointer for reading (must not be NULL)
 *   newsread - TRUE for news format parsing, FALSE for normal text
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Populates file_line array with allocated line content
 *   - Sets max_line to total number of lines read
 *   - Allocates memory for each line (must be freed by pg_cleanup)
 *   - Processes header markers and news formatting
 *
 * Notes:
 *   - Handles CSTR_L header indicators for highlighted sections
 *   - News format removes 2-character prefix and handles tab headers
 *   - Automatically adds page separation between headers
 *   - Limits reading to MAX_FILE_LINES for memory safety
 *   - Each line is individually malloc'd and stored
 */
static void
pg_readfile PARM_2(FILE *, fpin, int, newsread)
{
  int lcount = 0, is_header = FALSE, last_header = 0, len;
  char bigline[1000], *full_line;

  /* go through the entire file */
  while (!feof(fpin) && (lcount < MAX_FILE_LINES)) {

    /* get in a line of input */
  getnextline:
    if (fgets(bigline, 999, fpin) != NULL) {

      /* copy the new line */
      len = strlen(bigline);
      if (bigline[len - 1] == '\n') {
	bigline[--len] = '\0';
      }

      /* remove the news prefix */
      if (newsread == TRUE) {
	full_line = &(bigline[2]);
	len -= 2;
      } else {
	full_line = &(bigline[0]);
      }

      /* check for header lines */
      if ((is_header == TRUE) ||
	  ((newsread == TRUE) && (bigline[1] == '\t'))) {

	/* check to supply separated pages */
	if (lcount > 0) while (last_header++ < LINES - 2) {
	  if ((file_line[lcount].line =
	       (char *) malloc(sizeof(char) * 1)) == NULL) {
	    errormsg("Malloc error");
	    abrt();
	  }
	  file_line[lcount++].line[0] = '\0';
	}

	/* highlight the header line */
	len = pg_convert(string, full_line);
	if ((file_line[lcount].line =
	     (char *) malloc(sizeof(char) * (len + 1)))
	    == NULL) {
	  errormsg("Unable to allocate memory");
	  abrt();
	}
	strcpy(file_line[lcount].line, string);
	file_line[lcount++].highlight = TRUE;
	last_header = 0;

	/* have parsed the header */
	is_header = FALSE;
	if (newsread != TRUE) goto getnextline;

	/* add a blank line for news */
	bigline[0] = '\0';
	bigline[1] = '\0';
	full_line = bigline;
	last_header++;

      } else if (!strcmp(bigline, CSTR_L)) {
	/* header indicator */
	is_header = TRUE;
	goto getnextline;
      } else {
	if (last_header >= LINES - 2) {
	  last_header = 0;
	} else {
	  last_header++;
	}
      }

      /* now copy it */
      len = pg_convert(string, full_line);
      if ((file_line[lcount].line =
	   (char *) malloc(sizeof(char) * (len + 1)))
	  == NULL) {
        errormsg("Serious Error: unable to allocated memory");
	abrt();
      }
      strcpy(file_line[lcount].line, string);
      lcount++;

    }

  }

  /* fill out the page */
  while (last_header++ < LINES - 2) {
    if ((file_line[lcount].line =
	 (char *) malloc(sizeof(char) * 1)) == NULL) {
      errormsg("Malloc error");
      abrt();
    }
    file_line[lcount++].line[0] = '\0';
  }
  max_line = lcount;
}

/*
 * page_file - Main pager entry point for file viewing
 *
 * Primary function that implements the complete pager interface for viewing
 * text files. Handles file opening, content parsing, display management, and
 * user interaction. Provides full pager functionality with vi-like commands.
 *
 * Parameters:
 *   fname - Path to file to be viewed (must not be NULL)
 *   news - TRUE for news format parsing, FALSE for normal text files
 *
 * Returns:
 *   TRUE if file could not be opened, FALSE on normal exit
 *
 * Side Effects:
 *   - Opens and reads specified file
 *   - Takes over screen for pager interface
 *   - Processes all user input until exit
 *   - Cleans up all allocated memory before return
 *
 * Notes:
 *   - Supports 20+ navigation and search commands
 *   - Includes regex search when compiled with REGEXP support
 *   - Handles command counting for repeated operations
 *   - Provides comprehensive help and status information
 *   - Uses efficient scrolling and redraw algorithms
 *   - Returns immediately if file cannot be opened
 */
int
page_file PARM_2(char *, fname, int, news)
{
  int done = FALSE, hold, inp_ch, cmd_count = 0;
  FILE *fp_pg;

  /* check for and open the file */
  if ((fp_pg = fopen(fname, "r")) == NULL) {
    /* unable to open the file */
    return(TRUE);
  }

  /* read in the file */
  redraw = DRAW_FULL;
  pg_init();
  pg_readfile(fp_pg, news);

  /* close up the file */
  fclose(fp_pg);

  /* show the file at first */
  pg_setcurrent(0);
  pg_saveline();
  pg_redraw(TRUE);

  /* peruse the file and parse the input */
  while (done == FALSE) {

    /* change file display if needed */
    cmd_count = 0;
    if (old_line != cur_line) {
      if (abs(cur_line - old_line) >= LINES - 1) {
	pg_redraw(FALSE);
	pg_saveline();
      } else {
	pg_scroll();
      }
    }

    /* place the bottom prompt */
    pg_bottom();

    /* parse the input */
  parse_some_more:
    switch (inp_ch = next_char()) {
    case CNTRL_L:
      /* clear redraw */
      pg_redraw(TRUE);
      break;
    case CNTRL_R:
      /* no clear redraw */
      pg_redraw(FALSE);
      break;
    case 'Q':
    case 'q':
      /* leave it! */
      done = TRUE;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      /* increase command count */
      if (cmd_count == 0) {
	clear_bottom(1);
	mvaddstr(LINES - 1, 0, "Command count: ");
      }
      addch(inp_ch);
      refresh();
      cmd_count *= 10;
      cmd_count += (inp_ch - '0');
      goto parse_some_more;
    case 'h':
      /* help listing */
      pg_help();
      break;
    case '=':
      /* display status */
      pg_status();
      break;
    case 'J':
    case ' ':
      /* scroll forward */
      do {
	pg_setcurrent(cur_line + pager_scroll);
      } while (--cmd_count > 0);
      break;
    case '\r':
    case '\n':
    case 'j':
      /* scroll forward 1 line */
      do {
	pg_setcurrent(cur_line + 1);
      } while (--cmd_count > 0);
      break;
    case 'K':
    case 'b':
      /* scroll backward */
      do {
	pg_setcurrent(cur_line - pager_scroll);
      } while (--cmd_count > 0);
      break;
    case 'k':
    case '\b':
    case EXT_DEL:
      /* scroll backward 1 line */
      do {
	pg_setcurrent(cur_line - 1);
      } while (--cmd_count > 0);
      break;
    case 'g':
      /* back to where we once belonged */
      pg_setcurrent(cmd_count);
      break;
    case 'G':
      /* onward and downward */
      pg_setcurrent(max_line - cmd_count);
      break;
    case '?':
      /* new string search backward */
      mvaddch(LINES - 1, 0, '?');
      clrtoeol();
      get_string(string, STR_SPACE, COLS - 2);

      /* check the string */
      if (strlen(string) > 0) {

	/* check whether to just strcpy or compile the regexp */
	if (check_regexp(string) == TRUE) {
#ifdef REGEXP
	  char *temp_str;
	  if ((temp_str = re_comp(string)) != 0) {
	    /* problem with regular expression format */
	    if (strlen(search_str) == 0) {
	      /* fix old string */
	      check_regexp(search_str);
	    }
	    errormsg(temp_str);
	  }
#endif /* REGEXP */
	}
	strcpy(search_str, string);
      }
      /* fallthrough */
    case 'N':
      /* repeat search backward */
      if (strlen(search_str) == 0) {
	errormsg("There is no current search string");
	break;
      }
      do {
	if ((hold = pg_search(-1)) == -1) {
#ifdef REGEXP
	  pg_restore();
	  if (use_regexp == TRUE) {
	    errormsg("search pattern not found");
	    break;
	  }
#endif /* REGEXP */
	  errormsg("search string not found");
	  break;
	} else {
	  pg_setcurrent(hold);
	}
      } while (--cmd_count > 0);
      break;
    case '/':
      /* new string search forward */
      mvaddch(LINES - 1, 0, '/');
      clrtoeol();
      get_string(string, STR_SPACE, COLS - 2);

      /* check the string */
      if (strlen(string) > 0) {

	/* check whether to just strcpy or compile the regexp */
	if (check_regexp(string) == TRUE) {
#ifdef REGEXP
	  char *temp_str;
	  if ((temp_str = re_comp(string)) != 0) {
	    /* problem with regular expression format */
	    if (strlen(search_str) == 0) {
	      /* fix old string */
	      check_regexp(search_str);
	    }
	    errormsg(temp_str);
	  }
	  
#endif /* REGEXP */
	}
	strcpy(search_str, string);
      }
      /* fallthrough */
    case 'n':
      /* repeat search forward */
      if (strlen(search_str) == 0) {
	errormsg("There is no current search string");
	break;
      }
      do {
	if ((hold = pg_search(1)) == -1) {
#ifdef REGEXP
	  pg_restore();
	  if (use_regexp == TRUE) {
	    errormsg("search pattern not found");
	    break;
	  }
#endif /* REGEXP */
	  errormsg("search string not found");
	  break;
	} else {
	  pg_setcurrent(hold);
	}
      } while (--cmd_count > 0);
      break;
    case 'p':
      /* search for header line forward */
      do {
	if ((hold = pg_header(1)) == -1) {
	  pg_restore();
	  errormsg("header search failed");
	  break;
	} else {
	  pg_setcurrent(hold);
	}
      } while (--cmd_count > 0);
      break;
    case 'P':
      /* search for header line backward */
      do {
	if ((hold = pg_header(-1)) == -1) {
	  pg_restore();
	  errormsg("header search failed");
	  break;
	} else {
	  pg_setcurrent(hold);
	}
      } while (--cmd_count > 0);
      break;
    case 'm':
      /* mark the current line */
      marked_line = cur_line;
      break;
    case 'x':
      /* go to marked position */
      if (marked_line != (-1)) {
	pg_setcurrent(marked_line);
      } else {
	errormsg("There is no current mark");
      }
      break;
    case 's':
      /* get a new displacement setting */
      if (cmd_count == 0) {
	mvaddstr(LINES - 1, 0, "What is new scroll distance? ");
	clrtoeol();
	hold =  get_number(FALSE);
      } else {
	no_input = FALSE;
	hold = cmd_count;
      }
      if (no_input != TRUE) {
	if (hold < 1) {
	  errormsg("It must be greater than or equal to 1");
	} else if (hold >= LINES - 1) {
	  errormsg("That is greater than the screen size");
	} else {
	  pager_scroll = hold;
	}
      }
      break;
    case 'S':
      /* get a new displacement setting */
      if (cmd_count == 0) {
	mvaddstr(LINES - 1, 0, "Enter a new displacement setting? ");
	clrtoeol();
	hold =  get_number(FALSE);
      } else {
	hold = cmd_count;
	no_input = FALSE;
      }
      if (no_input != TRUE) {
	if (hold >= LINES - 1) {
	  errormsg("That is greater than the screen size");
	} else {
	  pg_setcurrent(cur_line + hold - pager_offset);
	  pg_saveline();
	  pager_offset = hold;
	}
      }
      break;
    case 't':
      /* set the tab width */
      if (cmd_count == 0) {
	mvaddstr(LINES - 1, 0, "Enter a new tab spacing? ");
	clrtoeol();
	hold =  get_number(FALSE);
      } else {
	hold = cmd_count;
	no_input = FALSE;
      }
      if (no_input != TRUE) {
	if (hold > 16) {
	  errormsg("That is much too large a setting");
	} else if (hold == 0) {
	  errormsg("A tab must be at least one space wide");
	} else {
	  pager_tab = hold;
	  pg_redraw(FALSE);
	}
      }
      break;
    default:
      /* unknown keystroke */
      beep();
      break;
    }

  }

  /* close up shop */
  pg_cleanup();
  return(FALSE);
}

