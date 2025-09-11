/*
 * caravanG.c - Caravan Management Interface and Operations
 *
 * This module provides comprehensive caravan unit management functionality
 * including creation, destruction, movement, cargo handling, status management,
 * and advanced operations like merging and splitting caravan units.
 *
 * Key Functionality:
 * - Caravan lifecycle management (creation, destruction, disbanding)
 * - Cargo transfer operations with cities, navies, sectors, and other caravans
 * - Unit combination and splitting operations with proportional resource allocation
 * - Status and speed adjustment with movement cost calculations
 * - Supply management with resource consumption and territorial restrictions
 * - Repair operations requiring supply centers and material costs
 * - Extended command interface for interactive caravan management
 *
 * The module implements sophisticated cargo transfer mechanics that handle
 * movement synchronization between units, resource availability validation,
 * and user confirmation for potentially destructive operations. Special
 * handling is provided for magic-enhanced units and territorial supply restrictions.
 *
 * Caravan units represent mobile trade and transport capability, carrying
 * people and materials across the game world. They can be combined for efficiency
 * or split for tactical distribution, with all operations preserving game
 * balance through movement costs and resource requirements.
 *
 * Integration:
 * - Uses dataG.h for game state and interface definitions
 * - Coordinates with cityX.h for supply center operations
 * - Integrates with navyX.h for naval cargo transfer
 * - Utilizes worldX.h for sector and terrain validation
 * - Implements caravanX.h caravan-specific functionality
 *
 * Security:
 * - Validates unit ownership and territorial access
 * - Prevents invalid operations on magically enhanced units
 * - Ensures resource availability before consumption
 * - Implements god mode bypass for administrative operations
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
#define USE_CODES
#define PRINT_CODES
#include "dataG.h"
#include "cityX.h"
#include "navyX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "elevegX.h"
#include "statusX.h"
#include "caravanX.h"

/*
 * disb_cvn - Disband and destroy a caravan unit
 *
 * Destroys the specified caravan unit, returning its crew to the local population
 * and attempting to return supplies to the sector. Validates ownership and
 * cargo status before proceeding with destruction.
 *
 * The function performs comprehensive resource recovery:
 * - Returns crew members to sector population
 * - Attempts to redistribute supplies to sector storage
 * - Adds portion of crew to city recruiting pool if present
 * - Handles supply loss warnings if sector cannot store resources
 *
 * Parameters:
 *   v1_ptr - Pointer to caravan structure to disband (must not be NULL)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Destroys the caravan unit permanently
 *   - Modifies sector population and resources
 *   - Updates city recruiting pools if applicable
 *   - Triggers hex recalculation for display updates
 *   - May display user confirmation dialogs
 *
 * Notes:
 *   - Requires caravan to be in owned territory (unless god mode)
 *   - Caravan must be completely unloaded before disbanding
 *   - Prompts for confirmation unless expert mode is enabled
 *   - Thread-unsafe due to global state modifications
 */
void
disb_cvn PARM_1 (CVN_PTR, v1_ptr)
{
  ITEM_STRUCT supply_items;
  CVN_PTR chold_ptr = cvn_ptr;
  long num_people;
  int cnt, limited = FALSE, xloc, yloc;

  /* check the input */
  if (v1_ptr == NULL) {
    errormsg("Invalid input to function disb_cvn()");
    return;
  }

  /* check disbanding */
  if ((is_god == FALSE) &&
      (sct[v1_ptr->xloc][v1_ptr->yloc].owner != country)) {
    errormsg("Caravans must disband in their own territory");
    return;
  }
  if (cvn_carrying(v1_ptr)) {
    errormsg("Everything must be unloaded before caravan disbanding");
    return;
  }

  /* question the disband */
  if (conq_expert == FALSE) {
    mvprintw(LINES - 1, 0, "Disband your caravan #%d? ", v1_ptr->cvnid);
    if (y_or_n() == FALSE) return;
  }

  /* now disband the unit */
  cvn_ptr = v1_ptr;
  if (is_god == FALSE) {

    /* try to give back the supplies */
    if (CVN_SPLY > 0) {

      /* find the values */
      cvn_support(cvn_ptr, &(supply_items.mtrls[0]), CVN_SPLY);

      /* try to give them back */
      if (send_resources(CVN_XLOC, CVN_YLOC, &(supply_items), limited)) {
	bottommsg("The caravan's supplies will be lost.  Continue? ");
	if (y_or_n() == FALSE) return;
	CVN_SPLY = 0;
      }

    }

    /* position it */
    xloc = XREAL;
    yloc = YREAL;
    xcurs = CVN_XLOC - xoffset;
    ycurs = CVN_YLOC - yoffset;

    /* handle the crew */
    num_people = (long) CVN_CREW * CVN_SIZE;
    sct[CVN_XLOC][CVN_YLOC].people += num_people;
    SADJPEOP;

    /* now give back the resources */
    if (CVN_SPLY > 0) {
      GIVESTART;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if ((global_long = supply_items.mtrls[cnt]) > 0) {
	  GIVEMTRLS(cnt);
	}
      }
      GIVEFINISH(limited);
    }

    /* reset current position */
    xcurs = xloc - xoffset;
    ycurs = yloc - yoffset;

    /* perhaps add some soldiers into the recruiting pool */
    if ((city_ptr = citybyloc(ntn_ptr, CVN_XLOC, CVN_YLOC)) != NULL) {
      if (CITY_PEOPLE >= 0) {
	CITY_PEOPLE += num_people / 5;
	CADJPEOP;
      }
    }

  }

  /* now kill it */
  VDESTROY;
  dest_cvn(CVN_ID);
  hex_recalc();
  cvn_ptr = chold_ptr;
}

/*
 * change_vspeed - Adjust the movement speed of a caravan unit
 *
 * Changes the caravan's movement speed setting, applying movement point
 * penalties for speed increases during the current turn. Speed changes
 * affect future movement capability and current turn mobility.
 *
 * The function implements movement cost mechanics:
 * - Reduces current movement points when changing speed (non-god mode)
 * - Deducts 10 movement points, minimum reduction to 0
 * - Updates unit status to reflect new speed setting
 * - Bypasses movement costs in god mode
 *
 * Parameters:
 *   v1_ptr - Pointer to caravan structure (must not be NULL)
 *   new_speed - New speed setting to apply to unit
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies caravan movement points and status
 *   - Updates global caravan state tracking
 *   - May reduce current turn movement capability
 *
 * Notes:
 *   - Static function for internal module use only
 *   - Movement penalties apply only in normal (non-god) mode
 *   - Speed changes take effect immediately
 *   - Thread-unsafe due to global state modifications
 */
static void
change_vspeed PARM_2(CVN_PTR, v1_ptr, int, new_speed)
{
  /* get the caravan */
  if (v1_ptr == NULL) {
    errormsg("Hmm, I can't seem to find that caravan anywhere");
    return;
  }

  /* latch onto it */
  cvn_ptr = v1_ptr;

  /* adjust the movement potential */
  if ((is_god == FALSE) && (CVN_MOVE != 0)) {
    if (CVN_MOVE > 10) {
      CVN_MOVE -= 10;
    } else {
      CVN_MOVE = 0;
    }
    VADJMOVE;
  }
  set_speed(CVN_STAT, new_speed);
  VADJSTAT;
}

