/*
 * computeX.c - National Economic and Statistical Computation Engine
 *
 * This module provides comprehensive computational utilities for calculating
 * national production, budget allocations, resource distribution, and various
 * statistical metrics within the Conquer game system. It handles complex
 * economic calculations including sector weighting, resource production/consumption,
 * supply chain management, and national attribute adjustments.
 *
 * Key Functionality:
 * - City weighting and distribution systems for resource allocation
 * - Regional production and consumption calculations with magic modifiers
 * - Military unit supply chain management (armies, navies, caravans)
 * - National statistics aggregation and totals computation
 * - Economic adjustments based on civilization and wizard magic effects
 * - Territory boundary detection and area calculations
 * - Unit reorganization and numbering systems
 * - Inflation calculations and mercenary cost rate adjustments
 *
 * The module operates on global game state through various data structures
 * and provides essential economic modeling for game balance and progression.
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
#include "armyX.h"
#include "cityX.h"
#include "itemX.h"
#include "navyX.h"
#include "butesX.h"
#include "calenX.h"
#include "desigX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "activeX.h"
#include "elevegX.h"
#include "statusX.h"
#include "tgoodsX.h"
#include "weightX.h"
#include "caravanX.h"

/*
 * dflt_cityweight - Calculate default resource distribution weight for a city
 *
 * Determines the base weighting value used for resource distribution calculations
 * based on the city's designation type and population. The weight affects how
 * much influence the city has in regional resource allocation and supply chains.
 * Larger settlements (capitals, cities) have higher weights than smaller ones
 * (towns, stockades), with population-based modifiers for dynamic scaling.
 *
 * Parameters:
 *   ntnnum - Nation number that owns the city (must be valid nation)
 *   c1_ptr - Pointer to city structure (must not be NULL)
 *
 * Returns:
 *   Calculated weight value based on designation type and population,
 *   0 if city is invalid, not owned by nation, or off-map
 *
 * Side Effects:
 *   - Temporarily modifies global sct_ptr for sector access
 *   - Reads from global sector array for ownership verification
 *
 * Notes:
 *   - Capital cities get highest fixed weight (WEIGHT_CAPITAL)
 *   - Cities and towns get base weight plus population bonus
 *   - Population bonus: cities +1 per 1000 people, towns +1 per 500 people
 *   - Stockades get minimal fixed weight
 *   - Invalid designations return 0 weight
 */
int
dflt_cityweight PARM_2(int, ntnnum, CITY_PTR, c1_ptr)
{
  SCT_PTR shold_ptr = sct_ptr;
  int i, j, hold = 0;

  /* check input */
  if (c1_ptr == NULL) return(hold);

  /* find the city sector */
  i = c1_ptr->xloc;
  j = c1_ptr->yloc;
  if (!XY_ONMAP(i, j) || (sct[i][j].owner != ntnnum)) return(hold);

  /* distribution */
  sct_ptr = &(sct[i][j]);

  /* find weighting based on designation */
  switch (major_desg(sct_ptr->designation)) {
  case MAJ_CAPITAL:
    hold = WEIGHT_CAPITAL;
    break;
  case MAJ_CITY:
    hold = WEIGHT_CITY + sct_ptr->people / 1000L;
    break;
  case MAJ_TOWN:
    hold = WEIGHT_TOWN + sct_ptr->people / 500L;
    break;
  case MAJ_STOCKADE:
    hold = WEIGHT_STOCKADE;
    break;
  default:
    hold = 0;
    break;
  }
  sct_ptr = shold_ptr;
  return(hold);
}

/*
 * dist_weights - Distribute city influence weight to surrounding sectors
 *
 * Internal helper function used by map_loop to distribute a city's influence
 * weight to sectors within its communication range. This creates the weighted
 * distribution network used for resource allocation calculations. Only sectors
 * owned by the current nation (global_int) receive weight distribution.
 *
 * Parameters:
 *   x - X coordinate of target sector
 *   y - Y coordinate of target sector
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies sum_weights array for owned sectors within range
 *   - Adds CITY_WEIGHT to the sector's total influence sum
 *
 * Notes:
 *   - Called via map_loop for each sector in city's communication range
 *   - Uses global_int to identify the nation being processed
 *   - Weight accumulates if multiple cities influence the same sector
 *   - Essential for resource production and consumption calculations
 */
static void
dist_weights PARM_2(int, x, int, y)
{
  if (sct[x][y].owner == global_int) {
    SUM_WEIGHTS(x, y) += CITY_WEIGHT;
  }
}

/*
 * set_weights - Calculate and set resource distribution weights for nations
 *
 * Computes the influence network for resource distribution by calculating
 * weighting values for all cities and distributing their influence to
 * surrounding sectors. This creates the economic foundation for regional
 * production and consumption calculations. Can operate on a single nation
 * or all nations, with optional weight recalculation from defaults.
 *
 * Parameters:
 *   recalc - If TRUE, recalculate city weights from defaults; if FALSE, use existing
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Allocates/reallocates global sum_weights array if needed
 *   - Updates CITY_WEIGHT values for all cities when recalc=TRUE
 *   - Distributes weights to sectors within communication range
 *   - Modifies global nation and city pointers during processing
 *   - Handles both single nation (country != UNOWNED) and all nations
 *
 * Notes:
 *   - Uses map_loop with dist_weights for efficient area processing
 *   - Applies special rules for devastated, sieged, and new settlements
 *   - Communication range calculated by r10_region function
 *   - Critical prerequisite for production and consumption calculations
 *   - Minimum weight of 1 enforced for special circumstances
 */
void
set_weights PARM_1(int, recalc)
{
  CITY_PTR chold_ptr = city_ptr;
  SCT_PTR shold_ptr = sct_ptr;
  int cntry, start, end, range, hold;

  /* allocate if needed */
  sum_weights = new_mapshort(sum_weights);

  /* check the range */
  ntn_tptr = ntn_ptr;
  if (country == UNOWNED) {
    start = 1;
    end = MAXNTN;
  } else {
    start = country;
    end = country + 1;
  }

  /* now set the values appropriately */
  for (cntry = start; cntry < end; cntry++) {

    /* check initialization */
    ntn_ptr = world.np[cntry];
    if (ntn_ptr == NULL) continue;

    /* go through all of the cities in the nation */
    for (city_ptr = ntn_ptr->city_list;
	 city_ptr != NULL;
	 city_ptr = city_ptr->next) {

      /* check the city location */
      if (!XY_INMAP(CITY_XLOC, CITY_YLOC)) continue;

      /* redo the calculation if needed */
      if (recalc == TRUE) {

	/* find the default weighting */
	hold = dflt_cityweight(cntry, city_ptr);

	/* store resulting weight */
	if (CITY_WEIGHT <= hold) {
	  CITY_WEIGHT = (uns_char) hold;
	}
      }

      /* distribute surrounding weights */
      sct_ptr = &(sct[CITY_XLOC][CITY_YLOC]);
      range = r10_region(ntn_ptr, city_ptr, sct_ptr) / 10;
      if ((CITY_PEOPLE < 0) ||
	  (minor_desg(sct_ptr->designation, MIN_DEVASTATED | MIN_SIEGED))) {
	/* keep sieges, devastation and new towns tight */
	if (CITY_WEIGHT == 0) {
	  CITY_WEIGHT = 1;
	}
      }

      /* now go about the area */
      global_int = cntry;
      map_loop(CITY_XLOC, CITY_YLOC, range, dist_weights);

    }

  }

  /* now reset the nation pointer */
  ntn_ptr = ntn_tptr;
  city_ptr = chold_ptr;
  sct_ptr = shold_ptr;
}

/* The variable to be used among the local routines */
static SHEET_PTR region_spread;
static int need_all;

