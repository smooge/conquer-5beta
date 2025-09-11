/* file containing code specific to sector calculations for conqrun */
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
#include "dataA.h"
#include "armyX.h"
#include "cityX.h"
#include "magicX.h"
#include "racesX.h"
#include "desigX.h"
#include "activeX.h"
#include "statusX.h"
#include "dstatusX.h"

/* Military Sector Summary Information */
typedef struct s_milsum {
  ntntype owner;		/* who owns the units */
  long size;			/* combined strength of units */
  struct s_milsum *next;
} MILSUM_STRUCT, *MILSUM_PTR;

typedef struct s_sctsum {
  maptype xloc, yloc;		/* the sector being summarized */
  ntntype owner;		/* the owner of the sector */
  MILSUM_STRUCT hold_troops;	/* troops preventing sector capture/siege */
  MILSUM_STRUCT siege_troops;	/* troops putting the sector under siege */
  MILSUM_PTR take_troops;	/* troops trying to capture the sector */
  MILSUM_PTR other_troops;	/* any other troops */
  struct s_sctsum *next;
} SCTSUM_STRUCT, *SCTSUM_PTR;

/*
 * crt_milsum - Allocate and initialize a military summary structure
 *
 * Creates a new MILSUM_STRUCT to track military unit information for a specific
 * nation during sector ownership calculations. This structure is used to aggregate
 * military strength data for capture and siege resolution algorithms.
 *
 * Parameters:
 *   who - Nation ID that owns the military units being summarized
 *
 * Returns:
 *   Pointer to newly allocated and initialized MILSUM_STRUCT on success
 *   Calls abrt() on memory allocation failure (does not return)
 *
 * Side Effects:
 *   - Allocates memory that must be freed by caller using free()
 *   - Terminates program on allocation failure via abrt()
 *   - Writes error message to fupdate on allocation failure
 *
 * Notes:
 *   - All MILSUM_STRUCT fields are initialized to safe defaults
 *   - Used internally by sector ownership and siege calculation system
 *   - Part of the military summary data aggregation framework
 */
static MILSUM_PTR
crt_milsum PARM_1(int, who)
{
  MILSUM_PTR MS_ptr;

  /* get the memory */
  if ((MS_ptr = (MILSUM_PTR) malloc(sizeof(MILSUM_STRUCT))) == NULL) {
    fprintf(fupdate, "Memory allocation error\n");
    abrt();
  }

  /* now initialize values */
  MS_ptr->owner = who;
  MS_ptr->size = 0;
  MS_ptr->next = NULL;
  return(MS_ptr);
}

/*
 * crt_sctsum - Allocate and initialize a sector summary structure
 *
 * Creates a new SCTSUM_STRUCT to track all military activity within a specific
 * sector during ownership change calculations. This structure aggregates holding
 * troops, siege troops, capturing troops, and other military units for comprehensive
 * sector control analysis during turn processing.
 *
 * Parameters:
 *   x - X coordinate of the sector being summarized (0-based map coordinates)
 *   y - Y coordinate of the sector being summarized (0-based map coordinates)
 *
 * Returns:
 *   Pointer to newly allocated and initialized SCTSUM_STRUCT on success
 *   Calls abrt() on memory allocation failure (does not return)
 *
 * Side Effects:
 *   - Allocates memory that must be freed by caller using sctsum_free()
 *   - Terminates program on allocation failure via abrt()
 *   - Writes error message to fupdate on allocation failure
 *
 * Notes:
 *   - All military troop lists are initialized to NULL/UNOWNED/0
 *   - Embedded MILSUM_STRUCT for hold_troops and siege_troops are initialized
 *   - Used as foundation for sector ownership and siege calculations
 *   - Part of the two-pass sector control algorithm in upd_capture()
 */
static SCTSUM_PTR
crt_sctsum PARM_2(int, x, int, y)
{
  SCTSUM_PTR SS_ptr;

  /* get the memory */
  if ((SS_ptr = (SCTSUM_PTR) malloc(sizeof(SCTSUM_STRUCT))) == NULL) {
    fprintf(fupdate, "Memory allocation error\n");
    abrt();
  }

  /* now initialize values */
  SS_ptr->xloc = x;
  SS_ptr->yloc = y;
  SS_ptr->owner = UNOWNED;
  SS_ptr->siege_troops.owner = UNOWNED;
  SS_ptr->siege_troops.size = 0L;
  SS_ptr->siege_troops.next = NULL;
  SS_ptr->hold_troops.owner = UNOWNED;
  SS_ptr->hold_troops.size = 0L;
  SS_ptr->hold_troops.next = NULL;
  SS_ptr->take_troops = NULL;
  SS_ptr->other_troops = NULL;
  SS_ptr->next = NULL;
  return(SS_ptr);
}

/*
 * sctsum_byloc - Retrieve a sector summary by its location
 *
 * Searches through a linked list of sector summaries to find the one matching
 * the specified coordinates. Used during military aggregation to locate existing
 * sector summary structures before creating new ones.
 *
 * Parameters:
 *   SS_ptr - Head of linked list of SCTSUM_STRUCT to search through
 *   x - X coordinate to match (0-based map coordinates)
 *   y - Y coordinate to match (0-based map coordinates)
 *
 * Returns:
 *   Pointer to SCTSUM_STRUCT matching the coordinates if found
 *   NULL if no matching sector summary is found in the list
 *
 * Side Effects:
 *   - None (read-only search operation)
 *
 * Notes:
 *   - Uses C parameter passing behavior for iteration
 *   - Linear search through linked list (O(n) complexity)
 *   - Part of sector summary management system
 *   - Coordinates must match exactly for successful lookup
 */
static SCTSUM_PTR
sctsum_byloc PARM_3(SCTSUM_PTR, SS_ptr, int, x, int, y)
{
  /* take advantage of C parameter passing */
  for (; SS_ptr != NULL; SS_ptr = SS_ptr->next) {
    if ((SS_ptr->xloc == x) && (SS_ptr->yloc == y)) break;
  }
  return(SS_ptr);
}

/*
 * milsum_byowner - Retrieve the military summary by owner nation
 *
 * Searches through a linked list of military summaries to find the one belonging
 * to the specified nation. Used during military aggregation to locate existing
 * military summary structures for a nation before creating new ones.
 *
 * Parameters:
 *   MS_ptr - Head of linked list of MILSUM_STRUCT to search through
 *   ntnnum - Nation ID to search for (1-based nation identifier)
 *
 * Returns:
 *   Pointer to MILSUM_STRUCT owned by the specified nation if found
 *   NULL if no matching military summary is found in the list
 *
 * Side Effects:
 *   - None (read-only search operation)
 *
 * Notes:
 *   - Uses C parameter passing behavior for iteration
 *   - Linear search through linked list (O(n) complexity)
 *   - Part of military summary management system
 *   - Nation ID must match exactly for successful lookup
 *   - Typo in original comment: "sumary" should be "summary"
 */