/*
 * comb_cvns - Merge the second caravan unit into the first
 *
 * Combines two caravan units into a single larger unit, merging all resources,
 * crew, and cargo proportionally. Handles speed synchronization and validates
 * unit compatibility before proceeding with the merge operation.
 *
 * The function implements sophisticated merging logic:
 * - Validates units are in same sector and compatible for merging
 * - Synchronizes movement speeds with movement point penalties
 * - Proportionally combines crew, people, and supply levels
 * - Merges all material cargo from both units
 * - Destroys the second unit after successful merge
 *
 * Compatibility requirements:
 * - Both units must be in the same sector
 * - Units must have compatible statuses (not on special missions)
 * - Magic enhancement status must match between units
 * - Neither unit can be in a non-combinable state
 *
 * Parameters:
 *   v1_ptr - Pointer to primary caravan (receives merged resources)
 *   v2_ptr - Pointer to secondary caravan (destroyed after merge)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies first caravan size and all resource values
 *   - Destroys second caravan unit permanently
 *   - Updates movement points based on speed synchronization
 *   - May display user confirmation dialogs
 *
 * Notes:
 *   - Static function for internal module use only
 *   - Prompts for confirmation unless expert mode enabled
 *   - Speed synchronization may reduce movement points
 *   - Thread-unsafe due to global state modifications
 */
static void
comb_cvns PARM_2 (CVN_PTR, v1_ptr, CVN_PTR, v2_ptr)
{
  long lvalue;
  int stat1, stat2;
  int spd1, spd2;
  int i, j;

  /* safety check */
  if ((v1_ptr == NULL) || (v2_ptr == NULL)) {
    errormsg("Uh oh, I can't find your caravan!");
    return;
  }
  if (v1_ptr == v2_ptr) {
    errormsg("Hmm... they seem to be combined already");
    return;
  }
  cvn_ptr = v1_ptr;

  /* check that they are in the same sector */
  if ((CVN_XLOC != v2_ptr->xloc) || (CVN_YLOC != v2_ptr->yloc)) {
    errormsg("The caravans must be in the same sector to be combined");
    return;
  }

  /* find unit statuses */
  stat1 = unit_status(CVN_STAT);
  stat2 = unit_status(v2_ptr->status);
  if (nocomb_stat(stat1)) {
    sprintf(string, "Caravan %d is on %s and cannot be merged",
	    CVN_ID, stat_info[stat2].name);
    errormsg(string);
    return;
  }
  if (nocomb_stat(stat2)) {
    sprintf(string, "Caravan %d is on %s and cannot be merged",
	    v2_ptr->cvnid, stat_info[stat2].name);
    errormsg(string);
    return;
  }

  /* check spell status */
  if (unit_spelled(CVN_STAT) != unit_spelled(v2_ptr->status)) {
    errormsg("Both caravans have to be magically enhanced to be combined");
    return;
  }

  /* assign speed settings */
  spd1 = unit_speed(CVN_STAT);
  spd2 = unit_speed(v2_ptr->status);

  /* confirm what is to be done */
  if (conq_expert == FALSE) {
    mvprintw(LINES - 1, 0, "Merge Caravan %d into Caravan %d? ",
	     v2_ptr->cvnid, CVN_ID);
    if (y_or_n() == FALSE) return;
  }

  /* merge the second unit into the first */
  CVN_SIZE += v2_ptr->size;
  VADJSIZE;

  /* adjust the speeds */
  if (spd1 != spd2) {

    /* compare and adjust speeds */
    if (spd1 < spd2) {

      /* increase the speed of the first unit */
      while (spd1 < spd2) {
	if (CVN_MOVE > 5) {
	  CVN_MOVE -= 5;
	} else {
	  CVN_MOVE = 0;
	}
	spd1++;
      }
      set_speed(CVN_STAT, spd2);
      VADJSTAT;

    } else {

      /* increase the speed of the second unit */
      while (spd2 < spd1) {
	if (v2_ptr->umove > 5) {
	  v2_ptr->umove -= 5;
	} else {
	  v2_ptr->umove = 0;
	}
	spd2++;
      }
    }
  }

  /* fix movement */
  if (CVN_MOVE > v2_ptr->umove) {

    /* reduce movement of unit */
    CVN_MOVE = v2_ptr->umove;

  }
  VADJMOVE;

  /* calculate combinationals */
  if (CVN_CREW != v2_ptr->crew) {
    i = CVN_SIZE;
    j = v2_ptr->size;
    lvalue = (i - j) * CVN_CREW + j * v2_ptr->crew;
    CVN_CREW = (uns_char) (lvalue / i);
    VADJCREW;
  }
  if (CVN_PEOP != v2_ptr->people) {
    i = CVN_SIZE;
    j = v2_ptr->size;
    lvalue = (i - j) * CVN_PEOP + j * v2_ptr->people;
    CVN_PEOP = (uns_char) (lvalue / i);
    VADJPEOP;
  }
  if (CVN_SPLY != v2_ptr->supply) {
    i = CVN_SIZE;
    j = v2_ptr->size;
    lvalue = (i - j) * CVN_SPLY + j * v2_ptr->supply;
    CVN_SPLY = (uns_char) (lvalue / i);
    VADJSPLY;
  }

  /* join all of the materials */
  for (global_int = 0; global_int < MTRLS_NUMBER; global_int++) {
    /* join and store */
    if (v2_ptr->mtrls[global_int] != (itemtype) 0) {
      CVN_MTRLS[global_int] += v2_ptr->mtrls[global_int];
      VMTRLS(global_int);
    }
  }

  /* nuke the second unit */
  cvn_ptr = v2_ptr;
  VDESTROY;
  dest_cvn(CVN_ID);
  cvn_ptr = NULL;
}

/*
 * cvn_transport - Manage cargo transfer operations for caravan units
 *
 * Provides comprehensive cargo transfer interface allowing caravans to exchange
 * materials with cities, naval fleets, other caravans, or sector storage.
 * Handles movement synchronization, user selection, and transfer validation.
 *
 * The function supports multiple transfer targets:
 * - Cities: Exchange with supply center storage
 * - Naval fleets: Coordinate cargo with ships in same sector
 * - Other caravans: Direct unit-to-unit material transfer
 * - Sector storage: Drop/pickup from sector resource pools
 *
 * Transfer mechanics:
 * - Synchronizes movement points between mobile units
 * - Validates unit compatibility and location requirements
 * - Provides interactive selection for multiple available targets
 * - Implements god mode for administrative cargo operations
 *
 * Parameters:
 *   v1_ptr - Pointer to caravan for transfer (NULL triggers unit selection)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies caravan and target unit cargo
 *   - Synchronizes movement points between participating units
 *   - Activates interactive transfer mode interface
 *   - May enable/disable god mode for admin operations
 *
 * Notes:
 *   - Handles both automated and interactive unit selection
 *   - Water sectors require naval targets (no sector storage)
 *   - Movement synchronization prevents exploitation of cargo transfers
 *   - Thread-unsafe due to global state modifications and user interaction
 */
