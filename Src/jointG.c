/* user interface routines that need to be "defined" for both */
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
#include "armyX.h"
#include "moveX.h"
#include "desigX.h"
#include "magicX.h"
#include "spellsX.h"
#include "tgoodsX.h"
#include "displayX.h"
#include "optionsX.h"

/*
 * target_value - Convert string to target value based on highlight style
 *
 * Parses a string representation of a target value according to the specified
 * highlight style type. Supports mineral designations, major designations,
 * nation ownership, and trade goods as target types. Used for configuring
 * display highlighting targets in the hexagonal map interface.
 *
 * Parameters:
 *   hstyle - Highlight style type (HI_MINDESG, HI_MAJDESG, HI_OWN, HI_TGOODS)
 *   str - String representation of the target value to parse
 *
 * Returns:
 *   Target value index on successful match, -2 on no match found, 0 for
 *   unknown highlight styles. Special values: UNOWNED for "*" with HI_OWN,
 *   TG_NONE for "*" with HI_TGOODS
 *
 * Side Effects:
 *   None - pure parsing function without state modification
 *
 * Notes:
 *   - Static function used internally by display setup routines
 *   - Uses str_test() for partial string matching on names
 *   - Handles special wildcard "*" syntax for ownership and trade goods
 *   - Case-sensitive string matching for exact target identification
 *   - Returns -2 specifically to indicate "no match" vs valid index 0
 *   - Uses PARM_X macro for K&R style function parameters
 */
static int
target_value PARM_2(int, hstyle, char *, str)
{
  int i;

  switch (hstyle) {
  case HI_MINDESG:
    for (i = 0; i < MIN_NUMBER; i++) {
      if (str_test(min_dinfo[i].name, str) == 0) {
	return(i);
      }
    }
    break;
  case HI_MAJDESG:
    for (i = 0; i < MAJ_NUMBER; i++) {
      if (str_test(maj_dinfo[i].name, str) == 0) {
	return(i);
      }
    }
    break;
  case HI_OWN:
    if (strcmp(str, "*") == 0) return(UNOWNED);
    for (i = 0; i < MAXNTN; i++) {
      if (str_test((world.np[i])->name, str) == 0) {
	return(i);
      }
    }
    break;
  case HI_TGOODS:
    if (strcmp(str, "*") == 0) return(TG_NONE);
    for (i = 0; i < tgclass_number; i++) {
      if (str_test(tgclass_info[i].name, str) == 0) {
	return(i);
      }
    }
    break;
  default:
    /* targeting value of zippo */
    return(0);
    /*NOTREACHED*/
    break;
  }

  /* no match */
  return(-2);
}

/*
 * dflt_disp_setup - Configure default display mode settings
 *
 * Parses and applies configuration settings for the default display mode.
 * Handles highlight styles, target specifications, focus positions, and
 * display styles for different hexagonal map positions. Provides comprehensive
 * error checking and validation for configuration file parsing.
 *
 * Parameters:
 *   str - Configuration string to parse
 *   fstr - Source filename for error reporting
 *   lnum - Line number in configuration file for error reporting
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Modifies global display_mode structure settings
 *   - Updates highlight styles, targets, focus, and display styles
 *   - Outputs error messages for invalid configuration syntax
 *   - Processes highlight, target, focus, and position directives
 *
 * Notes:
 *   - Parses "highlight [position] [style]" directives
 *   - Handles "target [position] [value]" specifications
 *   - Processes "focus [position]" focus point settings
 *   - Supports position-specific display style configuration
 *   - Validates all configuration parameters with detailed error messages
 *   - Uses global arrays: hex_list, highl_list, display_list for validation
 *   - Integrates with target_value() for target parsing
 *   - Uses PARM_X macro for K&R style function parameters
 */