static MILSUM_PTR
milsum_byowner PARM_2(MILSUM_PTR, MS_ptr, int, ntnnum)
{
  /* take advantage of C parameter passing */
  for (; MS_ptr != NULL; MS_ptr = MS_ptr->next) {
    if (MS_ptr->owner == ntnnum) break;
  }
  return(MS_ptr);
}

/*
 * sort_milsum - Sort a military summary linked list by troop size (descending)
 *
 * Implements an insertion sort algorithm to organize military summaries in
 * descending order by military strength. The largest military force appears
 * first in the list, which is crucial for determining primary attackers in
 * sector capture calculations.
 *
 * Parameters:
 *   MS_list - Head of linked list of MILSUM_STRUCT to sort
 *
 * Returns:
 *   New head of the sorted linked list (largest military force first)
 *   Original head if list is NULL or contains only one element
 *
 * Side Effects:
 *   - Modifies the linked list structure by reordering nodes
 *   - Preserves all MILSUM_STRUCT data, only changes link pointers
 *
 * Notes:
 *   - Uses insertion sort algorithm (O(n²) worst case, but acceptable for small lists)
 *   - Handles empty lists and single-element lists gracefully
 *   - Critical for sector capture logic where largest force determines outcome
 *   - Sorting is stable (equal values maintain relative order)
 *   - Used primarily by upd_capture() for take_troops prioritization
 */
static MILSUM_PTR
sort_milsum PARM_1(MILSUM_PTR, MS_list)
{
  MILSUM_PTR first_ptr, travel_ptr, hold_ptr;

  /* check input */
  if ((MS_list == NULL) || (MS_list->next == NULL)) {
    return(MS_list);
  }

  /* now go through the entire list */
  hold_ptr = first_ptr = MS_list;
  MS_list = MS_list->next;
  first_ptr->next = NULL;
  for (travel_ptr = MS_list; MS_list != NULL; travel_ptr = MS_list) {

    /* compare with the beginning of the list */
    if (travel_ptr->size > first_ptr->size) {

      /* add it to the beginning of the list */
      MS_list = MS_list->next;
      travel_ptr->next = first_ptr;
      first_ptr = travel_ptr;

    } else {

      /* find where to put it */
      for (hold_ptr = first_ptr;
	   hold_ptr->next != NULL;
	   hold_ptr = hold_ptr->next) {
	if (travel_ptr->size > hold_ptr->next->size) break;
      }

      /* add it here */
      MS_list = MS_list->next;
      travel_ptr->next = hold_ptr->next;
      hold_ptr->next = travel_ptr;
    }
  }

  /* now give back the new head of the list */
  return(first_ptr);
}

/*
 * sctsum_free - Free all memory associated with a sector summary structure
 *
 * Performs comprehensive cleanup of a SCTSUM_STRUCT including all linked lists
 * of military summaries (take_troops, siege_troops.next, other_troops). This
 * function ensures no memory leaks occur during sector ownership processing.
 *
 * Parameters:
 *   SS_ptr - Pointer to SCTSUM_STRUCT to free (must not be NULL)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Frees all MILSUM_STRUCT nodes in take_troops linked list
 *   - Frees all MILSUM_STRUCT nodes in siege_troops.next linked list
 *   - Frees all MILSUM_STRUCT nodes in other_troops linked list
 *   - Frees the main SCTSUM_STRUCT itself
 *   - Sets next pointer to NULL before freeing main structure
 *
 * Notes:
 *   - hold_troops and siege_troops are embedded structures, not separate allocations
 *   - Caller responsibility to ensure SS_ptr is valid before calling
 *   - Used in conjunction with upd_capture() cleanup phase
 *   - Critical for preventing memory leaks in turn processing
 *   - Does not handle NULL SS_ptr (undefined behavior if passed)
 */
static void
sctsum_free PARM_1(SCTSUM_PTR, SS_ptr)
{
  MILSUM_PTR MS1_ptr, MS2_ptr;

  /* first free up all of the capturing troops */
  for (MS1_ptr = SS_ptr->take_troops;
       MS1_ptr != NULL; MS1_ptr = MS2_ptr) {

    /* first keep track of next storage */
    MS2_ptr = MS1_ptr->next;
    MS1_ptr->next = NULL;

    /* now nuke it */
    free(MS1_ptr);
  }

  /* free up the list of siegers */
  for (MS1_ptr = SS_ptr->siege_troops.next;
       MS1_ptr != NULL; MS1_ptr = MS2_ptr) {

    /* first keep track of next storage */
    MS2_ptr = MS1_ptr->next;
    MS1_ptr->next = NULL;

    /* now nuke it */
    free(MS1_ptr);
  }

  /* now free up all of the remaining troops */
  for (MS1_ptr = SS_ptr->other_troops;
       MS1_ptr != NULL; MS1_ptr = MS2_ptr) {

    /* first keep track of next storage */
    MS2_ptr = MS1_ptr->next;
    MS1_ptr->next = NULL;

    /* now nuke it */
    free(MS1_ptr);
  }

  /* finally clear out the main storage */
  SS_ptr->next = NULL;
  free(SS_ptr);
}

/*
 * men_to_capture - Calculate number of troops required to capture a sector
 *
 * Computes the minimum military strength needed to successfully capture a sector
 * based on population, designation type, racial factors, diplomatic relations,
 * and fortifications. This is a critical function for the sector capture algorithm,
 * ensuring realistic capture requirements that vary by sector characteristics.
 *
 * Parameters:
 *   xloc - X coordinate of target sector (0-based map coordinates)
 *   yloc - Y coordinate of target sector (0-based map coordinates)
 *   by - Nation ID attempting the capture (1-based nation identifier)
 *
 * Returns:
 *   Minimum number of troops required to capture the sector
 *   TAKESECTOR constant if input validation fails or sector is invalid
 *
 * Side Effects:
 *   - Modifies global sct_ptr to point to target sector
 *   - None (read-only calculation)
 *
 * Notes:
 *   - Base capture difficulty varies by designation (Capital=3x, City=2x, Town=1.5x)
 *   - Canal sectors have 2x modifier due to government employee resistance
 *   - Fortified sectors add 20% to capture difficulty
 *   - Racial compatibility affects capture resistance (race_info matrix)
 *   - Jihad diplomatic status adds 10% resistance penalty
 *   - Population size directly impacts total troops required
 *   - Uses BASE_TAKEPCT as baseline percentage for calculations
 */
