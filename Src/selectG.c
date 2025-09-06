/*
 * selectG.c - Unit Selection and Navigation System for Conquer Game Interface
 *
 * This module provides comprehensive unit selection and navigation capabilities
 * for the Conquer game interface. It manages the selection cursor system that
 * allows players to choose and navigate between different units (armies, navies,
 * caravans) within map sectors, supporting both single-unit selection and
 * multi-page navigation through crowded sectors.
 *
 * Key Components:
 * - Interactive unit selection with keyboard navigation
 * - Multi-page sector navigation for areas with many units
 * - Unit type identification and retrieval (army, navy, caravan)
 * - Programmatic unit selection by ID for automated navigation
 * - Transport command integration for naval and caravan operations
 * - Extended command dispatch for unit-specific actions
 *
 * Selection Architecture:
 * - Two-level selection system: pager (page number) and selector (position)
 * - SCREEN_SIDE units displayed per page with automatic pagination
 * - Sequential ordering: armies first, then navies, then caravans
 * - Support for both player-owned and deity (all nations) viewing
 * - Cursor position tracking with visual feedback on map display
 *
 * Navigation Features:
 * - Interactive selection mode with full keyboard control
 * - Forward/backward navigation through units in current sector
 * - Direct jump to specific units by ID (goto_army, goto_navy, goto_cvn)
 * - Automatic wrapping at list boundaries
 * - Selection state preservation and restoration
 * - Integration with map display and movement systems
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
#include "moveX.h"
#include "displayG.h"
#include "keyvalsX.h"

/*
 * pick_previous - Navigate to previous unit in selection sequence
 *
 * Moves the selection cursor backward to the previous unit in the current
 * sector. Handles page boundaries by moving to previous page when necessary,
 * and wraps to the end of the unit list when reaching the beginning.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Modifies global selector and pager variables
 *   - May trigger page change if moving past page boundary
 *   - Wraps to last unit when moving past first unit
 *   - Calculates total units in sector for proper wrapping
 *
 * Notes:
 *   - Selector decremented by 2 (accounts for display formatting)
 *   - Automatic pagination when selector goes negative
 *   - Uses units_in_sector() to determine wrap-around point
 *   - Maintains consistent navigation experience across all unit types
 */
int
pick_previous PARM_0(void)
{
  /* back one */
  selector -= 2;
  if (selector < 0) {
    selector = (SCREEN_SIDE - 1) * 2;
    pager--;
  }

  /* move to last unit in the current sector */
  if (pager < 0) {
    selector = units_in_sector(country, XREAL, YREAL);
    if (selector > 0) {
      pager = (selector - 1) / SCREEN_SIDE;
      selector = (selector - (pager * SCREEN_SIDE) - 1) * 2;
    } else {
      selector = 0;
      pager = 0;
    }
  }
  return(0);
}

/*
 * pick_next - Navigate to next unit in selection sequence
 *
 * Moves the selection cursor forward to the next unit in the current
 * sector. Handles page boundaries by moving to next page when necessary,
 * and wraps to the beginning of the unit list when reaching the end.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Modifies global selector and pager variables
 *   - May trigger page change if moving past page boundary
 *   - Wraps to first unit when moving past last unit
 *   - Uses units_in_sector() to validate selection bounds
 *
 * Notes:
 *   - Selector incremented by 2 (accounts for display formatting)
 *   - Automatic pagination when selector exceeds SCREEN_SIDE
 *   - Boundary checking prevents selection beyond available units
 *   - Seamless navigation experience with automatic wrapping
 */
int
pick_next PARM_0(void)
{
  /* one down... one to go... */
  selector += 2;
  if (selector >= SCREEN_SIDE * 2) {
    selector = 0;
    pager += 1;
  }

  /* current selected unit is selector / 2 + SCREEN_SIDE * pager */
  if (selector / 2 + (pager * SCREEN_SIDE) + 1 >
      units_in_sector(country, XREAL, YREAL)) {
    pager = 0;
    selector = 0;
  }
  return(0);
}

/* variables to hold the old selector information */
int scnd_selector = -1;
int scnd_pager = -1;

