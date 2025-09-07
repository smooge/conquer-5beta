/*
 * customG.c - Configuration File Management and Serialization
 *
 * This module provides comprehensive configuration file generation and
 * management capabilities for the Conquer game system. It handles the
 * serialization of user preferences, key bindings, display modes, and
 * game settings to persistent configuration files that can be loaded
 * on subsequent program runs.
 *
 * Key Functionality:
 * - Configuration file generation with version headers and timestamps
 * - Key binding serialization for all game subsystems
 * - Display mode configuration management
 * - Boolean and numeric option serialization
 * - Symbol mapping for terrain, vegetation, and designations
 * - Platform-specific security and permission handling
 *
 * The module uses a differential approach, writing only settings that
 * differ from defaults to keep configuration files minimal and readable.
 * It supports complex nested configurations like display modes with
 * focus, style, highlighting, and target specifications.
 *
 * Security Considerations:
 * - Proper file permission management with umask control
 * - User ID switching on SWITCHID-enabled systems
 * - Secure file creation with restricted permissions
 *
 * Configuration File Structure:
 * - Header with version and creation information
 * - Boolean options (with ! prefix for false values)
 * - Numeric parameters (supply levels, pager settings, zoom)
 * - Symbol mappings (contour, vegetation, designation)
 * - Key binding modifications
 * - Display mode configurations
 *
 * Integration Points:
 * - Works with all key binding subsystems (global, move, email, etc.)
 * - Interfaces with display system for mode management
 * - Uses symbol definition arrays for terrain rendering
 * - Coordinates with options system for preference storage
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
#include "dataG.h"
#include "armyX.h"
#include "desigX.h"
#include "worldX.h"
#include "elevegX.h"
#include "tgoodsX.h"
#include "displayG.h"
#include "optionsX.h"
#include "patchlevel.h"

/*
 * send_keys - Write modified key bindings to configuration file
 *
 * Compares current key bindings with default bindings and outputs only
 * the differences to the configuration file. This includes new bindings,
 * rebound functions, and deleted bindings. The function tracks which
 * default bindings are found to identify deletions.
 *
 * Parameters:
 *   fp - Open file pointer to write configuration data
 *   key_info - Key system structure containing default bindings and metadata
 *   klist_p - Linked list of current key bindings to compare against defaults
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Writes key binding configuration commands to file
 *   - Uses global opt_list array for command names
 *   - Calls form_str() and find_func() utility functions
 *
 * Notes:
 *   - Uses temporary array fnd_kb[] to track which default bindings are found
 *   - Outputs OPT_BINDKEY for new bindings, OPT_REBIND for changed functions
 *   - Outputs OPT_UNBIND for deleted default bindings
 *   - Silently skips bindings if function lookup fails
 */
static void
send_keys PARM_3(FILE *, fp, KEYSYS_STRUCT, key_info, KLIST_PTR, klist_p)
{
  int count, max_kb;
  PARSE_PTR parse_ptr;
  char fnd_kb[BIGLTH];

  /* no bindings changed, if no bindings around */
  if (klist_p == NULL) return;

  /* first, mark all bindings as not found */
  max_kb = key_info.num_binds;
  for (count = 0; count < max_kb; count++) {
    fnd_kb[count] = FALSE;
  }

  /* go through the entire list */
  for (; klist_p != NULL; klist_p = klist_p->next) {

    /* go through and match the key bindings */
    for (count = 0; count < max_kb; count++) {
      /* check it out */
      if (strcmp(key_info.kbind_p[count].chlist,
		 klist_p->key_data.chlist) == 0) break;
    }

    /* record that it is found */
    fnd_kb[count] = TRUE;

    /* check out the results */
    if (count == max_kb) {

      /* new key binding */
      form_str(string, klist_p->key_data.chlist, FALSE);
      parse_ptr = find_func(klist_p->key_data.func,
			    key_info.parse_p, key_info.num_parse);
      if (parse_ptr != NULL) {
	fprintf(fp, "%s %s %s %s\n", opt_list[OPT_BINDKEY],
		key_info.name, string, parse_ptr->realname);
      }

    } else if (key_info.kbind_p[count].func != klist_p->key_data.func) {

      /* rebound function */
      form_str(string, klist_p->key_data.chlist, FALSE);
      parse_ptr = find_func(klist_p->key_data.func,
			    key_info.parse_p, key_info.num_parse);
      if (parse_ptr != NULL) {
	fprintf(fp, "%s %s %s %s\n", opt_list[OPT_REBIND],
		key_info.name, string, parse_ptr->realname);
      }

    }

  }

  /* report all deleted bindings */
  for (count = 0; count < max_kb; count++) {

    /* is it gone? */
    if (fnd_kb[count] == FALSE) {
      /* deleted keybinding */
      form_str(string, key_info.kbind_p[count].chlist, FALSE);
      fprintf(fp, "%s %s %s\n", opt_list[OPT_UNBIND],
	      key_info.name, string);
    }

  }
}