void
dflt_disp_setup PARM_3(char *, str, char *, fstr, int, lnum)
{
  int fposit, len, dstyle, hstyle, targ;

  /* parse the string and break it into the appropriate parts */
  if (strncmp(str, "highlight", 9) == 0) {

    /* clear out the front */
    str += 9;
    trim_str(str, FALSE);

    /* now check for the different positions */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was it invalid? */
    if (fposit == HXPOS_NUMBER) {

      /* invalid */
      sprintf(string, "%s [%d]: unknown position for highlight",
	      fstr, lnum);
      errormsg(string);

    } else {

      /* clean up the string for parsing */
      str += len;
      trim_str(str, FALSE);

      /* now locate the highlight type */
      for (hstyle = 0; hstyle <= HI_MAXIMUM; hstyle++) {
	if (strcmp(str, highl_list[hstyle]) == 0) break;
      }

      /* valid or not? */
      if (hstyle == HI_MAXIMUM + 1) {
	sprintf(string, "%s [%d]: unknown highlighting style option",
		fstr, lnum);
	errormsg(string);
      } else {
	/* do it */
	display_mode.highlight[fposit] = hstyle;
      }
    }

  } else if (strncmp(str, "target", 6) == 0) {

    /* clear out the front */
    str += 6;
    trim_str(str, FALSE);

    /* now check for the different positions */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was it invalid? */
    if (fposit == HXPOS_NUMBER) {

      /* invalid */
      sprintf(string, "%s [%d]: unknown position for target",
	      fstr, lnum);
      errormsg(string);

    } else if (hl_targets(display_mode.highlight[fposit]) == FALSE) {

      /* no target highlighting */
      sprintf(string, "%s [%d]: %s highlight doesn't need target",
	      fstr, lnum, hex_list[fposit]);
      errormsg(string);

    } else {

      /* clean up the string for parsing */
      str += len;
      trim_str(str, FALSE);

      /* now find the target value */
      if ((targ = target_value(display_mode.highlight[fposit], str))
	  != -2) {
	display_mode.target[fposit] = targ;
      }
    }

  } else if (strncmp(str, "focus", 5) == 0) {

    /* clear out start of string */
    str += 5;
    trim_str(str, FALSE);

    /* need to find the match for the focus position */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was there a match? */
    if (fposit == HXPOS_NUMBER) {

      /* bad setting */
      sprintf(string, "%s [%d]: unknown focus position",
	      fstr, lnum);
      errormsg(string);

    } else {
      /* got it */
      display_mode.focus = fposit;
    }

  } else {
    /* now check for the different positions */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was it invalid? */
    if (fposit == HXPOS_NUMBER) {

      /* invalid */
      sprintf(string, "%s [%d]: unknown default-display option",
	      fstr, lnum);
      errormsg(string);

    } else {

      /* clean up the string for parsing */
      str += len;
      trim_str(str, FALSE);

      /* find the display mode */
      for (dstyle = 0; dstyle <= DI_MAXIMUM; dstyle++) {
	if (strcmp(str, display_list[dstyle]) == 0) break;
      }

      /* valid or not? */
      if (dstyle == DI_MAXIMUM + 1) {
	sprintf(string, "%s [%d]: unknown display style option",
		  fstr, lnum);
	errormsg(string);
      } else {
	/* do it */
	display_mode.style[fposit] = dstyle;
      }
      
    }
  }
}

/*
 * display_setup - Configure named display modes
 *
 * Creates and configures named display modes with custom settings. Parses
 * display mode names from quoted strings and applies configuration settings
 * for highlight styles, targets, focus positions, and display styles.
 * Provides comprehensive display mode management with error validation.
 *
 * Parameters:
 *   str - Configuration string containing display mode name and settings
 *   fstr - Source filename for error reporting
 *   lnum - Line number in configuration file for error reporting
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Creates new display modes via crt_dmode() if they don't exist
 *   - Modifies display mode structures with new configuration settings
 *   - Updates highlight styles, targets, focus, and display styles for modes
 *   - Outputs error messages for invalid configuration syntax
 *
 * Notes:
 *   - Expects display mode name in quotes: "modename" [settings]
 *   - Supports same configuration syntax as dflt_disp_setup()
 *   - Creates custom display modes that can be switched between during play
 *   - Validates display mode name extraction and configuration parameters
 *   - Integrates with display mode management system
 *   - Uses target_value() for target specification parsing
 *   - Handles highlight, target, focus, and position-specific directives
 *   - Uses PARM_X macro for K&R style function parameters
 */
