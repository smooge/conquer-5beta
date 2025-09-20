/* This file handles most generic input/output commands */
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
#ifdef SYS_SIGNAL
#include <sys/signal.h>
#else
#include <signal.h>
#endif /* SYS_SIGNAL */
#include <unistd.h>
#include "armyX.h"
#include "cityX.h"
#include "navyX.h"
#include "itemX.h"
#include "calenX.h"
#include "desigX.h"
#include "magicX.h"
#include "statusX.h"
#include "stringX.h"
#include "elevegX.h"
#include "spellsX.h"
#include "tgoodsX.h"
#include "optionsX.h"
#include "keyvalsX.h"
#include "caravanX.h"
#include "patchlevel.h"
#ifdef WINCH_HANDLER
#include <sgtty.h>
#endif /* WINCH_HANDLER */
#ifdef ALLOW_EDIT_FORK
#include <sys/wait.h>
#endif /* ALLOW_EDIT_FORK */

/* indicate that a false character was sent */
static int fake_char = FALSE;

/* SEND_DUMMY_CHAR -- sends dummy char so system pretends it had been typed */
/*
 * send_dummy_char - Send dummy character to terminal for window size handling
 *
 * Sends a dummy character (ASCII 255) to the terminal using TIOCSTI ioctl
 * to simulate keyboard input. This triggers terminal processing and allows
 * the window size change handler to properly refresh the display.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Sets fake_char flag to TRUE to mark this as a dummy input
 *   - May call errormsg() if ioctl fails
 *   - Injects character into terminal input stream
 *
 * Testing Notes:
 *   Category: D (Mock Intensive) - Platform-specific ioctl operations requiring system-level mocking
 *   Approach: Mock ioctl(), file descriptor operations, and terminal interface
 *   Key Tests: Success path, ioctl failure handling, fake_char flag setting
 *   Dependencies: WINCH_HANDLER feature flag, file descriptor 2, errormsg()
 *   Mock Requirements: ioctl() system call, terminal device interface
 *   Complexity: Simple (11 lines) - straightforward logic but complex system dependencies
 *
 * Notes:
 *   - Only compiled when WINCH_HANDLER is defined
 *   - Uses file descriptor 2 (stderr) for ioctl operations
 *   - Critical for proper window resize handling in curses applications
 */
#ifdef WINCH_HANDLER
static void
send_dummy_char()
{
  char temp;
  temp = 255;
  if (ioctl(2, TIOCSTI, &temp) == -1) {
    errormsg("TIOCSTI ioctl failed");
  }
  fake_char = TRUE;
  return;
}
#endif /* WINCH_HANDLER */

/*
 * win_size_change - Signal handler for terminal window size changes
 *
 * Handles SIGWINCH signal when terminal window is resized. Queries current
 * window size, enforces minimum dimensions (80x24), updates global LINES/COLS
 * variables, and reinitializes the curses display to match new size.
 *
 * Parameters:
 *   void - Signal handler signature with PARM_0 macro
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Updates global LINES and COLS variables
 *   - May force window resize if too small (minimum 80x24)
 *   - Reinitializes curses screen with initscr()
 *   - Triggers complete screen redraw via do_redraw()
 *   - Sends dummy character to refresh input processing
 *   - May display error messages if window too small or ioctl fails
 *
 * Testing Notes:
 *   Category: D (Mock Intensive) - Complex signal handler with terminal ioctl operations requiring extensive mocking
 *   Approach: Mock signal handling, ioctl operations, curses functions, and global state
 *   Key Tests: Window resize scenarios, minimum size enforcement, ioctl failure handling
 *   Dependencies: SIGWINCH signal, TIOCGWINSZ/TIOCSWINSZ ioctls, curses environment
 *   Mock Requirements: ioctl() calls, signal context, curses functions, global LINES/COLS
 *   Complexity: Moderate (37 lines) - multiple system interactions and error handling paths
 *
 * Notes:
 *   - Only compiled when WINCH_HANDLER is defined
 *   - Registered as SIGWINCH signal handler during initialization
 *   - Uses ioctl with TIOCGWINSZ/TIOCSWINSZ for window size operations
 *   - Critical for maintaining proper display in resizable terminals
 */
void
win_size_change PARM_1(int, sig)
{
  (void)sig;  /* Suppress unused parameter warning */
#ifdef WINCH_HANDLER
  struct winsize w;
  int set_size = FALSE;

  if (ioctl(2, TIOCGWINSZ, &w) == -1) {
    errormsg("TIOCGWINSZ ioctl failed");
    return;
  }
  if (w.ws_col < 80) {
    w.ws_col = 80;
    set_size = TRUE;
  }
  if (w.ws_row < 24) {
    w.ws_row = 24;
    set_size = TRUE;
  }
  if (set_size == TRUE) {
    w.ws_xpixel = 0;
    w.ws_ypixel = 0;
    if (ioctl(2, TIOCSWINSZ, &w) == -1) {
      errormsg("TIOCSWINSZ ioctl failed");
      return;
    }
    errormsg("Screen too small.. please resize again.");
  }
  LINES = w.ws_row;
  COLS = w.ws_col;
  send_dummy_char();
  initscr();
  do_redraw();
  return;
#endif /* WINCH_HANDLER */
}

/*
 * copy_file - Copy contents from source file to destination file
 *
 * Copies all content from the source file to the destination file
 * character by character. Used by the editor fork functionality to
 * copy files to/from temporary editing locations.
 *
 * Parameters:
 *   from_file - Source file path to copy from (must not be NULL)
 *   to_file - Destination file path to copy to (must not be NULL)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Creates or overwrites destination file
 *   - Displays error messages if file operations fail
 *   - Uses global 'string' buffer for error message formatting
 *
 * Testing Notes:
 *   Category: A (Unit) - File copying algorithm with clear input/output, testable with mock files
 *   Approach: Unit testing with mock file operations and temporary test files
 *   Key Tests: Successful copy, source file missing, destination create failure, read/write errors
 *   Dependencies: FILE operations (fopen, fclose, getc, putc), global string buffer, errormsg()
 *   Mock Requirements: File system operations, error message display
 *   Complexity: Simple (32 lines) - straightforward file copying logic with clear error paths
 *
 * Notes:
 *   - Only compiled when ALLOW_EDIT_FORK is defined
 *   - Opens source in read mode, destination in write mode
 *   - Copies character by character until EOF
 *   - Properly closes both files even on error conditions
 *   - No error checking for partial writes or disk space issues
 */
#ifdef ALLOW_EDIT_FORK
static void
copy_file PARM_2(char *, from_file, char *, to_file)
{
  FILE *fp1, *fp2;
  int ch;

  /* open the source file */
  if ((fp1 = fopen(from_file, "r")) == NULL) {
    sprintf(string, "Unable to open file <%s> for reading.",
	    from_file);
    errormsg(string);
    return;
  }

  /* open the target file */
  if ((fp2 = fopen(to_file, "w")) == NULL) {
    sprintf(string, "Unable to open file <%s> for writing.",
	    to_file);
    errormsg(string);
    return;
  }

  /* perform the copy */
  while (!feof(fp1) &&
	 ((ch = getc(fp1)) != EOF)) {
    putc(ch, fp2);
  }

  /* close up */
  fclose(fp1);
  fclose(fp2);
}
#endif /* ALLOW_EDIT_FORK */