/*
 * target_str - Convert target values to configuration-friendly strings
 *
 * Converts numeric target values into string representations for writing
 * to configuration files. Handles different highlight target types including
 * designations, ownership, and trade goods. Returns static string buffer
 * that gets overwritten on each call.
 *
 * Parameters:
 *   hstyle - Highlight style type (HI_MINDESG, HI_MAJDESG, HI_OWN, HI_TGOODS)
 *   value - Numeric value to convert to string representation
 *
 * Returns:
 *   Pointer to static string buffer containing converted value
 *   Empty string for unknown highlight styles
 *
 * Side Effects:
 *   - Overwrites static output_str buffer on each call
 *   - Bounds-checks values and defaults to safe values if out of range
 *
 * Notes:
 *   - Uses strcpy() to populate static buffer (legacy string handling)
 *   - Returns "*" for unowned sectors and no trade goods
 *   - Accesses global arrays: min_dinfo, maj_dinfo, world.np, tgclass_info
 *   - Thread-unsafe due to static buffer reuse
 */
static char *
target_str PARM_2(int, hstyle, int, value)
{
  static char output_str[LINELTH];

  switch (hstyle) {
  case HI_MINDESG:
    /* give back proper range */
    if ((value < 0) ||
	(value >= MIN_NUMBER)) {
      value = 0;
    }
    strcpy(output_str, min_dinfo[value].name);
    break;
  case HI_MAJDESG:
    /* give back proper range */
    if ((value < 0) ||
	(value >= MAJ_NUMBER)) {
      value = 0;
    }
    strcpy(output_str, maj_dinfo[value].name);
    break;
  case HI_OWN:
    /* find proper nation */
    if ((value < 0) ||
	(value >= MAXNTN)) {
      value = UNOWNED;
    }
    if (value == UNOWNED) {
      strcpy(output_str, "*");
    } else {
      strcpy(output_str, (world.np[value])->name);
    }
    break;
  case HI_TGOODS:
    /* find proper tradegood */
    if ((value < 0) ||
	(value >= tgclass_number)) {
      value = TG_NONE;
    }
    if (value == TG_NONE) {
      strcpy(output_str, "*");
    } else {
      strcpy(output_str, tgclass_info[value].name);
    }
    break;
  default:
    /* no target possible */
    output_str[0] = '\0';
    break;
  }

  /* no match */
  return(&(output_str[0]));
}

/*
 * send_display - Write display mode configurations to file
 *
 * Serializes all display mode settings by comparing current modes against
 * default base modes and writing only the differences. Handles both modified
 * existing modes and completely new modes. Also outputs deleted modes and
 * the current default display settings.
 *
 * Parameters:
 *   fp - Open file pointer to write display configuration data
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Writes display mode configuration commands to file
 *   - Uses temporary array fnd_mode[] to track processed base modes
 *   - Accesses global arrays: base_modes, hex_list, display_list, highl_list
 *
 * Notes:
 *   - Compares against DMODE_NUMBER base modes for differences
 *   - Outputs complete configuration for new modes
 *   - Only outputs changed settings for existing modes
 *   - Calls target_str() for highlight target values
 *   - Uses hl_targets() to determine if target output is needed
 *   - Writes section headers with # comments for organization
 */
