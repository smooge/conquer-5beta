/* Misc. manipulation routines for army, navy and caravan units */
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
#include "armyX.h"
#include "cityX.h"
#include "itemX.h"
#include "navyX.h"
#include "butesX.h"
#include "calenX.h"
#include "desigX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "racesX.h"
#include "worldX.h"
#include "activeX.h"
#include "elevegX.h"
#include "statusX.h"
#include "nclassX.h"
#include "caravanX.h"
#include "dstatusX.h"

/*
 * army_support - Calculate the support costs for an army unit
 *
 * Calculates the maintenance costs required to support an army unit, including
 * food consumption, payment (talons), and special costs for monster units.
 * Handles different unit types with varying cost structures and applies
 * magical and mercenary cost adjustments.
 *
 * Parameters:
 *   a1_ptr - Pointer to army unit (must not be NULL)
 *   out_costs - Array to store calculated costs (must not be NULL)
 *   amount - Multiplier for cost calculation (supply periods)
 *
 * Returns:
 *   None (results stored in out_costs array)
 *
 * Side Effects:
 *   - Zeros out_costs array initially
 *   - Applies magical cost adjustments
 *   - Different cost calculations for monsters vs normal units
 *
 * Notes:
 *   - Free supply units (scouts) have zero costs
 *   - Monster units cost jewels based on missing magical powers
 *   - Normal units consume food and require talon payment
 *   - Mercenary units have higher costs based on market rates
 */
/* ARMY_SUPPORT -- Calculate the support costs for an army unit */
void
army_support PARM_3(ARMY_PTR, a1_ptr, itemtype *, out_costs, int, amount)
{
  int i, useval;

  /* check the input */
  if ((a1_ptr == NULL) &&
      (out_costs == NULL)) return;

  /* now clean costs */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    out_costs[i] = (itemtype) 0;
  }

  /* now assign costs */
  if (a_freesupply(a1_ptr->unittype)) return;

  /* monsters cost jewels only */
  if (a_ismonster(a1_ptr->unittype)) {

    /* gobble up them jewels */
    if ((useval = utype_mayuse(ntn_ptr, a1_ptr->unittype)) <= 0) {

      /* figure the cost; twice the missing powers plus two. */
      useval = 2 - useval;
      useval *= 2;

      /* adjust for monster nations */
      if (MIL_MAGIC(MM_ORC)) {
	useval--;
      }
      if (MIL_MAGIC(MM_OGRE)) {
	useval--;
      }
      if (MIL_MAGIC(MM_DRAGON)) {
	useval--;
      }

    }
    out_costs[MTRLS_JEWELS] = a1_ptr->strength * amount *
      ainfo_list[a1_ptr->unittype].maint * useval;

  } else {

    /* a normal unit eats food and takes pay */
    if (!a_isscout(a1_ptr->unittype)) {
      out_costs[MTRLS_FOOD] = amount * a1_ptr->strength;
    }
    out_costs[MTRLS_TALONS] = amount * a1_ptr->strength *
      ainfo_list[a1_ptr->unittype].maint + ARMYUNITCOST;
    if (a_ismerc(a1_ptr->unittype)) {
      out_costs[MTRLS_TALONS] *= merc_costrate();
      out_costs[MTRLS_TALONS] /= 100;
    }

  }
  mgk_cost_adjust(1, out_costs);
}

/*
 * a_issupplyable - Determine if an army unit can be supplied from given coordinates
 *
 * Checks whether an army unit is eligible to receive supplies from a specific
 * location based on unit status and positioning constraints. Validates that
 * units with certain statuses can only be supplied from their current location.
 *
 * Parameters:
 *   a1_ptr - Pointer to army unit to check (must not be NULL)
 *   x - X coordinate of potential supply source
 *   y - Y coordinate of potential supply source  
 *   sayit - If TRUE, display error messages for invalid supply attempts
 *
 * Returns:
 *   TRUE if unit can be supplied from (x,y), FALSE otherwise
 *
 * Side Effects:
 *   - May display error messages if sayit is TRUE
 *   - No state modifications
 *
 * Notes:
 *   - Units with certain statuses can only be supplied from current location
 *   - Supply restrictions prevent remote supply of immobilized units
 *   - Used by supply management system to validate supply operations
 */
/* A_ISSUPPLYABLE -- Determine if a unit is or is not supplyable from x, y */
int
a_issupplyable PARM_4(ARMY_PTR, a1_ptr, int, x, int, y, int, sayit)
{
  /* check input */
  if ((a1_ptr == NULL) ||
      !XY_ONMAP(x, y)) {
    return(FALSE);
  }

  /* check the statuses of the troops */
  if (splyinsect_stat(a1_ptr->status) &&
      ((a1_ptr->xloc != x) ||
       (a1_ptr->yloc != y))) {
    if (sayit) {
      sprintf(string,
	      "Unit %d has a status of %s and can't be supplied from here",
	      a1_ptr->armyid,
	      stat_info[unit_status(a1_ptr->status)].name);
      errormsg(string);
    }
    return(FALSE);
  }

  /* can be done */
  return(TRUE);
}

/*
 * cvn_support - Calculate the supply levels for a caravan
 *
 * Computes the maintenance costs required to support a caravan unit,
 * including crew and passenger food consumption and talon maintenance costs.
 * Similar to army_support but specialized for caravan logistics.
 *
 * Parameters:
 *   v1_ptr - Pointer to caravan unit (must not be NULL)
 *   out_costs - Array to store calculated costs (must not be NULL)
 *   amount - Multiplier for cost calculation (supply periods)
 *
 * Returns:
 *   None (results stored in out_costs array)
 *
 * Side Effects:
 *   - Zeros out_costs array initially
 *   - Applies magical cost adjustments for caravan type
 *
 * Notes:
 *   - Costs scale with caravan size, crew, and passenger count
 *   - Food requirements include both crew and transported people
 *   - Talon costs are fixed per caravan size unit
 */
/* CVN_SUPPORT -- Calculate the supply levels for a caravan */
void
cvn_support PARM_3(CVN_PTR, v1_ptr, itemtype *, out_costs, int, amount)
{
  int i;

  /* check the input */
  if ((v1_ptr == NULL) &&
      (out_costs == NULL)) return;

  /* now clean costs */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    out_costs[i] = (itemtype) 0;
  }

  /* compute costs */
  out_costs[MTRLS_TALONS] = (int)v1_ptr->size * CVNMAINT * amount;
  out_costs[MTRLS_FOOD] = v1_ptr->size * (int)v1_ptr->crew * amount;
  out_costs[MTRLS_FOOD] += v1_ptr->size * (int)v1_ptr->people * amount;
  mgk_cost_adjust(0, out_costs);
}

/*
 * cvn_redocosts - Calculate the repair costs for a caravan
 *
 * Computes the materials and costs required to repair a damaged caravan
 * back to full efficiency. Repair costs are proportional to damage level
 * and caravan size, requiring both talons and wood resources.
 *
 * Parameters:
 *   v1_ptr - Pointer to caravan unit (must not be NULL)
 *   out_costs - Array to store calculated repair costs (must not be NULL)
 *
 * Returns:
 *   None (results stored in out_costs array)
 *
 * Side Effects:
 *   - Zeros out_costs array initially
 *   - Applies magical cost adjustments
 *
 * Notes:
 *   - Repair costs scale with (100 - efficiency) percentage
 *   - Requires both talons and wood materials
 *   - No cost if caravan is at 100% efficiency
 */
/* CVN_REDOCOSTS -- Calculate the repair costs for the caravan */
void
cvn_redocosts PARM_2(CVN_PTR, v1_ptr, itemtype *, out_costs)
{
  int i;

  /* check the input */
  if ((v1_ptr == NULL) &&
      (out_costs == NULL)) return;

  /* now clean costs */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    out_costs[i] = (itemtype) 0;
  }

  /* compute the costs */
  out_costs[MTRLS_TALONS] = (v1_ptr->size * CARAVANCOST *
			     (100 - (int) v1_ptr->efficiency)) / 100;
  out_costs[MTRLS_WOOD] = (v1_ptr->size * CARAVANWOOD *
			   (100 - (int) v1_ptr->efficiency)) / 100;
  mgk_cost_adjust(0, out_costs);
}

/*
 * v_issupplyable - Determine if a caravan can be supplied from given coordinates
 *
 * Checks whether a caravan unit is eligible to receive supplies from a specific
 * location based on unit status and positioning constraints. Similar to
 * a_issupplyable but specialized for caravan supply validation.
 *
 * Parameters:
 *   v1_ptr - Pointer to caravan unit to check (must not be NULL)
 *   x - X coordinate of potential supply source
 *   y - Y coordinate of potential supply source
 *   sayit - If TRUE, display error messages for invalid supply attempts
 *
 * Returns:
 *   TRUE if caravan can be supplied from (x,y), FALSE otherwise
 *
 * Side Effects:
 *   - May display error messages if sayit is TRUE
 *   - No state modifications
 *
 * Notes:
 *   - Caravans with certain statuses can only be supplied from current location
 *   - Supply restrictions prevent remote supply of immobilized caravans
 *   - Used by caravan supply management system
 */
/* V_ISSUPPLYABLE -- Determine if a unit is or is not supplyable from x, y */
int
v_issupplyable PARM_4(CVN_PTR, v1_ptr, int, x, int, y, int, sayit)
{
  /* check input */
  if ((v1_ptr == NULL) ||
      !XY_ONMAP(x, y)) {
    return(FALSE);
  }

  /* check the statuses of the troops */
  if (splyinsect_stat(v1_ptr->status) &&
      ((v1_ptr->xloc != x) ||
       (v1_ptr->yloc != y))) {
    if (sayit) {
      sprintf(string,
	      "Caravan %d has a status of %s and can't be supplied from here",
	      v1_ptr->cvnid,
	      stat_info[unit_status(v1_ptr->status)].name);
      errormsg(string);
    }
    return(FALSE);
  }

  /* can be done */
  return(TRUE);
}

/*
 * navy_support - Calculate the supply levels for a naval unit
 *
 * Computes the maintenance costs required to support a naval fleet,
 * including crew food consumption, passenger food (for galleys), and
 * talon maintenance costs. Costs scale with ship types, sizes, and crew.
 *
 * Parameters:
 *   n1_ptr - Pointer to navy unit (must not be NULL)
 *   out_costs - Array to store calculated costs (must not be NULL)
 *   amount - Multiplier for cost calculation (supply periods)
 *
 * Returns:
 *   None (results stored in out_costs array)
 *
 * Side Effects:
 *   - Zeros out_costs array initially
 *   - Applies magical cost adjustments for naval type
 *
 * Notes:
 *   - Costs calculated per cargo hold across all ship types
 *   - Galleys require additional food for transported people
 *   - Crew food consumption scales with fleet composition
 */
/* NAVY_SUPPORT -- Calculate the supply levels for a naval unit */
void
navy_support PARM_3(NAVY_PTR, n1_ptr, itemtype *, out_costs, int, amount)
{
  int i, val;

  /* check the input */
  if ((n1_ptr == NULL) &&
      (out_costs == NULL)) return;

  /* now clean costs */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    out_costs[i] = (itemtype) 0;
  }

  /* compute food costs */
  for (i = 0; i < NSHP_NUMBER; i++) {
    val = cargo_holds(n1_ptr->ships[i]);
    out_costs[MTRLS_FOOD] += val * n1_ptr->crew * amount;
    out_costs[MTRLS_TALONS] += val * NAVYMAINT * amount;
    if (i == NSHP_GALLEYS) {
      out_costs[MTRLS_FOOD] += val * n1_ptr->people * amount;
    }
  }
  mgk_cost_adjust(2, out_costs);
}

/*
 * navy_redocosts - Calculate the repair costs for a naval fleet
 *
 * Computes the materials and costs required to repair damaged ships
 * back to full efficiency. Repair costs vary by ship type, size, and
 * damage level, requiring both talons and wood resources.
 *
 * Parameters:
 *   n1_ptr - Pointer to navy unit (must not be NULL)
 *   out_costs - Array to store calculated repair costs (must not be NULL)
 *
 * Returns:
 *   None (results stored in out_costs array)
 *
 * Side Effects:
 *   - Zeros out_costs array initially
 *   - Applies magical cost adjustments
 *
 * Notes:
 *   - Repair costs scale with (100 - efficiency) per ship type
 *   - Different ship types have different talon and wood costs
 *   - Costs calculated per cargo hold for each ship class
 */
/* NAVY_REDOCOSTS -- Calculate the repair costs for the navy */
void
navy_redocosts PARM_2(NAVY_PTR, n1_ptr, itemtype *, out_costs)
{
  int i, val;

  /* check the input */
  if ((n1_ptr == NULL) &&
      (out_costs == NULL)) return;

  /* now clean costs */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    out_costs[i] = (itemtype) 0;
  }

  /* scan each class of ships */
  for (i = 0; i < NSHP_NUMBER; i++) {
    val = cargo_holds(n1_ptr->ships[i]);
    out_costs[MTRLS_TALONS] += (val * ninfo_list[i].taloncost *
				(100 - (int) n1_ptr->efficiency[i])) / 100;
    out_costs[MTRLS_WOOD] += (val * ninfo_list[i].woodcost *
			      (100 - (int) n1_ptr->efficiency[i])) / 100;
  }
  mgk_cost_adjust(2, out_costs);
}

/*
 * n_issupplyable - Determine if a naval unit can be supplied from given coordinates
 *
 * Checks whether a naval fleet is eligible to receive supplies from a specific
 * location. Naval units have special restrictions: fleets at sea can only be
 * supplied from their current sector, while fleets in port can be supplied normally.
 *
 * Parameters:
 *   n1_ptr - Pointer to navy unit to check (must not be NULL)
 *   x - X coordinate of potential supply source
 *   y - Y coordinate of potential supply source
 *   sayit - If TRUE, display error messages for invalid supply attempts
 *
 * Returns:
 *   TRUE if navy can be supplied from (x,y), FALSE otherwise
 *
 * Side Effects:
 *   - May display error messages if sayit is TRUE
 *   - No state modifications
 *
 * Notes:
 *   - Fleets at sea (ELE_WATER) can only be supplied from same sector
 *   - This prevents unrealistic remote supply of naval units
 *   - Used by naval supply management system
 */
/* N_ISSUPPLYABLE -- Determine if a unit is or is not supplyable from x, y */
int
n_issupplyable PARM_4(NAVY_PTR, n1_ptr, int, x, int, y, int, sayit)
{
  /* check input */
  if ((n1_ptr == NULL) ||
      !XY_ONMAP(x, y)) {
    return(FALSE);
  }

  /* check the statuses of the troops */
  if ((sct[n1_ptr->xloc][n1_ptr->yloc].altitude == ELE_WATER) &&
      ((n1_ptr->xloc != x) ||
       (n1_ptr->yloc != y))) {
    if (sayit) {
      errormsg("Fleets may only be supplied by the same sector while at sea");
    }
    return(FALSE);
  }

  /* can be done */
  return(TRUE);
}