/*
 * fork_edit_on_file - Launch external editor on file copy with security handling
 *
 * Forks a child process to launch an external editor on a temporary copy of
 * the specified file. Handles user ID switching for security, environment
 * setup, and file management. After editing, copies modified file back to
 * original location and cleans up temporary files.
 *
 * Parameters:
 *   fname - Original file path to edit (may not exist for new files)
 *   pstr - Optional message to display before editor launch (may be NULL)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Forks child process and waits for completion
 *   - Creates temporary file in TMP_DIR with unique name
 *   - Switches user IDs for security if SWITCHID enabled
 *   - Changes working directory to TMP_DIR
 *   - Closes open file handles for security
 *   - Launches editor specified by ENV_EDITOR or DEFAULT_EDITOR
 *   - Copies modified file back to original location
 *   - Restores curses raw mode and noecho after editor exit
 *   - Displays error messages for various failure conditions
 *
 * Testing Notes:
 *   Category: E (Deferred/Skip) - Complex fork/exec with UID switching, security handling, extensive platform dependencies
 *   Approach: Skip until post-modernization - too complex for isolated testing
 *   Key Tests: Process creation, UID switching, file operations, editor execution, error handling
 *   Dependencies: Process management, security model, editor programs, filesystem, curses
 *   Mock Requirements: fork(), exec(), setuid(), file system, process synchronization
 *   Complexity: Extremely Complex (125+ lines) - multi-system coordination with security implications
 *
 * Notes:
 *   - Only compiled when ALLOW_EDIT_FORK is defined
 *   - Uses fork/exec pattern for secure editor launching
 *   - Temporary file named using process ID for uniqueness
 *   - Supports both SEARCH_PATHENV and direct exec methods
 *   - Handles UID/EUID switching for setuid game security
 *   - Parent process sleeps and waits for child completion
 *   - Critical for secure in-game text editing functionality
 */
void
fork_edit_on_file PARM_2(char *, fname, char *, pstr)
{
  /*ARGSUSED*/
#ifdef ALLOW_EDIT_FORK
  int proc_id, wproc_id;
  char edit_prog[FILELTH], tmp_file_targ[FILELTH];
#ifdef SWITCHID
#ifdef SYSV4
  uid_t uid, euid;
#else
  int uid, euid;
#endif /* SYSV4 */

  uid = getuid();
  euid = geteuid();
#endif /* SWITCHID */

  /* build the name of the temporary file */
  sprintf(tmp_file_targ, TMP_FILE_FMT, fname, getpid());

  /* spawn the editor */
  switch (proc_id = fork()) {
  case -1:
    errormsg("Unable to create an additional process for editing");
    break;
  case 0:
    /* attempt to get rid of any old file */
    (void) unlink(tmp_file_targ);

#ifndef VMS
    /* make sure file is editable by player */
    (void) umask(FULLWRITE_UMASK);
#endif /* VMS */

    /* make a copy of the original */
    if (exists(fname) == 0) {
      /* begin by copying the file over */
      copy_file(fname, tmp_file_targ);
    }

    /* now switch uids */
#ifdef SWITCHID
    if (euid != uid) {
#ifdef SETREUID
      setreuid(euid, uid);
#else /* SETREUID */
      setuid(euid);
#endif /* SETREUID */
    }
#endif /* SWITCHID */

    /* generate the name of the editor process */
    if (getenv(ENV_EDITOR)) {
      strcpy(edit_prog, getenv(ENV_EDITOR));
    } else {
      strcpy(edit_prog, DEFAULT_EDITOR);
    }

    /* now totally become the original person */
#ifdef SWITCHID
    if (euid != uid) {
#ifdef SETREUID
      setreuid(uid, uid);
#endif /* SETREUID */
    }
#endif /* SWITCHID */

    /* change the default directory */
    if (chdir(TMP_DIR)) {
      errormsg("Unable to access temporary directory");
      exit(FAIL);
    }

    /* final message */
    if (pstr != NULL) errormsg(pstr);

    /* for security purposes close up possible open files */
    if (fexe != NULL) {
      fclose(fexe);
    }

    /* spawn the new editor process */
#ifdef SEARCH_PATHENV
    if (execlp(edit_prog, edit_prog, tmp_file_targ, (char *)0) == -1) {
      errormsg("Unable to start the editor process");
      exit(FAIL);
    }
#else
    if (execl(edit_prog, edit_prog, tmp_file_targ, (char *)0) == -1) {
      errormsg("Unable to start the editor process");
      exit(FAIL);
    }
#endif /* SEARCH_PATHENV */
    exit(SUCCESS);
    break;
  default:
    /* this is the parent of a bouncing baby process */
    do {
      sleep(1);
      wproc_id = wait(0);
    } while ((wproc_id != proc_id) &&
	     (wproc_id != -1));

    /* gather up the file, if it exists */
    if (exists(tmp_file_targ) == 0) {

      /* relocate it back into the current directory */
      copy_file(tmp_file_targ, fname);

      /* attempt to get rid of the old file */
      (void) unlink(tmp_file_targ);
    }

    /* be sure that raw mode is properly enabled, if needed */
    if (in_curses) {
      crmode();
      noecho();
    }
    break;
  }
#else
  errormsg("This function is unavailable within this version of conquer");
#endif /* ALLOW_EDIT_FORK */
}

/*
 * do_redraw - Alternative method of refreshing the curses screen
 *
 * Clears the entire screen and forces a refresh. Used as an alternative
 * screen refresh method, particularly after window size changes or when
 * display corruption is detected.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   Always returns 0 (success)
 *
 * Side Effects:
 *   - Clears entire screen contents via clear()
 *   - Forces immediate screen refresh on VAXC systems
 *
 * Testing Notes:
 *   Category: B (Integration) - Curses screen management requiring display context
 *   Approach: Integration testing with curses environment setup
 *   Key Tests: Screen clearing functionality, VAXC-specific refresh behavior
 *   Dependencies: Curses environment (clear, refresh functions)
 *   Mock Requirements: Curses screen state, platform-specific behavior
 *   Complexity: Simple (12 lines) - minimal logic but requires curses context
 *
 * Notes:
 *   - Called by win_size_change() after window resize
 *   - VAXC-specific refresh() call for compatibility
 *   - Simple but effective screen cleanup mechanism
 */
int
do_redraw PARM_0(void)
{
  /* clean the screen */
  clear();

#ifdef VAXC
  /* stupid VAXC implementation */
  refresh();
#endif /* VAXC */
  return(0);
}

/* local storage for the input queue */
static int inch_count = 0;
static int inch_list[LINELTH];