long
men_to_capture PARM_3(int, xloc, int, yloc, ntntype, by)
{
  long hold = TAKESECTOR;
  int pval = BASE_TAKEPCT;
  NTN_PTR n1_ptr, n2_ptr;

  /* check the input */
  if (!XY_ONMAP(xloc, yloc) || ((n2_ptr = world.np[by]) == NULL))
    return(hold);

  /* now find any neccesary adjustments */
  sct_ptr = &(sct[xloc][yloc]);
  if ((sct_ptr->owner == UNOWNED) ||
      (sct_ptr->people == 0) ||
      ((n1_ptr = world.np[sct_ptr->owner]) == NULL)) {
    return(hold);
  }

  /* determine base value for designation */
  switch (major_desg(sct_ptr->designation)) {
  case MAJ_CAPITAL:
    /* the people live for the govt.; politicians */
    pval *= 3;
    break;
  case MAJ_CITY:
    /* tougher people live in the city */
    pval *= 2;
    break;
  case MAJ_TOWN:
  case MAJ_STOCKADE:
    /* 1 & 1/2 times normal capture cost */
    pval *= 3;
    pval /= 2;
    break;
  case MAJ_CANAL:
    /* doubled value since the canal crews are mostly govt. employees */
    pval *= 2;
    /* fallthrough */
  default:
    /* check for slight increase from fortifications */
    if (minor_desg(sct_ptr->designation, MIN_FORTIFIED)) {
      /* 20% increase */
      pval *= 5;
      pval /= 4;
    }
    break;
  }

  /* now find racial and diplomatic adjustments */
  pval *= race_info[n2_ptr->race].take_value[n1_ptr->race];
  pval /= 100;
  if (n1_ptr->dstatus[by] == DIP_JIHAD) {
    pval *= 110;
    pval /= 100;
  }

  /* now add in the population adjustment */
  hold += (sct_ptr->people * pval) / 100;
  return(hold);
}

/*
 * capture_land - Transfer sector ownership and handle population effects
 *
 * Executes the complete sector capture process including ownership transfer,
 * population casualties, fleeing civilians, city transfers, devastation effects,
 * and alignment-based modifiers. This is the core function that implements the
 * consequences of successful military conquest in the game.
 *
 * Parameters:
 *   whom - Nation ID gaining control of the sector (UNOWNED to abandon)
 *   xloc - X coordinate of sector being captured (0-based map coordinates)
 *   yloc - Y coordinate of sector being captured (0-based map coordinates)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Changes sector ownership to specified nation
 *   - Modifies population based on racial compatibility and alignment
 *   - Causes population fleeing to nearby friendly sectors
 *   - Transfers or destroys cities/capitals with associated bonuses
 *   - May cause sector devastation if casualties are severe (>50%)
 *   - Generates news reports and player messages
 *   - Temporarily modifies global country and ntn_ptr variables
 *
 * Notes:
 *   - If whom=UNOWNED, sector becomes abandoned and population zeroed
 *   - Kill/keep percentages determined by race_info compatibility matrix
 *   - Alignment differences affect population survival rates (±5% per alignment step)
 *   - Slaver magic reduces kill rate by 20% and increases keep rate by 50%
 *   - Cities are transferred with inventory, renamed if name conflicts exist
 *   - Capital capture grants +2 attack bonus, other cities grant +1 attack/defense
 *   - Devastation affects farms/fertile sectors and adds MIN_DEVASTATED flag
 *   - Preserves original global state (country, ntn_ptr) after execution
 */
void
capture_land PARM_3(ntntype, whom, int, xloc, int, yloc)
{
  NTN_PTR n1_ptr, n2_ptr, nhold_ptr = ntn_ptr;
  long num_kill, num_people;
  int i, pval, align_adj, nhold_cntry = country;

  /* check input */
  if (!XY_ONMAP(xloc, yloc)) return;
  if (whom >= ABSMAXNTN) return;

  /* get quickie information */
  sct_ptr = &(sct[xloc][yloc]);
  ntn_ptr = n2_ptr = world.np[sct_ptr->owner];
  country = sct_ptr->owner;

  /* check unowned */
  if ((whom == UNOWNED) ||
      ((n1_ptr = world.np[whom]) == NULL)) {

    /* nullify everything */
    sct_ptr->owner = UNOWNED;
    sct_ptr->people = 0;
    sct_ptr->designation = MAJ_NONE;

    /* eliminate any cities encountered */
    if (IS_CITY(sct_ptr->designation) &&
	((city_ptr = citybyloc(ntn_ptr, xloc, yloc)) != NULL)) {
      dest_city(CITY_NAME);
    }

  } else {

    /* give to somebody */
    if ((n2_ptr == NULL) ||
	((sct_ptr->people == 0) &&
	 !IS_CITY(sct_ptr->designation))) {

      /* just adjust the ownership */
      sct_ptr->owner = whom;

    } else {

      /* hold how many are in the sector */
      num_people = sct_ptr->people;

      /* find adjustment due to alignment */
      align_adj = 5 * (abs(n_alignment(n1_ptr->active) -
			   n_alignment(n2_ptr->active)) - 1);

      /* factor in the deaths */
      pval = race_info[n1_ptr->race].kill_pop[n2_ptr->race];
      pval -= align_adj;
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_SLAVER)) {
	pval *= 4;
	pval /= 5;
      }
      num_people *= pval;
      num_people /= 100;

      /* now factor in the amount kept in the land */
      num_kill = num_people;
      num_people = sct_ptr->people;
      pval = race_info[n1_ptr->race].keep_pop[n2_ptr->race];
      pval += align_adj;
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_SLAVER)) {
	pval *= 3;
	pval /= 2;
	if (pval > 100) {
	  pval = 100;
	}
      }
      num_people *= pval;
      num_people /= 100;

      /* cause the fleeing of the remainder */
      country = whom;
      flee_people(sct_ptr->people - num_people - num_kill, xloc, yloc);

      /* figure the final totals */
      sct_ptr->people -= num_kill;
      num_people = sct_ptr->people;

      /* check for any damage */
      if (num_kill > num_people / 2) {
	switch (major_desg(sct_ptr->designation)) {
	case MAJ_FARM:
	case MAJ_FERTILE:
	case MAJ_FRUITFUL:
	  /* knocks out the farm sector and causes devastation */
	  set_majordesg(sct_ptr->designation, MAJ_NONE);
	  /* fallthrough */
	default:
	  /* devastation */
	  set_minordesg(sct_ptr->designation, MIN_DEVASTATED);
	  break;
	}
      }

      /* now change the owner */
      sct_ptr->owner = whom;
      if (IS_CITY(sct_ptr->designation)) {

	/* transfer the city */
	if ((city_ptr = citybyloc(ntn_ptr, xloc, yloc)) == NULL) {
	  set_majordesg(sct_ptr->designation, MAJ_NONE);
	} else {

	  /* copy it */
	  city_tptr = new_city();
	  strcpy(CITYT_NAME, CITY_NAME);
	  CITYT_XLOC = CITY_XLOC;
	  CITYT_YLOC = CITY_YLOC;
	  CITYT_PEOPLE = CITY_PEOPLE;
	  CITYT_FORT = CITY_FORT;
	  CITYT_WEIGHT = CITY_WEIGHT;
	  CITYT_STALONS = CITY_STALONS;
	  for (i = 0; i < MTRLS_NUMBER; i++) {
	    if (CITY_MTRLS[i] > (itemtype) 0) {
	      CITYT_MTRLS[i] = CITY_MTRLS[i];
	    } else {
	      CITYT_MTRLS[i] = (itemtype) 0;
	    }
	    if (CITY_IMTRLS[i] > (itemtype) 0) {
	      CITYT_IMTRLS[i] = CITY_IMTRLS[i];
	    } else {
	      CITYT_IMTRLS[i] = (itemtype) 0;
	    }
	  }

	  /* get rid of original */
	  sprintf(string, "3.\tNation %s sacks %s %s in Nation %s\n",
		  n1_ptr->name,
		  maj_dinfo[major_desg(sct_ptr->designation)].name,
		  CITY_NAME,
		  n2_ptr->name);
	  sorted_news(whom, string);
	  dest_city(CITY_NAME);

	  /* check for a capital */
	  if (major_desg(sct_ptr->designation) == MAJ_CAPITAL) {
	    n1_ptr->aplus += 2;
	    set_majordesg(sct_ptr->designation, MAJ_CITY);
	  }
	  if (major_desg(sct_ptr->designation) > MAJ_STOCKADE) {
	    n1_ptr->aplus ++;
	    n1_ptr->dplus ++;
	  }
	  if (major_desg(sct_ptr->designation) != MAJ_CACHE) {
	    n1_ptr->aplus ++;
	  }

	  /* now add it to the other nation */
	  ntn_ptr = n1_ptr;
	  while (citybyname(CITYT_NAME) != NULL) {
	    /* adjust names */
	    do {
	      city_tptr->name[0] = 'A' + rand_val('Z' - 'A');
	    } while (!isprint(city_tptr->name[0]));
	  }

	  /* transfer it */
	  city_tptr->next = ntn_ptr->city_list;
	  ntn_ptr->city_list = city_tptr;
	  city_sort();

	}
      }
    }
  }

  /* reset values */
  ntn_ptr = nhold_ptr;
  country = nhold_cntry;
}

