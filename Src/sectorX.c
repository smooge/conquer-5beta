/*
 * sectorX.c - Sector Management and Resource System
 *
 * This file contains the core sector management system for the Conquer game,
 * handling sector designations, resource calculations, production/consumption,
 * validation, and economic modeling. It provides the foundation for the
 * game's economic and construction systems.
 *
 * KEY FUNCTIONAL AREAS:
 *
 * 1. DESIGNATION VALIDATION:
 *    - majdesg_costs() / desg_ok() - Major designation validation and costing
 *    - mindesg_costs() / mindesg_ok() - Minor designation validation and costing
 *    - Complex validation rules for terrain, ownership, resources, special cases
 *
 * 2. RESOURCE MANAGEMENT:
 *    - find_resources() - Collect available resources from cities/navies/caravans
 *    - take_resources() / send_resources() - Resource redistribution system
 *    - fr_accume() / tr_consume() - Resource accumulation and consumption helpers
 *
 * 3. ECONOMIC PRODUCTION:
 *    - sector_produce() - Calculate sector production output (food, metals, jewels, wood, taxes)
 *    - sector_consume() - Calculate sector consumption needs (food, materials, support)
 *    - Seasonal production variations, population effects, minor designation bonuses
 *
 * 4. TERRAIN EVALUATION:
 *    - tofood() / towood() - Calculate food/wood production potential by nation
 *    - tg_ok() - Trade good visibility and exploitation validation
 *    - Nation-specific bonuses (racial traits, magical powers, trade goods)
 *
 * 5. DEFENSIVE CALCULATIONS:
 *    - fort_val() - Fortification defensive value calculation
 *    - defense_val() - Terrain-based defensive bonuses
 *    - exposure_value() - Environmental exposure calculations for troops
 *
 * 6. RESOURCE VALUE EXTRACTION:
 *    - metal_value() / jewel_value() / magic_value() - Extract resource values
 *    - getmetal() / getjewel() / getspell() - Assign resource types to sectors
 *    - s_uselevel() - Calculate sector usage efficiency
 *
 * 7. SPECIAL UTILITIES:
 *    - is_habitable() - Determine if sector can support population
 *    - sct_cost_adjust() - Apply sector-specific cost modifications
 *    - distort_vision() - Apply magical vision distortion effects
 *    - rand_sector() - Generate random sector within range
 *
 * TECHNICAL NOTES:
 * - Extensive use of global variables for current nation, turn, and game state
 * - Heavy integration with data tables (maj_dinfo, min_dinfo, veg_info, ele_info)
 * - Complex magical system integration for bonuses and special abilities
 * - Sophisticated resource supply chain modeling with range calculations
 * - Seasonal and environmental effects on production and consumption
 * - Multi-layered validation system preventing invalid constructions
 * - Dynamic cost adjustment based on location, ownership, and magical effects
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
#include "dataX.h"
#include "cityX.h"
#include "itemX.h"
#include "butesX.h"
#include "calenX.h"
#include "desigX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "racesX.h"
#include "worldX.h"
#include "elevegX.h"
#include "hlightX.h"
#include "statusX.h"
#include "tgoodsX.h"

/*
 * majdesg_costs - Calculate resource costs for major sector designation
 *
 * Computes the material costs required to build or change a sector's major
 * designation (e.g., farm, city, mine). Costs depend on the current and
 * target designations, existing minor designations, and magical cost
 * adjustments. Returns an item structure containing the required materials.
 *
 * Parameters:
 *   which - Target major designation index (0 to MAJ_NUMBER-1)
 *   x - X coordinate of the sector
 *   y - Y coordinate of the sector
 *   verbal - Unused parameter (marked ARGSUSED)
 *
 * Returns:
 *   ITEM_PTR - Allocated item structure with cost breakdown by material type
 *   NULL - If invalid parameters provided
 *
 * Side Effects:
 *   - Allocates memory via new_item() that caller must free
 *   - Applies magical cost adjustments via mgk_cost_adjust()
 *   - Uses global designation info arrays (maj_dinfo, min_dinfo)
 *
 * Notes:
 *   - Cost calculation includes existing minor designations that remain
 *   - Multiplier difference between old and new designation affects cost
 *   - Magic powers can reduce material costs
 *   - Returned item has coordinates set to sector location
 */
ITEM_PTR
majdesg_costs PARM_4(int, which, int, x, int, y, int, verbal)
{
  /*ARGSUSED*/
  ITEM_PTR cost_ptr = NULL;
  SCT_PTR sptr;
  int olddesg, mult, i, j;

  /* input check */
  if (!XY_ONMAP(x, y) ||
      ((sptr = &(sct[x][y])) == NULL) ||
      (which < 0) ||
      (which >= MAJ_NUMBER)) {
    return(cost_ptr);
  }

  /* check the pricing data */
  olddesg = major_desg(sptr->designation);
  mult = maj_dinfo[which].multiplier - maj_dinfo[olddesg].multiplier;
  cost_ptr = new_item();
  cost_ptr->xloc = x;
  cost_ptr->yloc = y;
  cost_ptr->itemid = 0;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    cost_ptr->mtrls[i] = (itemtype) 0;
  }

  /* now compute the costs */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    cost_ptr->mtrls[i] = maj_dinfo[which].mtrls_cost[i];
    if (mult > 0) {
      for (j = 0; j < MIN_NUMBER; j++) {
	if ((MIN_START << j) == MIN_DEVASTATED) continue;
	if (minor_desg(sptr->designation, MIN_START << j) &&
	    !minor_desg(MIN_DEVASTATED | maj_dinfo[which].info_val,
			MIN_START << j)) {
	  cost_ptr->mtrls[i] += mult * min_dinfo[j].mtrls_cost[i];
	}
      }
    }
  }

  /* check the magical cost adjustment */
  mgk_cost_adjust(4, &(cost_ptr->mtrls[0]));
  return(cost_ptr);
}

/*
 * dgok_owncheck - Check sector ownership for bridge construction validation
 *
 * Static helper function used during bridge construction validation to
 * check if neighboring sectors are owned by the current nation. Counts
 * non-water sectors and sets global flag if unowned land is found.
 * Used by map_loop() to validate bridge placement requirements.
 *
 * Parameters:
 *   x - X coordinate of sector to check
 *   y - Y coordinate of sector to check
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Increments global_long for each non-water sector
 *   - Sets global_int to TRUE if sector not owned by current nation
 *   - Uses global 'country' variable for ownership comparison
 *
 * Notes:
 *   - Only counts land sectors (altitude != ELE_WATER)
 *   - Called by map_loop() during bridge construction validation
 *   - Bridge requires ownership of all neighboring land sectors
 */
static
void dgok_owncheck PARM_2(int, x, int, y)
{
  if (sct[x][y].altitude != ELE_WATER) {
    global_long++;
    if (sct[x][y].owner != country) {
      global_int = TRUE;
    }
  }
}

/*
 * desg_ok - Validate major sector designation change request
 *
 * Comprehensive validation function that checks if a major designation change
 * is legal and feasible for a given sector. Validates terrain compatibility,
 * resource requirements, ownership constraints, special building rules, and
 * material costs. This is the primary gatekeeper for sector development.
 *
 * Parameters:
 *   x - X coordinate of the sector
 *   y - Y coordinate of the sector  
 *   dtype - Target major designation type (MAJ_* constant)
 *   verbal - TRUE to display error messages, FALSE for silent validation
 *
 * Returns:
 *   TRUE - Designation change is valid and can proceed
 *   FALSE - Designation change is invalid or not affordable
 *
 * Side Effects:
 *   - May display error messages if verbal is TRUE
 *   - Calls majdesg_costs() and find_resources() to check affordability
 *   - Uses map_loop() for bridge construction validation
 *   - May perform complex validation logic for specific designations
 *
 * Notes:
 *   - God players (is_god) bypass most restrictions
 *   - Bridge construction requires ownership of neighboring land
 *   - Cities must be devastated before changing to resource extraction
 *   - Validates food support, population requirements, and terrain limits
 *   - Checks material costs and resource availability within range
 */
