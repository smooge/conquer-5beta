/* This file handles all of the time accessing checks for conquer */
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
/*
 *  Copyright (c) 1991 Tero T Mononen
 *  All rights reserved
 */

#include "dataG.h"

int time_check = TRUE;

#ifdef DO_TIME_CHECK
#include "keyvalsX.h"
#ifdef SYS_SIGNAL
#include <sys/signal.h>
#else
#include <signal.h>
#endif /* SYS_SIGNAL */
#include <sys/time.h>
#include <sys/resource.h>

#include <time.h>

#define TIME_CHECK_INTERVAL	(2 * 60)
#define TIME_UPDATE_WARN	(2 * 60)
#define TIME_CLOSING_WARN	(2 * 60)
#define LINE_LEN		100

#define TIME_HOURS_FILE 	"hours"
#define TIME_CLOSED		(-1)
#define TIME_DEFAULT_PRIORITY 	(4)

typedef enum { UPDATE=0, TIMELIMIT=1, CHECK=2 } alert_t;

typedef enum { SUN=0, MON=1, TUE=2, WED=3, THU=4, FRI=5, SAT=6, DIS=7
	       } daynum_t;

struct jtable_s {
  char 	*dayname;
  daynum_t	daynum;
} jtable[] = { 
  {"SUN", SUN},
  {"MON", MON},
  {"TUE", TUE},
  {"WED", WED},
  {"THU", THU},
  {"FRI", FRI},
  {"SAT", SAT},
  {"DIS", DIS},
  {NULL,  0}
};

/*
 * daynum - Convert day name string to numerical day code
 *
 * Converts a three-character day abbreviation (SUN, MON, TUE, etc.) to its
 * corresponding numerical daynum_t enumeration value. Used for parsing
 * the time configuration file to match current day with access rules.
 *
 * Parameters:
 *   day - Three-character day abbreviation string (case sensitive)
 *
 * Returns:
 *   Corresponding daynum_t enumeration value (0-6 for SUN-SAT, 7 for DIS)
 *   TIME_CLOSED (-1) if day string doesn't match any known day abbreviation
 *
 * Side Effects:
 *   None - read-only function
 *
 * Notes:
 *   - Uses exact 3-character string comparison via strncmp
 *   - Day abbreviations must match jtable entries exactly
 *   - DIS (disabled) is special case for host-specific restrictions
 */
static int
daynum PARM_1(char *, day) 
{
  int i = 0;
  for(i = 0; jtable[i].dayname != NULL; i++) {
    if(!strncmp(day, jtable[i].dayname, 3)) {
      return(jtable[i].daynum);
    } 
  }
  return((daynum_t)TIME_CLOSED);
}

static alert_t alertn = CHECK;

/*
 * tc_quit - Emergency exit from time check system
 *
 * Performs an emergency shutdown of the game when time check system
 * encounters a fatal error (such as setitimer failure or signal setup
 * problems). Ensures proper cleanup of nation data before terminating.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Does not return - terminates program via bye()
 *
 * Side Effects:
 *   - Closes current nation file if country is active (country != -1)
 *   - Calls bye() to terminate the entire program
 *   - Updates nation status to indicate emergency shutdown
 *
 * Notes:
 *   - Used when time check system fails to initialize properly
 *   - Ensures data integrity by properly closing nation files
 *   - Does not return to caller - program terminates completely
 */
static void
tc_quit PARM_0 (void)
{
  if (country != -1) {
    close_ntn(country, nationname);
  }
  bye();
  /*NOTREACHED*/
}

/*
 * alert - Set up interval timer for time check system
 *
 * Configures a SIGALRM signal to be delivered after specified seconds using
 * setitimer(). Sets up both initial timer value and interval for repeated
 * alarms. Used to implement periodic time checking and game access control.
 *
 * Parameters:
 *   sec - Number of seconds until first alarm and between subsequent alarms
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Sets up ITIMER_REAL timer with specified interval
 *   - Overwrites any existing timer configuration
 *   - Calls tc_quit() on setitimer failure (program termination)
 *
 * Notes:
 *   - Timer delivers SIGALRM signal when it expires
 *   - Both initial delay and repeat interval are set to same value
 *   - Fatal error handling - terminates program if timer setup fails
 *   - Uses system interval timer for precise timing control
 */
static void
alert PARM_1(long, sec)
{
  struct itimerval value;
  value.it_value.tv_sec = sec;
  value.it_value.tv_usec = 0;
  value.it_interval.tv_sec = sec;
  value.it_interval.tv_usec = 0;
  if (setitimer(ITIMER_REAL, &value, (struct itimerval *)NULL) != 0) {
    perror("Setitimer failed");
    tc_quit();
  }
}