static void
send_display PARM_1(FILE *, fp)
{
  DMODE_PTR travel_dmode;
  char fnd_mode[DMODE_NUMBER];
  int count, count2;

  /* indicate that displays are being worked on */
  fprintf(fp, "#\n# display mode adjustments... if any\n");

  /* mark all modes as not found */
  for (count = 0; count < DMODE_NUMBER; count++) {
    fnd_mode[count] = FALSE;
  }

  /* compare all of the current display settings with the default */
  for (travel_dmode = dmode_list;
       travel_dmode != NULL;
       travel_dmode = travel_dmode->next) {

    /* now search among the original modes for it */
    for (count = 0; count < DMODE_NUMBER; count++) {
      if (strcmp(base_modes[count].name, travel_dmode->d.name) == 0) break;
    }

    /* new mode altogether */
    if (count == DMODE_NUMBER) {

      /* write out everything */
      fprintf(fp, "display-mode \"%s\" focus %s\n",
	      travel_dmode->d.name,
	      hex_list[travel_dmode->d.focus]);
      for (count2 = 0; count2 < HXPOS_NUMBER; count2++) {
	if (travel_dmode->d.style[count2] != DI_KEEP) {
	  fprintf(fp, "display-mode \"%s\" %s %s\n",
		  travel_dmode->d.name, hex_list[count2],
		  display_list[travel_dmode->d.style[count2]]);
	}
	if (travel_dmode->d.highlight[count2] != HI_KEEP) {
	  fprintf(fp, "display-mode \"%s\" highlight %s %s\n",
		  travel_dmode->d.name,
		  hex_list[count2],
		  highl_list[travel_dmode->d.highlight[count2]]);
	  if (hl_targets(travel_dmode->d.highlight[count2])) {
	    fprintf(fp, "display-mode \"%s\" target %s %s\n",
		    travel_dmode->d.name,
		    hex_list[count2],
		    target_str(travel_dmode->d.highlight[count2],
			       travel_dmode->d.target[count2]));
	  }
	}
      }
      
    } else {
      /* got a match; check each component */
      fnd_mode[count] = TRUE;

      /* check focus */
      if (travel_dmode->d.focus != base_modes[count].focus) {
	fprintf(fp, "display-mode \"%s\" focus %s\n",
		travel_dmode->d.name,
		hex_list[travel_dmode->d.focus]);
      }

      /* check all of the styles */
      for (count2 = 0; count2 < HXPOS_NUMBER; count2++) {
	if (travel_dmode->d.style[count2] !=
	    base_modes[count].style[count2]) {
	  fprintf(fp, "display-mode \"%s\" %s %s\n",
		  travel_dmode->d.name, hex_list[count2],
		  display_list[travel_dmode->d.style[count2]]);
	}

	/* check highlighting */
	if (travel_dmode->d.highlight[count2] !=
	    base_modes[count].highlight[count2]) {
	  fprintf(fp, "display-mode \"%s\" highlight %s %s\n",
		  travel_dmode->d.name,
		  hex_list[count2],
		  highl_list[travel_dmode->d.highlight[count2]]);
	  if (hl_targets(travel_dmode->d.highlight[count2])) {
	    fprintf(fp, "display-mode \"%s\" target %s %s\n",
		    travel_dmode->d.name,
		    hex_list[count2],
		    target_str(travel_dmode->d.highlight[count2],
			       travel_dmode->d.target[count2]));
	  }
	}
      }
    }

  }

  /* indicate any display modes that have been eliminated */
  for (count = 0; count < DMODE_NUMBER; count++) {
    if (fnd_mode[count] == FALSE) {
      /* the mode has been removed/renamed */
      fprintf(fp, "display-mode \"%s\" delete\n",
	      base_modes[count].name);
    }
  }

  /* now send out the default settings */
  fprintf(fp, "#\n# default display mode\n");
  fprintf(fp, "default-display focus %s\n",
	  hex_list[display_mode.focus]);
  for (count2 = 0; count2 < HXPOS_NUMBER; count2++) {
    fprintf(fp, "default-display %s %s\n", hex_list[count2],
	    display_list[display_mode.style[count2]]);
  }
  for (count2 = 0; count2 < HXPOS_NUMBER; count2++) {
    fprintf(fp, "default-display highlight %s %s\n",
	    hex_list[count2],
	    highl_list[display_mode.highlight[count2]]);
    if (hl_targets(display_mode.highlight[count2])) {
      fprintf(fp, "default-display target %s %s\n",
	      hex_list[count2],
	      target_str(display_mode.highlight[count2],
			 display_mode.target[count2]));
    }
  }
}

/*
 * allkeys_init - Initialize all key binding systems
 *
 * Calls alignment functions for all key binding subsystems to ensure
 * that dynamic key bindings are properly synchronized with their
 * default configurations. This is typically called during program
 * startup or when resetting configurations.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Calls align functions for all key binding subsystems
 *   - May modify global key binding lists
 *
 * Notes:
 *   - Initializes: global, move, xfer, email, reader, magic, ninfo systems
 *   - Each align function synchronizes that subsystem's key bindings
 *   - Must be called before key binding systems are used
 *   - Part of the configuration management initialization sequence
 */