/*
 * next_char - Obtain the next character from input queue or direct input
 *
 * Returns the next input character, either from the input stack (if characters
 * have been pushed back) or directly from getch(). Handles fake characters
 * generated by window resize events by converting them to ignore signals.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   Next input character as integer
 *   EXT_IGN if character was a fake character from window resize
 *
 * Side Effects:
 *   - Decrements inch_count when popping from input stack
 *   - Resets fake_char flag when processing fake input
 *   - May block waiting for input if no characters queued
 *
 * Testing Notes:
 *   Category: A (Unit) - Input queue management with clear state logic, mockable dependencies
 *   Approach: Unit testing with mock getch() and controlled input stack state
 *   Key Tests: Empty stack (direct getch), stacked input (LIFO order), fake character handling
 *   Dependencies: Input stack (inch_count, inch_list), getch() function, fake_char flag
 *   Mock Requirements: getch() function, global state variables
 *   Complexity: Simple (19 lines) - clear conditional logic with straightforward state management
 *
 * Notes:
 *   - Part of input queue management system
 *   - Handles pushed-back characters from push_char()
 *   - Fake characters come from send_dummy_char() during window resize
 *   - Input stack operates as LIFO (last in, first out)
 */
int
next_char PARM_0(void)
{
  int c;

  /* is there any more input pending? */
  if (inch_count == 0) {
    /* straight input */
    c = getch();
    if (fake_char == TRUE) {
      fake_char = FALSE;
      c = EXT_IGN;
    }
    return(c);
  } else {
    /* input from the stack */
    return(inch_list[--inch_count]);
  }
}

/*
 * push_char - Store input character on the input stack for later processing
 *
 * Pushes a character onto the input stack to be retrieved by the next
 * call to next_char(). Used to "unget" characters that were read but
 * need to be processed later, typically during parsing or completion.
 *
 * Parameters:
 *   ch_in - Character to push onto the input stack
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Increments inch_count and stores character in inch_list
 *   - Displays serious error message if stack overflows
 *   - Character will be returned by next call to next_char()
 *
 * Testing Notes:
 *   Category: A (Unit) - Input stack manipulation with clear logic, minimal dependencies
 *   Approach: Unit testing with controlled stack state and overflow testing
 *   Key Tests: Normal push operation, stack overflow condition, LIFO behavior verification
 *   Dependencies: Input stack arrays (inch_count, inch_list), errormsg() for overflow
 *   Mock Requirements: errormsg() function for overflow handling
 *   Complexity: Simple (10 lines) - straightforward array manipulation with overflow check
 *
 * Notes:
 *   - Input stack has capacity LINELTH (maximum line length)
 *   - Stack operates as LIFO (last in, first out)
 *   - Used by completion routines and input parsing
 *   - Critical for proper input character handling
 */
void
push_char PARM_1(int, ch_in)
{
  if (inch_count == LINELTH) {
    errormsg("SERIOUS ERROR: Input Queue Overflow");
  } else {
    inch_list[inch_count++] = ch_in;
  }
}

/*
 * errorbar - Display highlighted message bar at bottom of screen
 *
 * Creates a highlighted status/error bar at the bottom of the screen with
 * version information and user messages. Used for displaying important
 * system messages, status information, and error notifications.
 *
 * Parameters:
 *   str1 - Primary message text to display after version info
 *   str2 - Secondary message text displayed right-aligned
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Draws highlighted bar at line LINES-4
 *   - Displays version and patch level information
 *   - Shows primary message after version info
 *   - Right-aligns secondary message
 *   - Draws separator line of dashes at LINES-2
 *   - Uses standout/standend for highlighting
 *
 * Testing Notes:
 *   Category: B (Integration) - Screen display function requiring curses environment and positioning
 *   Approach: Integration testing with curses environment setup and screen validation
 *   Key Tests: Message display formatting, alignment calculations, standout highlighting
 *   Dependencies: Curses environment (move, standout, mvprintw, addch), LINES/COLS globals
 *   Mock Requirements: Curses functions, screen dimensions, VERSION/PATCHLEVEL constants
 *   Complexity: Moderate (16 lines) - formatting logic with screen positioning calculations
 *
 * Notes:
 *   - Bar spans full screen width (COLS-1)
 *   - Format: " Conquer VERSION.PATCHLEVEL: str1" with str2 right-aligned
 *   - Creates visual separation with dash line
 *   - Critical for user feedback and status display
 */
void
errorbar PARM_2( char *, str1, char *, str2)
{
  int i;

  move(LINES - 4, 0);
  standout();
  for (i = 0; i < COLS - 1; i++)
    addch(' ');
  standend();
  mvprintw(LINES - 3, 0, " Conquer %s.%d: %s", VERSION, PATCHLEVEL, str1);
  mvaddstr(LINES - 3, COLS - strlen(str2) - 2, str2);
  move(LINES - 2, 0);
  for (i = 0; i < COLS - 1; i++)
    addch('-');
}

/*
 * presskey - Display "Press Any Key" prompt and wait for user input
 *
 * Shows a "Press Any Key" message at the bottom right of the screen,
 * waits for user to press any key, clears the message line, and returns
 * the character that was pressed.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   Integer value of the key pressed by the user
 *
 * Side Effects:
 *   - Displays " Press Any Key" at bottom right of screen
 *   - Forces screen refresh to show the message
 *   - Waits for user input (blocking)
 *   - Clears bottom line after key press
 *
 * Testing Notes:
 *   Category: B (Integration) - User interaction requiring curses display and input coordination
 *   Approach: Integration testing with curses environment and mock input
 *   Key Tests: Message display, user input handling, screen clearing
 *   Dependencies: Curses environment (mvaddstr, refresh), next_char(), clear_bottom()
 *   Mock Requirements: Curses functions, input simulation, screen state
 *   Complexity: Simple (10 lines) - straightforward user interaction pattern
 *
 * Notes:
 *   - Standard pause/continue mechanism in the game interface
 *   - Message positioned at LINES-1, COLS-16 for right alignment
 *   - Used throughout the game for "pause and continue" prompts
 *   - Returns actual key pressed for potential special handling
 */
int
presskey PARM_0(void)
{
  int hold;

  mvaddstr(LINES - 1, COLS - 16, " Press Any Key");
  refresh();
  hold = next_char();
  clear_bottom(1);
  return(hold);
}

/*
 * errormsg - Display error message and wait for user acknowledgment
 *
 * Displays an error or informational message to the user, either on the
 * screen bottom line (in curses mode) or to the update file (non-curses).
 * In curses mode, beeps and waits for user to press a key.
 *
 * Parameters:
 *   str - Error or message text to display (must not be NULL)
 *
 * Returns:
 *   In curses mode: returns the key pressed by user (from presskey())
 *   In non-curses mode: returns 0
 *
 * Side Effects:
 *   - In curses: displays message at bottom line, clears to end of line
 *   - In curses: generates beep sound for attention
 *   - In curses: waits for user keypress via presskey()
 *   - In non-curses: writes message to fupdate file with newline
 *
 * Testing Notes:
 *   Category: B (Integration) - Dual-mode output requiring curses or file I/O coordination
 *   Approach: Integration testing with both curses and file output modes
 *   Key Tests: Curses mode display, file mode output, mode switching behavior
 *   Dependencies: Curses environment or file output, in_curses flag, presskey(), fupdate
 *   Mock Requirements: Curses functions, file I/O, mode flag, beep/audio
 *   Complexity: Moderate (15 lines) - dual-mode logic with different output paths
 *
 * Notes:
 *   - Primary error/message display function throughout the game
 *   - Behavior depends on in_curses global flag
 *   - Used for errors, warnings, confirmations, and status messages
 *   - In curses mode provides interactive feedback with beep and pause
 */