/* data on sector accessibility */
static char reach_init = FALSE;
static int reach_xloc, reach_yloc, reach_limit, reach_target;
static int reach_val[11][11];
static char reach_visit[11][11];

/*
 * align_xdiff - Adjust X coordinate difference for world map wrapping
 *
 * Handles the cylindrical world map topology where the map wraps around at the
 * X boundaries. Calculates the shortest distance between two X coordinates,
 * accounting for the possibility that going "the other way around" might be shorter.
 *
 * Parameters:
 *   xd_val - Raw X coordinate difference (target_x - source_x)
 *
 * Returns:
 *   Adjusted X difference taking map wrapping into account
 *   Positive values indicate eastward direction, negative indicate westward
 *
 * Side Effects:
 *   - None (pure calculation function)
 *
 * Notes:
 *   - World map wraps only on X-axis (cylindrical), Y-axis has fixed boundaries
 *   - Compares abs(xd_val) with MAPX - abs(xd_val) to find shorter path
 *   - If wraparound is shorter, adjusts sign to indicate proper direction
 *   - Critical for accurate distance calculations in reach and movement systems
 *   - Used by pathfinding functions to determine actual movement distances
 *   - MAPX constant defines the width of the world map
 */
static int
align_xdiff PARM_1(int, xd_val)
{
  /* check for edge wrapping */
  if (abs(xd_val) > MAPX - abs(xd_val)) {
    if (xd_val <= 0) {
      xd_val = MAPX - abs(xd_val);
    } else {
      xd_val = abs(xd_val) - MAPX;
    }
  }
  return(xd_val);
}

/*
 * next_checkp - Recursive pathfinding function for reach calculation
 *
 * Implements a flood-fill algorithm to determine which sectors are reachable
 * from a starting point within a specified range. Recursively explores adjacent
 * sectors while respecting movement restrictions and diplomatic relations.
 * This function is the core of the reachability system used for movement validation.
 *
 * Parameters:
 *   xloc - X coordinate of current sector being checked
 *   yloc - Y coordinate of current sector being checked
 *
 * Returns:
 *   None (void function, modifies global reach arrays)
 *
 * Side Effects:
 *   - Marks sectors as visited in reach_visit array
 *   - Sets reachability values in reach_val array based on ownership
 *   - Recursively calls itself for adjacent sectors via map_loop()
 *   - Modifies global reach calculation state
 *
 * Notes:
 *   - Stops recursion if sector is outside reach_limit range
 *   - Respects food/movement restrictions via tofood() function
 *   - Handles diplomatic restrictions (MV_OTHNATION) for non-allied nations
 *   - Uses reach_target for special target marking (sets to MAXNTN when found)
 *   - Works within 11x11 local coordinate system centered on origin
 *   - Critical component of movement validation and AI pathfinding
 *   - Called initially by set_reach(), then recursively explores neighbors
 */
static void
next_checkp PARM_2(int, xloc, int, yloc)
{
  int xspot, yspot, xdiff, sown;

  /* check the range */
  if (!map_within(reach_xloc, reach_yloc, xloc, yloc, reach_limit)) {
    return;
  }

  /* set indicators */
  xdiff = align_xdiff(xloc - reach_xloc);
  xspot = xdiff + reach_limit;
  yspot = yloc - reach_yloc + reach_limit;

  /* have we been here before? */
  if (reach_visit[xspot][yspot]) return;

  /* must we stop with this visit? */
  reach_visit[xspot][yspot] = 1;
  if (tofood(&(sct[xloc][yloc]), country) == 0) return;

  /* set the information */
  sown = sct[xloc][yloc].owner;
  if ((reach_target == -1) &&
      (ntn_ptr != NULL)) {
    /* check for "MV_OTHNATION" restriction */
    int dstat = ntn_ptr->dstatus[sown];
    if ((sown != country) &&
	(sown != UNOWNED) &&
	(dstat != DIP_ALLIED)) return;
    reach_val[xspot][yspot] = sown;
  } else {
    reach_val[xspot][yspot] = sown;
    if (reach_val[xspot][yspot] == reach_target)
      reach_val[xspot][yspot] = MAXNTN;
  }

  /* now play with recursion */
  map_loop(xloc, yloc, 1, next_checkp);
}

/*
 * set_reach - Initialize reachability calculation for a specific location
 *
 * Sets up the global reach calculation system centered on the specified coordinates.
 * Initializes the 11x11 reach arrays and starts the recursive pathfinding process
 * to determine which sectors are accessible from the starting point within a 2-sector
 * radius. This is the entry point for movement validation and AI pathfinding.
 *
 * Parameters:
 *   xloc - X coordinate of center point for reach calculation
 *   yloc - Y coordinate of center point for reach calculation
 *   value - Target value for special marking (-1 for general reachability)
 *
 * Returns:
 *   None (void function, initializes global reach state)
 *
 * Side Effects:
 *   - Sets reach_init to TRUE to indicate valid reach data
 *   - Initializes reach_xloc, reach_yloc as calculation center
 *   - Sets reach_limit to 2 (standard movement range)
 *   - Sets reach_target for special target identification
 *   - Clears all reach_val and reach_visit arrays to default state
 *   - Triggers recursive pathfinding via next_checkp()
 *
 * Notes:
 *   - Creates 11x11 local coordinate system (5 sectors in each direction)
 *   - reach_limit of 2 allows checking sectors up to 2 moves away
 *   - All arrays initialized to -1 (unreachable) and 0 (unvisited)
 *   - Must be called before using get_reach() or change_reach()
 *   - Used by movement validation, AI pathfinding, and relocation systems
 */
