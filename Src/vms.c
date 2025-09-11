/*
 * VMS Platform-Specific Functions for CONQUER
 * 
 * Provides VAX/VMS-specific implementations of functions that are not
 * available or behave differently on VMS systems. Includes terminal
 * handling, password input, and formatted output compatibility.
 *
 * Original Author: John R Carr <JRCARR@IUP.BITNET>
 * Written for CONQUER
 */
#include "header.h"
#include "paramX.h"
#define PASSLTH 8
#ifdef VAXC
#include <stdio.h>
#include <string.h>
#include <varargs.h>
#include <curses.h>

/*
 * mvprintw - VAX C implementation of curses mvprintw function
 *
 * Provides a mvprintw function for VAX C systems that may not have
 * a complete curses implementation. Moves cursor to specified position
 * and prints formatted output using variable arguments.
 *
 * Parameters:
 *   y - Row position for cursor movement
 *   x - Column position for cursor movement  
 *   fspec - Format specification string (printf-style)
 *   va_alist - Variable argument list for format string
 *
 * Returns:
 *   int (return value not explicitly defined in this implementation)
 *
 * Side Effects:
 *   - Moves cursor to specified screen position
 *   - Outputs formatted text to screen at cursor position
 *   - Uses internal string buffer for formatting
 *
 * Notes:
 *   - VAX C specific implementation using varargs
 *   - Limited to 256 character output buffer
 *   - Compatibility function for incomplete curses libraries
 *   - Uses vsprintf for safe formatted string creation
 */
int
mvprintw(y, x, fspec, va_alist)
int y,x;
char *fspec;
va_dcl
{
  va_list varap;
  char string[256];

  string[0] = 0;

  move(y, x);
  va_start(varap);

  vsprintf(string, fspec, varap);
  printw("%s", string);
}

/*
 * getlogon - Retrieve VMS user login name
 *
 * Obtains the current user's login name from the VMS system and
 * processes it for use within the application. Handles VMS-specific
 * username format and null termination.
 *
 * Parameters:
 *   user - User identifier (purpose unclear in current implementation)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Calls cuserid() to get current user ID
 *   - Processes username string for proper termination
 *   - Copies processed name to temp2 (destination unclear)
 *
 * Notes:
 *   - VMS-specific user identification function
 *   - Implementation appears incomplete (temp2 not allocated)
 *   - May have been intended for user validation or logging
 *   - Limited to 13 character usernames (VMS limitation)
 *   - Contains potential buffer overflow (temp2 unallocated)
 */
void
getlogon PARM_1 (int, user)
{
  char temp[13];
  /* Note: This function appears incomplete - temp2 destination unclear */
  /* Commenting out unsafe strncpy to unallocated pointer */
  int i;

  cuserid(temp);
  for(i=0;i<13;i++){
    if(temp[i]=='\0') break;
  }
  /* TODO: strncpy(temp2, temp, i+1); - temp2 not allocated */
}

/*
 * getpass - VMS implementation of password input function
 *
 * Provides a VMS-specific implementation of the Unix getpass() function
 * for secure password input. Disables terminal echo during input to
 * prevent password display on screen.
 *
 * Parameters:
 *   prompt - String to display as password prompt
 *
 * Returns:
 *   Pointer to static buffer containing entered password
 *
 * Side Effects:
 *   - Temporarily disables terminal echo
 *   - Displays password prompt
 *   - Reads password input from user
 *   - Restores terminal echo
 *   - Uses static buffer for password storage
 *
 * Notes:
 *   - VMS-specific implementation using DCL commands
 *   - Limited to PASSLTH (8) characters
 *   - Uses system() calls for terminal control (potential security issue)
 *   - Static buffer means only one password at a time
 *   - gets() is unsafe and deprecated (buffer overflow risk)
 *   - Compatibility function for missing Unix getpass()
 */
char *
getpass PARM_1(char *, prompt)
{
  static unsigned char buffer[PASSLTH + 1];

  system("set term/noecho");
  printf("%s", prompt);
  if (fgets(buffer, PASSLTH, stdin) != NULL) {
    /* Remove trailing newline if present */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
      buffer[len-1] = '\0';
    }
  }
  system("set term/echo");
  return(buffer);
}
#endif /*VAXC*/

#ifdef VMS
#include <descrip.h>
#include <iodef.h>
#include <tt2def.h>

static $DESCRIPTOR (term_name, "SYS$INPUT:");
struct char_buffer_type { unsigned short int dummy;
			  unsigned short int size;
			  unsigned long int tchars;
			  unsigned long int tchars2; } oldbuf, newbuf;
short term_chan;

/*
 * setterm_pas - Configure VMS terminal for passthrough mode
 *
 * Sets up the VMS terminal for special character handling by configuring
 * terminal characteristics. Saves current settings and optionally enables
 * passthrough mode for terminal servers when TSERVER is defined.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Assigns terminal channel to SYS$INPUT
 *   - Reads current terminal characteristics into oldbuf
 *   - Copies current settings to newbuf for modification
 *   - Enables TT2$M_PASTHRU mode if TSERVER defined
 *   - Sets new terminal characteristics
 *   - Modifies global variables: term_chan, oldbuf, newbuf
 *
 * Notes:
 *   - VMS-specific terminal control using system services
 *   - Conditional passthrough mode for terminal server environments
 *   - Essential for proper terminal behavior in VMS curses applications
 *   - Paired with resetterm() for proper cleanup
 *   - Uses VMS sys$assign, sys$qiow system services
 */
void
setterm_pas()
{
  sys$assign(&term_name,&term_chan,0,0);

  sys$qiow(0,term_chan,IO$_SENSEMODE,0,0,0,&oldbuf,12,0,0,0,0);
  newbuf = oldbuf;

#ifdef TSERVER
  newbuf.tchars2 = newbuf.tchars2 | TT2$M_PASTHRU;
#endif /* TSERVER */

  sys$qiow(0,term_chan,IO$_SETMODE,0,0,0,&newbuf,12,0,0,0,0);
}

/*
 * resetterm - Restore original VMS terminal characteristics
 *
 * Restores the VMS terminal to its original state by reverting
 * the terminal characteristics that were saved by setterm_pas().
 * Essential cleanup function to prevent terminal corruption.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Restores terminal characteristics from oldbuf
 *   - Deassigns the terminal channel
 *   - Cleans up VMS system resources
 *
 * Notes:
 *   - Must be called to restore terminal after setterm_pas()
 *   - VMS-specific terminal cleanup using system services
 *   - Prevents terminal corruption on program exit
 *   - Uses VMS sys$qiow and sys$dassgn system services
 *   - Critical for proper VMS terminal session management
 */
void
resetterm()
{
  sys$qiow(0,term_chan,IO$_SETMODE,0,0,0,&oldbuf,12,0,0,0,0);
  sys$dassgn(term_chan);
}
#endif /* VMS */

/* Prevent empty translation unit warning on non-VMS systems */
#if !defined(VAXC) && !defined(VMS)
typedef int vms_dummy_type;
#endif