int
errormsg PARM_1 (char *, str)
{
  /* error message to stderr or screen bottom */
  if (in_curses) {
    mvaddstr(LINES - 1, 0, str);
    clrtoeol();
    beep();
    return(presskey());
  } else {
    fprintf(fupdate, "%s\n", str);
  }
  return(0);
}

/*
 * bottommsg - Display message at bottom of screen without waiting for input
 *
 * Displays a message at the bottom of the screen (in curses mode) or writes
 * to the update file (non-curses mode), but does not wait for user input.
 * Used for status updates and information that doesn't require acknowledgment.
 *
 * Parameters:
 *   str - Message text to display (must not be NULL)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - In curses: displays message at bottom line and clears to end of line
 *   - In curses: forces immediate screen refresh
 *   - In non-curses: writes message to fupdate file with newline
 *   - Does not beep or wait for user input
 *
 * Testing Notes:
 *   Category: B (Integration) - Similar to errormsg() but simpler, still requires curses/file coordination
 *   Approach: Integration testing with both curses and file output modes
 *   Key Tests: Curses mode display, file mode output, non-blocking behavior
 *   Dependencies: Curses environment or file output, in_curses flag, fupdate
 *   Mock Requirements: Curses functions, file I/O, mode flag
 *   Complexity: Simple (11 lines) - simpler version of errormsg() without user interaction
 *
 * Notes:
 *   - Similar to errormsg() but without beep or user interaction
 *   - Used for status messages, progress updates, and information display
 *   - Behavior depends on in_curses global flag
 *   - Non-blocking message display for continuous operations
 */
void
bottommsg PARM_1 (char *, str)
{
  if (in_curses) {
    mvaddstr(LINES - 1, 0, str);
    clrtoeol();
    refresh();
  } else {
    fprintf(fupdate, "%s\n", str);
  }
}

/*
 * y_or_n - Get user input and return TRUE for 'y' or 'Y' responses
 *
 * Waits for user to press a key and returns TRUE if the key was 'y' or 'Y',
 * FALSE for any other key. Used for simple yes/no prompts throughout the game.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   TRUE if user pressed 'y' or 'Y'
 *   FALSE for any other key press
 *
 * Side Effects:
 *   - Forces screen refresh before waiting for input
 *   - Consumes one character from input stream
 *   - Does not provide feedback about invalid responses
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple input processing with clear logic, mockable dependencies
 *   Approach: Unit testing with mock next_char() function
 *   Key Tests: 'y' returns TRUE, 'Y' returns TRUE, other keys return FALSE
 *   Dependencies: next_char() function (mockable), refresh() function
 *   Mock Requirements: next_char() for input simulation, refresh() for screen
 *   Complexity: Simple (13 lines) - straightforward character comparison logic
 *
 * Notes:
 *   - Simple binary choice input function
 *   - Case-insensitive for 'y'/'Y' but no special handling for 'n'
 *   - Used for yes/no confirmations and boolean choices
 *   - Does not loop or validate input - any non-y key means FALSE
 */
int
y_or_n PARM_0(void)
{
  int ch;

  /* just get it */
  refresh();
  if (((ch = next_char()) == 'y') ||
      (ch == 'Y')) {
    return(TRUE);
  }
  return(FALSE);
}

/*
 * cr_or_y - Return TRUE for 'y', 'Y', space, or return key presses
 *
 * Waits for user input and returns TRUE for several "affirmative" keys:
 * 'y', 'Y', space, newline, or carriage return. Used for prompts where
 * both explicit yes and default acceptance (return/space) are acceptable.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   TRUE if user pressed 'y', 'Y', space, newline, or carriage return
 *   FALSE for any other key press
 *
 * Side Effects:
 *   - Forces screen refresh before waiting for input
 *   - Consumes one character from input stream
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple input processing with clear switch logic, mockable dependencies
 *   Approach: Unit testing with mock next_char() function
 *   Key Tests: 'y'/'Y' return TRUE, space/return return TRUE, other keys return FALSE
 *   Dependencies: next_char() function (mockable), refresh() function
 *   Mock Requirements: next_char() for input simulation, refresh() for screen
 *   Complexity: Simple (19 lines) - straightforward switch statement with multiple affirmative cases
 *
 * Notes:
 *   - More permissive than y_or_n() - accepts default confirmation
 *   - Space and return keys treated as affirmative responses
 *   - Used for prompts where "press enter to continue" behavior desired
 *   - Common pattern for "continue or abort" type prompts
 */
int
cr_or_y PARM_0(void)
{
  /* just get it */
  refresh();
  switch (next_char()) {
  case 'Y':
  case 'y':
  case ' ':
  case '\n':
  case '\r':
    return(TRUE);
  }
  return(FALSE);
}

/*
 * cq_init - Initialize the curses display system for the game
 *
 * Sets up the curses terminal interface, configures signal handlers,
 * validates terminal size requirements, and establishes proper input modes.
 * Critical initialization function for all curses-based game interfaces.
 *
 * Parameters:
 *   progname - Program name for error messages (must not be NULL)
 *
 * Returns:
 *   void (may exit program if terminal requirements not met)
 *
 * Side Effects:
 *   - Calls initscr() to initialize curses
 *   - Sets up SIGWINCH handler for window resize events
 *   - Sets in_curses global flag to TRUE
 *   - Validates minimum terminal size (80x24)
 *   - Exits program if terminal too small
 *   - Enables raw input mode (crmode)
 *   - Disables character echoing (noecho)
 *   - Platform-specific terminal setup (TSERVER, VMS)
 *
 * Testing Notes:
 *   Category: C (System Level) - Critical curses initialization requiring complete system environment
 *   Approach: System testing with full curses environment and terminal setup
 *   Key Tests: Curses initialization, signal handler setup, terminal size validation
 *   Dependencies: Curses library, signal handling, terminal environment, platform-specific code
 *   Mock Requirements: Complete curses environment, signal system, terminal state
 *   Complexity: Moderate (28 lines) - platform-specific initialization with multiple system interactions
 *
 * Notes:
 *   - Must be called before any other curses operations
 *   - Enforces minimum 80x24 terminal size requirement
 *   - Sets up signal handling for proper window resize support
 *   - Platform-specific code for TSERVER and VMS systems
 *   - Exits with SUCCESS if terminal size insufficient
 */
void
cq_init PARM_1 (char *, progname)
{
#ifdef TSERVER
  extern void setterm_pas();
  setterm_pas();
#endif /* TSERVER */
#ifdef WINCH_HANDLER
  signal(SIGWINCH, win_size_change);
#endif /* WINCH_HANDLER */
  initscr();
#ifdef VMS
  system("set term/numeric");
#endif /* VMS */

  /* may now check terminal size */
  in_curses = TRUE;
  if (COLS < 80 || LINES < 24) {
    mvprintw(LINES - 2, 0, "%s: terminal should be at least 80x24", progname);
    errormsg("Please try again with a different setup");
    cq_bye(SUCCESS);
    in_curses = FALSE;
  }

  /* set raw mode and turn off echoing */
  crmode();
  noecho();
}

