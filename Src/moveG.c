/* interactive routines for moving units and items */
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
#include "armyX.h"
#include "itemX.h"
#include "moveX.h"
#include "navyX.h"
#include "calenX.h"
#include "desigX.h"
#include "magicX.h"
#include "elevegX.h"
#include "statusX.h"
#include "caravanX.h"
#include "dstatusX.h"
#include "keyvalsX.h"

/* key binding definitions */
extern KBIND_STRUCT mparse_klist[];
extern PARSE_STRUCT mparse_funcs[];
KLIST_PTR mparse_bindings = NULL;

/* information structure concerning movement keybindings */
KEYSYS_STRUCT mparse_keysys = {
  "move", mparse_funcs, mparse_klist, 0, 0
};

/* variables local to this file */
static int mp_type, last_mcost, last_vegetation;
static int mp_xnew, mp_ynew, mp_done, mp_noinp;
static int mp_havemoved, mp_remaining;
static long lost_men, unit_men = 0;

/* variables used in the various movement routines */
extern int range_limit, xhome, yhome, mult_10;

/*
 * move_select - Determine the type of unit to move and initiate movement
 *
 * Identifies which type of unit (army, navy, or caravan) is currently selected
 * and initiates the movement sequence. Handles god mode authentication and
 * ensures the unit returns to its final location after movement completion.
 * This is the main entry point for interactive unit movement in the game.
 *
 * The function checks for unit availability in priority order:
 * 1. Army units (including groups and monsters)
 * 2. Naval fleets 
 * 3. Caravans (trade/supply units)
 *
 * Parameters:
 *   None (uses global unit selection state)
 *
 * Returns:
 *   MOVECOST - Movement cost incurred during the movement session
 *   0 - No movement occurred (no unit selected or movement blocked)
 *
 * Side Effects:
 *   - Calls move_parse() to handle the interactive movement interface
 *   - Modifies global unit state and location
 *   - Updates map display and recalculates hexagonal positioning
 *   - May trigger god mode authentication sequence
 *   - Sets mult_10 flag during movement for display purposes
 *
 * Notes:
 *   - Preserves the original unit selection after movement completion
 *   - God mode allows moving any nation's units with special authentication
 *   - Movement cost calculation includes terrain and unit type factors
 *   - Uses goto_* functions to return focus to moved unit
 */
int
move_select PARM_0(void)
{
  int which_style = 0;

  /* find the unit to be relocated */
  if ((army_ptr = get_armyselect()) != NULL) {
    which_style = MOVE_ARMY;
    global_long = ARMY_ID;
  } else if ((navy_ptr = get_navyselect()) != NULL) {
    which_style = MOVE_NAVY;
    global_long = NAVY_ID;
  } else if ((cvn_ptr = get_cvnselect()) != NULL) {
    which_style = MOVE_CVN;
    global_long = CVN_ID;
  } else {
    errormsg("There is no selected unit to move");
    return(0);
  }

  /* call the function which performs the movement */
  if ((is_god == TRUE) &&
      get_god(global_int, TRUE)) {
    return(0);
  }
  mult_10 = TRUE;
  move_parse(which_style);
  mult_10 = FALSE;
  if (is_god == TRUE) {
    reset_god();
  }
  hex_recalc();

  /* now return to the selected unit */
  switch (which_style) {
  case MOVE_ARMY:
    if (army_ptr != NULL) {
      goto_army(ARMY_ID);
    }
    break;
  case MOVE_NAVY:
    if (navy_ptr != NULL) {
      goto_navy(NAVY_ID);
    }
    break;
  case MOVE_CVN:
    if (cvn_ptr != NULL) {
      goto_cvn(CVN_ID);
    }
    break;
  default:
    break;
  }
  return(MOVECOST);
}

/*
 * move_stval - Convert movement cost value to string representation
 *
 * Converts a movement cost value into a formatted string for display purposes.
 * Handles both positive movement costs (as percentages) and negative error codes
 * with descriptive text. Supports both compact and verbose display formats for
 * different UI contexts (map overlay vs. status messages).
 *
 * For positive values, displays movement cost as percentage with optional
 * decimal precision. For negative values, converts movement restriction codes
 * into user-friendly error messages explaining why movement is blocked.
 *
 * Parameters:
 *   out_str - Output buffer to store the formatted string (must be allocated)
 *   mval - Movement cost value (positive) or error code (negative)
 *   cmpct - Boolean flag for compact display format (TRUE for short form)
 *
 * Returns:
 *   None (result stored in out_str buffer)
 *
 * Side Effects:
 *   - Modifies the out_str buffer with formatted text
 *   - Uses sprintf for percentage formatting
 *   - Accesses global mult_10 and conq_infomode flags
 *
 * Notes:
 *   - Compact format uses abbreviations (e.g., "tera" vs "land locked")
 *   - Error codes include: MV_LANDING, MV_GROUND, MV_WATER, MV_OFFMAP, etc.
 *   - Percentage display adjusts precision based on mult_10 global flag
 *   - Buffer overflow protection relies on caller providing adequate space
 */
void
move_stval PARM_3(char *, out_str, int, mval, int, cmpct)
{
  /* give reasoning */
  if (mval < 0) {
    /* display based on why movement is not allowed */
    switch (mval) {
    case MV_LANDING:
      if (cmpct) {
	strcpy(out_str, "15%+");
      } else {
	strcpy(out_str, "costa");
      }
      break;
    case MV_GROUND:
      strcpy(out_str, "tera");
      break;
    case MV_WATER:
      strcpy(out_str, "aqua");
      break;
    case MV_OFFMAP:
      if (cmpct) {
	strcpy(out_str, "!map");
      } else {
	strcpy(out_str, "off map");
      }
      break;
    case MV_OTHNATION:
      if (cmpct) {
	strcpy(out_str, "othr");
      } else {
	strcpy(out_str, "other");
      }
      break;
    case MV_NOTWALL:
      if (cmpct) {
	strcpy(out_str, "!wal");
      } else {
	strcpy(out_str, "not wall");
      }
      break;
    case MV_INHABITABLE:
      if (cmpct) {
	strcpy(out_str, "unhb");
      } else {
	strcpy(out_str, "uninhab");
      }
      break;
    case MV_TOOFAR:
      strcpy(out_str, "far ");
      break;
    case MV_UNMET:
      if (cmpct) {
	strcpy(out_str, "unmt");
      } else {
	strcpy(out_str, "unmet");
      }
      break;
    case MV_ACTOFWAR:
      if (cmpct) {
	strcpy(out_str, "war ");
      } else {
	strcpy(out_str, "actofwar");
      }
      break;
    case MV_HOSTILE:
    case MV_HOSTILESCOUT:
      if (cmpct) {
	strcpy(out_str, "hstl");
      } else {
	strcpy(out_str, "hostile");
      }
      break;
    case MV_NOACCESS:
      if (cmpct) {
	strcpy(out_str, "!pth");
      } else {
	strcpy(out_str, "no path");
      }
      break;
    case MV_DRAG:
      if (cmpct) {
	strcpy(out_str, "!drg");
      } else {
	strcpy(out_str, "no drag");
      }
      break;
    case MV_UNSEEN:
      if (cmpct) {
	strcpy(out_str, "!see");
      } else {
	strcpy(out_str, "unseen");
      }
      break;
    case MV_BADBRIDGE:
      if (cmpct) {
	strcpy(out_str, "!brg");
      } else {
	strcpy(out_str, "bridge out");
      }
      break;
    case MV_ONEWAY:
      if (cmpct) {
	strcpy(out_str, "blkd");
      } else {
	strcpy(out_str, "blocked");
      }
      break;
    default:
      if (cmpct) {
	strcpy(out_str, "****");
      } else {
	sprintf(out_str, "move: %*s", (conq_infomode? 0 : 2), "*");
      }
      break;
    }
   } else {
    if (cmpct) {
      if (mult_10) {
	if (mval >= 100) {
	  sprintf(out_str, "%3d%%", (mval + 9) / 10);
	} else {
	  sprintf(out_str, "%d.%d%%", mval / 10, mval % 10);
	}
      } else {
	sprintf(out_str, "%3d%%", mval);
      }
    } else if (mult_10) {
      sprintf(out_str, "move: %*d", (conq_infomode? 0 : 2),
	      (mval + 9) / 10);
    } else {
      sprintf(out_str, "move: %*d", (conq_infomode? 0 : 2), mval);
    }
  }
}