/*
 * rg_prodval - Calculate sector's contribution to regional resource production
 *
 * Internal helper function that computes how much a specific sector contributes
 * to the regional production totals based on its resource output and the city's
 * influence weight. Distributes sector production proportionally according to
 * the weight distribution system, accumulating results in region_spread.
 *
 * Parameters:
 *   x - X coordinate of producing sector
 *   y - Y coordinate of producing sector
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Accumulates production values in global region_spread structure
 *   - Updates people count, sector count, and material totals
 *   - Modifies type-specific arrays when need_all is TRUE
 *   - Calls sector_produce to get base production values
 *
 * Notes:
 *   - Only processes sectors owned by current nation with population > 0
 *   - Skips sectors with zero total weight (no city influence)
 *   - Applies proportional distribution: (sector_output * CITY_WEIGHT) / total_weight
 *   - Handles overflow protection with BIGITEM limits
 *   - Called via map_loop from region_produce function
 *   - Essential for calculating what a city can access from its region
 */
static void
rg_prodval PARM_2(int, x, int, y)
{
  SHEET_STRUCT sector_spread;
  int total_weight, dtype, count;
  itemtype share;
  long share_peop;

  /* check if it is a target sector */
  if ((sct[x][y].owner == country) &&
      (sct[x][y].people > 0L)) {

    /* find balance of sector */
    total_weight = SUM_WEIGHTS(x, y);

    /* doesn't have any place to go */
    if (total_weight == 0) return;

    /* now distribute sector production */
    sector_produce(x, y, &sector_spread);

    /* find the designation type */
    dtype = major_desg(sct[x][y].designation);

    /* count information relevant to all sectors */
    region_spread->people += 
      (share_peop = ((sector_spread.people * CITY_WEIGHT)
		     / total_weight));
    region_spread->sectors++;

    /* take care of some detailed sector specifics */
    share = (sector_spread.mtrls[MTRLS_TALONS] * CITY_WEIGHT) / total_weight;
    if (need_all == TRUE) {
      if (region_spread->type_talons[dtype] + share > BIGITEM) {
	region_spread->type_talons[dtype] = BIGITEM;
      } else {
	region_spread->type_talons[dtype] += share;
      }
      region_spread->type_people[dtype] += share_peop;
      region_spread->type_sectors[dtype]++;
    }

    /* raw materials produced */
    for (count = 0; count < MTRLS_NUMBER; count++) {
      share = (sector_spread.mtrls[count] * CITY_WEIGHT) / total_weight;
      if (region_spread->mtrls[count] + share > BIGITEM) {
	region_spread->mtrls[count] = BIGITEM;
      } else {
	region_spread->mtrls[count] += share;
      }
    }

  }
}

/*
 * region_produce - Calculate total resource production within city's communication range
 *
 * Computes the aggregate resource production available to a specified city
 * from all sectors within its communication range. This includes raw materials,
 * population, and detailed sector-type breakdowns. The calculation considers
 * weighted distribution based on city influence and sector ownership.
 *
 * Parameters:
 *   cname - Name of the city to calculate production for (must exist)
 *   fullinfo - If TRUE, calculate detailed type-specific information; if FALSE, basic totals only
 *
 * Returns:
 *   Dynamically allocated SHEET_PTR containing production totals,
 *   NULL if city not found, invalid, or not owned by current nation
 *
 * Side Effects:
 *   - Allocates memory for SHEET_STRUCT (caller must free)
 *   - Sets global need_all flag for detailed calculations
 *   - Uses map_loop with rg_prodval to process regional sectors
 *   - Modifies global city_ptr and sct_ptr during processing
 *
 * Notes:
 *   - Communication range determined by r10_region function
 *   - Only processes sectors owned by current nation
 *   - Weighted distribution ensures proportional resource allocation
 *   - Essential for economic planning and resource management
 *   - HEREIAM comment indicates planned minor designation adjustments
 *   - Memory allocation failure triggers abort sequence
 */
SHEET_PTR
region_produce PARM_2(char *, cname, int, fullinfo)
{
  int xloc, yloc, range;

  /* first check the national validity */
  need_all = fullinfo;
  if ((ntn_ptr == NULL) || ((city_ptr = citybyname(cname)) == NULL)) {
    return((SHEET_PTR) NULL);
  }

  /* set the sector */
  xloc = CITY_XLOC;
  yloc = CITY_YLOC;

  /* check the sector */
  if (!XY_ONMAP(xloc, yloc)) {
    return((SHEET_PTR) NULL);
  }
  sct_ptr = &(sct[xloc][yloc]);
  if ((sct_ptr->owner == UNOWNED) ||
      (sct_ptr->owner != country) ||
      !IS_CITY(sct_ptr->designation)) {
    return((SHEET_PTR) NULL);
  }

  /* now set bounds and other information */
  range = r10_region(ntn_ptr, city_ptr, sct_ptr) / 10;

  /* set region storage */
  if ((region_spread = (SHEET_PTR) malloc(sizeof(SHEET_STRUCT))) == NULL) {
    fprintf(fupdate, "Unable to allocate SHEET_STRUCT in region_produce()\n");
    abrt();
  }

  /* initialize storage */
  clr_memory((char *) region_spread, sizeof(SHEET_STRUCT));

  /* just to be safe */
  region_spread->sectors = 0;

  /* calculate for region */
  map_loop(xloc, yloc, range, rg_prodval);

  /* HEREIAM -- adjust the regional calculations based on minor desgs */  

  /* give back the results */
  return(region_spread);
}

/*
 * rg_consval - Calculate sector's contribution to regional resource consumption
 *
 * Internal helper function that computes how much a specific sector consumes
 * from the regional resource pool based on its consumption requirements and
 * available city resources. Handles complex calculations including talon costs
 * based on available resources and jewel multipliers for enhanced economics.
 *
 * Parameters:
 *   x - X coordinate of consuming sector
 *   y - Y coordinate of consuming sector
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Accumulates consumption values in global region_spread structure
 *   - Updates people count, sector count, and material consumption totals
 *   - Modifies type-specific arrays when need_all is TRUE
 *   - Calls sector_consume and find_resources for calculations
 *   - Frees item_tptr memory after processing
 *
 * Notes:
 *   - Only processes sectors owned by current nation
 *   - Skips sectors without available resources (find_resources returns NULL)
 *   - Special handling for talon consumption with jewel enhancement (10x multiplier)
 *   - Proportional consumption based on available city resources vs sector needs
 *   - Handles divide-by-zero with total_weight checks
 *   - Called via map_loop from region_consume function
 *   - Critical for supply chain and economic balance calculations
 */