void
display_setup PARM_3(char *, str, char *, fstr, int, lnum)
{
  DMODE_PTR work_ptr;
  int fposit, len = 0, dstyle, hstyle, targ;
  char dmodename[DISPLAYLTH+1];

  /* locate the name of the display */
  for (; *str != '\0'; str++) {
    if (*str == '"') break;
  }
  if (*str == '\0') return;
  for (str++; (*str != '\0') && (*str != '"'); str++) {
    dmodename[len++] = *str;
  }

  /* check it */
  if (*str == '\0') {
    /* just weren't nothing there */
    sprintf(string, "%s [%d]: could not find display mode name",
	    fstr, lnum);
    errormsg(string);
    return;
  }

  /* end name and skip over final quote */
  dmodename[len] = '\0';
  str++;
  trim_str(str, FALSE);

  /* now find/create the display mode */
  if ((work_ptr = crt_dmode(dmodename)) == NULL) {
    return;
  }

  /* parse the string and break it into the appropriate parts */
  if (strncmp(str, "highlight", 9) == 0) {

    /* clear out the front */
    str += 9;
    trim_str(str, FALSE);

    /* now check for the different positions */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was it invalid? */
    if (fposit == HXPOS_NUMBER) {

      /* invalid */
      sprintf(string, "%s [%d]: unknown position for highlight",
	      fstr, lnum);
      errormsg(string);

    } else {

      /* clean up the string for parsing */
      str += len;
      trim_str(str, FALSE);

      /* now locate the highlight type */
      for (hstyle = 0; hstyle <= HI_MAXIMUM; hstyle++) {
	if (strcmp(str, highl_list[hstyle]) == 0) break;
      }

      /* valid or not? */
      if (hstyle == HI_MAXIMUM + 1) {
	sprintf(string, "%s [%d]: unknown highlighting style option",
		fstr, lnum);
	errormsg(string);
      } else {
	/* do it */
	work_ptr->d.highlight[fposit] = hstyle;
      }
    }

  } else if (strncmp(str, "target", 6) == 0) {

    /* clear out the front */
    str += 6;
    trim_str(str, FALSE);

    /* now check for the different positions */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was it invalid? */
    if (fposit == HXPOS_NUMBER) {

      /* invalid */
      sprintf(string, "%s [%d]: unknown position for target",
	      fstr, lnum);
      errormsg(string);

    } else if (hl_targets(work_ptr->d.highlight[fposit]) == FALSE) {

      /* no target highlighting */
      sprintf(string, "%s [%d]: %s highlight doesn't need target",
	      fstr, lnum, hex_list[fposit]);
      errormsg(string);

    } else {

      /* clean up the string for parsing */
      str += len;
      trim_str(str, FALSE);

      /* now find the target value */
      if ((targ = target_value(work_ptr->d.highlight[fposit], str))
	  != -2) {
	work_ptr->d.target[fposit] = targ;
      }
    }

  } else if (strncmp(str, "focus", 5) == 0) {

    /* clear out start of string */
    str += 5;
    trim_str(str, FALSE);

    /* need to find the match for the focus position */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was there a match? */
    if (fposit == HXPOS_NUMBER) {

      /* bad setting */
      sprintf(string, "%s [%d]: unknown focus position",
	      fstr, lnum);
      errormsg(string);

    } else {
      /* got it */
      work_ptr->d.focus = fposit;
    }

  } else {
    /* now check for the different positions */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was it invalid? */
    if (fposit == HXPOS_NUMBER) {

      /* invalid */
      sprintf(string, "%s [%d]: unknown display-mode option",
	      fstr, lnum);
      errormsg(string);

    } else {

      /* clean up the string for parsing */
      str += len;
      trim_str(str, FALSE);

      /* find the display mode */
      for (dstyle = 0; dstyle <= DI_MAXIMUM; dstyle++) {
	if (strcmp(str, display_list[dstyle]) == 0) break;
      }

      /* valid or not? */
      if (dstyle == DI_MAXIMUM + 1) {
	sprintf(string, "%s [%d]: unknown display style option",
		fstr, lnum);
	errormsg(string);
      } else {
	/* do it */
	work_ptr->d.style[fposit] = dstyle;
      }
      
    }
  }
}