void
set_reach PARM_3(int, xloc, int, yloc, int, value)
{
  register int i, j;

  /* it has a proper setting */
  reach_init = TRUE;
  reach_xloc = xloc;
  reach_yloc = yloc;
  reach_limit = 2;
  reach_target = value;

  /* initialize entire settings */
  for (i = 0; i < 11; i++) {
    for (j = 0; j < 11; j++) {
      reach_val[i][j] = -1;
      reach_visit[i][j] = 0;
    }
  }

  /* start it */
  next_checkp(xloc, yloc);
}

/*
 * get_reach - Query reachability status for a specific sector
 *
 * Retrieves the reachability value for a sector from the previously calculated
 * reach arrays. This function is used after set_reach() to determine if a sector
 * is accessible and what nation owns it (or special status values).
 *
 * Parameters:
 *   xloc - X coordinate of sector to query
 *   yloc - Y coordinate of sector to query
 *
 * Returns:
 *   Nation ID of sector owner if reachable (0-MAXNTN)
 *   MAXNTN if sector matches reach_target value
 *   -1 if sector is unreachable or calculation not initialized
 *
 * Side Effects:
 *   - None (read-only query function)
 *
 * Notes:
 *   - Requires set_reach() to be called first (checks reach_init flag)
 *   - Validates Y coordinate is on map and within reach_limit
 *   - Uses align_xdiff() to handle map wrapping for X coordinates
 *   - Converts world coordinates to local 11x11 array coordinates
 *   - Returns ownership information for movement and diplomatic validation
 *   - Critical for movement validation and AI decision making
 *   - Typo in original comment: "Is is" should be "Is it"
 */
int
get_reach PARM_2(int, xloc, int, yloc)
{
  int xdiff;

  /* invalid input */
  if ((reach_init == FALSE) ||
      !Y_ONMAP(yloc) ||
      !map_within(reach_xloc, reach_yloc, xloc, yloc, reach_limit)) {
    return(-1);
  }

  /* give the result */
  xdiff = align_xdiff(xloc - reach_xloc);
  return(reach_val[xdiff + reach_limit]
	 [yloc - reach_yloc + reach_limit]);
}

/*
 * change_reach - Modify reachability value for a specific sector
 *
 * Updates the reach value for a sector that was previously calculated by set_reach().
 * This function allows dynamic modification of reachability data, typically used
 * when sector ownership changes during movement or when adjusting for special
 * conditions like relocation calculations.
 *
 * Parameters:
 *   xloc - X coordinate of sector to modify
 *   yloc - Y coordinate of sector to modify
 *   value - New reachability value to set for the sector
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies reach_val array at the specified coordinates
 *   - Writes warning messages to fupdate if validation fails
 *
 * Notes:
 *   - Requires reach system to be initialized (reach_init must be TRUE)
 *   - Validates sector is within reach_limit range before modification
 *   - Warns if attempting to modify unset/uninitialized values (-1)
 *   - Uses align_xdiff() to handle map wrapping for X coordinates
 *   - Converts world coordinates to local 11x11 array coordinates
 *   - Used for dynamic updates during movement and relocation calculations
 *   - Critical for maintaining accurate reachability during state changes
 */
void
change_reach PARM_3(int, xloc, int, yloc, int, value)
{
  int xind, yind, xdiff;

  /* invalid input */
  if (reach_init == FALSE) {
    fprintf(fupdate, "Warning: change_reach()... changing unitialized value\n");
    return;
  }
  if (!map_within(reach_xloc, reach_yloc, xloc, yloc, reach_limit)) {
    fprintf(fupdate, "Warning: change_reach()... sector out of range\n");
    return;
  }

  /* now find the position */
  xdiff = align_xdiff(xloc - reach_xloc);
  xind = xdiff + reach_limit;
  yind = yloc - reach_yloc + reach_limit;
  if (reach_val[xind][yind] == -1) {
    fprintf(fupdate, "Warning: change_reach()... adjusting unset value\n");
  }
  reach_val[xind][yind] = value;
}

/*
 * adj_reloc - Adjust reach settings for population relocation calculations
 *
 * Helper function for set_relocation() that modifies reach values to reflect
 * sector attractiveness for population relocation. Converts basic reachability
 * data into attractiveness values used by the population relocation system.
 *
 * Parameters:
 *   x - X coordinate of sector to evaluate for relocation
 *   y - Y coordinate of sector to evaluate for relocation
 *
 * Returns:
 *   None (void function, modifies reach arrays)
 *
 * Side Effects:
 *   - Calls get_reach() to query current reachability value
 *   - Calls change_reach() to update with new attractiveness value
 *   - May call attract_val() to calculate sector attractiveness
 *
 * Notes:
 *   - Only processes sectors that are reachable (get_reach() != -1)
 *   - For friendly sectors (same country), sets value to attract_val()
 *   - For non-friendly sectors, sets value to -1 (not suitable for relocation)
 *   - Uses global country variable for ownership comparison
 *   - Part of the population relocation and movement system
 *   - Called by map_loop() in set_relocation() for each nearby sector
 */
static void
adj_reloc PARM_2(int, x, int, y)
{
  int rval;

  if ((rval = get_reach(x, y)) != -1) {
    if (rval == country) {
      change_reach(x, y, attract_val(x, y));
    } else {
      change_reach(x, y, -1);
    }
  }
}

/*
 * set_relocation - Initialize reach system for population relocation
 *
 * Sets up the reachability system specifically for population relocation
 * calculations. Initializes basic reachability from the specified center point,
 * then adjusts all reachable sectors to reflect their attractiveness for
 * population movement and resettlement.
 *
 * Parameters:
 *   xloc - X coordinate of center point for relocation calculation
 *   yloc - Y coordinate of center point for relocation calculation
 *
 * Returns:
 *   None (void function, initializes reach system for relocation)
 *
 * Side Effects:
 *   - Calls set_reach() to initialize basic reachability (-1 target)
 *   - Calls map_loop() with adj_reloc() to adjust all nearby sectors
 *   - Modifies global reach arrays with attractiveness values
 *
 * Notes:
 *   - Uses -1 as reach_target to establish general reachability
 *   - Processes all sectors within 2-sector radius via map_loop()
 *   - Converts reachability data into attractiveness data for relocation
 *   - Used by population movement and relocation systems
 *   - Sectors owned by same country get attract_val(), others get -1
 *   - Critical for determining where populations can relocate to
 */