static void
rg_consval PARM_2(int, x, int, y)
{
  SHEET_STRUCT sector_spread;
  double share;
  long share_peop;
  int dtype, count, total_weight;

  /* calculate for region */
  if (sct[x][y].owner == country) {

    /* find resource balance of sector */
    item_tptr = find_resources(x, y,
			       minor_desg(sct[x][y].designation, MIN_SIEGED));
    total_weight = SUM_WEIGHTS(x, y);

    /* no go */
    if (item_tptr == NULL) return;

    /* now find the sector consumption */
    sector_consume(x, y, &sector_spread);

    /* find the designation type */
    dtype = major_desg(sct[x][y].designation);

    /* count information relevant to all sectors */
    if (total_weight > 0) {
      region_spread->people += 
	(share_peop = (sector_spread.people
		       * CITY_WEIGHT) / total_weight);
    }  else {
      share_peop = 0;
    }
    share = (double) 0;
    if ((sector_spread.mtrls[MTRLS_TALONS] > 0) &&
	(ITEMT_MTRLS[MTRLS_TALONS] >= sector_spread.mtrls[MTRLS_TALONS])) {
      share = ((double)sector_spread.mtrls[MTRLS_TALONS] *
	       (CITY_MTRLS[MTRLS_TALONS] + 10 * CITY_MTRLS[MTRLS_JEWELS])) /
		 ITEMT_MTRLS[MTRLS_TALONS];
      if (region_spread->mtrls[MTRLS_TALONS] + (itemtype) share > BIGITEM) {
	region_spread->mtrls[MTRLS_TALONS] = BIGITEM;
      } else {
	region_spread->mtrls[MTRLS_TALONS] += (itemtype) share;
      }
    }
    region_spread->sectors++;

    /* take care of detailed sector specifics */
    if (need_all) {
      if (region_spread->type_talons[dtype] + (itemtype) share > BIGITEM) {
	region_spread->type_talons[dtype] = BIGITEM;
      } else {
	region_spread->type_talons[dtype] += (itemtype) share;
      }
      region_spread->type_people[dtype] += share_peop;
      region_spread->type_sectors[dtype]++;
    }

    /* now calculate the other commodities; skipping talons */
    for (count = 0; count < MTRLS_NUMBER; count++) {

      /* skip talons... already done */
      if (count == MTRLS_TALONS) continue;

      /* now distribute the materials */
      if (ITEMT_MTRLS[count] > 0) {
	share = ((double) sector_spread.mtrls[count] *
		 CITY_MTRLS[count]) / ITEMT_MTRLS[count];
	if (region_spread->mtrls[count] + (itemtype) share > BIGITEM) {
	  region_spread->mtrls[count] = BIGITEM;
	} else {
	  region_spread->mtrls[count] += (itemtype) share;
	}
      } else if ((is_update == FALSE) &&
		 (ITEMT_ID > 0)) {
	share = ((double) sector_spread.mtrls[count]) / ITEMT_ID;
	if (region_spread->mtrls[count] + (itemtype) share > BIGITEM) {
	  region_spread->mtrls[count] = BIGITEM;
	} else {
	  region_spread->mtrls[count] += (itemtype) share;
	}
      }

    }

    /* make sure not to eat up the memory */
    free(item_tptr);
  }
}

/*
 * region_consume - Calculate total resource consumption within city's range
 *
 * Computes the comprehensive resource consumption requirements for a specified
 * city region, including sector consumption, military unit supply costs (armies,
 * navies, caravans), and economic overhead. This provides the complete economic
 * demand picture for resource planning and budget calculations.
 *
 * Parameters:
 *   cname - Name of the city to calculate consumption for (must exist)
 *   fullinfo - If TRUE, calculate detailed type-specific information; if FALSE, basic totals only
 *
 * Returns:
 *   Dynamically allocated SHEET_PTR containing consumption totals,
 *   NULL if city not found, invalid, or not owned by current nation
 *
 * Side Effects:
 *   - Allocates memory for SHEET_STRUCT (caller must free)
 *   - Processes all military units within communication range
 *   - Calls army_support, navy_support, cvn_support for unit costs
 *   - Applies magic cost adjustments via mgk_cost_adjust
 *   - Updates unit counts (army_men, ship_holds, caravan_wagons)
 *
 * Notes:
 *   - Monster nations have zero consumption (early return)
 *   - No processing if MAXSUPPLIES is 0 (supply system disabled)
 *   - Complex supply chain validation with resource availability checks
 *   - Handles different unit types: regular troops, leaders, monsters
 *   - Special talon cost calculations with jewel enhancement
 *   - Accumulates costs: army_cost, navy_cost, cvn_cost added to material totals
 *   - Essential for economic balance and military sustainability
 */