/*
 * keysys_setup - Configure keybinding systems
 *
 * Comprehensive keybinding configuration system that manages key mappings
 * for multiple interface subsystems. Supports binding, rebinding, and
 * unbinding keys to functions across global, email, reader, magic, and
 * other specialized key mapping systems. Handles escape sequence processing
 * and function validation.
 *
 * Parameters:
 *   action - Action type (OPT_BINDKEY, OPT_REBIND, OPT_UNBIND)
 *   str - Configuration string containing keysystem, key, and function
 *   fstr - Source filename for error reporting
 *   lnum - Line number in configuration file for error reporting
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Modifies keybinding lists for specified subsystems
 *   - Creates new key bindings or updates existing ones
 *   - Removes key bindings when unbinding
 *   - Outputs error messages for invalid configurations
 *
 * Notes:
 *   - Supports multiple keysystems: global, email, reader, magic, mparse, ninfo, xfer
 *   - Parses quoted key sequences with escape sequence support
 *   - Validates function names against subsystem-specific function lists
 *   - Handles key binding creation, modification, and removal
 *   - Uses convert_kbind() for escape sequence processing
 *   - Integrates with func_match() for function name validation
 *   - Manages separate binding lists for each keysystem
 *   - Uses PARM_X macro for K&R style function parameters
 */
void
keysys_setup PARM_4(int, action, char *, str, char *, fstr, int, lnum)
{
  KEYSYS_PTR curmap_keysys;
  KLIST_PTR *list_of_keys, klist_ptr;
  FNCI func_ptr;
  char start_ch, ksysname[LINELTH], keystr[LINELTH];
  int len = 0;

  /* first find the type of the keybindings */
  for (; !isspace(*str); str++) {
    ksysname[len++] = *str;
  }
  ksysname[len] = '\0';

  /* check if we found a system name */
  if (len == 0) {
    sprintf(string, "%s [%d]: could not find keypad type",
	    fstr, lnum);
    errormsg(string);
  }
  len = 0;

  /* now compare with the different types */
  if (strcmp(ksysname, global_keysys.name) == 0) {
    curmap_keysys = &global_keysys;
    list_of_keys = &cq_bindings;
  } else if (strcmp(ksysname, email_keysys.name) == 0) {
    curmap_keysys = &email_keysys;
    list_of_keys = &email_bindings;
  } else if (strcmp(ksysname, reader_keysys.name) == 0) {
    curmap_keysys = &reader_keysys;
    list_of_keys = &reader_bindings;
  } else if (strcmp(ksysname, magic_keysys.name) == 0) {
    curmap_keysys = &magic_keysys;
    list_of_keys = &mg_bindings;
  } else if (strcmp(ksysname, mparse_keysys.name) == 0) {
    curmap_keysys = &mparse_keysys;
    list_of_keys = &mparse_bindings;
  } else if (strcmp(ksysname, ninfo_keysys.name) == 0) {
    curmap_keysys = &ninfo_keysys;
    list_of_keys = &ni_bindings;
  } else if (strcmp(ksysname, xfer_keysys.name) == 0) {
    curmap_keysys = &xfer_keysys;
    list_of_keys = &xfer_bindings;
  } else {
    sprintf(string, "%s [%d]: unknown key map type encountered",
	    fstr, lnum);
    errormsg(string);
    return;
  }
  
  /* check the beginning of the line */
  trim_str(str, FALSE);
  if (((start_ch = *str) != '"') &&
      (start_ch != '\'')) {
    sprintf(string, "%s [%d]: could not find key binding",
	    fstr, lnum);
    errormsg(string);
    return;
  }

  /* get the keybinding */
  for (str++; (*str != '\0') && (*str != start_ch); str++) {
    if ((*str == '\\') && (*(str + 1) != '\0')) {
      keystr[len++] = *str;
      str++;
    }
    keystr[len++] = *str;
  }
  keystr[len] = '\0';

  /* was it found? */
  if (*str == '\0') {
    sprintf(string, "%s [%d]: could not find key binding",
	    fstr, lnum);
    errormsg(string);
    return;
  }

  /* clean up the key binding */
  convert_kbind(keystr);

  /* clean up the string, which should now only hold a function, if needed */
  str++;
  trim_str(str, FALSE);

  /* separate based on type */
  switch (action) {
  case OPT_BINDKEY:
  case OPT_REBIND:
    /* (re)bind the key */
    klist_ptr = find_keys(*list_of_keys, keystr);

    /* get the function */
    if ((func_ptr = func_match(str, curmap_keysys->parse_p,
			       curmap_keysys->num_parse)) == NULL) {
      sprintf(string, "%s [%d]: unknown %s function", fstr,
	      lnum, ksysname);
      errormsg(string);
      break;
    }

    /* now do the binding */
    if (klist_ptr == NULL) {
      bind_keys(list_of_keys, keystr, func_ptr);
    } else {
      klist_ptr->key_data.func = func_ptr;
    }
    break;
  case OPT_UNBIND:
    /* nuke it */
    (void) rm_keys(list_of_keys, keystr);
    break;
  default:
    /* strange */
    break;
  }
}