/*
 * move_init - Initialize movement interface and validate unit movement capability
 *
 * Sets up the movement interface for a specific unit type, validates the unit's
 * ability to move, and configures the display with appropriate movement information.
 * Handles complex validation including movement points, group status, trapped units,
 * and special movement modes (patrol, flying, etc.). This is the core initialization
 * function that must complete successfully before any movement can occur.
 *
 * The function performs extensive validation:
 * - Movement point availability and nomove status checking
 * - Group leadership and following relationships
 * - Hostile territory and trapped unit detection
 * - Flying unit and patrol special case handling
 * - Range limits for teleportation and people movement
 *
 * Parameters:
 *   type - Movement type constant (MOVE_ARMY, MOVE_NAVY, MOVE_CVN, MOVE_PEOPLE, MOVE_TELEPORT)
 *
 * Returns:
 *   Actual movement type to use (may differ from input for special cases)
 *   MOVE_NOMOVE - Unit cannot move (insufficient points, trapped, etc.)
 *   MOVE_CANCEL - User cancelled movement (group confirmation, etc.)
 *   MOVE_PATROL - Wall patrol mode (limited to one sector)
 *   MOVE_ONEWAY - Trapped unit with limited movement options
 *   MOVE_FLYARMY/MOVE_FLYCVN - Flying movement mode
 *
 * Side Effects:
 *   - Clears screen bottom and displays movement information
 *   - Updates global movement variables (mp_remaining, unit_men, etc.)
 *   - Modifies unit group relationships if group member is moved
 *   - Sets range limits and home coordinates for special movement types
 *   - Calls set_movepotential() to configure movement system
 *
 * Notes:
 *   - God mode bypasses most movement restrictions
 *   - Group members inherit leader status when separated
 *   - Trapped units can only return to last safe location
 *   - Flying units have special landing restrictions
 *   - Movement points displayed as decimal percentages (X.Y%)
 */
static int
move_init PARM_1 (int, type)
{
  int hold = type;
  int sown, dstat;

  /* initialize and display appropriate screen information */
  clear_bottom(0);
  sown = sct[XREAL][YREAL].owner;
  if (sown == UNOWNED) {
    dstat = DIP_NEUTRAL;
  } else {
    dstat = (world.np[sown])->dstatus[country] % dstatus_number;
  }

  switch (type) {
  case MOVE_ARMY:
    /* check for move points left */
    mp_remaining = 10 * (int) ARMY_MOVE;
    if ((is_god == FALSE) &&
	(nomove_stat(ARMY_STAT) ||
	 (ARMY_MOVE == 0) ||
	 (army_mvpts(ntn_ptr, army_ptr) == 0))) {

      /* check for a patrol */
      if (wall_patrol(country, army_ptr)) {
	hold = MOVE_PATROL;
	mvprintw( LINES - 2, 0, "Wall %s Patrol %d",
		 ainfo_list[ARMY_TYPE].name,
		 ARMY_ID );
	mvaddstr( LINES - 1, 0, "Remaining Movement: 1 sector" );
	break;
      }
      global_int = FALSE;
      return(MOVE_NOMOVE);
    }

    /* check for a grouped unit */
    if (unit_status(ARMY_STAT) == ST_GROUPED) {
      mvprintw( LINES - 3, 0, "That unit is part of group %d.",
	       ARMY_LEAD);
      mvprintw( LINES - 2, 0, "Do you really wish to move it? ");
      if (y_or_n() == FALSE) {
	global_int = TRUE;
	return(MOVE_NOMOVE);
      }
      if ((army_tptr = armybynum(ARMY_LEAD)) == NULL) {
	ARMY_LEAD = EMPTY_HOLD;
	AADJLEAD;
	set_status(ARMY_STAT, ST_DEFEND);
      } else {
	set_status(ARMY_STAT, unit_status(ARMYT_STAT));
	set_speed(ARMY_STAT, unit_speed(ARMYT_STAT));
	if (unit_flight(ARMYT_STAT)) {
	  set_flight(ARMY_STAT);
	}
	AADJSTAT;
	ARMY_LASTX = ARMYT_LASTX;
	ARMY_LASTY = ARMYT_LASTY;
	AADJOLOC;
	if (check_leader(ARMYT_ID) == FALSE) {

	  /* change the leader status; saving army_ptr */
	  ARMY_PTR a1_ptr = army_ptr;
	  army_ptr = army_tptr;
	  set_unlead(ARMY_STAT);
	  AADJSTAT;
	  army_ptr = a1_ptr;

	}
      }
      AADJSTAT;
      clear_bottom(0);
    }

    /* determine sizing */
    if (unit_leading(ARMY_STAT)) {
      unit_men = men_ingroup(ARMY_ID, FALSE);
    } else {
      unit_men = army_captsize(army_ptr, FALSE);
    }

    /* display information about unit being moved */
    if (a_isnormal(ARMY_TYPE) || a_ismonster(ARMY_TYPE)) {
      mvprintw( LINES - 2, 0, "Moving %s Unit %d", ainfo_list[ARMY_TYPE].name,
	       ARMY_ID );
    } else if (unit_leading(ARMY_STAT)) {
      mvprintw( LINES - 2, 0, "Moving Group Number %d", ARMY_ID );
    } else {
      mvprintw( LINES - 2, 0, "Moving %s [Unit %d]", ainfo_list[ARMY_TYPE].name,
	       ARMY_ID );
    }
    mvprintw( LINES - 1, 0, "Remaining Movement: %d.%d%%",
	     mp_remaining / 10, mp_remaining % 10 );

    /* check for resuming movement */
    if ((is_god == FALSE) &&
	(sown != country) &&
	(dstat >= DIP_HOSTILE) &&
	(men_ingarrison(XREAL, YREAL, unit_flight(ARMY_STAT) != 0) >
	 unit_men)) {
      if (map_within(XREAL, YREAL, ARMY_LASTX, ARMY_LASTY, 1)) {
	hold = MOVE_ONEWAY;
	xhome = ARMY_LASTX;
	yhome = ARMY_LASTY;
      } else if (!unit_flight(ARMY_STAT)) {
	errormsg("Your unit is trapped by hostile forces");
	global_int = TRUE;
	return(MOVE_NOMOVE);
      }
    }

    /* check for a flying unit */
    if (unit_flight(ARMY_STAT)) {
      hold = MOVE_FLYARMY;
      xhome = XREAL;
      yhome = YREAL;
    }
    break;
  case MOVE_NAVY:
    /* check for movement points */
    mp_remaining = 10 * (int) NAVY_MOVE;
    if ((is_god == FALSE) &&
	(nomove_stat(NAVY_STAT) ||
	 (NAVY_MOVE == 0) ||
	 (navy_mvpts(ntn_ptr, navy_ptr) == 0))) {
      global_int = FALSE;
      return(MOVE_NOMOVE);
    }
    mvprintw( LINES - 2, 0, "Moving Fleet %d", NAVY_ID );
    mvprintw( LINES - 1, 0, "Remaining Movement: %d.%d%%",
	     mp_remaining / 10, mp_remaining % 10 );
    break;
  case MOVE_CVN:
    /* check for a flying caravan */
    mp_remaining = 10 * (int) CVN_MOVE;
    if ((is_god == FALSE) &&
	(nomove_stat(CVN_STAT) ||
	 (CVN_MOVE == 0) ||
	 (cvn_mvpts(ntn_ptr, cvn_ptr) == 0))) {
      global_int = FALSE;
      return(MOVE_NOMOVE);
    }
    if (unit_flight(CVN_STAT)) {
      xhome = XREAL;
      yhome = YREAL;
      hold = MOVE_FLYCVN;
    }
    mvprintw( LINES - 2, 0, "Moving Caravan %d", CVN_ID );
    mvprintw( LINES - 1, 0, "Remaining Movement: %d.%d%%",
	     mp_remaining / 10, mp_remaining % 10 );
    break;
  case MOVE_PEOPLE:
    /* set the starting location and the limit */
    mp_remaining = 100;
    mvprintw( LINES - 2, 0, "Relocating people from sector [%d,%d]",
	     xloc_relative(XREAL), yloc_relative(YREAL) );
    if (is_god == FALSE) {
      mvaddstr( LINES - 1, 0, "Choose a sector within a range of 2" );
    } else {
      mvaddstr( LINES - 1, 0, "Choose any target sector");
    }
    xhome = XREAL;
    yhome = YREAL;
    range_limit = 2;
    break;
  case MOVE_TELEPORT:
    /* save starting location and set limit */
    range_limit = global_int;
    mp_remaining = 100;
    xhome = XREAL;
    yhome = YREAL;
    mvaddstr( LINES - 2, 0, "Teleporting the selected unit" );
    if (is_god == FALSE) {
      mvprintw( LINES - 1, 0, "Choose a sector within a range of %d",
	       range_limit );
    } else {
      mvaddstr( LINES - 1, 0, "Choose any target sector");
    }
    break;
  default:
    /* this should not take place */
    mp_remaining = 100;
    errormsg("Who blew it this time?");
    break;
  }

  /* show movement information */
  mvaddstr( LINES - 4, 0, "Movement Mode:  Hit Space when Done" );
  mvaddstr( LINES - 3, 0, "[see documentation for more information]" );

  /* send back the results */
  set_movepotential(hold);
  return(hold);
}

