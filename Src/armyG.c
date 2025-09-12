/* This file contains routines allowing user manipulation of units */
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
 * THIS FILE FORMERLY extcmds.c
 *
 * additions made in tut.fi (winter 1987 and spring 1988)
 * by mta@tut.fi and esc@tut.fi
 *
 * These commands have been incorporated into the code; and
 * were totally rewritten for version 5.0 due to the extensive
 * changes to the army and global data structures.
 *                        adb@cs.bu.edu
 */
#define USE_CODES
#define PRINT_CODES
#include "dataG.h"
#include "armyX.h"
#include "cityX.h"
#include "navyX.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "statusX.h"
#include "dstatusX.h"
#include "keyvalsX.h"

/*
 * comb_armies - Merge the second army into the first army unit
 *
 * Combines two army units of the same type into a single unit, merging
 * their strength, supplies, efficiency, and other attributes. The second
 * army is destroyed after successful merging. This function performs
 * extensive validation to ensure units can be legally combined.
 *
 * Parameters:
 *   a1_ptr - Target army unit to merge into (must not be NULL)
 *   a2_ptr - Source army unit to merge from (will be destroyed, must not be NULL)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Increases size of first army by strength of second army
 *   - Destroys the second army unit completely
 *   - Adjusts supply, efficiency, spell points, and movement values
 *   - Updates group movement if units are grouped or leading
 *   - May prompt user for confirmation if not in expert mode
 *
 * Notes:
 *   - Units must be same type, same location, compatible status
 *   - Leaders and scouts cannot be combined
 *   - Flying/non-flying units cannot be mixed
 *   - Spell enhancement status must match between units
 *   - Supply and efficiency values are weighted by unit size
 *   - Movement is set to minimum of both units
 */
static void
comb_armies PARM_2 (ARMY_PTR, a1_ptr, ARMY_PTR, a2_ptr)
{
  int stat1, stat2;
  int spd1, spd2, splyval, effval;

  /* safety check */
  if (a1_ptr == NULL || a2_ptr == NULL) {
    errormsg("Uh oh, I can't find your army!");
    return;
  }
  if (a1_ptr == a2_ptr) {
    errormsg("Hmm... they seem to be combined already");
    return;
  }
  army_ptr = a1_ptr;

  /* check if the units are the same type */
  if (ARMY_TYPE != a2_ptr->unittype) {
    errormsg("Only troops of the same type may be combined");
    return;
  }

  /* check if the type is a leader */
  if (a_isleader(ARMY_TYPE)) {
    errormsg("Leaders exist only as separate units");
    return;
  }
  if (a_isscout(ARMY_TYPE)) {
    errormsg("Scouts only work alone");
    return;
  }
  if (unit_flight(ARMY_STAT) &&
      !unit_flight(a2_ptr->status)) {
    errormsg("You may not combine a landed unit into a unit that is flying");
    return;
  }

  /* check that they are in the same sector */
  if ((ARMY_XLOC != a2_ptr->xloc) || (ARMY_YLOC != a2_ptr->yloc)) {
    errormsg("The units must be in the same sector to be combined");
    return;
  }

  /* find unit statuses */
  stat1 = real_stat(army_ptr);
  stat2 = real_stat(a2_ptr);

  /* check if the stats are the same */
  if ((is_god == FALSE) &&
      (stat1 != stat2 || nolike_stat(stat1))) {

    /* check if status of the first unit prevents merging */
    if (nocomb_stat(stat1)) {
      sprintf(string, "%s %d has a status of %s, and cannot merge",
	      ainfo_list[ARMY_TYPE].name, ARMY_ID,
	      stat_info[stat1].name);
      errormsg(string);
      return;
    }

    /* check if status of the second unit prevents merging */
    if (nocomb_stat(stat2)) {
      sprintf(string, "%s %d has a status of %s, and cannot merge",
	      ainfo_list[a2_ptr->unittype].name,
	      a2_ptr->armyid, stat_info[stat2].name);
      errormsg(string);
      return;
    }

  }

  /* check spell status */
  if (unit_spelled(ARMY_STAT) != unit_spelled(a2_ptr->status)) {
    errormsg("Both units have to be magically enhanced to be combined");
    return;
  }

  /* assign speed settings */
  spd1 = unit_speed(ARMY_STAT);
  spd2 = unit_speed(a2_ptr->status);

  /* get confirmation */
  if (conq_expert == FALSE) {
    mvprintw(LINES - 1, 0, "Merge %s Unit %d into %s Unit %d? ",
	     ainfo_list[a2_ptr->unittype].name, a2_ptr->armyid,
	     ainfo_list[ARMY_TYPE].name, ARMY_ID);
    if (y_or_n() == FALSE) return;
  }

  /* now combine them */
  if ((spd1 == SPD_STUCK) ||
      ((spd1 < spd2) && (spd2 != SPD_STUCK))) {
    set_speed(ARMY_STAT, spd2);
  }
  if (unit_healed(a2_ptr->status)) {
    set_healed(ARMY_STAT);
  }

  /* check the supply levels */
  if (ARMY_SPLY != a2_ptr->supply) {
    splyval = ((int) ARMY_SPLY * ARMY_SIZE) +
      ((int) a2_ptr->supply * a2_ptr->strength);
    ARMY_SPLY = (uns_char) (splyval / (ARMY_SIZE + a2_ptr->strength));
    AADJSPLY;
  }

  /* adjust the efficiency */
  if (ARMY_MAXEFF != a2_ptr->max_eff) {
    effval = ((int) ARMY_MAXEFF * ARMY_SIZE) +
      ((int) a2_ptr->max_eff * a2_ptr->strength);
    ARMY_MAXEFF = (uns_char) (effval / (ARMY_SIZE + a2_ptr->strength));
    AADJMAXEFF;
  }
  if (ARMY_EFF != a2_ptr->efficiency) {
    effval = ((int) ARMY_EFF * ARMY_SIZE) +
      ((int) a2_ptr->efficiency * a2_ptr->strength);
    ARMY_EFF = (uns_char) (effval / (ARMY_SIZE + a2_ptr->strength));
    AADJEFF;
  }

  /* insert the second unit into the first */
  ARMY_SIZE += a2_ptr->strength;
  AADJSIZE;
  if (a2_ptr->spellpts > 0) {
    if ((int) ARMY_SPTS + (int) a2_ptr->spellpts > 200) {
      ARMY_SPTS = 200;
    } else {
      ARMY_SPTS += a2_ptr->spellpts;
    }
    AADJSPTS;
  }
  if (ARMY_MOVE > a2_ptr->umove) {
    /* reduce movement of unit */
    ARMY_MOVE = a2_ptr->umove;
  }

  /* check if it is grouped */
  if ((unit_status(ARMY_STAT) == ST_GROUPED) &&
      ((army_tptr = armybynum(ARMY_LEAD)) != NULL)) {

    /* set the movement value of a group */
    set_grpmove(ARMY_LEAD, unit_speed(ARMY_STAT), ARMY_MOVE);
    army_ptr = army_tptr;
    GADJMOVE;

  } else if (unit_leading(ARMY_STAT)) {

    /* properly adjust the movement of the group */
    set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
    GADJMOVE;

  } else {
    AADJSTAT;
    AADJMOVE;
  }

  /* nuke the second unit */
  army_ptr = a2_ptr;
  ADESTROY;
  dest_army(ARMY_ID);
  army_ptr = NULL;
}

/*
 * change_astatus - Adjust the status of an army unit to a new status
 *
 * Changes the operational status of an army unit (attack, defend, siege, etc.)
 * and handles all associated side effects including movement penalties, group
 * management, and special status transitions like sortie from siege.
 *
 * Parameters:
 *   a1_ptr - Army unit to change status for (must not be NULL)
 *   new_stat - New status value to assign to the unit
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies unit's operational status
 *   - Reduces movement points based on status type (10-25 points)
 *   - Handles ungrouping if unit was grouped
 *   - Updates group leader status if necessary
 *   - May prompt for sortie confirmation if sieged
 *   - Adjusts speed from STUCK to SLOW if appropriate
 *
 * Notes:
 *   - Sieged units can only switch to sortie, reserve, or attack
 *   - Attack status from siege automatically becomes sortie (irrevocable)
 *   - Grouped units inherit some properties from their leader
 *   - Location-dependent statuses cost more movement points
 *   - Gods bypass movement penalties
 */