SHEET_PTR
region_consume PARM_2(char *, cname, int, fullinfo)
{
  int i, xloc, yloc, range;
  double holddbl;
  itemtype acct_vals[MTRLS_NUMBER];

  /* first check the national validity */
  need_all = fullinfo;
  if ((ntn_ptr == NULL) || ((city_ptr = citybyname(cname)) == NULL)) {
    return((SHEET_PTR) NULL);
  }

  /* set the sector */
  xloc = CITY_XLOC;
  yloc = CITY_YLOC;

  /* check the sector */
  if (!XY_ONMAP(xloc, yloc)) {
    return((SHEET_PTR) NULL);
  }
  sct_ptr = &(sct[xloc][yloc]);
  if ((sct_ptr->owner == UNOWNED) ||
      (sct_ptr->owner != country) ||
      !IS_CITY(sct_ptr->designation)) {
    return((SHEET_PTR) NULL);
  }

  /* now set bounds and other information */
  range = r10_region(ntn_ptr, city_ptr, sct_ptr) / 10;

  /* set region storage */
  if ((region_spread = (SHEET_PTR) malloc(sizeof(SHEET_STRUCT))) == NULL) {
    fprintf(fupdate, "Unable to allocate SHEET_STRUCT in region_consume()\n");
    abrt();
  }

  /* initialize storage */
  clr_memory( (char *) region_spread, sizeof(SHEET_STRUCT) );

  /* no consumption for monster nations! */
  if (n_ismonster(ntn_ptr->active)) {
    return(region_spread);
  }

  /* do the sector consumption */
  map_loop(xloc, yloc, range, rg_consval);

  /* no need to continue if there is no supplying */
  if (MAXSUPPLIES == 0) return(region_spread);

  /* check for supplying of troops, navies and carvans */
  army_tptr = NULL;
  item_tptr = NULL;
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {

    /* anywhere in region */
    if (map_within(CITY_XLOC, CITY_YLOC, ARMY_XLOC, ARMY_YLOC, range) &&
	a_issupplyable(army_ptr, CITY_XLOC, CITY_YLOC, FALSE)) {

      /* get support costs */
      army_support(army_ptr, &(acct_vals[0]), 1);

      /* find resource balance of sector */
      if ((item_tptr = find_resources(ARMY_XLOC, ARMY_YLOC,
				      splyinsect_stat(ARMY_STAT)))
	  == NULL)
	continue;
      if (ITEMT_MTRLS[MTRLS_TALONS] <
	  acct_vals[MTRLS_TALONS] + region_spread->army_cost) {
	/* empty supply sector */
	free(item_tptr);
	continue;
      }

      /* check the type */
      for (i = 0; i < MTRLS_NUMBER; i++) {
	if (acct_vals[i] != 0) {
	  switch (i) {
	  case MTRLS_TALONS:
	    /* find amount that must be contributed */
	    if (ITEMT_MTRLS[MTRLS_TALONS] > 0) {
	      holddbl = (100 * (double) (CITY_MTRLS[MTRLS_TALONS] +
					 10 * CITY_MTRLS[MTRLS_JEWELS]))
		/ ((double) ITEMT_MTRLS[MTRLS_TALONS]);
	      region_spread->army_cost +=
		(itemtype) (holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  case MTRLS_JEWELS:
	    /* compute any jewel costs */
	    if (ITEMT_MTRLS[i] > 0) {
	      holddbl = (100 * (double) CITY_MTRLS[i]) /
		ITEMT_MTRLS[i];
	      region_spread->monst_jewels +=
		(itemtype) ((double) holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  default:
	    /* everything else is just a bit of give and take */
	    if (ITEMT_MTRLS[i] > 0) {
	      holddbl = (100 * (double) CITY_MTRLS[i]) /
		ITEMT_MTRLS[i];
	      region_spread->mtrls[i] +=
		(itemtype) (holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  }
	}
      }

      /* count 'em */
      if (a_ismonster(ARMY_TYPE)) {
	region_spread->monst_troops += ARMY_SIZE;
      } else if (a_isleader(ARMY_TYPE)) {
	region_spread->army_men++;
      } else {
	region_spread->army_men += ARMY_SIZE;
      }
      free(item_tptr);
    }
  }
  region_spread->mtrls[MTRLS_TALONS] += region_spread->army_cost;
  region_spread->mtrls[MTRLS_JEWELS] += region_spread->monst_jewels;

  /* now go through all of the navy units */
  for (navy_ptr = ntn_ptr->navy_list;
       navy_ptr != NULL;
       navy_ptr = navy_ptr->next) {

    /* check if it is in range and supplyable */
    if (n_issupplyable(navy_ptr, CITY_XLOC, CITY_YLOC, FALSE) &&
	map_within(CITY_XLOC, CITY_YLOC, NAVY_XLOC, NAVY_YLOC, range)) {
      /* get the costs */
      navy_support(navy_ptr, &(acct_vals[0]), 1);
      mgk_cost_adjust(2, &(acct_vals[0]));

      /* check how much of expenses must be met */
      if ((item_tptr =
	   find_resources(NAVY_XLOC, NAVY_YLOC,
			  (sct[NAVY_XLOC][NAVY_YLOC].altitude
			   == ELE_WATER))) == NULL)
	continue;
      if (ITEMT_MTRLS[MTRLS_TALONS] <
	  acct_vals[MTRLS_TALONS] + region_spread->army_cost +
	  region_spread->navy_cost) {
	/* empty supply sector */
	free(item_tptr);
	continue;
      }
      
      for (i = 0; i < MTRLS_NUMBER; i++) {
	if (acct_vals[i] > 0) {
	  switch (i) {
	  case MTRLS_TALONS:
	    /* find amount that must be contributed */
	    if (ITEMT_MTRLS[MTRLS_TALONS] > 0) {
	      holddbl = (100 * (double) (CITY_MTRLS[MTRLS_TALONS] +
					 10 * CITY_MTRLS[MTRLS_JEWELS]))
		/ (double) (ITEMT_MTRLS[MTRLS_TALONS]);
	      region_spread->navy_cost +=
		(itemtype) (holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  default:
	    /* everything else is just a bit of give and take */
	    if (ITEMT_MTRLS[i] > 0) {
	      holddbl = (100 * (double) CITY_MTRLS[i]) /
		ITEMT_MTRLS[i];
	      region_spread->mtrls[i] +=
		(itemtype) (holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  }
	}
      }

      /* count up the holds */
      for (xloc = 0; xloc < NSHP_NUMBER; xloc++) {
	region_spread->ship_holds += navy_holds(navy_ptr, xloc);
      }
      free(item_tptr);

    }
  }
  /* cost for maintaining the ships */
  region_spread->mtrls[MTRLS_TALONS] += region_spread->navy_cost;

  /* tromp through the caravans */
  for (cvn_ptr = ntn_ptr->cvn_list;
       cvn_ptr != NULL;
       cvn_ptr = cvn_ptr->next) {

    /* check if it is within range */
    if (map_within(CVN_XLOC, CVN_YLOC, CITY_XLOC, CITY_YLOC, range) &&
	v_issupplyable(cvn_ptr, CITY_XLOC, CITY_YLOC, FALSE)) {

      /* compute the costs */
      cvn_support(cvn_ptr, &(acct_vals[0]), 1);

      /* find resource balance of sector */
      if ((item_tptr = find_resources(CVN_XLOC, CVN_YLOC,
				      splyinsect_stat(CVN_STAT))) == NULL)
	continue;
      if (ITEMT_MTRLS[MTRLS_TALONS] <
	  acct_vals[MTRLS_TALONS] + region_spread->army_cost +
	  region_spread->navy_cost + region_spread->cvn_cost) {
	/* useless supply sector */
	free(item_tptr);
	continue;
      }
      for (i = 0; i < MTRLS_NUMBER; i++) {
	if (acct_vals[i] > 0) {
	  switch (i) {
	  case MTRLS_TALONS:
	    /* keep track of the talons */
	    if (ITEMT_MTRLS[MTRLS_TALONS] > 0) {
	      holddbl = (100 * (double) (CITY_MTRLS[MTRLS_TALONS] +
					 10 * CITY_MTRLS[MTRLS_JEWELS]))
		/ (double) (ITEMT_MTRLS[MTRLS_TALONS]);
	      region_spread->cvn_cost +=
		(itemtype) (holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  default:
	    /* just do it */
	    if (ITEMT_MTRLS[i] > 0) {
	      holddbl = (100 * (double) CITY_MTRLS[i]) /
		ITEMT_MTRLS[i];
	      region_spread->mtrls[i] +=
		(itemtype) (holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  }
	}
      }

      /* count them up */
      region_spread->caravan_wagons += CVN_SIZE * WAGONS_IN_CVN;
    }
  }
  region_spread->mtrls[MTRLS_TALONS] += region_spread->cvn_cost;

  /* give back the results */
  return(region_spread);
}

/*
 * ntn_totals - Calculate comprehensive national statistics and resource totals
 *
 * Aggregates all national assets including materials, population, military units,
 * ships, wagons, and territorial information. This provides a complete statistical
 * overview of a nation's economic and military strength for reporting and
 * game balance calculations. Processes all entities owned by the nation.
 *
 * Parameters:
 *   nation - Nation number to calculate totals for (must be valid)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Updates all totals fields in the nation structure (ntn_tptr)
 *   - Zeroes existing totals before recalculation
 *   - Counts sectors, unsupplied sectors, population, military units
 *   - Aggregates materials from cities, items, armies, navies, caravans
 *   - Calculates ship counts, wagon counts, monster counts
 *
 * Notes:
 *   - Processes entire nation territory within edge boundaries
 *   - Handles map wraparound with modulo arithmetic ((i + MAPX) % MAPX)
 *   - Distinguishes between leaders, regular troops, and monsters
 *   - Includes civilian population on ships (galleys) and caravans
 *   - Counts unsupplied sectors using sum_weights array
 *   - Essential for national reporting, diplomacy, and victory conditions
 *   - Updates: tmil, tciv, tmonst, tleaders, tsctrs, tunsctrs, tships, twagons
 */
void
ntn_totals PARM_1(int, nation)
{
  register int i, j;

  /* check the information */
  if ((nation < 0) || (nation >= ABSMAXNTN) ||
      ((ntn_tptr = world.np[nation]) == NULL)) return;

  /* initialize nation values */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    ntn_tptr->mtrls[i] = (itemtype) 0;
  }
  ntn_tptr->tships = 0;
  ntn_tptr->twagons = 0;
  ntn_tptr->tmonst = 0;
  ntn_tptr->tleaders = 0;
  ntn_tptr->tsctrs = 0;
  ntn_tptr->tunsctrs = 0;
  ntn_tptr->tmil = 0L;
  ntn_tptr->tciv = 0L;

  /* go through all of the sectors */
  for (i = ntn_tptr->leftedge; i <= ntn_tptr->rightedge; i++) {
    for (j = ntn_tptr->bottomedge; j <= ntn_tptr->topedge; j++) {
      if (sct[(i + MAPX) % MAPX][j].owner == nation) {
	ntn_tptr->tciv += sct[(i + MAPX) % MAPX][j].people;
	ntn_tptr->tsctrs++;
	if ((sum_weights != NULL) &&
	    (SUM_WEIGHTS((i + MAPX) % MAPX, j) == 0))
	  ntn_tptr->tunsctrs++;
      }
    }
  }

  /* summarize all of the city totals */
  for (city_ptr = ntn_tptr->city_list;
       city_ptr != NULL;
       city_ptr = city_ptr->next) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      ntn_tptr->mtrls[i] += CITY_MTRLS[i];
    }
  }

  /* include all of the commodities */
  for (item_ptr = ntn_tptr->item_list;
       item_ptr != NULL;
       item_ptr = item_ptr->next) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      ntn_tptr->mtrls[i] += ITEM_MTRLS[i];
    }
  }

  /* count all of the little toy soldiers */
  for (army_ptr = ntn_tptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {
    if (a_isleader(ARMY_TYPE)) {
      ntn_tptr->tmil++;
      ntn_tptr->tleaders++;
    } else if (a_ismonster(ARMY_TYPE)) {
      ntn_tptr->tmonst += ARMY_SIZE;
    } else {
      ntn_tptr->tmil += ARMY_SIZE;
    }
  }

  /* account for all of the people on ships */
  for (navy_ptr = ntn_tptr->navy_list;
       navy_ptr != NULL;
       navy_ptr = navy_ptr->next) {
    for (i = 0; i < NSHP_NUMBER; i++) {
      ntn_tptr->tships += (NAVY_CNTSHIPS(i, N_LIGHT) +
			   NAVY_CNTSHIPS(i, N_MEDIUM) + 
			   NAVY_CNTSHIPS(i, N_HEAVY));
    }
    for (i = 0; i < MTRLS_NUMBER; i++) {
      ntn_tptr->mtrls[i] += NAVY_MTRLS[i];
    }
    ntn_tptr->tciv += (long) NAVY_PEOP *
      navy_holds(navy_ptr, NSHP_GALLEYS);
  }

  /* account for all of the people on caravans */
  for (cvn_ptr = ntn_tptr->cvn_list;
       cvn_ptr != NULL;
       cvn_ptr = cvn_ptr->next) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      ntn_tptr->mtrls[i] += CVN_MTRLS[i];
    }
    ntn_tptr->twagons += WAGONS_IN_CVN * CVN_SIZE;
    ntn_tptr->tciv += (long) CVN_PEOP * CVN_SIZE;
  }
}

/*
 * adjust_production - Apply magic and national modifiers to resource production
 *
 * Modifies raw production values based on active civilization and wizard magic
 * effects, tax rates, and other national attributes. This system allows magic
 * research to provide economic benefits and creates strategic choices in
 * magical development paths. Handles both basic totals and detailed type breakdowns.
 *
 * Parameters:
 *   produce_ptr - Pointer to production sheet to modify (must not be NULL)
 *   fullinfo - If TRUE, apply adjustments to type-specific arrays; if FALSE, basic totals only
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies production values in-place within produce_ptr
 *   - Applies percentage-based adjustments to material quantities
 *   - Updates both general totals and type-specific arrays when fullinfo=TRUE
 *
 * Notes:
 *   - Accountant magic: +20% talon income (MC_ACCOUNTANT)
 *   - Socialism magic: +10% talon income (MC_SOCIALISM)
 *   - Tax rate: Applied as percentage to talon income
 *   - Farming magic: +20% food production (MC_FARMING)
 *   - Earth magic: +10% food production (MW_EARTH, stacks with farming)
 *   - Druidism magic: +20% wood production (MW_DRUIDISM)
 *   - Miner magic: +20% jewels and metals production (MC_MINER)
 *   - All adjustments use integer arithmetic with percentage scaling
 *   - Critical for game balance and magic system effectiveness
 */
void
adjust_production PARM_2( SHEET_PTR, produce_ptr, int, fullinfo )
{
  int i;

  /* check input */
  if ((produce_ptr == NULL) || (ntn_ptr == NULL)) return;

  /* check for better book keeping */
  if (CIV_MAGIC(MC_ACCOUNTANT)) {
    produce_ptr->mtrls[MTRLS_TALONS] *= 120;
    produce_ptr->mtrls[MTRLS_TALONS] /= 100;
    if (fullinfo) {
      for (i = 0; i < MAJ_NUMBER; i++) {
	produce_ptr->type_talons[i] *= 120;
	produce_ptr->type_talons[i] /= 100;
      }
    }
  }

  /* check for health plan costs */
  if (CIV_MAGIC(MC_SOCIALISM)) {
    produce_ptr->mtrls[MTRLS_TALONS] *= 110;
    produce_ptr->mtrls[MTRLS_TALONS] /= 100;
    if (fullinfo) {
      for (i = 0; i < MAJ_NUMBER; i++) {
	produce_ptr->type_talons[i] *= 110;
	produce_ptr->type_talons[i] /= 100;
      }
    }
  }

  /* handle the taxes */
  produce_ptr->mtrls[MTRLS_TALONS] *= ntn_ptr->attribute[BUTE_TAXRATE];
  produce_ptr->mtrls[MTRLS_TALONS] /= 100;
  if (fullinfo) {
    for (i = 0; i < MAJ_NUMBER; i++) {
      produce_ptr->type_talons[i] *= ntn_ptr->attribute[BUTE_TAXRATE];
      produce_ptr->type_talons[i] /= 100;
    }
  }

  /* check food production increase */
  i = 100;
  if (CIV_MAGIC(MC_FARMING)) {
    i += 20;
  }
  if (WIZ_MAGIC(MW_EARTH)) {
    i += 10;
  }
  produce_ptr->mtrls[MTRLS_FOOD] *= i;
  produce_ptr->mtrls[MTRLS_FOOD] /= 100;

  /* check wood production increase */
  if (WIZ_MAGIC(MW_DRUIDISM)) {
    produce_ptr->mtrls[MTRLS_WOOD] *= 120;
    produce_ptr->mtrls[MTRLS_WOOD] /= 100;
  }

  /* mining production increase */
  if (CIV_MAGIC(MC_MINER)) {
    produce_ptr->mtrls[MTRLS_JEWELS] *= 120;
    produce_ptr->mtrls[MTRLS_JEWELS] /= 100;
    produce_ptr->mtrls[MTRLS_METALS] *= 120;
    produce_ptr->mtrls[MTRLS_METALS] /= 100;
  }
}

/*
 * mgk_cost_adjust - Apply magic cost reduction modifiers to expenses
 *
 * Reduces various expense categories based on active magic powers, providing
 * economic incentives for magical research investments. The type parameter
 * determines which magic categories apply, allowing selective cost reductions
 * for military, naval, construction, and resource processing activities.
 *
 * Parameters:
 *   type - Bitmask indicating expense categories (1=military, 2=naval, 4=construction)
 *   expenses - Array of material costs to modify (must not be NULL)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies expense values in-place within expenses array
 *   - Applies percentage-based cost reductions
 *   - Stacks multiple magic effects where applicable
 *
 * Notes:
 *   - Military (type & 1): Warrior chain reduces talon costs (20-40% total)
 *   - Military (type & 1): Orc chain reduces jewel costs (20-60% total)
 *   - Naval (type & 2): Sailor chain reduces talon costs (20-40% total)
 *   - Construction (type & 4): Architect reduces metals/talons/wood costs (20%)
 *   - Universal: Jeweler reduces jewel costs (20%), Metalcraft reduces metal costs (20%)
 *   - Universal: Woodcraft reduces wood costs (20%)
 *   - Magic effects stack additively within categories
 *   - Essential for magic system balance and economic strategy
 */
void
mgk_cost_adjust PARM_2(int, type, itemtype *, expenses)
{
  int i;

  /* check input */
  if (expenses == NULL) return;

  /* go through the warrior powers */
  if (type & 1) {

    /* check for decreased soldier costs */
    if (MIL_MAGIC(MM_WARRIOR)) {
      i = 20;
      if (MIL_MAGIC(MM_CAPTAIN)) {
	i += 10;
      }
      if (MIL_MAGIC(MM_WARLORD)) {
	i += 10;
      }
      expenses[MTRLS_TALONS] *= (100 - i);
      expenses[MTRLS_TALONS] /= 100;
    }

    /* check for reduced monster support */
    if (MIL_MAGIC(MM_ORC)) {
      i = 20;
      if (MIL_MAGIC(MM_OGRE)) {
	i += 20;
      }
      if (MIL_MAGIC(MM_DRAGON)) {
	i += 20;
      }
      expenses[MTRLS_JEWELS] *= (100 - i); 
      expenses[MTRLS_JEWELS] /= 100;
    }
  }

  /* is it a naval purchase? */
  if (type & 2) {

    /* check for reduced fleet costs */
    if (CIV_MAGIC(MC_SAILOR)) {
      i = 20;
      if (CIV_MAGIC(MC_MARINE)) {
	i += 20;
      }
      expenses[MTRLS_TALONS] *= (100 - i);
      expenses[MTRLS_TALONS] /= 100;
    }

  }

  /* construction bonuses */
  if (type & 4) {

    /* check adjustments */
    if (CIV_MAGIC(MC_ARCHITECT)) {
      expenses[MTRLS_METALS] *= 80;
      expenses[MTRLS_METALS] /= 100;
      expenses[MTRLS_TALONS] *= 80;
      expenses[MTRLS_TALONS] /= 100;
      expenses[MTRLS_WOOD] *= 80;
      expenses[MTRLS_WOOD] /= 100;
    }

  }

  /* check for reduced jewel expenditures */
  if (CIV_MAGIC(MC_JEWELER)) {
    expenses[MTRLS_JEWELS] *= 80;
    expenses[MTRLS_JEWELS] /= 100;
  }

  /* check for reduced metal costs */
  if (CIV_MAGIC(MC_METALCRAFT)) {
    expenses[MTRLS_METALS] *= 80;
    expenses[MTRLS_METALS] /= 100;
  }

  /* check for better wood use */
  if (CIV_MAGIC(MC_WOODCRAFT)) {
    expenses[MTRLS_WOOD] *= 80;
    expenses[MTRLS_WOOD] /= 100;
  }
}

/*
 * r10_region - Calculate communication/supply range for a city (scaled by 10)
 *
 * Determines the effective communication range of a supply center based on
 * its designation type, national communication technology, construction status,
 * and available trade goods. The range affects resource distribution, unit
 * supply, and regional economic calculations. Returns value scaled by 10
 * for precision in fractional range calculations.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation structure (must not be NULL)
 *   c1_ptr - Pointer to city structure (must not be NULL)
 *   s1_ptr - Pointer to sector structure (must not be NULL)
 *
 * Returns:
 *   Communication range scaled by 10 (multiply by 10 for precise fractional ranges),
 *   0 if invalid parameters or non-supply center
 *
 * Side Effects:
 *   - None (pure calculation function)
 *
 * Notes:
 *   - Base range from national BUTE_COMMRANGE attribute
 *   - Stockade: 1/4 range, minimum 1.0 (returns 10)
 *   - Town: 1/2 range, minimum 2.0 (returns 20)
 *   - City: full range, minimum 3.0 (returns 30)
 *   - Capital: full range + 1, minimum 3.0 (returns 30+)
 *   - Under construction cities get downgraded one designation level
 *   - Sieged or devastated centers return 0 (no communication)
 *   - Trade goods can enhance range if conditions met
 *   - Cache and lower designations not considered supply centers
 */
int
r10_region PARM_3(NTN_PTR, n1_ptr, CITY_PTR, c1_ptr, SCT_PTR, s1_ptr)
{
  int dval = 0, hold = 0;

  /* check it */
  if ((n1_ptr == NULL) ||
      (c1_ptr == NULL) ||
      (s1_ptr == NULL) ||
      /* check for non-supply centers */
      ((dval = major_desg(s1_ptr->designation)) <= MAJ_CACHE) ||
      /* check for full sieges and devastation */
      minor_desg(s1_ptr->designation, MIN_DEVASTATED | MIN_SIEGED)) {
    return(hold);
  }

  /* sites under contruction mean downgraded ranges */
  if ((c1_ptr->i_people < 0) &&
      (dval > MAJ_STOCKADE)) {
    dval--;
  }

  /* find the national communication range */
  hold = n1_ptr->attribute[BUTE_COMMRANGE];
  switch (dval) {
  case MAJ_STOCKADE:
    /* stockade is only 1/4 range; minimum 1 */
    hold /= 4;
    if (hold < 10) hold = 10;
    break;
  case MAJ_TOWN:
    /* town is only 1/2 range; minimum 2 */
    hold /= 2;
    if (hold < 20) hold = 20;
    break;
  case MAJ_CITY:
    /* full range; minimum 3 */
    if (hold < 30) hold = 30;
    break;
  case MAJ_CAPITAL:
    /* full range + 1 sector; minimum 3 */
    hold += 10;
    if (hold < 30) hold = 30;
    break;
  default:
    /* don't know how this could happen */
    break;
  }

  /* check the tradegood type */
  if (tg_communicate(s1_ptr->tradegood) &&
      (tg_info[s1_ptr->tradegood].need_desg <= dval) &&
      (tg_info[s1_ptr->tradegood].pop_support <= s1_ptr->people)) {
    hold += tg_info[s1_ptr->tradegood].value;
  }
  return(hold);
}

/*
 * attract_val - Calculate population attraction value for a sector
 *
 * Computes how attractive a sector is for population growth and migration
 * based on terrain, designation, racial preferences, seasonal factors,
 * and supply network connectivity. This drives the population movement
 * and growth mechanics that shape territorial development over time.
 *
 * Parameters:
 *   xloc - X coordinate of sector to evaluate
 *   yloc - Y coordinate of sector to evaluate
 *
 * Returns:
 *   Attraction value (higher = more attractive), 0 if unattractive or invalid
 *
 * Side Effects:
 *   - Uses ntn_ptr if available, otherwise derives nation from sector owner
 *   - Temporarily sets sct_tptr for sector access
 *
 * Notes:
 *   - Combines vegetation, elevation, and designation attractiveness
 *   - Special handling for extreme environments (ice, desert, swamp, jungle)
 *   - Racial preferences affect base attractiveness values
 *   - Mining sectors get bonus based on resource value and trade good viability
 *   - Farming sectors vary by season and food scarcity
 *   - Urban magic provides city attraction bonus
 *   - Unsupplied sectors get major attraction penalty (÷8)
 *   - Sieged and for-sale sectors are completely unattractive (return 0)
 *   - Complex seasonal modifiers for agricultural sectors
 *   - Essential for population dynamics and territorial growth
 */
int
attract_val PARM_2(int, xloc, int, yloc)
{
  NTN_PTR n1_ptr;
  int sum = 0, cnt, cntry, ival, rval;

  /* check input */
  if (!XY_ONMAP(xloc, yloc)) {
    return(0);
  }
  if (ntn_ptr == NULL) {
    n1_ptr = world.np[cntry = sct[xloc][yloc].owner];
  } else {
    n1_ptr = ntn_ptr;
    cntry = country;
  }
  if (n1_ptr == NULL) return(0);
  rval = n1_ptr->race;

  /* can't move into a sieged or real estate sector */
  sct_tptr = &(sct[xloc][yloc]);
  if (minor_desg(sct_tptr->designation, MIN_FORSALE | MIN_SIEGED)) {
    return(0);
  }

  /* first find the vegetation attraction */
  switch (sct_tptr->vegetation) {
  case VEG_ICE:
  case VEG_DESERT:
    /* is there a dervish or destroyer nation? */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_DERVISH) ||
	MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DESTROYER)) {
      sum = 60;
    } else {
      return(0);
    }
    break;
  case VEG_SWAMP:
  case VEG_JUNGLE:
    /* any amphibian traits? */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_AMPHIBIAN)) {
      sum = 50;
    } else {
      return(0);
    }
    break;
  default:
    /* just normal vegetation settings */
    sum = (int) veg_info[sct_tptr->vegetation].attract[rval];
    break;
  }

  /* now adjust due to elevation */
  sum *= (int) ele_info[sct_tptr->altitude].attract[rval];
  sum /= 100;
  if (sum == 0) return(0);

  /* compute in designation modifier */
  ival = major_desg(sct_tptr->designation);
  sum *= (int) maj_dinfo[ival].attract[rval];
  sum /= 100;
  if (sum == 0) return(0);
  switch (ival) {
  case MAJ_METALMINE:
    if (!tg_ok(sct_tptr, cntry)) {
      sum /= 4;
    } else {
      sum += 3 * metal_value(sct_tptr);
    }
    break;
  case MAJ_JEWELMINE:
    if (!tg_ok(sct_tptr, cntry)) {
      sum /= 4;
    } else {
      sum += 3 * jewel_value(sct_tptr);
    }
    break;
  case MAJ_LUMBERYARD:
    sum /= 2;
    sum += (towood(sct_tptr, cntry) * 3) / 2;
    break;
  default:
    if (IS_CITY(ival)) {
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_URBAN)) {
	sum *= 102;
	sum /= 100;
      }
    } else if (IS_FARM(ival)) {
      if ((world.np[cntry] != NULL) &&
	  ((world.np[cntry])->mtrls[MTRLS_FOOD] / 2 <
	   (world.np[cntry])->tciv)) {
	sum += 5 * tofood(sct_tptr, cntry);
      } else {
	sum += 3 * tofood(sct_tptr, cntry);
      }
      switch (SEASON(TURN)) {
      case WINTER_SEASON:
	/* no like; but no like leave */
	sum /= 4;
	break;
      case SPRING_SEASON:
	/* okay, lets get back in slowly */
	sum /= 2;
	break;
      case SUMMER_SEASON:
	/* should be creapin' up now */
	if (ival != MAJ_FERTILE) {
	  sum *= 2;
	  sum /= 3;
	} else {
	  sum *= 3;
	  sum /= 4;
	}
	break;
      case FALL_SEASON:
	/* jump on in thar */
	if (ival != MAJ_FRUITFUL) {
	  sum *= 2;
	  sum /= 3;
	}
	break;
      }
    }
    break;
  }

  /* now check minor designation adjustments */
  for (cnt = 0; cnt < MIN_NUMBER; cnt++) {
    if (minor_desg(sct_tptr->designation, MIN_START << cnt)) {
      sum *= (int) min_dinfo[cnt].attract[rval];
      sum /= 100;
    }
  }

  /* is it supported? */
  if ((ntn_ptr != NULL) &&
      (SUM_WEIGHTS(xloc, yloc) <= 0)) {
    /* just ain't as pretty */
    sum /= 8;
  }

  /* all done... */
  return(sum);
}

/*
 * find_area - Calculate territorial boundary edges for a nation
 *
 * Determines the rectangular boundary that encompasses all sectors owned
 * by a nation, setting the edge coordinates used for efficient territorial
 * processing. Handles map wraparound and optimizes the boundary rectangle
 * to minimize area while including all owned territory.
 *
 * Parameters:
 *   ntnnum - Nation number to calculate boundaries for (must be valid)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Updates nation's edge fields: leftedge, rightedge, topedge, bottomedge
 *   - Sets edges to -1 if nation has no territory
 *   - Allocates temporary memory for horizontal edge detection (non-SIMPLE_FIND)
 *
 * Notes:
 *   - Searches entire map to find owned sectors
 *   - bottomedge: lowest Y coordinate with owned sectors
 *   - topedge: highest Y coordinate with owned sectors
 *   - leftedge/rightedge: optimized for minimal rectangular area
 *   - SIMPLE_FIND version: basic left/right edge detection
 *   - Advanced version: handles map wraparound and gap optimization
 *   - Chooses smaller of two possible rectangles when territory wraps map
 *   - Essential for efficient sector iteration in other functions
 *   - Used by ntn_totals and other nation-wide processing functions
 */
void
find_area PARM_1(int, ntnnum)
{
  register int i, j;
#ifndef SIMPLE_FIND
  int last_on, center_left, center_right;
  char *x_owned;
#endif

  /* verify the nation */
  if ((ntn_tptr = world.np[ntnnum]) == NULL) return;

#ifndef SIMPLE_FIND
  /* provide space for horizontal edge detection */
  if ((x_owned = (char *) malloc(sizeof(char) * MAPX)) == NULL) {
    fprintf(fupdate, "Memory allocation error in find_area()\n");
    abrt();
  }

  /* clear the array */
  for (i = 0; i < MAPX; i++) x_owned[i] = FALSE;
#endif /*SIMPLE_FIND*/

  /* first find the bottom bounds */
  ntn_tptr->bottomedge = (-1);
  for (j = 0; j < MAPY; j++) {
    for (i = 0; i < MAPX; i++) {

      /* check for the target */
      if (sct[i][j].owner == ntnnum) {
	ntn_tptr->bottomedge = j;
	break;
      }

    }

    /* check for end */
    if (ntn_tptr->bottomedge == j) break;
  }

  /* now find the lowest */
  ntn_tptr->topedge = (-1);
  for (j = MAPY - 1; j >= 0; j--) {
    for (i = 0; i < MAPX; i++) {

      /* check for the target */
      if (sct[i][j].owner == ntnnum) {
	ntn_tptr->topedge = j;
	break;
      }

    }

    /* check for end */
    if (ntn_tptr->topedge == j) break;
  }

  /* configure the horizontal locations */
  ntn_tptr->leftedge = MAPX;
  ntn_tptr->rightedge = 0;

  /* given those, search for left and right sides */
  for (j = ntn_tptr->bottomedge; j <= ntn_tptr->topedge; j++) {

    /* find the leftmost position */
    for (i = 0; i < ntn_tptr->leftedge; i++) {
      if (sct[i][j].owner == ntnnum) {
	ntn_tptr->leftedge = i;
#ifndef SIMPLE_FIND
	x_owned[i] = TRUE;
#endif
	break;
      }
    }

#ifndef SIMPLE_FIND
    /* mark all owned locations */
    for (; i < MAPX; i++) {
      if (sct[i][j].owner == ntnnum) {
	if (i > ntn_tptr->rightedge) ntn_tptr->rightedge = i;
	x_owned[i] = TRUE;
      }
    }
#else
    /* find the rightmost position */
    for (i = MAPX - 1; i > ntn_tptr->rightedge; i--) {
      if (sct[i][j].owner == ntnnum) {
	ntn_tptr->rightedge = i;
	break;
      }
    }
#endif /*SIMPLE_FIND*/
  }

#ifndef SIMPLE_FIND
  /* setting configurations */
  last_on = ntn_tptr->leftedge;
  center_left = center_right = (-1);

  /* first set the left edge */
  for (i = ntn_tptr->leftedge; i <= ntn_tptr->rightedge; i++) {

    /* check off and on status */
    if (last_on != i - 1) {
      if (x_owned[i] == TRUE) {
	if (center_right - center_left < i - last_on) {
	  center_right = i;
	  center_left = last_on;
	}
      }
    }

    /* mark that it is now within owned region */
    if (x_owned[i] == TRUE) {
      last_on = i;
    }

  }

  /* now compare the two large gaps */
  if (center_right - center_left >
      MAPX + ntn_tptr->leftedge - ntn_tptr->rightedge) {
    ntn_tptr->leftedge = center_right - MAPX;
    ntn_tptr->rightedge = center_left;
  }

  free(x_owned);
#endif /*SIMPLE_FIND*/
}

/*
 * infl_of - Calculate inflation adjustment for talon expenditures
 *
 * Computes the additional cost imposed by national inflation on talon-based
 * transactions. Inflation represents economic inefficiency and monetary
 * devaluation that increases the real cost of purchases and maintenance.
 * Used throughout the economic system for realistic cost modeling.
 *
 * Parameters:
 *   talon_value - Base talon amount to apply inflation to
 *
 * Returns:
 *   Additional inflation cost in talons, 0 if no nation context or zero value
 *
 * Side Effects:
 *   - None (pure calculation function)
 *
 * Notes:
 *   - Uses nation's BUTE_INFLATION attribute as inflation percentage
 *   - Applied to absolute value to handle both positive and negative amounts
 *   - Returns additional cost only, not total adjusted cost
 *   - Must be added to original value by caller
 *   - No inflation applied if ntn_ptr is NULL or talon_value is 0
 *   - Essential for economic realism and game balance
 *   - Affects all talon-based transactions throughout the game
 */
itemtype
infl_of PARM_1( itemtype, talon_value )
{
  /* check input */
  if ((ntn_ptr == NULL) || (talon_value == 0)) {
    return((itemtype) 0);
  }

  return((itemtype) ntn_ptr->attribute[BUTE_INFLATION] * abs(talon_value));
}

/*
 * find_newarmynum - Find next available army unit number for unit type
 *
 * Determines an appropriate unique unit identifier for a new army unit
 * based on unit type and existing unit numbers. Ensures no conflicts
 * with existing units while maintaining logical numbering patterns
 * that group similar unit types together.
 *
 * Parameters:
 *   utype - Unit type to determine base numbering range for
 *
 * Returns:
 *   Next available unit number, skipping EMPTY_HOLD reserved values
 *
 * Side Effects:
 *   - None (pure calculation function)
 *
 * Notes:
 *   - Uses unit_basenum to determine starting range for unit type
 *   - Increments past EMPTY_HOLD reserved numbers
 *   - Searches existing army list to find first unused number
 *   - Maintains ascending order assumption in army list
 *   - Breaks early when gap found in numbering sequence
 *   - Essential for unit creation and organization systems
 *   - Prevents duplicate unit numbers that would cause conflicts
 *   - Groups similar unit types in logical number ranges
 */
int
find_newarmynum PARM_1(int, utype)
{
  ARMY_PTR a1_ptr;
  int hold;

  /* find the starting slot */
  if ((hold = unit_basenum(utype)) == EMPTY_HOLD) {
    hold++;
  }

  /* find the lowest number greater than or equal to hold */
  for (a1_ptr = ntn_ptr->army_list;
       a1_ptr != NULL;
       a1_ptr = a1_ptr->next) {
    /* increment if a unit already has that number */
    if (hold == a1_ptr->armyid) {
      do {
	hold++;
      } while (hold == EMPTY_HOLD);
    }
    else if (hold < a1_ptr->armyid) break;
  }
  return(hold);
}

/*
 * army_reorganize - Renumber all army units with optimized unit IDs
 *
 * Systematically renumbers all army units in a nation to eliminate gaps
 * and optimize the numbering system. Maintains proper references between
 * units (especially leader relationships) while creating a clean, organized
 * unit numbering scheme for better management and display.
 *
 * Parameters:
 *   None (operates on current nation via ntn_ptr)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Renumbers all army units in the nation
 *   - Rebuilds army list with new numbering
 *   - Updates leader references to maintain command relationships
 *   - Calls army_sort to maintain list organization
 *
 * Notes:
 *   - Temporarily removes units from list during renumbering
 *   - Uses find_newarmynum for optimal number assignment
 *   - Updates ARMYT_LEAD references when leaders are renumbered
 *   - Maintains command structure integrity during reorganization
 *   - Rebuilds list in reverse order, then sorts properly
 *   - Essential for unit management cleanup and organization
 *   - Safe operation that preserves all unit relationships
 *   - Improves user interface and reduces numbering confusion
 */
void
army_reorganize PARM_0(void)
{
  ARMY_PTR army_next;
  int oldnumber;

  /* check the settings */
  if (ntn_ptr == NULL) return;

  /* go through the list of army units */
  army_ptr = ntn_ptr->army_list;
  ntn_ptr->army_list = NULL;
  for (/* DONE: army_ptr = ntn_ptr->army_list */;
       army_ptr != NULL;
       army_ptr = army_next) {
    /* ready for next unit */
    army_next = army_ptr->next;

    /* renumber and trace through the naval units */
    oldnumber = ARMY_ID;
    army_renum(find_newarmynum(ARMY_TYPE));

    /* make sure to renumber any additional references as well */
    if (a_isleader(ARMY_TYPE)) {
      for (army_tptr = army_next;
	   army_tptr != NULL;
	   army_tptr = army_tptr->next) {
	if (ARMYT_LEAD == oldnumber) {
	  ARMYT_LEAD = ARMY_ID;
	}
      }
    }

    /* next step */
    army_ptr->next = ntn_ptr->army_list;
    ntn_ptr->army_list = army_ptr;
    army_sort(TRUE);
  }
  army_sort(FALSE);
}

/*
 * adjust_attribute - Modify national attribute with boundary enforcement
 *
 * Safely adjusts a national attribute by a specified amount while enforcing
 * minimum and maximum bounds defined in the attribute system. Prevents
 * attribute values from exceeding design limits that could break game
 * balance or cause undefined behavior in dependent calculations.
 *
 * Parameters:
 *   bute_list - Array of attribute values to modify (must not be NULL)
 *   butenum - Index of specific attribute to adjust (must be valid BUTE_ constant)
 *   amount - Amount to add to current value (can be negative for reduction)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies the specified attribute value in-place
 *   - Clamps result to valid range defined by bute_info bounds
 *
 * Notes:
 *   - Validates butenum is within valid range [0, BUTE_NUMBER)
 *   - Uses bute_info[butenum].max_base and .min_base for bounds checking
 *   - Addition performed in temporary variable to avoid overflow issues
 *   - Essential for safe attribute modification throughout the game
 *   - Prevents exploits that could manipulate attributes beyond design limits
 *   - Used by various systems for attribute rewards, penalties, and adjustments
 *   - Maintains game balance by enforcing consistent attribute ranges
 */
void
adjust_attribute PARM_3(short *, bute_list, int, butenum, int, amount)
{
  int hold;

  /* check the input */
  if (bute_list == NULL) return;
  if ((butenum < 0) || (butenum >= BUTE_NUMBER)) return;

  /* now perform the adjustment */
  hold = bute_list[butenum] + amount;
  if (hold > bute_info[butenum].max_base) {
    bute_list[butenum] = bute_info[butenum].max_base;
  } else if (hold < bute_info[butenum].min_base) {
    bute_list[butenum] = bute_info[butenum].min_base;
  } else {
    bute_list[butenum] = hold;
  }
}

/*
 * merc_costrate - Calculate mercenary cost multiplier percentage
 *
 * Determines the cost multiplier for mercenary hiring based on the nation's
 * mercenary reputation attribute. Nations with better reputations pay less
 * for mercenary services, while those with poor reputations face premium
 * pricing. This creates strategic incentives for maintaining good relations
 * with mercenary organizations.
 *
 * Parameters:
 *   None (uses current nation via ntn_ptr)
 *
 * Returns:
 *   Cost percentage (100 = normal rate, <100 = discount, >100 = premium),
 *   100 if no nation context available
 *
 * Side Effects:
 *   - None (pure calculation function)
 *
 * Notes:
 *   - Formula: 150 - BUTE_MERCREP = cost percentage
 *   - BUTE_MERCREP range typically 0-50, giving costs 150% to 100%
 *   - Higher reputation (higher BUTE_MERCREP) = lower costs
 *   - Default 100% cost when ntn_ptr is NULL
 *   - Used by mercenary hiring and maintenance cost calculations
 *   - Provides economic incentive for diplomatic/reputation management
 *   - Essential for balanced mercenary economics in the game
 */
int
merc_costrate PARM_0(void)
{
  /* depends on nation pointer */
  if (ntn_ptr == NULL) return(100);
  return (150 - (int)ntn_ptr->attribute[BUTE_MERCREP]);
}