/*
 * set_select - Interactive unit selection mode with keyboard navigation
 *
 * Provides a full-screen interactive selection interface allowing users to
 * navigate through units in the current sector using keyboard commands.
 * Displays help text, updates the sidebar, and processes navigation input
 * until the user confirms selection or cancels the operation.
 *
 * Parameters:
 *   type - Selection mode flag (TRUE for interactive mode)
 *   desc - Description string shown in selection prompt
 *
 * Returns:
 *   TRUE if selection confirmed, FALSE if cancelled or aborted
 *
 * Side Effects:
 *   - Saves and potentially restores original selector position
 *   - Updates screen display with selection interface and help text
 *   - Processes keyboard input for navigation and selection
 *   - Modifies global selector and pager variables
 *   - Shows cursor position and refreshes screen display
 *
 * Notes:
 *   - Interactive mode only active when type parameter is TRUE
 *   - Supports movement type detection for proper display mode
 *   - Keyboard commands: p/RETURN (next), o/DELETE (previous), q/SPACE (confirm), Q/ESC (cancel)
 *   - Preserves original selection state for cancellation
 *   - Integrates with makeside() for real-time display updates
 */
int
set_select PARM_2(int, type, char *, desc)
{
  /* type is as yet unused, but must be TRUE... to be limitation indicator */
  int keep_going = TRUE, mtype = 0;

  /* calculate the movement type */
  if (display_mode.style[display_mode.focus] == DI_FMOVE) {
    mtype = MOVEIND_FLY;
  } else if (display_mode.style[display_mode.focus] == DI_NMOVE) {
    mtype = MOVEIND_WATER;
  } else {
    mtype = MOVEIND_NORM;
  }

  /* show the usage information */
  scnd_pager = pager;
  scnd_selector = selector;

  /* now keep adjusting the selection */
  if (type) {

    /* continuous loop */
    while (keep_going) {

      /* showing side */
      makeside(FALSE, mtype);
      clear_bottom(0);

      /* now display the prompt */
      mvaddstr(LINES - 3, 0, "  'q' or SPACE to end selection");
      mvaddstr(LINES - 3, COLS / 2,
	       "'p' or RETURN to move downward");
      mvaddstr(LINES - 2, 0, "  'Q' or ESC to abort selection");
      mvaddstr(LINES - 2, COLS / 2,
	       "'o' or DELETE to move backward");
      mvprintw(LINES - 4, 0, "Unit Selection Mode -- %s", desc);

      /* now go back into the proper sector for display */
      show_cursor();
      refresh();

      /* now adjust the selection -- TEMPORARY NON-KEYBINDING METHOD */
      switch (next_char()) {
      case EXT_DEL:
      case '\b':
      case 'o':
	/* move backward */
	pick_previous();
	break;
      case '\n':
      case '\r':
      case 'p':
	/* move forward */
	pick_next();
	break;
      case 'q':
      case ' ':
	/* exit; selection done */
	scnd_selector = (-1);
	scnd_pager = (-1);
	return(TRUE);
      case 'Q':
      case EXT_ESC:
	/* exit and reset to original position */
	keep_going = FALSE;
	break;
      default:
	/* oops */
	errormsg("That key is unavailable in selection mode");
	break;
      }

    }

  }

  /* restore and exit */
  selector = scnd_selector;
  pager = scnd_pager;
  scnd_selector = (-1);
  scnd_pager = (-1);
  return(FALSE);
}

/*
 * ext_cmd - Execute extended commands on currently selected unit
 *
 * Dispatches extended command operations to the appropriate handler based
 * on the type of currently selected unit. Provides a unified interface
 * for accessing unit-specific extended command menus and operations.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   MOVECOST if command executed successfully, 0 if no unit selected
 *
 * Side Effects:
 *   - Calls unit-specific extended command handlers
 *   - May modify unit state through executed commands
 *   - Resets deity mode if active after command completion
 *   - Displays error message if no unit is selected
 *
 * Notes:
 *   - Checks selection in order: army, navy, caravan
 *   - Uses get_*select() functions to determine current selection type
 *   - Delegates to ext_armycmd(), ext_navycmd(), or ext_cvncmd()
 *   - Integrated with game's action cost system (returns MOVECOST)
 */