/*
 * upcase - Convert string to uppercase in place
 *
 * Converts all lowercase letters in the input string to uppercase letters.
 * Modifies the original string directly rather than creating a copy.
 * Used for normalizing day names when parsing time configuration file.
 *
 * Parameters:
 *   str - Null-terminated string to convert to uppercase (modified in place)
 *
 * Returns:
 *   Pointer to the modified input string (same as str parameter)
 *
 * Side Effects:
 *   - Modifies the input string by converting lowercase to uppercase
 *   - Only affects alphabetic characters that are lowercase
 *   - Non-alphabetic characters remain unchanged
 *
 * Notes:
 *   - Uses islower() and toupper() from ctype.h for character testing/conversion
 *   - In-place modification for memory efficiency
 *   - Safe for strings with mixed case, numbers, and special characters
 */
static char *
upcase PARM_1(char *, str)
{
  char *p;
  for(p = str; *p; p++)
    if (islower(*p))
      *p = toupper(*p);
  return str;
}

#define SKIP_SPACE(p) do { for(; *p == ' ' || *p == '\t'; p++) \
			     ; \
			     } while(0)
#define SKIP_NONSPACE(p) do { for(; *p != ' ' && *p != '\t' && *p; p++) \
			     ; \
			     } while(0)
static char host[64];


/*
 * priority - Parse time configuration file and determine current game access status
 *
 * Reads the "hours" configuration file to determine the current game access
 * status based on day of week, hour, and minute. The file format contains
 * lines with day abbreviations followed by 48 characters representing 24 hours
 * (2 characters per hour - priority and update minute). Also handles host-specific
 * restrictions using DIS entries.
 *
 * Parameters:
 *   None - uses current system time
 *
 * Returns:
 *   Character code indicating current status:
 *   'O' - Error opening time file
 *   'A' - Game access denied for this host (DIS entry match)
 *   'U' - Update is starting (minute matches update field)
 *   'N' - Normal access with default priority
 *   '.' - Game allowed with normal status
 *   'X' - Game is closed (default if no day match found)
 *   '0'-'9' - Game allowed with specified priority level
 *   'I'/'i' - Illegal character in time file format
 *   'S' - Syntax error in time file (line too short)
 *
 * Side Effects:
 *   - Opens and reads TIME_HOURS_FILE ("hours")
 *   - May display error messages via bottommsg()
 *   - Closes file on successful day match
 *
 * Notes:
 *   - Time file format: "DAY NNNN..." where N is priority/update character
 *   - Each hour uses 2 characters: priority (first) and update minute (second)
 *   - Update minute field (second char): '0'-'5' indicates 10-minute intervals
 *   - Priority field (first char): '.', 'N', 'X', or '0'-'9'
 *   - Host restrictions use "DIS hostname" format
 *   - Requires exactly 47+ characters in time field (24 hours * 2 chars - 1)
 */
static int
priority PARM_0(void)
{
  FILE *in;
  char line[LINE_LEN];
  long clock_val;
  struct tm *t;
  char *p, *day, *values;
  char c1, c2;
  int d;

  clock_val = time(NULL);
  in = fopen(TIME_HOURS_FILE, "r");
  if (in == NULL) {
    return('O');
  }
  t = localtime(&clock_val);
  while (fgets(line, LINE_LEN, in) != NULL) {
    p = line;
    SKIP_SPACE(p);
    day = p;
    SKIP_NONSPACE(p);
    *p++ = '\0';
    d = daynum(upcase(day));

    if (d == DIS) {
      SKIP_SPACE(p);
      values = p;
      if (strncmp(host, values, strlen(host)) == 0) {
	return 'A';
      }
    }
    if (t->tm_wday == d) {
      fclose(in);
      SKIP_SPACE(p);
      values = p;
      if (strlen(values) >= 47)	{
	c1 = values[2 * t->tm_hour];
	c2 = values[2 * t->tm_hour + 1];
	if (c2 != ' ') {
	  if (c2 < '0' || c2 > '5') {
	    bottommsg("Illegal character in update field");
	    return('I');
	  }
	  if (t->tm_min >= 10 * (c2 - '0') &&
	      t->tm_min < 10 * (c2 - '0' + 1)) {
	    return('U');
	  }
	}
	if (index(".NX0123456789", c1) != NULL) {
	  return (c1);
	} else {
	  bottommsg("Illegal character in first field");
	  return('i');
	}
      } else {
	bottommsg("Syntax error in time file");
	return('S');
      }
    }
  }
  return 'X';
}