/*
 * cq_reset - Terminate the curses display gracefully and restore terminal
 *
 * Cleanly shuts down the curses interface, restores normal terminal modes,
 * clears the screen, and resets signal handlers. Should be called before
 * program exit to leave terminal in proper state.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Disables SIGWINCH signal handler
 *   - Clears entire screen contents twice for thoroughness
 *   - Forces screen refresh to ensure changes are visible
 *   - Restores normal terminal input mode (nocrmode)
 *   - Re-enables character echoing (echo)
 *   - Calls endwin() to properly terminate curses
 *   - Sets in_curses global flag to FALSE
 *   - Platform-specific terminal reset (VMS)
 *
 * Testing Notes:
 *   Category: C (System Level) - Critical curses cleanup requiring complete system environment
 *   Approach: System testing with full curses environment and terminal state management
 *   Key Tests: Signal handler cleanup, screen clearing, terminal mode restoration
 *   Dependencies: Curses library, signal handling, terminal environment, platform-specific code
 *   Mock Requirements: Complete curses environment, signal system, terminal state
 *   Complexity: Moderate (30 lines) - comprehensive cleanup with multiple system interactions
 *
 * Notes:
 *   - Should be called before any program exit
 *   - Double clear/refresh ensures clean terminal state
 *   - Restores terminal to state before cq_init() was called
 *   - Platform-specific reset code for VMS systems
 *   - Critical for proper terminal cleanup
 */
void
cq_reset PARM_0(void)
{
#ifdef VMS
  extern void resetterm();
#endif /* VMS */
#ifdef WINCH_HANDLER
  signal(SIGWINCH, SIG_IGN);
#endif /* WINCH_HANDLER */

  /* really clear it */
  move(0,0);
  clrtobot();
  refresh();
  clear();
  move(0,0);
  refresh();

  /* clean up the display */
  nocrmode();
  echo();
  endwin();
#ifdef VMS
  resetterm();
#endif /* VMS */
  in_curses = FALSE;
}

/*
 * cq_bye - Clean up curses display and exit program with specified status
 *
 * Performs complete curses cleanup by calling cq_reset() and then
 * terminates the program with the specified exit status. Used throughout
 * the game for clean program termination.
 *
 * Parameters:
 *   status - Exit status code to pass to exit() (typically SUCCESS or FAIL)
 *
 * Returns:
 *   Does not return (calls exit())
 *
 * Side Effects:
 *   - Calls cq_reset() to clean up curses display
 *   - Terminates program with specified exit status
 *   - All cq_reset() side effects apply (terminal cleanup, etc.)
 *
 * Testing Notes:
 *   Category: B (Integration) - Simple wrapper requiring curses cleanup coordination
 *   Approach: Integration testing with mock cq_reset() and exit() functions
 *   Key Tests: Cleanup function calling, exit status passing
 *   Dependencies: cq_reset() function, exit() system call
 *   Mock Requirements: cq_reset() function, exit() call interception
 *   Complexity: Simple (3 lines) - straightforward wrapper function
 *
 * Notes:
 *   - Standard way to exit the game with proper cleanup
 *   - Ensures terminal is always left in proper state
 *   - Used for both normal and error exits
 *   - Combines cleanup and exit into single function call
 */
void
cq_bye PARM_1 (int, status)
{
  cq_reset();
  exit(status);
}

/*
 * clear_bottom - Clear message area at bottom of screen
 *
 * Clears the specified number of lines at the bottom of the screen,
 * typically used to clear message areas, status lines, and prompt regions.
 * Only operates in curses mode.
 *
 * Parameters:
 *   l - Number of lines to clear from bottom (0 defaults to 5 lines)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Clears l lines from bottom of screen (LINES-l to LINES-1)
 *   - Uses clrtoeol() to clear each line from cursor to end
 *   - No effect if not in curses mode
 *
 * Testing Notes:
 *   Category: B (Integration) - Screen area management requiring curses environment
 *   Approach: Integration testing with curses environment and screen state validation
 *   Key Tests: Line clearing functionality, parameter validation, curses mode checking
 *   Dependencies: Curses environment (move, clrtoeol), in_curses flag, LINES global
 *   Mock Requirements: Curses functions, screen state, global variables
 *   Complexity: Simple (11 lines) - straightforward loop with curses operations
 *
 * Notes:
 *   - Commonly used to clear message areas before new output
 *   - Default of 5 lines covers typical message area size
 *   - Preserves screen content above the cleared area
 *   - Used by presskey() and other message functions
 */
void
clear_bottom PARM_1 (int, l)
{
  if (in_curses == FALSE) return;
  if (l == 0) l = 5;
  for (; l > 0 ; l--) {
    move(LINES - l, 0);
    clrtoeol();
  }
}

/*
 * show_char - Display a character at current cursor position with formatting
 *
 * Displays a character with optional full formatting (quotes and separator).
 * Non-printable characters are shown in caret notation (^X format).
 * Used for displaying key bindings and character representations.
 *
 * Parameters:
 *   ch - Character to display
 *   full - If TRUE, add quotes around character and trailing dash
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Displays character at current cursor position
 *   - Non-printable chars shown as ^X (caret notation)
 *   - If full=TRUE: printable chars shown as 'X'-, non-printable as ^X-
 *   - If full=FALSE: characters shown without decoration
 *   - Advances cursor position
 *
 * Testing Notes:
 *   Category: A (Unit) - Character formatting with clear logic, minimal dependencies
 *   Approach: Unit testing with mock curses output and character validation
 *   Key Tests: Printable characters, non-printable characters, full vs compact formatting
 *   Dependencies: Curses environment (addch), isprint() function, non_cntrl() function
 *   Mock Requirements: Curses output functions, character classification
 *   Complexity: Simple (16 lines) - straightforward character formatting logic
 *
 * Notes:
 *   - Used primarily for key binding displays and character references
 *   - Caret notation converts control chars to visible form
 *   - Full format useful for lists, compact format for inline display
 *   - Part of character display utility functions
 */
void
show_char PARM_2(char, ch, int, full)
{
  /* show the character */
  if (!isprint(ch)) {
    addch('^');
    addch(non_cntrl(ch));
  } else {
    if (full) addch('\'');
    addch(ch);
    if (full) addch('\'');
  }

  /* finish */
  if (full) addch('-');
}

/*
 * unshow_char - Remove a previously displayed character by overwriting with spaces
 *
 * Erases a character that was displayed by show_char() by overwriting each
 * position with spaces and restoring the cursor to its original position.
 * Handles both full and compact display formats.
 *
 * Parameters:
 *   ch - Character that was previously displayed (for format calculation)
 *   full - Must match the full parameter used in original show_char() call
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Overwrites displayed character positions with spaces
 *   - Moves cursor backward to erase each displayed character
 *   - Restores cursor to position after the erased character
 *   - Handles different widths for printable vs non-printable characters
 *
 * Notes:
 *   - Must be called with same parameters as matching show_char() call
 *   - Non-printable chars take 2 positions (^X), printable take 1
 *   - Full format adds quotes and dash, requiring additional positions
 *   - Used for dynamic character display updates and corrections
 */
void
unshow_char PARM_2(char, ch, int, full)
{
  int xpos, ypos;

  /* find the current position */
  getyx(stdscr, ypos, xpos);

  /* remove separating dash */
  if (full == TRUE) {
    move(ypos, --xpos);
    addch(' ');
  }

  /* show the key itself */
  if (!isprint(ch)) {
    move(ypos, --xpos);
    addch(' ');
    move(ypos, --xpos);
    addch(' ');
  } else {
    if (full) {
      move(ypos, --xpos);
      addch(' ');
    }
    move(ypos, --xpos);
    addch(' ');
    if (full) {
      move(ypos, --xpos);
      addch(' ');
    }
  }

  /* get back to where we once belonged */
  move(ypos, xpos);
}