/*
 * move_check - Validate movement step and determine if movement should continue
 *
 * Performs comprehensive validation of a single movement step, checking movement
 * point costs, unit survival (exposure/attrition), hostile territory effects,
 * and movement termination conditions. This function is called for each step
 * during movement to ensure the move is legal and to apply movement consequences.
 *
 * The function handles complex movement mechanics:
 * - Movement point deduction and exhaustion checking
 * - Unit attrition from environmental exposure (desert, swamp, etc.)
 * - Hostile fortification and garrison blocking
 * - Flying unit archer interception and forced landings
 * - Naval landing confirmation and invasion mechanics
 * - Caravan blockage by hostile cities and walls
 *
 * Parameters:
 *   x - Target x coordinate for movement
 *   y - Target y coordinate for movement  
 *   mcost - Movement cost for this step (in movement points)
 *   type - Movement type (MOVE_ARMY, MOVE_NAVY, MOVE_CVN, etc.)
 *
 * Returns:
 *   TRUE - Movement step successful, continue movement
 *   FALSE - Movement step failed but movement can continue
 *   MOVE_NOMOVE - Movement must stop (insufficient points, blocked, landed)
 *
 * Side Effects:
 *   - Deducts movement points from mp_remaining
 *   - Updates unit last position (ARMY_LASTX, NAVY_LASTX, CVN_LASTX)
 *   - Calculates and applies unit losses from environmental exposure
 *   - Sets lost_men global for display of casualties
 *   - May force unit destruction if all men lost to attrition
 *   - Displays confirmation prompts for naval landings
 *
 * Notes:
 *   - God mode bypasses most restrictions except special confirmations
 *   - Environmental losses scaled by unit type, speed, and season
 *   - Scout and undead units have reduced exposure casualties
 *   - Flying units can be forced to land by hostile archers
 *   - Naval units require 15% movement to attempt landing
 *   - Movement can be blocked by walls, fortifications, or diplomatic status
 */
static int
move_check PARM_4 (int, x, int, y, int, mcost, int, type)
{
  int hold = TRUE, sown, dstat, dval;

  switch (type) {
  case MOVE_ONEWAY:
  case MOVE_ARMY:
  case MOVE_FLYARMY:
    /* check for the switchover */
    if (type == MOVE_ONEWAY) {
      mp_type = MOVE_ARMY;
    }

    /* check remaining movement */
    if ((mcost > 0) &&
	(mp_remaining < 1000) &&
	(mcost > mp_remaining) &&
	(((sct[ARMY_LASTX][ARMY_LASTY].altitude != ELE_WATER) &&
	  (sct[XREAL][YREAL].altitude != ELE_WATER)) ||
	 (major_desg(sct[XREAL][XREAL].designation) == MAJ_BRIDGE))) {
      sprintf(string, "Your unit does not have %d.%d%% movement left",
	      mcost / 10, mcost % 10);
      errormsg(string);
      hold = FALSE;
      break;
    }

    /* check for how many men might have been lost */
    if (!a_ismonster(ARMY_TYPE) &&
	(!a_isleader(ARMY_TYPE) ||
	 unit_leading(ARMY_STAT)) &&
	(citybyloc(ntn_ptr, ARMY_LASTX, ARMY_LASTY) == NULL)) {

      /* may lose troops while moving through the sector */
      int pval = exposure_value(ntn_ptr, &(sct[ARMY_LASTX][ARMY_LASTY]));
      pval *= (unit_speed(ARMY_STAT) + 1);
      pval /= 2;
      pval *= last_mcost;
      pval /= 1000;
      if (a_isundead(ARMY_TYPE)) pval /= 2;
      if (a_isscout(ARMY_TYPE)) pval /= 3;
      if ((type == MOVE_FLYARMY) &&
	  (SEASON(TURN) != WINTER_SEASON)) {
	/* easier flying */
	pval /= (1 + (SEASON(TURN) % 2));
      }

      /* check for any loses */
      if (pval > 0) {
	if (pval > 1) {
	  pval = rand_val(pval / 2 + 1) + pval / 2;
	}
	if (!a_isleader(ARMY_TYPE)) {
	  if (ARMY_SIZE > 1) {
	    lost_men = (pval * ARMY_SIZE) / 100;
	  } else if (rand_val(100) < pval) {
	    lost_men = 1;
	  }
	  if (lost_men > 0) {
	    last_vegetation = sct[ARMY_LASTX][ARMY_LASTY].vegetation;
	    if (lost_men >= ARMY_SIZE) {
	      hold = MOVE_NOMOVE;
	    }
	  }
	} else {
	  /* UNIMPLEMENTED -- lose of portion of men in a group */
	}
      }

    }

    /* subtract move points */
    if (is_god == FALSE) {
      if (mp_remaining > mcost) {
	mp_remaining -= mcost;
      } else {
	mp_remaining = 0;
	hold = MOVE_NOMOVE;
	break;
      }
    }

    /* keep track of movement */
    ARMY_LASTX = XREAL;
    ARMY_LASTY = YREAL;

    /* check for movement stoppage */
    if ((is_god == TRUE) ||
	(a_slippery(ARMY_TYPE) &&
	 (ARMY_SIZE < 50))) break;
    sct_ptr = &(sct[x][y]);
    sown = sct_ptr->owner;
    if ((sown == UNOWNED) ||
	(sown == country)) break;
    dval = major_desg(sct_ptr->designation);
    if (type == MOVE_FLYARMY) {
      if (((dval == MAJ_WALL) ||
	   (dval >= MAJ_TOWN)) &&
	  (men_ingarrison(x, y, TRUE) > unit_men / 10)) {

	dstat = (world.np[sown])->dstatus[country] % dstatus_number;
	if (dstat == DIP_UNMET || dstat > DIP_NEUTRAL) {
	  errormsg("Halting... hostile archers force a landing.");
	  mp_remaining = 0;
	  hold = MOVE_NOMOVE;
	}
      }
      break;
    }
      

    /* check the designation and protective troops */
    if ((dval == MAJ_WALL) ||
	(minor_desg(sct_ptr->designation, MIN_FORTIFIED) &&
	 (men_ingarrison(x, y, FALSE) >= TAKESECTOR))) {

      dstat = (world.np[sown])->dstatus[country] % dstatus_number;
      if (dstat == DIP_UNMET || dstat > DIP_NEUTRAL) {
	if (dval == MAJ_WALL) {
	  errormsg("Halting... a large wall blocks further movement.");
	} else {
	  errormsg("Halting... hostile fortification stops movement.");
	}
	mp_remaining = 0;
	hold = MOVE_NOMOVE;
      }
    }
    break;
  case MOVE_NAVY:
    /* check remaining movement */
    if ((mcost > 0) &&
	(mp_remaining < 1000) &&
	(mcost > mp_remaining)) {
      sprintf(string, "Your fleet does not have %d.%d%% move left",
	      mcost / 10, mcost % 10);
      errormsg(string);
      hold = FALSE;
      break;
    }

    if (mcost == MV_LANDING) {

      /* check for adequate move cost */
      if (mp_remaining < 150) {
	errormsg("Your fleet needs 15% movement to land");
	hold = FALSE;
	break;
      }

      /* query landing */
      mvaddstr( LINES - 1, 0, "Do you wish to ");
      sown = sct[x][y].owner;
      if (sown == UNOWNED) dstat = DIP_UNMET;
      else dstat = (world.np[sown])->dstatus[country] % dstatus_number;

      if ((sown != country) &&
	  (dstat == DIP_UNMET || dstat > DIP_ALLIED)) {
	addstr("invade? [yn]");
      } else {
	addstr("land? [yn]");
      }

      /* get the answer */
      if (y_or_n()) {
	mp_remaining = 0;
	hold = MOVE_NOMOVE;
      } else {
	hold = FALSE;
      }

    } else if (is_god == FALSE) {

      /* remove required movement points */
      if (mp_remaining > mcost) {
	mp_remaining -= mcost;
      } else {
	mp_remaining = 0;
	hold = MOVE_NOMOVE;
      }

    }
    NAVY_LASTX = XREAL;
    NAVY_LASTY = YREAL;
    break;
  case MOVE_CVN:
  case MOVE_FLYCVN:
    /* check remaining movement */
    if ((mcost > 0) &&
	(mp_remaining < 1000) &&
	(mcost > mp_remaining)) {
      sprintf(string, "Your caravan does not have %d.%d%% move left",
	      mcost / 10, mcost % 10);
      errormsg(string);
      hold = FALSE;
      break;
    }

    CVN_LASTX = XREAL;
    CVN_LASTY = YREAL;

    /* deduct cost */
    if (is_god == FALSE) {
      if (mp_remaining > mcost) {
	mp_remaining -= mcost;
      } else {
	mp_remaining = 0;
	hold = MOVE_NOMOVE;
	break;
      }
    }
    if (type == MOVE_FLYCVN) break;

    /* check for stoppage of movment */
    sct_ptr = &(sct[x][y]);
    if ((is_god == FALSE) &&
	((major_desg(sct_ptr->designation) == MAJ_WALL) ||
	 IS_CITY(sct_ptr->designation))) {

      sown = sct_ptr->owner;
      if (sown == UNOWNED || sown == country) break;
      dstat = (world.np[sown])->dstatus[country] % dstatus_number;

      /* hostile sectors stop movement */
      if (dstat == DIP_UNMET || dstat > DIP_NEUTRAL) {
      	errormsg("Halting... hostile fortification encountered");
	mp_remaining = 0;
	hold = MOVE_NOMOVE;
      }
    }
    break;
  case MOVE_PATROL:
    /* only one sector worth of movement */
    if (is_god == FALSE) {
      ARMY_LASTX = XREAL;
      ARMY_LASTY = YREAL;
      mp_remaining = 0;
      hold = MOVE_NOMOVE;
    }
    break;
  case MOVE_PEOPLE:
  case MOVE_TELEPORT:
  default:
    /* movement always allowed */
    break;
  }

  /* send back results */
  return(hold);
}