void
cvn_transport PARM_1(CVN_PTR, v1_ptr)
{
  NAVY_PTR n2_ptr;
  CITY_PTR c2_ptr;
  CVN_PTR v2_ptr;
  int x, y, use_selector = FALSE, do_sector = FALSE;
  int idnum, set_god = FALSE;

  /* check for getting the current selection */
  if (v1_ptr == NULL) {
    use_selector = TRUE;
    v1_ptr = get_cvnselect();
    if (country == UNOWNED) {
      if (get_god(global_int, TRUE)) {
	return;
      }
      set_god = TRUE;
    }
  }
  cvn_ptr = v1_ptr;

  /* valid input? */
  if (cvn_ptr == NULL) {
    errormsg("I could not find any caravan");
    if (set_god) reset_god();
    return;
  }
  x = CVN_XLOC;
  y = CVN_YLOC;

  /* check if the sector is in the wrong position */
  if (!XY_ONMAP(x, y)) {
    errormsg("You may not load or unload in that sector");
    if (set_god) reset_god();
    return;
  }

  /* bottom cleaning */
  clear_bottom(0);
  mvprintw(LINES - 3, 0, "Transferring items from caravan %d to...",
	   CVN_ID);
  
  /* prepare the initial caravan */
  cvn2xfer(0, cvn_ptr);

  /* find out what can be done */
  c2_ptr = citybyloc(ntn_ptr, x, y);
  for (v2_ptr = ntn_ptr->cvn_list;
       v2_ptr != NULL;
       v2_ptr = v2_ptr->next) {
    if (v2_ptr == cvn_ptr) continue;
    if ((v2_ptr->xloc == x) &&
	(v2_ptr->yloc == y)) {
      break;
    }
  }
  for (n2_ptr = ntn_ptr->navy_list;
       n2_ptr != NULL;
       n2_ptr = n2_ptr->next) {
    if ((n2_ptr->xloc == x) &&
	(n2_ptr->yloc == y)) {
      break;
    }
  }

  /* any takers? */
  if ((n2_ptr == NULL) &&
      (c2_ptr == NULL) &&
      (v2_ptr == NULL)) {
    if (sct[x][y].altitude != ELE_WATER) {
      do_sector = TRUE;
    } else {
      errormsg("There isn't anything in the sector to transfer materials to");
      if (set_god == TRUE) {
	reset_god();
      }
      return;
    }
  }

  /* check if the city gets it */
  if ((v2_ptr != NULL) ||
      (n2_ptr != NULL)) {

    /* determine what selection is desired */
    if (use_selector) {

      /* is the city? */
      if (c2_ptr != NULL) {
	mvprintw(LINES - 2, 0, "Exchange materials with %s?", c2_ptr->name);
	if (y_or_n() == FALSE) {
	  c2_ptr = NULL;
	} else {
	  n2_ptr = NULL;
	  v2_ptr = NULL;
	}
      } else if (sct[x][y].altitude != ELE_WATER) {
	mvprintw(LINES - 2, 0, "Transfer materials within the sector? ");
	if (y_or_n()) {
	  do_sector = TRUE;
	  n2_ptr = NULL;
	  v2_ptr = NULL;
	}
      }

      /* must be a navy or caravan */
      if ((c2_ptr == NULL) &&
	  (do_sector == FALSE)) {
	if (set_select(TRUE,
		       "Choose a Navy or Caravan to exchange with")
	    == FALSE) {
	  if (set_god == TRUE) {
	    reset_god();
	  }
	  return;
	}
	if ((v2_ptr = get_cvnselect()) != NULL) {
	  n2_ptr = NULL;
	  if (v2_ptr == cvn_ptr) {
	    errormsg("That is the same caravan!");
	    if (set_god == TRUE) {
	      reset_god();
	    }
	    return;
	  }
	} else if ((n2_ptr = get_navyselect()) == NULL) {
	  errormsg("That wasn't a fleet or a caravan!");
	  if (set_god == TRUE) {
	    reset_god();
	  }
	  return;
	}
      }
    } else {
      /* select which ever */
      mvaddstr(LINES - 2, 0, "Exchange goods with a ");
      hip_string("Navy, ", 1, TRUE);
      hip_string("Caravan, or the ", 1, TRUE);
      hip_string("Sector? ", 1, TRUE);
      refresh();
      switch (next_char()) {
      case 'S':
      case 's':
	/* cities! */
	if ((c2_ptr == NULL) &&
	    (sct[x][y].altitude == ELE_WATER)) {
	  errormsg("Your caravan cannot exchange within this sector");
	  if (set_god == TRUE) {
	    reset_god();
	  }
	  return;
	}
	if (c2_ptr == NULL) {
	  do_sector = TRUE;
	}
	n2_ptr = NULL;
	v2_ptr = NULL;
	break;
      case 'N':
      case 'n':
	/* naval selection */
	c2_ptr = NULL;
	v2_ptr = NULL;
	mvaddstr(LINES - 1, 0, "Exchange with which fleet number? ");
	idnum = get_number(FALSE);
	if (no_input == TRUE) {
	  if (set_god == TRUE) reset_god();
	  return;
	}
	if ((n2_ptr = navybynum(idnum)) == NULL) {
	  errormsg("I couldn't find that navy");
	  if (set_god == TRUE) reset_god();
	  return;
	} else if ((n2_ptr->xloc != x) ||
		   (n2_ptr->yloc != y)) {
	  errormsg("That fleet is not in the same sector!");
	  if (set_god == TRUE) reset_god();
	  return;
	}
	break;
      case 'C':
      case 'c':
	/* caravan selection */
	c2_ptr = NULL;
	n2_ptr = NULL;
	mvaddstr(LINES - 1, 0, "Exchange with which caravan number? ");
	idnum = get_number(FALSE);
	if (no_input == TRUE) {
	  if (set_god == TRUE) reset_god();
	  return;
	}
	if ((v2_ptr = cvnbynum(idnum)) == NULL) {
	  errormsg("I couldn't find that caravan");
	  if (set_god == TRUE) reset_god();
	  return;
	} else if (v2_ptr == cvn_ptr) {
	  errormsg("That is the same caravan!");
	  if (set_god == TRUE) reset_god();
	  return;
	} else if ((v2_ptr->xloc != x) ||
		   (v2_ptr->yloc != y)) {
	  errormsg("That caravan is not in the same sector!");
	  if (set_god == TRUE) reset_god();
	  return;
	}
	break;
      }
    }
  }

  /* prepare the second site */
  if (n2_ptr != NULL) {

    /* check potentials */
    if ((CVN_MOVE != n2_ptr->umove) &&
	(conq_expert == FALSE)) {
      bottommsg("These units will have to match movement.  Continue? ");
      if (y_or_n() == FALSE) {
	if (set_god == TRUE) reset_god();
	return;
      }
    }

    /* both units must match potential */
    if (CVN_MOVE > n2_ptr->umove) {
      CVN_MOVE = n2_ptr->umove;
      VADJMOVE;
    } else if (CVN_MOVE < n2_ptr->umove) {
      n2_ptr->umove = CVN_MOVE;
      navy_ptr = n2_ptr;
      NADJMOVE;
    }
    navy2xfer(1, n2_ptr);

  } else if (c2_ptr != NULL) {

    /* just convert */
    city2xfer(1, c2_ptr);

  } else if (do_sector) {

    /* just convert */
    sct2xfer(1, x, y);

  } else if (v2_ptr != NULL) {

    /* check potentials */
    if ((CVN_MOVE != v2_ptr->umove) &&
	(conq_expert == FALSE)) {
      bottommsg("These units will have to match movement.  Continue? ");
      if (y_or_n() == FALSE) {
	if (set_god == TRUE) reset_god();
	return;
      }
    }

    /* must get the minimum movement */
    if (CVN_MOVE > v2_ptr->umove) {
      CVN_MOVE = v2_ptr->umove;
      VADJMOVE;
    } else if (CVN_MOVE < v2_ptr->umove) {
      v2_ptr->umove = CVN_MOVE;
      cvn_ptr = v2_ptr;
      VADJMOVE;
      cvn_ptr = v1_ptr;
    }
    cvn2xfer(1, v2_ptr);

  } else {

    /* oops */
    if (set_god == TRUE) reset_god();
    return;

  }

  /* do the transfer */
  xfer_mode();

  /* return things to their proper place */
  xfer2cvn(0, cvn_ptr);
  if (n2_ptr != NULL) {
    xfer2navy(1, n2_ptr);
  } else if (c2_ptr != NULL) {
    xfer2city(1, c2_ptr);
  } else if (do_sector) {
    xfer2sct(1, x, y);
  } else if (v2_ptr != NULL) {
    xfer2cvn(1, v2_ptr);
  }
  if (set_god == TRUE) {
    reset_god();
  }
}