void
set_relocation PARM_2(int, xloc, int, yloc)
{
  /* initialize what is reachable */
  set_reach(xloc, yloc, -1);

  /* reassign values */
  map_loop(xloc, yloc, 2, adj_reloc);
}

/*
 * flee_find - Count friendly sectors available for fleeing populations
 *
 * Helper function for flee_people() that counts potential destinations for
 * fleeing civilians. Categorizes reachable sectors as either allied/owned
 * (preferred) or neutral/friendly (secondary option) for population distribution.
 *
 * Parameters:
 *   x - X coordinate of sector to evaluate as fleeing destination
 *   y - Y coordinate of sector to evaluate as fleeing destination
 *
 * Returns:
 *   None (void function, modifies global counters)
 *
 * Side Effects:
 *   - Increments global_int for allied/owned sectors
 *   - Increments global_long for neutral/friendly sectors
 *   - Uses get_reach() to query sector reachability and ownership
 *
 * Notes:
 *   - Only processes sectors that are reachable (get_reach() returns valid owner)
 *   - Preferred destinations: same country or allied nations (DIP_ALLIED)
 *   - Secondary destinations: neutral/friendly nations (<=DIP_NEUTRAL, !=DIP_UNMET)
 *   - Uses global country variable for diplomatic status checking
 *   - Part of population fleeing system used during sector capture
 *   - Called by map_loop() in flee_people() to survey all nearby sectors
 */
static void
flee_find PARM_2(int, x, int, y)
{
  NTN_PTR n1_ptr;
  int who;

  /* check input */
  who = get_reach(x, y);
  if ((who < 0) ||
      (who > ABSMAXNTN)) return;

  /* find allies and ourselves */
  if ((who == country) ||
      (((n1_ptr = world.np[who]) != NULL) &&
       (n1_ptr->dstatus[country] == DIP_ALLIED))) {
    global_int++;
  } else if ((n1_ptr != NULL) &&
	     (n1_ptr->dstatus[country] <= DIP_NEUTRAL) &&
	     (n1_ptr->dstatus[country] != DIP_UNMET)) {
    global_long++;
  }
}

/*
 * flee_run - Execute population distribution to fleeing destinations
 *
 * Helper function for flee_people() that actually places fleeing civilians
 * into friendly sectors. Uses the counts from flee_find() to determine
 * which sectors should receive population and distributes people accordingly.
 *
 * Parameters:
 *   x - X coordinate of potential destination sector
 *   y - Y coordinate of potential destination sector
 *
 * Returns:
 *   None (void function, modifies sector population)
 *
 * Side Effects:
 *   - Increases population of suitable destination sectors
 *   - Uses global_long as population amount to distribute per sector
 *   - Uses global_int as distribution mode selector
 *
 * Notes:
 *   - Mode 0 (global_int==0): Distribute to allied/owned sectors only
 *   - Mode 1 (global_int==1): Distribute to neutral/friendly sectors only
 *   - Only adds population if sector matches the current distribution mode
 *   - Distribution amount (global_long) calculated by flee_people()
 *   - Part of population fleeing system used during sector capture
 *   - Called by map_loop() in flee_people() after flee_find() survey
 *   - Provides realistic population dispersal during wartime/conquest
 */
static void
flee_run PARM_2(int, x, int, y)
{
  NTN_PTR n1_ptr;
  int who;

  /* get the value */
  who = get_reach(x, y);
  if ((who < 0) || (who >= ABSMAXNTN)) {
    n1_ptr = NULL;
  } else {
    n1_ptr = world.np[who];
  }

  /* people... get your people, here! */
  if ((global_int == 0) &&
      ((who == country) ||
       ((n1_ptr != NULL) &&
	(n1_ptr->dstatus[country] == DIP_ALLIED)))) {
    /* distribute to self and allies */
    sct[x][y].people += global_long;
  } else if ((global_int == 1) && 
	     (n1_ptr != NULL) &&
	     (n1_ptr->dstatus[country] <= DIP_NEUTRAL) &&
	     (n1_ptr->dstatus[country] != DIP_UNMET)) {
    /* distribute to "just" friends */
    sct[x][y].people += global_long;
  }

}

/*
 * flee_people - Cause population to flee from a sector to nearby friendly areas
 *
 * Implements the population fleeing mechanism used during sector capture, combat,
 * or other traumatic events. Calculates reachable friendly destinations and
 * distributes fleeing civilians among them. If no safe destinations exist,
 * some population may be lost to casualties.
 *
 * Parameters:
 *   amount - Number of people attempting to flee (cannot exceed sector population)
 *   xloc - X coordinate of source sector (where people are fleeing from)
 *   yloc - Y coordinate of source sector (where people are fleeing from)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Reduces population of source sector by amount (or portion thereof)
 *   - Increases population of reachable friendly sectors
 *   - May cause casualties if no friendly destinations available
 *   - Temporarily modifies global country variable
 *   - Uses global_int and global_long for calculation state
 *
 * Notes:
 *   - Prioritizes allied/owned sectors, falls back to neutral/friendly if needed
 *   - Uses reach system with -2 target for special fleeing calculations
 *   - Distribution modes: 0=allies only, 1=neutrals only, 2=no destinations (casualties)
 *   - In mode 2 (no safe destinations), 25% of fleeing population dies
 *   - Population is distributed evenly among available destination sectors
 *   - Critical for realistic population dynamics during warfare and conquest
 *   - Preserves original global state after execution
 */
void
flee_people PARM_3(long, amount, int, xloc, int, yloc)
{
  int found, friends, ontnnum, mode = 0;

  /* check input */
  if (!XY_ONMAP(xloc, yloc))
    return;
  if (amount > sct[xloc][yloc].people) {
    amount = sct[xloc][yloc].people;
  }
  if (amount <= 0) return;

  /* check ownership and sizing */
  if (((global_int = sct[xloc][yloc].owner) == UNOWNED) ||
      (world.np[global_int] == NULL)) {
    /* no need to go on */
    return;
  }
  ontnnum = country;
  country = global_int;

  /* find possible sectors within range */
  set_reach(xloc, yloc, -2);
  if (get_reach(xloc, yloc) != -1) {
    change_reach(xloc, yloc, -1);
  }

  /* distribute the people */
  global_int = 0;
  global_long = 0;
  map_loop(xloc, yloc, 2, flee_find);

  /* check if only friendly land is within range */
  found = global_int;
  friends = global_long;
  if (found == 0) {
    if (friends > 0) {
      found = friends;
      mode = 1;
    } else {
      mode = 2;
    }
  }

  /* set the distribution */
  if (mode != 2) {

    /* how many per sector? */
    global_long = amount / found;
    global_int = mode;

    /* go to it */
    if (global_long > 0) {
      map_loop(xloc, yloc, 2, flee_run);
    }

    /* they have flown the coop */
    sct[xloc][yloc].people -= amount;

  } else {

    /* kill a quarter of those trying to flee */
    sct[xloc][yloc].people -= amount / 4;

  }
  country = ontnnum;
}