/*
 * mv_maymove - Check if movement to a sector is possible within current movement points
 *
 * Helper function used by map_loop() to test movement feasibility to adjacent sectors.
 * Calculates movement cost to the specified sector and sets global_long flag if
 * movement is both legal and affordable with remaining movement points. Used primarily
 * for flying unit landing validation to determine if the unit has escape routes.
 *
 * This function is called for each sector in a radius during movement possibility
 * checking, particularly for flying units that need to verify they can move away
 * from their current position before being allowed to land.
 *
 * Parameters:
 *   x - Target sector x coordinate to test
 *   y - Target sector y coordinate to test
 *
 * Returns:
 *   None (result stored in global_long flag)
 *
 * Side Effects:
 *   - Sets global_long to TRUE if movement is possible and affordable
 *   - Calls move_cost() which may access terrain and diplomatic data
 *   - Uses global_int as movement type parameter for cost calculation
 *
 * Notes:
 *   - Used as callback function for map_loop() iteration
 *   - Movement cost calculation considers unit type stored in global_int
 *   - Only positive movement costs within mp_remaining budget set the flag
 *   - Global_long remains FALSE if any movement restriction applies
 */
static void
mv_maymove PARM_2(int, x, int, y)
{
  int mc = move_cost(x, y, global_int);

  /* is there a nearby sector accessible? */
  if ((mc >= 0) &&
      (mc <= mp_remaining)) {
    global_long = TRUE;
  }
}

/*
 * move_space - Validate if movement can be terminated at current location
 *
 * Checks whether the current sector is a valid location to end movement for
 * the specified unit type. Different movement modes have different restrictions
 * on where they can safely terminate (e.g., flying units cannot land on water,
 * people can only move to owned sectors, teleportation has diplomatic restrictions).
 *
 * The function performs comprehensive location validation:
 * - Flying units: Cannot land on water, volcanoes (without fire magic), or mountain peaks
 * - People movement: Must end in owned, non-sieged sectors
 * - Teleportation: Cannot enter water, volcanoes, peaks, or hostile territory
 * - Regular movement: Cannot end on water without bridges
 *
 * Parameters:
 *   type - Movement type (MOVE_FLYARMY, MOVE_FLYCVN, MOVE_PEOPLE, MOVE_TELEPORT, etc.)
 *
 * Returns:
 *   TRUE - Movement can be safely terminated at current location
 *   FALSE - Current location is invalid for movement termination
 *
 * Side Effects:
 *   - Displays error messages explaining why termination is invalid
 *   - Accesses sector terrain, ownership, and designation data
 *   - Checks diplomatic status for teleportation restrictions
 *   - May call mv_maymove() to check escape routes for flying units
 *
 * Notes:
 *   - Flying units must have escape routes available before landing restrictions apply
 *   - Volcano restrictions can be bypassed with fire magic (WIZ_MAGIC(MW_FIRE))
 *   - Mountain peak restrictions are absolute except for avian flying units
 *   - Diplomatic checks prevent teleportation into unmet or hostile nations
 *   - Bridge sectors override water landing restrictions
 */
static int
move_space PARM_1( int, type )
{
  int hold = TRUE;

  sct_ptr = &(sct[XREAL][YREAL]);
  switch (type) {
  case MOVE_FLYARMY:
  case MOVE_FLYCVN:
    /* check the surround sectors for the ability to leave */
    global_long = FALSE;
    global_int = type;
    map_loop(XREAL, YREAL, 1, mv_maymove);
    if (global_long == FALSE) {
      /* no where else to go, must be able to stop */
      break;
    }
    
    /* may not end on certain types of sectors */
    if ((sct_ptr->altitude == ELE_WATER) &&
	(major_desg(sct_ptr->designation) != MAJ_BRIDGE)) {
      errormsg("We will drown if we stop over water!");
      hold = FALSE;
    } else if ((sct_ptr->vegetation == VEG_VOLCANO) &&
	       (country != UNOWNED) &&
	       !WIZ_MAGIC(MW_FIRE)) {
      errormsg("The volcano will burn us if we land");
      hold = FALSE;
    } else if (sct_ptr->altitude == ELE_PEAK &&
	       (type != MOVE_FLYARMY || !a_isavian(ARMY_TYPE))) {
      errormsg("We will be destroyed if we try to land on this mountain peak");
      hold = FALSE;
    }
    break;
  case MOVE_PEOPLE:
    /* may only stop in an owned sector */
    if (sct_ptr->owner != country) {
      errormsg("But that land is not owned by our nation");
      hold = FALSE;
    } else if (minor_desg(sct_ptr->designation, MIN_SIEGED)) {
      errormsg("Your people may not enter a sector under siege");
      hold = FALSE;
    }
    break;
  case MOVE_TELEPORT:
    /* may not stop on water sector */
    if ((sct_ptr->altitude == ELE_WATER) &&
	(major_desg(sct_ptr->designation) != MAJ_BRIDGE)) {
      errormsg("Teleportation into water is not likely to be helpful");
      hold = FALSE;
    } else if ((sct_ptr->vegetation == VEG_VOLCANO) &&
	       (country != UNOWNED) &&
	       !WIZ_MAGIC(MW_FIRE)) {
      errormsg("Teleporting into a volcano is not one of your better ideas");
      hold = FALSE;
    } else if (sct_ptr->altitude == ELE_PEAK) {
      errormsg("Teleporting onto a mountain peak will destroy your unit");
      hold = FALSE;
    } else {
      int sown, dstat;

      sown = sct_ptr->owner;
      if (sown == country || sown == UNOWNED) break;
      dstat = (world.np[sown])->dstatus[country] % dstatus_number;

      if (dstat == DIP_UNMET) {
	errormsg("Teleportation is not permitted into unmet nations");
	hold = FALSE;
      } else if ((dstat > DIP_NEUTRAL) &&
		 (ntn_ptr->dstatus[sown] % dstatus_number < DIP_HOSTILE)) {
	errormsg("Must be hostile to enter land owned by this nation");
	hold = FALSE;
      }
    }
    break;
  case MOVE_ARMY:
  case MOVE_CVN:
  case MOVE_PATROL:
    if ((sct_ptr->altitude == ELE_WATER) &&
	(major_desg(sct_ptr->designation) != MAJ_BRIDGE)) {
      errormsg("We will drown if we stop over water!");
      hold = FALSE;
    }
  case MOVE_NAVY:
  default:
    /* allowable to end movement in this sector */
    break;
  }

  /* give back the results */
  return(hold);
}

/*
 * mvbtm_hex - Display movement costs for adjacent sectors in hexagonal format
 *
 * Renders movement cost information for the six adjacent sectors in a hexagonal
 * map layout at the bottom of the screen. Calculates movement costs for each
 * direction and formats them using move_stval() for compact display. The layout
 * matches the hexagonal movement pattern with proper spacing and connecting lines.
 *
 * The display shows a visual representation of the hex grid:
 *     NW  N  NE
 *   ------+====+------
 *     SW  S  SE
 * 
 * Where the center (====) represents the current position and surrounding
 * cells show movement costs or restriction codes for each direction.
 *
 * Parameters:
 *   type - Movement type for cost calculation (MOVE_ARMY, MOVE_NAVY, etc.)
 *
 * Returns:
 *   None (displays directly to screen)
 *
 * Side Effects:
 *   - Calls move_cost() for each adjacent sector
 *   - Uses move_stval() to format cost strings in compact mode
 *   - Draws to screen at fixed bottom location (LINES-4 to LINES-2)
 *   - Uses global coordinates (xoffset, xcurs, XREAL, YREAL)
 *
 * Notes:
 *   - Hexagonal coordinate calculation adjusts for even/odd column offsets
 *   - Display uses fixed character positions for alignment
 *   - Connecting lines (/ and \) show directional relationships
 *   - Only used when world.hexmap is TRUE
 */
