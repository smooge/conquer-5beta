/* routines to perform the action of monster units and nations */
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
#include "navyX.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "elevegX.h"
#include "statusX.h"

/*
 * monster_growth - Expand military forces and treasures for monster nations
 *
 * Implements population growth and resource accumulation for monster/NPC nations.
 * Increases army sizes, naval fleet strength, and material treasures based on
 * configured growth rates. Growth applies probabilistically using random values
 * to prevent predictable expansion patterns.
 *
 * Parameters:
 *   None (operates on current nation via global ntn_ptr)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Increases army sizes for monster and non-leader units
 *   - Adds mercenary forces based on mercenary growth rate
 *   - Expands naval fleets by adding warships
 *   - Increases material treasures in cities
 *   - Uses random number generation for probabilistic growth
 *
 * Notes:
 *   - Early return if no current nation (ntn_ptr == NULL)
 *   - Growth rates controlled by PMERCMONST, PMONSTER, and GROWTH constants
 *   - Monster armies only grow if size > 1 to prevent solo monster proliferation
 *   - Naval growth applies to all warship types (N_HEAVY and below)
 *   - Material growth is half the rate of military growth (divide by 2400 vs 1200)
 *   - Thread safety: not thread-safe, relies on global state
 */
void
monster_growth PARM_0(void)
{
  int i, j, rate_inc, rate_merc;

  /* set quickie references */
  if (ntn_ptr == NULL) return;
  rate_merc = PMERCMONST;
  if (rate_merc < 0) rate_merc = 0;
  rate_inc = PMONSTER - rate_merc;
  rate_merc = (GROWTH * rate_merc) / 10;
  rate_inc = (GROWTH * rate_inc) / 10;
  if ((rate_merc == 0) && (rate_inc == 0)) return;

  /* go through all of the armies */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {
    /* figure the type */
    if (a_ismonster(ARMY_TYPE)) {
      /* monsters will only increase in size if there is more than one */
      if ((ARMY_SIZE > 1) &&
	  (ARMY_SIZE * rate_inc > rand_val(1200))) {
	ARMY_SIZE++;
      }
    } else if (!a_isleader(ARMY_TYPE)) {
      MERCMEN += (ARMY_SIZE * rate_merc) / 1200;
      ARMY_SIZE += (ARMY_SIZE * rate_inc) / 1200;
    }
  }

  /* adjust all of the navies */
  for (navy_ptr = ntn_ptr->navy_list;
       navy_ptr != NULL;
       navy_ptr = navy_ptr->next) {
    for (i = 0; i <= N_HEAVY; i++) {
      j = (int) N_CNTSHIPS(NAVY_SHIPS[NSHP_WARSHIPS], i);
      if (j * rate_inc > rand_val(1200)) {
	NAVY_SHIPS[NSHP_WARSHIPS] =
	  navy_addships(NAVY_SHIPS[NSHP_WARSHIPS], i, 1);
      }
    }
  }

  /* now adjust the raw materials slightly */
  for (city_ptr = ntn_ptr->city_list;
       city_ptr != NULL;
       city_ptr = city_ptr->next) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (CITY_MTRLS[i] > 0)  {
	/* slower increase for prizes */
	CITY_MTRLS[i] += (CITY_MTRLS[i] * rate_inc) / 2400;
      }
    }
  }
}

/* variables to keep track of sector types for relocation */
static int veg_type, ele_type;