/*
 * upd_capture - Compute sector ownership changes and siege establishment
 *
 * Implements the comprehensive two-pass algorithm for resolving all sector
 * ownership changes and siege establishment during turn processing. This is
 * the master function that coordinates military strength analysis, diplomatic
 * considerations, and capture/siege resolution across the entire game world.
 *
 * Algorithm Overview:
 * Pass 1: Traverse all army units, building sector summaries with military data
 * Pass 2: Process each sector summary to determine ownership changes and sieges
 *
 * Parameters:
 *   None (operates on global game state)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Creates and manages linked lists of SCTSUM_STRUCT for all contested sectors
 *   - Calls capture_land() for successful sector captures
 *   - Sets siege status flags for cities under successful siege
 *   - Generates comprehensive news reports and player messages
 *   - Writes detailed status information to fupdate log
 *   - Allocates and frees substantial temporary memory structures
 *
 * Notes:
 *   - Military summaries categorize troops: hold_troops, siege_troops, take_troops, other_troops
 *   - Capture requires attackers to exceed TAKE_RATIO * defenders in strength
 *   - Successful capture also requires meeting men_to_capture() population threshold
 *   - Siege requires 2:1 ratio of siege troops to holding troops (cities only)
 *   - Diplomatic relations affect which troops aid/hinder capture attempts
 *   - Allied/treaty nations can assist captures, belligerent+ nations defend
 *   - Complex news generation system provides detailed battle reports
 *   - Memory management includes comprehensive cleanup via sctsum_free()
 *   - Used during turn processing as core warfare resolution mechanism
 */