static void
mvbtm_hex PARM_1(int, type)
{
  int i, j, x, y, value;

  /* loop over the spectrum */
  for (j = 0; j < 3; j += 2) {
    for (i = -1; i < 2; i++) {
      x = xoffset + xcurs + i;
      if (i == 0) {
	y = YREAL - j + 1;
      } else if (j == 0) {
	y = YREAL + (XREAL % 2);
      } else {
	y = YREAL - (XREAL + 1) % 2;
      }

      /* now find and display the movecost */
      value = move_cost(x, y, type);
      move_stval(string, value, TRUE);
      mvaddstr(LINES - 4 + j, 57 + (i * 7), string);
      if (i != 1) {
	addch(' ');
	if (((i == -1) && (j == 0)) ||
	    ((i == 0) && (j == 2))) {
	  addch('\\');
	} else {
	  addch('/');
	}
      }
    }
  }
  mvaddstr(LINES - 3, 50, "------+====+------");
}

/*
 * mvbtm_rect - Display movement costs for adjacent sectors in rectangular format  
 *
 * Renders movement cost information for the eight adjacent sectors in a rectangular
 * map layout at the bottom of the screen. Calculates movement costs for each
 * cardinal and diagonal direction and formats them for compact display. The layout
 * matches the rectangular movement pattern with proper spacing and dividers.
 *
 * The display shows a 3x3 grid representation:
 *   NW  | N  | NE
 *   ----+====+----
 *   W   | == | E 
 *   ----+====+----
 *   SW  | S  | SE
 *
 * Where the center (====) represents the current position and surrounding
 * cells show movement costs or restriction codes for each direction.
 *
 * Parameters:
 *   type - Movement type for cost calculation (MOVE_ARMY, MOVE_NAVY, etc.)
 *
 * Returns:
 *   None (displays directly to screen)
 *
 * Side Effects:
 *   - Calls move_cost() for each adjacent sector
 *   - Uses move_stval() to format cost strings in compact mode
 *   - Draws to screen at fixed bottom location (LINES-4 to LINES-2)
 *   - Uses global coordinates (XREAL, YREAL)
 *
 * Notes:
 *   - Rectangular coordinate calculation is simpler than hexagonal
 *   - Display uses fixed character positions for grid alignment
 *   - Vertical bars (|) separate columns visually
 *   - Only used when world.hexmap is FALSE
 *   - Center position shows "====" as current location marker
 */
static void
mvbtm_rect PARM_1(int, type)
{
  int i, j, x, y, value;

  /* loop over the spectrum */
  for (j = 0; j < 3; j ++) {
    for (i = -1; i < 2; i++) {
      x = XREAL + i;
      y = YREAL - j + 1;

      /* now find and display the movecost */
      if ((i == 0) &&
	  (j == 1)) {
	strcpy(string, "====");
      } else {
	value = move_cost(x, y, type);
	move_stval(string, value, TRUE);
      }
      mvaddstr(LINES - 4 + j, 56 + (i * 6), string);
      if (i != 1) {
	addch(' ');
	addch('|');
      }
    }
  }
}

/*
 * move_bottom - Display comprehensive movement status at bottom of screen
 *
 * Updates the bottom portion of the screen with detailed movement information
 * including unit identification, remaining movement points, last movement cost,
 * and casualty reports. Clears the bottom area and displays unit-specific
 * information formatted for the movement interface. Also renders the movement
 * cost grid for adjacent sectors.
 *
 * The display includes:
 * - Unit identification (type, ID, group status)
 * - Movement statistics (remaining points, last move cost)
 * - Casualty reports from environmental exposure
 * - Range limitations for special movement types
 * - Movement cost grid for surrounding sectors
 *
 * Parameters:
 *   lmcost - Last movement cost incurred (for display purposes)
 *   type - Movement type determining information displayed
 *
 * Returns:
 *   None (displays directly to screen)
 *
 * Side Effects:
 *   - Clears bottom screen area (clear_bottom(0))
 *   - Displays unit information and movement statistics
 *   - Updates casualty information and reduces unit size if losses occurred
 *   - Calls mvbtm_hex() or mvbtm_rect() to show movement cost grid
 *   - Modifies global lost_men counter and ARMY_SIZE for casualties
 *
 * Notes:
 *   - Group leaders show group number instead of unit type
 *   - Flying and regular army movement show different information formats
 *   - Naval landing requires special movement point calculations
 *   - Patrol movement shows fixed "1 sector" limit
 *   - Environmental casualties permanently reduce unit size
 *   - Movement cost grid adapts to world map type (hex vs rectangular)
 */
static void
move_bottom PARM_2( int, lmcost, int, type )
{
  /* display mode information */
  clear_bottom(0);
  mvaddstr( LINES - 4, 0, "Movement Mode:  Hit Space when Done" );

  switch (type) {
  case MOVE_ARMY:
  case MOVE_FLYARMY:
    /* display unit information */
    if (!a_isleader(ARMY_TYPE)) {
      mvprintw( LINES - 2, 0, "Moving %s Unit %d",
	       ainfo_list[ARMY_TYPE].name, (int) ARMY_ID );
    } else if (unit_leading(ARMY_STAT)) {
      mvprintw( LINES - 2, 0, "Moving Group Number %d",
	       (int) ARMY_ID );
    } else {
      mvprintw( LINES - 2, 0, "Moving %s [Unit %d]",
	       ainfo_list[ARMY_TYPE].name, (int) ARMY_ID );
    }
    mvprintw( LINES - 3, 0, "Move Left: %d.%d%%  Last Move: %d.%d%%",
	     mp_remaining / 10, mp_remaining % 10,
	     lmcost / 10, lmcost % 10);
    if (lost_men > 0) {
      ARMY_SIZE -= lost_men;
      AADJSIZE;
      mvprintw( LINES - 1, 0, "%ld men didn't survive the %s",
	       lost_men, veg_info[last_vegetation].name);
      lost_men = 0;
    }
    break;
  case MOVE_PATROL:
    /* display patrol information */
    mvprintw( LINES - 2, 0, "Wall %s Patrol %d",
	     ainfo_list[ARMY_TYPE].name,
	     (int) ARMY_ID );
    mvaddstr( LINES - 3, 0, "Move Left: 1 sector" );
    break;
  case MOVE_NAVY:
    /* display fleet information */
    mvprintw( LINES - 2, 0, "Moving Fleet %d", (int) NAVY_ID );
    mvprintw( LINES - 3, 0, "Move Left: %d.%d%%  Last Move: %d.%d%%",
	     mp_remaining / 10, mp_remaining % 10,
	     lmcost / 10, lmcost % 10);
    break;
  case MOVE_CVN:
  case MOVE_FLYCVN:
    /* display caravan information */
    mvprintw( LINES - 2, 0, "Moving Caravan %d", (int) CVN_ID );
    mvprintw( LINES - 3, 0, "Move Left: %d.%d%%  Last Move: %d.%d%%",
	     mp_remaining / 10, mp_remaining % 10,
	     lmcost / 10, lmcost % 10);
    break;
  case MOVE_PEOPLE:
    /* just say to move it along */
    mvaddstr( LINES - 3, 0, "  [hit ESCAPE key to abort]");
    mvprintw( LINES - 2, 0, "Relocating people from sector [%d,%d]",
	     xloc_relative(xhome), yloc_relative(yhome) );
    mvaddstr( LINES - 1, 0, "Choose a sector within a range of 2" );
    break;
  case MOVE_TELEPORT:
    /* just say a quick blurb about teleportation */
    mvprintw( LINES - 2, 0, "Teleporting from sector %d, %d", xhome,
	     yhome );
    mvprintw( LINES - 3, 0, "Choose a sector within a range of %d",
	     range_limit );
    mvaddstr( LINES - 1, 0, "  [hit ESCAPE key to abort]");
    break;
  default:
    /* hmm, this shouldn't happen */
    errormsg("Okay, what did I miss?");
    break;
  }

  /* display the actual costs in a little doohicky */
  if (world.hexmap) {
    mvbtm_hex(type);
  } else {
    mvbtm_rect(type);
  }
}