int
ext_cmd PARM_0(void)
{
  /* check the current selection */
  if (get_armyselect() != NULL) {
    ext_armycmd(-1);
  } else if (get_navyselect() != NULL) {
    ext_navycmd(-1);
  } else if (get_cvnselect() != NULL) {
    ext_cvncmd(-1);
  } else {
    errormsg("There is no unit selected.");
    return(0);
  }
  if (is_god == TRUE) {
    reset_god();
  }
  return(MOVECOST);
}

/*
 * trans_cmd - Execute transport operations on selected naval or caravan units
 *
 * Provides a unified interface for cargo loading and unloading operations
 * on transport-capable units (navies and caravans). Automatically determines
 * the selected unit type and calls the appropriate transport handler.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (transport operations handle their own return values)
 *
 * Side Effects:
 *   - Initiates transport interface for cargo operations
 *   - May modify unit cargo and sector inventory
 *   - Displays error message if non-transport unit selected
 *
 * Notes:
 *   - Only works with navy and caravan units (not armies)
 *   - Delegates to navy_transport() or cvn_transport() as appropriate
 *   - Transport handlers manage their own user interface and validation
 *   - Provides clear error feedback for invalid unit types
 */
int
trans_cmd PARM_0(void)
{
  /* check the type */
  if (get_navyselect() != NULL) {
    navy_transport(NULL);
  } else if (get_cvnselect() != NULL) {
    cvn_transport(NULL);
  } else {
    errormsg("You must select a fleet or caravan to transport items");
  }

  /* all done */
  return(0);
}

/*
 * get_armyselect - Retrieve currently selected army unit
 *
 * Searches through army units in the current sector to find the unit
 * corresponding to the current selection position. Handles both single-nation
 * and deity (all-nations) viewing modes, and maintains proper counting
 * sequence for multi-type unit displays.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to selected army unit, or NULL if selection is not an army
 *
 * Side Effects:
 *   - Sets global_int to nation number where army was found (if found)
 *   - Sets global_int to total army count (if not found)
 *   - Traverses nation army lists and sector unit chains
 *
 * Notes:
 *   - Armies appear first in selection sequence
 *   - Uses pager and selector to calculate target position
 *   - Supports both player nation and deity (all nations) modes
 *   - Follows army->nrby chain for units in same sector
 *   - Critical for maintaining selection state across screen updates
 */
ARMY_PTR 
get_armyselect PARM_0(void)
{
  ARMY_PTR a1_ptr;
  NTN_PTR n1_ptr;
  int c_start, c_end, c_cnt, army_count, hold;

  /* initialize */
  army_count = 0;
  hold = selector / 2 + pager * SCREEN_SIDE;
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* loop over the nations */
  for (c_cnt = c_start; c_cnt < c_end; c_cnt++) {

    /* check in input */
    if ((n1_ptr = world.np[c_cnt]) == NULL) continue;

    /* find armies in sector */
    for (a1_ptr = n1_ptr->army_list;
	 a1_ptr != NULL;
	 a1_ptr = a1_ptr->next) {
      if ((a1_ptr->xloc == XREAL) &&
	  (a1_ptr->yloc == YREAL)) break;
    }

    /*count to proper army in sector*/
    for ( ; a1_ptr != NULL;
	 a1_ptr = a1_ptr->nrby) {

      /* is there a match? */
      if (army_count++ >= hold) {
	/* store the nation where it is found */
	global_int = c_cnt;
	return(a1_ptr);
      }

    }
  }

  /* store the number of armies counted for later use */
  global_int = army_count;
  return((ARMY_PTR) NULL);
}

/*
 * get_navyselect - Retrieve currently selected naval unit
 *
 * Searches through naval units in the current sector to find the unit
 * corresponding to the current selection position. Accounts for preceding
 * army units in the selection sequence and handles both single-nation
 * and deity viewing modes.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to selected naval unit, or NULL if selection is not a navy
 *
 * Side Effects:
 *   - Sets global_int to nation number where navy was found (if found)
 *   - Sets global_int to cumulative unit count including armies (if not found)
 *   - First calls get_armyselect() to account for army units
 *
 * Notes:
 *   - Navies appear second in selection sequence (after armies)
 *   - Must account for all preceding army units in position calculation
 *   - Returns NULL immediately if current selection is an army
 *   - Searches all nations in deity mode, single nation otherwise
 *   - Position calculation includes army count offset from global_int
 */