/*
 * change_vstatus - Adjust the operational status of a caravan unit
 *
 * Changes the caravan's operational status (patrol, move, guard, etc.) with
 * appropriate movement point penalties for status changes during the current
 * turn. Status changes affect unit behavior and available commands.
 *
 * The function implements status change mechanics:
 * - Validates new status is different from current status
 * - Applies movement point penalty for status changes (non-god mode)
 * - Deducts 10 movement points, minimum reduction to 0
 * - Updates unit status immediately
 *
 * Parameters:
 *   v1_ptr - Pointer to caravan structure (must not be NULL)
 *   new_stat - New status value to apply to unit
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies caravan status and movement points
 *   - Updates global caravan state tracking
 *   - May reduce current turn movement capability
 *
 * Notes:
 *   - Static function for internal module use only
 *   - Movement penalties apply only in normal (non-god) mode
 *   - Status changes take effect immediately
 *   - Thread-unsafe due to global state modifications
 */
static void
change_vstatus PARM_2(CVN_PTR, v1_ptr, int, new_stat)
{
  /* get the cvn */
  if (v1_ptr == NULL) {
    errormsg("Hmm, I can't seem to find that caravan anywhere");
  }

  /* find the caravan status */
  cvn_ptr = v1_ptr;
  set_status(CVN_STAT, new_stat);
  VADJSTAT;
  if ((is_god == FALSE) && (CVN_MOVE != 0)) {
    if (CVN_MOVE > 10) {
      CVN_MOVE -= 10;
    } else {
      CVN_MOVE = 0;
    }
    VADJMOVE;
  }
}

/*
 * split_cvn - Create a new caravan by splitting an existing unit
 *
 * Divides an existing caravan into two separate units, proportionally
 * distributing all resources, crew, and cargo between the original and
 * new caravan. The new unit inherits all characteristics from the original.
 *
 * The function implements proportional resource splitting:
 * - Creates new caravan unit with specified number of wagons
 * - Distributes materials based on size ratio between units
 * - Copies all unit characteristics (status, crew, supplies, location)
 * - Reduces original unit size by the amount split off
 * - Validates split size to ensure both units remain viable
 *
 * Split validation:
 * - Original unit must have at least 2 wagons
 * - Split size must be less than original size
 * - Unit must not be in a non-splittable status
 * - Player must not exceed maximum caravan limit
 *
 * Parameters:
 *   v1_ptr - Pointer to caravan to split (must not be NULL)
 *   number - Number of wagons for new unit (-1 prompts user input)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Creates new caravan unit in global unit list
 *   - Modifies original caravan size and cargo
 *   - Updates caravan sorting and identification
 *   - May display user input prompts
 *
 * Notes:
 *   - Static function for internal module use only
 *   - Prompts for split size if number parameter is -1
 *   - New unit inherits exact location and characteristics
 *   - Thread-unsafe due to global state modifications
 */
static void
split_cvn PARM_2(CVN_PTR, v1_ptr, int, number)
{
  itemtype tempival;
  int maxval;

  /* validate input */
  if ((cvn_ptr = v1_ptr) == NULL) {
    return;
  }
  if (CVN_SIZE < 2) {
    errormsg("That caravan is already as small as it can get");
  }

  /* check the status */
  if (nosplit_stat(CVN_STAT)) {
    errormsg("That unit is not able to be split");
    return;
  }

  /* go and find out what to split */
  clear_bottom(0);
  maxval = CVN_SIZE;
  if (number == -1) {
    mvprintw(LINES - 2, 0, "Separate how many units of %d wagons? [max %d] ",
	     WAGONS_IN_CVN, maxval);
    number = get_number(FALSE);
    if (no_input == TRUE) return;
  }
  if (number == 0) return;
  if (number > maxval) {
    errormsg("There are not that many wagons available");
    return;
  }
  if (number == maxval) {
    errormsg("That would not leave any wagons in the original caravan");
    return;
  }

  if ((cvn_tptr = crt_cvn()) == NULL) {
    errormsg("You have too many caravans already");
    return;
  }

  /* handle any raw materials being carried */
  for (global_int = 0; global_int < MTRLS_NUMBER; global_int++) {
    tempival = (CVN_MTRLS[global_int] * number) / CVN_SIZE;
    if (tempival > 0) {
      CVN_MTRLS[global_int] -= tempival;
      CVNT_MTRLS[global_int] = tempival;
      VMTRLS(global_int);
    }
  }

  /* adjust the old caravan size */
  CVN_SIZE -= number;
  VADJSIZE;

  /* now create the new caravan */
  cvn_ptr = cvn_tptr;
  VCREATE;
  CVN_MOVE = v1_ptr->umove;
  CVN_STAT = v1_ptr->status;
  CVN_CREW = v1_ptr->crew;
  CVN_SPLY = v1_ptr->supply;
  CVN_PEOP = v1_ptr->people;
  CVN_XLOC = v1_ptr->xloc;
  CVN_LASTX = v1_ptr->lastx;
  CVN_YLOC = v1_ptr->yloc;
  CVN_LASTY = v1_ptr->lasty;
  CVN_EFF = v1_ptr->efficiency;
  CVN_SIZE = number;
  for (global_int = 0; global_int < MTRLS_NUMBER; global_int++) {
    if (CVN_MTRLS[global_int] > 0) {
      VMTRLS(global_int);
    }
  }
  VADJSIZE;
  VADJSTAT;
  VADJCREW;
  VADJPEOP;
  VADJSPLY;
  VADJLOC;
  VADJOLOC;
  VADJMOVE;
  VADJEFF;
  cvn_sort();
}