static void
change_astatus PARM_2(ARMY_PTR, a1_ptr, int, new_stat)
{
  /* get the army */
  if (a1_ptr == NULL) {
    errormsg("Hmm, I can't seem to find that army anywhere");
    return;
  }

  /* find the army status */
  army_ptr = a1_ptr;

  /* special check for when sieged */
  if (real_stat(army_ptr) == ST_SIEGED) {

    /* check for going onto sortie */
    if (new_stat == ST_ATTACK) {
      bottommsg("Do you wish to launch an irrevocable sortie? ");
      if (y_or_n() == FALSE) {
	return;
      }
      new_stat = ST_SORTIE;
    }

  }

  /* location dependent statuses make armies stay in place */
  if ((is_god == FALSE) && (ARMY_MOVE != 0)) {
    if (nomove_stat(new_stat)) {
      if (ARMY_MOVE > 25) {
	ARMY_MOVE -= 25;
      } else {
	ARMY_MOVE = 0;
      }
    } else {
      if (ARMY_MOVE > 10) {
	ARMY_MOVE -= 10;
      } else {
	ARMY_MOVE = 0;
      }
    }
    if (unit_leading(ARMY_STAT)) {
      set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
      GADJMOVE;
    } else {
      AADJMOVE;
    }
  }

  /* first check if it was a grouped unit */
  if ((unit_status(ARMY_STAT) == ST_GROUPED) &&
      ((army_tptr = armybynum(ARMY_LEAD)) != NULL)) {

    /* inherit old info and automatically ungroup */
    set_status(ARMY_STAT, new_stat);
    set_speed(ARMY_STAT, unit_speed(ARMYT_STAT));
    ARMY_LASTX = ARMYT_LASTX;
    ARMY_LASTY = ARMYT_LASTY;
    AADJOLOC;

    /* is the leader still leading? */
    if (check_leader(ARMY_LEAD) == FALSE) {

      /* adjust the leader status; saving army_ptr value */
      ARMY_PTR atmp_ptr = army_ptr;
      army_ptr = army_tptr;
      set_unlead(ARMY_STAT);
      AADJSTAT;
      army_ptr = atmp_ptr;

    }

  } else {
    /* just set the status */
    set_status(ARMY_STAT, new_stat);
  }

  /* check for speed tracking */
  if ((unit_speed(ARMY_STAT) == SPD_STUCK) &&
      !nomove_stat(ARMY_STAT)) {
    set_speed(ARMY_STAT, SPD_SLOW);
  }
  AADJSTAT;
}

/*
 * change_aspeed - Adjust the movement rate of an army unit
 *
 * Changes the movement speed setting of an army unit (slow, normal, fast)
 * and handles group movement synchronization if the unit is leading a group.
 * Movement speed affects how far units can move per turn.
 *
 * Parameters:
 *   a1_ptr - Army unit to change speed for (must not be NULL)
 *   new_speed - New speed setting (SPD_SLOW, SPD_NORMAL, SPD_FAST)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies unit's movement speed setting
 *   - Reduces current movement points by 10 unless god
 *   - Updates group movement if unit is leading a group
 *   - Adjusts unit status flags for display
 *
 * Notes:
 *   - Gods bypass movement point penalties
 *   - Leading units synchronize group movement to their speed
 *   - Speed changes consume movement points to prevent exploitation
 *   - Movement penalties are waived for leaders (handled in group logic)
 */
static void
change_aspeed PARM_2(ARMY_PTR, a1_ptr, int, new_speed)
{
  /* get the army */
  if (a1_ptr == NULL) {
    errormsg("Hmm, I can't seem to find that army anywhere");
    return;
  }

  /* find the army status */
  army_ptr = a1_ptr;

  /* adjust the movement potential */
  if ((is_god == FALSE) && (ARMY_MOVE != 0)) {
    if (ARMY_MOVE > 10) {
      ARMY_MOVE -= 10;
    } else {
      ARMY_MOVE = 0;
    }
    if (!unit_leading(ARMY_STAT)) {
      AADJMOVE;
    }
  }
  set_speed(ARMY_STAT, new_speed);
  if (unit_leading(ARMY_STAT)) {
    set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
    GADJMOVE;
  } else {
    AADJSTAT;
  }
}

/*
 * reduce_army - Separate out men from the unit to create a new army
 *
 * Splits a specified number of men from an existing army unit to create
 * a new independent army unit with identical properties. The new unit
 * inherits all characteristics from the original except for size.
 *
 * Parameters:
 *   a1_ptr - Source army unit to split from (must not be NULL)
 *   men - Number of men to separate into new unit (must be > 0)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Creates a new army unit with specified size
 *   - Reduces source army size by the specified amount
 *   - Distributes spell points proportionally between units
 *   - New unit inherits location, status, supply, efficiency, leadership
 *   - Triggers army sorting to maintain list order
 *
 * Notes:
 *   - Leaders cannot be split (they are individual units)
 *   - Mercenaries require minimum 10 men in each resulting unit
 *   - Cannot split entire unit (use renumbering instead)
 *   - Units with certain statuses cannot be split
 *   - Spell points are distributed proportionally by unit size
 *   - New unit gets next available army ID number
 */
static void
reduce_army PARM_2(ARMY_PTR, a1_ptr, long, men)
{
  /* validate input */
  if ((army_ptr = a1_ptr) == NULL) {
    return;
  }
  if (men <= 0) {
    errormsg("Huh?  You want to split how many men?");
    return;
  }

  /* check if the unit is not reducable */
  if (a_isleader(ARMY_TYPE)) {
    errormsg("Eh?  Perhaps I should lop an arm off that leader for you?");
    return;
  }
  if (nosplit_stat(ARMY_STAT)) {
    errormsg("That unit is not able to separate");
    return;
  }

  /* check if there are enough men */
  if (ARMY_SIZE == men) {
    errormsg("That is the entire unit!  Why not just renumber it?");
    return;
  }
  if (ARMY_SIZE < men) {
    errormsg("There are not that many troops in the unit");
    return;
  }

  /* check mercenary limits */
  if ((is_god == FALSE) &&
      a_ismerc(ARMY_TYPE)) {
    if ((men < 10) || (ARMY_SIZE - men < 10)) {
      errormsg("The mercenaries refuse to work in such small units");
      return;
    }
  }

  /* build a new unit */
  if ((army_tptr = crt_army(ARMY_TYPE)) == NULL) {
    errormsg("Another one?!  You have too many armies already!");
    return;
  }
  if (a_castspells(ARMY_TYPE) &&
      (ARMY_SPTS > 0)) {
    ARMYT_SPTS = (ARMY_SPTS * men) / ARMY_SIZE;
    ARMY_SPTS -= ARMYT_SPTS;
    AADJSPTS;
  }
  ARMY_SIZE -= men;
  AADJSIZE;
  army_ptr = army_tptr;
  ACREATE;
  ARMY_MOVE = a1_ptr->umove;
  ARMY_LEAD = a1_ptr->leader;
  ARMY_STAT = a1_ptr->status;
  ARMY_SPLY = a1_ptr->supply;
  ARMY_XLOC = a1_ptr->xloc;
  ARMY_LASTX = a1_ptr->lastx;
  ARMY_YLOC = a1_ptr->yloc;
  ARMY_LASTY = a1_ptr->lasty;
  ARMY_EFF = a1_ptr->efficiency;
  ARMY_MAXEFF = a1_ptr->max_eff;
  ARMY_SIZE = men;
  AADJSTAT;
  AADJLEAD;
  AADJSPLY;
  AADJSIZE;
  AADJMAXEFF;
  AADJEFF;
  AADJLOC;
  AADJOLOC;
  AADJMOVE;
  if (ARMY_SPTS > 0) {
    AADJSPTS;
  }
  army_sort(FALSE);
}

/*
 * split_army - Interactive interface to separate men from a unit
 *
 * Prompts the user to specify how many men to split from an army unit
 * and then calls reduce_army to perform the actual splitting operation.
 * This is the user-facing interface for the army splitting functionality.
 *
 * Parameters:
 *   a1_ptr - Army unit to split (must not be NULL)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Prompts user for number of men to split
 *   - Calls reduce_army if valid input provided
 *   - May create new army unit if operation successful
 *
 * Notes:
 *   - User can cancel operation by providing no input
 *   - Input validation is handled by reduce_army function
 *   - Uses get_number() for user input with cancellation support
 */