/*
 * move_relocate - Complete final unit relocation and apply movement consequences
 *
 * Finalizes unit movement by updating unit positions, applying movement costs,
 * handling special movement mode termination (flying units landing), and
 * managing movement consequences including unit destruction from hazardous
 * landing sites. This function is called after movement is complete to
 * commit all changes and handle any post-movement effects.
 *
 * The function handles complex post-movement processing:
 * - Flying unit landing and potential destruction from hazardous terrain
 * - Movement point finalization and remaining point calculations
 * - Unit position updates and database consistency maintenance
 * - Group movement coordination for army leaders
 * - Naval transport coordination (armies and caravans on ships)
 * - Environmental casualty processing and unit survival
 *
 * Parameters:
 *   type - Movement type that was performed (MOVE_ARMY, MOVE_FLYARMY, etc.)
 *   verbal - Boolean flag controlling error message display (TRUE for messages)
 *
 * Returns:
 *   None (modifies unit state directly)
 *
 * Side Effects:
 *   - Updates unit location coordinates (ARMY_XLOC, NAVY_XLOC, CVN_XLOC)
 *   - Modifies movement points remaining (ARMY_MOVE, NAVY_MOVE, CVN_MOVE)
 *   - May destroy units landing in hazardous terrain (water, volcanoes, peaks)
 *   - Processes environmental casualties and updates unit sizes
 *   - Sorts army lists to maintain database consistency
 *   - Recursively relocates transported units (armies on ships, etc.)
 *   - Updates hex calculation display
 *
 * Notes:
 *   - Flying units automatically lose flight status unless they are avian
 *   - Hazardous landings trigger severance pay calculations for destroyed units
 *   - Group leaders move entire groups and coordinate member positions
 *   - Navy movement automatically moves transported armies and caravans
 *   - God mode prevents some movement point deductions
 *   - Unit destruction nullifies unit pointers requiring careful handling
 */
void
move_relocate PARM_2( int, type, int, verbal )
{
  /* check if movement was actually made */
  if (mp_havemoved == FALSE) {
    return;
  }

  switch (type) {
  case MOVE_FLYARMY:
    /* fly like an eagle */
    if ((is_god == FALSE) &&
	!(a_isavian(ARMY_TYPE))) {
      set_unflight( ARMY_STAT );
      AADJSTAT;
    }

    /* check for destruction -- ack! */
    sct_ptr = &(sct[XREAL][YREAL]);

    /* aqua flyer */
    if ((sct_ptr->altitude == ELE_WATER) &&
	(major_desg(sct_ptr->designation) != MAJ_BRIDGE)) {
      if (verbal) errormsg("Your unit lands in water and drowns");
      if (a_needpay(ARMY_TYPE)) {
	sprintf(string, "%ld talons in severance pay is sent to their widows",
		ainfo_list[ARMY_TYPE].tal_enlist * ARMY_SIZE);
	if (verbal) errormsg(string);
	command_cost += (ainfo_list[ARMY_TYPE].tal_enlist * ARMY_SIZE);
      }
      ADESTROY;
      dest_army( ARMY_ID );
      army_ptr = NULL;
      break;
    }

    /* fried flyer */
    if ((sct_ptr->vegetation == VEG_VOLCANO) &&
	(country != UNOWNED) &&
	!WIZ_MAGIC(MW_FIRE)) {
      if (verbal)
	errormsg("Your unit lands in a volcano and is enveloped in flames");
      if (a_needpay(ARMY_TYPE)) {
	sprintf(string, "%ld talons in severance pay is sent to their widows",
		ainfo_list[ARMY_TYPE].tal_enlist * ARMY_SIZE);
	if (verbal) errormsg(string);
	command_cost += (ainfo_list[ARMY_TYPE].tal_enlist * ARMY_SIZE);
      }
      ADESTROY;
      dest_army( ARMY_ID );
      army_ptr = NULL;
      break;
    }

    /* mountain peak demolishing */
    if (sct_ptr->altitude == ELE_PEAK && !a_isavian(ARMY_TYPE)) {
      if (verbal)
	errormsg("Your unit crashes into a mountain peak and is destroyed");
      if (a_needpay(ARMY_TYPE)) {
	sprintf(string, "%ld talons in severance pay is sent to their widows",
		ainfo_list[ARMY_TYPE].tal_enlist * ARMY_SIZE);
	if (verbal) errormsg(string);
	command_cost += (ainfo_list[ARMY_TYPE].tal_enlist * ARMY_SIZE);
      }
      ADESTROY;
      dest_army( ARMY_ID );
      army_ptr = NULL;
      break;
    }

  case MOVE_ARMY:
  case MOVE_PATROL:
    /* reduce the movement by one */
    if ((is_god == FALSE) &&
	(mp_remaining > 50)) {
      mp_remaining -= 50;
    } else {
      mp_remaining = 0;
    }
    if (lost_men > 0) {
      if (ARMY_SIZE > lost_men) {
	ARMY_SIZE -= lost_men;
	AADJSIZE;
	sprintf(string, "%ld men didn't survive the %s",
		lost_men, veg_info[last_vegetation].name);
      } else {
	sprintf(string, "You lost contact with your unit");
	ADESTROY;
	dest_army( ARMY_ID );
	army_ptr = NULL;
	break;
      }
      lost_men = 0;
      if (verbal) errormsg(string);
    }

    /* set army location and movement */
    ARMY_MOVE = (uns_char) (mp_remaining / 10);
    if (unit_leading(ARMY_STAT)) {

      /* must move entire group into sector */
      AADJOLOC;
      set_grploc(ARMY_ID, XREAL, YREAL);
      GADJLOC;
      if (is_god == FALSE) {
	set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
	GADJMOVE;
      }

    } else {

      /* move the unit itself */
      ARMY_XLOC = XREAL;
      ARMY_YLOC = YREAL;
      AADJLOC;
      AADJOLOC;
      AADJMOVE;

      /* now resort entire list -- reset near pointers */
      army_sort(FALSE);

    }
    break;

  case MOVE_FLYCVN:
    /* fly like a big eagle? */
    if (is_god == FALSE) {
      set_unflight( CVN_STAT );
      VADJSTAT;
    }

    /* check for destruction -- ack! */
    sct_ptr = &(sct[XREAL][YREAL]);

    /* aqua flyer */
    if ((sct_ptr->altitude == ELE_WATER) &&
	(major_desg(sct_ptr->designation) != MAJ_BRIDGE)) {
      if (verbal) errormsg("Your caravan lands in water and drowns");
      VDESTROY;
      dest_cvn( CVN_ID );
      cvn_ptr = NULL;
      break;
    }

    /* fried flyer */
    if ((sct_ptr->vegetation == VEG_VOLCANO) &&
	(country != UNOWNED) &&
	!WIZ_MAGIC(MW_FIRE)) {
      if (verbal)
	errormsg("Your caravan lands in a volcano and is enveloped in flames");
      VDESTROY;
      dest_cvn( CVN_ID );
      cvn_ptr = NULL;
      break;
    }

    /* mountain peak demolishing */
    if (sct_ptr->altitude == ELE_PEAK) {
      if (verbal)
	errormsg("Your caravan crashes into a mountain peak and is destroyed");
      VDESTROY; 
      dest_cvn( CVN_ID );
      cvn_ptr = NULL;
      break;
    }

  case MOVE_CVN:
    /* adjust the movement value */
    if ((is_god == FALSE) &&
	(mp_remaining > 0)) {
      if (mp_remaining > 50) {
	mp_remaining -= 50;
      } else {
	mp_remaining = 0;
      }
    }

    /* relocate the caravan and it's cargo */
    CVN_MOVE = (uns_char) (mp_remaining / 10);
    CVN_XLOC = XREAL;
    CVN_YLOC = YREAL;
    VADJMOVE;
    VADJLOC;
    VADJOLOC;
    break;

   case MOVE_NAVY:
    /* reduce the movement by one */
    if ((is_god == FALSE) &&
	(mp_remaining > 0)) {
      if (mp_remaining > 50) {
	mp_remaining -= 50;
      } else {
	mp_remaining = 0;
      }
    }

    /* relocate the navy */
    NAVY_MOVE = (uns_char) (mp_remaining / 10);
    NAVY_XLOC = XREAL;
    NAVY_YLOC = YREAL;
    NADJMOVE;
    NADJLOC;
    NADJOLOC;
    if ((army_ptr = armybynum(NAVY_ARMY)) != NULL) {
      ARMY_LASTX = XREAL;
      ARMY_LASTY = YREAL;
      move_relocate(MOVE_ARMY, verbal);
    }
    if ((cvn_ptr = cvnbynum(NAVY_CVN)) != NULL) {
      CVN_LASTX = XREAL;
      CVN_LASTY = YREAL;
      move_relocate(MOVE_CVN, verbal);
    }
    break;

  case MOVE_PEOPLE:
  case MOVE_TELEPORT:
    /* XREAL, YREAL now hold the sector of choice */
    break;

  default:
    /* this wrong */
    errormsg("Okay, now what did I miss?");
    break;
  }

  /* just make sure that the screen viewing is up to date */
  hex_recalc();
}