/*
 * check_play - Periodic game access and priority checking
 *
 * Called periodically by timer to check current game access status and
 * adjust process priority, set warning timers, or initiate shutdown
 * procedures. Handles all priority() return codes and implements
 * appropriate responses including user notifications and system actions.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - May modify process priority via setpriority()
 *   - Sets timer alerts for various warning periods
 *   - May display status messages via bottommsg()
 *   - Can terminate program via tc_quit() for host restrictions
 *   - Updates alertn global variable to control timer behavior
 *   - Maintains static error counter for file open failures
 *
 * Notes:
 *   - Handles all priority() return codes with appropriate actions
 *   - 'X' (closed): Sets TIMELIMIT alert for shutdown warning
 *   - 'N' (normal): Sets default priority and normal check interval
 *   - 'A' (access denied): Terminates program if time_check enabled
 *   - 'U' (update): Sets UPDATE alert for update warning
 *   - '0'-'9': Sets priority based on numeric value (2 * digit)
 *   - 'O' (open error): Retries up to 3 times before giving up
 *   - Error conditions ('i','I','S'): No action taken
 *   - Uses different timer intervals for different warning types
 */
static void
check_play PARM_0(void)
{
  static int openerrcnt = 0;
  int pr;

  switch(pr = priority()) {
  case 'X':
    openerrcnt = 0;
    alertn = TIMELIMIT;
    bottommsg("Game is closing, please quit");
    alert(TIME_CLOSING_WARN);
    break;
  case 'N':
    openerrcnt = 0;
#ifdef SETPRIORITY
    setpriority(PRIO_PROCESS, 0, TIME_DEFAULT_PRIORITY);
#endif /* SETPRIORITY */
    alert(TIME_CHECK_INTERVAL);
    break;
  case 'A':
    if (time_check == TRUE) {
      bottommsg("Game has been disabled in this host");
      tc_quit();
    }
    alert(TIME_CHECK_INTERVAL);
    break;
  case 'i':
  case 'I':
  case 'S':
    break;
  case 'O':
    if (openerrcnt == 0) {
      bottommsg("Error opening time file");
    }
    if (openerrcnt < 3) {
      alert(TIME_CHECK_INTERVAL);
      openerrcnt++;
    }
    break;
  case '.':
    openerrcnt = 0;
    alert(TIME_CHECK_INTERVAL);
    break;
  case 'U':
    openerrcnt = 0;
    alertn = UPDATE;
    bottommsg("Game is starting update soon.");
    alert(TIME_UPDATE_WARN);
    break;
  default:
    openerrcnt = 0;
#ifdef SETPRIORITY
    setpriority(PRIO_PROCESS, 0, (int)(2 * (pr - '0')));
#endif /* SETPRIORITY */
    alert(TIME_CHECK_INTERVAL);
    break;
  }
}

/*
 * doupexit - Handle game update notification and exit procedure
 *
 * Called when the game update is about to start. If time checking is
 * enabled, terminates the game session to allow the update to proceed.
 * If time checking is disabled, only displays a notification message.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Does not return if time_check is TRUE (calls hangup())
 *   Returns normally if time_check is FALSE
 *
 * Side Effects:
 *   - Displays update notification via bottommsg()
 *   - May terminate game session via hangup() if time_check enabled
 *   - Forces player logout to prevent interference with update process
 *
 * Notes:
 *   - Behavior controlled by global time_check flag
 *   - Update warnings are given before this function is called
 *   - Ensures clean game state before system update begins
 *   - Uses hangup() for immediate session termination
 */
static void
doupexit PARM_0(void)
{
  if (time_check == FALSE) {
    bottommsg("Update is starting.");
    return;
  }
  bottommsg("Exiting for a update");
  hangup();
  /*NOTREACHED*/
}

/*
 * doexit - Handle game closure notification and exit procedure
 *
 * Called when the game is closing down for the day/period according to
 * the time configuration. If time checking is enabled, terminates the
 * game session. If disabled, only displays a notification message.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Does not return if time_check is TRUE (calls hangup())
 *   Returns normally if time_check is FALSE
 *
 * Side Effects:
 *   - Displays closure notification via bottommsg()
 *   - May terminate game session via hangup() if time_check enabled
 *   - Forces player logout when game hours end
 *
 * Notes:
 *   - Behavior controlled by global time_check flag
 *   - Called when time configuration indicates game should be closed
 *   - Ensures players are logged out during non-gaming hours
 *   - Uses hangup() for immediate session termination
 */
static void
doexit PARM_0(void)
{
  if (time_check == FALSE) {
    bottommsg("Game is closed down");
    return;
  }
  bottommsg("Game is closed down");
  hangup();
  /*NOTREACHED*/
}