int
desg_ok PARM_4(int, x, int, y, int, dtype, int, verbal)
{
  CITY_PTR c1_ptr = NULL;
  SCT_PTR sptr;
  ITEM_PTR cost_ptr;
  int count, okval, olddesg, is_sieged = FALSE;

  /* check the input */
  if (!XY_ONMAP(x, y)) {
    if (verbal) errormsg("ERROR: Invalid parameters to desg_ok");
    return(FALSE);
  }
  sptr = &(sct[x][y]);
  olddesg = major_desg(sptr->designation);
  if ((sptr == NULL) || (dtype < 0) || (dtype > MAJ_CAPITAL)) {
    if (verbal) errormsg("ERROR: Invalid parameters to desg_ok");
    return(FALSE);
  }
  if (minor_desg(sptr->designation, MIN_SIEGED)) {
    is_sieged = TRUE;
  }

  /* check if there is water */
  if (sptr->altitude == ELE_WATER) {
    if (dtype != MAJ_BRIDGE) {
      if (verbal) {
	sprintf(string, "You may not build a %s underwater",
		maj_dinfo[dtype].name);
	errormsg(string);
      }
      return(FALSE);
    }
  } else {
    if (dtype == MAJ_BRIDGE) {
      if (verbal) errormsg("You may only build bridges in water sectors");
      return(FALSE);
    }
  }

  /* check if the designation is the same */
  if (dtype == olddesg) {
    if (verbal) errormsg("That designation is already there!");
    return(FALSE);
  }

  /* if not god then check limitations */
  if (is_god != TRUE) {

    /* check the vegetation */
    if ((sptr->altitude != ELE_WATER) &&
	(dtype != MAJ_NONE) &&
	((dtype < MAJ_CANAL) ||
	 (dtype > MAJ_STOCKADE)) &&
	(tofood(sptr, country) == 0)) {
      if (verbal) errormsg("That sector cannot support your people");
      return(FALSE);
    }

    /* check for fertile and fruitful designations */
    if ((dtype == MAJ_FERTILE) || (dtype == MAJ_FRUITFUL)) {
      if (verbal) errormsg("You must begin by building a Farm");
      return(FALSE);
    }

    /* check for construction going on */
    if (IS_CITY(olddesg) &&
	((c1_ptr = citybyloc(ntn_ptr, x, y)) != NULL) &&
	(c1_ptr->i_people < 0)) {
      if (verbal) errormsg("That sector is currently under construction");
      return(FALSE);
    }

    /* check for not removing materials */
    if (IS_CITY(olddesg) &&
	(c1_ptr != NULL) &&
	!IS_CITY(dtype)) {
      if (c1_ptr->c_mtrls[MTRLS_TALONS] < 0) {
	if (verbal) {
	  errormsg("A debt may not be absolved by destroying the treasury");
	}
	return(FALSE);
      }
      for (count = 0; count < MTRLS_NUMBER; count++) {
	if (c1_ptr->c_mtrls[count] > (itemtype) 0) {
	  if (verbal) {
	    errormsg("Remove all raw materials before destruction");
	  }
	  return(FALSE);
	}
      }
    }

    /* now check other qualifications */
    switch (dtype) {
    case MAJ_BRIDGE:
      /* check neighboring lands */
      if (is_god == FALSE) {
	global_int = FALSE;
	global_long = 0;
	map_loop(x, y, 1, dgok_owncheck);
	if (global_int == TRUE) {
	  if (verbal) {
	    errormsg("You must own all neighboring lands to build a bridge");
	  }
	  return(FALSE);
	}
	if (global_long) {
	  if (verbal) {
	    errormsg("A bridge must have at least one edge on land");
	  }
	  return(FALSE);
	}
      }
      break;
    case MAJ_METALMINE:
      if ((olddesg == MAJ_CITY) &&
	  !minor_desg(sptr->designation, MIN_DEVASTATED)) {
	if (verbal)
	  errormsg("Cities must first be devastated or turned into towns");
	return(FALSE);
      }
      if (!tg_ok(sptr, country) || (metal_value(sptr) == 0)) {
	if (verbal) errormsg("Do you want your people to be unemployed?");
	return(FALSE);
      }
      break;
    case MAJ_JEWELMINE:
      if ((olddesg == MAJ_CITY) &&
	  !minor_desg(sptr->designation, MIN_DEVASTATED)) {
	if (verbal)
	  errormsg("Cities must first be devastated or turned into towns");
	return(FALSE);
      }
      if (!tg_ok(sptr, country) || (jewel_value(sptr) == 0)) {
	if (verbal) errormsg("Do you want your people to be unemployed?");
	return(FALSE);
      }
      break;
    case MAJ_CITY:
      if ((olddesg != MAJ_TOWN) ||
	  !minor_desg(sptr->designation, MIN_FORTIFIED)) {
	if (verbal)
	  errormsg("Only fortified towns may become cities");
	return(FALSE);
      }
      if (sptr->people < 1000) {
	if (verbal)
	  errormsg("This town is not large enough to become a city");
	return(FALSE);
      }
      break;
    case MAJ_TOWN:
      /* must have enough people in the town to build it */
      if ((sptr->people < 500) &&
	  (olddesg != MAJ_CITY)) {
	if (verbal)
	  errormsg("You need 500 people to build a town");
	return(FALSE);
      }
      break;
    case MAJ_CAPITAL:
      /* must have a city */
      if (olddesg != MAJ_CITY) {
	if (verbal)
	  errormsg("Only cities may be used as the national Capital");
	return(FALSE);
      }
      break;
    case MAJ_LUMBERYARD:
      /* can't kill a city */
      if ((olddesg == MAJ_CITY) &&
	  !minor_desg(sptr->designation, MIN_DEVASTATED)) {
	if (verbal)
	  errormsg("Cities must first be devastated or turned into towns");
	return(FALSE);
      }

      /* check the wood value */
      if (towood(sptr, country) == 0) {
	if (verbal) errormsg("Do you want your people to be unemployed?");
	return(FALSE);
      }
      break;
    case MAJ_FARM:
      /* cannot kill a city */
      if ((olddesg == MAJ_CITY) &&
	  !minor_desg(sptr->designation, MIN_DEVASTATED)) {
	if (verbal)
	  errormsg("Cities must first be devastated or turned into towns");
	return(FALSE);
      }

      /* need to be able to farm it */
      if (tofood(sptr, country) < DESFOOD) {
	if (verbal) errormsg("Your people cannot cultivate a farm there");
	return(FALSE);
      }
      break;
    case MAJ_CANAL:
      if ((sptr->altitude > ELE_HILL) ||
	  (sptr->altitude < ELE_CLEAR)) {
	if (verbal) errormsg("The elevation prevents canal construction");
	return(FALSE);
      }
    default:
      if ((olddesg == MAJ_CITY) &&
	  !minor_desg(sptr->designation, MIN_DEVASTATED)) {
	if (verbal)
	  errormsg("Cities must first be devastated or turned into towns");
	return(FALSE);
      }
      break;
    }

    /* check the costs */
    if ((item_tptr = find_resources(x, y, is_sieged)) == NULL) {
      if (verbal) {
	errormsg("There are no available resources within range");
      }
      return(FALSE);
    }

    /* get the costs */
    if ((cost_ptr = majdesg_costs(dtype, x, y, verbal)) == NULL) {
      errormsg("Help me, I am so confused");
      free(item_tptr);
      return(0);
    }

    /* check if it is okay */
    okval = TRUE;
    for (count = 0; count < MTRLS_NUMBER; count++) {
      if (ITEMT_MTRLS[count] < cost_ptr->mtrls[count]) {
	if (verbal) {
	  sprintf(string, "You do not have %.0f %s available\n",
		  (double) cost_ptr->mtrls[count],
		  mtrls_info[count].lname);
	  errormsg(string);
	}
	okval = FALSE;
      }
    }

    /* don't need them anymore */
    free(item_tptr);
    free(cost_ptr);
    if (okval == FALSE) {
      return(FALSE);
    }

  }
  return(TRUE);
}

/*
 * water_nearby - Check for water in neighboring sector (helper function)
 *
 * Static helper function used by map_loop() to detect water sectors in
 * the vicinity. Sets global_int to TRUE if the specified sector contains
 * water. Used primarily for harbor construction validation.
 *
 * Parameters:
 *   x - X coordinate of sector to check
 *   y - Y coordinate of sector to check
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Sets global_int to TRUE if sector altitude is ELE_WATER
 *   - Called by map_loop() during harbor placement validation
 *
 * Notes:
 *   - Simple water detection for proximity checks
 *   - Used to ensure harbors have access to water
 *   - Part of the map traversal validation system
 */
static void
water_nearby PARM_2(int, x, int, y)
{
  if (sct[x][y].altitude == ELE_WATER) {
    global_int = TRUE;
  }
}

/*
 * canal_nearby - Check for owned canal in neighboring sector (helper function)
 *
 * Static helper function used by map_loop() to detect canals owned by the
 * current nation in neighboring sectors. Sets global_int to TRUE if an
 * owned canal is found. Used for harbor construction validation when
 * direct water access is not available.
 *
 * Parameters:
 *   x - X coordinate of sector to check
 *   y - Y coordinate of sector to check
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Sets global_int to TRUE if owned canal found
 *   - Uses global 'country' variable for ownership comparison
 *   - Called by map_loop() during harbor placement validation
 *
 * Notes:
 *   - Canals can provide water access for harbors in landlocked areas
 *   - Only canals owned by the current nation are considered valid
 *   - Part of the complex harbor placement validation system
 */
static void
canal_nearby PARM_2(int, x, int, y)
{
  if ((sct[x][y].owner == country) &&
      (major_desg(sct[x][y].designation) == MAJ_CANAL)) {
    global_int = TRUE;
  }
}

/*
 * mindesg_costs - Calculate resource costs for minor sector designation
 *
 * Computes material costs required to add minor designations to a sector
 * (e.g., fortifications, harbors, trading posts). Costs vary based on
 * the major designation's multiplier, special location factors, and
 * magical adjustments. Handles special pricing for harbors without
 * water access and fortification level costs.
 *
 * Parameters:
 *   which - Minor designation index (0 to MIN_NUMBER-1)
 *   x - X coordinate of the sector
 *   y - Y coordinate of the sector
 *   verbal - TRUE to display error messages for validation failures
 *
 * Returns:
 *   ITEM_PTR - Allocated item structure with cost breakdown
 *   NULL - If invalid parameters or validation failures
 *
 * Side Effects:
 *   - Allocates memory via new_item() that caller must free
 *   - May use map_loop() to check water proximity for harbors
 *   - Applies sector and magical cost adjustments
 *   - May display error messages if verbal is TRUE
 *
 * Notes:
 *   - Harbor costs quadruple if no water access available
 *   - Fortification costs scale with current fortress level
 *   - Devastation rebuilding costs are halved
 *   - Uses citybyloc() for city-specific validations
 */
ITEM_PTR
mindesg_costs PARM_4(int, which, int, x, int, y, int, verbal)
{
  ITEM_PTR cost_ptr = NULL;
  SCT_PTR sptr;
  int value, majval, mult, i, j;

  /* input check */
  if (!XY_ONMAP(x, y) ||
      ((sptr = &(sct[x][y])) == NULL) ||
      (which < 0) ||
      (which >= MIN_NUMBER)) {
    return(cost_ptr);
  }

  /* check the pricing */
  value = MIN_START << which;
  majval = major_desg(sptr->designation);
  mult = maj_dinfo[majval].multiplier;
  cost_ptr = new_item();
  cost_ptr->xloc = x;
  cost_ptr->yloc = y;
  cost_ptr->itemid = 0;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    cost_ptr->mtrls[i] = (itemtype) 0;
  }

  /* increase the multiplier for harbors */
  if (value == MIN_HARBOR) {
    if (is_god == FALSE) {
      global_int = FALSE;
      map_loop(x, y, 1, water_nearby);
      if (global_int == FALSE) {
	/* dramatically increased cost for land-bound harbors */
	mult *= 4;
      }
    }
  }

  /* check which costs */
  if (((value != MIN_FORTIFIED) &&
       (value != MIN_DEVASTATED)) ||
      (!minor_desg(sptr->designation, value))) {

    /* assign cost */
    for (i = 0; i < MTRLS_NUMBER; i++) {
      cost_ptr->mtrls[i] = mult * min_dinfo[which].mtrls_cost[i];
    }

  } else if (value == MIN_DEVASTATED) {

    /* compute the costs for rebuilding the designation */
    for (i = 0; i < MTRLS_NUMBER; i++) {
      cost_ptr->mtrls[i] = maj_dinfo[majval].mtrls_cost[i];
      for (j = 0; j < MIN_NUMBER; j++) {
	if ((MIN_START << j) == MIN_DEVASTATED) continue;
	if (minor_desg(sptr->designation, MIN_START << j)) {
	  cost_ptr->mtrls[i] += mult * min_dinfo[j].mtrls_cost[i];
	}
      }
      cost_ptr->mtrls[i] /= 2;
    }

  } else if ((city_ptr = citybyloc(ntn_ptr, x, y)) != NULL) {

    /* check current level and cost */
    if (majval == MAJ_CACHE) {
      if (verbal)
	errormsg("Caches cannot contain extensive fortifications");
      free(cost_ptr);
      return(cost_ptr = NULL);
    } else if (CITY_FORT >= MAXFORTVAL) {
      if (verbal)
	errormsg("Your fortifications are as strong as possible");
      free(cost_ptr);
      return(cost_ptr = NULL);
    }
    cost_ptr->mtrls[MTRLS_TALONS] = (CITY_FORT + 1) * 3 * FORTCOST;
    cost_ptr->mtrls[MTRLS_METALS] = (CITY_FORT + 1) * 2 * FORTCOST;
    cost_ptr->mtrls[MTRLS_WOOD] = (CITY_FORT + 1) * 1 * FORTCOST;
    if (CIV_MAGIC(MC_ARCHITECT)) {
      for (i = 0; i < MTRLS_NUMBER; i++) {
	cost_ptr->mtrls[i] *= 80;
	cost_ptr->mtrls[i] /= 100;
      }
    }

  } else {

    /* oops */
    if (verbal) errormsg("Your city seems to have been relocated");
    free(cost_ptr);
    return(cost_ptr = NULL);

  }

  /* find the adjustment costs */
  sct_cost_adjust(0, sptr, &(cost_ptr->mtrls[0]));
  mgk_cost_adjust(0, &(cost_ptr->mtrls[0]));
  return(cost_ptr);
}