/*
 * mp_quit - Abort movement mode for teleportation and people relocation
 *
 * Attempts to cancel movement mode and return to normal map view. Only
 * certain movement types (MOVE_PEOPLE and MOVE_TELEPORT) can be aborted
 * safely, as these are special targeted movement modes rather than
 * continuous unit movement. Regular unit movement cannot be aborted
 * once initiated.
 *
 * When abortion is successful, the function resets the cursor position
 * to the original starting location and sets the movement completion flag.
 * This allows the user to escape from targeted movement modes if they
 * change their mind about the destination.
 *
 * Parameters:
 *   None (operates on global movement state)
 *
 * Returns:
 *   0 - Always returns 0 (standard command function return)
 *
 * Side Effects:
 *   - Resets cursor position to home coordinates (xhome, yhome)
 *   - Sets mp_done flag to terminate movement loop
 *   - Displays error message for non-abortable movement types
 *   - Uses global movement type variable (mp_type)
 *
 * Notes:
 *   - Only MOVE_PEOPLE and MOVE_TELEPORT movement can be aborted
 *   - Regular unit movement (armies, navies, caravans) cannot be canceled
 *   - Bound to ESC key in movement mode key bindings
 *   - Provides safety mechanism for targeted movement commands
 *   - Cursor position reset ensures map returns to original view
 */
static int
mp_quit PARM_0(void)
{
  /* end input for people relocation and teleport */
  if (mp_type == MOVE_PEOPLE || mp_type == MOVE_TELEPORT) {
    xcurs = xhome - xoffset;
    ycurs = yhome - yoffset;
    mp_done = TRUE;
  } else {
    errormsg("This form of movement is unable to be aborted");
  }
  return(0);
}

/*
 * mp_southwest - Move the unit one sector to the southwest
 *
 * Sets movement coordinates for one step southwest movement, adjusting
 * for hexagonal or rectangular map coordinates as appropriate. In hexagonal
 * mode, southwest movement requires special coordinate calculations based
 * on the current column's even/odd status to maintain proper hex grid
 * alignment. Sets movement input flag to indicate new movement is ready.
 *
 * Parameters:
 *   None (operates on global coordinate state)
 *
 * Returns:
 *   0 - Always returns 0 (standard movement command return)
 *
 * Side Effects:
 *   - Sets mp_xnew and mp_ynew to target coordinates
 *   - Clears mp_noinp flag to indicate movement input is ready
 *   - Uses global coordinate variables (xoffset, xcurs, XREAL, YREAL)
 *
 * Notes:
 *   - Hexagonal maps require column parity calculation for Y coordinate
 *   - Rectangular maps use simple diagonal coordinate adjustment
 *   - Movement validation occurs in calling function, not here
 *   - Part of the directional movement command suite
 */
static int
mp_southwest PARM_0(void)
{
  /* move southwest */
  if (world.hexmap) {
    mp_ynew = YREAL - (XREAL + 1) % 2;
    mp_xnew = xoffset + xcurs - 1;
  } else {
    mp_xnew = xoffset + xcurs - 1;
    mp_ynew = YREAL - 1;
  }
  mp_noinp = FALSE;
  return(0);
}

/*
 * mp_west - Move the unit one sector to the west  
 *
 * Sets movement coordinates for westward movement. In hexagonal maps,
 * direct west movement is not geometrically possible (hex grids only
 * support six-directional movement), so this generates an error message
 * unless god mode is active. In rectangular maps, west movement simply
 * decreases the X coordinate.
 *
 * Parameters:
 *   None (operates on global coordinate state)
 *
 * Returns:
 *   0 - Always returns 0 (standard movement command return)
 *
 * Side Effects:
 *   - Sets mp_xnew and mp_ynew to target coordinates (rectangular maps)
 *   - Clears mp_noinp flag if movement is valid
 *   - Displays error message for invalid hexagonal west movement
 *   - Uses global coordinate variables and world.hexmap flag
 *
 * Notes:
 *   - Hexagonal geometry prevents direct east/west movement
 *   - God mode overrides hexagonal movement restrictions
 *   - Error message explains geometric limitation to users
 *   - Rectangular maps support full eight-directional movement
 */
static int
mp_west PARM_0(void)
{
  /* move west */
  if ((world.hexmap) &&
      (is_god == FALSE)) {
    errormsg("Moving directly west is not possible in this world");
  } else {
    mp_xnew = xoffset + xcurs - 1;
    mp_ynew = YREAL;
    mp_noinp = FALSE;
  }
  return(0);
}

/* MP_SOUTH -- Move the unit one to the south */
static int
mp_south PARM_0(void)
{
  /* move south */
  mp_ynew = YREAL - 1;
  mp_xnew = xoffset + xcurs;
  mp_noinp = FALSE;
  return(0);
}

/* MP_NORTH -- Move the unit one to the north */
static int
mp_north PARM_0(void)
{
  /* move north */
  mp_xnew = xoffset + xcurs;
  mp_ynew = YREAL + 1;
  mp_noinp = FALSE;
  return(0);
}

/* MP_EAST -- Move the unit one sector to the east */
static int
mp_east PARM_0(void)
{
  /* move east */
  if ((world.hexmap) &&
      (is_god == FALSE)) {
    errormsg("It is not possible to move directly east in this world");
  } else {
    mp_xnew = xoffset + xcurs + 1;
    mp_ynew = YREAL;
    mp_noinp = FALSE;
  }
  return(0);
}

/* MP_SOUTHEAST -- Move unit one sector to the south east */
static int
mp_southeast PARM_0(void)
{
  /* move southeast */
  if (world.hexmap) {
    mp_ynew = YREAL - (XREAL + 1) % 2;
    mp_xnew = xoffset + xcurs + 1;
  } else {
    mp_xnew = xoffset + xcurs + 1;
    mp_ynew = YREAL - 1;
  }
  mp_noinp = FALSE;
  return(0);
}

/* MP_NORTHEAST -- Move unit one sector to the north east */
static int
mp_northeast PARM_0(void)
{
  /* move northeast */
  if (world.hexmap) {
    mp_ynew = YREAL + (XREAL % 2);
    mp_xnew = xoffset + xcurs + 1;
  } else {
    mp_xnew = xoffset + xcurs + 1;
    mp_ynew = YREAL + 1;
  }
  mp_noinp = FALSE;
  return(0);
}

/* MP_NORTHWEST -- Move unit one sector to the north west */
static int
mp_northwest PARM_0(void)
{
  /* move northwest */
  if (world.hexmap) {
    mp_ynew = YREAL + (XREAL % 2);
    mp_xnew = xoffset + xcurs - 1;
  } else {
    mp_ynew = YREAL + 1;
    mp_xnew = xoffset + xcurs - 1;
  }
  mp_noinp = FALSE;
  return(0);
}

/* MP_EXIT -- Indicate that movement is complete */
static int
mp_exit PARM_0(void)
{
  /* check for completion */
  mp_done = move_space(mp_type);
  return(0);
}

/* MP_HELP -- List the commands for the movement mode */
static int
mp_help PARM_0(void)
{
  /* show the help list */
  create_help("Conquer Movement Mode Command List",
	      mparse_bindings, mparse_funcs, mparse_keysys.num_parse);
  redraw = DRAW_FULL;
  return(0);
}