void
allkeys_init PARM_0(void)
{
  align_global_keys();
  align_move_keys();
  align_xfer_keys();
  align_email_keys();
  align_reader_keys();
  align_magic_keys();
  align_ninfo_keys();
}

/*
 * allkeys_check - Validate all key binding systems
 *
 * Performs validation checks on all key binding subsystems to detect
 * conflicts, invalid bindings, or other problems. Returns the total
 * number of errors found across all subsystems.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Total number of key binding errors found across all subsystems
 *   0 if all key bindings are valid
 *
 * Side Effects:
 *   - None (read-only validation)
 *
 * Notes:
 *   - Checks: global, move, email, reader, xfer, magic, ninfo systems
 *   - Each check_keys() call returns error count for that subsystem
 *   - Should be called after configuration changes to verify validity
 *   - Part of the configuration validation workflow
 */
int
allkeys_check PARM_0(void)
{
  int hold;

  hold = check_keys(&cq_bindings, global_keysys);
  hold += check_keys(&mparse_bindings, mparse_keysys);
  hold += check_keys(&email_bindings, email_keysys);
  hold += check_keys(&reader_bindings, reader_keysys);
  hold += check_keys(&xfer_bindings, xfer_keysys);
  hold += check_keys(&mg_bindings, magic_keysys);
  hold += check_keys(&ni_bindings, ninfo_keysys);

  /* give back results */
  return(hold);
}

/*
 * write_keybinds - Write all key binding configurations to file
 *
 * Outputs configuration data for all key binding subsystems by calling
 * send_keys() for each system. Writes a section header and then serializes
 * all modified key bindings for the seven main subsystems.
 *
 * Parameters:
 *   fp - Open file pointer to write key binding configuration data
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Writes key binding section header and data to file
 *   - Calls send_keys() for each subsystem
 *
 * Notes:
 *   - Processes: global, email, reader, xfer, magic, mparse, ninfo systems
 *   - Each send_keys() call handles one subsystem's bindings
 *   - Part of the complete configuration file generation process
 *   - Outputs only changed bindings, not complete binding sets
 */
static void
write_keybinds PARM_1(FILE *, fp)
{
  /* call each of the key binding functions */
  fprintf(fp, "#\n# key binding information (if any)\n");
  send_keys(fp, global_keysys, cq_bindings);
  send_keys(fp, email_keysys, email_bindings);
  send_keys(fp, reader_keysys, reader_bindings);
  send_keys(fp, xfer_keysys, xfer_bindings);
  send_keys(fp, magic_keysys, mg_bindings);
  send_keys(fp, mparse_keysys, mparse_bindings);
  send_keys(fp, ninfo_keysys, ni_bindings);
}

/*
 * write_custom - Generate complete configuration file with current settings
 *
 * Creates a comprehensive configuration file containing all current game
 * settings, including boolean options, numeric parameters, symbol mappings,
 * key bindings, and display modes. Handles platform-specific security
 * considerations for file creation and permission management.
 *
 * Parameters:
 *   fname - Filename/path where configuration file should be written
 *
 * Returns:
 *   0 on successful file creation and writing
 *   -1 on file creation failure
 *
 * Side Effects:
 *   - Creates/overwrites configuration file at specified path
 *   - Temporarily changes umask for file permissions (non-VMS)
 *   - May switch user IDs for security on SWITCHID systems
 *   - Writes comprehensive configuration data
 *
 * Notes:
 *   - Includes version header with creation timestamp
 *   - Writes boolean options with ! prefix for false values
 *   - Outputs contour, vegetation, and designation symbol mappings
 *   - Calls write_keybinds() and send_display() for complex configurations
 *   - Platform-specific code for VMS, SWITCHID, and SYSV4 systems
 *   - Uses CUSTOM_UMASK for restrictive file permissions
 *   - Restores original umask and user ID on completion
 */