void
upd_capture PARM_0(void)
{
  /* This routine is much more rugged and time consuming than
     prior methods.  It involves two passes:
        1. traverse all army units in all nations, gathering
           the data into a summary on a per sector basis.
        2. go through all summarized sectors, determining
           whether a siege or ownership change takes place.
   */
  SCTSUM_PTR SS_list = NULL, SS_ptr = NULL, SS_nptr;
  MILSUM_PTR MS_ptr = NULL;
  ntntype ntnnum;
  int xspot, yspot, dstat, ustat, owner_change;
  long total_troops, thold_troops;

  /* initialize the sector adjustment message */
  msg_ginit("Sector Ownership Report");
  fprintf(fupdate, "=== Checking Troops for Captures and Sieges\n");

  /* go through all nations, accumulating the data */
  for (country = 1; country < MAXNTN; country++) {

    /* check it out */
    if ((ntn_ptr = world.np[country]) == NULL) continue;
    if (n_notactive(ntn_ptr->active)) continue;

    /* go through all armies in the nation */
    for (army_ptr = ntn_ptr->army_list;
	 army_ptr != NULL;
	 army_ptr = army_ptr->next) {

      /* check the location */
      if (!XY_INMAP(ARMY_XLOC, ARMY_YLOC)) continue;
      if (ARMY_SIZE <= 0) continue;

      /* find the size */
      if ((total_troops = army_captsize(army_ptr, TRUE)) > 0) {

	/* obtain the sector summary */
	if ((SS_ptr == NULL) ||
	    (SS_ptr->xloc != ARMY_XLOC) ||
	    (SS_ptr->yloc != ARMY_YLOC)) {

	  if ((SS_ptr =
	       sctsum_byloc(SS_list, ARMY_XLOC, ARMY_YLOC)) == NULL) {
	    /* need a new one */
	    SS_ptr = crt_sctsum(ARMY_XLOC, ARMY_YLOC);
	    SS_ptr->owner = sct[ARMY_XLOC][ARMY_YLOC].owner;
	    if (world.np[SS_ptr->owner] == NULL) {
	      /* check for errors */
	      SS_ptr->owner = UNOWNED;
	    }

	    /* now add to the list */
	    if (SS_list != NULL) {
	      SS_ptr->next = SS_list;
	    }
	    SS_list = SS_ptr;
	  }

	}

	/* now obtain the proper military summary; if needed */
	dstat = ntn_ptr->dstatus[SS_ptr->owner];
	ustat = real_stat(army_ptr);
	if (fort_stat(ustat)) {

	  /* just add into the protectors */
	  SS_ptr->hold_troops.size += total_troops;

	} else if (capture_stat(ustat) &&
		   ((SS_ptr->owner == UNOWNED) ||
		    (dstat >= DIP_WAR))) {

	  /* find the military summary */
	  if ((MS_ptr =
	       milsum_byowner(SS_ptr->take_troops, country)) == NULL) {
	    /* need a new one */
	    MS_ptr = crt_milsum(country);
	    if (SS_ptr->take_troops != NULL) {
	      MS_ptr->next = SS_ptr->take_troops;
	    }
	    SS_ptr->take_troops = MS_ptr;
	  }

	  /* now add it in */
	  MS_ptr->size += total_troops;

	  /* check for sieges */
	  if (unit_status(ARMY_STAT) == ST_SIEGE) {
	    SS_ptr->siege_troops.size += total_troops;
	    if (SS_ptr->siege_troops.owner == UNOWNED) {
	      SS_ptr->siege_troops.owner = country;
	    } else if ((SS_ptr->siege_troops.owner != country) &&
		       (milsum_byowner(SS_ptr->siege_troops.next,
				       country)) == NULL) {
	      MS_ptr = crt_milsum(country);
	      if (SS_ptr->siege_troops.next != NULL) {
		MS_ptr->next = SS_ptr->siege_troops.next;
	      }
	      SS_ptr->siege_troops.next = MS_ptr;
	    }
	  }

	} else {

	  /* find the military summary for others */
	  if ((MS_ptr =
	       milsum_byowner(SS_ptr->other_troops, country)) == NULL) {
	    /* need a new one */
	    MS_ptr = crt_milsum(country);
	    if (SS_ptr->other_troops != NULL) {
	      MS_ptr->next = SS_ptr->other_troops;
	    }
	    SS_ptr->other_troops = MS_ptr;
	  }

	  /* now add it in */
	  MS_ptr->size += total_troops;

	}

      }

    }

  }

  /* now go through all of the sector summaries */
  for (SS_ptr = SS_list;
       SS_ptr != NULL;
       SS_ptr = SS_nptr) {

    /* get sector information */
    if (!XY_INMAP(SS_ptr->xloc, SS_ptr->yloc))
      goto free_it_up;

    /* keep quick reference for sector */
    sct_ptr = &(sct[xspot = SS_ptr->xloc][yspot = SS_ptr->yloc]);
    country = SS_ptr->owner;
    ntn_ptr = world.np[country];

    /* must sort the list of takers */
    SS_ptr->take_troops = sort_milsum(SS_ptr->take_troops);

    /* first determine ownership adjustment */
    owner_change = FALSE;
    if ((SS_ptr->take_troops != NULL) &&
	(SS_ptr->hold_troops.size == 0)) {

      /* there is a possible sector takeover */
      MS_ptr = SS_ptr->take_troops;
      total_troops = MS_ptr->size;
      ntnnum = MS_ptr->owner;
      thold_troops = SS_ptr->hold_troops.size;

      /* check for additional support on both sides */
      for (MS_ptr = MS_ptr->next;
	   MS_ptr != NULL;
	   MS_ptr = MS_ptr->next) {
	if ((ntn_tptr = world.np[MS_ptr->owner])
	    ->dstatus[ntnnum] >= DIP_BELLICOSE) {
	  /* prevents sector capturing */
	  thold_troops += MS_ptr->size;
	} else if ((ntn_tptr->dstatus[ntnnum] <= DIP_NEUTRAL) &&
		   (ntn_tptr->dstatus[ntnnum] != DIP_UNMET)) {
	  /* aids in the capture */
	  total_troops += MS_ptr->size;
	}
      }

      /* now examine remaining troops for help or hurt */
      for (MS_ptr = SS_ptr->other_troops;
	   MS_ptr != NULL;
	   MS_ptr = MS_ptr->next) {
	if ((ntn_tptr = world.np[MS_ptr->owner])
	    ->dstatus[ntnnum] >= DIP_BELLICOSE) {
	  /* prevents sector capturing */
	  thold_troops += MS_ptr->size;
	} else if (((ntn_tptr->dstatus[ntnnum] == DIP_ALLIED) ||
		    (ntn_tptr->dstatus[ntnnum] == DIP_TREATY)) &&
		   (ntn_tptr->dstatus[country] >= DIP_NEUTRAL)) {
	  /* aids in the capture */
	  total_troops += MS_ptr->size;
	}
      }

      /* compare sizing */
      if (total_troops > TAKE_RATIO * thold_troops) {

	/* overpowered enemy troops */
	int desg_val = major_desg(sct[xspot][yspot].designation);

	MS_ptr = SS_ptr->take_troops;
	ntn_tptr = world.np[MS_ptr->owner];
	if (total_troops - thold_troops >=
	    men_to_capture(xspot, yspot, MS_ptr->owner)) {

	  /* Sector Is Taken */
	  if ((country == UNOWNED) || (ntn_ptr == NULL)) {
	    sprintf(string, "3.\tNation %s claims unowned land\n",
		    ntn_tptr->name);
	    sorted_news(MS_ptr->owner, string);
	    sprintf(string, "unowned land claimed as own");
	  } else {
	    sprintf(string, "3.\tNation %s captures land from %s\n",
		    ntn_tptr->name, ntn_ptr->name);
	    sorted_news(MS_ptr->owner, string);
	    sprintf(string, "your %s is taken by nation %s",
		    (desg_val == MAJ_NONE) ? "land" :
		    maj_dinfo[desg_val].name,
		    ntn_tptr->name);
	    msg_grouped(SS_ptr->owner, xspot, yspot, string);
	    sprintf(string, "%s captured from nation %s",
		    (desg_val == MAJ_NONE) ? "land" :
		    maj_dinfo[desg_val].name,
		    ntn_ptr->name);
	  }

	  /* inform the victor */
	  msg_grouped(MS_ptr->owner, xspot, yspot, string);
	  capture_land(MS_ptr->owner, xspot, yspot);

	} else if (MS_ptr->size >= TAKESECTOR) {

	  /* Populace too much */
	  sprintf(string, "your %s resists capture by nation %s",
		  (desg_val == MAJ_NONE) ? "sector" :
		  maj_dinfo[desg_val].name,
		  ntn_tptr->name);
	  msg_grouped(SS_ptr->owner, xspot, yspot, string);
	  sprintf(string, "%s population resists capture",
		  ((desg_val == MAJ_NONE) ||
		   (desg_val == MAJ_CACHE)) ? "sector" :
		  maj_dinfo[desg_val].name);
	  msg_grouped(MS_ptr->owner, xspot, yspot, string);

	}

      }

    }

    /* only a city sector may be placed under siege */
    if (!IS_CITY(sct_ptr->designation) ||
	(ntn_ptr == NULL)) goto no_siege_here;

    /* now check for sieges */
    if ((owner_change == FALSE) &&
	(SS_ptr->owner != UNOWNED) &&
	(SS_ptr->siege_troops.size >
	 (2 * SS_ptr->hold_troops.size))) {

      /* there is a possible siege */
      total_troops = 2 * SS_ptr->hold_troops.size;
      ntnnum = SS_ptr->siege_troops.owner;

      /* check if any of the remaining troops oppose the siege */
      for (MS_ptr = SS_ptr->other_troops;
	   MS_ptr != NULL;
	   MS_ptr = MS_ptr->next) {
	if (((ntn_tptr = world.np[MS_ptr->owner]) != NULL) &&
	    (ntn_tptr->dstatus[ntnnum] >= DIP_WAR)) {
	  total_troops += MS_ptr->size;
	}
      }

      /* now check if the siege takes place */
      if (SS_ptr->siege_troops.size >
	  total_troops) {

	/* A siege takes place */
	SCT_STATUS(SS_ptr->xloc, SS_ptr->yloc) |= SET_SIEGE;

	/* notify sector owner */
	sprintf(string, "your %s is under siege.",
		maj_dinfo[major_desg(sct_ptr->designation)].name);
	msg_grouped(SS_ptr->owner, xspot ,yspot, string);

	/* notify the news */
	fprintf(fnews, "2.\t%s in nation %s is under siege.\n",
		maj_dinfo[major_desg(sct_ptr->designation)].name,
		world.np[SS_ptr->owner]->name);

	/* notify siegers */
	sprintf(string, "siege against nation %s successful",
		world.np[SS_ptr->owner]->name);
	msg_grouped(SS_ptr->siege_troops.owner, xspot, yspot, string);
	for (MS_ptr = SS_ptr->siege_troops.next;
	     MS_ptr != NULL;
	     MS_ptr = MS_ptr->next) {
	  msg_grouped(MS_ptr->owner, xspot, yspot, string);
	}
	goto free_it_up;

      }

    }

    /* inform siegers that no siege takes place */
  no_siege_here:
    if (SS_ptr->siege_troops.size > 0) {
      sprintf(string, "your siege was broken");
      msg_grouped(SS_ptr->siege_troops.owner, xspot, yspot, string);
      for (MS_ptr = SS_ptr->siege_troops.next;
	   MS_ptr != NULL;
	   MS_ptr = MS_ptr->next) {
	msg_grouped(MS_ptr->owner, xspot, yspot, string);
      }
    }

    /* now free up the memory, keeping track of location */
  free_it_up:
    SS_nptr = SS_ptr->next;
    sctsum_free(SS_ptr);
  }

  /* close up the group messages and news */
  send_sortednews();
  msg_gfinish();
}