/*
 * ev_test - Test if elevation and vegetation match relocation criteria
 *
 * Evaluates whether a given map coordinate is suitable for monster army
 * relocation based on elevation, vegetation, and ownership requirements.
 * Uses global variables veg_type and ele_type to specify search criteria.
 *
 * Parameters:
 *   x - X coordinate to test (must be valid map coordinate)
 *   y - Y coordinate to test (must be valid map coordinate)
 *
 * Returns:
 *   TRUE if sector is suitable for relocation, FALSE otherwise
 *
 * Side Effects:
 *   - Reads sector data from global sct array
 *   - Accesses global variables: veg_type, ele_type, country
 *
 * Notes:
 *   - Rejects water sectors (ELE_WATER) and mountain peaks (ELE_PEAK)
 *   - Rejects wall-designated sectors (MAJ_WALL)
 *   - Prefers owned sectors (sct[x][y].owner == country)
 *   - For unowned sectors, matches elevation if ele_type != -1
 *   - For unowned sectors, matches vegetation if veg_type != -1
 *   - Value -1 for ele_type or veg_type means "any type acceptable"
 *   - Used by monster movement system for intelligent relocation
 */
static int
ev_test PARM_2(int, x, int, y)
{
  int ele;
  if (((ele = sct[x][y].altitude) != ELE_WATER) &&
      (ele != ELE_PEAK) &&
      (major_desg(sct[x][y].designation) != MAJ_WALL)) {
    if ((sct[x][y].owner == country) ||
	(((ele_type == -1) ||
	  (ele_type != ele)) &&
	 ((veg_type == -1) ||
	  (veg_type == sct[x][y].vegetation)))) {
      return(TRUE);
    }
  }
  return (FALSE);
}

/*
 * mn_cnt_type - Count valid sectors for monster army relocation
 *
 * Callback function used with map_loop to count the number of sectors
 * that meet relocation criteria. Increments global_int for each valid
 * sector found during the search radius scan.
 *
 * Parameters:
 *   x - X coordinate to evaluate (from map_loop iteration)
 *   y - Y coordinate to evaluate (from map_loop iteration)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Increments global_int counter for each valid sector
 *   - Calls ev_test() to determine sector suitability
 *
 * Notes:
 *   - Designed as callback for map_loop() function
 *   - Works in conjunction with mn_move_it() for two-phase relocation
 *   - Phase 1: Count valid options (this function)
 *   - Phase 2: Select random valid option (mn_move_it)
 *   - Uses global variables set by monster_move_army()
 */
static void
mn_cnt_type PARM_2(int, x, int, y)
{
  /* is it a match? */
  if (ev_test(x, y)) {
    global_int++;
  }
}

/*
 * mn_move_it - Relocate army to selected valid sector
 *
 * Callback function used with map_loop to perform the actual relocation
 * of monster army to a randomly selected valid sector. Works as the second
 * phase of the two-phase relocation system, selecting the target location
 * from the counted options.
 *
 * Parameters:
 *   x - X coordinate to evaluate (from map_loop iteration)
 *   y - Y coordinate to evaluate (from map_loop iteration)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Modifies ARMY_XLOC and ARMY_YLOC when target sector is reached
 *   - Decrements global_int as countdown to selected sector
 *   - Updates current army position via global army_ptr
 *
 * Notes:
 *   - Designed as callback for map_loop() function
 *   - global_int serves as countdown to target sector (randomly selected)
 *   - When global_int reaches 0, that sector becomes the new army location
 *   - Only processes sectors that pass ev_test() validation
 *   - Expects global_int to be pre-set to target sector number (1-based)
 */
static void
mn_move_it PARM_2(int, x, int, y)
{
  if ((global_int > 0) &&
      (ev_test(x, y))) {
    if (--global_int == 0) {
      ARMY_XLOC = x;
      ARMY_YLOC = y;
    }
  }
}