NAVY_PTR
get_navyselect PARM_0(void)
{
  NAVY_PTR n1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, c_cnt;
  int navy_count = 0, hold;

  /* count preceeding army units */
  if (get_armyselect() != NULL)
    return ((NAVY_PTR)NULL);
  navy_count += global_int;

  /* find all navy units in sector */
  hold = selector / 2 + pager * SCREEN_SIDE;
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* loop over the nations */
  for (c_cnt = c_start; c_cnt < c_end; c_cnt++) {

    /* check in input */
    if ((nt_ptr = world.np[c_cnt]) == NULL) continue;

    /* loop over the fleets */
    for (n1_ptr = nt_ptr->navy_list;
	 (n1_ptr != NULL);
	 n1_ptr = n1_ptr->next) {

      /* find one? */
      if ((n1_ptr->xloc == XREAL) &&
	  (n1_ptr->yloc == YREAL)) {
	if (navy_count++ >= hold) {
	  /* this is it */
	  global_int = c_cnt;
	  return(n1_ptr);
	}
      }

    }
  }

  /* return results */
  global_int = navy_count;
  return ((NAVY_PTR)NULL);
}

/*
 * get_cvnselect - Retrieve currently selected caravan unit
 *
 * Searches through caravan units in the current sector to find the unit
 * corresponding to the current selection position. Accounts for all preceding
 * army and navy units in the selection sequence, representing the final
 * unit type in the selection order.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to selected caravan unit, or NULL if selection is not a caravan
 *
 * Side Effects:
 *   - Sets global_int to nation number where caravan was found (if found)
 *   - Sets global_int to cumulative count of all units (if not found)
 *   - Calls get_armyselect() and get_navyselect() to account for preceding units
 *
 * Notes:
 *   - Caravans appear last in selection sequence (after armies and navies)
 *   - Must account for all preceding army and navy units in position calculation
 *   - Returns NULL immediately if current selection is army or navy
 *   - Position calculation includes cumulative count from previous unit types
 *   - Essential for complete unit selection coverage in mixed sectors
 */
CVN_PTR
get_cvnselect PARM_0(void) 
{
  CVN_PTR c1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, c_cnt;
  int cvn_count = 0, hold;

  /* check if selected unit is not a caravan */
  if ((get_armyselect() != NULL) ||
      (get_navyselect() != NULL))
    return ((CVN_PTR)NULL);
  cvn_count += global_int;

  /* now search for the proper caravan in the sector */
  hold = selector / 2 + pager * SCREEN_SIDE;
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* loop over the nations */
  for (c_cnt = c_start; c_cnt < c_end; c_cnt++) {

    /* check in input */
    if ((nt_ptr = world.np[c_cnt]) == NULL) continue;

    /* loop over the caravans */
    for (c1_ptr = nt_ptr->cvn_list;
	 (c1_ptr != NULL);
	 c1_ptr = c1_ptr->next) {

      /* find one? */
      if ((c1_ptr->xloc == XREAL) &&
	  (c1_ptr->yloc == YREAL)) {
	/* this it? */
	if (cvn_count++ >= hold) {
	  global_int = c_cnt;
	  return(c1_ptr);
	}
      }

    }
  }

  /* return failure */
  global_int = cvn_count;
  return ((CVN_PTR)NULL);
}

/*
 * set_armyselect - Set selection cursor to specific army unit by ID
 *
 * Programmatically moves the selection cursor to point to a specific army
 * unit identified by its ID number. Searches through all army units in the
 * current sector and calculates the appropriate pager and selector values.
 *
 * Parameters:
 *   idnum - Army ID number to select
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies global pager and selector variables if army found
 *   - Searches through nation army lists to locate target unit
 *   - Counts position relative to all units in sector
 *
 * Notes:
 *   - Only affects selection if specified army ID exists in current sector
 *   - Calculates pager (page number) and selector (position) automatically
 *   - Supports both single-nation and deity viewing modes
 *   - Used for programmatic navigation (goto commands, unit following)
 *   - Selection position accounts for army-only sequence (no offset needed)
 */