/*
 * navy_addships - Add ships of specified type and size to a fleet element
 *
 * Increases the ship count in a fleet information word by adding nships
 * of the specified size category. Performs bounds checking to ensure the
 * resulting count doesn't exceed the maximum allowed ships per size class.
 *
 * Parameters:
 *   info - Current fleet information word (bit-packed ship counts)
 *   sh_size - Ship size class (N_LIGHT, N_MEDIUM, N_HEAVY)
 *   nships - Number of ships to add (must be positive)
 *
 * Returns:
 *   Updated fleet information word on success, -1 on failure
 *
 * Side Effects:
 *   - None (pure function, returns modified value)
 *
 * Notes:
 *   - Ship information is bit-packed with size-specific bit positions
 *   - Maximum ships per size class is limited by N_MASK
 *   - Validates size parameters before modification
 *   - Used for fleet construction and reinforcement operations
 */
/* NAVY_ADDSHIPS -- increase element by nships of given type
                    return resulting element or -1 for failure */
int
navy_addships PARM_3(uns_short, info, int, sh_size, int, nships)
{
  int hold = nships;

  /* check for valid input */
  if ((sh_size < N_LIGHT) || (sh_size > N_HEAVY))
    return(-1);

  /* check the sizing */
  hold += N_CNTSHIPS(info, sh_size);
  if (hold > N_MASK) {
    return(-1);
  }

  /* place new value in proper place */
  hold <<= (N_BITSIZE * sh_size);

  /* set it in variable */
  info |= hold;

  /* form mask so other values won't be changed */
  hold |= ~(N_MASK << (N_BITSIZE * sh_size));

  /* now change the variable */
  info &= hold;
  return(info);
}

/*
 * navy_subships - Remove ships of specified type and size from a fleet element
 *
 * Decreases the ship count in a fleet information word by removing nships
 * of the specified size category. Performs validation to ensure sufficient
 * ships exist before removal to prevent negative counts.
 *
 * Parameters:
 *   info - Current fleet information word (bit-packed ship counts)
 *   sh_size - Ship size class (N_LIGHT, N_MEDIUM, N_HEAVY)
 *   nships - Number of ships to remove (must be positive)
 *
 * Returns:
 *   Updated fleet information word on success, -1 on failure
 *
 * Side Effects:
 *   - None (pure function, returns modified value)
 *
 * Notes:
 *   - Validates sufficient ships exist before removal
 *   - Ship information is bit-packed with size-specific bit positions
 *   - Used for fleet reduction, losses, and ship transfers
 *   - Prevents invalid negative ship counts
 */
/* NAVY_SUBSHIPS -- remove nships of given shipsize for a given fleet
                    return result or -1 if it is not possible */
int
navy_subships PARM_3(uns_short, info, int, sh_size, int, nships)
{
  short hold;

  /* check for valid input */
  if ((sh_size < N_LIGHT) || (sh_size > N_HEAVY))
    return(-1);

  /* check number of remaining ships */
  hold = N_CNTSHIPS(info, sh_size) - nships;
  if (hold < 0) {
    return(-1);
  }

  /* place new value in proper place */
  hold <<= (N_BITSIZE * sh_size);

  /* set it in variable */
  info |= hold;

  /* form mask so other values won't be changed */
  hold |= ~(N_MASK << (N_BITSIZE * sh_size));

  /* now change the variable */
  info &= hold;
  return(info);
}

/*
 * navy_mvpts - Calculate movement points for a naval fleet
 *
 * Determines the movement speed of a naval fleet based on ship composition,
 * crew levels, nation magical powers, and unit status. Slower ships limit
 * the entire fleet's speed, and various factors can enhance or reduce speed.
 *
 * Parameters:
 *   nat_ptr - Pointer to nation owning the fleet (must not be NULL)
 *   nvy_ptr - Pointer to navy unit (must not be NULL)
 *
 * Returns:
 *   Movement points available for the fleet, 0 if invalid parameters
 *
 * Side Effects:
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Fleet speed limited by slowest ship type and size
 *   - Crew levels affect speed proportionally
 *   - Magical powers (SAILOR, MARINE, WATER) provide speed bonuses
 *   - Unit speed status affects final movement calculation
 *   - Heavy ships are slower than light ships of same type
 */
/* NAVY_MVPTS --  Find speed of naval fleet */
int
navy_mvpts PARM_2(NTN_PTR, nat_ptr, NAVY_PTR, nvy_ptr)
{
  int i, j, hold = 999;

  /* check input */
  if ((nat_ptr == NULL) || (nvy_ptr == NULL)) return(0);

  /* go over all of the ship types */
  for (j = 0; j < NSHP_NUMBER; j++) {

    /* check speed */
    if (hold < ninfo_list[j].speed) continue;

    /* light ships faster than heavy ships */
    for (i = N_LIGHT; i <= N_HEAVY; i++)
      if ((N_CNTSHIPS(nvy_ptr->ships[j], i) > 0) &&
	  (hold > (int) ninfo_list[j].speed + (2 - i) * N_SIZESPD)) {
	hold = (int) ninfo_list[j].speed + (2 - i) * N_SIZESPD;
      }

  }

  /* check results */
  if (hold == 999) {
    hold = N_NOSPD;
  } else {

    /* adjust for the speed setting */
    hold *= (int) (unit_speed(nvy_ptr->status) + 1);
    hold /= 2;

    /* enhance speed of ships if nation is skilled or knowledgable */
    if (MAGIC(nat_ptr->powers[MAG_CIVILIAN], MC_SAILOR)) {
      hold *= 5;
      hold /= 4;
    }
    if (MAGIC(nat_ptr->powers[MAG_CIVILIAN], MC_MARINE)) {
      hold *= 5;
      hold /= 4;
    }
    if (MAGIC(nat_ptr->powers[MAG_WIZARDRY], MW_WATER)) {
      hold *= 5;
      hold /= 4;
    }
  }

  /* figure the crew into it */
  if (nvy_ptr->crew != MAXNAVYCREW) {
    hold *= (int) nvy_ptr->crew;
    hold /= MAXNAVYCREW;
  }
  return(hold);
}

/*
 * cvn_mvpts - Calculate movement points for a caravan
 *
 * Determines the movement speed of a caravan based on the nation's maximum
 * movement capability, crew levels, and unit speed status. Caravans have
 * simpler movement calculations than naval fleets.
 *
 * Parameters:
 *   nat_ptr - Pointer to nation owning the caravan (must not be NULL)
 *   c_ptr - Pointer to caravan unit (must not be NULL)
 *
 * Returns:
 *   Movement points available for the caravan, 0 if invalid parameters
 *
 * Side Effects:
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Base speed is min(nation_maxmove, 12) divided by 2
 *   - Unit speed status modifies movement
 *   - Crew levels affect speed proportionally
 *   - Simpler calculation than naval or army movement
 */
/* CVN_MVPTS --  Find speed of caravan */
int
cvn_mvpts PARM_2(NTN_PTR, nat_ptr, CVN_PTR, c_ptr)
{
  int hold;

  /* check input */
  if ((nat_ptr == NULL) || (c_ptr == NULL)) return(0);

  /* move value is min(12,maxmove) modified by speed */
  hold = max(nat_ptr->maxmove, 12) / 2;
  hold *= (1 + (int) unit_speed(c_ptr->status));
  hold /= 2;

  /* figure the crew into it */
  if (c_ptr->crew != MAXCVNCREW) {
    hold *= (int) c_ptr->crew;
    hold /= MAXCVNCREW;
  }
  return(hold);
}

/*
 * main_leader - Find the top-level leader of a unit group
 *
 * Traverses the leadership chain to find the main leader of a group,
 * following leader pointers until reaching a unit that is not grouped
 * under another leader. Handles circular references and invalid chains.
 *
 * Parameters:
 *   idnum - Army ID number to start leadership search from
 *
 * Returns:
 *   Pointer to main leader unit, or NULL if not found
 *
 * Side Effects:
 *   - None (read-only traversal)
 *
 * Notes:
 *   - Follows leader chain until finding ungrouped unit
 *   - Protects against infinite loops with circular leader references
 *   - Used for group command and coordination operations
 *   - Returns the actual decision-making leader of a group
 */
/* MAIN_LEADER -- Get the setting of the guy at the front of a group */
ARMY_PTR
main_leader PARM_1( int, idnum )
{
  ARMY_PTR a1_ptr, a2_ptr;

  /* first get the army */
  a1_ptr = armybynum(idnum);
  while (a1_ptr != NULL) {

    /* is this it? */
    if (unit_status(a1_ptr->status) != ST_GROUPED) {
      break;
    }

    /* keep going */
    a2_ptr = armybynum(a1_ptr->leader);
    if (a1_ptr == a2_ptr) break;
    a1_ptr = a2_ptr;

  }
  return(a1_ptr);
}

/*
 * men_ingroup - Count the number of soldiers in a given group
 *
 * Recursively counts all units under a group leader's command, including
 * the leader and all grouped subordinates. Can optionally count only
 * anti-aircraft capable units for air defense calculations.
 *
 * Parameters:
 *   idnum - Army ID of the group leader
 *   onlyaair - If TRUE, count only anti-aircraft capable units
 *
 * Returns:
 *   Total count of soldiers/units in the group
 *
 * Side Effects:
 *   - None (read-only counting operation)
 *
 * Notes:
 *   - Validates leader is actually leading before counting
 *   - Includes leader in count if appropriate
 *   - Recursively counts sub-groups led by subordinate leaders
 *   - Uses army capture size for non-leader units
 *   - Essential for group management and combat calculations
 */
/* MEN_INGROUP -- Return the number of soldiers in a given group */
int
men_ingroup PARM_2( int, idnum, int, onlyaair )
{
  ARMY_PTR a1_ptr, a2_ptr;
  long count = 0L;

  /* find the leader unit */
  a1_ptr = armybynum(idnum);

  /* validate that it is leading */
  if ((a1_ptr == NULL) ||
      !a_isleader(a1_ptr->unittype) ||
      !unit_leading(a1_ptr->status) ) return(count);

  /* add in the leader */
  if (!onlyaair ||
      a_antiair(a1_ptr->unittype)) {
    count++;
  }

  /* now go through nation list and find proper location */
  for (a2_ptr = ntn_ptr->army_list;
       a2_ptr != NULL;
       a2_ptr = a2_ptr->next) {

    /* locate the sector the leader is in */
    if ((a1_ptr->xloc == a2_ptr->xloc)
	&& (a1_ptr->yloc == a2_ptr->yloc)) {
      break;
    }

  }

  /* go through all of the units in the sector */
  for (; a2_ptr != NULL; a2_ptr = a2_ptr->nrby) {

    /* skip the leader */
    if (a2_ptr == a1_ptr) continue;

    /* count it if it is grouped */
    if ((unit_status(a2_ptr->status) == ST_GROUPED) &&
	(a2_ptr->leader == idnum)) {

      /* count 'em up */
      if (a_isleader(a2_ptr->unittype)) {
	if (unit_leading(a2_ptr->status)) {
	  count += men_ingroup(a2_ptr->armyid, onlyaair);
	} else if (!onlyaair ||
		   a_antiair(a2_ptr->unittype)) {
	  count++;
	}
      } else if (!onlyaair ||
		 a_antiair(a2_ptr->unittype)) {
	count += army_captsize(a2_ptr, FALSE);
      }

    }

  }

  /* give back the result */
  return(count);
}

/*
 * ships_in_sector - Count the total number of ships in a given sector
 *
 * Scans all naval fleets in the current nation located at the specified
 * coordinates and counts the total number of individual ships across all
 * ship types and sizes. Used for naval traffic and capacity analysis.
 *
 * Parameters:
 *   x - X coordinate of sector to count
 *   y - Y coordinate of sector to count
 *
 * Returns:
 *   Total number of individual ships in the sector
 *
 * Side Effects:
 *   - None (read-only counting operation)
 *
 * Notes:
 *   - Counts ships across all size classes (light, medium, heavy)
 *   - Sums ships from all fleet types (galleys, warships, etc.)
 *   - Only counts ships belonging to current nation (ntn_ptr)
 *   - Returns 0 if no nation context or no ships found
 */
/* SHIPS_IN_SECTOR -- Retrun the number of ships in a given sector */
int
ships_in_sector PARM_2( int, x, int, y)
{
  NAVY_PTR n1_ptr;
  int i, j, num = 0;

  /* check if a search is possible */
  if (ntn_ptr == NULL) return(0);

  /* find all of the fleets in the given sector */
  for (n1_ptr = ntn_ptr->navy_list;
       n1_ptr != NULL;
       n1_ptr = n1_ptr->next) {
    /* did we find any? */
    if ((n1_ptr->xloc == x) && (n1_ptr->yloc == y)) {
      for (j = 0; j < NSHP_NUMBER; j++) {
	for (i = N_LIGHT; i <= N_HEAVY; i++) {
	  num += N_CNTSHIPS(n1_ptr->ships[j], i);
	}
      }
    }
  }

  /* got it.. now go */
  return(num);
}

/*
 * wagons_in_sector - Count the total number of caravan wagons in a sector
 *
 * Scans all caravan units in the current nation located at the specified
 * coordinates and counts the total number of individual wagons. Each caravan
 * unit contributes multiple wagons based on its size.
 *
 * Parameters:
 *   x - X coordinate of sector to count
 *   y - Y coordinate of sector to count
 *
 * Returns:
 *   Total number of individual wagons in the sector
 *
 * Side Effects:
 *   - None (read-only counting operation)
 *
 * Notes:
 *   - Each caravan unit contributes (size * WAGONS_IN_CVN) wagons
 *   - Only counts caravans belonging to current nation (ntn_ptr)
 *   - Used for traffic analysis and logistical planning
 *   - Returns 0 if no nation context or no caravans found
 */