/*
 * mindesg_ok - Validate minor sector designation change request
 *
 * Validates whether a minor designation can be added to a sector. Checks
 * compatibility with major designation, resource requirements, special
 * constraints (e.g., harbor water access), and affordability. Handles
 * special cases like devastation, fortification levels, and seasonal
 * restrictions for granaries.
 *
 * Parameters:
 *   x - X coordinate of the sector
 *   y - Y coordinate of the sector
 *   mdtype - Minor designation type index (0 to MIN_NUMBER-1)
 *   verbal - TRUE to display error messages, FALSE for silent validation
 *
 * Returns:
 *   TRUE - Minor designation change is valid and can proceed
 *   FALSE - Minor designation change is invalid or not affordable
 *
 * Side Effects:
 *   - May display error messages if verbal is TRUE
 *   - Calls mindesg_costs() and find_resources() for cost validation
 *   - May use map_loop() for harbor water access validation
 *   - Checks current game turn for seasonal restrictions
 *
 * Notes:
 *   - God players bypass most restrictions except parameter validation
 *   - Harbors require water access or canal connectivity
 *   - Granaries must be built before harvest season (month 8)
 *   - Fortifications have maximum level limits (MAXFORTVAL)
 *   - Some constructions are restricted by major designation type
 */
int
mindesg_ok PARM_4(int, x, int, y, int, mdtype, int, verbal)
{
  CITY_PTR c1_ptr;
  SCT_PTR sptr;
  int i, closed_harbor = FALSE, majdesg, value, is_sieged = FALSE;

  /* check the input */
  if (!XY_ONMAP(x, y)) {
    if (verbal) errormsg("ERROR: Invalid parameters to mindesg_ok()");
    return(FALSE);
  }
  sptr = &(sct[x][y]);
  if ((sptr == NULL) || (mdtype < 0) || (mdtype >= MIN_NUMBER)) {
    if (verbal) errormsg("ERROR: Invalid parameters to mindesg_ok()");
    return(FALSE);
  }
  if (minor_desg(sptr->designation, MIN_SIEGED)) {
    is_sieged = TRUE;
  }
  majdesg = major_desg(sptr->designation);

  /* set the value */
  value = MIN_START << mdtype;
  if ((is_god == FALSE) &&
      (value != MIN_DEVASTATED) &&
      (value < MIN_TRADINGPOST)) {
    if (verbal) errormsg("You may not build that construction here");
    return(FALSE);
  }

  /* check for construction */
  if (is_god == FALSE) {
    if (IS_CITY(majdesg) &&
	((c1_ptr = citybyloc(ntn_ptr, x, y)) != NULL) &&
	(c1_ptr->i_people < 0)) {
      if (verbal) errormsg("That sector is currently under construction");
      return(FALSE);
    }
  }

  /* special checks for harbors */
  if (value == MIN_HARBOR) {

    /* only in stockades or better */
    if (!IS_CITY(majdesg) ||
	(majdesg == MAJ_CACHE)) {
      if (verbal) errormsg("Harbors may only be built in population centers");
      return(FALSE);
    }

    /* check for neighboring water */
    if (is_god == FALSE) {
      global_int = FALSE;
      map_loop(x, y, 1, water_nearby);
      if (global_int == FALSE) {
	closed_harbor = TRUE;
	map_loop(x, y, 1, canal_nearby);
	if (global_int == FALSE) {
	  if (verbal) errormsg("A harbor needs access to water");
	  return(FALSE);
	}
      }
    }

  }

  /* is it possible here? */
  if (minor_desg(maj_dinfo[majdesg].info_val, value)) {
    if (verbal) errormsg("That construction is not possible in this sector");
    return(FALSE);
  }

  /* check for devastation */
  if (value == MIN_DEVASTATED) {
    if (IS_CITY(majdesg) &&
	!minor_desg(sptr->designation, value)) {
      if (verbal) errormsg("City sectors may not be devastated");
      return(FALSE);
    }
  } else {

    /* check seasons for granaries */
    if ((value == MIN_GRANARY) &&
	(MONTH(TURN) > 8)) {
      if (verbal) errormsg("The granary needs to be build before the harvest");
      return(FALSE);
    }

    /* is the construction already built? */
    if (minor_desg(sptr->designation, value) &&
	((value != MIN_FORTIFIED) ||
	 (majdesg == MAJ_CACHE) ||
	 !IS_CITY(sptr->designation))) {
      if (verbal) errormsg("Huh?  We've already got that here.");
      return(FALSE);
    }

  }

  /* now for the price checking */
  if (is_god == FALSE) {

    /* figure out the costs */
    ITEM_PTR cost_ptr;
    int hold;

    /* get the information */
    if ((item_tptr = find_resources(x, y, is_sieged)) == NULL) {
      if (verbal) errormsg("There are no resources available to this sector");
      return(FALSE);
    }

    /* get the costs and check on them */
    if ((cost_ptr = mindesg_costs(mdtype, x, y, verbal)) == NULL) {
      free(item_tptr);
      return(FALSE);
    }

    /* now compare */
    hold = TRUE;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (cost_ptr->mtrls[i] > item_tptr->mtrls[i]) {
	if (verbal) {
	  sprintf(string, "You do not have %.0f %s available",
		  (double) cost_ptr->mtrls[i],
		  mtrls_info[i].lname);
	  errormsg(string);
	}
	hold = FALSE;
      }
    }

    /* done... free them */
    free(cost_ptr);
    free(item_tptr);
    return(hold);

  }

  /* everything a_ok */
  return(TRUE);
}

/*
 * fr_accume - Accumulate resources from supply centers (helper function)
 *
 * Static helper function that accumulates resources from various supply
 * sources (cities, navies, caravans) into a single item structure. Creates
 * a new item structure if needed, or adds to an existing one. Handles
 * special talon/jewel conversion for monetary calculations.
 *
 * Parameters:
 *   i_ptr - Existing item pointer to accumulate into (NULL to create new)
 *   stash_ptr - Array of material quantities to add
 *   xloc - X coordinate for the accumulated item location
 *   yloc - Y coordinate for the accumulated item location
 *
 * Returns:
 *   ITEM_PTR - Updated or newly created item with accumulated resources
 *   Original i_ptr if stash_ptr is NULL
 *
 * Side Effects:
 *   - May allocate new item via new_item() if i_ptr is NULL
 *   - Increments itemid counter for tracking number of sources
 *   - Converts jewels to talons (10:1 ratio) for monetary calculations
 *
 * Notes:
 *   - Used by find_resources() to combine multiple supply sources
 *   - Special handling for talons includes jewel value conversion
 *   - Item coordinates set to specified location
 *   - Counter (itemid) tracks number of contributing sources
 */
static ITEM_PTR
fr_accume PARM_4(ITEM_PTR, i_ptr, itemtype *, stash_ptr, int, xloc, int, yloc)
{
  int i;

  /* check it out */
  if (stash_ptr == NULL) return(i_ptr);

  /* need a new one? */
  if (i_ptr == NULL) {
    i_ptr = new_item();
    i_ptr->xloc = xloc;
    i_ptr->yloc = yloc;
    i_ptr->itemid = 0;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      i_ptr->mtrls[i] = (itemtype) 0;
    }
  }

  /* count it up */
  i_ptr->itemid++;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (i == MTRLS_TALONS) {
      i_ptr->mtrls[i] += (stash_ptr[i] +
			  10 * stash_ptr[MTRLS_JEWELS]);
    } else {
      i_ptr->mtrls[i] += stash_ptr[i];
    }
  }

  /* done */
  return(i_ptr);
}

/*
 * find_resources - Collect all available resources within range of location
 *
 * Searches all cities, navies, and caravans within supply range of the
 * specified location and accumulates their available resources into a
 * single item structure. Used for construction cost validation and
 * resource availability calculations. Range calculations consider unit
 * types and supply status.
 *
 * Parameters:
 *   xloc - X coordinate of location needing resources
 *   yloc - Y coordinate of location needing resources
 *   insect_only - TRUE to limit range to zero (same sector only)
 *
 * Returns:
 *   ITEM_PTR - Allocated item with total available resources
 *   NULL - If no resources found within range
 *
 * Side Effects:
 *   - Allocates memory via fr_accume() that caller must free
 *   - Uses global ntn_ptr to access current nation's units
 *   - Checks supply status of navies and caravans
 *   - Uses map_within() for distance calculations
 *
 * Notes:
 *   - City range based on r10_region() calculation
 *   - Navy/caravan range uses NVSPLYDIST or COMM_I_RANGE
 *   - Only counts units in supply status
 *   - Naval units must match water/land terrain of target location
 *   - Used extensively for construction validation
 */