void
set_armyselect PARM_1 (int, idnum)
{
  ARMY_PTR a1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, c_cnt;
  int hold = 0;

  /* set beginning loop */
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* peruse the nation lists */
  for (c_cnt = c_start; c_cnt < c_end; c_cnt++) {

    /* check in input */
    if ((nt_ptr = world.np[c_cnt]) == NULL) continue;

    /* find the first unit in the current sector */
    for (a1_ptr = nt_ptr->army_list;
	 a1_ptr != NULL;
	 a1_ptr = a1_ptr->next) {
      if ((a1_ptr->xloc == XREAL) &&
	  (a1_ptr->yloc == YREAL)) {
	break;
      }
    }

    /* now find the proper army unit */
    for ( ; (a1_ptr != NULL);
	 a1_ptr = a1_ptr->nrby) {
      /* set it if found */
      if (a1_ptr->armyid == idnum) {
	pager = hold / SCREEN_SIDE;
	selector = (hold % SCREEN_SIDE) * 2;
      }
      hold++;
    }

  }
}

/*
 * set_navyselect - Set selection cursor to specific naval unit by ID
 *
 * Programmatically moves the selection cursor to point to a specific naval
 * unit identified by its ID number. Accounts for preceding army units in
 * the selection sequence and calculates appropriate positioning values.
 *
 * Parameters:
 *   idnum - Navy ID number to select
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies global pager and selector variables if navy found
 *   - Searches through nation navy lists to locate target unit
 *   - Counts armies in sector to calculate proper offset
 *
 * Notes:
 *   - Selection position includes offset for all armies in sector
 *   - Uses armies_in_sector() to calculate position offset
 *   - Only affects selection if specified navy ID exists in current sector
 *   - Maintains proper selection sequence (armies first, then navies)
 *   - Essential for goto_navy() and other navigation functions
 */
void
set_navyselect PARM_1 (int, idnum)
{
  NAVY_PTR n1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, c_cnt;
  int hold = 0;

  /* set beginning loop */
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* peruse the nation lists */
  for (c_cnt = c_start; c_cnt < c_end; c_cnt++) {

    /* check in input */
    if ((nt_ptr = world.np[c_cnt]) == NULL) continue;

    /* scan all of the naval units */
    for (n1_ptr = nt_ptr->navy_list;
	 n1_ptr != NULL;
	 n1_ptr = n1_ptr->next) {

      /* look for units in the sector */
      if ((n1_ptr->xloc == XREAL) &&
	  (n1_ptr->yloc == YREAL)) {
	if (n1_ptr->navyid == idnum) {
	  hold += armies_in_sector(country, XREAL, YREAL);
	  pager = hold / SCREEN_SIDE;
	  selector = (hold % SCREEN_SIDE) * 2;
	}
	hold++;
      }
    }

  }
}

/*
 * set_cvnselect - Set selection cursor to specific caravan unit by ID
 *
 * Programmatically moves the selection cursor to point to a specific caravan
 * unit identified by its ID number. Accounts for all preceding army and navy
 * units in the selection sequence to calculate proper positioning.
 *
 * Parameters:
 *   idnum - Caravan ID number to select
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies global pager and selector variables if caravan found
 *   - Searches through nation caravan lists to locate target unit
 *   - Counts armies and navies in sector to calculate proper offset
 *
 * Notes:
 *   - Selection position includes offset for all armies and navies in sector
 *   - Uses armies_in_sector() and navies_in_sector() for position calculation
 *   - Only affects selection if specified caravan ID exists in current sector
 *   - Maintains proper selection sequence (armies, navies, then caravans)
 *   - Critical for goto_cvn() and complete unit navigation support
 */