/*
 * check_spells - Validate spell number and report errors
 *
 * Simple validation function that checks if a spell number is within
 * the valid range of defined spells. Reports error messages with location
 * information when invalid spell numbers are encountered, typically during
 * map data loading or spell processing.
 *
 * Parameters:
 *   spellnum - Spell number to validate
 *   xloc - X coordinate for error reporting context
 *   yloc - Y coordinate for error reporting context
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Outputs error message if spell number is invalid
 *   - Uses global string buffer for error message formatting
 *
 * Notes:
 *   - Validates against global spell_number constant
 *   - Provides location context for debugging invalid spell references
 *   - Used during map data processing and spell system validation
 *   - Simple bounds checking without complex spell validation logic
 *   - Uses PARM_X macro for K&R style function parameters
 */
void
check_spells PARM_3(int, spellnum, int, xloc, int, yloc)
{
  /* check it */
  if (spellnum > spell_number) {
    /* unknown spell? */
    sprintf(string, "Error: Unknown spell #%d in [%d,%d]",
	    spellnum, xloc, yloc);
    errormsg(string);
  }
}

/* temporary global to enable transition of function lists */
PARSE_PTR tmp_parsep;

/*
 * bind_func - Return function name string for given function index
 *
 * Utility function that returns the real name string for a function based
 * on its index in the current parser function list. Used for keybinding
 * display and configuration management. Provides bounds checking and
 * NULL return for invalid indices.
 *
 * Parameters:
 *   which - Function index to look up in the parser function table
 *
 * Returns:
 *   Function name string on success, NULL for invalid index
 *
 * Side Effects:
 *   None - pure lookup function without state modification
 *
 * Notes:
 *   - Uses global tmp_parsep pointer for parser function table access
 *   - Validates index against global_int boundary
 *   - Returns realname field from parser function structure
 *   - Used for displaying function names in keybinding interfaces
 *   - Temporary global access pattern for function list transition
 *   - Uses PARM_X macro for K&R style function parameters
 */
char *
bind_func PARM_1(int, which)
{
  if ((which < 0) || (which > global_int))
    return((char *) NULL);
  return(tmp_parsep[which].realname);
}

/*
 * hangup - Signal handler for graceful program termination
 *
 * Signal catching routine that performs graceful cleanup when the program
 * receives termination signals (typically SIGHUP). Ensures proper nation
 * data saving, mail system cleanup, and resource deallocation before
 * program exit. Critical for maintaining game state integrity during
 * unexpected disconnections.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   void (does not return - calls exit())
 *
 * Side Effects:
 *   - Performs movement relocation if in movement mode
 *   - Closes nation data and saves state
 *   - Cleans up mail reading/writing operations and locks
 *   - Exits program with FAIL status
 *   - May update file system state through cleanup operations
 *
 * Notes:
 *   - Registered as signal handler for graceful shutdown
 *   - Handles both mail sending and reading cleanup scenarios
 *   - Ensures file locks are properly released to prevent deadlocks
 *   - Critical for multi-user game integrity during disconnections
 *   - Calls email_close() and rmail_close() for mail system cleanup
 *   - Uses movemode check for movement system state management
 *   - Uses PARM_X macro for K&R style function parameters
 */
void
hangup PARM_1(int, sig)
{
  (void)sig; /* Signal number not used in current implementation */
  /* finish up the nation */
  if (movemode != MOVE_NOMOVE) {
    move_relocate(movemode, FALSE);
  }    
  close_ntn(country, nationname);

  /* remove any existing mail reading/writing file */
  if (mail_ind & MAIL_SENDING) {
    /* clean up lock on target nation */
    email_close();
  }
  if (mail_ind & MAIL_READING) {
    /* save the mail messages and remove the lock file */
    rmail_close();
  }

  /* exit program */
  exit(FAIL);
}