static void
split_army PARM_1(ARMY_PTR, a1_ptr)
{
  long men;

  /* query amount */
  mvaddstr(LINES - 1, 0, "How many men to split from unit? ");
  clrtoeol();
  refresh();
  men = get_number(FALSE);

  /* check for valid input */
  if (no_input == TRUE) {
    return;
  }

  /* now reduce the unit */
  reduce_army(a1_ptr, men);
}

/*
 * add_group - Add an army unit to a group under a leader
 *
 * Groups an army unit under a leader for coordinated movement and command.
 * Grouped units move together and inherit certain properties from their
 * leader. This function handles both interactive and programmatic grouping.
 *
 * Parameters:
 *   a1_ptr - Army unit to add to a group (must not be NULL)
 *   use_selector - If TRUE, use graphical selector; if FALSE, prompt for leader ID
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Changes unit status to ST_GROUPED
 *   - Sets unit's leader reference to group leader
 *   - Synchronizes movement and speed with leader
 *   - Updates leader's status to indicate leadership
 *   - Adjusts group movement calculations
 *
 * Notes:
 *   - Units and leader must be in same location
 *   - Leader must be a valid leader unit type
 *   - Scouts cannot be grouped (work independently)
 *   - Flying/landed units must match leader status
 *   - Units on transports cannot join groups
 *   - Cannot group under already-grouped units
 *   - Speed is synchronized to faster unit
 */
static void
add_group PARM_2(ARMY_PTR, a1_ptr, int, use_selector)
{
  ARMY_PTR a2_ptr;
  int group, spd1, spd2, stat1, stat2;

  /* check input */
  if (a1_ptr == NULL) return;
  army_ptr = a1_ptr;

  /* check if the stat is adjustable */
  stat1 = real_stat(army_ptr);
  if (nocomb_stat(stat1)) {
    sprintf(string, "Unit %d has a status of %s and cannot be grouped",
	    ARMY_ID, stat_info[unit_status(ARMY_STAT)].name);
    errormsg(string);
    return;
  }
  if (a_isscout(ARMY_STAT)) {
    errormsg("Your scout must work independently");
    return;
  }

  /* query as to what unit will lead */
  if (use_selector) {
    /* set up the selector */
    if (set_select(TRUE, "Select a group leader for this unit") == FALSE) {
      return;
    }
    if ((a2_ptr = get_armyselect()) == NULL) {
      errormsg("You must select a leader unit to group under");
      return;
    }
    group = a2_ptr->armyid;
  } else {
    mvaddstr(LINES - 1, 0, "Group under which leader? ");
    clrtoeol();
    refresh();
    group = get_number(FALSE);
    if (no_input == TRUE) return;

    /* find the leader unit */
    if ((a2_ptr = armybynum(group)) == NULL) {
      errormsg("There is no such unit");
      return;
    }
  }

  /* now check the information */
  if (a1_ptr == a2_ptr) {
    errormsg("How can you group a unit under itself?");
    return;
  }

  /* check if it is a leader */
  if (!a_isleader(a2_ptr->unittype)) {
    errormsg("That unit is not a leader");
    return;
  }

  /* check location */
  if ((ARMY_XLOC != a2_ptr->xloc) || (ARMY_YLOC != a2_ptr->yloc)) {
    errormsg("The unit must group under a leader in the same sector");
    return;
  }
  if (!unit_flight(ARMY_STAT) &&
      unit_flight(a2_ptr->status)) {
    errormsg("You may not group a landed unit under a leader that is flying");
    return;
  }

  /* disallow grouping onto a transport */
  stat2 = unit_status(a2_ptr->status);
  if (onship_stat(stat2)) {
    errormsg("You are unable to join a group on a transport");
    return;
  }
  if (stat2 == ST_GROUPED) {
    errormsg("You may not group under a unit which is already grouped");
    return;
  }

  /* compare speeds */
  spd1 = unit_speed(ARMY_STAT);
  spd2 = unit_speed(a2_ptr->status);

  /* set the speeds */
  if ((spd1 == SPD_STUCK) ||
      ((spd1 < spd2) && (spd2 != SPD_STUCK))) {
    set_speed(ARMY_STAT, spd2);
  }

  /* now do all the fixing */
  if (a2_ptr->umove < ARMY_MOVE) {
    ARMY_MOVE = a2_ptr->umove;
    AADJMOVE;
  } else if (a2_ptr->umove > ARMY_MOVE) {
    a2_ptr->umove = ARMY_MOVE;
  }
  set_status(ARMY_STAT, ST_GROUPED);
  AADJSTAT;
  ARMY_LEAD = group;
  AADJLEAD;
  army_ptr = a2_ptr;
  set_leading(ARMY_STAT);
  AADJSTAT;
  set_grpmove(group, unit_speed(ARMY_STAT), ARMY_MOVE);
  GADJMOVE;
}

/*
 * disb_army - Disband an army unit and handle resource redistribution
 *
 * Completely disbands an army unit, returning soldiers to civilian population
 * and redistributing supplies back to the sector. This is a complex operation
 * that handles various unit types differently and manages economic effects.
 *
 * Parameters:
 *   a1_ptr - Army unit to disband (must not be NULL)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Destroys the army unit completely
 *   - Adds soldiers to sector civilian population
 *   - Returns supplies to sector resource pools
 *   - Increases mercenary pool for mercenary units
 *   - Deducts payment for units requiring pay-offs
 *   - Updates city recruiting pools where applicable
 *   - Triggers hex display recalculation
 *
 * Notes:
 *   - Leaders cannot be disbanded (refuse suicide)
 *   - Normal soldiers must disband in own territory
 *   - Requires functioning capital for organization
 *   - May prompt for confirmation unless expert mode
 *   - Units needing pay require talons for dismissal
 *   - Undead units don't contribute to population
 *   - Half-men units contribute reduced population
 *   - Supply redistribution may be lost if sector inaccessible
 */