/* WAGONS_IN_SECTOR -- Return the number of caravan wagons in a given sector */
int
wagons_in_sector PARM_2( int, x, int, y)
{
  CVN_PTR c1_ptr;
  int num = 0;

  /* check if a search is possible */
  if (ntn_ptr == NULL) return(0);

  /* find all of the caravan units in the given sector */
  for (c1_ptr = ntn_ptr->cvn_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {
    /* did we find any? */
    if ((c1_ptr->xloc == x) && (c1_ptr->yloc == y)) {
      num += c1_ptr->size * WAGONS_IN_CVN;
    }
  }

  /* got it.. now go */
  return(num);
}

/*
 * men_ingarrison - Count garrison troops in a sector across all allied nations
 *
 * Counts all military units with garrison status in the specified sector,
 * including troops from the sector owner and allied nations. Can optionally
 * count only anti-aircraft capable units for air defense calculations.
 *
 * Parameters:
 *   x - X coordinate of sector to examine
 *   y - Y coordinate of sector to examine
 *   onlyaair - If TRUE, count only anti-aircraft capable units
 *
 * Returns:
 *   Total count of garrisoned soldiers in the sector
 *
 * Side Effects:
 *   - Temporarily switches nation context during counting
 *   - Restores original nation context when complete
 *
 * Notes:
 *   - Only counts units with ST_GARRISON status
 *   - Includes allied nation troops if they have alliance with sector owner
 *   - Recursively counts grouped units under garrison leaders
 *   - Essential for defensive strength calculations
 *   - Validates sector ownership before counting
 */
/* MEN_INGARRISON -- Return the number of men garrisoned in the sector */
long
men_ingarrison PARM_3( int, x, int, y, int, onlyaair )
{
  NTN_PTR n1_ptr;
  ARMY_PTR a1_ptr;
  int i, sown;
  long count = 0;

  /* validation */
  if (!XY_ONMAP(x, y)) {
    return(count);
  }

  /* check ownership */
  if (((sown = sct[x][y].owner) == UNOWNED) ||
      (world.np[sown] == NULL)) {
    return(count);
  }

  /* go through all nations */
  n1_ptr = ntn_ptr;
  for (i = 1; i < MAXNTN; i++)
  if ((ntn_ptr = world.np[i]) != NULL) {

    /* no need to go on if they are not allied */
    if ((sown != i) &&
	(ntn_ptr->dstatus[sown] % dstatus_number != DIP_ALLIED)) {
      continue;
    }

    /* find the first army in the sector */
    for (a1_ptr = ntn_ptr->army_list;
	 a1_ptr != NULL;
	 a1_ptr = a1_ptr->next) {
      /* check for current location */
      if ((a1_ptr->xloc == x) &&
	  (a1_ptr->yloc == y)) break;
    }

    /* now count all units in garrison in the current sector */
    for (; a1_ptr != NULL; a1_ptr = a1_ptr->nrby) {

      /* garrison unit */
      if (unit_status(a1_ptr->status) == ST_GARRISON) {

	/* check for a leader */
	if (a_isleader(a1_ptr->unittype)) {

	  /* count as many as are in group with leader */
	  if (unit_leading(a1_ptr->status)) {
	    count += men_ingroup(a1_ptr->armyid, onlyaair);
	  } else if (!onlyaair ||
		     a_antiair(a1_ptr->unittype)) {
	    count++;
	  }

	} else if (!onlyaair ||
		   a_antiair(a1_ptr->unittype)) {
	  count += army_captsize(a1_ptr, FALSE);
	}

      }
    }

  }

  /* now, restore the value */
  ntn_ptr = n1_ptr;

  /* done, now send back results */
  return(count);
}

/*
 * set_grpmove - Assign movement settings to an entire unit group
 *
 * Sets the movement speed and movement points for a group leader and all
 * units grouped under that leader. Recursively applies settings to sub-groups
 * and handles orphaned units when the leader no longer exists.
 *
 * Parameters:
 *   idnum - Army ID of the group leader
 *   speed - Speed setting to apply (SPD_SLOW to SPD_STUCK)
 *   mval - Movement points value to assign
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Modifies unit speed and movement for entire group
 *   - May orphan grouped units if leader doesn't exist
 *   - Uses static depth counter to prevent infinite recursion
 *
 * Notes:
 *   - Recursively handles nested group structures
 *   - Orphaned units are reset to ST_DEFEND status
 *   - Validates speed parameters before application
 *   - Essential for coordinated group movement
 */
/* SET_GRPMOVE -- Assign a given movement to an entire group */
void
set_grpmove PARM_3(int, idnum, int, speed, int, mval)
{
  ARMY_PTR a1_ptr, a2_ptr;
  static int depth = 0;

  /* verify speed */
  if (speed < SPD_SLOW || speed > SPD_STUCK) return;

  /* verify that the group exists */
  if (depth == 0) {
    a1_ptr = main_leader(idnum);
  } else {
    a1_ptr = armybynum(idnum);
  }
  if (a1_ptr != NULL) {

    /* set the group number */
    idnum = a1_ptr->armyid;

    /* set it for the leader */
    set_speed(a1_ptr->status, speed);
    a1_ptr->umove = (uns_char) mval;

    /* get the first unit in the same sector */
    for (a2_ptr = ntn_ptr->army_list;
	 a2_ptr != NULL;
	 a2_ptr = a2_ptr->next) {
      if ((a2_ptr->xloc == a1_ptr->xloc) &&
	  (a2_ptr->yloc == a1_ptr->yloc)) break;
    }

    /* now assign any grouped army the correct movement */
    for (; a2_ptr != NULL; a2_ptr = a2_ptr->nrby) {

      /* grouped units get the assigned value */
      if (a2_ptr == a1_ptr) continue;
      if ((a2_ptr->leader == idnum) &&
	  (unit_status(a2_ptr->status) == ST_GROUPED)) {
	if (unit_leading(a2_ptr->status)) {
	  depth++;
	  set_grpmove(a2_ptr->armyid, speed, mval);
	  depth--;
	} else {
	  set_speed(a2_ptr->status, speed);
	  a2_ptr->umove = mval;
	}
      }

    }

  } else {

    /* deassign any troops with grouped status with idnum */
    for (a2_ptr = ntn_ptr->army_list;
	 a2_ptr != NULL;
	 a2_ptr = a2_ptr->next) {

      /* find troops under the "leader" */
      if (a2_ptr->leader == idnum) {
	a2_ptr->leader = EMPTY_HOLD;
	if (unit_status(a2_ptr->status) == ST_GROUPED) {
	  set_status(a2_ptr->status, ST_DEFEND);
	}
      }

    }

  }
}

/*
 * set_grploc - Assign a location to an entire unit group
 *
 * Moves a group leader and all units grouped under that leader to the
 * specified coordinates. Recursively handles sub-groups and triggers
 * army sorting to maintain proper sector organization.
 *
 * Parameters:
 *   idnum - Army ID of the group leader
 *   x - Target X coordinate (must be valid map position)
 *   y - Target Y coordinate (must be valid map position)
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Modifies unit locations for entire group
 *   - Triggers army_sort() to reorganize unit lists
 *   - May orphan grouped units if leader doesn't exist
 *   - Uses static depth counter to prevent infinite recursion
 *
 * Notes:
 *   - Validates map coordinates before movement
 *   - Recursively handles nested group structures
 *   - Orphaned units are reset to ST_DEFEND status
 *   - Essential for coordinated group movement operations
 */
/* SET_GRPLOC -- Assign a given location to an entire group */
void
set_grploc PARM_3(int, idnum, int, x, int, y)
{
  ARMY_PTR a1_ptr, a2_ptr;
  static int depth = 0;

  /* verify speed */
  if (!XY_ONMAP(x, y)) return;

  /* verify that the group exists */
  if (depth == 0) {
    a1_ptr = main_leader(idnum);
  } else {
    a1_ptr = armybynum(idnum);
  }
  if (a1_ptr != NULL) {

    /* now be sure of the group id */
    idnum = a1_ptr->armyid;

    /* find armies in the same sector */
    for (a2_ptr = ntn_ptr->army_list;
	 a2_ptr != NULL;
	 a2_ptr = a2_ptr->next) {
      /* locate the units grouped under this leader */
      if ((a1_ptr == a2_ptr) ||
	  ((a2_ptr->xloc == a1_ptr->xloc) &&
	   (a2_ptr->yloc == a1_ptr->yloc)) ||
	  ((a2_ptr->leader == idnum) &&
	   (unit_status(a2_ptr->status) == ST_GROUPED)))
	break;
    }

    /* now assign the location */
    a1_ptr->xloc = x;
    a1_ptr->yloc = y;

    /* now assign any grouped army the correct movement */
    for (; a2_ptr != NULL; a2_ptr = a2_ptr->nrby) {

      /* grouped units get the assigned value */
      if ((unit_status(a2_ptr->status) == ST_GROUPED) &&
	  (a2_ptr->leader == idnum)) {
	if (unit_leading(a2_ptr->status)) {
	  depth++;
	  set_grploc(a2_ptr->armyid, x, y);
	  depth--;
	} else {
	  a2_ptr->xloc = x;
	  a2_ptr->yloc = y;
	}
      }

    }

  } else {

    /* deassign any troops with grouped status with idnum */
    for (a2_ptr = ntn_ptr->army_list;
	 a2_ptr != NULL;
	 a2_ptr = a2_ptr->next) {

      /* find troops under the "leader" */
      if (a2_ptr->leader == idnum) {
	a2_ptr->leader = EMPTY_HOLD;
	if (unit_status(a2_ptr->status) == ST_GROUPED) {
	  set_status(a2_ptr->status, ST_DEFEND);
	}
      }

    }

  }

  /* army sorting is now needed */
  army_sort(FALSE);
}

/*
 * group_stat - Determine the effective status of a grouped unit
 *
 * Follows the leadership chain to find the actual status of a grouped unit
 * by tracing through leader pointers until finding a unit with a non-grouped
 * status. Handles circular references and broken chains gracefully.
 *
 * Parameters:
 *   idnum - Army ID of the unit to check status for
 *
 * Returns:
 *   Effective status of the unit (ST_DEFEND if problems detected)
 *
 * Side Effects:
 *   - May modify unit status to ST_DEFEND if circular reference detected
 *   - May display error messages for invalid group chains
 *
 * Notes:
 *   - Prevents infinite loops with depth counter (max 20 iterations)
 *   - Defaults to ST_DEFEND for safety when problems detected
 *   - Essential for determining real unit behavior in groups
 *   - Fixes self-referential leader pointers automatically
 */
/* GROUP_STAT -- Return the status of the given group */
int
group_stat PARM_1(int, idnum)
{
  ARMY_PTR a1_ptr, a2_ptr;
  int count, ival, statval = ST_DEFEND;

  /* verify that the group exists */
  if ((a1_ptr = armybynum(idnum)) != NULL) {

    /* keep going until the actual status is determined */
    count = 0;
    a2_ptr = a1_ptr;
    statval = unit_status(a2_ptr->status);
    while (statval == ST_GROUPED) {
      if (a2_ptr->leader == a2_ptr->armyid) {
	a2_ptr->leader = EMPTY_HOLD;
	set_status(a2_ptr->status, ST_DEFEND);
	statval = ST_DEFEND;
	errormsg("Defaulting grouped status to Defend");
      } else if ((count >= 20) ||
		 ((ival = a2_ptr->leader) == EMPTY_HOLD) ||
		 ((a2_ptr = armybynum(ival)) == NULL)) {
	set_status(a2_ptr->status, ST_DEFEND);
	statval = ST_DEFEND;
	errormsg("Defaulting grouped status to Defend");
      } else {
	count++;
	statval = unit_status(a2_ptr->status);
      }
    }

  }
  return(statval);
}

/*
 * real_stat - Get the effective operational status of an army unit
 *
 * Determines the actual status that governs unit behavior, following
 * group leadership chains if the unit is grouped under another leader.
 * Provides the status used for combat, movement, and other decisions.
 *
 * Parameters:
 *   a1_ptr - Pointer to army unit (must not be NULL)
 *
 * Returns:
 *   Effective status governing unit behavior
 *
 * Side Effects:
 *   - None (read-only status resolution)
 *
 * Notes:
 *   - Returns unit's own status if not grouped
 *   - Follows leadership chain if unit has ST_GROUPED status
 *   - Used throughout game logic for status-dependent decisions
 *   - Simpler interface than group_stat for direct unit queries
 */
/* REAL_STAT -- Return the real status of the army unit */
int
real_stat PARM_1(ARMY_PTR, a1_ptr)
{
  int hold;

  /* check the actual status of the unit */
  if ((hold = unit_status(a1_ptr->status)) == ST_GROUPED) {
    hold = group_stat(a1_ptr->leader);
  }
  return(hold);
}

/*
 * defaultunit - Select the optimal default unit type for nation drafting
 *
 * Determines the best available unit type for a nation to draft based on
 * their magical powers, race characteristics, and available unit types.
 * Prioritizes more powerful units when the nation has appropriate powers.
 *
 * Parameters:
 *   None (uses global ntn_ptr for nation context)
 *
 * Returns:
 *   Unit type ID for default drafting, or DEFAULT_ARMYTYPE if none found
 *
 * Side Effects:
 *   - None (read-only unit type selection)
 *
 * Notes:
 *   - Vampire nations prefer zombies if available
 *   - Orc nations prefer uruk-hai and olog-hai based on powers
 *   - Archery nations prefer archers
 *   - Nomad nations prefer light cavalry
 *   - Falls back to infantry if no special preferences match
 *   - Essential for automated unit recruitment decisions
 */
/* DEFAULTUNIT -- Selection of the default unit for drafting */
int
defaultunit PARM_0(void)
{
  int hold = -1;

  /* check it */
  if (ntn_ptr != NULL) {

    /* check for zombies first */
    if (WIZ_MAGIC(MW_VAMPIRE) &&
	((hold = unitbyname("Zombie")) != -1)) {
      return(hold);
    }

    /* check for better orcish types */
    if (MIL_MAGIC(MM_OGRE)) {
      if (CIV_MAGIC(MC_BREEDER) &&
	  ((hold = unitbyname("Olog-Hai")) != -1)) {
	return(hold);
      }
      if ((hold = unitbyname("Uruk-Hai")) != -1) {
	return(hold);
      }
    }

    /* check for other types */
    if (MIL_MAGIC(MM_ARCHERY) &&
	((hold = unitbyname("Archers")) != -1)) {
      return(hold);
    }
    if (MIL_MAGIC(MM_ORC) &&
	((hold = unitbyname("Orcs")) != -1)) {
      return(hold);
    }
    if (n_isnomad(ntn_ptr->active) &&
	((hold = unitbyname("Lt_Cavalry")))) {
      return(hold);
    }
  }

  /* default of infantry is all that is left */
  if ((hold = unitbyname("Infantry")) != -1) {
    return(hold);
  }
  return(DEFAULT_ARMYTYPE);
}

/*
 * getruler - Get the ruler unit type for a nation class
 *
 * Looks up the appropriate ruler unit type for the specified nation class
 * from the nation class configuration table. Rulers are the highest-level
 * leaders available to each nation type.
 *
 * Parameters:
 *   class - Nation class index (0 to nclass_number-1)
 *
 * Returns:
 *   Unit type ID for the ruler, or DEFAULT_RULERTYPE if not found
 *
 * Side Effects:
 *   - May write warning messages to fupdate file
 *
 * Notes:
 *   - Each nation class has a predefined ruler type
 *   - Falls back to DEFAULT_RULERTYPE if ruler not found
 *   - Logs warnings for missing ruler types
 *   - Used during nation initialization and leader creation
 */
/* GETRULER -- Return the major leader for the national class */
int
getruler PARM_1(int, class)
{
  int lead_val;

  /* check input */
  if ((class < 0) ||
      (class >= nclass_number)) {
    fprintf(fupdate, "MAJOR ERROR: unknown nation class %d\n", class);
    return(-1);
  }

  /* now get the return value */
  if ((lead_val = unitbyname(nclass_list[class].rulertype)) == -1) {
    lead_val = DEFAULT_RULERTYPE;
    fprintf(fupdate, "Warning: could not find national ruler: %s\n",
	    nclass_list[class].rulertype);
  }
  return(lead_val);
}

/*
 * getminleader - Get the minor leader unit type for a nation class
 *
 * Looks up the appropriate minor leader unit type for the specified nation
 * class from the nation class configuration table. Minor leaders are
 * mid-level commanders below the main ruler.
 *
 * Parameters:
 *   class - Nation class index (0 to nclass_number-1)
 *
 * Returns:
 *   Unit type ID for the minor leader, or DEFAULT_LEADERTYPE if not found
 *
 * Side Effects:
 *   - May write warning messages to fupdate file
 *
 * Notes:
 *   - Each nation class has a predefined minor leader type
 *   - Falls back to DEFAULT_LEADERTYPE if leader not found
 *   - Logs warnings for missing leader types
 *   - Used for creating secondary commanders and group leaders
 */
/* GETMINLEADER -- Return the minor leader for the national class */
int
getminleader PARM_1(int, class)
{
  int lead_val;

  /* check input */
  if ((class < 0) ||
      (class >= nclass_number)) {
    fprintf(fupdate, "MAJOR ERROR: unknown nation class %d\n", class);
    return(-1);
  }

  /* now get the return value */
  if ((lead_val = unitbyname(nclass_list[class].minleadtype)) == -1) {
    fprintf(fupdate, "Warning: could not find minor leader: %s\n",
	    nclass_list[class].minleadtype);
    lead_val = DEFAULT_LEADERTYPE;
  }
  return(lead_val);
}

/*
 * cargo_holds - Calculate total cargo capacity from ship information word
 *
 * Computes the total cargo hold capacity from a bit-packed ship information
 * word by summing holds across all ship sizes. Light ships have fewer holds
 * than heavy ships, with capacity scaling by size class.
 *
 * Parameters:
 *   shipinfo - Bit-packed ship information (0 = no ships)
 *
 * Returns:
 *   Total number of cargo holds across all ships
 *
 * Side Effects:
 *   - None (pure calculation function)
 *
 * Notes:
 *   - Light ships = 1 hold each, Medium = 2 holds, Heavy = 3 holds
 *   - Quick return of 0 if no ships present
 *   - Used for capacity calculations and resource transport planning
 *   - Works with bit-packed ship data format
 */
/* CARGO_HOLDS -- The number of holds in the given ship information */
int
cargo_holds PARM_1(int, shipinfo)
{
  register int i;
  int count = 0;

  /* check for quick out */
  if (shipinfo == 0) return(count);

  /* go through the information counting the number of holds */
  for (i = N_LIGHT; i <= N_HEAVY; i++) {
    count += (i + 1) * N_CNTSHIPS(shipinfo, i);
  }
  return(count);
}

/*
 * navy_holds - Get cargo capacity for a specific ship class in a fleet
 *
 * Returns the total cargo hold capacity for a specific ship class
 * (galleys, warships, etc.) within a naval fleet. Delegates to
 * cargo_holds for the actual calculation.
 *
 * Parameters:
 *   n1_ptr - Pointer to navy unit
 *   classnum - Ship class index (0 to NSHP_NUMBER-1)
 *
 * Returns:
 *   Number of cargo holds for the specified ship class
 *
 * Side Effects:
 *   - None (read-only capacity query)
 *
 * Notes:
 *   - Simple wrapper around cargo_holds for specific ship class
 *   - Used when calculating capacity for specific ship types
 *   - Ship classes include galleys, warships, etc.
 */
/* NAVY_HOLDS -- The number of holds among a class of ships in a given fleet */
int
navy_holds PARM_2(NAVY_PTR, n1_ptr, int, classnum)
{
  return( cargo_holds(n1_ptr->ships[classnum]) );
}

/*
 * check_leader - Verify if a unit is actively leading other units
 *
 * Checks whether the specified unit is actually functioning as a group
 * leader by scanning for other units in the same sector that are grouped
 * under this unit's command.
 *
 * Parameters:
 *   idnum - Army ID of potential leader to check
 *
 * Returns:
 *   TRUE if unit is actively leading others, FALSE otherwise
 *
 * Side Effects:
 *   - None (read-only leadership verification)
 *
 * Notes:
 *   - Searches same sector for units with this leader ID
 *   - Unit must exist and be in a valid location
 *   - Returns FALSE if unit doesn't exist or has no followers
 *   - Used to validate leadership status for game mechanics
 */
/* CHECK_LEADER -- return TRUE or FALSE if a leader is really leading */
int
check_leader PARM_1(int, idnum)
{
  ARMY_PTR a1_ptr, a2_ptr;

  if ((a1_ptr = armybynum(idnum)) == NULL) {
    return (FALSE);
  }

  /* find the first unit in the same sector */
  for (a2_ptr = ntn_ptr->army_list;
       a2_ptr != NULL;
       a2_ptr = a2_ptr->next) {
    if ((a2_ptr->xloc == a1_ptr->xloc) &&
	(a2_ptr->yloc == a1_ptr->yloc)) break;
  }

  /* now go through all the units */
  for (; a2_ptr != NULL; a2_ptr = a2_ptr->nrby) {
    if ((unit_status(a2_ptr->status) == ST_GROUPED) &&
	(a2_ptr->leader == idnum)) return(TRUE);
  }
  return(FALSE);
}

/*
 * army_mvpts - Calculate movement points for an army unit
 *
 * Computes the movement points available to an army unit based on nation
 * movement capability, unit type speed, unit status, and group leadership
 * constraints. Group leaders are limited by their slowest subordinates.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation owning the unit (must not be NULL)
 *   a1_ptr - Pointer to army unit (must not be NULL)
 *
 * Returns:
 *   Movement points available for the unit, 0 if invalid parameters
 *
 * Side Effects:
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Formula: (nation_maxmove * unit_speed * speed_status) / 20
 *   - Group leaders limited by slowest grouped subordinate
 *   - SPD_STUCK status results in 0 movement
 *   - Leaders get slight movement penalty when managing groups
 *   - Essential for movement planning and turn management
 */
/* ARMY_MVPTS -- Return the starting movement for the specified unit */
int
army_mvpts PARM_2(NTN_PTR, n1_ptr, ARMY_PTR, a1_ptr)
{
  ARMY_PTR a2_ptr;
  int hold, hld2, speed_val;

  /* check input */
  if ((n1_ptr == NULL) || (a1_ptr == NULL)) return(0);

  /* now perform the calculation */
  if ((speed_val = unit_speed(a1_ptr->status)) == SPD_STUCK) {
    speed_val = 0;
  } else {
    speed_val++;
  }
  hold = (n1_ptr->maxmove *
	  ainfo_list[a1_ptr->unittype].speed *
	  speed_val) / 20;

  /* minimize the movement if grouped */
  if (unit_leading(a1_ptr->status)) {

    /* go through all of the units in the sector */
    for (a2_ptr = n1_ptr->army_list;
	 a2_ptr != NULL;
	 a2_ptr = a2_ptr->next) {
      if ((a2_ptr->xloc == a1_ptr->xloc) &&
	  (a2_ptr->yloc == a1_ptr->yloc)) break;
    }

    /* cover all of the units */
    for (; a2_ptr != NULL; a2_ptr = a2_ptr->nrby) {
      if (a2_ptr == a1_ptr) continue;
      if (a2_ptr->leader != a1_ptr->armyid) continue;
      if (unit_status(a2_ptr->status) != ST_GROUPED) continue;

      hld2 = army_mvpts(n1_ptr, a2_ptr);
      if (a_isleader(a2_ptr->unittype)) {
	if (hld2 < hold) {
	  hold = hld2;
	}
      } else {
	if (hld2 + 2 < hold) {
	  hold = hld2 + 2;
	}
      }
    }
  }
  return(hold);
}

/* TAKE_VALUE -- Status adjustment for land capture */
static int
take_value PARM_1(int, statval)
{
  int hold = 1;

  /* check the status */
  statval = unit_status(statval);

  /* check for non-holding statuses */
  if (nohold_stat(statval)) {
    /* no value here */
    hold = 0;
  } else if (holdit_stat(statval)) {
    /* double trouble */
    hold = 2;
  }
  return(hold);
}

/*
 * army_captsize - Calculate unit strength for sector capture operations
 *
 * Determines the effective strength of an army unit for capturing or holding
 * territory, considering unit type, status, efficiency, and capture capabilities.
 * Different unit types contribute different amounts to territorial control.
 *
 * Parameters:
 *   a1_ptr - Pointer to army unit (must not be NULL)
 *   takefigure - If TRUE, apply status and efficiency modifiers
 *
 * Returns:
 *   Effective capture strength, 0 if unit cannot capture territory
 *
 * Side Effects:
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Scout units cannot capture territory (return 0)
 *   - Leaders count as 1 regardless of strength
 *   - Normal units use strength * capture_value / 10
 *   - Status affects capture ability (attack vs defend vs reserve)
 *   - Efficiency reduces effective strength when takefigure is TRUE
 *   - Essential for territorial control calculations
 */
/* ARMY_CAPTSIZE -- Strength of the unit in relation to sector taking */
long
army_captsize PARM_2(ARMY_PTR, a1_ptr, int, takefigure)
{
  long lvalue;
  int multval, statval;

  /* check the input */
  if ((a1_ptr == NULL) ||
      (a1_ptr->strength <= 0)) return(0);

  /* scouting units are right out */
  if (a_isscout(a1_ptr->unittype)) {
    return(0);
  }

  /* get the status of the unit */
  statval = real_stat(a1_ptr);
  if (takefigure) {
    if ((multval = take_value(statval)) == 0) {
      return(0);
    }
  } else {
    multval = 1;
  }

  /* differentiate on unit types */
  if (a_isleader(a1_ptr->unittype)) {
    /* really just a single entity */
    lvalue = 1;
  } else {
    lvalue = (a1_ptr->strength *
	      (int) ainfo_list[a1_ptr->unittype].capt_val) / 10;
  }

  /* find results */
  if (takefigure) {
    /* now figure in any adjustment */
    return((lvalue * multval * a1_ptr->efficiency) / 100);
  }
  return(lvalue * multval);
}

/*
 * army_worksize - Calculate unit strength for construction and work projects
 *
 * Determines the effective work capacity of an army unit for building
 * projects, construction, and other labor-intensive activities. Different
 * unit types have varying work capabilities.
 *
 * Parameters:
 *   a1_ptr - Pointer to army unit (must not be NULL)
 *
 * Returns:
 *   Effective work capacity, 0 if invalid unit
 *
 * Side Effects:
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Leaders contribute minimal work capacity (1 unit)
 *   - Normal units use strength * work_value / 10
 *   - Efficiency affects final work output
 *   - Used for construction time and labor calculations
 *   - Essential for building and engineering projects
 */
/* ARMY_WORKSIZE -- Strength of the unit in relation to building potential */
long
army_worksize PARM_1(ARMY_PTR, a1_ptr)
{
  long lvalue;

  /* check the input */
  if ((a1_ptr == NULL) ||
      (a1_ptr->strength <= 0)) return(0);

  /* differentiate on unit types */
  if (a_isleader(a1_ptr->unittype)) {
    /* really just a single entity */
    lvalue = 1;
  } else {
    lvalue = (a1_ptr->strength *
	      (int) ainfo_list[a1_ptr->unittype].work_val) / 10;
  }

  /* now figure in any adjustment */
  return((lvalue * a1_ptr->efficiency) / 100);
}

/*
 * rand_monstunit - Select a random monster unit within strength limits
 *
 * Randomly selects an available monster unit type that doesn't exceed the
 * specified maximum strength requirement. Used for random monster encounters
 * and automated monster summoning with appropriate power levels.
 *
 * Parameters:
 *   maxstrength - Maximum strength limit for selected monster
 *
 * Returns:
 *   Unit type ID of randomly selected monster
 *
 * Side Effects:
 *   - None (read-only selection using random numbers)
 *
 * Notes:
 *   - Only considers monster units (a_ismonster)
 *   - Excludes non-draftable monsters (a_nodraft)
 *   - Filters by minimum strength requirements
 *   - Uses two-pass algorithm: count valid options, then select randomly
 *   - Essential for balanced random encounters
 */
/* RAND_MONSTUNIT -- Give random monster unit as limited by size */
int
rand_monstunit PARM_1(int, maxstrength)
{
  int count, number = 0;

  /* round 'em up */
  for (count = 0; count < num_armytypes; count++) {
    if (a_ismonster(count) &&
	!a_nodraft(count) &&
	(ainfo_list[count].minsth <= maxstrength)) {
      /* got one */
      number++;
    }
  }

  /* now find a random selection */
  number = rand_val(number);
  for (count = 0; count < num_armytypes; count++) {
    if (a_ismonster(count) &&
	!a_nodraft(count) &&
	(ainfo_list[count].minsth <= maxstrength)) {
      /* got one */
      if (number == 0) break;
      number--;
    }
  }
  return(count);
}

/*
 * mtrls_load - Calculate total weight of a set of raw materials
 *
 * Computes the total weight of materials by multiplying each material
 * quantity by its weight and summing the results. Uses the global
 * materials information table for weight lookups.
 *
 * Parameters:
 *   m_ptr - Array of material quantities (must not be NULL)
 *
 * Returns:
 *   Total weight as long integer, 0 if invalid parameters
 *
 * Side Effects:
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Iterates through all MTRLS_NUMBER material types
 *   - Weight calculation: quantity * mtrls_info[i].weight
 *   - Used for transport capacity and logistical calculations
 *   - Essential for determining cargo load limits
 */
/* MTRLS_LOAD -- Return the value of the load of a set of raw materials */
long
mtrls_load PARM_1(itemtype *, m_ptr)
{
  int i;
  long lng_sum = 0L;

  /* check the input */
  if (m_ptr == NULL) {
    return(lng_sum);
  }

  for (i = 0; i < MTRLS_NUMBER; i++) {
    lng_sum += (long) m_ptr[i] * mtrls_info[i].weight;
  }
  return(lng_sum);
}

/*
 * army_load - Calculate the transport load weight of an army unit
 *
 * Determines the effective load weight of an army unit for transport
 * calculations. Leaders count as their group size when leading, monsters
 * use capture value scaling, and normal units use their strength directly.
 *
 * Parameters:
 *   a_ptr - Pointer to army unit (must not be NULL)
 *
 * Returns:
 *   Transport load weight as long integer, 0 if invalid parameters
 *
 * Side Effects:
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Leaders: group size if leading, 1 if not leading
 *   - Monsters: strength * capture_value / 10
 *   - Normal units: strength value directly
 *   - Used for naval and caravan transport capacity planning
 *   - Essential for determining unit transport requirements
 */
/* ARMY_LOAD -- The amount of load within an army unit */
long
army_load PARM_1(ARMY_PTR, a_ptr)
{
  long lng_sum = 0L;
  int utype;

  /* check the input */
  if (a_ptr == NULL) {
    return(lng_sum);
  }

  /* now compute it */
  utype = a_ptr->unittype;
  if (a_isleader(utype)) {
    if (unit_leading(a_ptr->status)) {
      lng_sum = men_ingroup(a_ptr->armyid, FALSE);
    } else {
      lng_sum = 1;
    }
  } else if (a_ismonster(utype)) {
    lng_sum = (a_ptr->strength *
	       (long)ainfo_list[utype].capt_val) / 10L;
  } else {
    lng_sum = a_ptr->strength;
  }

  /* return the results */
  return(lng_sum);
}

/*
 * cvn_load - Calculate the transport load weight of a caravan unit
 *
 * Determines the effective load weight of a caravan unit for transport
 * calculations. The load is based on caravan size multiplied by the
 * standard wagon-to-caravan ratio.
 *
 * Parameters:
 *   v_ptr - Pointer to caravan unit (must not be NULL)
 *
 * Returns:
 *   Transport load weight as long integer, 0 if invalid parameters
 *
 * Side Effects:
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Load calculation: size * WAGONS_IN_CVN
 *   - Currently only accounts for wagon weight, not materials carried
 *   - Future enhancement needed to include materials weight
 *   - Used for naval transport capacity planning
 *   - Essential for determining caravan transport requirements
 */
/* CVN_LOAD -- The amount of load within a caravan unit;
               This will need to later have mtrls weight added */
long
cvn_load PARM_1(CVN_PTR, v_ptr)
{
  long lng_sum = 0L;

  /* check the input */
  if (v_ptr == NULL) {
    return(lng_sum);
  }

  /* now compute it */
  lng_sum = ((long) v_ptr->size) * WAGONS_IN_CVN;

  /* return the results */
  return(lng_sum);
}

/*
 * army_shipleader - Find the leader of a unit when onboard ships
 *
 * Recursively traces through group leadership chains to find the top-level
 * leader when units are onboard ships. Handles grouped units by following
 * leader pointers and fixes circular references automatically.
 *
 * Parameters:
 *   a1_ptr - Pointer to army unit to check (must not be NULL)
 *
 * Returns:
 *   Army ID of ship leader if onboard, EMPTY_HOLD otherwise
 *
 * Side Effects:
 *   - May fix circular leader references by setting status to ST_DEFEND
 *   - May modify leader field to EMPTY_HOLD for self-referential units
 *
 * Notes:
 *   - ST_GROUPED units: recursively find leader's shipleader
 *   - ST_ONBOARD units: return their own army ID
 *   - Other statuses: return EMPTY_HOLD (not onboard)
 *   - Automatically fixes self-referential leader pointers
 *   - Essential for naval transport management
 */
/* ARMY_SHIPLEADER -- Return leader if onboard otherwise EMPTY_HOLD */
int
army_shipleader PARM_1(ARMY_PTR, a1_ptr)
{
  int hold = EMPTY_HOLD;

  /* check the input */
  if (a1_ptr == NULL) return(hold);

  /* check the unit status */
  switch (unit_status(a1_ptr->status)) {
  case ST_GROUPED:
    /* find the leader's shipleader */
    if (a1_ptr->armyid == a1_ptr->leader) {
      set_status(a1_ptr->status, ST_DEFEND);
      a1_ptr->leader = EMPTY_HOLD;
      hold = a1_ptr->armyid;
    } else {
      hold = army_shipleader(armybynum(a1_ptr->leader));
    }
    break;
  case ST_ONBOARD:
    hold = a1_ptr->armyid;
    break;
  default:
    /* this guy ain't onboard anything */
    break;
  }
  return(hold);
}

/*
 * wallpat_check - Check sector for neighboring walls owned by specified nation
 *
 * Helper function for wall_patrol that examines a sector to determine if it
 * contains a wall owned by the specified nation. Uses global variables to
 * communicate with the calling function through map_loop iteration.
 *
 * Parameters:
 *   x - X coordinate of sector to check
 *   y - Y coordinate of sector to check
 *
 * Returns:
 *   None (results stored in global_long)
 *
 * Side Effects:
 *   - Sets global_long to TRUE if owned wall found
 *   - Uses global_int as nation ID for ownership comparison
 *
 * Notes:
 *   - Static helper function for wall_patrol only
 *   - Checks for MAJ_WALL designation and matching ownership
 *   - Part of wall patrol validation system
 *   - Uses global variables for map_loop callback interface
 */
/* WALLPAT_CHECK -- check for neighboring walls to patrol */
static void
wallpat_check PARM_2(int, x, int, y)
{
  /* must have an owned wall nearby */
  if ((sct[x][y].owner == global_int) &&
      (major_desg(sct[x][y].designation) == MAJ_WALL)) {
    global_long = TRUE;
  }
}

/*
 * wall_patrol - Determine if an army unit is acting as a wall patrol
 *
 * Validates whether an army unit is functioning as a wall patrol by checking
 * multiple requirements: garrison status, stuck speed, proper location on a
 * wall, correct ownership, and presence of neighboring walls.
 *
 * Parameters:
 *   cntry - Nation ID that should own the wall
 *   a1_ptr - Pointer to army unit to check (must not be NULL)
 *
 * Returns:
 *   TRUE if unit is a valid wall patrol, FALSE otherwise
 *
 * Side Effects:
 *   - Uses global variables for map_loop communication
 *   - Temporarily modifies global_long and global_int
 *
 * Notes:
 *   - Unit must have ST_GARRISON status and SPD_STUCK speed
 *   - Must be located on a wall (MAJ_WALL) owned by specified nation
 *   - Must have at least one adjacent wall for patrol connectivity
 *   - Uses map_loop with wallpat_check to scan neighboring sectors
 *   - Essential for wall defense and patrol validation
 */
/* WALL_PATROL -- Is an army unit acting as a wall patrol? */
int
wall_patrol PARM_2(int, cntry, ARMY_PTR, a1_ptr)
{
  SCT_PTR s1_ptr;

  /* check information */
  if ((a1_ptr == NULL) ||
      !XY_INMAP(a1_ptr->xloc, a1_ptr->yloc)) {
    return(FALSE);
  }

  /* only garrison status */
  if ((unit_status(a1_ptr->status) != ST_GARRISON) ||
      (unit_speed(a1_ptr->status) != SPD_STUCK)) {
    return(FALSE);
  }

  /* now check the sector */
  if (((s1_ptr = &(sct[a1_ptr->xloc][a1_ptr->yloc])) == NULL) ||
      (s1_ptr->owner != cntry)) {
    return(FALSE);
  }

  /* now check the designation */
  if (major_desg(s1_ptr->designation) != MAJ_WALL) {
    return(FALSE);
  }

  /* finally check for adjoining walls */
  global_long = FALSE;
  global_int = cntry;
  map_loop(a1_ptr->xloc, a1_ptr->yloc, 1, wallpat_check);
  if (global_long == FALSE) {
    return(FALSE);
  }

  /* what do ya know... it is a wall patrol */
  return(TRUE);
}

/*
 * set_armycosts - Calculate unit recruitment costs by type and size
 *
 * Computes the material costs required to recruit a specified number of units
 * of a given type. Applies nation-specific modifiers such as sapper discounts,
 * mercenary cost adjustments, and magical cost modifications.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation (for power checks and modifiers)
 *   cost_ptr - Array to store calculated costs (must not be NULL)
 *   type - Unit type index (0 to num_armytypes-1)
 *   size - Number of units to recruit (must be >= 0)
 *
 * Returns:
 *   None (results stored in cost_ptr array)
 *
 * Side Effects:
 *   - Modifies cost_ptr array with calculated material costs
 *   - Uses global ntn_ptr for sapper and mercenary calculations
 *   - Temporarily switches nation context for mercenary cost rates
 *
 * Notes:
 *   - Base costs: talons, metals from unit type, food from supply level
 *   - Sapper units get 50% discount on talons/metals if nation has sapper power
 *   - Mercenary units use dynamic cost rates based on market conditions
 *   - Applies sector and magical cost adjustments
 *   - Essential for recruitment planning and resource management
 */
/* SET_ARMYCOSTS -- Set the costs of the given unit of given size */
void
set_armycosts PARM_4(NTN_PTR, n1_ptr, itemtype *, cost_ptr,
		     int, type, int, size)
{
  int cnt;

  /* check the input */
  if ((cost_ptr == NULL) ||
      (size < 0) ||
      (type < 0) ||
      (type >= num_armytypes)) {
    return;
  }

  /* initialize it */
  for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
    cost_ptr[cnt] = (itemtype) 0;
  }

  /* figure out the costs */
  cost_ptr[MTRLS_TALONS] = size * ainfo_list[type].tal_enlist;
  cost_ptr[MTRLS_METALS] = size * ainfo_list[type].ore_enlist;
  cost_ptr[MTRLS_FOOD] = size * conq_supply_level;

  /* check for sapper */
  if (a_sapper(type) &&
      (ntn_ptr != NULL) &&
      MAGIC(n1_ptr->powers[MAG_MILITARY], MM_SAPPER)) {
    cost_ptr[MTRLS_TALONS] /= 2;
    cost_ptr[MTRLS_METALS] /= 2;
  }

  /* check for mercs */
  if (a_ismerc(type)) {
    NTN_PTR nhold_ptr = ntn_ptr;
    ntn_ptr = n1_ptr;
    cost_ptr[MTRLS_TALONS] *= merc_costrate();
    cost_ptr[MTRLS_TALONS] /= 100;
    ntn_ptr = nhold_ptr;
  }

  /* check sector and magical adjustments */
  sct_cost_adjust(0, sct_ptr, cost_ptr);
  mgk_cost_adjust(1, cost_ptr);
}