/*
 * monster_move_army - Relocate current army unit within one sector radius
 *
 * Implements intelligent monster army relocation by searching for suitable
 * sectors within a one-sector radius and randomly selecting from valid options.
 * Uses a two-phase approach: first count valid sectors, then randomly select
 * one for relocation.
 *
 * Parameters:
 *   vtype - Preferred vegetation type (-1 for any vegetation type)
 *   etype - Preferred elevation type (-1 for any elevation type)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Sets global variables veg_type and ele_type for search criteria
 *   - Modifies army position (ARMY_XLOC, ARMY_YLOC) if valid sector found
 *   - Uses global_int as working variable for counting and selection
 *   - Calls map_loop twice: once to count, once to select
 *
 * Notes:
 *   - Search radius is fixed at 1 sector from current army position
 *   - Only moves if at least one valid sector is found (global_int > 0)
 *   - Random selection prevents predictable movement patterns
 *   - If multiple options exist, uses rand_val() for random selection
 *   - Respects terrain preferences: vegetation type and elevation type
 *   - Value -1 for vtype or etype means "accept any type"
 */
void
monster_move_army PARM_2(int, vtype, int, etype)
{
  /* set the search mechanisms */
  veg_type = vtype;
  ele_type = etype;
  global_int = 0;
  map_loop(ARMY_XLOC, ARMY_YLOC, 1, mn_cnt_type);
  if (global_int > 0) {
    if (global_int > 1) global_int = rand_val(global_int) + 1;
    map_loop(ARMY_XLOC, ARMY_YLOC, 1, mn_move_it);
  }
}

/*
 * upd_lizards - Update lizard monster behavior and actions
 *
 * Implements the behavior pattern for lizard monster armies, including
 * growth, movement, and tactical decisions. Lizards prefer swamps and
 * light vegetation, with different behavior patterns based on army size
 * and current sector ownership.
 *
 * Parameters:
 *   None (operates on current nation via global ntn_ptr)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Calls monster_growth() for population/resource expansion
 *   - Sets army status (garrison, defend, attack, sweep)
 *   - Relocates armies using monster_move_army()
 *   - Accesses and modifies global army_ptr chain
 *
 * Notes:
 *   - Small armies (< 200) and archers prefer defensive positions
 *   - Garrison status assigned if in owned fort; otherwise move to light vegetation
 *   - Large armies (>= 200) prefer swamp terrain for roaming
 *   - Tactical decisions: garrison > defend > attack/sweep
 *   - Random elements prevent predictable behavior patterns
 *   - Uses fort_val() to detect fortification presence
 *   - Respects map boundaries with XY_INMAP() validation
 */
void
upd_lizards PARM_0(void)
{
  /* start with the basics */
  monster_growth();

  /* go through all of the lizard troops */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {
    /* find the current sector */
    if (!XY_INMAP(ARMY_XLOC, ARMY_YLOC)) continue;
    sct_ptr = &sct[ARMY_XLOC][ARMY_YLOC];

    /* make sure that garrisons are secured */
    if (a_isarcher(ARMY_TYPE) ||
	(ARMY_SIZE < 200)) {
      if (sct_ptr->owner == country) {
	if (fort_val(ARMY_XLOC, ARMY_YLOC)) {
	  set_status(ARMY_STAT, ST_GARRISON);
	} else {
	  monster_move_army(VEG_LT_VEG, -1);
	  set_status(ARMY_STAT, ST_DEFEND);
	}
      } else if (rand_val(2) == 0) {
	set_status(ARMY_STAT, ST_ATTACK);
      } else {
	monster_move_army(VEG_LT_VEG, -1);
	set_status(ARMY_STAT, ST_DEFEND);
      }
    } else {
      if (sct_ptr->owner == country) {
	/* go roaming */
	set_status(ARMY_STAT, ST_DEFEND);
	monster_move_army(VEG_SWAMP, -1);
      } else if (sct_ptr->owner == UNOWNED) {
	set_status(ARMY_STAT, ST_ATTACK);
      } else {
	if (rand_val(3)) {
	  /* go roaming */
	  set_status(ARMY_STAT, ST_DEFEND);
	  monster_move_army(VEG_SWAMP, -1);
	} else {
	  set_status(ARMY_STAT, ST_SWEEP);
	}
      }
    }
  }
}