void
disb_army PARM_1 (ARMY_PTR, a1_ptr)
{
  ITEM_STRUCT supply_items;
  long num_peop = 0;
  int cnt, limited = FALSE, xloc, yloc;

  /* verify the unit */
  if (a1_ptr == NULL) return;
  army_ptr = a1_ptr;

  /* check if the disband is possible */
  if (is_god == FALSE) {
    if (a_isleader(ARMY_TYPE)) {
      errormsg("Your leader doesn't seem to like the idea of suicide");
      return;
    }
    if (a_isnormal(ARMY_TYPE) &&
	!a_anydisband(ARMY_TYPE) &&
	(sct[ARMY_XLOC][ARMY_YLOC].owner != country)) {
      errormsg("Soldiers must disband in their own territory");
      return;
    }
    if ((city_ptr =
	 citybyloc(ntn_ptr, ntn_ptr->capx, ntn_ptr->capy)) == NULL) {
      errormsg("Your nation is to disorganized to issue disband orders");
      return;
    }
  }

  /* question the disband */
  clear_bottom(0);
  if (conq_expert == FALSE) {
    if ((is_god == FALSE) &&
	a_needpay(ARMY_TYPE)) {
      mvprintw(LINES - 4, 0, "Pay %s %d %ld talons to be quiet? ",
	       ainfo_list[ARMY_TYPE].name,
	       ARMY_ID,
	       ainfo_list[ARMY_TYPE].tal_enlist * ARMY_SIZE );
    } else {
      mvprintw(LINES - 2, 0, "Disband your %s #%d? ",
	       ainfo_list[ARMY_TYPE].name, ARMY_ID);
    }
    if (y_or_n() == FALSE) return;
  }

  /* now disband the unit */
  if (is_god == FALSE) {

    /* check for civilian population influx */
    if (a_isnormal(ARMY_TYPE) &&
	!a_anydisband(ARMY_TYPE)) {

      /* try to give back the supplies */
      if ((ARMY_SPLY > 0) &&
	  !a_freesupply(ARMY_TYPE)) {

	/* find the values */
	army_support(army_ptr, &(supply_items.mtrls[0]), ARMY_SPLY);
	limited = real_stat(army_ptr);
	if (limited == ST_SIEGED) {
	  limited = TRUE;
	} else {
	  limited = FALSE;
	}

	/* try to give them back */
	if (send_resources(ARMY_XLOC, ARMY_YLOC, &(supply_items), limited)) {
	  bottommsg("Their supplies will be lost.  Continue? ");
	  if (y_or_n() == FALSE) return;
	  ARMY_SPLY = 0;
	}

      }

      /* the people go into the populous */
      xloc = XREAL;
      yloc = YREAL;
      if (!a_isundead(ARMY_TYPE)) {
	global_int = (PMERCVAL * ARMY_SIZE) / 100;
	if (a_halfmen(ARMY_TYPE)) {
	  global_int /= 2;
	}
	if (global_int < 1) global_int = 1;
	ARMY_SIZE -= global_int;
	AADJMERC;
	xcurs = ARMY_XLOC - xoffset;
	ycurs = ARMY_YLOC - yoffset;

	/* set the population of the sector */
	if ((num_peop = ARMY_SIZE) > 0) {
	  sct[XREAL][YREAL].people += num_peop;
	  SADJPEOP;
	}

      }

      /* now give back the resources */
      if ((ARMY_SPLY > 0) &&
	  !a_freesupply(ARMY_TYPE)) {
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
      if (!a_halfmen(ARMY_TYPE) &&
	  !a_isundead(ARMY_TYPE) &&
	  (city_ptr = citybyloc(ntn_ptr, ARMY_XLOC, ARMY_YLOC)) != NULL) {
	if (CITY_PEOPLE >= 0) {
	  CITY_PEOPLE += num_peop / 2;
	  CADJPEOP;
	}
      }

    } else if (a_ismerc(ARMY_TYPE)) {
      /* direct increase of the mercenary pool */
      global_int = ARMY_SIZE;
      AADJMERC;
    }

    /* check for pay offs */
    if (a_needpay(ARMY_TYPE)) {
      global_int = MTRLS_TALONS;
      CITY_MTRLS[MTRLS_TALONS] -=
	(ainfo_list[ARMY_TYPE].tal_enlist * ARMY_SIZE);
      CITY_IMTRLS[MTRLS_TALONS] -=
	(ainfo_list[ARMY_TYPE].tal_enlist * ARMY_SIZE);
      if (CITY_IMTRLS[MTRLS_TALONS] < 0) {
	CITY_IMTRLS[MTRLS_TALONS] = 0;
      }
      CADJMTRLS(global_int);
      CADJIMTRLS(global_int);
    }
  }
  ADESTROY;
  dest_army(ARMY_ID);
  army_ptr = NULL;
  hex_recalc();
}

/*
 * supply_army - Set the supply level of an army unit
 *
 * Manages the supply level of an army unit by either taking resources from
 * the current sector to increase supplies, or redistributing excess supplies
 * back to the sector. This function handles resource calculations, validation,
 * and transaction processing for army supply management.
 *
 * Parameters:
 *   a1_ptr - Army unit to supply (must not be NULL)
 *   level - Desired supply level (0 to MAXSUPPLIES)
 *   doquery - If TRUE, prompt user for confirmation of resource costs
 *
 * Returns:
 *   TRUE if operation should be retried/held, FALSE if completed successfully
 *
 * Side Effects:
 *   - Modifies army supply level to specified value
 *   - Takes resources from sector when increasing supplies
 *   - Returns excess resources to sector when decreasing supplies
 *   - Updates cursor position and display during resource transactions
 *
 * Notes:
 *   - Units with free supply don't need this function
 *   - Must be in owned territory or have special supply access
 *   - Monsters and paid units won't give back supplies
 *   - Resource availability is checked before transaction
 *   - Supply costs vary by unit type and game configuration
 *   - Insect-type units can only get supplies from current sector
 *   - Siege status limits supply sources to current sector only
 */
int
supply_army PARM_3(ARMY_PTR, a1_ptr, int, level, int, doquery)
{
  ARMY_PTR ahold_ptr = army_ptr;
  ITEM_PTR itest_ptr;
  ITEM_STRUCT supt_cost;
  int oxloc = XREAL, oyloc = YREAL;
  int insectonly = FALSE, cnt, ok_val, diff, hold = TRUE;

  /* check input */
  if (a1_ptr == NULL) {
    errormsg("I can't find that army unit");
    return(hold);
  }
  if ((level < 0) ||
      (level > MAXSUPPLIES)) {
    errormsg("Army units may not carry that many supplies");
    return(hold);
  }

  /* check the difference */
  army_ptr = a1_ptr;
  diff = level - ARMY_SPLY;
  if (diff == 0) {
    /* already set */
    hold = FALSE;
    goto bug_out;
  }

  /* check if they really need them */
  if (a_freesupply(ARMY_TYPE)) {
    sprintf(string, "Your %s %d does not need any supplies",
	    ainfo_list[ARMY_TYPE].name, ARMY_ID);
    hold = FALSE;
    goto bug_out;
  }

  /* now allow it */
  if (is_god == FALSE) {

    /* check if they can get them anywhere */
    if (splyinsect_stat(ARMY_STAT)) {
      insectonly = TRUE;      
    }

    /* check for unwilling give backs */
    if ((diff < 0) &&
	(a_ismonster(ARMY_TYPE) ||
	 a_needpay(ARMY_TYPE))) {
      sprintf(string, "Your %s %d refuses to give back their pay",
	      ainfo_list[ARMY_TYPE].name, ARMY_ID);
      errormsg(string);
      goto bug_out;
    }

    /* get the costs */
    army_support(army_ptr, &(supt_cost.mtrls[0]), abs(diff));

    /* check if we are giving or getting */
    if (diff > 0) {

      /* check for unreachable supplying */
      if (sct[ARMY_XLOC][ARMY_YLOC].owner != country) {
	insectonly = TRUE;      
      }

      /* now check that they are available */
      if ((itest_ptr = find_resources(ARMY_XLOC, ARMY_YLOC, insectonly))
	  == NULL) {
	if (insectonly) {
	  sprintf(string, "%s %d can only get resources from that sector",
		  ainfo_list[ARMY_TYPE].name, ARMY_ID);
	} else {
	  sprintf(string, "%s %d cannot find enough available resources",
		  ainfo_list[ARMY_TYPE].name, ARMY_ID);
	}
	errormsg(string);
	goto bug_out;
      }

      /* now compare the values */
      ok_val = TRUE;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if (itest_ptr->mtrls[cnt] < supt_cost.mtrls[cnt]) {
	  sprintf(string, "There are not %ld %s for %s %d",
		  (long) supt_cost.mtrls[cnt],
		  mtrls_info[cnt].lname,
		  ainfo_list[ARMY_TYPE].name,
		  ARMY_ID);
	  errormsg(string);
	  ok_val = FALSE;
	}
      }

      /* clean up */
      if (ok_val == FALSE) {
	free(itest_ptr);
	goto bug_out;
      }

      /* query about it */
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

      /* take it away */
      xcurs = ARMY_XLOC - xoffset;
      ycurs = ARMY_YLOC - yoffset;
      TAKESTART;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if ((global_long = supt_cost.mtrls[cnt]) > 0) {
	  TAKEMTRLS(cnt);
	}
      }
      TAKEFINISH(insectonly);
      xcurs = oxloc - xoffset;
      ycurs = oyloc - yoffset;
      take_resources(ARMY_XLOC, ARMY_YLOC, &supt_cost, insectonly);
      free(itest_ptr);

    } else {

      /* otherwise, just redistribute them in */
      if (send_resources(ARMY_XLOC, ARMY_YLOC, &supt_cost, insectonly)) {
	sprintf(string, "The supplies of %s %d would be lost",
		ainfo_list[ARMY_TYPE].name, ARMY_ID);
	errormsg(string);
	goto bug_out;
      }
      xcurs = ARMY_XLOC - xoffset;
      ycurs = ARMY_YLOC - yoffset;
      GIVESTART;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if ((global_long = supt_cost.mtrls[cnt]) > 0) {
	  GIVEMTRLS(cnt);
	}
      }
      GIVEFINISH(insectonly);
      xcurs = oxloc - xoffset;
      ycurs = oyloc - yoffset;

    }

  }
  ARMY_SPLY = level;
  AADJSPLY;

  /* reset before bugging out */
  hold = FALSE;
 bug_out:
  army_ptr = ahold_ptr;
  return(hold);
}