ITEM_PTR
find_resources PARM_3(int, xloc, int, yloc, int, insect_only)
{
  int range, check_val;
  CITY_PTR c1_ptr;
  NAVY_PTR n1_ptr;
  CVN_PTR v1_ptr;

  /* go through all cities and add in any resources */
  item_tptr = NULL;
  for (c1_ptr = ntn_ptr->city_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {

    /* set the range based on the type of "city" */
    if (!XY_INMAP(c1_ptr->xloc, c1_ptr->yloc)) continue;
    if (insect_only) {
      range = 0;
    } else {
      range = r10_region(ntn_ptr, c1_ptr,
			 &(sct[c1_ptr->xloc][c1_ptr->yloc])) / 10;
    }
    if (range == 0) {
      check_val = ((xloc == c1_ptr->xloc) && (yloc == c1_ptr->yloc));
    } else {
      check_val = map_within(c1_ptr->xloc, c1_ptr->yloc, xloc, yloc, range);
    }

    /* now check if it was within the area */
    if (check_val) {

      /* create it if it ain't there */
      item_tptr = fr_accume(item_tptr, &(c1_ptr->c_mtrls[0]), xloc, yloc);

    }
  }

  /* search for neighboring navies */
  for (n1_ptr = ntn_ptr->navy_list;
       n1_ptr != NULL;
       n1_ptr = n1_ptr->next) {

    /* check supplying status */
    if (!XY_INMAP(n1_ptr->xloc, n1_ptr->yloc)) continue;
    if (!supply_stat(n1_ptr->status)) continue;

    /* check the range */
    if (insect_only) {
      range = 0;
    } else {
      if (NVSPLYDIST == NTN_DISTIND) {
	range = COMM_I_RANGE;
      } else {
	range = NVSPLYDIST;
      }
    }
    if (range == 0) {
      check_val = ((xloc == n1_ptr->xloc) && (yloc == n1_ptr->yloc));
    } else {
      check_val = map_within(n1_ptr->xloc, n1_ptr->yloc, xloc, yloc, range);
    }

    /* now check if it was within the area */
    if ((check_val) &&
	((sct[xloc][yloc].altitude == ELE_WATER) ==
	 (sct[n1_ptr->xloc][n1_ptr->yloc].altitude == ELE_WATER))) {

      /* create it if it ain't there */
      item_tptr = fr_accume(item_tptr, &(n1_ptr->mtrls[0]), xloc, yloc);

    }
  }

  /* search for neighboring caravans */
  for (v1_ptr = ntn_ptr->cvn_list;
       v1_ptr != NULL;
       v1_ptr = v1_ptr->next) {

    /* check supplying status */
    if (!XY_INMAP(v1_ptr->xloc, v1_ptr->yloc)) continue;
    if (!supply_stat(v1_ptr->status)) continue;

    /* check the range */
    if (insect_only) {
      range = 0;
    } else {
      if (NVSPLYDIST == NTN_DISTIND) {
	range = COMM_I_RANGE;
      } else {
	range = NVSPLYDIST;
      }
    }
    if (range == 0) {
      check_val = ((xloc == v1_ptr->xloc) && (yloc == v1_ptr->yloc));
    } else {
      check_val = map_within(v1_ptr->xloc, v1_ptr->yloc, xloc, yloc, range);
    }

    /* now check if it was within the area */
    if (check_val) {

      /* create it if it ain't there */
      item_tptr = fr_accume(item_tptr, &(v1_ptr->mtrls[0]), xloc, yloc);

    }
  }

  /* done */
  return(item_tptr);
}

/*
 * tr_consume - Perform proportional material consumption from supply centers
 *
 * Static helper function that removes resources from supply centers based on
 * proportional need calculations. Handles special talon/jewel conversion for
 * monetary calculations and optionally tracks consumption in extra tracking
 * arrays. Used by take_resources() to implement proportional resource
 * distribution across multiple supply sources.
 *
 * Parameters:
 *   need_ptr - Array of required material amounts per total supply
 *   stash_ptr - Array of available materials to consume from
 *   extra_ptr - Optional tracking array for consumption amounts (may be NULL)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Decrements stash_ptr arrays by proportional consumption amounts
 *   - If extra_ptr provided, decrements it and clamps to zero
 *   - Handles talon consumption with automatic jewel conversion (10:1 ratio)
 *   - Uses global ITEMT_MTRLS array for proportional calculations
 *
 * Notes:
 *   - Talons consume from both talon and jewel reserves (10 jewels = 1 talon)
 *   - Other materials consume directly from their respective reserves
 *   - Consumption is proportional to availability vs. total need
 *   - Extra tracking helps maintain separate accounting for cities vs navies
 *   - Critical function for resource redistribution during construction
 */
static void
tr_consume PARM_3(itemtype *, need_ptr, itemtype *, stash_ptr,
		  itemtype *, extra_ptr)
{
  double tvalue;
  int i;

  /* check input */
  if ((need_ptr == NULL) ||
      (stash_ptr == NULL)) {
    return;
  }

  /* now eat it */
  if (need_ptr[MTRLS_TALONS] > 0) {
    tvalue = ( need_ptr[MTRLS_TALONS] *
	      (((double) (stash_ptr[MTRLS_TALONS] +
			  10 * stash_ptr[MTRLS_JEWELS])) /
	       ITEMT_MTRLS[MTRLS_TALONS]) );
    if (extra_ptr != NULL) {
      stash_ptr[MTRLS_TALONS] -= (itemtype) tvalue;
      extra_ptr[MTRLS_TALONS] -= (itemtype) tvalue;
      if (extra_ptr[MTRLS_TALONS] < (itemtype) 0) {
	extra_ptr[MTRLS_TALONS] = (itemtype) 0;
      }
    } else {
      if ((itemtype) tvalue <= stash_ptr[MTRLS_TALONS]) {
	stash_ptr[MTRLS_TALONS] -= (itemtype) tvalue;
      } else {
	tvalue -= (double) stash_ptr[MTRLS_TALONS];
	stash_ptr[MTRLS_TALONS] = 0;
	stash_ptr[MTRLS_JEWELS] -= (itemtype) (tvalue / 10);
      }
    }

  }
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (i == MTRLS_TALONS) continue;
    if ((need_ptr[i] > 0) &&
	(stash_ptr[i] > 0)) {
      tvalue = ( need_ptr[i] *
		((double) stash_ptr[i] / ITEMT_MTRLS[i]));
      stash_ptr[i] -= (itemtype) tvalue;
      if (extra_ptr != NULL) {
	extra_ptr[i] -= (itemtype) tvalue;
	if (extra_ptr[i] < (itemtype) 0) {
	  extra_ptr[i] = (itemtype) 0;
	}
      }
    }
  }
}

/*
 * take_resources - Remove resources from neighboring supply centers for construction
 *
 * Proportionally consumes materials from all cities, navies, and caravans within
 * supply range of the specified location. First validates that sufficient resources
 * are available before beginning consumption. Used for construction projects to
 * distribute material costs across multiple supply sources based on their relative
 * contributions to the total available pool.
 *
 * Parameters:
 *   xloc - X coordinate of location consuming resources
 *   yloc - Y coordinate of location consuming resources  
 *   take_ptr - Item structure specifying required material amounts
 *   insect - TRUE to limit range to same sector only (siege conditions)
 *
 * Returns:
 *   TRUE - Insufficient resources available, no consumption occurred
 *   FALSE - Resources successfully consumed from supply centers
 *
 * Side Effects:
 *   - Calls find_resources() to validate total availability first
 *   - Proportionally decrements materials from cities, navies, caravans
 *   - Uses tr_consume() to handle proportional consumption calculations
 *   - Frees temporary item_tptr allocation before returning
 *
 * Notes:
 *   - Cities: Uses r10_region() for range, consumes from c_mtrls and i_mtrls
 *   - Navies: Must be in supply status, range based on NVSPLYDIST
 *   - Caravans: Must be in supply status, same range calculations as navies
 *   - Naval units must match terrain type (water vs land) of target location
 *   - Siege conditions (insect=TRUE) limit all ranges to zero (same sector)
 *   - Critical validation prevents consumption beyond available resources
 */
int
take_resources PARM_4(int, xloc, int, yloc, ITEM_PTR, take_ptr, int, insect)
{
  CITY_PTR c1_ptr;
  NAVY_PTR n1_ptr;
  CVN_PTR v1_ptr;
  int i, range, check_val;

  /* must first collect all available resources in the area */
  item_tptr = find_resources(xloc, yloc, insect);
  if (item_tptr == NULL) return(TRUE);
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if ((take_ptr->mtrls[i] > 0) &&
	(ITEMT_MTRLS[i] < take_ptr->mtrls[i])) {
              free(item_tptr);
      return(TRUE);
    }
  }

  /* go through all cities and add in any resources */
  for (c1_ptr = ntn_ptr->city_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {

    /* set the range based on the type of "city" */
    if (!XY_INMAP(c1_ptr->xloc, c1_ptr->yloc)) continue;
    if (insect) {
      range = 0;
    } else {
      range = r10_region(ntn_ptr, c1_ptr,
			 &(sct[c1_ptr->xloc][c1_ptr->yloc])) / 10;
    }
    if (range == 0) {
      check_val = ((xloc == c1_ptr->xloc) && (yloc == c1_ptr->yloc));
    } else {
      check_val = map_within(c1_ptr->xloc, c1_ptr->yloc, xloc, yloc, range);
    }

    /* now check those within range */
    if (check_val) {

      /* remove proportional amount */
      tr_consume(&(take_ptr->mtrls[0]),
		 &(c1_ptr->c_mtrls[0]),
		 &(c1_ptr->i_mtrls[0]));

    }
  }

  /* search for neighboring navies */
  for (n1_ptr = ntn_ptr->navy_list;
       n1_ptr != NULL;
       n1_ptr = n1_ptr->next) {

    /* check supplying status */
    if (!XY_INMAP(n1_ptr->xloc, n1_ptr->yloc)) continue;
    if (!supply_stat(n1_ptr->status)) continue;

    /* check the range */
    if (insect) {
      range = 0;
    } else {
      if (NVSPLYDIST == NTN_DISTIND) {
	range = COMM_I_RANGE;
      } else {
	range = NVSPLYDIST;
      }
    }
    if (range == 0) {
      check_val = ((xloc == n1_ptr->xloc) && (yloc == n1_ptr->yloc));
    } else {
      check_val = map_within(n1_ptr->xloc, n1_ptr->yloc, xloc, yloc, range);
    }

    /* now check if it was within the area */
    if (check_val) {

      /* remove proportional amount */
      tr_consume(&(take_ptr->mtrls[0]),
		 &(n1_ptr->mtrls[0]),
		 (itemtype *) NULL);

    }
  }

  /* search for neighboring caravans */
  for (v1_ptr = ntn_ptr->cvn_list;
       v1_ptr != NULL;
       v1_ptr = v1_ptr->next) {

    /* check supplying status */
    if (!XY_INMAP(v1_ptr->xloc, v1_ptr->yloc)) continue;
    if (!supply_stat(v1_ptr->status)) continue;

    /* check the range */
    if (insect) {
      range = 0;
    } else {
      if (NVSPLYDIST == NTN_DISTIND) {
	range = COMM_I_RANGE;
      } else {
	range = NVSPLYDIST;
      }
    }
    if (range == 0) {
      check_val = ((xloc == v1_ptr->xloc) && (yloc == v1_ptr->yloc));
    } else {
      check_val = map_within(v1_ptr->xloc, v1_ptr->yloc, xloc, yloc, range);
    }

    /* now check if it was within the area */
    if (check_val) {

      /* remove proportional amount */
      tr_consume(&(take_ptr->mtrls[0]),
		 &(v1_ptr->mtrls[0]),
		 (itemtype *) NULL);

    }
  }

  /* clean up and leave */
  free(item_tptr);
  return(FALSE);
}

/*
 * send_resources - Redistribute materials to nearby supply centers
 *
 * Distributes resources from a source location to all cities within supply
 * range, proportional to each city's weight in the total supply network. Used
 * for resource redistribution during sector destruction, unit disbanding, or
 * resource transfers. Only cities can receive redistributed materials (not
 * navies or caravans).
 *
 * Parameters:
 *   xloc - X coordinate of source location (wrapped to map bounds)
 *   yloc - Y coordinate of source location
 *   give_ptr - Item structure containing materials to distribute
 *   insect - TRUE to limit range to same sector only (siege conditions)
 *
 * Returns:
 *   TRUE - No supply centers within range or invalid parameters
 *   FALSE - Resources successfully distributed to supply centers
 *
 * Side Effects:
 *   - Adds materials to c_mtrls arrays of cities within range
 *   - Uses proportional distribution based on city weights
 *   - Wraps xloc coordinate to handle map boundaries
 *
 * Notes:
 *   - Only distributes to cities (not navies or caravans)
 *   - Uses SUM_WEIGHTS() to calculate total distribution weight
 *   - Each city receives: (city_weight * materials) / total_weight
 *   - Range calculated via r10_region() for each city
 *   - Siege conditions (insect=TRUE) limit range to zero (same sector)
 *   - Early exit if no supply centers have positive weight
 *   - Handles coordinate wrapping for X axis automatically
 */