/*
 * set_upgcosts - Calculate unit upgrade costs for transforming unit types
 *
 * Computes the material costs required to upgrade an existing army unit to
 * a new unit type. Considers differences in recruitment costs, support costs,
 * training requirements, and unit-specific modifiers.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation (for power checks and modifiers)
 *   a1_ptr - Pointer to army unit being upgraded (must not be NULL)
 *   cost_ptr - Array to store calculated upgrade costs (must not be NULL)
 *   newtype - Target unit type index (0 to num_armytypes-1)
 *
 * Returns:
 *   None (results stored in cost_ptr array)
 *
 * Side Effects:
 *   - Modifies cost_ptr array with calculated upgrade costs
 *   - Temporarily modifies a1_ptr->unittype during cost calculations
 *   - Restores original unittype after calculations
 *
 * Notes:
 *   - Only charges difference in recruitment costs between old and new types
 *   - Half-men units: only half upgrade, affecting cost calculations
 *   - Training units (a_needtrain) have lower upgrade costs (50% vs 20%)
 *   - Includes differential support costs between old and new types
 *   - Sapper units get 50% discount if nation has sapper power
 *   - Applies sector and magical cost adjustments
 *   - Essential for unit advancement and military planning
 */
/* SET_UPGCOSTS -- Set the to upgrade to the given unit of given size */
void
set_upgcosts PARM_4(NTN_PTR, n1_ptr, ARMY_PTR, a1_ptr,
		    itemtype *, cost_ptr, int, newtype)
{
  itemtype old_cost[MTRLS_NUMBER], new_cost[MTRLS_NUMBER];
  itemtype diff;
  int oldtype, size; 
  int cnt, divval;

  /* check the input */
  if ((cost_ptr == NULL) ||
      (newtype < 0) ||
      (newtype >= num_armytypes)) {
    return;
  }

  /* initialize it */
  for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
    cost_ptr[cnt] = (itemtype) 0;
  }
  if ((n1_ptr == NULL) ||
      (a1_ptr == NULL)) return;

  /* set the figures */
  size = a1_ptr->strength;
  oldtype = a1_ptr->unittype;
  army_support(a1_ptr, &(old_cost[0]), a1_ptr->supply);
  a1_ptr->unittype = newtype;
  army_support(a1_ptr, &(new_cost[0]), a1_ptr->supply);
  a1_ptr->unittype = oldtype;

  /* check for half recruit type units */
  if (a_halfmen(oldtype)) {
    /* only half of the men will upgrade */
    size++;
    size /= 2;
  }

  /* figure out the costs */
  if ((diff = ainfo_list[newtype].tal_enlist -
       ainfo_list[oldtype].tal_enlist) > 0) {
    cost_ptr[MTRLS_TALONS] = size * diff;
  }
  if ((diff = ainfo_list[newtype].ore_enlist -
       ainfo_list[oldtype].ore_enlist) > 0) {
    cost_ptr[MTRLS_METALS] = size * diff;
  }

  /* determine percentage of added expenses */
  if (a_needtrain(newtype)) {
    divval = 2;
  } else {
    divval = 5;
  }

  /* now add it in */
  cost_ptr[MTRLS_TALONS] += (size * ainfo_list[newtype].tal_enlist) / divval;
  cost_ptr[MTRLS_METALS] += (size * ainfo_list[newtype].ore_enlist) / divval;
  if (cost_ptr[MTRLS_TALONS] > size * ainfo_list[newtype].tal_enlist) {
    cost_ptr[MTRLS_TALONS] = size * ainfo_list[newtype].tal_enlist;
  }
  if (cost_ptr[MTRLS_METALS] > size * ainfo_list[newtype].ore_enlist) {
    cost_ptr[MTRLS_METALS] = size * ainfo_list[newtype].ore_enlist;
  }

  /* check for sapper */
  if (a_sapper(newtype) &&
      (ntn_ptr != NULL) &&
      MAGIC(n1_ptr->powers[MAG_MILITARY], MM_SAPPER)) {
    cost_ptr[MTRLS_TALONS] /= 2;
    cost_ptr[MTRLS_METALS] /= 2;
  }

  /* figure in support costs */
  for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
    if (new_cost[cnt] > old_cost[cnt]) {
      cost_ptr[cnt] += (new_cost[cnt] - old_cost[cnt]);
    }
  }

  /* check sector and magical adjustments */
  sct_cost_adjust(0, sct_ptr, cost_ptr);
  mgk_cost_adjust(1, cost_ptr);
}