int
write_custom PARM_1(char *, fname)
{
  FILE *cust_fp;
  int count;
#ifndef VMS
#ifdef SWITCHID
#ifdef SYSV4
  uid_t uid, euid;
#else
  int uid, euid;
#endif /* SYSV4 */

  uid = getuid();
  euid = geteuid();

  /* does the effective uid == real uid */
  if (euid != uid) {
#ifdef SVSV4
    setuid(uid);
#else
    setreuid(euid, uid);
#endif /* SYSV4 */
  }
#endif /* SWITCHID */

  /* set the umask */
  (void ) umask(CUSTOM_UMASK);

#endif /* VMS */  

  /* open the file */
  if ((cust_fp = fopen(fname, "w")) == NULL) {
#ifndef VMS
    /* reset the umask */
    (void ) umask(DEFAULT_UMASK);
#ifdef SWITCHID
    if (euid != uid) {
#ifdef SYSV4
      setuid(euid);
#else
      setreuid(uid, euid);
#endif /* SYSV4 */
    }
#endif /* SWITCHID */
#endif /* VMS */  
    return(-1);
  }

  /* give the initial header */
  fprintf(cust_fp, "#\n");
  fprintf(cust_fp, "# Conquer Configuration File\n");
  fprintf(cust_fp, "#\n");
  fprintf(cust_fp, "# Automatically Generated by Conquer %s.%d\n",
	  VERSION, PATCHLEVEL);
  fprintf(cust_fp, "# Created: %s\n", mach_time());
  fprintf(cust_fp, "#\n");

  /* is this option set? */
  if (conq_allblanks) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_ALLBLANKS]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_ALLBLANKS]);
  }

  /* well, is this option set? */
  if (conq_bottomlines) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_BOTTOMLINES]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_BOTTOMLINES]);
  }

  /* how about this one? */
  if (conq_expert) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_EXPERT]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_EXPERT]);
  }

  /* or this one? */
  if (conq_gaudy) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_GAUDY]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_GAUDY]);
  }

  /* mail headers mode? */
  if (conq_mheaders) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_HEADERS]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_HEADERS]);
  }

  /* how about he information mode */
  if (conq_infomode) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_INFOMODE]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_INFOMODE]);
  }

  /* system mail check? */
  if (dosysm_check) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_MAILCHECK]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_MAILCHECK]);
  }

  /* how about the terminal bell? */
  if (conq_beeper) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_TBELL]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_TBELL]);
  }

  /* and then let's check this one? */
  if (conq_waterbottoms) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_WATER_BTM]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_WATER_BTM]);
  }

  /* store the pager information */
  fprintf(cust_fp, "%s: %d\n", opt_list[OPT_SUPPLY],
	  (conq_supply_level == MAXSUPPLIES)? 0 : conq_supply_level);
  fprintf(cust_fp, "%s: %d\n", opt_list[OPT_PAGETAB], pager_tab);
  fprintf(cust_fp, "%s: %d\n", opt_list[OPT_PAGESCROLL], pager_scroll);
  fprintf(cust_fp, "%s: %d\n", opt_list[OPT_PAGEOFFSET],
	  (pager_offset == LINES - 1)? 0 : pager_scroll);

  /* show the zoom level */
  fprintf(cust_fp, "zoom-level: %s\n", zooms[zoom_level]);

  /* store all of the contour symbols */
  fprintf(cust_fp, "#\n# countour symbols\n");
  for (count = 0; count < ELE_NUMBER; count++) {
    fprintf(cust_fp, "contour %s=%c\n",
	    ele_info[count].name,
	    ele_info[count].symbol);
  }

  /* store all of the vegetation symbols */
  fprintf(cust_fp, "#\n# vegetation symbols\n");
  for (count = 0; count < VEG_NUMBER; count++) {
    fprintf(cust_fp, "vegetation %s=%c\n",
	    veg_info[count].name,
	    veg_info[count].symbol);
  }

  /* store all of the designation symbols */
  fprintf(cust_fp, "#\n# designation symbols\n");
  for (count = 0; count < MAJ_NUMBER; count++) {
    fprintf(cust_fp, "designation %s=%c\n",
	    maj_dinfo[count].name,
	    maj_dinfo[count].symbol);
  }

  /* now handle the display modes and key mappings */
  write_keybinds(cust_fp);
  send_display(cust_fp);

  /* end of file */
  fprintf(cust_fp, "#\n# end of prefab file\n#\n");
  fclose(cust_fp);
#ifndef VMS
  /* reset the umask */
  (void ) umask(DEFAULT_UMASK);
#ifdef SWITCHID
  if (euid != uid) {
#ifdef SYSV4
    setuid(euid);
#else
    setreuid(uid, euid);
#endif /* SYSV4 */
  }
#endif /* SWITCHID */
#endif /* VMS */  
  return(0);
}