int
send_resources PARM_4(int, xloc, int, yloc, ITEM_PTR, give_ptr, int, insect)
{
  CITY_PTR c1_ptr;
  int check_val, range;
  int i, total_weight;

  /* input check */
  if (!Y_ONMAP(yloc) ||
      (give_ptr == NULL)) {
    return(TRUE);
  }
  xloc = (MAPX + xloc) % MAPX;

  /* is this within range of a center? */
  if ((total_weight = SUM_WEIGHTS(xloc, yloc)) <= 0) {
    return(TRUE);
  }

  /* so distribute it */
  for (c1_ptr = ntn_ptr->city_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {

    /* set the range based on the type of "city" */
    if (!XY_INMAP(c1_ptr->xloc, c1_ptr->yloc)) continue;
    if (insect) {
      range = 0;
    } else {
      range = r10_region(ntn_ptr, c1_ptr,
			 &(sct[c1_ptr->xloc][c1_ptr->yloc])) / 10;
    }
    if (range == 0) {
      check_val = ((xloc == c1_ptr->xloc) && (yloc == c1_ptr->yloc));
    } else {
      check_val = map_within(c1_ptr->xloc, c1_ptr->yloc, xloc, yloc, range);
    }

    /* now check those within range */
    if (check_val) {

      /* add in appropriate amount */
      for (i = 0; i < MTRLS_NUMBER; i++) {
	if (give_ptr->mtrls[i] > 0) {
	  c1_ptr->c_mtrls[i] += (c1_ptr->weight * give_ptr->mtrls[i]) /
	    total_weight;
	}
      }

    }
  }

  /* all done */
  return(FALSE);
}

/*
 * fort_val - Calculate defensive fortification value of sector
 *
 * Computes the total fortification value of a sector based on city
 * fortress levels, fortification minor designations, and walls. Used
 * for combat calculations to determine defensive bonuses. Different
 * sector types provide different base defensive values.
 *
 * Parameters:
 *   x - X coordinate of the sector
 *   y - Y coordinate of the sector
 *
 * Returns:
 *   int - Total fortification value (0 or positive integer)
 *
 * Side Effects:
 *   - Uses global sct_tptr for sector access
 *   - Accesses global world.np array for nation data
 *   - Calls citybyloc() to find city structures
 *
 * Notes:
 *   - City fortress value multiplied by 10 for base defense
 *   - Fortified cities get doubled value plus 10 bonus
 *   - Walls provide base 20 defense, doubled if fortified
 *   - All fortified sectors get additional 10 defense bonus
 *   - Returns 0 for unowned or invalid sectors
 *   - Critical for combat system defensive calculations
 */
int
fort_val PARM_2(int, x, int, y)
{
  int sown, value = 0;
  NTN_PTR targ_nptr;
  CITY_PTR c1_ptr;

  /* provide quick reference to the sector */
  sct_tptr = &(sct[x][y]);
  sown = sct_tptr->owner;
  if ((sown == UNOWNED) ||
      (world.np[sown] == NULL)) {
    return(value);
  }

  /* check the designation */
  if (IS_CITY(sct_tptr->designation)) {

    /* find the owner */
    targ_nptr = world.np[sown];

    /* locate the city and compute the value */
    if ((c1_ptr = citybyloc(targ_nptr, x, y)) != NULL) {

      /* compute the value */
      value += c1_ptr->fortress * 10;
      if (minor_desg(sct_tptr->designation, MIN_FORTIFIED)) {
	value *= 2;
	value += 10;
      }

    }

  } else if (major_desg(sct_tptr->designation) == MAJ_WALL) {
    value += 20;
    if (minor_desg(sct_tptr->designation, MIN_FORTIFIED)) {
      value *= 2;
      value += 10;
    }
  }

  /* check if the sector has been fortified */
  if (minor_desg(sct_tptr->designation, MIN_FORTIFIED)) {
    value += 10;
  }

  /* finito */
  return(value);
}

/*
 * defense_val - Compute terrain-based defensive value for a sector
 *
 * Calculates the natural defensive bonus provided by a sector's terrain
 * features (elevation and vegetation). These bonuses are used in combat
 * calculations to provide defensive advantages to troops based on their
 * position. Different terrain types provide different levels of defensive
 * benefit, with mountains and jungles offering the highest protection.
 *
 * Parameters:
 *   x - X coordinate of the sector
 *   y - Y coordinate of the sector
 *
 * Returns:
 *   int - Total defensive value from terrain (0 or positive integer)
 *
 * Side Effects:
 *   - Sets global sct_tptr for sector access
 *   - Read-only operation on sector data
 *
 * Notes:
 *   - Elevation bonuses: Mountain +40, Valley +30, Hill +20, others +0
 *   - Vegetation bonuses: Jungle/Swamp +30, Forest +20, Wood +10, others +0
 *   - Bonuses are cumulative (elevation + vegetation)
 *   - Does not include fortification bonuses (see fort_val())
 *   - Water sectors still receive vegetation-based bonuses if applicable
 *   - Used in conjunction with fort_val() for total defensive calculations
 *   - Critical component of the combat system's defensive mechanics
 */
int
defense_val PARM_2(int, x, int, y)
{
  int value = 0;

  /* check for existing sector */
  if (!XY_ONMAP(x, y)) {
    return(value);
  }

  /* provide quick reference to sector */
  sct_tptr = &(sct[x][y]);

  /* compute defensive adjustment from terrain */
  switch (sct_tptr->altitude) {
  case ELE_MOUNTAIN:
    value += 40;
    break;
  case ELE_VALLEY:
    value += 30;
    break;
  case ELE_HILL:
    value += 20;
    break;
  default:
    /* no adjustment */
    break;
  }

  /* compute defense from vegetation */
  switch (sct_tptr->vegetation) {
  case VEG_JUNGLE:
  case VEG_SWAMP:
    value += 30;
    break;
  case VEG_FOREST:
    value += 20;
    break;
  case VEG_WOOD:
    value += 10;
    break;
  }

  /* now send back the results */
  return(value);
}

/*
 * getmetal - Assign random metal trade good and value to a sector
 *
 * Randomly selects a metal-type trade good and assigns it to a sector along
 * with a randomized mineral value. Used during world generation to populate
 * sectors with metal resources for mining operations. Only assigns trade
 * goods to sectors that don't already have one assigned.
 *
 * Parameters:
 *   sptr - Pointer to sector structure to modify
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Sets sptr->tradegood to a random metal type or leaves unchanged
 *   - Sets sptr->minerals to a randomized value based on trade good value
 *   - Early exit if sector already has a trade good assigned
 *
 * Notes:
 *   - Only operates on sectors with tradegood == TG_NONE
 *   - Uses rand_tgood(TG_METALS, 0) to select random metal type
 *   - Mineral value randomized around base trade good value (±33%)
 *   - Minimum value is (base_value + 2) / 3 to ensure reasonable minimums
 *   - Used during initial world generation and resource discovery
 *   - Creates exploitable metal resources for metalmine construction
 *   - Part of the economic foundation for metal-based production chains
 */
void
getmetal PARM_1(SCT_PTR, sptr)
{
  int tgoodnum, rand1, rand2;

  /* assign the metal amount and type in the sector */
  if (sptr->tradegood != TG_NONE) {
    return;
  }

  /* get the trade good value */
  if ((tgoodnum = rand_tgood(TG_METALS, 0)) == TG_NONE) {
    return;
  }

  /* now build the sector based on it */
  sptr->tradegood = tgoodnum;
  rand1 = tg_info[tgoodnum].value;
  rand2 = (rand1 + 2) / 3;
  if (rand2 >= rand1) {
    sptr->minerals = rand2;
  } else {
    sptr->minerals = rand_val(rand1 - rand2) + rand2;
  }
}

/*
 * getjewel - Assign random jewel trade good and value to a sector
 *
 * Randomly selects a jewel-type trade good and assigns it to a sector along
 * with a randomized mineral value. Used during world generation to populate
 * sectors with precious stone resources for jewelmine operations. Only assigns
 * trade goods to sectors that don't already have one assigned.
 *
 * Parameters:
 *   sptr - Pointer to sector structure to modify
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Sets sptr->tradegood to a random jewel type or leaves unchanged
 *   - Sets sptr->minerals to a randomized value based on trade good value
 *   - Early exit if sector already has a trade good assigned
 *
 * Notes:
 *   - Only operates on sectors with tradegood == TG_NONE
 *   - Uses rand_tgood(TG_JEWELS, 0) to select random jewel type
 *   - Mineral value randomized around base trade good value (±33%)
 *   - Minimum value is (base_value + 2) / 3 to ensure reasonable minimums
 *   - Used during initial world generation and resource discovery
 *   - Creates exploitable jewel resources for jewelmine construction
 *   - Part of the economic foundation for luxury goods and magical components
 *   - Identical algorithm to getmetal() but for jewel-type trade goods
 */
void
getjewel PARM_1(SCT_PTR, sptr)
{
  int tgoodnum, rand1, rand2;

  /* assign the metal amount and type in the sector */
  if (sptr->tradegood != TG_NONE) {
    return;
  }

  /* get the trade good value for jewels */
  if ((tgoodnum = rand_tgood(TG_JEWELS, 0)) == TG_NONE) {
    return;
  }

  /* now build the sector based on it */
  sptr->tradegood = tgoodnum;
  rand1 = tg_info[tgoodnum].value;
  rand2 = (rand1 + 2) / 3;
  if (rand2 >= rand1) {
    sptr->minerals = rand2;
  } else {
    sptr->minerals = rand_val(rand1 - rand2) + rand2;
  }
}

/*
 * getspell - Assign random spell trade good and magical value to a sector
 *
 * Randomly selects a spell-type trade good and assigns it to a sector along
 * with a randomized mineral value representing magical potency. Used during
 * world generation to populate sectors with magical resources for shrine
 * operations and spell research. Only assigns trade goods to sectors that
 * don't already have one assigned.
 *
 * Parameters:
 *   sptr - Pointer to sector structure to modify
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Sets sptr->tradegood to a random spell type or leaves unchanged
 *   - Sets sptr->minerals to a randomized value based on trade good value
 *   - Early exit if sector already has a trade good assigned
 *
 * Notes:
 *   - Only operates on sectors with tradegood == TG_NONE
 *   - Uses rand_tgood(TG_SPELLS, 0) to select random spell type
 *   - Mineral value randomized around base trade good value (±33%)
 *   - Minimum value is (base_value + 2) / 3 to ensure reasonable minimums
 *   - Used during initial world generation and magical resource discovery
 *   - Creates exploitable magical resources for shrine construction
 *   - Part of the magical system's foundation for spell research and casting
 *   - Identical algorithm to getmetal/getjewel but for spell-type trade goods
 */
void
getspell PARM_1( SCT_PTR, sptr )
{
  int tgoodnum;
  int rand1, rand2;

  /* assign the metal amount and type in the sector */
  if (sptr->tradegood != TG_NONE) {
    return;
  }

  /* get the trade good value for magical items */
  if ((tgoodnum = rand_tgood(TG_SPELLS, 0)) == TG_NONE) {
    return;
  }

  /* now build the sector based on it */
  sptr->tradegood = tgoodnum;
  rand1 = tg_info[tgoodnum].value;
  rand2 = (rand1 + 2) / 3;
  if (rand2 >= rand1) {
    sptr->minerals = rand2;
  } else {
    sptr->minerals = rand_val(rand1 - rand2) + rand2;
  }
}

/*
 * tg_ok - Determine if a trade good can be exploited/seen by a nation
 *
 * Complex validation function that checks whether a nation has the necessary
 * technology, attributes, and sector development to detect and exploit a
 * sector's trade good resources. Combines sector designation requirements,
 * nation attribute thresholds, and habitability checks to determine resource
 * accessibility for production purposes.
 *
 * Parameters:
 *   sptr - Pointer to sector containing the trade good to check
 *   nation - Nation index to test for trade good accessibility
 *
 * Returns:
 *   TRUE - Nation can detect and exploit the trade good
 *   FALSE - Nation lacks requirements to use the trade good
 *
 * Side Effects:
 *   - Read-only operation on sector and nation data
 *   - Accesses global world.np array for nation attribute data
 *
 * Notes:
 *   - Always returns TRUE for UNOWNED nation or NULL nation pointers
 *   - Automatically passes if sector designation matches trade good requirements
 *   - City designations can exploit any trade good requiring lower designations
 *   - Farm designations are interchangeable for agricultural trade goods
 *   - Metal trade goods require sufficient MINING + METALWORK attributes
 *   - Jewel trade goods require sufficient MINING + JEWELWORK attributes
 *   - Spell trade goods validation is disabled (#ifdef NOTDONE)
 *   - All trade goods require basic sector habitability (tofood > 0)
 *   - Attribute threshold: (attr1 + attr2)/2 + 2 >= trade_good_value * 1.5
 *   - Critical for determining mine and shrine placement viability
 */
int
tg_ok PARM_2( SCT_PTR, sptr, int, nation )
{
  NTN_PTR n1_ptr;
  int mdesg, tdesg;

  /* check for default truth */
  if ((nation == UNOWNED)
      ||((n1_ptr = world.np[nation]) == NULL)){
    return(TRUE);
  }
  mdesg = major_desg(sptr->designation);
  tdesg = tg_info[sptr->tradegood].need_desg;
  if ((mdesg == tdesg) ||
      (IS_CITY(tdesg) &&
       mdesg > tdesg) ||
      (IS_FARM(tdesg) &&
       IS_FARM(mdesg))) {
    return(TRUE);
  }

  /* otherwise test using the trade good */
  if (tg_ismetal(sptr->tradegood)) {
    /* must be within a certain amount */
    if ((n1_ptr->attribute[BUTE_MINING] +
	 n1_ptr->attribute[BUTE_METALWORK]) / 2 + 2 <
	tg_info[sptr->tradegood].value * 3 / 2) {
      return(FALSE);
    }
  } else if (tg_isjewel(sptr->tradegood)) {
    if ((n1_ptr->attribute[BUTE_MINING] +
	 n1_ptr->attribute[BUTE_JEWELWORK]) / 2 + 2 <
	tg_info[sptr->tradegood].value * 3 / 2) {
      return(FALSE);
    }
#ifdef NOTDONE
  } else if (tg_isspell(sptr->tradegood)) {
#endif /* NOTDONE */
  }

  /* check the food value of the sector */
  if (tofood(sptr, nation) == 0) return(FALSE);
  return(TRUE);
}

/*
 * tofood - Calculate food production value of sector for specific nation
 *
 * Computes the food production potential of a sector based on vegetation,
 * elevation, nation-specific racial bonuses, magical powers, and trade
 * goods. This is a fundamental function for economic calculations and
 * habitability assessment. Different races and magical powers provide
 * bonuses in specific terrain types.
 *
 * Parameters:
 *   sptr - Pointer to sector structure to evaluate
 *   cntry - Nation index for racial and magical bonuses
 *
 * Returns:
 *   int - Food production value (0 or positive integer)
 *
 * Side Effects:
 *   - Read-only operation, no modifications made
 *   - Accesses global world.np array for nation data
 *
 * Notes:
 *   - Base value from vegetation and elevation tables
 *   - Dervish/Destroyer magic enables desert/ice cultivation
 *   - Amphibian magic provides jungle/swamp bonuses
 *   - Botany magic increases all food production
 *   - Elves (woodwinter races) get forest bonuses and barren penalties
 *   - Fishing and farming trade goods add production bonuses
 *   - Returns 0 for sectors that cannot support life
 */
int
tofood PARM_2(SCT_PTR, sptr, int, cntry)
{
  NTN_PTR n1_ptr;
  int foodvalue;

  /* find the normal value */
  foodvalue = veg_info[sptr->vegetation].food_val;
  if (foodvalue > 0) {
    foodvalue += ele_info[sptr->altitude].food_val;
  }

  /* adjust based on nation type */
  if ((n1_ptr = world.np[cntry]) != NULL ) {
    if (foodvalue == 0) {
      if ((MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_DERVISH) ||
	   MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DESTROYER)) &&
	  (sptr->vegetation == VEG_DESERT || sptr->vegetation == VEG_ICE))
	foodvalue = 6;
      else if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_AMPHIBIAN)) {
	if (sptr->vegetation == VEG_JUNGLE)
	  foodvalue = 6;
	else if (sptr->vegetation == VEG_SWAMP)
	  foodvalue = 5;
      }

    }

    /* botany helps */
    if ((foodvalue > 0) &&
	MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_BOTANY)) {
      foodvalue++;
    }

    /* elves are better with trees around */
    if (r_woodwinter(n1_ptr->race)) {
      if (sptr->vegetation == VEG_FOREST) foodvalue += 3;
      else if (sptr->vegetation == VEG_BARREN) foodvalue--;
    }
  }

  /* provide bonuses for special tradegoods */
  if (tg_fishing(sptr->tradegood) ||
      tg_farming(sptr->tradegood)) {
    foodvalue += tg_info[sptr->tradegood].value;
  }

  /* done with computation, so give back results */
  return( foodvalue );
}