/*
 * max_numunit - Calculate maximum number of units that can be recruited
 *
 * Determines the maximum number of units of a specified type that can be
 * recruited based on available resources, population, spell points (for monsters),
 * and recruitment costs. Handles both normal units and monster summoning.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation (must not be NULL)
 *   c1_ptr - Pointer to city for recruitment (NULL for remote recruitment)
 *   utype - Unit type index (0 to num_armytypes-1)
 *   spts - Spell points available (for monster summoning)
 *
 * Returns:
 *   Maximum number of units that can be recruited, 0 if impossible
 *
 * Side Effects:
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Monster units: limited by spell points and power requirements
 *   - Normal units: limited by population, materials, and recruitment costs
 *   - Mercenary units: limited by global mercenary availability
 *   - Half-men units: double population efficiency (2 units per person)
 *   - Remote recruitment: requires spy network and resource availability
 *   - Shrine bonuses reduce monster summoning costs
 *   - Returns 1 for monsters if spell points sufficient, 0 otherwise
 *   - Essential for recruitment planning and strategic resource allocation
 */
/* MAX_NUMUNIT -- Return the value of the number of men possible */
long
max_numunit PARM_4(NTN_PTR, n1_ptr, CITY_PTR, c1_ptr, int, utype, int, spts)
{
  itemtype cost_mtrls[MTRLS_NUMBER];
  ITEM_PTR i_ptr;
  long hold = 0, tval;
  int cnt;

  /* check the input */
  if ((n1_ptr == NULL)) return(hold);
  if ((utype < 0) ||
      (utype >= num_armytypes)) return(hold);

  /* check the monster skills */
  if (a_ismonster(utype)) {
    /* check for the spell points */
    int ut_val = 0;
    extern int shrine_helped;

    cnt = (int) ainfo_list[utype].tal_enlist;
    if (spts < 0) {
      cnt *= 2;
      spts = -spts;
    }
    if ((ut_val = utype_mayuse(n1_ptr, utype)) <= 0) {
      ut_val = (2 + ut_val) * 2;
    }
    cnt *= (1 + ut_val);
    cnt /= 2;
    if ((shrine_helped) && (cnt > 0)) {
      cnt *= (100 - shrine_helped * 2);
      cnt /= 100;
      if (cnt <= 0) cnt = 1;
    }
    return(cnt <= spts);
  }

  /* determine the expenses */
  if (city_ptr == NULL) return(hold);
  set_armycosts(n1_ptr, &(cost_mtrls[0]), utype, 1);

  /* now check the pricing */
  if (c1_ptr != citybyloc(n1_ptr, c1_ptr->xloc, c1_ptr->yloc)) {
    /* check if the city is developed enough to have a spy */
    if ((major_desg(sct[c1_ptr->xloc][c1_ptr->yloc].designation) < MAJ_TOWN) ||
	(sct[c1_ptr->xloc][c1_ptr->yloc].people < 500) ||
	(c1_ptr->i_people < 0)) {
      return(hold);
    }

    /* find resources from neigboring region */
    if ((i_ptr = find_resources(c1_ptr->xloc, c1_ptr->yloc,
				    FALSE)) == NULL) return 0;

    /* compare */
    for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
      if (cost_mtrls[cnt] > 0) {
	if (i_ptr->mtrls[cnt] < cost_mtrls[cnt]) return 0;
      }
    }
    return 1;

  } else {
    /* mercs? */
    if (a_ismerc(utype)) {
      hold = (MERCMEN / (2 * MAXNTN)) - conq_mercsused;
      if (hold < 0) {
	hold = 0;
      }
    } else {
      hold = c1_ptr->i_people;
      if (a_halfmen(utype)) hold *= 2;
      if (sct[c1_ptr->xloc][c1_ptr->yloc].people < hold) {
	hold = sct[c1_ptr->xloc][c1_ptr->yloc].people;
      }
    }

    /* check costs */
    for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
      if (cost_mtrls[cnt] > 0) {

	/* determine if this is a limiting factor */
	tval = (long) (c1_ptr->c_mtrls[cnt] / cost_mtrls[cnt]);
	if (tval < hold) {
	  hold = tval;
	}

      }
    }
  }
  return(hold);
}