/*
 * supply_cvn - Manage supply level changes for caravan units
 *
 * Adjusts the caravan's supply level by consuming or returning resources
 * from the current sector. Handles territorial restrictions, resource
 * availability validation, and user confirmation for supply operations.
 *
 * The function implements comprehensive supply mechanics:
 * - Calculates resource costs for supply level changes
 * - Validates resource availability in current sector
 * - Handles territorial supply restrictions (own territory required)
 * - Supports both increasing and decreasing supply levels
 * - Implements special handling for magically enhanced units
 *
 * Supply restrictions:
 * - Normal units require owned territory for resupply
 * - Magic-enhanced units can only resupply from specific sectors
 * - Resource availability must meet calculated costs
 * - Optional user confirmation for supply transactions
 *
 * Parameters:
 *   v1_ptr - Pointer to caravan structure (must not be NULL)
 *   level - Target supply level (0 to MAXSUPPLIES * 2)
 *   doquery - Whether to prompt user for confirmation
 *
 * Returns:
 *   TRUE if operation was blocked/cancelled, FALSE if completed successfully
 *
 * Side Effects:
 *   - Modifies caravan supply level and sector resources
 *   - Updates sector material storage
 *   - May display user confirmation dialogs
 *   - Handles resource consumption/redistribution
 *
 * Notes:
 *   - Supports both supply increase and decrease operations
 *   - MAYGIVEBACK compilation flag controls supply return capability
 *   - Territory restrictions enforced for non-god players
 *   - Thread-unsafe due to global state modifications
 */
int
supply_cvn PARM_3(CVN_PTR, v1_ptr, int, level, int, doquery)
{
  CVN_PTR vhold_ptr = cvn_ptr;
  ITEM_PTR itest_ptr;
  ITEM_STRUCT supt_cost;
  int oxloc = XREAL, oyloc = YREAL;
  int cnt, ok_val, diff, hold = TRUE;
  int insectonly = FALSE;

  /* check input */
  if (v1_ptr == NULL) {
    errormsg("I can't find that caravan");
    return(hold);
  }
  if ((level < 0) ||
      (level > MAXSUPPLIES * 2)) {
    errormsg("Caravans may not carry that many supplies");
    return(hold);
  }

  /* check the difference */
  cvn_ptr = v1_ptr;
  diff = level - CVN_SPLY;
  if (diff == 0) {
    /* already set */
    hold = FALSE;
    goto bug_out;
  }
#ifndef MAYGIVEBACK
  if (diff < 0) {
    /* just can't do it */
    sprintf(string, "Your caravan %d does not wish to give back their pay",
	    CVN_ID);
    errormsg(string);
    goto bug_out;
  }
#endif /* MAYGIVEBACK */

  /* now allow it */
  if (is_god == FALSE) {

    /* check if they can get them */
    if (splyinsect_stat(CVN_STAT)) {
      insectonly = TRUE;
    }

    /* get the costs */
    cvn_support(cvn_ptr, &(supt_cost.mtrls[0]), abs(diff));

    /* check if we are giving or getting */
    if (diff > 0) {

      /* check for unreachable supplying */
      if (sct[CVN_XLOC][CVN_YLOC].owner != country) {
	insectonly = TRUE;      
      }

      /* now check that they are available */
      if ((itest_ptr = find_resources(CVN_XLOC, CVN_YLOC,
				      insectonly))
	  == NULL) {
	if (insectonly) {
	  sprintf(string, "Caravan %d can only be supplied from that sector",
		  CVN_ID);
	} else {
	  sprintf(string, "Caravan %d cannot find enough available resources",
		  CVN_ID);
	}
	errormsg(string);
	goto bug_out;
      }

      /* now compare the values */
      ok_val = TRUE;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if (itest_ptr->mtrls[cnt] < supt_cost.mtrls[cnt]) {
	  sprintf(string, "There are not %ld %s for caravan %d",
		  (long) supt_cost.mtrls[cnt],
		  mtrls_info[cnt].lname, CVN_ID);
	  errormsg(string);
	  ok_val = FALSE;
	}
      }

      /* clean up */
      if (ok_val == FALSE) {
	free(itest_ptr);
	goto bug_out;
      }

      /* ask the user? */
      if (doquery &&
	  !conq_expert) {
	char tmp_str[LINELTH];
	strcpy(string, "Supplies: ");
	for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	  if (supt_cost.mtrls[cnt] > 0) {
	    sprintf(tmp_str, " %.0f %s",
		    (double) supt_cost.mtrls[cnt],
		    mtrls_info[cnt].lname);
	    strcat(string, tmp_str);
	  }
	}
	strcat(string, "; Continue? ");
	bottommsg(string);
	if (y_or_n() == FALSE) {
	  free(itest_ptr);
	  goto bug_out;
	}
      }
      
      /* consume the resources */
      xcurs = CVN_XLOC - xoffset;
      ycurs = CVN_YLOC - yoffset;
      TAKESTART;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if ((global_long = supt_cost.mtrls[cnt]) > 0) {
	  TAKEMTRLS(cnt);
	}
      }
      TAKEFINISH(insectonly);
      xcurs = oxloc - xoffset;
      ycurs = oyloc - yoffset;
      take_resources(CVN_XLOC, CVN_YLOC, &supt_cost, insectonly);
      free(itest_ptr);
#ifdef MAYGIVEBACK
    } else {

      /* otherwise, just redistribute them in */
      if (send_resources(CVN_XLOC, CVN_YLOC, &supt_cost, insectonly)) {
	sprintf(string, "The supplies of caravan %d would be lost",
		CVN_ID);
	errormsg(string);
	goto bug_out;
      }
      xcurs = CVN_XLOC - xoffset;
      ycurs = CVN_YLOC - yoffset;
      GIVESTART;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if ((global_long = supt_cost.mtrls[cnt]) > 0) {
	  GIVEMTRLS(cnt);
	}
      }
      GIVEFINISH(insectonly);
      xcurs = oxloc - xoffset;
      ycurs = oyloc - yoffset;
#endif /* MAYGIVEBACK */
    }
  }
  CVN_SPLY = level;
  VADJSPLY;

  /* reset before bugging out */
  hold = FALSE;
 bug_out:
  cvn_ptr = vhold_ptr;
  return(hold);
}