/*
 * astat_ok - Validate if an army status change is legally allowed
 *
 * Checks whether an army unit can legally change to a specified status
 * based on current conditions, unit type, location, and diplomatic status.
 * This function enforces game rules for status transitions.
 *
 * Parameters:
 *   new_stat - Proposed new status to validate
 *   verbal - If TRUE, display error messages to user; if FALSE, silent check
 *
 * Returns:
 *   TRUE if status change is allowed, FALSE if prohibited
 *
 * Side Effects:
 *   - May display error messages if verbal is TRUE
 *   - No state changes (pure validation function)
 *
 * Notes:
 *   - Scouts cannot change status (always defend)
 *   - Covering units cannot sweep
 *   - Some statuses cannot be changed once set
 *   - Siege status requires enemy fortification
 *   - Garrison requires friendly fortification
 *   - Reserve requires fortified supply center
 *   - Ambush/engage require controlled territory
 *   - Diplomatic status affects territorial restrictions
 *   - Gods bypass most restrictions
 *   - Sieged units have limited status options
 */
static int
astat_ok PARM_2(int, new_stat, int, verbal)
{
  int old_stat, sown;
  Diplotype dstat;

  /* check the army */
  if (army_ptr == NULL) {
    if (verbal) {
      errormsg("Hmm, I can't seem to find that army anywhere");
    }
    return(FALSE);
  }

  /* find the army status */
  old_stat = real_stat(army_ptr);

  /* scouts stats must stay the same */
  if (a_isscout(ARMY_TYPE)) {
    if (verbal) {
      errormsg("Scouts may not adjust their status");
    }
    return(FALSE);
  } else if ((new_stat == ST_SWEEP) &&
	     a_coverbonus(ARMY_TYPE)) {
    if (verbal) {
      errormsg("Covering units are not capable of sweeping maneuvers");
    }
    return(FALSE);
  }

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

  /* check for unalterable status */
  if (nochange_stat(old_stat)) {

    /* special cases when sieged */
    if (old_stat == ST_SIEGED) {

      /* check for going onto sortie */
      if (new_stat == ST_ATTACK) {
	/* attack status possible */
	return(TRUE);
      }

      /* may switch out for sortie or rule */
      if ((new_stat != ST_SORTIE) && (new_stat != ST_RESERVE)) {
	if (verbal) {
	  errormsg("A besieged unit may only sortie or go on reserve");
	}
	return(FALSE);
      }

    } else {

      /* the status may not be adjusted */
      if (verbal) {
	sprintf(string, "%s %d has a status of %s and cannot be adjusted",
		ainfo_list[ARMY_TYPE].name, ARMY_ID,
		stat_info[old_stat].name );
	errormsg(string);
      }
      return(FALSE);

    }
  }

  /* quick referencing */
  sct_ptr = &(sct[ARMY_XLOC][ARMY_YLOC]);
  sown = sct_ptr->owner;
  if ((ntn_tptr = world.np[sown]) == NULL) {
    sown = UNOWNED;
    dstat = DIP_NEUTRAL;
  } else {
    dstat = ntn_tptr->dstatus[country] % dstatus_number;
  }

  /* check garrision possibilities */
  if (new_stat == ST_GARRISON) {
    if ((sown == UNOWNED) ||
	((sown != country) && (dstat != DIP_ALLIED)) ||
	(fort_val(ARMY_XLOC, ARMY_YLOC) == 0)) {
      if (verbal) {
	errormsg("We don't have a fortification there to garrison");
      }
      return(FALSE);
    }
  } else if (new_stat == ST_SIEGE) {
    if ((sown == country) ||
	((ntn_ptr->dstatus[sown]) % dstatus_number < DIP_HOSTILE) ||
	(fort_val(ARMY_XLOC, ARMY_YLOC) == 0)) {
      if (verbal) {
	errormsg("You want to lay siege to this sector??");
      }
      return(FALSE);
    }
  } else if (new_stat == ST_RESERVE) {
    if ((sown == UNOWNED) ||
	((sown != country) && (dstat != DIP_ALLIED)) ||
	(fort_val(ARMY_XLOC, ARMY_YLOC) == 0) ||
	(major_desg(sct_ptr->designation) < MAJ_STOCKADE)) {
      if (verbal) {
	errormsg("Reserves are only possible in fortified supply centers");
      }
      return(FALSE);
    }
  } else if ((new_stat == ST_AMBUSH) ||
	     (new_stat == ST_ENGAGE)) {
    if ((sown != country) && (dstat != DIP_ALLIED)) {
      if (verbal) {
	if (new_stat == ST_AMBUSH) {
	  errormsg("You may only set ambushes in territory you control");
	} else {
	  errormsg("You may only engage within territory you control");
	}
      }
      return(FALSE);
    }
  }
  return(TRUE);
}

/*
 * aspeed_ok - Validate if an army speed change is legally allowed
 *
 * Checks whether an army unit can legally change to a specified movement
 * speed based on current movement points and unit status. Prevents
 * speed manipulation exploits while allowing legitimate speed changes.
 *
 * Parameters:
 *   new_speed - Proposed new speed setting to validate
 *   verbal - If TRUE, display error messages to user; if FALSE, silent check
 *
 * Returns:
 *   TRUE if speed change is allowed, FALSE if prohibited
 *
 * Side Effects:
 *   - May display error messages if verbal is TRUE
 *   - No state changes (pure validation function)
 *
 * Notes:
 *   - Units with nomove status cannot change speed
 *   - Cannot reduce speed if unit has moved too far (< 25 movement)
 *   - Cannot set speed to same as current speed
 *   - Validation prevents speed-change exploits
 *   - Movement restrictions ensure tactical realism
 */
static int
aspeed_ok PARM_2(int, new_speed, int, verbal)
{
  /* check the army */
  if (army_ptr == NULL) {
    if (verbal) {
      errormsg("Hmm, I can't seem to find that army anywhere");
    }
    return(FALSE);
  }

  /* is it a movable unit? */
  if (nomove_stat(ARMY_STAT)) {
    if (verbal) {
      errormsg("That unit is stopped, its speed is not adjustable");
    }
    return(FALSE);
  }
  if ((ARMY_MOVE < 25) &&
      (unit_speed(ARMY_STAT) > new_speed)) {
    if (verbal) {
      errormsg("The unit has gone too far to reduce speed");
    }
    return(FALSE);
  }
  if (unit_speed(ARMY_STAT) == new_speed) {
    if (verbal) {
      errormsg("The unit is already moving at that speed");
    }
    return(FALSE);
  }
  return(TRUE);
}

/*
 * ext_armyinfo - Display comprehensive information about an army unit
 *
 * Shows detailed information about an army unit including type, size, strength,
 * status, location, traits, and supply information. This provides players with
 * complete unit details for strategic planning and unit management.
 *
 * Parameters:
 *   a1_ptr - Army unit to display information for (must not be NULL)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Clears bottom screen area and displays unit information
 *   - Waits for user keypress before returning
 *   - No permanent state changes
 *
 * Notes:
 *   - Displays 4 lines of detailed unit information
 *   - Line 1: Unit type, size, strength, and spell power
 *   - Line 2: Status, speed, movement remaining, and movement ability
 *   - Line 3: Health, location, and special traits (leading, spelled, healing)
 *   - Line 4: Supply level and per-turn supply costs
 *   - Monster strength calculated differently (size * min strength)
 *   - Supply information only shown for units that need supplies
 *   - Coordinates shown in relative format for player reference
 */