void
set_cvnselect PARM_1 (int, idnum)
{
  CVN_PTR c1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, c_cnt;
  int hold = 0;

  /* set beginning loop */
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* peruse the nation lists */
  for (c_cnt = c_start; c_cnt < c_end; c_cnt++) {

    /* check in input */
    if ((nt_ptr = world.np[c_cnt]) == NULL) continue;

    /* scan all of the units */
    for (c1_ptr = nt_ptr->cvn_list;
	 (c1_ptr != NULL);
	 c1_ptr = c1_ptr->next) {

      /* sector sitter */
      if ((c1_ptr->xloc == XREAL) &&
	  (c1_ptr->yloc == YREAL)) {
	/* found it */
	if (c1_ptr->cvnid == idnum) {
	  hold += armies_in_sector(country, XREAL, YREAL);
	  hold += navies_in_sector(country, XREAL, YREAL);
	  pager = hold / SCREEN_SIDE;
	  selector = (hold % SCREEN_SIDE) * 2;
	}
	hold++;
      }

    }
  }
}

/*
 * goto_city - Navigate map view to specified city location
 *
 * Moves the map cursor to the location of a specified city, updating
 * the display coordinates and resetting unit selection to the first
 * position. Provides quick navigation to city locations.
 *
 * Parameters:
 *   c1_ptr - Pointer to city structure to navigate to
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Updates global xcurs and ycurs relative to map offsets
 *   - Resets selector and pager to first unit position
 *   - Changes current map view location (XREAL, YREAL)
 *
 * Notes:
 *   - Returns immediately if city pointer is NULL
 *   - Cursor position calculated relative to current map offsets
 *   - Selection automatically reset to first unit in new sector
 *   - Used by city-related navigation and information commands
 */
void
goto_city PARM_1 (CITY_PTR, c1_ptr)
{
  /* if god, just bop out */
  if (c1_ptr == NULL) return;

  /* set XREAL, YREAL and selector */
  xcurs = ((int) c1_ptr->xloc) - xoffset;
  ycurs = ((int) c1_ptr->yloc) - yoffset;
  selector = 0;
  pager = 0;
}

/*
 * goto_army - Navigate to specific army unit by ID
 *
 * Moves the map view to the location of a specified army unit and sets
 * the selection cursor to that unit. Supports both specific ID navigation
 * and sequential navigation through army units. Provides comprehensive
 * army navigation with error handling.
 *
 * Parameters:
 *   idnum - Army ID to navigate to, or -1 for next army in sequence
 *
 * Returns:
 *   FALSE on successful navigation, TRUE on error or if deity mode
 *
 * Side Effects:
 *   - Updates map cursor position if army location differs from current
 *   - Sets selection cursor to target army unit
 *   - May display error messages for invalid or missing armies
 *
 * Notes:
 *   - Returns TRUE immediately if in deity mode (country == UNOWNED)
 *   - ID value -1 triggers navigation to next army in nation's list
 *   - Handles wrap-around to first army if at end of list
 *   - Updates both map position and selection state for complete navigation
 *   - Provides user feedback for non-existent army units
 */
int
goto_army PARM_1 (int, idnum)
{
  ARMY_PTR a1_ptr;

  /* if only god, just bop out */
  if (country == UNOWNED) return(TRUE);

  /* select the army */
  if (idnum == (-1)) {
    a1_ptr = get_armyselect();
    if (a1_ptr == NULL || (a1_ptr = a1_ptr->next) == NULL)
      a1_ptr = ntn_ptr->army_list;
    if (a1_ptr == NULL) {
      errormsg("You have no army units to go to... oh well.");
      return(TRUE);
    }
    idnum = a1_ptr->armyid;
  } else {
    a1_ptr = armybynum(idnum);
  }

  if (a1_ptr != NULL) {
    /* set XREAL, YREAL and selector */
    if (XREAL != a1_ptr->xloc) {
      xcurs = ((int) a1_ptr->xloc) - xoffset;
    }
    if (YREAL != a1_ptr->yloc) {
      ycurs = ((int) a1_ptr->yloc) - yoffset;
    }
    set_armyselect(idnum);
  } else {
    /* unknown army */
    errormsg("Hmm... that unit only exists in your imagination");
    return(TRUE);
  }
  return(FALSE);
}