/*
 * show_str - Display an entire string with character formatting
 *
 * Formats and displays a complete string using the same character formatting
 * rules as show_char(). Calls form_str() to prepare the formatted version
 * then displays it at the current cursor position.
 *
 * Parameters:
 *   str - String to display (must not be NULL)
 *   full - If TRUE, use full formatting for each character
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Creates formatted version of string in local buffer
 *   - Displays formatted string at current cursor position
 *   - Advances cursor to end of displayed string
 *
 * Notes:
 *   - Uses form_str() to handle character formatting consistently
 *   - Each character in string formatted according to show_char() rules
 *   - Useful for displaying key sequences and formatted text
 *   - Buffer size limited to LINELTH characters
 */
void
show_str PARM_2(char *, str, int, full)
{
  char str_data[LINELTH];

  /* show all of the characters in the string */
  form_str(str_data, str, full);
  addstr(str_data);
}

/*
 * get_number - Read integer input from user with editing and validation
 *
 * Interactive integer input function with full editing support including
 * backspace, line clear, and overflow protection. Handles negative numbers
 * if requested and provides visual feedback during input.
 *
 * Parameters:
 *   allowneg - If TRUE, allows negative numbers (leading minus sign)
 *
 * Returns:
 *   Entered number as long integer
 *   0 if no input entered (sets no_input = TRUE)
 *   -1 if user pressed escape (sets no_input = TRUE)
 *
 * Side Effects:
 *   - Sets global no_input flag based on user input
 *   - Displays characters as user types them
 *   - Handles special keys: ^L/^R (redraw), ESC (abort), ^U (clear line)
 *   - Supports backspace/delete for character deletion
 *   - Limits input to 12 characters with overflow protection
 *
 * Notes:
 *   - Prevents integer overflow by checking against BIGINT/10
 *   - Negative sign only accepted as first character
 *   - Visual editing with immediate character feedback
 *   - Returns to beginning of line on ^U (line clear)
 *   - Critical input function used throughout game interface
 */
long
get_number PARM_1(int, allowneg)
{
  long sum=0;
  int ch, negval=FALSE;
  int done=FALSE, count=0, xpos, ypos;

  /* enter input until a return is hit */
  refresh();
  while (!done) {
    ch = next_char();
    if (isdigit(ch) &&
	(sum < BIGINT / 10) &&
	(count < 12)) {
      /* only print numbers to the screen */
      addch(ch);
      refresh();
      sum *= 10L;
      if (count == 0) {
	negval = FALSE;
      }
      count++;
      sum += (long)(ch - '0');
    } else if (allowneg && ch == '-' && count == 0) {
      addch(ch);
      refresh();
      negval = TRUE;
      count++;
      sum = 0L;
    } else if (ch == CNTRL_L || ch == CNTRL_R) {
      /* make sure that redrawing works */
      wrefresh(curscr);
    } else if (ch == EXT_ESC) {
      /* quick out */
      no_input = TRUE;
      return(-1L);
    } else if (ch == CNTRL_U) {
      /* make sure that ^U works */
      getyx(stdscr, ypos, xpos);
      while (count > 0) {
	move(ypos, --xpos);
	addch(' ');
	count--;
      }
      move(ypos, xpos);
      refresh();
      sum = 0L;
    } else if ((ch == '\b' || ch == EXT_DEL) && count) {
      /* only delete what was printed */
      getyx(stdscr, ypos, xpos);
      move(ypos, --xpos);
      addch(' ');
      move(ypos, xpos);
      refresh();
      sum /= 10L;
      count--;
    } else if((ch == '\n') || (ch == '\r')) {
      done = TRUE;
    }
  }

  /* set variable for no input */
  if (count == 0) {
    no_input = TRUE;
    return(0L);
  }
  no_input = FALSE;
  if (negval == TRUE) {
    sum = -sum;
  } else if (sum < 0) {
    sum = 0;
  }
  return( sum );
}

/* GET_DOUBLE -- Read a double; Set no_input for just carriage return */
double
get_double PARM_1(int, allowneg)
{
  double sum=0.0, decimal=1.0;
  int ch, negval=FALSE, lastdigit=0;
  int done=FALSE, count=0, xpos, ypos;

  /* enter input until a return is hit */
  refresh();
  while (!done) {
    ch = next_char();
    if (isdigit(ch) && count < 12) {
      /* only print numbers to the screen */
      addch(ch);
      refresh();
      if (decimal > 2.0) {
	lastdigit = (int)(ch - '0');
	sum += (float) lastdigit / decimal;
	decimal *= 10.0;
      } else {
	sum *= 10.0;
	sum += (double)(ch - '0');
      }
      if (count == 0) {
	negval = FALSE;
      }
      count++;
    } else if (allowneg == TRUE && ch == '-' && count == 0) {
      addch(ch);
      refresh();
      negval = TRUE;
      count++;
      sum = 0L;
    } else if (ch == '.' && decimal < 2.0) {
      addch(ch);
      refresh();
      count++;
      decimal = 10.0;
    } else if (ch == CNTRL_L || ch == CNTRL_R) {
      /* make sure redraw works */
      wrefresh(curscr);
    } else if (ch == CNTRL_U) {
      /* make sure that ^U works */
      getyx(stdscr, ypos, xpos);
      while (count > 0) {
	move(ypos, --xpos);
	addch(' ');
	count--;
      }
      move(ypos, xpos);
      refresh();
      decimal = 1.0;
      sum = 0.0;
    } else if ((ch == '\b' || ch == EXT_DEL) && count) {
      /* only delete what was printed */
      getyx(stdscr, ypos, xpos);
      move(ypos, --xpos);
      addch(' ');
      move(ypos, xpos);
      refresh();
      if (decimal > 20.0) {
	decimal /= 10.0;
	sum -= (float) lastdigit / decimal;
      } else if (decimal > 2.0) {
	decimal = 1.0;
      } else {
	sum /= 10.0;
      }
      count--;
    } else if((ch == '\n') || (ch == '\r')) {
      done = TRUE;
    }
  }
  /* set variable for no input */
  if (count == 0) {
    no_input = TRUE;
    return(0.0);
  }
  no_input = FALSE;
  if (negval == TRUE) {
    sum = -sum;
  } else if (sum < 0.0) {
    sum = 0.0;
  }
  return( sum );
}

/* GET_OPTION -- Enter a possible option string */
int
get_option PARM_0(void)
{
  int i;

  /* first get the string */
  get_string(string, STR_OPTION, OPT_LTH);
  if (no_input == TRUE) return(options_number);

  for (i = 0; i < options_number; i++) {
    if (str_test(opt_list[i], string) == 0) break;
  }
  return(i);
}