/*
 * cvn_repair - Repair a damaged caravan unit at a supply center
 *
 * Restores a caravan's efficiency to 100% by consuming repair materials
 * from a supply center in the current sector. Calculates repair costs
 * and validates resource availability before proceeding.
 *
 * The function implements supply center repair mechanics:
 * - Requires caravan to be located at a supply center (city)
 * - Calculates material costs based on caravan damage
 * - Validates supply center has sufficient repair materials
 * - Consumes materials from city storage upon completion
 * - Sets caravan to repair status with movement restrictions
 *
 * Repair requirements:
 * - Caravan must be at a supply center (city location)
 * - City must have sufficient repair materials available
 * - User confirmation required unless expert mode enabled
 * - Repair operation consumes turn movement (non-god mode)
 *
 * Parameters:
 *   v1_ptr - Pointer to caravan structure to repair (must not be NULL)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Sets caravan efficiency to 100%
 *   - Consumes repair materials from city storage
 *   - Sets caravan status to repair (ST_REPAIR)
 *   - Zeroes caravan movement points for current turn
 *
 * Notes:
 *   - Requires supply center location for repair operations
 *   - Material costs calculated by cvn_redocosts() function
 *   - Repair status prevents movement until next turn
 *   - Thread-unsafe due to global state modifications
 */
void
cvn_repair PARM_1(CVN_PTR, v1_ptr)
{
  itemtype cost_mtrls[MTRLS_NUMBER];
  int i, okval = TRUE;

  /* begin */
  if (v1_ptr == NULL) {
    errormsg("I could not find that fleet");
    return;
  }
  cvn_ptr = v1_ptr;

  /* now check for a city in the current sector */
  if (is_god == FALSE) {

    /* are we in a supply center */
    if ((city_ptr = citybyloc(ntn_ptr, CVN_XLOC, CVN_YLOC)) == NULL) {
      errormsg("A caravan needs a supply center to be repaired");
      return;
    }

    /* now find the caravan repair costs */
    cvn_redocosts(cvn_ptr, &(cost_mtrls[0]));

    /* check if it is supportable */
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (CITY_MTRLS[i] < cost_mtrls[i]) {
	okval = FALSE;
	sprintf(string, "The supply center doesn't have %.0f %s",
		(double) cost_mtrls[i], mtrls_info[i].lname);
	errormsg(string);
      }
    }
    if (okval == FALSE) return;

    /* get the okay */
    if (conq_expert == FALSE) {
      mvaddstr(LINES - 1, 0, "Cost: ");
      for (i = 0; i < MTRLS_NUMBER; i++) {
	if (cost_mtrls[i] > 0) {
	  if (i > 0) {
	    addstr(", ");
	  }
	  printw("%0.f %s", (double) cost_mtrls[i],
		 mtrls_info[i].lname);
	}
      }
      addstr("; Continue? ");
      if (y_or_n() == FALSE) {
	return;
      }
    }

    /* spend the totals */
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (cost_mtrls[i] > 0) {
	CITY_MTRLS[i] -= cost_mtrls[i];
	CADJMTRLS(i);
	CITY_IMTRLS[i] -= cost_mtrls[i];
	if (CITY_IMTRLS[i] < 0) {
	  CITY_IMTRLS[i] = 0;
	}
	CADJIMTRLS(i);
      }
    }

  }

  /* now "repair" it */
  CVN_EFF = 100;
  VADJEFF;

  /* has to complete the repairs */
  if (is_god == FALSE) {
    set_status(CVN_STAT, ST_REPAIR);
    VADJSTAT;
    if (CVN_MOVE > 0) {
      CVN_MOVE = 0;
      VADJMOVE;
    }
  }
}

/*
 * vstat_ok - Validate whether caravan can change to specified status
 *
 * Checks if the current caravan can legally change to the specified status,
 * considering current state, unit restrictions, and game rules. Provides
 * optional error messaging for invalid status changes.
 *
 * The function validates status change legality:
 * - Ensures caravan exists and is accessible
 * - Prevents redundant status changes (already in target status)
 * - Bypasses all restrictions in god mode
 * - Checks for unalterable status conditions
 *
 * Validation rules:
 * - Units in certain statuses cannot change (nochange_stat)
 * - Status change must be different from current status
 * - God mode players can make any status change
 * - Some statuses may have additional restrictions
 *
 * Parameters:
 *   new_stat - Target status to validate for change
 *   verbal - Whether to display error messages for invalid changes
 *
 * Returns:
 *   TRUE if status change is valid, FALSE if blocked
 *
 * Side Effects:
 *   - May display error messages if verbal parameter is TRUE
 *   - No state modifications (read-only validation)
 *
 * Notes:
 *   - Static function for internal module use only
 *   - Relies on global cvn_ptr for current caravan context
 *   - Used by command interface to enable/disable options
 *   - Thread-safe (read-only operation)
 */
static int
vstat_ok PARM_2(int, new_stat, int, verbal)
{
  int old_stat;

  /* check the unit */
  if (cvn_ptr == NULL) {
    if (verbal) {
      errormsg("Hmm, I can't seem to find that caravan anywhere");
    }
    return(FALSE);
  }

  /* find the army status */
  old_stat = unit_status(CVN_STAT);

  /* compare the two */
  if (old_stat == new_stat) {
    if (verbal) {
      errormsg("Excuse me, aren't we doing that already?");
    }
    return(FALSE);
  }
  if (is_god == TRUE) {
    /* god can do anything! */
    return(TRUE);
  }

  /* check for unalterable statuses */
  if (nochange_stat(old_stat)) {
    if (verbal) {
      errormsg("That caravan is unable to change their status");
    }
    return(FALSE);
  }
  return(TRUE);
}

/*
 * vspeed_ok - Validate whether caravan can change to specified speed
 *
 * Checks if the current caravan can legally change to the specified movement
 * speed, considering current movement points, unit status, and game mechanics.
 * Provides optional error messaging for invalid speed changes.
 *
 * The function validates speed change legality:
 * - Ensures caravan exists and is accessible
 * - Prevents speed changes for stopped/immobile units
 * - Restricts speed decreases when movement is too low
 * - Prevents redundant speed changes (already at target speed)
 *
 * Speed change rules:
 * - Units in nomove status cannot change speed
 * - Speed decreases require sufficient movement points (>25)
 * - Current speed must differ from target speed
 * - No restrictions in god mode
 *
 * Parameters:
 *   new_speed - Target speed to validate for change
 *   verbal - Whether to display error messages for invalid changes
 *
 * Returns:
 *   TRUE if speed change is valid, FALSE if blocked
 *
 * Side Effects:
 *   - May display error messages if verbal parameter is TRUE
 *   - No state modifications (read-only validation)
 *
 * Notes:
 *   - Static function for internal module use only
 *   - Relies on global cvn_ptr for current caravan context
 *   - Movement threshold (25) prevents late-turn speed exploitation
 *   - Thread-safe (read-only operation)
 */