/*
 * utype_mayuse - Check if a nation can use a specific unit type
 *
 * Validates whether a nation has the required magical powers and abilities
 * to recruit or use a specific unit type. Different unit types require
 * different combinations of military, wizardry, and civilian powers.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation to check (must not be NULL unless god)
 *   utype - Unit type index to validate (0 to num_armytypes-1)
 *
 * Returns:
 *   >0 if nation can use unit type
 *   0 if nation cannot use unit type
 *   <0 for monsters: power deficit (more powers needed)
 *
 * Side Effects:
 *   - None (read-only power validation)
 *
 * Notes:
 *   - God nations can use any unit type
 *   - Normal units: require exact power matches for military/wizardry/civilian
 *   - Monster units: use complex power counting system
 *   - Monsters with summon power: different cost calculation
 *   - Returns power deficit for monsters (negative = missing powers)
 *   - Returns power surplus for monsters with summon (positive = easier)
 *   - Essential for recruitment validation and unit availability
 */
/* UTYPE_MAYUSE -- Check if the given unit type may be used by the nation */
int
utype_mayuse PARM_2(NTN_PTR, n1_ptr, int, utype)
{
  int count;

  /* check the input */
  if ((utype < 0) ||
      (utype >= num_armytypes)) {
    return(0);
  }
  if ((ntn_ptr == NULL) ||
      (is_god == TRUE)) {
    return(is_god);
  }

  /* check the powers */
  if (a_ismonster(utype)) {
    int num_have = 0, num_need = 0;

    /* count how many powers are needed */
    for (count = 0; count < MAG_NUMBER; count++) {
      num_need += num_bits_on(ainfo_list[utype].pow_need[count]);
    }

    /* check how many of them they have */
    for (count = 0; count < MAG_NUMBER; count++) {
      num_have += num_bits_on(n1_ptr->powers[count] &
			      ainfo_list[utype].pow_need[count]);
    }

    /* check for the summon power */
    if (!MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_SUMMON)) {
      return(num_have - num_need);
    }

    /* free or at cost? */
    if (num_need == 0) return(1);
    return(num_need - num_have + 1);

  } else if (a_isnormal(utype)) {

    /* confirm that the nation possesses the needed powers */
    if (MAGIC(n1_ptr->powers[MAG_MILITARY],
	      ainfo_list[utype].pow_need[MAG_MILITARY]) &&
	MAGIC(n1_ptr->powers[MAG_WIZARDRY],
	      ainfo_list[utype].pow_need[MAG_WIZARDRY]) &&
	MAGIC(n1_ptr->powers[MAG_CIVILIAN],
	      ainfo_list[utype].pow_need[MAG_CIVILIAN])) {
      return(1);
    }

  }
  return(0);
}

/*
 * utype_ok - Validate unit type availability for recruitment or upgrade
 *
 * Comprehensive validation function that checks if a unit type can be
 * recruited or upgraded in the current context. Validates powers, resources,
 * population, and upgrade requirements with optional error reporting.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation (must not be NULL)
 *   c1_ptr - Pointer to city for recruitment (may be NULL)
 *   utype - Unit type index to validate (0 to num_armytypes-1)
 *   upg_men - Number of men being upgraded (>0 for upgrades)
 *   eout - If TRUE, display error messages to user
 *
 * Returns:
 *   TRUE if unit type is available and affordable, FALSE otherwise
 *
 * Side Effects:
 *   - May display error messages if eout is TRUE
 *   - Uses global string buffer for error message formatting
 *
 * Notes:
 *   - God nations bypass all restrictions
 *   - Validates unit type availability using utype_mayuse
 *   - For upgrades: checks minimum unit requirements and costs
 *   - For recruitment: checks population and resource availability
 *   - Provides user-friendly error messages for different failure conditions
 *   - Essential for user interface validation and command processing
 */
/* UTYPE_OK -- Check if the given unit type is okay for the current city */
int
utype_ok PARM_5(NTN_PTR, n1_ptr, CITY_PTR, c1_ptr, int, utype,
		int, upg_men, int, eout)
{
  int maxmen;

  /* god can get anything */
  if (is_god == TRUE) return(TRUE);

  /* check the information */
  if ((n1_ptr == NULL) ||
      (utype < 0) ||
      (utype >= num_armytypes)) {
    if (eout) {
      strcpy(string, "Error: invalid parameters to utype_ok()");
      errormsg(string);
    }
    return(FALSE);
  }

  /* now test the powers */
  if (utype_mayuse(n1_ptr, utype) <= 0) {
    if (eout) {
      if (a_ismonster(utype)) {
	errormsg("That monster is not available to your nation");
      } else {
	errormsg("That army type is not available to your nation");
      }
    }
    return(FALSE);
  }

  /* now test the minimum number of men */
  if ((upg_men > 0) &&
      !a_ismonster(utype)) {
    int i;
    ITEM_STRUCT cost_vals;

    /* check if the city is here */
    if (army_ptr == NULL) {
      if (eout) {
	errormsg("Invalid parameters to function utype_ok()");
	return(FALSE);
      }
    }
    if (c1_ptr == NULL) {
      if (eout)
	errormsg("There are not enough resources to upgrade to that type");
      return(FALSE);
    }

    /* check the pricing for full upgrade! */
    set_upgcosts(n1_ptr, army_ptr, &(cost_vals.mtrls[0]), utype);
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (c1_ptr->c_mtrls[i] < cost_vals.mtrls[i]) {
	break;
      }
    }

    /* was it a successful match? */
    if (i != MTRLS_NUMBER) {
      if (eout)
	errormsg("There are not enough resources to upgrade to that type");
      return(FALSE);
    }

  } else {

    /* check for getting just one */
    if ((maxmen = max_numunit(n1_ptr, c1_ptr, utype, upg_men)) < 1) {
      if (eout) {
	if (a_ismerc(utype) &&
	    (conq_mercsused >= (MERCMEN / (2 * MAXNTN)))) {
	  errormsg("You couldn't find any more mercs to hire");
	} else if (a_ismonster(utype)) {
	  errormsg("You don't have enough magical power to call that monster");
	} else {
	  errormsg("There are not enough resources for that type of unit");
	}
      }
      return(FALSE);
    } else if (a_ismerc(utype) &&
	       (maxmen < 10)) {
      if (eout) {
	errormsg("You couldn't find any mercs of that type");
      }
      return(FALSE);
    }

  }
  return(TRUE);
}

/*
 * navy_carrying - Check if a naval unit is carrying cargo or passengers
 *
 * Determines whether a naval unit is currently carrying any cargo, materials,
 * passengers, or army units. Used to validate movement, combat, and unloading
 * operations for naval units.
 *
 * Parameters:
 *   n1_ptr - Pointer to naval unit (must not be NULL)
 *
 * Returns:
 *   TRUE if navy is carrying anything, FALSE if empty
 *
 * Side Effects:
 *   - None (read-only cargo check)
 *
 * Notes:
 *   - Checks for civilian passengers (people field)
 *   - Checks for army units onboard (armynum field)
 *   - Checks all material types for cargo
 *   - Essential for naval movement and combat restrictions
 *   - Used to prevent certain actions while carrying cargo
 */
/* NAVY_CARRYING -- Return TRUE if the given unit has something onboard */
int
navy_carrying PARM_1(NAVY_PTR, n1_ptr)
{
  int i;

  /* check for people onboard */
  if ((n1_ptr->people != 0) ||
      (n1_ptr->armynum != EMPTY_HOLD)) {
    return(TRUE);
  }

  /* check the supplies */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (n1_ptr->mtrls[i] != 0) {
      return(TRUE);
    }
  }
  return(FALSE);
}

/*
 * cvn_carrying - Check if a caravan unit is carrying cargo or passengers
 *
 * Determines whether a caravan unit is currently carrying any cargo, materials,
 * or passengers. Used to validate movement, combat, and unloading operations
 * for caravan units.
 *
 * Parameters:
 *   c1_ptr - Pointer to caravan unit (must not be NULL)
 *
 * Returns:
 *   TRUE if caravan is carrying anything, FALSE if empty
 *
 * Side Effects:
 *   - None (read-only cargo check)
 *
 * Notes:
 *   - Checks for civilian passengers (people field)
 *   - Checks all material types for cargo
 *   - Does not check for army units (caravans don't transport armies)
 *   - Essential for caravan movement and combat restrictions
 *   - Used to prevent certain actions while carrying cargo
 */
/* CVN_CARRYING -- Return TRUE if the given unit has something onboard */
int
cvn_carrying PARM_1(CVN_PTR, c1_ptr)
{
  int i;

  /* check for people onboard */
  if (c1_ptr->people != 0) {
    return(TRUE);
  }

  /* check the supplies */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (c1_ptr->mtrls[i] != 0) {
      return(TRUE);
    }
  }
  return(FALSE);
}

/*
 * upg_class - Check if a unit class allows upgrades (static helper)
 *
 * Determines whether units of a specific class can be upgraded to higher
 * tiers within the same class. Certain special unit classes cannot be
 * upgraded due to their unique nature.
 *
 * Parameters:
 *   class - Unit class to check (AC_* constants)
 *
 * Returns:
 *   TRUE if class allows upgrades, FALSE otherwise
 *
 * Side Effects:
 *   - None (read-only class validation)
 *
 * Notes:
 *   - Leaders (AC_LEADER): cannot upgrade (unique individuals)
 *   - Monsters (AC_MONSTER): cannot upgrade (summoned creatures)
 *   - Mercenaries (AC_MERCS): cannot upgrade (hired units)
 *   - Unique units (AC_UNIQUE): cannot upgrade (special units)
 *   - All other classes: can upgrade to higher tiers
 *   - Used by may_upgrade to validate upgrade possibilities
 */
/* UPG_CLASS -- Return TRUE or FALSE if the class is upgradable */
static int
upg_class PARM_1(int, class)
{
  switch (class) {
  case AC_LEADER:
  case AC_MONSTER:
  case AC_MERCS:
  case AC_UNIQUE:
    /* no way, jose */
    return(FALSE);
    /*NOTREACHED*/
    break;
  default:
    /* hunky dorey */
    break;
  }
  return(TRUE);
}

/*
 * may_upgrade - Check if an army unit can be upgraded to a higher tier
 *
 * Validates whether an army unit of the specified type can be upgraded,
 * considering unit class restrictions, available upgrade targets, nation
 * powers, and resource requirements.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation (must not be NULL)
 *   c1_ptr - Pointer to city for resource checks (must not be NULL)
 *   utype - Current unit type to check for upgrades
 *   men - Number of men in the unit (must be > 0)
 *
 * Returns:
 *   TRUE if unit can be upgraded, FALSE otherwise
 *
 * Side Effects:
 *   - Sets global_int to unit type during processing
 *
 * Notes:
 *   - Only normal units (a_isnormal) can be upgraded
 *   - Unit class must allow upgrades (leaders, monsters, mercs cannot)
 *   - Must find a valid higher-tier unit of the same class
 *   - Target upgrade must pass utype_ok validation (powers, resources)
 *   - Searches linearly through higher unit types for same class
 *   - Essential for upgrade command validation
 */
/* MAY_UPGRADE -- Is the army unit able to be upgraded? */
int
may_upgrade PARM_4(NTN_PTR, n1_ptr, CITY_PTR, c1_ptr, int, utype, int, men)
{
  int i, cls;

  /* check the input */
  if ((n1_ptr == NULL) ||
      (c1_ptr == NULL) ||
      (men <= 0) ||
      (utype < 0) ||
      (utype >= num_armytypes)) {
    return(FALSE);
  }

  /* first check if it is a normal unit */
  if (!a_isnormal(utype)) return(FALSE);

  /* check if the class is upgradable */
  global_int = utype;
  cls = ainfo_list[utype].class;
  if (upg_class(cls) == FALSE) return(FALSE);

  /* now check if there is a type to upgrade to */
  for (i = utype + 1; i < num_armytypes; i++) {

    /* it is a better unit of the same class? */
    if ((int)ainfo_list[i].class == cls) {

      /* is it ok? */
      if (utype_ok(n1_ptr, c1_ptr, i, men, FALSE)) {
	return(TRUE);
      }

    }

  }

  /* couldn't find anything to upgrade to */
  return(FALSE);
}