/*
 * towood - Calculate wood production value of sector for specific nation
 *
 * Computes the wood/lumber production potential of a sector based on
 * vegetation, elevation, magical bonuses, and trade goods. Used for
 * lumberyard production calculations and construction material sourcing.
 * Simpler than food calculation with fewer nation-specific modifiers.
 *
 * Parameters:
 *   sptr - Pointer to sector structure to evaluate
 *   cntry - Nation index for magical bonuses
 *
 * Returns:
 *   int - Wood production value (0 or positive integer)
 *
 * Side Effects:
 *   - Read-only operation, no modifications made
 *   - Accesses global world.np array for nation data
 *
 * Notes:
 *   - Base value from vegetation and elevation tables
 *   - Botany magic increases wood production
 *   - Lumber trade goods provide significant bonuses
 *   - Negative base values are clamped to 0 for safety
 *   - Less complex than food calculation (no racial modifiers)
 *   - Used primarily for lumberyard designation validation and production
 */
int
towood PARM_2(SCT_PTR, sptr, int, cntry)
{
  NTN_PTR n1_ptr;
  int woodvalue;

  /* find the normal value */
  woodvalue = veg_info[sptr->vegetation].wood_val;
  if (woodvalue > 0) {
    woodvalue += ele_info[sptr->altitude].wood_val;
  } else if (woodvalue < 0) {
    /* precautionary */
    woodvalue = 0;
  }

  /* adjust based on nation type */
  if ((n1_ptr = world.np[cntry]) != NULL ) {

    /* increase production for some magical powers */
    if ((woodvalue > 0) &&
	MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_BOTANY)) {
      woodvalue++;
    }
  }

  /* provide bonuses for special tradegoods */
  if (tg_islumber(sptr->tradegood)) {
    woodvalue += tg_info[sptr->tradegood].value;
  }

  /* done with computation, so give back results */
  return( woodvalue );
}

/*
 * is_habitable - Determine if a sector can support human habitation
 *
 * Tests whether a sector's terrain conditions (elevation and vegetation) allow
 * for basic human settlement and population growth. Used as a fundamental
 * check for construction placement, population movement, and sector development
 * validation. Excludes extreme terrain that cannot support life or construction.
 *
 * Parameters:
 *   x - X coordinate of the sector to test
 *   y - Y coordinate of the sector to test
 *
 * Returns:
 *   TRUE - Sector can support human habitation and construction
 *   FALSE - Sector terrain is too extreme for habitation
 *
 * Side Effects:
 *   - Read-only operation on sector data
 *   - Accesses global sct array directly
 *
 * Notes:
 *   - Water (ELE_WATER) and mountain peaks (ELE_PEAK) are uninhabitable
 *   - Habitable vegetation: Barren, Light Vegetation, Good, Wood, Forest
 *   - Uninhabitable vegetation: Desert, Ice, Tundra, Jungle, Swamp, None
 *   - Used for basic construction validation before detailed checks
 *   - Does not consider magical powers that might enable extreme terrain use
 *   - Simpler than tofood() - only checks basic survivability
 *   - Foundation check for population centers and most constructions
 *   - Critical filter in site selection algorithms
 */
int
is_habitable PARM_2( int, x, int, y )
{
  int tval;

  /* check if the elevation makes it unlivable */
  if ((tval = sct[x][y].altitude) == ELE_WATER ||
      (tval == ELE_PEAK)) return(FALSE);

  /* now check the vegetation */
  switch (sct[x][y].vegetation) {
  case VEG_BARREN:
  case VEG_LT_VEG:
  case VEG_GOOD:
  case VEG_WOOD:
  case VEG_FOREST:
    tval = TRUE;
    break;
  default:
    /* not a habitable vegetation */
    tval = FALSE;
    break;
  }

  /* done */
  return(tval);
}

/*
 * jewel_value - Extract jewel production value from sector trade good
 *
 * Simple extraction function that returns the mineral value of a sector if
 * it contains jewel-type trade goods, or zero otherwise. Used for jewelmine
 * production calculations and economic evaluations. Part of the resource
 * value extraction system for determining sector productivity.
 *
 * Parameters:
 *   sptr - Pointer to sector structure to evaluate
 *
 * Returns:
 *   int - Jewel mineral value if sector has jewel trade good, 0 otherwise
 *
 * Side Effects:
 *   - Read-only operation on sector data
 *   - No modifications to sector or global state
 *
 * Notes:
 *   - Uses tg_isjewel() to validate trade good type
 *   - Returns sptr->minerals value for valid jewel trade goods
 *   - Returns 0 for non-jewel trade goods or empty sectors
 *   - Used by production calculations in sector_produce()
 *   - Critical for jewelmine designation validation and output
 *   - Part of unified resource value extraction API
 *   - Companion functions: metal_value(), magic_value()
 */
int
jewel_value PARM_1(SCT_PTR, sptr)
{
  int hold;

  /* check if the tradegood is a jewel type */
  if (tg_isjewel(sptr->tradegood)) {
    hold = sptr->minerals;
  } else {
    hold = 0;
  }
  return(hold);
}