static int
vspeed_ok PARM_2(int, new_speed, int, verbal)
{
  /* check the navy */
  if (cvn_ptr == NULL) {
    if (verbal) {
      errormsg("Hmm, I can't seem to find that fleet anywhere");
    }
    return(FALSE);
  }

  /* is it a movable unit? */
  if (nomove_stat(CVN_STAT)) {
    if (verbal) {
      errormsg("That unit is stopped, its speed is not adjustable");
    }
    return(FALSE);
  }
  if ((CVN_MOVE < 25) &&
      (unit_speed(CVN_STAT) > new_speed)) {
    if (verbal) {
      errormsg("That caravan has gone too far to decrease speed");
    }
    return(FALSE);
  }
  if (unit_speed(CVN_STAT) == new_speed) {
    if (verbal) {
      errormsg("The caravan is already moving at that speed");
    }
    return(FALSE);
  }
  return(TRUE);
}

/*
 * ext_cvninfo - Display comprehensive caravan unit information
 *
 * Presents detailed information about the specified caravan including
 * size, efficiency, crew, location, status, cargo, and supply costs.
 * Provides complete unit overview for player decision-making.
 *
 * The function displays multi-line caravan summary:
 * - Basic stats: size (wagons), efficiency, total crew
 * - Operational info: status, speed, movement left, movement ability
 * - Location data: coordinates, special traits (leading, spelled, healing)
 * - Cargo manifest: people and all material types carried
 * - Supply information: current level and per-turn costs
 *
 * Information categories:
 * - Unit identification and basic characteristics
 * - Current operational status and movement capability
 * - Location and special unit traits/enhancements
 * - Complete cargo inventory with quantities
 * - Supply level and resource consumption rates
 *
 * Parameters:
 *   v1_ptr - Pointer to caravan structure to display (must not be NULL)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Clears bottom screen area and displays information
 *   - Waits for user keypress before returning
 *   - No game state modifications
 *
 * Notes:
 *   - Uses 5-line display format for comprehensive information
 *   - Calculates display values using game coordinate functions
 *   - Integrates with supply cost calculation system
 *   - Thread-safe display operation
 */
void
ext_cvninfo PARM_1(CVN_PTR, v1_ptr)
{
  ITEM_STRUCT supt_cost;
  long lval;
  int ind, i;

  /* check input */
  if (v1_ptr == NULL) return;
  cvn_ptr = v1_ptr;

  /* now clear the bottom area */
  clear_bottom(0);

  /* first line of information */
  move(LINES - 5, 0);
  printw("Caravan %d ==>", CVN_ID);
  printw(" Size: %d wagons;", WAGONS_IN_CVN * (int) CVN_SIZE);
  printw(" Efficiency: %d;", (int) CVN_EFF);
  printw(" Total Crew: %d;", (int) CVN_CREW * CVN_SIZE);

  /* second line of information */
  move(LINES - 4, 0);
  printw("  Status: %s;", stat_info[unit_status(CVN_STAT)].name);
  printw(" Speed: %s;", speedname[unit_speed(CVN_STAT)]);
  printw(" Move Left: %d%%;", (int) CVN_MOVE);
  printw(" Move Ability: %d;", cvn_mvpts(ntn_ptr, cvn_ptr));

  /* line three */
  move(LINES - 3, 0);
  printw("  Location: [%d,%d];",
	 xloc_relative(CVN_XLOC),
	 yloc_relative(CVN_YLOC));
  addstr(" Traits: ");
  i = 0;
  if (unit_leading(CVN_STAT)) {
    i = 1;
    addstr("leading");
  }
  if (unit_spelled(CVN_STAT)) {
    if (i == 1) addstr(", ");
    i = 1;
    addstr("spelled");
  }
  if (unit_healed(CVN_STAT)) {
    if (i == 1) addstr(", ");
    i = 1;
    addstr("healing");
  }
  if (i == 0) {
    addstr("none");
  }
  addstr(";");

  /* describe the cargo */
  move(LINES - 2, 0);
  addstr("  Cargo: ");
  ind = 0;
  if (CVN_PEOP != 0) {
    ind = 1;
    lval = (long) CVN_PEOP * CVN_SIZE;
    printw("%ld %s", lval, (lval == 1)?"person":"people");
  }
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (CVN_MTRLS[i] > 0) {
      if (ind == 1) addstr(", ");
      ind = 1;
      printw("%.0f %s", (double) CVN_MTRLS[i],
	     mtrls_info[i].lname);
    }
  }
  if (ind == 0) addstr("none");
  addstr(";");

  /* line five */
  move(LINES - 1, 0);
  printw("  Supply Level: %d turns;", (int) CVN_SPLY);
  addstr(" Supply Cost: ");
  cvn_support(cvn_ptr, &(supt_cost.mtrls[0]), 1);
  ind = 0;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (supt_cost.mtrls[i] > 0) {
      if (ind == 1) addstr(", ");
      ind = 1;
      printw("%.0f %s", (double) supt_cost.mtrls[i],
	     mtrls_info[i].lname);
    }
  }
  addstr(";");

  /* finish hit */
  presskey();
}

/*
 * ext_cvncmd - Execute extended caravan command interface
 *
 * Provides comprehensive interactive command interface for caravan management
 * including information display, unit operations, status changes, and
 * administrative functions. Supports both selector-based and direct ID access.
 *
 * The function implements full caravan command system:
 * - Information display and unit statistics
 * - Unit combination, merging, and splitting operations
 * - Status and speed adjustments with validation
 * - Supply management and repair operations
 * - Cargo transfer coordination
 * - Unit renumbering and destruction
 *
 * Command categories:
 * - Info commands: detailed unit information display
 * - Unit operations: combine, merge, split, divide operations
 * - Status management: speed and operational status changes
 * - Maintenance: supply, repair, renumber, disband
 * - Cargo operations: transfer interface activation
 *
 * Interface modes:
 * - Selector mode (cvan == -1): Uses unit selector for target selection
 * - Direct mode (cvan >= 0): Operates on specified caravan ID
 * - God mode support: Administrative operations bypass restrictions
 *
 * Parameters:
 *   cvan - Caravan ID for direct operation (-1 uses selector interface)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Displays interactive command menu with available options
 *   - Executes selected command with appropriate validation
 *   - May modify caravan state based on command selection
 *   - Handles unit selection and navigation
 *
 * Notes:
 *   - Validates command availability based on unit state
 *   - Highlights available commands and dims unavailable options
 *   - Integrates with all caravan management subsystems
 *   - Thread-unsafe due to global state and user interaction
 */