static void
ext_armyinfo PARM_1 (ARMY_PTR, a1_ptr)
{
  ITEM_STRUCT supt_cost;
  int i;
  long str_val;

  /* check input */
  if (a1_ptr == NULL) return;
  army_ptr = a1_ptr;

  /* just provide the information about the unit */
  clear_bottom(0);
  move(LINES - 5, 0);

  /* give the unit description */
  if (a_isleader(ARMY_TYPE)) {
    addstr("Leader");
  } else if (a_ismonster(ARMY_TYPE)) {
    addstr("Monster");
  } else {
    addstr("Army");
  }
  printw(" Unit %d ==>", ARMY_ID);
  printw(" Type: %s;", ainfo_list[ARMY_TYPE].name);
  if (!a_isleader(ARMY_TYPE)) {
    printw(" Size: %ld;", ARMY_SIZE);
  }
  if (a_ismonster(ARMY_TYPE)) {
    str_val = ARMY_SIZE * ainfo_list[ARMY_TYPE].minsth;
  } else {
    str_val = ARMY_SIZE;
  }
  printw(" Strength: %ld %s;", str_val, (str_val == 1)?"man":"men");
  if (a_isleader(ARMY_TYPE) ||
      a_castspells(ARMY_TYPE)) {
    printw(" Power: %d;", ARMY_SPTS);
  }

  /* line two */
  move(LINES - 4, 0);
  printw("  Status: %s;", stat_info[unit_status(ARMY_STAT)].name);
  if (unit_flight(ARMY_STAT)) {
    sprintf(string, "Fly %s",  speedname[unit_speed(ARMY_STAT)]);
  } else {
    strcpy(string, speedname[unit_speed(ARMY_STAT)]);
  }
  printw(" Speed: %s;", string);
  printw(" Move Left: %d%%;", (int) ARMY_MOVE);
  printw(" Move Ability: %d;", army_mvpts(ntn_ptr, army_ptr));

  /* line three */
  move(LINES - 3, 0);
  printw("  Health: %d%%;", (int)ARMY_EFF);
  printw(" Location: [%d,%d];",
	 xloc_relative(ARMY_XLOC),
	 yloc_relative(ARMY_YLOC));
  addstr(" Traits: ");
  i = 0;
  if (unit_leading(ARMY_STAT)) {
    i = 1;
    addstr("leading");
  }
  if (unit_spelled(ARMY_STAT)) {
    if (i == 1) addstr(", ");
    i = 1;
    addstr("spelled");
  }
  if (unit_healed(ARMY_STAT)) {
    if (i == 1) addstr(", ");
    i = 1;
    addstr("healing");
  }
  if (i == 0) {
    addstr("none");
  }
  addstr(";");

  /* line four */
  move(LINES - 2, 0);
  if (a_freesupply(ARMY_TYPE)) {
    printw("  Supply Level: free; Supply Cost: none;");
  } else {
    printw("  Supply Level: %d turns;", (int) ARMY_SPLY);
    addstr(" Supply Cost: ");
    army_support(army_ptr, &(supt_cost.mtrls[0]), 1);
    str_val = 0;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (supt_cost.mtrls[i] > 0) {
	if (str_val == 1) addstr(", ");
	str_val = 1;
	printw("%.0f %s", (double) supt_cost.mtrls[i],
	       mtrls_info[i].lname);
      }
    }
    addstr(" per turn;");
  }

  /* done */
  presskey();
}

/*
 * ext_armycmd - Main interface for extended army unit operations
 *
 * Comprehensive command interface for army unit manipulation including
 * combining, splitting, grouping, status changes, speed adjustments,
 * supply management, and detailed unit information. This is the primary
 * army management interface in the game.
 *
 * Parameters:
 *   armie - Army unit ID to operate on, or -1 to use unit selector
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - May modify army units in various ways based on user command
 *   - Updates display and prompts user for input
 *   - Can create, destroy, or modify army units
 *   - May trigger army list resorting and display updates
 *
 * Notes:
 *   - If armie is -1, uses graphical unit selector
 *   - Displays all available commands with highlighting for valid options
 *   - Commands include: info (?), combine (+), merge (M), split (-/), 
 *     ungroup (U), group (G), renumber (#), speed changes (<>=), 
 *     disband (D), supply (S), leader adjustment (!), and status changes
 *   - Command availability depends on unit type and current status
 *   - Speed commands shown with abbreviated names (< > =)
 *   - Status commands displayed with highlighting for valid statuses
 *   - Expert mode skips some confirmations
 *   - Navigation automatically follows unit after operations when appropriate
 */