/* MOVE_PARSE -- relocate a unit using an interface */
void
move_parse PARM_1 (int, type)
{
  FNCI fnc_ptr;
  int hold_mvval, hold_m10, just_started = TRUE;
  int mcost, m_ind;

  /* initialize settings */
  mp_xnew = 0;
  mp_ynew = 0;
  last_mcost = 0;
  lost_men = 0;
  mp_done = FALSE;
  mp_havemoved = FALSE;

  /* check for god realignment */
  if (is_god == TRUE) {
    switch (type) {
    case MOVE_ARMY:
      if (goto_army(global_long)) return;
      redraw = DRAW_NOFRILLS;
      coffmap();
      break;
    case MOVE_NAVY:
      if (goto_navy(global_long)) return;
      redraw = DRAW_NOFRILLS;
      coffmap();
      break;
    case MOVE_CVN:
      if (goto_cvn(global_long)) return;
      redraw = DRAW_NOFRILLS;
      coffmap();
      break;
    }
  }

  /* initialize the keys */
  align_move_keys();

  /* redrawing information */
  redraw = DRAW_DONE;
  if ((mp_type = move_init(type)) == MOVE_NOMOVE) {
    if (global_int == FALSE) {
      errormsg("That unit is not able to move");
    }
    return;
  } else if (mp_type == MOVE_CANCEL) {
    return;
  }
  if (mp_type == MOVE_FLYARMY || mp_type == MOVE_FLYCVN) {
    m_ind = MOVEIND_FLY;
  } else if (mp_type == MOVE_NAVY) {
    m_ind = MOVEIND_WATER;
  } else {
    m_ind = MOVEIND_NORM;
  }
  movemode = mp_type;

  /* continue movement until complete */
  while (mp_done == FALSE) {

    /* show the bottom? */
    hold_m10 = mult_10;
    mult_10 = FALSE;
    if (just_started == FALSE) {

      /* always redraw */
      if (redraw == DRAW_FULL) {
	clear();
      }
      redraw = DRAW_NOFRILLS;
      hold_mvval = movemode;
      movemode = MOVE_NOMOVE;
      coffmap();
      see_around( XREAL, YREAL );
      makeside(TRUE, m_ind);
      movemode = hold_mvval;
      redraw = DRAW_DONE;
      move_bottom( last_mcost, mp_type );

    } else {

      /* show the various movement costs */
      makeside(TRUE, m_ind);
      if (world.hexmap) {
	mvbtm_hex(mp_type);
      } else {
	mvbtm_rect(mp_type);
      }

    }
    mult_10 = hold_m10;

    /* initialize and display */
    just_started = FALSE;
    mp_noinp = TRUE;
    show_cursor();
    refresh();

    /* now get the input */
    if ((fnc_ptr = parse_keys(mparse_bindings, FALSE)) == NULL) {

      /* not valid */
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Unknown key binding: ");
      show_str(string, FALSE);
      presskey();

    } else {

      /* perform the function */
      (*fnc_ptr)();

    }

    /* process new movement */
    if (mp_noinp == FALSE) {

      /* find the cost of movement into the next sector */
      mcost = move_cost( mp_xnew, mp_ynew, mp_type );

      /* check the movement */
      switch (mcost) {
      case MV_IMPASSABLE:
	errormsg("That sector is impassable");
	break;
      case MV_WATER:
	errormsg("Water walking hasn't been invented yet");
	break;
      case MV_INHABITABLE:
	errormsg("That sector is not habitable");
	break;
      case MV_GROUND:
	errormsg("That sector is land locked");
	break;
      case MV_TOOFAR:
	errormsg("That sector is beyond your range");
	break;
      case MV_OFFMAP:
	errormsg("That sector is out of this world");
	break;
      case MV_NOTWALL:
	errormsg("A patrol may only move along a wall");
	break;
      case MV_UNMET:
	errormsg("You may not enter nations which have not yet met you");
	break;
      case MV_HOSTILESCOUT:
	errormsg("You must be hostile to scout neutral or hostile territory");
	break;
      case MV_HOSTILE:
	errormsg("Transports may not enter lands owned by hostile nations");
	break;
      case MV_ACTOFWAR:
	errormsg("It is an act of war to move units into foreign lands");
	break;
      case MV_NOACCESS:
	errormsg("Your fleet may only leave the shore the same way it landed");
	break;
      case MV_DRAG:
	errormsg("You cannot drag your ships along the ground like that");
	break;
      case MV_OTHNATION:
	errormsg("You may not move people through someone else's land");
	break;
      case MV_UNSEEN:
	errormsg("You cannot jump into land which is not visible");
	break;
      case MV_BADBRIDGE:
	errormsg("The bridge in that sector is not functional");
	break;
      case MV_ONEWAY:
	sprintf(string, "Hostile troops block movement except into [%d,%d]",
		xloc_relative(xhome),
		yloc_relative(yhome));
	errormsg(string);
	break;
      default:
	/* check the movement results */
	if ((mp_noinp = move_check( (mp_xnew + MAPX) % MAPX,
				   mp_ynew, mcost, mp_type )) != FALSE) {

	  /* assign new location and display information */
	  xcurs = mp_xnew - xoffset;
	  ycurs = mp_ynew - yoffset;
	  mp_havemoved = TRUE;
	  last_mcost = mcost;

	}
	break;
      }

      /* check if the movement is complete */
      if (mp_noinp == MOVE_NOMOVE) {
	mp_done = TRUE;
      }

    }
  }

  /* relocate unit */
  move_relocate(mp_type, TRUE);
  redraw = DRAW_DONE;
  movemode = MOVE_NOMOVE;
}

/* MP_OPTIONS -- Quickie to enable the options for movement */
static int
mp_options PARM_0(void)
{
  option_cmd(mparse_keysys, &mparse_bindings);
  return(0);
}

/* The list of functions */
PARSE_STRUCT mparse_funcs[] = {
  {adj_display, "adjust-display",
     "Adjust the display mode used on the conquer map screen"},
  {change_view, "adjust-view",
     "Alter what characters are used to represent what items"},
  {mp_options, "conquer-options",
     "Adjust the conquer environment by changing various options"},
  {custom_display, "customize-display",
     "Tailor the display modes to tastes of the user"},
  {hl_all, "highlight-all",
     "Adjust the highlight selection of all of the slots"},
  {hl_current, "highlight-current",
     "Adjust the highlight setting of the slot at the focus"},
  {hl_horz, "highlight-horizontal",
     "Adjust the slots on the same horizontal as the focus"},
  {hl_ullr, "highlight-upleft-lowright",
     "Adjust the highlight setting of upper left and lower right slots"},
  {hl_urll, "highlight-upright-lowleft",
     "Adjust the hightight setting of upper right and lower left slots"},
  {hl_vert, "highlight-vertical",
     "Adjust the slots on the same verticle as the focus"},
  {do_ignore, "ignore-key",
     "Do nothing when a key sequence is entered"},
  {mp_east, "move-east",
     "Move-mode, move the current item one sector to the east"},
  {mp_exit, "move-exit",
     "Move-mode, end the movement at the current sector"},
  {mp_help, "move-help",
     "Move-mode, display the list of movement mode commands"},
  {mp_north, "move-north",
     "Move-mode, move the current item one sector to the north"},
  {mp_northeast, "move-northeast",
     "Move-mode, move the current item one sector to the north and east"},
  {mp_northwest, "move-northwest",
     "Move-mode, move the current item one sector to the north and west"},
  {mp_quit, "move-quit",
     "Move-mode, abort teleportation or population relocation"},
  {mp_south, "move-south",
     "Move-mode, move the current item one sector to the south"},
  {mp_southeast, "move-southeast",
     "Move-mode, move the current item one sector to the south and east"},
  {mp_southwest, "move-southwest",
     "Move-mode, move the current item one sector to the south and west"},
  {mp_west, "move-west",
     "Move-mode, move the current item one sector to the west"},
  {do_refresh, "redraw-screen",
     "Clear and redraw the display screen"},
  {recenter_screen, "recenter-map",
     "Relocate the current sector to the center of the screen"},
  {shift_focus, "shift-focus", "Shift the focus slot to the next position"},
  {shift_rfocus, "shift-focus-back",
     "Shift the focus slot to the previous position"},
  {toggle_infomode, "toggle-infomode",
     "Switch in and out of information mode on the map display"},
  {troop_listing, "troop-listing",
     "Show listing of any visible troops in sector not under your control"}
};

/* the list of keybindings */
KBIND_STRUCT mparse_klist[] = {
  { ESTR_IGN, do_ignore },
  { ESTR_ESC, mp_quit },
  { CSTR_D, change_view },
  { CSTR_L, do_refresh },
  { CSTR_R, recenter_screen },
  { "@", toggle_infomode },
  { "\t", troop_listing },
  { "=", hl_all },
  { "/", hl_urll },
  { "\\", hl_ullr },
  { "|", hl_vert },
  { "-", hl_horz },
  { ",", hl_current },
  { "(", shift_rfocus },
  { ")", shift_focus },
  { "+", shift_focus },
  { "?", mp_help },
  { " ", mp_exit },
  { "b", mp_southwest },
  { "d", adj_display },
  { "D", custom_display },
  { "h", mp_west },
  { "j", mp_south },
  { "k", mp_north },
  { "l", mp_east },
  { "n", mp_southeast },
  { "O", mp_options },
  { "Q", mp_quit },
  { "q", mp_exit },
  { "u", mp_northeast },
  { "y", mp_northwest },
  { "1", mp_southwest },
  { "2", mp_south },
  { "3", mp_southeast },
  { "4", mp_west },
  { "5", do_ignore },
  { "6", mp_east },
  { "7", mp_northwest },
  { "8", mp_north },
  { "9", mp_northeast }
};

/* ALIGN_MOVE_KEYS -- Align all of the movement keys */
void
align_move_keys PARM_0(void)
{
  /* initialize the system */
  if (mparse_bindings == NULL) {
    mparse_keysys.num_binds = (sizeof(mparse_klist)/sizeof(KBIND_STRUCT));
    mparse_keysys.num_parse = (sizeof(mparse_funcs)/sizeof(PARSE_STRUCT));
    init_keys(&mparse_bindings, mparse_klist, mparse_keysys.num_binds);
  }
}