/*
 * heal_rate - Calculate natural healing rate for an army unit
 *
 * Computes the percentage of health an army unit recovers per turn through
 * natural healing, considering terrain, weather, nation powers, unit status,
 * and environmental factors. Base rate modified by multiple factors.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation (for power bonuses)
 *   a1_ptr - Pointer to army unit (must not be NULL)
 *
 * Returns:
 *   Healing percentage per turn (0-50+), 0 if no healing possible
 *
 * Side Effects:
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Base rate: 10% (5% after final division by 2)
 *   - Undead units and magically healed units: 0% healing
 *   - Military powers (warrior, captain, warlord): +1% each
 *   - Water terrain: -3% (or -1% with sailor power)
 *   - Mountain terrain: -1% per elevation level above hills
 *   - Vegetation effects: good (+1%), harsh (-2% to -10%)
 *   - Season effects: winter (-5%), summer (-1%), spring (+1%)
 *   - Status bonuses: fortified (x2), reserve (x3), garrison (+2)
 *   - Environmental powers reduce harsh terrain penalties
 *   - Essential for unit recovery and strategic planning
 */
/* HEAL_RATE -- The percentage of health healed for an army unit */
int
heal_rate PARM_2(NTN_PTR, n1_ptr, ARMY_PTR, a1_ptr)
{
  /* base heal rate times two of a unit */
  int hold = 10, rstat;
  SCT_PTR s1_ptr;

  /* magically healed or undead units will not heal */
  if (a_isundead(a1_ptr->unittype) ||
      unit_healed(a1_ptr->status)) {
    return(0);
  }
  rstat = real_stat(a1_ptr);

  /* check location */
  if (!XY_INMAP(a1_ptr->xloc, a1_ptr->yloc)) {
    return (0);
  }
  s1_ptr = &(sct[a1_ptr->xloc][a1_ptr->yloc]);

  /* quicker reviitalization for better warriors */
  if (MAGIC(n1_ptr->powers[MAG_MILITARY], MM_WARRIOR)) {
    hold++;
  }
  if (MAGIC(n1_ptr->powers[MAG_MILITARY], MM_CAPTAIN)) {
    hold++;
  }
  if (MAGIC(n1_ptr->powers[MAG_MILITARY], MM_WARLORD)) {
    hold++;
  }

  /* check for slower strength gain on water */
  if (!a_mayunload(a1_ptr->unittype) &&
      (s1_ptr->altitude == ELE_WATER)) {
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_SAILOR)) {
      hold--;
    } else {
      hold -= 3;
    }
  }

  /* check terrain and weather */
  if ((s1_ptr->altitude > ELE_HILL) &&
      !r_mountaineer(n1_ptr->race)) {
    hold -= (s1_ptr->altitude - ELE_HILL);
  }
  switch (s1_ptr->vegetation) {
  case VEG_GOOD:
    hold++;
    break;
  case VEG_TUNDRA:
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_DERVISH) ||
	MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DESTROYER)) break;
    hold -= 10;
    break;
  case VEG_DESERT:
  case VEG_ICE:
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_DERVISH) ||
	MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DESTROYER)) break;
    hold -= 8;
    break;
  case VEG_SWAMP:
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_AMPHIBIAN)) break;
    hold -= 3;
    break;
  case VEG_JUNGLE:
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_AMPHIBIAN)) break;
    hold -= 2;
    break;
  default:
    /* no change */
    break;
  }
  switch (SEASON(TURN)) {
  case WINTER_SEASON:
    hold -= 5;
    break;
  case SUMMER_SEASON:
    hold--;
    break;
  case SPRING_SEASON:
    hold++;
    break;
  default:
    break;
  }
  if (hold < 0) {
    hold = 0;
  } else {
    /* improved healing for fortified troops */
    if (fort_stat(rstat)) {
      /* best healing on reserve troops */
      if (dieslast_stat(rstat)) {
	hold *= 3;
      } else {
	hold *= 2;
      }
    } else if (nomove_stat(rstat)) {
      hold += 2;
    }
  }
  return (hold/2);
}

/*
 * armies_in_sector - Count army units belonging to a nation in a sector
 *
 * Counts the total number of army units belonging to the specified nation
 * that are located in the given sector coordinates. Used for tactical
 * analysis, movement planning, and sector capacity calculations.
 *
 * Parameters:
 *   natn - Nation ID to count units for (1 to MAXNTN-1)
 *   xloc - X coordinate of sector to examine
 *   yloc - Y coordinate of sector to examine
 *
 * Returns:
 *   Number of army units found in the sector, 0 if none or invalid params
 *
 * Side Effects:
 *   - None (read-only counting operation)
 *
 * Notes:
 *   - Only counts units belonging to the specified nation
 *   - Validates map coordinates before searching
 *   - Validates nation ID before accessing nation data
 *   - Iterates through nation's army list checking locations
 *   - Used for tactical display and movement validation
 *   - Essential for determining sector congestion
 */
/* ARMIES_IN_SECTOR -- Report the number of army units in a sector */
int
armies_in_sector PARM_3 (int, natn, int, xloc, int, yloc)
{
  ARMY_PTR a1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, army_count = 0;

  /* set boundaries */
  if (natn == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = natn;
    c_end = natn + 1;
  }

  /* loop it */
  for (natn = c_start; natn < c_end; natn++) {

    /* set the nation */
    if ((nt_ptr = world.np[natn]) == NULL) continue;

    /* count the troops */
    for (a1_ptr = nt_ptr->army_list;
	 a1_ptr != NULL;
	 a1_ptr = a1_ptr->next) {
      if ((a1_ptr->xloc == xloc) && (a1_ptr->yloc == yloc)) break;
    }
    for ( ;
	 a1_ptr != NULL;
	 a1_ptr = a1_ptr->nrby) {
      army_count++;
    }
  }
  return(army_count);
}

/*
 * navies_in_sector - Count naval units belonging to a nation in a sector
 *
 * Counts the total number of naval units belonging to the specified nation
 * (or all nations if UNOWNED specified) that are located in the given sector
 * coordinates. Used for naval tactical analysis and movement planning.
 *
 * Parameters:
 *   natn - Nation ID to count for (1 to MAXNTN-1), or UNOWNED for all
 *   xloc - X coordinate of sector to examine
 *   yloc - Y coordinate of sector to examine
 *
 * Returns:
 *   Number of naval units found in the sector, 0 if none
 *
 * Side Effects:
 *   - None (read-only counting operation)
 *
 * Notes:
 *   - If natn is UNOWNED, counts naval units from all nations
 *   - Only counts naval units at exact coordinates
 *   - Does not validate map coordinates (assumes valid input)
 *   - Used for naval congestion analysis and tactical planning
 *   - Essential for determining naval capacity in ports
 */
/* NAVIES_IN_SECTOR -- Report the number of naval units in a sector */
int
navies_in_sector PARM_3 (int, natn, int, xloc, int, yloc)
{
  NAVY_PTR n1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, navy_count = 0;

  /* set boundaries */
  if (natn == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = natn;
    c_end = natn + 1;
  }

  /* loop it */
  for (natn = c_start; natn < c_end; natn++) {

    /* set the nation */
    if ((nt_ptr = world.np[natn]) == NULL) continue;

    /* count the navies */
    for (n1_ptr = nt_ptr->navy_list;
	 n1_ptr != NULL;
	 n1_ptr = n1_ptr->next) {
      if ((n1_ptr->xloc == xloc) && (n1_ptr->yloc == yloc))
	navy_count ++;
    }

  }
  return(navy_count);
}

/*
 * cvns_in_sector - Count caravan units belonging to a nation in a sector
 *
 * Counts the total number of caravan units belonging to the specified nation
 * (or all nations if UNOWNED specified) that are located in the given sector
 * coordinates. Used for trade route analysis and movement planning.
 *
 * Parameters:
 *   natn - Nation ID to count for (1 to MAXNTN-1), or UNOWNED for all
 *   xloc - X coordinate of sector to examine
 *   yloc - Y coordinate of sector to examine
 *
 * Returns:
 *   Number of caravan units found in the sector, 0 if none
 *
 * Side Effects:
 *   - None (read-only counting operation)
 *
 * Notes:
 *   - If natn is UNOWNED, counts caravan units from all nations
 *   - Only counts caravan units at exact coordinates
 *   - Does not validate map coordinates (assumes valid input)
 *   - Used for trade congestion analysis and route planning
 *   - Essential for determining caravan capacity in trading centers
 */
/* CVNS_IN_SECTOR -- Report the number of caravans in a sector */
int
cvns_in_sector PARM_3 (int, natn, int, xloc, int, yloc)
{
  CVN_PTR c1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, cvn_count = 0;

  /* set boundaries */
  if (natn == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = natn;
    c_end = natn + 1;
  }

  /* loop it */
  for (natn = c_start; natn < c_end; natn++) {

    /* set the nation */
    if ((nt_ptr = world.np[natn]) == NULL) continue;

    /* count the caravans */
    for (c1_ptr = nt_ptr->cvn_list;
	 c1_ptr != NULL;
	 c1_ptr = c1_ptr->next) {
      if ((c1_ptr->xloc == xloc) && (c1_ptr->yloc == yloc))
	cvn_count++;
    }

  }
  return(cvn_count);
}

/*
 * units_in_sector - Count all unit types for a nation in a sector
 *
 * Counts the total number of all unit types (armies, navies, and caravans)
 * belonging to the specified nation in the given sector. Provides a
 * comprehensive view of nation presence in a sector.
 *
 * Parameters:
 *   natn - Nation ID to count for (1 to MAXNTN-1), or UNOWNED for all
 *   x - X coordinate of sector to examine
 *   y - Y coordinate of sector to examine
 *
 * Returns:
 *   Total number of all unit types in the sector
 *
 * Side Effects:
 *   - None (read-only counting operation)
 *
 * Notes:
 *   - Aggregates results from armies_in_sector, navies_in_sector, cvns_in_sector
 *   - Used for comprehensive sector analysis and capacity planning
 *   - Essential for determining total nation presence in a location
 *   - Helpful for strategic planning and resource allocation
 */
/* UNITS_IN_SECTOR -- return the number of armies, caravans, and
                      navies in a sector for a nation.           */
int
units_in_sector PARM_3 (int, natn, int, x, int, y)
{
  int hold = 0;

  hold += armies_in_sector(natn, x, y);
  hold += navies_in_sector(natn, x, y);
  hold += cvns_in_sector(natn, x, y);

  return(hold);
}

/*
 * make_madunit - Create a hostile army unit at specified location
 *
 * Creates a new army unit with attack status and magical enhancements,
 * typically used for spawning hostile creatures, random encounters, or
 * automated defensive units. Assigns to savage nations if owner invalid.
 *
 * Parameters:
 *   own - Nation ID to own the unit (1 to MAXNTN-1, or <0 for auto-assign)
 *   atype - Unit type to create
 *   asize - Size/strength of the unit
 *   axloc - X coordinate for unit placement
 *   ayloc - Y coordinate for unit placement
 *
 * Returns:
 *   FALSE if unit created successfully, TRUE if creation failed
 *
 * Side Effects:
 *   - Creates new army unit in specified nation
 *   - Temporarily switches nation context during creation
 *   - Modifies global army sorting and unit lists
 *
 * Notes:
 *   - If owner invalid, searches for first savage nation
 *   - Unit starts with ST_ATTACK status and magical enhancements
 *   - Non-monster nations get standard supply settings
 *   - Restores original nation context after creation
 *   - Essential for random encounters and automated spawning
 *   - "Mad" units are typically hostile to player nations
 */
/* MAKE_MADUNIT -- Create an army of given type size and location */
int
make_madunit PARM_5(int, own, int, atype, int, asize, int, axloc, int, ayloc)
{
  NTN_PTR tmp_nptr = ntn_ptr;
  ARMY_PTR tmp_aptr = army_ptr;
  int hold_cnt = country;

  /* find the nation for the nasty monster */
  if ((own < 0) ||
      (own >= MAXNTN) ||
      ((ntn_tptr = world.np[own]) == NULL)) {
    for (own = 1; own < MAXNTN; own++) {
      /* find the first savage nation */
      if ((ntn_tptr = world.np[own]) == NULL) continue;
      if (n_issavage(ntn_tptr->active)) break;
    }
    if (own == MAXNTN) {
      return(TRUE);
    }
  }

  /* now build it */
  if (ntn_tptr != NULL) {

    /* assume control of the nation */
    ntn_ptr = ntn_tptr;
    country = own;

    /* now get the new unit */
    army_ptr = crt_army(atype);
    set_status(ARMY_STAT, ST_ATTACK);
    set_spelled(ARMY_STAT);	/* always start off nasty */
    ARMY_SIZE = asize;
    ARMY_XLOC = axloc;
    ARMY_YLOC = ayloc;
    if (!n_ismonster(ntn_ptr->active)) {
      /* standard supply setting */
      ARMY_SPLY = start_unitsply();
    }
    army_sort(FALSE);

    /* resume original settings */
    ntn_ptr = tmp_nptr;
    army_ptr = tmp_aptr;
    country = hold_cnt;
  }
  return(FALSE);
}

/*
 * resetnumbers - Clear unit numbering scheme for current context
 *
 * Clears the unit numbering scheme for either the current nation or the
 * global default scheme, freeing all allocated numbering structures.
 * Used to reset custom unit numbering preferences.
 *
 * Parameters:
 *   None (operates on global ntn_ptr context)
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Frees all unit numbering structures
 *   - Clears nation's numbering list or global default list
 *   - Deallocates memory for numbering preferences
 *
 * Notes:
 *   - If ntn_ptr is NULL, clears global default numbering scheme
 *   - If ntn_ptr is valid, clears nation-specific numbering scheme
 *   - Safely handles empty lists and NULL pointers
 *   - Essential for resetting unit identification preferences
 *   - Used when players want to return to default numbering
 */
/* RESETNUMBERS -- Clear out the default numbering scheme */
void
resetnumbers PARM_0(void)
{
  UNUM_PTR holdlist_ptr, holdunum_ptr;

  /* clear out the full numbering scheme */
  if (ntn_ptr == NULL) {
    holdlist_ptr = world.dflt_unum;
    world.dflt_unum = NULL;
  } else {
    holdlist_ptr = ntn_ptr->unum_list;
    ntn_ptr->unum_list = NULL;
  }
  while (holdlist_ptr != NULL) {
    holdunum_ptr = holdlist_ptr->next;
    holdlist_ptr->next = NULL;
    free(holdlist_ptr);
    holdlist_ptr = holdunum_ptr;
  }
}

/*
 * startnumber - Parse and assign custom unit numbering from string
 *
 * Parses a string specification for custom unit numbering and assigns the
 * starting number for a unit class or specific unit type. Supports both
 * class-wide numbering ("all_infantry 100") and specific type numbering.
 *
 * Parameters:
 *   str - String specification ("type_name number" format)
 *   estr - Error prefix string for error messages
 *
 * Returns:
 *   Assigned slot number on success, 0 on error
 *
 * Side Effects:
 *   - Creates or updates numbering scheme entries
 *   - Sets global_int and global_long for caller use
 *   - May display error messages
 *
 * Notes:
 *   - Format: "unit_type number" or "all_class_name number"
 *   - "all_" prefix indicates class-wide numbering
 *   - Number must be 1 to MAX_IDTYPE
 *   - Validates unit types and class names
 *   - Essential for custom unit identification schemes
 */