/*
 * magic_value - Extract magical production value from sector trade good
 *
 * Simple extraction function that returns the mineral value of a sector if
 * it contains spell-type trade goods, or zero otherwise. Used for shrine
 * production calculations and magical research evaluations. Part of the
 * resource value extraction system for determining magical sector productivity.
 *
 * Parameters:
 *   sptr - Pointer to sector structure to evaluate
 *
 * Returns:
 *   int - Magical mineral value if sector has spell trade good, 0 otherwise
 *
 * Side Effects:
 *   - Read-only operation on sector data
 *   - No modifications to sector or global state
 *
 * Notes:
 *   - Uses tg_isspell() to validate trade good type
 *   - Returns sptr->minerals value for valid spell trade goods
 *   - Returns 0 for non-spell trade goods or empty sectors
 *   - Used by production calculations in sector_produce() (when implemented)
 *   - Critical for shrine designation validation and magical output
 *   - Part of unified resource value extraction API
 *   - Companion functions: metal_value(), jewel_value()
 *   - Supports magical system for spell research and casting
 */
int
magic_value PARM_1(SCT_PTR, sptr)
{
  int hold;

  /* check if the tradegood is a jewel type */
  if (tg_isspell(sptr->tradegood)) {
    hold = sptr->minerals;
  } else {
    hold = 0;
  }
  return(hold);
}

/*
 * metal_value - Extract metal production value from sector trade good
 *
 * Simple extraction function that returns the mineral value of a sector if
 * it contains metal-type trade goods, or zero otherwise. Used for metalmine
 * production calculations and economic evaluations. Part of the resource
 * value extraction system for determining sector productivity.
 *
 * Parameters:
 *   sptr - Pointer to sector structure to evaluate
 *
 * Returns:
 *   int - Metal mineral value if sector has metal trade good, 0 otherwise
 *
 * Side Effects:
 *   - Read-only operation on sector data
 *   - No modifications to sector or global state
 *
 * Notes:
 *   - Uses tg_ismetal() to validate trade good type
 *   - Returns sptr->minerals value for valid metal trade goods
 *   - Returns 0 for non-metal trade goods or empty sectors
 *   - Used by production calculations in sector_produce()
 *   - Critical for metalmine designation validation and output
 *   - Part of unified resource value extraction API
 *   - Companion functions: jewel_value(), magic_value()
 *   - Foundation for metal-based construction and economic systems
 */
int
metal_value PARM_1(SCT_PTR, sptr)
{
  int hold;

  /* check if the tradegood is a jewel type */
  if (tg_ismetal(sptr->tradegood)) {
    hold = sptr->minerals;
  } else {
    hold = 0;
  }
  return(hold);
}

/*
 * sector_produce - Calculate sector production output for current turn
 *
 * Computes all production output for a sector based on its designation,
 * population, seasonal factors, minor designations, and magical bonuses.
 * Handles different production types (food, metals, jewels, wood) and
 * calculates taxable income. Updates mineral depletion for extraction
 * sectors. Core function of the economic simulation.
 *
 * Parameters:
 *   xloc - X coordinate of the sector
 *   yloc - Y coordinate of the sector
 *   out_ptr - Output structure to fill with production data
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Fills out_ptr with calculated production values
 *   - May decrement sector mineral reserves for mines
 *   - Uses global variables for turn, country, and adjustment tracking
 *   - May set global adjustment flags for mineral depletion
 *
 * Notes:
 *   - Production varies by season using maj_dinfo production tables
 *   - Working population calculated via poptoworkers()
 *   - Minor designations provide multiplier bonuses (mills, blacksmiths)
 *   - Natural production bonuses for populous sectors (>100 people)
 *   - Tax value calculated based on designation and production amount
 *   - Mineral depletion probability based on population excess
 */
void
sector_produce PARM_3(int, xloc, int, yloc, SHEET_PTR, out_ptr)
{
  long prod_amount, workpop;
  int dtype, pval, count, dcval = 0;

  /* initialize structure */
  if (out_ptr == NULL) return;
  clr_memory( (char *) out_ptr, sizeof(SHEET_STRUCT) );

  /* check location */
  if (!XY_ONMAP(xloc, yloc)) return;

  /* check for quick out */
  sct_ptr = &(sct[xloc][yloc]);
  if ((sct_ptr->people == 0) ||
      ((sct_ptr->owner != country) && (is_god == FALSE))) return;

  /* calculate information */
  out_ptr->people = sct_ptr->people;

  /* now check the production */
  dtype = major_desg(sct_ptr->designation);
  pval = maj_dinfo[dtype].production[SEASON(TURN)];

  /* determine a relative figure for number of people working */
  workpop = poptoworkers(sct_ptr->people);
  if ((is_update == TRUE) &&
      (adjust_made == FALSE)) {
    dcval = (int) (100 * (sct_ptr->people - workpop)) / sct_ptr->people;
  }

  /* compute any adjustments due to minor designations */
  for (count = 0; count < MIN_NUMBER; count++) {
    if (minor_desg(sct_ptr->designation, MIN_START << count)) {

      /* found one, check for the appropriate type */
      if (((MIN_START << count) == MIN_MILL) &&
	  (dtype != MAJ_FARM) &&
	  (dtype != MAJ_LUMBERYARD))
	continue;
      if (((MIN_START << count) == MIN_BLACKSMITH) &&
	  (dtype != MAJ_METALMINE))
	continue;
      pval = (pval * (int) min_dinfo[count].multiplier) / 100;

    }
  }

  /* go find the output */
  switch (dtype) {
  case MAJ_FARM:
  case MAJ_FERTILE:
  case MAJ_FRUITFUL:
    /* calculate production of food */
    out_ptr->mtrls[MTRLS_FOOD] =
      (tofood(sct_ptr, country) * workpop * pval) / 100;
    prod_amount = out_ptr->mtrls[MTRLS_FOOD] / 5;
    break;
  case MAJ_METALMINE:
    /* calculate production of metals */
    if (tg_ok(sct_ptr, country)) {
      out_ptr->mtrls[MTRLS_METALS] =
	(metal_value(sct_ptr) * workpop * pval) / 100;
    }
    prod_amount = out_ptr->mtrls[MTRLS_METALS];
    if ((sct_ptr->minerals > 0) &&
	(dcval > rand_val(100))) {
      if (adjust_made == FALSE) {
	adjust_made = TRUE;
	adjust_xloc = xloc;
	adjust_yloc = yloc;
      }
      sct_ptr->minerals--;
    }
    break;
  case MAJ_JEWELMINE:
    /* calculate production of jewels */
    if (tg_ok(sct_ptr, country)) {
      out_ptr->mtrls[MTRLS_JEWELS] =
	(jewel_value(sct_ptr) * workpop * pval) / 100;
    }
    prod_amount = out_ptr->mtrls[MTRLS_JEWELS];
    if ((sct_ptr->minerals > 0) &&
	(dcval > rand_val(100))) {
      if (adjust_made == FALSE) {
	adjust_made = TRUE;
	adjust_xloc = xloc;
	adjust_yloc = yloc;
      }
      sct_ptr->minerals--;
    }
    break;
  case MAJ_LUMBERYARD:
    /* calculate production of wood */
    out_ptr->mtrls[MTRLS_WOOD] =
      10 * (towood(sct_ptr, country) * workpop * pval) / 100;
    prod_amount = out_ptr->mtrls[MTRLS_WOOD] / 2;
    break;
  default:
    /* people are the production amounts for the remaining types */
    prod_amount = (sct_ptr->people * pval) / 100;
    break;
  }

  /* sector benefits */
  if (sct_ptr->people > 100) {
    out_ptr->mtrls[MTRLS_WOOD] += towood(sct_ptr, country);
    out_ptr->mtrls[MTRLS_FOOD] += tofood(sct_ptr, country);
  }

  /* now calculate the taxable output (before tax percentage) */
  if (pval > 0) {
    out_ptr->mtrls[MTRLS_TALONS] =
      prod_amount * maj_dinfo[dtype].tax_value;
  }
}

/*
 * sector_consume - Calculate sector resource consumption for current turn
 *
 * Computes all resource consumption needs for a sector based on its
 * designation, population, minor designations, and seasonal factors.
 * Handles food consumption, maintenance costs, and support costs for
 * various constructions. Applies cost adjustments for devastated sectors
 * and magical effects.
 *
 * Parameters:
 *   xloc - X coordinate of the sector
 *   yloc - Y coordinate of the sector
 *   out_ptr - Output structure to fill with consumption data
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Fills out_ptr with calculated consumption values
 *   - Applies sector and magical cost adjustments
 *   - Uses global variables for turn, country, and nation eat rate
 *
 * Notes:
 *   - Food consumption based on population and NTN_D_EATRATE
 *   - Farms with granaries are self-sufficient in winter
 *   - Major designation determines base maintenance costs
 *   - Minor designations add support costs scaled by multiplier
 *   - Devastated sectors have reduced costs (1/4) except food and jewels
 *   - Seasonal factors affect food requirements for certain designations
 *   - Applies sector-specific cost adjustments via sct_cost_adjust()
 */
void
sector_consume PARM_3(int, xloc, int, yloc, SHEET_PTR, out_ptr)
{
  int count, i;
  int dtype, pval;

  /* initialize structure */
  if (out_ptr == NULL) return;
  clr_memory( (char *) out_ptr, sizeof(SHEET_STRUCT) );

  /* check location */
  if (!XY_ONMAP(xloc, yloc)) return;

  /* check for quick out */
  sct_ptr = &(sct[xloc][yloc]);
  if ((sct_ptr->owner != country) && (is_god == FALSE)) return;

  /* record population */
  out_ptr->people = sct_ptr->people;

  /* now check the consumption */
  dtype = major_desg(sct_ptr->designation);

  /* first check food consumption */
  if (!IS_FARM(dtype) ||
      ((SEASON(TURN) == WINTER_SEASON) &&
       !minor_desg(sct_ptr->designation, MIN_GRANARY))) {
    /* farms with granaries are self feeding during winter */
    out_ptr->mtrls[MTRLS_FOOD] +=
      (itemtype) (sct_ptr->people * NTN_D_EATRATE);
  }

  /* set costs for the major designation */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    out_ptr->mtrls[i] += maj_dinfo[dtype].mtrls_spt[i];
  }
  pval = maj_dinfo[dtype].multiplier;

  /* now go through support of minor designations */
  for (count = 0; count < MIN_NUMBER; count++) {

    /* that designation exists */
    if (minor_desg(sct_ptr->designation, MIN_START << count)) {

      /* charge it! */
      for (i = 0; i < MTRLS_NUMBER; i++) {
	out_ptr->mtrls[i] += min_dinfo[count].mtrls_spt[i] * pval;
      }

    }
    
  }

  /* devastated sectors only have 1/4 metal, wood and talons cost */
  if (minor_desg(sct_ptr->designation, MIN_DEVASTATED)) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if ((i != MTRLS_FOOD) &&
	  (i != MTRLS_JEWELS)) {
	out_ptr->mtrls[i] /= 4;
      }
    }
  }
  sct_cost_adjust(0, sct_ptr, &(out_ptr->mtrls[0]));
  mgk_cost_adjust(0, &(out_ptr->mtrls[0]));
}