void
ext_armycmd PARM_1 (int, armie)
{
  ARMY_PTR ahold_ptr, a2_ptr;
  static int last_ch;
  int i, x, y;
  int astat, men = 0;

  /* find target army unit */
  if (armie == -1) {
    army_ptr = get_armyselect();
    if ((country == UNOWNED) &&
	get_god(global_int, TRUE)) {
      return;
    }
    if (army_ptr != NULL) {
      goto_army(ARMY_ID);
    }
  } else {
    army_ptr = armybynum(armie);
  }
  if (army_ptr == NULL) {
    errormsg("I could not find that army unit");
    return;
  }

  /* Display first line of possible adjustment commands */
  clear_bottom(0);
  y = LINES - 5;
  x = 1;
  move(y++, x);
  hip_string("? Info, ", 1, TRUE);
  hip_string("+ Combine, ", 1, !nocomb_stat(ARMY_STAT) &&
	     !a_isleader(ARMY_TYPE) && (armie == -1));
  hip_string("Merge, ", 1, !nocomb_stat(ARMY_STAT) &&
	     !a_isleader(ARMY_TYPE));
  hip_string("- Split, ", 1, !a_isleader(ARMY_TYPE));
  hip_string("/ Split 1/2, ", 1, !a_isleader(ARMY_TYPE));
  hip_string("Ungroup, ", 1, (unit_status(ARMY_STAT) == ST_GROUPED));
  hip_string("Group, ", 1, TRUE);

  /* next line of commands */
  move(y++, x);
  hip_string("# Renum, ", 1, TRUE);
  for (i = 0; i < SPD_STUCK; i++) {
    sprintf(string, "%s %s, ", shortspeed[i], speedname[i]);
    hip_string(string, 1, aspeed_ok(i, FALSE));
  }
  hip_string("Disband, ", 1, !a_isleader(ARMY_TYPE));
  hip_string("Supply, ", 1, !a_freesupply(ARMY_TYPE) &&
	     (MAXSUPPLIES > 0));
  hip_string("! Adj Ldr, ", 1, !a_isleader(ARMY_TYPE) &&
	     (unit_status(ARMY_STAT) != ST_GROUPED));

  /* now for the statuses */
  move(y++, x);
  for (i = 0; i < num_statuses; i++) {
    if (usable_stat(i) &&
	aunit_stat(i)) {

      /* check for overruns */
      if (x + (int)strlen(stat_info[i].input) > COLS - 8) {
	addstr(",");
	x = 1;
	move(y++, x);
      }

      /* display the string */
      if (x != 1) {
	addstr(", ");
	x += 2;
      }
      x += strlen(stat_info[i].input) + 2;
      hip_string(stat_info[i].input,
		 stat_info[i].key_char,
		 astat_ok(i, FALSE));

    }
  }

  /* now give the prompt */
  if (armie == -1) {
    mvaddstr(y, 0, "Extended Army Command: ");
  } else {
    mvaddstr(y, 0, "Army Adjustment Command: ");
  }
  refresh();

  switch (last_ch = next_char()) {
  case '?':
    /* the army information function */
    ext_armyinfo(army_ptr);
    break;
  case '!':
    /* check if the troop is being lead */
    if (unit_status(ARMY_STAT) == ST_GROUPED) {
      errormsg("A grouped unit must be lead by its group leader!");
      break;
    }

    /* find out who should lead */
    clear_bottom(0);
    mvaddstr(LINES - 1, 0, "Which leader will this unit follow? ");
    clrtoeol();
    men = get_number(FALSE);
    if (no_input != TRUE) {

      /* validate the leader number */
      if ((army_tptr = armybynum(men)) == NULL) {
	errormsg("There is no such unit");
      } else if (!a_isleader(ARMYT_TYPE)) {
	errormsg("That unit is not a leader!");
      } else {
	ARMY_LEAD = men;
	AADJLEAD;
      }

    }
    break;
  case '-':
    /* split army */
    ahold_ptr = army_ptr;
    split_army(army_ptr);
    if (armie == (-1)) {
      goto_army(ahold_ptr->armyid);
    }
    break;
  case '/':
    /* divide army */
    men = ARMY_SIZE / 2;
    ahold_ptr = army_ptr;
    reduce_army(army_ptr, men);
    if (armie == (-1)) {
      goto_army(ahold_ptr->armyid);
    }
    break;
  case '+':
    /* combine armies */
    if (armie != -1) {
      errormsg("Huh?  I have no idea what you are talking about.");
      break;
    }
    selector += 2;
    a2_ptr = get_armyselect();
    selector -= 2;
    if (a2_ptr == NULL) {
      errormsg("There are no following army units to combine with");
      break;
    }
    ahold_ptr = army_ptr;
    comb_armies(army_ptr, a2_ptr);
    if (armie == (-1)) {
      goto_army(ahold_ptr->armyid);
    }
    break;
  case 'M':
    /* check the basics */
    if (a_isleader(ARMY_TYPE)) {
      errormsg("Leaders exist only as separate units");
      break;
    }
    if (a_isscout(ARMY_TYPE)) {
      errormsg("Scouts only work alone");
      break;
    }

    /* select a unit to merge into */
    if (armie == (-1)) {

      /* set the selector properly */
      if (set_select(TRUE, "Choose an Army Unit to Merge With") == FALSE) {
	break;
      }

      /* now retrieve the information */
      if ((a2_ptr = get_armyselect()) == NULL) {
	errormsg("That is not an Army Unit");
	goto_army(ARMY_ID);
	break;
      }

    } else {

      /* ask for the new unit */
      mvaddstr(LINES - 1, 0, "Merge with what Army Unit number? ");
      clrtoeol();
      refresh();

      /* now get it */
      men = get_number(FALSE);
      if (no_input) break;
      a2_ptr = armybynum(men);

    }

    /* verify it */
    if (a2_ptr == NULL) {
      errormsg("There is no such unit");
      break;
    }
    if (army_ptr == a2_ptr) {
      errormsg("That is the same unit");
      break;
    }
    ahold_ptr = army_ptr;
    comb_armies(army_ptr, a2_ptr);
    if (armie == (-1)) {
      goto_army(ahold_ptr->armyid);
    }
    break;
  case '>':
  case '<':
  case '=':
    /* find which speed it is */
    for (astat = 0; astat < SPD_STUCK; astat++) {
      if (shortspeed[astat][0] == last_ch) break;
    }
    if (astat == SPD_STUCK) {
      errormsg("Invalid Input");
      break;
    }

    /* check if the speed is possible */
    if (aspeed_ok(astat, TRUE)) {
      change_aspeed(army_ptr, astat);
    }
    break;
  case '#':
    /* change the unit identifier */
    mvaddstr(LINES - 1, 0, "Enter a new unit id? ");
    clrtoeol();
    refresh();
    men = get_number(FALSE);
    if (no_input == TRUE) break;

    /* validate the number */
    if (men >= MAX_IDTYPE || men == EMPTY_HOLD) {
      errormsg("That is an invalid unit number.");
      break;
    } else if (armybynum(men) != NULL) {
      errormsg("That number is already in use.");
      break;
    }
    global_int = ARMY_ID;
    army_renum(men);
    AADJID;
    army_sort(FALSE);
    if (armie == -1) goto_army(men);
    break;
  case 'D':
    /* disband a unit */
    disb_army(army_ptr);
    break;
  case 'U':
    /* ungroup a unit */
    if (unit_status(ARMY_STAT) != ST_GROUPED) {
      errormsg("That unit is not in a group");
      break;
    }

    /* now be very sure that it is not on a ship */
    army_tptr = armybynum(ARMY_LEAD);
    if (army_tptr == NULL) {
      set_status(ARMY_STAT, ST_DEFEND);
    } else {
      /* get the real status */
      if (onship_stat(men = group_stat(ARMY_LEAD))) {
	errormsg("Troops must be unloaded from the transport!");
	break;
      }
      set_status(ARMY_STAT, men);
      if (unit_flight(ARMYT_STAT)) {
	set_flight(ARMY_STAT);
      }
      if ((is_god == FALSE) &&
	  (ARMY_MOVE != 0)) {
	if (ARMY_MOVE > 10) {
	  ARMY_MOVE -= 10;
	} else {
	  ARMY_MOVE = 0;
	}
	if (unit_leading(ARMY_STAT)) {
	  set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
	  GADJMOVE;
	} else {
	  AADJMOVE;
	}
      }

      /* preserve relocation information */
      ARMY_LASTX = ARMYT_LASTX;
      ARMY_LASTY = ARMYT_LASTY;
      AADJOLOC;

      /* check if the leader is still leading */
      if (check_leader(ARMY_LEAD) == FALSE) {

	/* adjust the leader status; saving army_ptr value */
	a2_ptr = army_ptr;
	army_ptr = army_tptr;
	set_unlead(ARMY_STAT);
	AADJSTAT;
	army_ptr = a2_ptr;

      }
    }
    AADJSTAT;
    break;
  case 'G':
    add_group(army_ptr, (armie == -1));
    break;
  case 'S':
  case 's':
    /* set the supply level of the unit */
    if (MAXSUPPLIES == 0) {
      errormsg("Supply rules are not in effect for this campaign");
      break;
    }
    if (a_freesupply(ARMY_TYPE)) {
      errormsg("That unit does not need any supplies");
      break;
    }

    /* get the value */
    mvprintw(LINES - 1, 0, "Set supply to what value? [default=%d] ",
	     ARMY_SPLY);
    men = get_number(FALSE);

    /* check the input */
    if ((no_input == TRUE) || (men == ARMY_SPLY)) break;
    if (men < 0) {
      errormsg("Wow, negative supplies... why not just kill 'em?");
      break;
    }
    if (men > MAXSUPPLIES) {
      errormsg("They cannot carry that many supplies!");
      break;
    }
    supply_army(army_ptr, men, TRUE);
    break;
  case ' ':
  case '\n':
  case '\r':
    /* peaceful exit on space */
    break;
  default:
    /* army status? */
    for (astat = 0; astat < num_statuses; astat++) {
      if (!usable_stat(astat) ||
	  !aunit_stat(astat)) continue;
      if (stat_info[astat].input[stat_info[astat].key_char - 1] == last_ch)
	break;
    }
    if (astat == num_statuses) {
      errormsg("Huh?  That isn't any command I have in my list.");
      break;
    }

    /* check if the speed is possible */
    if (astat_ok(astat, TRUE)) {
      change_astatus(army_ptr, astat);
    }
    break;
  }
}

/*
 * aunit_numbering - Interface to the army unit automatic numbering system
 *
 * Provides a comprehensive interface for managing automatic army unit ID
 * numbering rules. Allows setting default numbers, class-specific numbers,
 * and unit-type-specific numbers to control how new units are numbered
 * when created. Supports both global defaults and nation-specific schemes.
 *
 * Parameters:
 *   None (void parameter list)
 *
 * Returns:
 *   0 on successful completion
 *
 * Side Effects:
 *   - May modify global or nation-specific numbering schemes
 *   - Updates display with current numbering rules
 *   - Can trigger complete renumbering of existing units
 *   - Modifies persistent game configuration
 *
 * Notes:
 *   - Works for both god (global defaults) and nations (specific schemes)
 *   - Displays paginated list of current numbering rules
 *   - Commands: S)et default, A)rmy class, U)nittype, D)efaults, 
 *     R)enumber all, C)lear, Q)uit
 *   - Numbering priority: specific unit type > army class > default
 *   - Nations inherit global defaults if no specific scheme set
 *   - Renumbering applies new scheme to all existing units
 *   - Supports three numbering rule types: default (0), class (1), unit (2)
 *   - Range validation ensures unit numbers stay within valid limits
 *   - Changes are persistent and affect future unit creation
 */