/* STARTNUMBER -- Assign a starting number using the given string */
int
startnumber PARM_2(char *, str, char *, estr)
{
  int slot, which, type;
  char s1[LINELTH], strtype[LINELTH];

  /* check the for the information */
  strcpy(s1, str);
  if (sscanf(s1, "%s %d", strtype, &slot) != 2) {
    sprintf(string, "error%s: startnumber, invalid format <%s>",
	    estr, str);
    errormsg(string);
    return(0);
  }
  if ((slot <= 0) || (slot > MAX_IDTYPE)) {
    sprintf(string, "error%s: startnumber, number %d out of range",
	    estr, slot);
    errormsg(string);
    return(0);
  }

  /* now analyze the string */
  if (str_ntest(strtype, "all_", 4) == 0) {
    type = 1;
    if ((which = uclassbyname(&(strtype[4]))) == -1) {
      sprintf(string, "error%s: startnumber unknown class <%s>",
	      estr, &(strtype[4]));
      errormsg(string);
      return(0);
    }
  } else {
    /* get the unit type */
    type = 2;
    if ((which = unitbyname(strtype)) == -1) {
      sprintf(string, "error%s: startnumber unknown unittype <%s>",
	      estr, strtype);
      errormsg(string);
      return(0);
    }
  }
  newslotnumber(slot, which, type);
  global_int = which;
  global_long = type;
  return(slot);
}

/*
 * unum_copydefault - Copy global default numbering scheme to current nation
 *
 * Copies the global default unit numbering scheme to the current nation's
 * personal numbering scheme. Used when a nation wants to adopt the global
 * defaults as their starting point for customization.
 *
 * Parameters:
 *   None (operates on global ntn_ptr context)
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Clears current nation's numbering scheme
 *   - Creates new numbering entries based on global defaults
 *   - Allocates memory for numbering structures
 *
 * Notes:
 *   - Requires valid ntn_ptr (current nation context)
 *   - First clears existing nation numbering scheme
 *   - Iterates through global default list copying entries
 *   - Used when nations want to start from global defaults
 *   - Essential for numbering scheme inheritance
 */
/* UNUM_COPYDEFAULT -- Copy the default numbering scheme */
void
unum_copydefault PARM_0(void)
{
  UNUM_PTR trace_ptr;

  /* do it */
  if (ntn_ptr == NULL) return;
  resetnumbers();
  for (trace_ptr = world.dflt_unum;
       trace_ptr != NULL;
       trace_ptr = trace_ptr->next) {
    newslotnumber(trace_ptr->number, trace_ptr->ident, trace_ptr->type);
  }
}

/*
 * newslotnumber - Create or update a unit numbering scheme entry
 *
 * Creates a new numbering scheme entry or updates an existing one with the
 * specified base number, identifier, and numbering type. Manages the linked
 * list of numbering preferences for the current context.
 *
 * Parameters:
 *   base - Starting number to assign (1 to MAX_IDTYPE)
 *   idnum - Unit type or class identifier
 *   numtype - Numbering type (0=all, 1=class, 2=specific type)
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Creates new numbering entry if not found
 *   - Updates existing entry if found
 *   - Modifies global or nation-specific numbering lists
 *
 * Notes:
 *   - If ntn_ptr is NULL, operates on global default scheme
 *   - If ntn_ptr is valid, operates on nation-specific scheme
 *   - Type 0: universal numbering (all units)
 *   - Type 1: class-specific numbering (idnum = class)
 *   - Type 2: unit-specific numbering (idnum = unit type)
 *   - Maintains sorted linked list for efficient lookup
 */
/* NEWSLOTNUMBER -- set the default number to the given value */
void
newslotnumber PARM_3(int, base, int, idnum, int, numtype)
{
  UNUM_PTR temp_ptr, trace_ptr;

  /* go through the current list */
  if (ntn_ptr == NULL) {
    temp_ptr = world.dflt_unum;
  } else {
    temp_ptr = ntn_ptr->unum_list;
  }
  for (/* done above */; 
       temp_ptr != NULL;
       temp_ptr = temp_ptr->next) {
    if ((temp_ptr->ident == idnum) &&
	(temp_ptr->type == numtype)) break;
  }

  /* check for a replacement */
  if (temp_ptr != NULL) {
    /* just replace the number */
    temp_ptr->number = base;
  } else {
    /* create a new item */
    temp_ptr = new_unum();
    temp_ptr->next = NULL;
    temp_ptr->number = base;
    temp_ptr->ident = idnum;
    temp_ptr->type = numtype;
    if (ntn_ptr == NULL) {
      if (world.dflt_unum == NULL) {
	world.dflt_unum = temp_ptr;
      } else {
	for (trace_ptr = world.dflt_unum;
	     trace_ptr->next != NULL;
	     trace_ptr = trace_ptr->next) ;
	trace_ptr->next = temp_ptr;
      }
    } else {
      if (ntn_ptr->unum_list == NULL) {
	ntn_ptr->unum_list = temp_ptr;
      } else {
	for (trace_ptr = ntn_ptr->unum_list;
	     trace_ptr->next != NULL;
	     trace_ptr = trace_ptr->next) ;
	trace_ptr->next = temp_ptr;
      }
    }
  }
}

/*
 * unit_basenum - Determine the starting number for a unit type
 *
 * Looks up the appropriate starting number for a unit type based on the
 * current numbering scheme. Follows priority order: specific type numbers
 * override class numbers, which override universal numbers.
 *
 * Parameters:
 *   utype - Unit type to look up numbering for
 *
 * Returns:
 *   Starting number for the unit type, 1 if no specific number found
 *
 * Side Effects:
 *   - None (read-only lookup)
 *
 * Notes:
 *   - Uses nation-specific scheme if available, else global default
 *   - Priority: type-specific (2) > class-specific (1) > universal (0)
 *   - Higher type values override lower type values
 *   - Falls back to 1 if no matching numbering scheme found
 *   - Essential for automatic unit numbering assignment
 */
/* UNIT_BASENUM -- Determine the base unit number of a unit */
int
unit_basenum PARM_1(int, utype)
{
  int hold = 1, curlevel = -1;
  UNUM_PTR unum_ptr;

  /* go through the entire spectrum of possibe unit numbers */
  if (ntn_ptr->unum_list == NULL) {
    unum_ptr = world.dflt_unum;
  } else {
    unum_ptr = ntn_ptr->unum_list;
  }
  for (/* done in prior two lines */;
       unum_ptr != NULL;
       unum_ptr = unum_ptr->next) {
    if (curlevel <= unum_ptr->type) {
      switch (unum_ptr->type) {
      case 0:
	/* always matches */
	curlevel = unum_ptr->type;
	hold = unum_ptr->number;
	break;
      case 1:
	/* matches proper unit class */
	if ((int)ainfo_list[utype].class == unum_ptr->ident) {
	  curlevel = unum_ptr->type;
	  hold = unum_ptr->number;
	}
	break;
      case 2:
	if (utype == unum_ptr->ident) {
	  hold = unum_ptr->number;
	  curlevel = unum_ptr->type;
	}
	break;
      default:
	break;
      }
    }
  }
  return (hold);
}

/*
 * army_renum - Renumber an army unit and update all references
 *
 * Changes the ID number of the current army unit and updates all references
 * to the old ID throughout the game data structures. Ensures referential
 * integrity across armies, navies, and items.
 *
 * Parameters:
 *   newnum - New ID number to assign to the army unit
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Updates army unit's ID number
 *   - Updates leader references in other army units
 *   - Updates army references in naval units
 *   - Updates army references in item structures
 *
 * Notes:
 *   - Requires valid ntn_ptr and army_ptr
 *   - If unit is a leader, updates follower leader references
 *   - If unit is onboard ships, updates naval cargo references
 *   - If unit has items, updates item ownership references
 *   - Essential for maintaining data consistency during renumbering
 */
/* ARMY_RENUM -- Renumber the current army unit in the current nation */
void
army_renum PARM_1(int, newnum)
{
  /* check data */
  if ((ntn_ptr == NULL) || (army_ptr == NULL)) return;
  if (a_isleader(ARMY_TYPE)) {
    for (army_tptr = ntn_ptr->army_list;
	 army_tptr != NULL;
	 army_tptr = army_tptr->next) {
      if (ARMYT_LEAD == ARMY_ID) {
	ARMYT_LEAD = newnum;
      }
    }
  }
  if ((ARMY_ID != EMPTY_HOLD) &&
      (onship_stat(ARMY_STAT))) {
    for (navy_tptr = ntn_ptr->navy_list;
	 navy_tptr != NULL;
	 navy_tptr = navy_tptr->next) {
      if (NAVYT_ARMY == ARMY_ID) {
	NAVYT_ARMY = newnum;
      }
    }
  }
  if (iteminfo_stat(ARMY_STAT)) {
    for (item_tptr = ntn_ptr->item_list;
	 item_tptr != NULL;
	 item_tptr = item_tptr->next) {
      if (ITEMT_ARMY == ARMY_ID) {
	ITEMT_ARMY = newnum;
      }
    }
  }
  ARMY_ID = newnum;
}

/*
 * navy_renum - Renumber a naval unit and update all references
 *
 * Changes the ID number of the current naval unit and updates all references
 * to the old ID throughout the game data structures. Ensures referential
 * integrity for items that belong to this naval unit.
 *
 * Parameters:
 *   newnum - New ID number to assign to the naval unit
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Updates naval unit's ID number
 *   - Updates naval references in item structures
 *
 * Notes:
 *   - Requires valid ntn_ptr and navy_ptr
 *   - If unit has items, updates item ownership references
 *   - Does not affect army or caravan references (navies only carry armies/caravans)
 *   - Essential for maintaining data consistency during naval renumbering
 */
/* NAVY_RENUM -- Renumber the current navy unit in the current nation */
void
navy_renum PARM_1(int, newnum)
{
  /* check data */
  if ((ntn_ptr == NULL) || (navy_ptr == NULL)) return;
  if (iteminfo_stat(NAVY_STAT)) {
    for (item_tptr = ntn_ptr->item_list;
	 item_tptr != NULL;
	 item_tptr = item_tptr->next) {
      if (ITEMT_NAVY == NAVY_ID) {
	ITEMT_NAVY = newnum;
      }
    }
  }
  NAVY_ID = newnum;
}

/*
 * cvn_renum - Renumber a caravan unit and update all references
 *
 * Changes the ID number of the current caravan unit and updates all references
 * to the old ID throughout the game data structures. Ensures referential
 * integrity across naval units and items that reference this caravan.
 *
 * Parameters:
 *   newnum - New ID number to assign to the caravan unit
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Updates caravan unit's ID number
 *   - Updates caravan references in naval units (if onboard)
 *   - Updates caravan references in item structures
 *
 * Notes:
 *   - Requires valid ntn_ptr and cvn_ptr
 *   - If caravan is onboard ships, updates naval cargo references
 *   - If caravan has items, updates item ownership references
 *   - Essential for maintaining data consistency during caravan renumbering
 */
/* CVN_RENUM -- Renumber the current caravan unit in the current nation */
void
cvn_renum PARM_1(int, newnum)
{
  /* check data */
  if ((ntn_ptr == NULL) || (cvn_ptr == NULL)) return;
  if ((CVN_ID != EMPTY_HOLD) &&
      onship_stat(CVN_STAT)) {
    for (navy_tptr = ntn_ptr->navy_list;
	 navy_tptr != NULL;
	 navy_tptr = navy_tptr->next) {
      if (NAVYT_CVN == CVN_ID) {
	NAVYT_CVN = newnum;
      }
    }
  }
  if (iteminfo_stat(CVN_STAT)) {
    for (item_tptr = ntn_ptr->item_list;
	 item_tptr != NULL;
	 item_tptr = item_tptr->next) {
      if (ITEMT_CVN == CVN_ID) {
	ITEMT_CVN = newnum;
      }
    }
  }
  CVN_ID = newnum;
}

/*
 * army_spellpts - Calculate maximum spell points for an army unit
 *
 * Determines the maximum spell points an army unit should have based on
 * unit type, caster abilities, nation spell point capacity, and unit
 * characteristics. Different unit types have varying spell capacities.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation (for spell point attributes)
 *   a1_ptr - Pointer to army unit (must not be NULL)
 *
 * Returns:
 *   Maximum spell points for the unit, 0 if unit cannot cast spells
 *
 * Side Effects:
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Only leaders (a_isleader) and spell casters (a_castspells) get spell points
 *   - Rulers and full casters: full nation spell point attribute
 *   - Other leaders: half nation spell point attribute
 *   - Non-caster leaders: quarter nation spell point attribute
 *   - Monster units: 75% of calculated value with strength scaling
 *   - Final value affected by unit strength for monsters
 *   - Essential for magical combat and spellcasting capacity
 */
/* ARMY_SPELLPTS -- How many spell points maximum should a unit get? */
int
army_spellpts PARM_2(NTN_PTR, n1_ptr, ARMY_PTR, a1_ptr)
{
  int hold = 0;

  /* check inputs */
  if ((n1_ptr == NULL) || (a1_ptr == NULL)) return(hold);

  /* check the type */
  if (!a_isleader(ARMY_TYPE) &&
      !a_castspells(ARMY_TYPE)) return(hold);

  /* find the base */
  if (a_isruler(ARMY_TYPE) || a_fullcaster(ARMY_TYPE)) {
    hold = n1_ptr->attribute[BUTE_SPELLPTS];
  } else {
    hold = (n1_ptr->attribute[BUTE_SPELLPTS] + 1) / 2;
  }
  if (!a_castspells(ARMY_TYPE)) {
    hold++;
    hold /= 2;
  }
  if (a_ismonster(ARMY_TYPE)) {
    hold++;
    hold *= 3;
    hold /= 4;
  }

  /* monster units combine */
  if (!a_isleader(ARMY_TYPE)) {
    hold *= ARMY_SIZE;
  }

  /* maximum limit */
  if (hold > 200) hold = 200;
  return(hold);
}

/*
 * start_unitsply - Get standard starting supply value for new units
 *
 * Determines the initial supply level that newly created units should
 * have when recruited or created. Provides a reasonable default that
 * balances early unit effectiveness with resource management.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Standard starting supply value (2, or MAXSUPPLIES if less than 2)
 *
 * Side Effects:
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Returns 2 as standard starting supply if MAXSUPPLIES >= 2
 *   - Returns MAXSUPPLIES if system limit is less than 2
 *   - Ensures new units start with reasonable supply levels
 *   - Used during unit recruitment and creation processes
 *   - Prevents units from starting with excessive or inadequate supplies
 *   - Essential for consistent unit initialization
 */
/* START_UNITSPLY -- Return the standard starting supply value for new units */
int
start_unitsply PARM_0(void)
{
  if (MAXSUPPLIES < 2) {
    return(MAXSUPPLIES);
  }
  return(2);
}