/*
 * alrm_handler - Signal handler for SIGALRM timer events
 *
 * Handles SIGALRM signals generated by the interval timer to perform
 * time-based game management tasks. Dispatches to appropriate handler
 * function based on the current alert type (UPDATE, TIMELIMIT, or CHECK).
 *
 * Parameters:
 *   None (signal handler signature)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Resets signal handler to itself for subsequent alarms
 *   - May trigger game exit procedures via doupexit() or doexit()
 *   - May perform periodic checking via check_play()
 *   - Global alertn variable determines which action to take
 *
 * Notes:
 *   - Called automatically by system when SIGALRM is delivered
 *   - Must be async-signal-safe (limited function calls allowed)
 *   - Signal handler reset needed for reliable operation
 *   - Three alert types:
 *     UPDATE: Game update is starting - call doupexit()
 *     TIMELIMIT: Game hours ended - call doexit()
 *     CHECK: Periodic status check - call check_play()
 *   - Error handling minimal due to signal context restrictions
 */
static void
alrm_handler PARM_0(void)
{
  /* reset the alarm, can't worry about failure here */
  signal(SIGALRM, alrm_handler);
  switch(alertn) {
  case UPDATE:
    doupexit();
    break;
  case TIMELIMIT:
    doexit();
    break;
  case CHECK:
    check_play();
    break;
  }
}
#endif /* DO_TIME_CHECK */

/*
 * init_time_check - Initialize the time checking and access control system
 *
 * Sets up the timer-based time checking system if a time configuration file
 * exists. Installs the SIGALRM signal handler and starts the initial timer
 * for periodic access control checking.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Checks for existence of TIME_HOURS_FILE ("hours")
 *   - Installs alrm_handler as SIGALRM signal handler
 *   - Starts timer with TIME_CHECK_INTERVAL (2 minutes)
 *   - May terminate program via tc_quit() if signal setup fails
 *
 * Notes:
 *   - Only active if DO_TIME_CHECK is defined at compile time
 *   - Requires "hours" file to exist for time checking to be enabled
 *   - Signal handler setup failure is fatal error
 *   - Initial timer starts the periodic checking cycle
 *   - Called once during game initialization
 */
void
init_time_check PARM_0(void)
{
#ifdef DO_TIME_CHECK
  if (exists(TIME_HOURS_FILE) == 0) {
    if ((int) signal(SIGALRM, alrm_handler) == -1) {
      bottommsg("Signal failed");
      tc_quit();
    }
    alert(TIME_CHECK_INTERVAL);
  }
#endif /* DO_TIME_CHECK */
}

/*
 * initial_check - Perform initial game access validation before startup
 *
 * Checks if the game should be allowed to start based on current time,
 * day of week, and host restrictions. Called once during game startup
 * to determine if the player should be allowed to connect.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 - Game access allowed, continue with startup
 *   1 - Game is closed or update starting, deny access
 *   -1 - Host access denied, prevent startup
 *
 * Side Effects:
 *   - Gets hostname and stores in global host variable
 *   - May set process priority via setpriority()
 *   - Displays status messages via bottommsg()
 *   - Reads and parses time configuration file
 *
 * Notes:
 *   - Only active if DO_TIME_CHECK is defined at compile time
 *   - Returns 0 immediately if TIME_HOURS_FILE doesn't exist
 *   - Behavior controlled by global time_check flag
 *   - Different return codes for different restriction types:
 *     'X' (closed): return 1 if time_check enabled
 *     'A' (access denied): return -1 if time_check enabled
 *     'U' (update starting): return 1 if time_check enabled
 *     'N' (normal): sets default priority, continues
 *     '0'-'9': sets priority based on digit value, continues
 *     Error conditions: continue with startup
 *   - Host restriction check using gethostname()
 */
int
initial_check PARM_0(void)
{
#ifdef DO_TIME_CHECK
  int pr;

  /* is there an hours file in place? */
  if (exists(TIME_HOURS_FILE) != 0) return(0);

  /* perform the check */
  gethostname(host, 64);
  switch(pr = priority()) {
  case 'X':
    bottommsg("Game is closed");
    if (time_check == TRUE) {
      return(1);
    }
    break;
  case 'N':
#ifdef SETPRIORITY
    setpriority(PRIO_PROCESS, 0, TIME_DEFAULT_PRIORITY);
#endif /* SETPRIORITY */
    break;
  case 'A':
    bottommsg("Game is not allowed in this host");
    if (time_check == TRUE) {
      return(-1);
    }
    break;
  case 'i':
  case 'I':
  case 'S':
    break;
  case 'O':
    break;
  case '.':
    break;
  case 'U':
    bottommsg("Update Starting really soon...");
    if (time_check == TRUE) {
      return(1);
    }
    break;
  default:
#ifdef SETPRIORITY
    setpriority(PRIO_PROCESS, 0, (int)(2 * (pr - '0')));
#endif /* SETPRIORITY */
    break;
  }
#endif /* DO_TIME_CHECK */
  return 0;
}