/*
 * upd_savages - Update savage monster behavior and actions
 *
 * Implements the behavior pattern for savage monster armies. Savages are
 * aggressive and nomadic, constantly moving and attacking without regard
 * for terrain preferences. They represent the most unpredictable and
 * hostile monster type.
 *
 * Parameters:
 *   None (operates on current nation via global ntn_ptr)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Calls monster_growth() for population/resource expansion
 *   - Sets all armies to attack status (ST_ATTACK)
 *   - Relocates armies randomly (no terrain preference)
 *   - Accesses and modifies global army_ptr chain
 *
 * Notes:
 *   - Simplest monster behavior: always aggressive (ST_ATTACK)
 *   - 2/3 probability of movement each turn (rand_val(3))
 *   - No terrain preferences: vtype=-1, etype=-1 (any terrain acceptable)
 *   - No tactical considerations: always attack, never defend or garrison
 *   - Respects map boundaries with XY_INMAP() validation
 *   - Most unpredictable due to random movement and constant aggression
 */
void
upd_savages PARM_0(void)
{
  /* start with the basics */
  monster_growth();

  /* go through all of the savages */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {
    /* find the current sector */
    if (!XY_INMAP(ARMY_XLOC, ARMY_YLOC)) continue;
    sct_ptr = &sct[ARMY_XLOC][ARMY_YLOC];

    /* go roaming */
    set_status(ARMY_STAT, ST_ATTACK);
    if (rand_val(3)) {
      monster_move_army(-1, -1);
    }
  }
}

/*
 * upd_nomads - Update nomadic tribe behavior and actions
 *
 * Implements the behavior pattern for nomadic monster armies. Nomads
 * prefer mountainous terrain and use sweep tactics to systematically
 * clear areas. They represent a more organized and terrain-specific
 * monster type compared to savages.
 *
 * Parameters:
 *   None (operates on current nation via global ntn_ptr)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Calls monster_growth() for population/resource expansion
 *   - Sets all armies to sweep status (ST_SWEEP)
 *   - Relocates armies to mountain terrain (ELE_MOUNTAIN)
 *   - Accesses and modifies global army_ptr chain
 *
 * Notes:
 *   - Tactical preference: sweep operations for systematic clearing
 *   - Terrain preference: mountain elevation (ELE_MOUNTAIN)
 *   - No vegetation preference: vtype=-1 (any vegetation acceptable)
 *   - More organized than savages but less defensive than lizards
 *   - Respects map boundaries with XY_INMAP() validation
 *   - Sweep status implies systematic area denial tactics
 */
void
upd_nomads PARM_0(void)
{
  /* start with the basics */
  monster_growth();

  /* go through all of the nomads */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {
    /* find the current sector */
    if (!XY_INMAP(ARMY_XLOC, ARMY_YLOC)) continue;
    sct_ptr = &sct[ARMY_XLOC][ARMY_YLOC];

    /* go roaming */
    set_status(ARMY_STAT, ST_SWEEP);
    monster_move_army(-1, ELE_MOUNTAIN);
  }
}

/*
 * upd_pirates - Update pirate behavior and actions
 *
 * Implements the behavior pattern for pirate monster nations. Currently
 * provides only basic growth without specific movement or tactical patterns.
 * Pirates likely operate primarily through naval forces rather than
 * land-based armies.
 *
 * Parameters:
 *   None (operates on current nation via global ntn_ptr)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Calls monster_growth() for population/resource expansion
 *   - Expands naval fleets and increases material treasures
 *
 * Notes:
 *   - Simplest implementation: only growth, no movement or tactics
 *   - Pirate behavior likely focuses on naval operations
 *   - May be placeholder for future naval-specific AI implementation
 *   - Growth includes naval fleet expansion (warships) and treasure accumulation
 *   - No land-based army movement or tactical decisions implemented
 *   - Relies on monster_growth() for all behavioral updates
 */
void
upd_pirates PARM_0(void)
{
  /* start with the basics */
  monster_growth();
}