/*
 * sct_cost_adjust - Apply sector-specific cost modifications based on minor designations
 *
 * Modifies material costs and expenses based on the minor designations present
 * in a sector. Different constructions provide economic benefits that reduce
 * certain material costs or modify pricing. Used in both construction costing
 * and ongoing maintenance calculations to reflect infrastructure advantages.
 *
 * Parameters:
 *   type - Cost type flag (bit 0: if 0, apply trading post talon adjustments)
 *   s1_ptr - Pointer to sector containing minor designations
 *   expenses - Array of material costs to modify (MTRLS_NUMBER elements)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Modifies expenses array values based on sector designations
 *   - Early exit if sector not owned by current nation
 *   - Applies percentage-based cost modifications
 *
 * Notes:
 *   - Trading Post: -2% talon costs (if type bit 0 is clear)
 *   - Mill: -5% wood costs in mill areas
 *   - Siege: +10% talon costs when under siege
 *   - Blacksmith: -10% metal costs for good workmanship
 *   - Only affects sectors owned by current nation (country)
 *   - Type parameter controls selective application of adjustments
 *   - Used in both construction and maintenance cost calculations
 *   - Represents economic benefits of infrastructure investments
 *   - Applied before magical cost adjustments via mgk_cost_adjust()
 */
void
sct_cost_adjust PARM_3(int, type, SCT_PTR, s1_ptr, itemtype *, expenses)
{
  /* check owner */
  if ((s1_ptr == NULL) ||
      (s1_ptr->owner != country)) return;

  /* check input */
  if (expenses == NULL) return;

  /* check the minor designations */
  if ((type & 1) == 0) {
    if (minor_desg(s1_ptr->designation, MIN_TRADINGPOST)) {
      /* trade tarriff supplementing some costs */
      expenses[MTRLS_TALONS] *= 98;
      expenses[MTRLS_TALONS] /= 100;
    }
  }
  if (minor_desg(s1_ptr->designation, MIN_MILL)) {
    /* reduced wood costs in mill areas */
    expenses[MTRLS_WOOD] *= 95;
    expenses[MTRLS_WOOD] /= 100;
  }
  if (minor_desg(s1_ptr->designation, MIN_SIEGED)) {
    /* higher prices when under siege */
    expenses[MTRLS_TALONS] *= 110;
    expenses[MTRLS_TALONS] /= 100;
  }
  if (minor_desg(s1_ptr->designation, MIN_BLACKSMITH)) {
    /* good metal workmanship */
    expenses[MTRLS_METALS] *= 90;
    expenses[MTRLS_METALS] /= 100;
  }
}

/*
 * s_uselevel - Calculate usage efficiency/productivity level of a sector
 *
 * Determines the productive value or efficiency rating of a sector based on
 * its major designation and resource characteristics. Used for display
 * purposes and AI decision-making to evaluate sector worth and productivity
 * potential. Returns designation-specific metrics that indicate how valuable
 * or productive a sector is within its functional category.
 *
 * Parameters:
 *   s1_ptr - Pointer to sector structure to evaluate
 *
 * Returns:
 *   int - Usage level/productivity value specific to sector designation
 *         0 if sector cannot produce food or has invalid/unknown designation
 *
 * Side Effects:
 *   - Uses global variables: is_god, country, sct_ptr
 *   - Read-only operation on sector and nation data
 *   - Calls tofood(), tg_ok() for resource evaluations
 *
 * Notes:
 *   - Returns 0 if sector has no food production capability (uninhabitable)
 *   - Jewelmine/Metalmine: Returns mineral value if exploitable by nation
 *   - Shrine: Returns magic value if exploitable by nation
 *   - Lumberyard: Returns wood production value from towood()
 *   - Farms: Returns food production value from tofood()
 *   - Other designations: Returns 0 (no specific productivity metric)
 *   - Visibility restricted by nation ownership, god status, or MW_SEEALL magic
 *   - Used for sector evaluation in strategic planning and display systems
 *   - Critical for AI assessment of sector development priorities
 */
int
s_uselevel PARM_1(SCT_PTR, s1_ptr)
{
  /* check for null value */
  if (tofood(s1_ptr, country) == 0) return(0);

  /* different values for different classes */
  switch (major_desg(s1_ptr->designation)) {
  case MAJ_JEWELMINE:
    /* jewel value */
    if ((is_god == TRUE) ||
	(UNOWNED == s1_ptr->owner) ||
	(country == s1_ptr->owner) ||
	WIZ_MAGIC(MW_SEEALL)) {
      if (tg_ok(s1_ptr, country)) {
	return(jewel_value(sct_ptr));
      }
    }
    break;
  case MAJ_METALMINE:
    /* metal value */
    if ((is_god == TRUE) ||
	(UNOWNED == s1_ptr->owner) ||
	(country == s1_ptr->owner) ||
	WIZ_MAGIC(MW_SEEALL)) {
      if (tg_ok(s1_ptr, country)) {
	return(metal_value(sct_ptr));
      }
    }
    break;
  case MAJ_SHRINE:
    /* spell casting value */
    if ((is_god == TRUE) ||
	(UNOWNED == s1_ptr->owner) ||
	(country == s1_ptr->owner) ||
	WIZ_MAGIC(MW_SEEALL)) {
      if (tg_ok(s1_ptr, country)) {
	return(magic_value(sct_ptr));
      }
    }
    break;
  case MAJ_LUMBERYARD:
    /* just the wood value */
    return(towood(s1_ptr, country));
    break;
  default:
    /* food value for farms, silch for others */
    if (IS_FARM(s1_ptr->designation)) {
      return(tofood(s1_ptr, country));
    }
    break;
  }
  return(0);
}

/*
 * exposure_value - Calculate environmental exposure damage for troops in a sector
 *
 * Computes the environmental hazard level that troops face when stationed in
 * a sector, based on vegetation, elevation, season, and nation-specific
 * resistances. Different terrain types and weather conditions cause varying
 * levels of exposure damage that can harm or kill troops over time. Nation
 * racial traits and magical powers provide resistance to specific environments.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation structure for racial/magical resistances
 *   s1_ptr - Pointer to sector structure containing terrain information
 *
 * Returns:
 *   int - Exposure damage value scaled by global EXPOSURE setting
 *         0 if EXPOSURE is disabled or sector is completely safe
 *
 * Side Effects:
 *   - Read-only operation on nation and sector data
 *   - Uses global EXPOSURE setting and SEASON(TURN) for calculations
 *
 * Notes:
 *   - Base exposure from vegetation and elevation tables (seasonal variation)
 *   - Mountaineer races ignore elevation exposure penalties
 *   - Sailor/Marine magic reduces water-based exposure (VEG_NONE)
 *   - Woodwinter races are immune to forest exposure (VEG_WOOD/FOREST)
 *   - Dervish magic halves desert/ice/tundra exposure
 *   - Destroyer magic reduces desert/ice/tundra exposure to 75%
 *   - Amphibian magic halves swamp/jungle exposure
 *   - Final value: (total_exposure * EXPOSURE + 5) / 10
 *   - Used by military systems to calculate troop attrition
 *   - Critical for strategic deployment and troop survival
 */
int
exposure_value PARM_2(NTN_PTR, n1_ptr, SCT_PTR, s1_ptr)
{
  int hold = 0, vegval, eleval;

  /* check the input */
  if ((s1_ptr == NULL) ||
      (EXPOSURE == 0)) return(hold);
  vegval = s1_ptr->vegetation;
  eleval = s1_ptr->altitude;

  /* start with the base values */
  hold += veg_info[vegval].exposure[SEASON(TURN)];
  if ((n1_ptr == NULL) ||
      !r_mountaineer(n1_ptr->race)) {
    hold += ele_info[eleval].exposure[SEASON(TURN)];
  }

  /* adjust if needed */
  if (n1_ptr != NULL) {

    /* check for vegetation specific powers */
    switch (vegval) {
    case VEG_NONE:
      /* sailors and marines can handle it */
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_SAILOR)) {
	hold *= 4;
	hold /= 5;
      }
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_MARINE)) {
	hold *= 4;
	hold /= 5;
      }
      break;
    case VEG_WOOD:
    case VEG_FOREST:
      /* some races are protected by the woods */
      if (r_woodwinter(n1_ptr->race)) {
	hold = 0;
      }
      break;
    case VEG_DESERT:
    case VEG_ICE:
    case VEG_TUNDRA:
      /* less exposure in the 'dervish' realms */
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_DERVISH)) {
	hold /= 2;
      }
      if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DESTROYER)) {
	hold *= 3;
	hold /= 4;
      }
      break;
    case VEG_SWAMP:
    case VEG_JUNGLE:
      /* the amphibians can handle it */
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_AMPHIBIAN)) {
	hold /= 2;
      }
      break;
    default:
      /* don't do nothin' */
      break;
    }

    /* check for dervish powers */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_DERVISH)) {
      hold /= 2;
    }
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DESTROYER)) {
      hold *= 3;
      hold /= 4;
    }
  }

  /* adjust and then output */
  if (hold > 0) {
    hold *= EXPOSURE;
    hold += 5;
    hold /= 10;
  }
  return(hold);
}

/* DISTORT_VISION -- Distort numerical values based on vision of a sector */
long
distort_vision PARM_4(int, xval, int, yval, long, trueval, long, distval)
{
  int sown, distortion = 25;
  long lval = trueval;

  /* check and set sector ownership */
  xval = (xval + MAPX) % MAPX;
  sown = sct[xval][yval].owner;
  if ((sown == UNOWNED) ||
      (sown < 0) ||
      (sown > MAXNTN) ||
      (ntn_tptr == NULL)) {
    sown = UNOWNED;
  }
  
  /* display the food value of a sector */
  if ((country != UNOWNED) &&
      (sown != UNOWNED) &&
      (country != sown)) {

    if (MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_ILLUSION)) {
      distortion += 25;
    }
    if (MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_THEVOID)) {
      distortion *= 2;
    }
    if (WIZ_MAGIC(MW_VISION)) {
      distortion /= 2;
    }
    if (WIZ_MAGIC(MW_SEEALL)) {
      distortion /= 2;
    }

    /* check increased viewing */
    if (is_update == FALSE) {
      distortion -= (VIS_CHECK(xval, yval) * 5);
    }
    if (distortion <= 5) distortion = 5;
    else if (distortion > 100) distortion = 100;

    /* now do the distortion */
    lval = (trueval * ((100 - distortion) +
		       nrand_val(distortion * 2, distval))) / 100;

  }
  return lval;
}

/* RAND_SECTOR -- Set the global int and global longs to a random locale */
void
rand_sector PARM_5(int, x, int, y, int, range, int, nowater, int, visionrand)
{
  int count = 0, newx, newy;

  /* keep searching until a sector is found */
  do {
    if ((visionrand) &&
	(count++ < 20)) {
      newx = x - range +
	rand_val(distort_vision(x , y, range, MAPX * MAPY) + 1);
      newy = y - range +
	rand_val(distort_vision(x , y, range, MAPX * MAPY) + 1);
    } else {
      newx = x + (range - rand_val(range + 1));
      newy = y + (range - rand_val(range + 1));
    }
    newx = (newx + MAPX) % MAPX;
  } while (!Y_ONMAP(newy) ||
	   (nowater &&
	    (sct[newx][newy].altitude == ELE_WATER)));
  global_int = newx;
  global_long = newy;
}