/*
 * goto_navy - Navigate to specific naval unit by ID
 *
 * Moves the map view to the location of a specified naval unit and sets
 * the selection cursor to that unit. Supports both specific ID navigation
 * and sequential navigation through naval units. Provides comprehensive
 * naval navigation with error handling.
 *
 * Parameters:
 *   idnum - Navy ID to navigate to, or -1 for next navy in sequence
 *
 * Returns:
 *   FALSE on successful navigation, TRUE on error or if deity mode
 *
 * Side Effects:
 *   - Updates map cursor position if navy location differs from current
 *   - Sets selection cursor to target naval unit
 *   - May display error messages for invalid or missing navies
 *
 * Notes:
 *   - Returns TRUE immediately if in deity mode (country == UNOWNED)
 *   - ID value -1 triggers navigation to next navy in nation's list
 *   - Handles wrap-around to first navy if at end of list
 *   - Updates both map position and selection state for complete navigation
 *   - Provides user feedback for non-existent naval units
 */
int
goto_navy PARM_1 (int, idnum)
{
  NAVY_PTR n1_ptr;

  /* if god, just bop out */
  if (country == UNOWNED) return(TRUE);

  /* select the navy */
  if (idnum == (-1)) {
    n1_ptr = get_navyselect();
    if ((n1_ptr == NULL) || ((n1_ptr = n1_ptr->next) == NULL))
      n1_ptr = ntn_ptr->navy_list;
    if (n1_ptr == NULL) {
      errormsg("You have no navies to go to");
      return(TRUE);
    }
    idnum = n1_ptr->navyid;
  } else {
    n1_ptr = navybynum(idnum);
  }

  if (n1_ptr != NULL) {
    /* set XREAL, YREAL and selector */
    if (XREAL != n1_ptr->xloc) {
      xcurs = ((int) n1_ptr->xloc) - xoffset;
    }
    if (YREAL != n1_ptr->yloc) {
      ycurs = ((int) n1_ptr->yloc) - yoffset;
    }
    set_navyselect(idnum);
  } else {
    /* unknown navy */
    errormsg("Hmm... that fleet only exists in your imagination");
    return(TRUE);
  }
  return(FALSE);
}

/*
 * goto_cvn - Navigate to specific caravan unit by ID
 *
 * Moves the map view to the location of a specified caravan unit and sets
 * the selection cursor to that unit. Supports both specific ID navigation
 * and sequential navigation through caravan units. Provides comprehensive
 * caravan navigation with error handling.
 *
 * Parameters:
 *   idnum - Caravan ID to navigate to, or -1 for next caravan in sequence
 *
 * Returns:
 *   FALSE on successful navigation, TRUE on error or if deity mode
 *
 * Side Effects:
 *   - Updates map cursor position if caravan location differs from current
 *   - Sets selection cursor to target caravan unit
 *   - May display error messages for invalid or missing caravans
 *   - Contains coordinate bug: checks YREAL against xloc (should be XREAL)
 *
 * Notes:
 *   - Returns TRUE immediately if in deity mode (country == UNOWNED)
 *   - ID value -1 triggers navigation to next caravan in nation's list
 *   - Handles wrap-around to first caravan if at end of list
 *   - BUG: Line 611 incorrectly compares YREAL with c1_ptr->xloc
 *   - Updates both map position and selection state for complete navigation
 */
int
goto_cvn PARM_1 (int, idnum)
{
  CVN_PTR c1_ptr;

  /* if god, just bop out */
  if (country == UNOWNED) return(TRUE);

  /* select the caravan */
  if (idnum == (-1)) {
    c1_ptr = get_cvnselect();
    if (c1_ptr == NULL || (c1_ptr = c1_ptr->next) == NULL)
      c1_ptr = ntn_ptr->cvn_list;
    if (c1_ptr == NULL) {
      errormsg("You have no caravans to go to");
      return(TRUE);
    }
    idnum = c1_ptr->cvnid;
  } else {
    c1_ptr = cvnbynum(idnum);
  }

  if (c1_ptr != NULL) {
    /* set XREAL, YREAL and selector */
    if (YREAL != c1_ptr->xloc) {
      xcurs = ((int) c1_ptr->xloc) - xoffset;
    }
    if (YREAL != c1_ptr->yloc) {
      ycurs = ((int) c1_ptr->yloc) - yoffset;
    }
    set_cvnselect(idnum);
  } else {
    /* unknown caravan */
    errormsg("Hmm... that caravan only exists in your imagination");
    return(TRUE);
  }
  return(FALSE);
}