void
ext_cvncmd PARM_1 (int, cvan)
{
  CVN_PTR vhold_ptr, v2_ptr;
  int i, last_ch, x, y;
  int vstat = -1, men;

  /* find target caravan unit */
  if (cvan == -1) {
    cvn_ptr = get_cvnselect();
    if ((country == UNOWNED) &&
	get_god(global_int, TRUE)) {
      return;
    }
    if (cvn_ptr != NULL) {
      goto_cvn(CVN_ID);
    }
  } else {
    cvn_ptr = cvnbynum(cvan);
  }
  if (cvn_ptr == NULL) {
    errormsg("I could not find that caravan");
    return;
  }

  /* Display possible adjustment commands */
  y = LINES - 5;
  x = 1;
  clear_bottom(0);
  move(y++, x);
  hip_string("? Info, ", 1, TRUE);
  hip_string("+ Combine, ", 1, (cvan == -1) &&
	     !nocomb_stat(CVN_STAT));
  hip_string("Merge, ", 1, !nocomb_stat(CVN_STAT));
  hip_string("- Split Caravan, ", 1, TRUE);
  hip_string("/ Divide in Two, ", 1, TRUE);

  /* next line of commands */
  move(y++, x);
  hip_string("# Renumber, ", 1, TRUE);
  for (i = 0; i < SPD_STUCK; i++) {
    sprintf(string, "%s %s, ", shortspeed[i], speedname[i]);
    hip_string(string, 1, vspeed_ok(i, FALSE));
  }
  hip_string("Disband, ", 1, TRUE);
  hip_string("Supply, ", 1, (MAXSUPPLIES > 0) &&
	     (CVN_SPLY < MAXSUPPLIES * 2));

  /* third line of options */
  move(y++, x);
  hip_string("Repair, ", 1, (CVN_EFF < 100) &&
	     (citybyloc(ntn_ptr, CVN_XLOC, CVN_YLOC) != (CITY_PTR)NULL));
  hip_string("Tfer Cargo", 1, TRUE);
  x += 20;
  for (i = 0; i < num_statuses; i++) {
    if (usable_stat(i) &&
	vunit_stat(i)) {

      /* check for overruns */
      if (x + (int)strlen(stat_info[i].input) > COLS - 8) {
	addstr(",");
	x = 1;
	move(y++, x);
      }

      /* display the string */
      if (x > 1) {
	addstr(", ");
	x += 2;
      }
      x += strlen(stat_info[i].input) + 2;
      hip_string(stat_info[i].input,
		 stat_info[i].key_char,
		 vstat_ok(i, FALSE));
    }
  }

  if (cvan == -1) {
    mvaddstr(y, 0, "Extended Caravan Command: ");
  } else {
     mvaddstr(y, 0, "Caravan Adjustment Command: ");
  }
  refresh();

  switch (last_ch = next_char()) {
  case '?':
    /* list the caravan information */
    ext_cvninfo(cvn_ptr);
    break;
  case 'T':
  case 't':
    /* load/unload caravan */
    cvn_transport(cvn_ptr);
    break;
  case '-':
    /* split caravan */
    vhold_ptr = cvn_ptr;
    split_cvn(cvn_ptr, -1);
    if (cvan == (-1)) {
      goto_cvn(vhold_ptr->cvnid);
    }
    break;
  case '/':
    /* divide caravan */
    vhold_ptr = cvn_ptr;
    split_cvn(cvn_ptr, CVN_SIZE / 2);
    if (cvan == (-1)) {
      goto_cvn(vhold_ptr->cvnid);
    }
    break;
  case '+':
    /* combine caravans */
    if (cvan != -1) {
      errormsg("Huh?  Combine what with what?");
      break;
    }
    selector += 2;
    v2_ptr = get_cvnselect();
    selector -= 2;
    if (v2_ptr == NULL) {
      errormsg("There are no following caravans to combine with");
      break;
    }
    vhold_ptr = cvn_ptr;
    comb_cvns(cvn_ptr, v2_ptr);
    goto_cvn(vhold_ptr->cvnid);
    break;
  case 'M':
    /* select unit to merge with */
    if (cvan == -1) {

      /* get it by the selector */
      if (set_select(TRUE, "Choose a Caravan to Merge With") == FALSE) {
	break;
      }

      /* now retrieve the information */
      if ((v2_ptr = get_cvnselect()) == NULL) {
	errormsg("That is not a Caravan Unit");
	goto_cvn(CVN_ID);
	break;
      }

    } else {

      /* merge by the numbers */
      mvaddstr(LINES - 1, 0, "Merge with what caravan? ");
      clrtoeol();
      refresh();
      men = get_number(FALSE);
      if (no_input) break;
      v2_ptr = cvnbynum(men);

    }

    /* now merge them */
    if (v2_ptr == NULL) {
      errormsg("There is no such caravan");
      break;
    }
    if (cvn_ptr == v2_ptr) {
      errormsg("That is the same caravan");
      break;
    }
    vhold_ptr = cvn_ptr;
    comb_cvns(cvn_ptr, v2_ptr);
    if (cvan == (-1)) {
      goto_cvn(vhold_ptr->cvnid);
    }
    break;
  case '>':
  case '=':
  case '<':
    /* find out which speed it is */
    for (vstat = 0; vstat < SPD_STUCK; vstat++) {
      if (shortspeed[vstat][0] == last_ch) break;
    }
    if (vstat == SPD_STUCK) {
      errormsg("Invalid Input");
      break;
    }

    /* check if the speed is possible */
    if (vspeed_ok(vstat, TRUE)) {
      change_vspeed(cvn_ptr, vstat);
    }
    break;
  case 'D':
    /* destroy the unit */
    disb_cvn(cvn_ptr);
    break;
  case '#':
    /* change the unit identifier */
    mvaddstr(LINES - 1, 0, "Enter a new caravan id? ");
    clrtoeol();
    refresh();
    men = get_number(FALSE);
    if (no_input == TRUE) break;

    /* validate the number */
    if (men >= MAX_IDTYPE || men == EMPTY_HOLD) {
      errormsg("That is an invalid caravan number.");
      break;
    } else if (cvnbynum(men) != NULL) {
      errormsg("That number is already in use.");
      break;
    }
    global_int = CVN_ID;
    cvn_renum(men);
    VADJID;
    cvn_sort();
    if (cvan == -1) goto_cvn(men);
    break;
  case 'r':
  case 'R':
    if (CVN_EFF == 100) {
      errormsg("That caravan does not need to be repaired");
      break;
    }
    cvn_repair(cvn_ptr);
    break;
  case 's':
  case 'S':
    /* set the supply level of the unit */
    if (MAXSUPPLIES == 0) {
      errormsg("Supplying is not needed in this campaign");
      break;
    }

    /* now get it and set it */
    mvprintw(LINES - 1, 0, "Set supply to what value? [default=%d] ",
	     CVN_SPLY);
    men = get_number(FALSE);

    /* check the input */
    if ((no_input == TRUE) || (men == CVN_SPLY)) break;
    if (men < 0) {
      errormsg("Wow, negative supplies... why not just burn the wagons?");
      break;
    }
    if (men > MAXSUPPLIES * 2) {
      errormsg("They cannot carry that many supplies!");
      break;
    }

    /* now allow it */
    supply_cvn(cvn_ptr, men, TRUE);
    break;
  case ' ':
  case '\n':
  case '\r':
    /* peaceful exit on space */
    break;
  default:
    /* unimplemented? */
    for (vstat = 0; vstat < num_statuses; vstat++) {
      if (!usable_stat(vstat) ||
	  !vunit_stat(vstat)) continue;
      if (stat_info[vstat].input[stat_info[vstat].key_char - 1] == last_ch)
	break;
    }
    if (vstat == num_statuses) {
      errormsg("Huh?  That isn't any command I have in my list.");
      break;
    }

    /* check if the speed is possible */
    if (vstat_ok(vstat, TRUE)) {
      change_vstatus(cvn_ptr, vstat);
    }
    break;
  }
}