int
aunit_numbering PARM_0(void)
{
  UNUM_PTR trace_ptr = NULL, page_ptr = NULL;
  char title[LINELTH];
  int x, ltitle, count, forw_page = FALSE, done = FALSE;
  int idnum;

  /* get god's identity */
  if (is_god == TRUE) {
    get_god(-1, FALSE);
  }
  redraw = DRAW_FULL;
  if (ntn_ptr == NULL) {
    strcpy(title, "World Default Army Numbering");
  } else {
    sprintf(title, "Nation %s Army Numbering", ntn_ptr->name);
  }
  ltitle = strlen(title);

  /* keep going until an exit is requested */
  do {

    /* clear appropriately */
    if (redraw == DRAW_FULL) {
      do_redraw();
      redraw = DRAW_DONE;
    } else {
      move(0,0);
      clrtobot();
    }

    /* provide a header */
    standout();
    mvaddstr(1, (COLS - ltitle) / 2, title);
    standend();

    /* align the page pointer */
    if (page_ptr == NULL) {
      if (ntn_ptr == NULL) {
	page_ptr = world.dflt_unum;
      } else {
	page_ptr = ntn_ptr->unum_list;
      }
    }
    count = 3;
    if (forw_page == TRUE) {
      if (trace_ptr == NULL) {
	if (ntn_ptr == NULL) {
	  page_ptr = world.dflt_unum;
	} else {
	  page_ptr = ntn_ptr->unum_list;
	}
      } else {
	page_ptr = trace_ptr;
	mvaddstr(count++, 5, "...more...");
      }
    }

    /* show a page worth of the listings */
    for (trace_ptr = page_ptr;
	 trace_ptr != NULL;
	 trace_ptr = trace_ptr->next) {

      /* check for boundary */
      if (count >= LINES - 7) {
	mvaddstr(count, 5, "...more...");
	break;
      }

      /* display the item */
      switch (trace_ptr->type) {
      case 0:
	mvprintw(count, 3, "Default numbering begins at %d",
		 trace_ptr->number);
	break;
      case 1:
	mvprintw(count, 3, "Armies of Class %s start at %d",
		 ainfo_clist[trace_ptr->ident],
		 trace_ptr->number);
	break;
      case 2:
	mvprintw(count, 3, "%s Armies start at %d",
		 ainfo_list[trace_ptr->ident].name,
		 trace_ptr->number);
	break;
      }

      /* continue */
      count++;

    }

    /* check for empty list */
    if (count == 3) {
      standout();
      if (ntn_ptr == NULL) {
	mvaddstr(count, 3, "No entries:  Global Default Number ==> 1");
      } else {
	mvaddstr(count, 3, "-- global default numbering in use --");
      }
      standend();
    }

    /* give a prompt listing possible commands */
    move(LINES - 4, (COLS - 60) / 2);
    hip_string("Set Default Number, ", 1, TRUE);
    hip_string("Army Class Number, ", 1, TRUE);
    hip_string("Unittype Number", 1, TRUE);
    move(LINES - 3, (COLS - 49) / 2);
    hip_string("Defaults, ", 1, TRUE);
    hip_string("Renumber ALL Units, ", 1, (ntn_ptr != NULL));
    hip_string("Clear Numbering", 1, TRUE);
    move(LINES - 2, (COLS - 27) / 2);
    hip_string("? for Help or ", 1, TRUE);
    hip_string("Q to Exit", 1, TRUE);
    refresh();

    /* get the selection */
    switch (next_char()) {
    case CNTRL_L:
    case CNTRL_R:
      /* full redraw */
      redraw = DRAW_FULL;
      break;
    case 'Q':
    case 'q':
      done = TRUE;
      break;
    case ' ':
    case '\n':
    case '\r':
      forw_page = TRUE;
      break;
    case '?':
      clear_bottom(0);
      mvaddstr(LINES - 5, 3,
  "The army unit numbering system is used to determine what id numbers");
      mvaddstr(LINES - 4, 3,
  "are given to units when they are first created.  When a unit is created");
      mvaddstr(LINES - 3, 3,
  "the type (Militia, Archers, etc.) is checked for a starting number,");
      mvaddstr(LINES - 2, 3,
  "then the type (Mercenary, Scouts, etc.) then the default base number is");
      mvaddstr(LINES - 1, 3,
  "used.  If a nation has no scheme set, the global default will be used.");
      presskey();
      break;
    case 'D':
    case 'd':
      /* set the defaults */
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Replace current settings with defaults? ");
      if (y_or_n()) {
	page_ptr = NULL;
	if (ntn_ptr == NULL) {
	  XUNUMDEFAULT;
	  unum_defaults();
	} else {
	  XUNUMCOPY;
	  unum_copydefault();
	}
      }
      break;
    case 'C':
    case 'c':
      /* clear the numbering */
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Clear out all of the current settings? ");
      if (y_or_n()) {
	page_ptr = NULL;
	XUNUMRESET;
	resetnumbers();
      }
      break;
    case 'R':
    case 'r':
      /* renumbering of all of the units */
      if (ntn_ptr == NULL) {
	errormsg("You don't have any units to renumber");
	break;
      }
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Renumber all of the units in the nation? ");
      if (y_or_n()) {
	page_ptr = NULL;
	XNRENUM;
	army_reorganize();
      }
      break;
    case 'S':
    case 's':
      /* set the base number */
      clear_bottom(0);
      mvaddstr(LINES - 3, 0, "Enter a default starting number? ");
      x = get_number(FALSE);
      if (no_input == TRUE) break;

      /* check the range */
      if ((x <= 0) ||
	  (x > MAX_IDTYPE)) {
	sprintf(string, "A unit number must be between 1 and %ld",
		MAX_IDTYPE);
	errormsg(string);
      } else {
	page_ptr = NULL;
	newslotnumber(x, 0, 0);
	global_int = 0;
	global_long = 0;
	XUNUMSLOT;
      }
      break;
    case 'U':
    case 'u':
      /* by the unit type */
      clear_bottom(0);
      mvaddstr(LINES - 3, 0, "Number which Unit Type? ");
      idnum = get_atype();
      if (no_input == TRUE) break;

      /* check the range */
      if (idnum == num_armytypes) {
	errormsg("That is not any army unit type that I know of");
      } else {
	mvprintw(LINES - 2, 0, "Enter a starting number for %s units? ",
		 ainfo_list[idnum].name);
	x = get_number(FALSE);
	if (no_input == TRUE) break;

	/* check the range */
	if ((x <= 0) ||
	    (x > MAX_IDTYPE)) {
	  sprintf(string, "A unit number must be between 1 and %ld",
		  MAX_IDTYPE);
	  errormsg(string);
	} else {
	  page_ptr = NULL;
	  newslotnumber(x, idnum, 2);
	  global_long = 2;
	  global_int = idnum;
	  XUNUMSLOT;
	}
      }
      break;
    case 'A':
    case 'a':
      /* by the army class */
      clear_bottom(0);
      mvaddstr(LINES - 3, 0, "Number which Army Class? ");
      idnum = get_aclass();
      if (no_input == TRUE) break;

      /* check the range */
      if (idnum == num_aclasses) {
	errormsg("That is not any army unit type that I know of");
      } else {
	mvprintw(LINES - 2, 0, "Enter a starting number for %s class units? ",
		 ainfo_clist[idnum]);
	x = get_number(FALSE);
	if (no_input == TRUE) break;

	/* check the range */
	if ((x <= 0) ||
	    (x > MAX_IDTYPE)) {
	  sprintf(string, "A unit number must be between 1 and %ld",
		  MAX_IDTYPE);
	  errormsg(string);
	} else {
	  page_ptr = NULL;
	  newslotnumber(x, idnum, 1);
	  global_long = 1;
	  global_int = idnum;
	  XUNUMSLOT;
	}
      }
      break;
    default:
      /* huh? */
      errormsg("Unknown command selection");
      break;
    }
  } while (done == FALSE);

  /* finish up */
  redraw = DRAW_FULL;
  if (is_god == TRUE) reset_god();
  return(0);
}

/*
 * ungroup_scouts - Remove any scouts from a group and set to defend
 *
 * Automatically ungroups any scout units that are grouped under a specific
 * leader and sets them to defend status. This enforces the game rule that
 * scouts must work independently and cannot remain grouped.
 *
 * Parameters:
 *   grpnum - Group leader ID number to check for grouped scouts
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Changes grouped scout units to defend status
 *   - Removes scouts from group leadership
 *   - Updates unit status flags for affected scouts
 *
 * Notes:
 *   - Only affects scout units grouped under specified leader
 *   - Scouts are set to ST_DEFEND status after ungrouping
 *   - Searches units in same location as the group leader
 *   - Preserves army_ptr state by saving and restoring
 *   - Called automatically when group conditions change
 *   - Enforces game rule that scouts work alone
 */
void
ungroup_scouts PARM_1(int, grpnum)
{
  ARMY_PTR a1_ptr, ahold_ptr = army_ptr;

  /* first get the main leader */
  if ((a1_ptr = armybynum(grpnum)) == NULL) return;

  /* search for the sector with the leader */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {
    if ((a1_ptr->xloc == army_ptr->xloc) &&
	(a1_ptr->yloc == army_ptr->yloc)) break;
  }

  /* now search for scouts */
  for (; army_ptr != NULL; army_ptr = army_ptr->nrby) {
    if (a_isscout(ARMY_TYPE) &&
	(ARMY_LEAD == grpnum) &&
	(unit_status(ARMY_STAT) == ST_GROUPED)) {
      set_status(ARMY_STAT, ST_DEFEND);
      AADJSTAT;
    }
  }
  army_ptr = ahold_ptr;
}