/* TEST_COMPLETE -- Set buffer and completion string if completion */
static int
test_complete PARM_4 (char *, s1, char *, s2, char *, bstr, char *, cstr)
{
  int hold=FALSE;

  /* is it a completion? */
  if (completion(s1, s2)) {
    hold = TRUE;

    /* is the first completion? */
    if (bstr[0] == '\0') {
      (void) strcpy(bstr, s2);
      (void) strcpy(cstr, bstr);
    } else {
      /* find the smallest unique portion */
      while ( !completion(cstr, s2) ) {
	cstr[strlen(cstr) - 1] = '\0';
      }

      /* append any matches */
      if (strlen(bstr) + strlen(s2) < BIGLTH - 2) {
	(void) strcat(bstr, " ");
	(void) strcat(bstr, s2);
      }
    }
  }
  return(hold);
}

/* EXTEND_STR -- For certain string types complete unique words */
static int
extend_str PARM_2 (char *, str, int, stype)
{
  int matches = 0, count, start, end;
  int xspot, yspot;
  char bufstr[BIGLTH], cmpstr[BIGLTH];

  /* initialization */
  bufstr[0] = cmpstr[0] = '\0';

  /* perform completion based on type of input requested */
  switch(stype) {
  case STR_XNAME:
    /* add in the "news" name */
    if (str[0] == 'n') {
      if (test_complete(str, "news", bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_NAME:
    /* compare with all nation names */
    if (str[0] == 'g') {
      if (test_complete(str, "god", bufstr, cmpstr)) {
	matches++;
      }
    }
    for (count = 0; count < MAXNTN; count++) {
      if ((ntn_tptr = world.np[count]) != NULL) {
	if (test_complete(str, ntn_tptr->name, bufstr, cmpstr)) {
	  matches++;
	}
      }
    }
    break;
  case STR_CITY:
    if (country == 0) {
      /* go through all cities in the world */
      start = 0;
      end = MAXNTN;
    } else {
      /* compare with all city names in the current nation */
      start = country;
      end = country + 1;
    }

    for (count = start; count < end; count++) {
      if ((ntn_tptr = world.np[count]) != NULL) {
	/* compare with all city names in this nation */
	for (city_tptr = ntn_tptr->city_list;
	     city_tptr != NULL;
	     city_tptr = city_tptr->next) {
	  if (test_complete(str, city_tptr->name, bufstr, cmpstr)) {
	    matches++;
	  }
	}
      }
    }
    break;
  case STR_ARMY:
    /* compare with army type names */
    for (count = 0; count < num_armytypes; count++) {
      if (test_complete(str, ainfo_list[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_ACLASS:
    /* compare with army type names */
    for (count = 0; count < num_aclasses; count++) {
      if (test_complete(str, ainfo_clist[count], bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_STATUS:
    /* compare with all of the army statuses */
    for (count = 0; count < num_statuses; count++) {
      if (test_complete(str, stat_info[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_MONTH:
    /* compare with all of the months */
    for (count = 0; count < 12; count++) {
      if (test_complete(str, monthstr[count], bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_SPEED:
    /* compare with all of the speeds */
    for (count = SPD_MINIMUM; count < SPD_NUMBER; count++) {
      if (test_complete(str, speedname[count], bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_DESG:
    /* check all of the major designations */
    for (count = 0; count < MAJ_NUMBER; count++) {
      if (test_complete(str, maj_dinfo[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_MINDESG:
    /* check all of the minor designations */
    for (count = 0; count < MIN_NUMBER; count++) {
      if (test_complete(str, min_dinfo[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_DMODES:
    /* go through the list of display modes */
    for (dmode_tptr = dmode_list;
	 dmode_tptr != NULL;
	 dmode_tptr = dmode_tptr->next) {
      if (test_complete(str, dmode_tptr->d.name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_ALT:
    /* check all of the elevations */
    for (count = 0; count < ELE_NUMBER; count++) {
      if (test_complete(str, ele_info[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_VEG:
    /* check all of the vegetations */
    for (count = 0; count < VEG_NUMBER; count++) {
      if (test_complete(str, veg_info[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_TGOOD:
    /* check all of the tradegoods */
    for (count = 0; count < tgoods_number; count++) {
      if (test_complete(str, tg_info[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_TGCLASS:
    /* check all of the tradegoods */
    for (count = 0; count < tgclass_number; count++) {
      if (test_complete(str, tgclass_info[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_OPTION:
    /* check among the list of options */
    for (count = 0; count < options_number; count++) {
      if (test_complete(str, opt_list[count], bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_FUNCS:
    /* check among bindable functions */
    for (count = 0; count < global_int; count++) {
      if (test_complete(str, bind_func(count), bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  default:
    /* something funny here -- oh well */
    break;
  }

  /* check for no completions */
  if (matches == 0) return(FALSE);

  /* extend string as far as possible */
  end = strlen(cmpstr);
  for (count = strlen(str); count <= end; count++) {
    if (cmpstr[count] != '\0') addch(cmpstr[count]);
    str[count] = cmpstr[count];
  }

  /* check for non-unique completion */
  if (matches > 1) {

    /* end at last full completion if the list is too long */
    if ((int)strlen(bufstr) > COLS - 35) {
      for (count = COLS - 35; count > 0;
	   count--) {
	if (bufstr[count] != ' ') break;
      }
      bufstr[count] = '\0';
      strcat(bufstr, " ...");
    }

    /* save the current screen location */
    getyx(stdscr, yspot, xspot);

    /* display list of completions */
    snprintf(cmpstr, sizeof(cmpstr), "Matches: %.489s", bufstr);
    if (!isspace(count = errormsg(cmpstr))) {
      push_char(count);
    }

    /* return to the proper screen location */
    move(yspot, xspot);
  }

  refresh();
  return(TRUE);
}

/* BADFILECHAR -- Return TRUE for bad file name characters */
static
int badfilechar PARM_1(int, ch)
{
  if (isalnum(ch) ||
      (ch == '_')) {
    return(FALSE);
  }
  return(TRUE);
}

/* GET_STRING -- Read in a string of characters; Return length < NAMELTH
                 The parameter stype indicates type of input requested.
		 Return -1 and exit for escape key */
int
get_string PARM_3 (char *, str, int, stype, int, max_len)
{
  int i, ch;
  int done=0, count=0, xpos, ypos;

  /* obtain all user input */
  refresh();
  while (!done) {
    ch = next_char();
    if (ch == EXT_ESC) {
      /* check for the quick out */
      str[0] = '\0';
      no_input = TRUE;
      return(-1);
    } else if ((stype != STR_SPACE) &&
	       (ch == '\\')) {
      /* quoted input */
      addch('\\');
      refresh();
      getyx(stdscr, ypos, xpos);
      move(ypos, --xpos);
      ch = next_char();
      if (isprint(ch) &&
	  ((stype != STR_NAME) ||
	   !badfilechar(ch)) &&
	  (ch != '\t')) {
	if (count < max_len) {
	  if ((count == 0) &&
	      ((stype == STR_ARMY) ||
	       (stype == STR_ACLASS) ||
	       ((stype < STR_TGOOD) &&
		(stype >= STR_STATUS)))) {
	    /* first letter always upper case */
	    if (islower(ch)) {
	      ch = toupper(ch);
	    }
	  }
	  addch(ch);
	  str[count++] = ch;
	} else {
	  addch(' ');
	  move(ypos, xpos);
	  beep();
	}
      } else {
	addch(' ');
	move(ypos, xpos);
	beep();
      }
      refresh();
    } else if ((stype != STR_SPACE) &&
	       (ch == ' ' || ch == '?' || ch == '\t')) {
      /* check tabs, question marks and spaces */
      str[count] = '\0';
      if (stype != STR_NORM) {
	if (extend_str(str, stype)) {
	  count = strlen(str);
	} else {
	  getyx(stdscr, ypos, xpos);
	  if (!isspace(ch = errormsg("There are no matches"))) {
	    push_char(ch);
	  }
	  move(ypos, xpos);
	  refresh();
	}
      }
    } else if ((stype == STR_XNAME) &&
	       ((ch == ',') || (ch == '\n') || (ch == '\r'))) {
      /* the end... don't lose the character */
      push_char(ch);
      global_int = TRUE;
      done = TRUE;
    } else if (isprint(ch)) {
      /* only input displayable characters */
      if (count < max_len) {
	if ((count == 0) &&
	    ((stype == STR_ARMY) ||
	     (stype == STR_ACLASS) ||
	     ((stype < STR_TGOOD) &&
	      (stype >= STR_STATUS)))) {
	  /* first letter always upper case */
	  if (islower(ch)) {
	    ch = toupper(ch);
	  }
	}
	addch(ch);
	refresh();
	str[count++] = ch;
      } else {
	beep();
      }
    } else if (ch == CNTRL_U) {
      /* make sure that ^U works */
      getyx(stdscr, ypos, xpos);
      while (count > 0) {
	move(ypos,--xpos);
	addch(' ');
	count--;
      }
      move(ypos, xpos);
      refresh();
    } else if (ch == CNTRL_L || ch == CNTRL_R) {
      wrefresh(curscr);
    } else if ((ch == '\b' || ch == EXT_DEL) && count) {
      /* only delete what was printed */
      getyx(stdscr, ypos, xpos);
      move(ypos, --xpos);
      addch(' ');
      move(ypos, xpos);
      refresh();
      count--;
    } else if ((ch == '\n') || (ch == '\r')) {
      global_int = FALSE;
      for (i = count - 1; i > 0; i--) {
	if (str[i] != ' ') break;
	count--;
      }
      done = TRUE;
    }
  }
  str[count] = '\0';
  if (count == 0) {
    no_input = TRUE;
  } else {
    no_input = FALSE;
  }
  return(count);
}

/* GET_PASS -- Enter a string without echoing; Return length */
int
get_pass PARM_1 (char *, str)
{
  int ch;
  int done = FALSE, count = 0;

  refresh();
  no_input = FALSE;
  while(done == FALSE) {
    ch = next_char();
    if (ch == '\b' || ch == EXT_DEL) {
      /* delete any entered characters */
      if (count > 0) {
	count--;
      }
    } else if ((ch == CNTRL_L) || (ch == CNTRL_R)) {
      wrefresh(curscr);
    } else if (ch == EXT_ESC) {
      str[0] = '\0';
      no_input = TRUE;
      return(-1);
    } else if (ch == CNTRL_U) {
      /* make sure that ^U works */
      count = 0;
    } else if ((ch == '\n') || (ch == '\r')) {
      done = TRUE;
    } else if (ch != '\0') {
      /* add any other character to the string */
      if (count < PASSLTH) {
	/* don't try adding too many */
	str[count] = ch;
      }
      count++;
    }
  }

  /* truncate too long a password and end others properly */
  if (count < PASSLTH) {
    str[count] = '\0';
  } else {
    str[PASSLTH] = '\0';
  }
  if (count == 0) {
    no_input = TRUE;
  }
  return(count);
}

/* GET_ATYPE -- Return an army type interactively;
                   num_armytypes indicates invalid input  */
int
get_atype PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_ARMY, 20);
  if (no_input == TRUE) return(num_armytypes);

  /* find the match */
  for (i = 0; i < num_armytypes; i++) {
    if (str_test(buffer, ainfo_list[i].name) == 0) break;
  }

  /* give back the results */
  strcpy(string, buffer);
  return(i);
}

/* GET_ACLASS -- Return an army class; AC_NUMBER on invalid input */
int
get_aclass PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_ACLASS, 20);
  if (no_input == TRUE) return(num_aclasses);

  /* find the match */
  for (i = 0; i < num_aclasses; i++) {
    if (str_test(buffer, ainfo_clist[i]) == 0) break;
  }

  /* give back the results */
  strcpy(string, buffer);
  return(i);
}

/* GET_MONTH -- Return a month interactively */
int
get_month PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_MONTH, 20);
  if (no_input == TRUE) return(12);

  /* find the match */
  for (i = 0; i < 12; i++) {
    if (str_test(buffer, monthstr[i]) == 0) break;
  }

  /* give back the results */
  return(i);
}

/* GET_SPEED -- Return a speed value interactively */
int
get_speed PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_SPEED, 20);
  if (no_input == TRUE) return(SPD_NUMBER);

  /* find the match */
  for (i = SPD_MINIMUM; i < SPD_NUMBER; i++) {
    if (str_test(buffer, speedname[i]) == 0) break;
  }

  /* give back the results */
  return(i);
}

/* GET_STATUS -- Return a status value interactively */
int
get_status PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_STATUS, 20);
  if (no_input == TRUE) return(num_statuses);

  /* find the match */
  for (i = 0; i < num_statuses; i++) {
    if (str_test(buffer, stat_info[i].name) == 0) break;
  }

  /* give back the results */
  return(i);
}

/* ONE_CHAR -- get one character which is contained in the given set */
int
one_char PARM_2(char *, set, int, fold)
{
  char emsg[LINELTH];
  int ch, xpos, ypos;

  /* clean up and display selection */
  refresh();
  no_input = FALSE;
  if (set) sprintf(emsg, "Choose one of \"%s\"", set);

  /* must get one of the indicated set */
  for (;;) {

    /* get and check the input */
    ch = next_char();
    if (fold) {
      if (islower(ch)) {
	ch = toupper(ch);
      }
    }
    if ((ch == '\n') ||
	(ch == ' ') ||
	(ch == '\r')) {
      no_input = TRUE;
      break;
    }

    /* is it within the selection list */
    if (!set ||
	(set[0] == '\0') ||
	char_in_str(ch, set)) {
      /* got char in set or no set to limit it with. */
      break;
    }

    /* not in set; complain */
    getyx(stdscr, ypos, xpos);
    if (!isspace(ch = errormsg(emsg))) {
      push_char(ch);
    }
    move(ypos, xpos);
    refresh();
  }

  /* got it, now gone */
  return(ch);
}

/* HIP_STRING -- Highlight a character in parenthesis on true */
void
hip_string PARM_3(char *, str_in, int, keychar, int, bolden)
{
  int i;

  /* place the initial characters */
  for (i = 0; i < keychar - 1; i++) {
    if (str_in[i] == '\0') return;
    if (str_in[i] == '-') {
      addch(' ');
    } else {
      addch(str_in[i]);
    }
  }
  addch('(');
  if (bolden) standout();
  addch(str_in[i++]);
  if (bolden) standend();
  addch(')');
  for (; str_in[i] != '\0'; i++) {
    if (str_in[i] == '-') {
      addch(' ');
    } else {
      addch(str_in[i]);
    }
  }
}
