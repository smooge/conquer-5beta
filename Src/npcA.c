/* routines to make a computer calculated update for a nation */
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
#include "moveX.h"
#include "activeX.h"
#include "statusX.h"

/* structure for use by NPC nations */
typedef struct s_npcinfo {
  long total_troops;	/* total number of troops for nation	*/
  long rov_troops;	/* how many troops still need assigning	*/
  long att_troops;	/*  for each of the tasks:  attack,	*/
  long def_troops;	/*  defense, garrison, and roving.	*/
  long gar_troops;
} NPCINFO_STRUCT, *NPCINFO_PTR;

/* sectors targetted for action */
typedef struct s_target {
  maptype mapx, mapy;		/* the location of the sector */
  int value;			/* the "target" value of the sector */
  struct s_target *next;	/* linked list information */
} TARGET_STRUCT, *TARGET_PTR;

/* storage for the NPC code */
TARGET_PTR target_list = NULL;
NPCINFO_STRUCT military_stats;

/*
 * cpu_update - Computer controlled update of the nation
 *
 * Implements the AI strategy system for computer-controlled (non-monster) nations.
 * Currently provides placeholder functionality with framework for comprehensive
 * AI strategy implementation. Most strategic functions are disabled via #ifdef NOT_DONE
 * but the infrastructure exists for full AI nation management.
 *
 * Parameters:
 *   None (operates on current nation via global ntn_ptr)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Writes update message to fupdate file stream
 *   - Would call strategic AI functions if NOT_DONE was undefined
 *   - Accesses nation's aggression level for update messaging
 *
 * Notes:
 *   - Currently incomplete implementation (marked "not fully implemented")
 *   - Framework exists for: strategy assignment, sector duties, buildup,
 *     rover management, stabilization, expansion, distribution, construction
 *   - Uses aggressname array to describe nation's strategic approach
 *   - Designed for future expansion of AI capabilities
 *   - Placeholder for sophisticated nation AI that would rival human players
 */
static void
cpu_update PARM_0(void)
{
  /* give the outward indicator */
  fprintf(fupdate,
	  "    %s mode computer update being made (not fully implemented)\n",
	  aggressname[n_aggression(ntn_ptr->active)]);

#ifdef NOT_DONE
  /* determine strategy */
  assign_strategy();

  /* assign sector designations */
  sector_duties();

  /* reinforce garrisons and build new troops */
  buildup();

  /* set rovers */
  check_rovers();

  /* defensive set */
  stabilize();

  /* offensive set */
  expand();

  /* redistribute goods */
  npc_distribute();

  /* handle any constructions */
  npc_construct();
#endif /* NOT_DONE */
}

/*
 * move_for_ntn - Perform a turn of movement for the current nation
 *
 * Main entry point for nation-level AI updates and movement decisions. Routes
 * nations to appropriate update functions based on their type (monster vs normal)
 * and specific characteristics. Implements the primary AI decision tree for
 * determining how each nation behaves during game turns.
 *
 * Parameters:
 *   None (operates on current nation via global ntn_ptr and country)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Calls monster-specific update functions (upd_lizards, upd_savages, etc.)
 *   - Calls cpu_update() for normal computer-controlled nations
 *   - Writes update messages to fupdate file stream
 *   - Validates nation pointer consistency with global country variable
 *
 * Notes:
 *   - Early return if invalid nation data (NULL pointer or UNOWNED country)
 *   - Performs safety check: ntn_ptr must match world.np[country]
 *   - Monster nations route to specialized behavior functions:
 *     * Lizards: upd_lizards() - defensive swamp-dwellers
 *     * Savages: upd_savages() - aggressive nomadic raiders
 *     * Pirates: upd_pirates() - naval-focused treasure hunters
 *     * Nomads: upd_nomads() - mountain-dwelling systematic sweepers
 *     * Rebellion: placeholder (unimplemented)
 *   - Non-monster nations use cpu_update() for general AI strategy
 *   - Central coordination point for all AI nation behaviors
 */
void
move_for_ntn PARM_0(void)
{
  /* check data being worked on */
  if ((ntn_ptr == NULL) || (country == UNOWNED)) return;
  if (ntn_ptr != world.np[country]) {
    errormsg(" Serious Error:  nation pointer doesn't match country\n");
    return;
  }

  /* determine which form the update will take */
  if (n_ismonster(ntn_ptr->active)) {
    if (n_islizard(ntn_ptr->active)) {
      upd_lizards();
    } else if (n_issavage(ntn_ptr->active)) {
      upd_savages();
    } else if (n_ispirate(ntn_ptr->active)) {
      upd_pirates();
    } else if (n_isnomad(ntn_ptr->active)) {
      upd_nomads();
    } else {
      /* rebellion -- do nothing yet */
      fprintf(fupdate,
	      "    %s mode computer update being made (unimplemented)\n",
	      aggressname[n_aggression(ntn_ptr->active)]);
    }
  } else {
    /* computerized motion */
    cpu_update();
  }
}

/*
 * rover_value - Determine the value of a sector to rover units
 *
 * Calculates the strategic value of a map sector for roving army units.
 * Used by the AI to identify attractive targets for exploration, expansion,
 * or resource gathering. Returns a numeric value representing sector desirability.
 *
 * Parameters:
 *   x - X coordinate of sector to evaluate (must be valid map coordinate)
 *   y - Y coordinate of sector to evaluate (must be valid map coordinate)
 *
 * Returns:
 *   Integer value representing sector attractiveness (0 = no value, higher = better)
 *   Returns 0 for invalid coordinates or already-owned sectors
 *
 * Side Effects:
 *   - Reads sector data from global sct array
 *   - Calls attract_val() to calculate base attractiveness
 *
 * Notes:
 *   - Only considers unowned sectors (sct[x][y].owner == UNOWNED)
 *   - Rejects out-of-bounds coordinates (uses XY_ONMAP validation)
 *   - Currently uses simple attractiveness calculation via attract_val()
 *   - Could be enhanced with strategic considerations like resources, terrain, threats
 *   - Part of rover AI system for intelligent army movement and exploration
 *   - Foundation for more sophisticated territory valuation algorithms
 */
static int
rover_value PARM_2(int, x, int, y)
{
  /* start with a simple ownership check */
  if (!XY_ONMAP(x, y) ||
      (sct[x][y].owner != UNOWNED)) {
    return(0);
  }

  /* just use attractiveness value for now */
  return(attract_val(x, y));
}

/* some variable to store rover information in */
static int rv_xloc, rv_yloc, rv_axloc, rv_ayloc;
static int rv_maxtotal, rv_maxneighbors, rv_mntotal;
static int rvn_total;

/*
 * build_rove - Add rover value of current sector to neighborhood total
 *
 * Callback function used with map_loop to accumulate the total rover value
 * of sectors in a neighborhood. Part of the two-phase rover evaluation system
 * that considers both individual sector value and neighborhood potential.
 *
 * Parameters:
 *   x - X coordinate of sector to evaluate (from map_loop iteration)
 *   y - Y coordinate of sector to evaluate (from map_loop iteration)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Adds sector's rover value to global rvn_total accumulator
 *   - Calls rover_value() to get sector's individual attractiveness
 *
 * Notes:
 *   - Designed as callback for map_loop() function
 *   - Works with rove_loopfunc() to evaluate sector neighborhoods
 *   - Uses global variable rvn_total as accumulator
 *   - Part of intelligent army movement system that considers surrounding potential
 *   - Enables AI to prefer sectors with valuable neighbors (strategic positioning)
 *   - Simple aggregation function supporting complex territorial analysis
 */
static void
build_rove PARM_2(int, x, int, y)
{
  rvn_total += rover_value(x, y);
}

/*
 * rove_loopfunc - Determine the best sector for army relocation
 *
 * Callback function used with map_loop to evaluate potential relocation targets
 * for roving armies. Implements sophisticated sector evaluation considering both
 * individual sector value and neighborhood potential. Tracks the best candidates
 * for different strategic approaches (direct value vs neighborhood value).
 *
 * Parameters:
 *   x - X coordinate of sector to evaluate (from map_loop iteration)
 *   y - Y coordinate of sector to evaluate (from map_loop iteration)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Updates global variables tracking best sector options:
 *     * rv_maxtotal, rv_xloc, rv_yloc - highest individual sector value
 *     * rv_maxneighbors, rv_axloc, rv_ayloc - highest neighborhood value
 *     * rv_mntotal - individual value of best neighborhood sector
 *   - Calls map_loop with build_rove to calculate neighborhood totals
 *   - Uses rvn_total as working variable for neighborhood calculations
 *
 * Notes:
 *   - Designed as callback for map_loop() function called from rove_army()
 *   - Implements dual optimization strategy: best individual vs best neighborhood
 *   - Neighborhood evaluation considers 1-radius surrounding sectors
 *   - Enables strategic positioning for future expansion opportunities
 *   - Global variables serve as "return values" for best sector coordinates
 *   - Part of intelligent movement system that rivals human strategic thinking
 */
static void
rove_loopfunc PARM_2(int, x, int, y)
{
  /* get the value for this sector */
  int mytot = rover_value(x, y);

  /* compute the neighboring potential */
  rvn_total = 0;
  map_loop(x, y, 1, build_rove);

  /* Is this the best neighboring total? */
  if (rvn_total > rv_maxneighbors) {
    rv_maxneighbors = rvn_total;
    rv_mntotal = mytot;
    rv_axloc = x;
    rv_ayloc = y;
  }

  /* Is this the best total? */
  if (mytot > rv_maxtotal) {
    rv_maxtotal = mytot;
    rv_xloc = x;
    rv_yloc = y;
  }
}

/*
 * rove_army - Make the current army unit rove intelligently
 *
 * Implements intelligent army movement for NPC nations, causing armies to seek
 * valuable unoccupied territories. Uses sophisticated decision algorithms that
 * balance immediate sector value against neighborhood potential and include
 * fallback random movement when no attractive targets exist.
 *
 * Parameters:
 *   None (operates on current army via global army_ptr)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Modifies army position if movement is beneficial
 *   - Sets global movemode based on army flight capability
 *   - Calls map_loop with rove_loopfunc to evaluate movement options
 *   - Uses npc_movearmy() to execute actual movement
 *   - May perform multiple random movement attempts (up to 100 tries)
 *
 * Notes:
 *   - Early return if army is NULL, has low movement (< 5), or current sector valuable
 *   - Sets movement mode: MOVE_FLYARMY for flying units, MOVE_ARMY for others
 *   - Three-tier decision algorithm:
 *     1. Stay put if current sector has value (rover_value > 0)
 *     2. Strategic movement to best individual or neighborhood sector
 *     3. Random movement if no strategic targets found
 *   - Probabilistic choice between individual vs neighborhood targets
 *   - Random fallback prevents armies from getting permanently stuck
 *   - Commented out retreat functionality: npc_retreat_army()
 *   - Integrates with broader NPC movement and territorial expansion systems
 */
void
rove_army PARM_0(void)
{
  int count = 0;

  /* stand still if the current sector is of interest */
  if ((army_ptr == NULL) ||
      (ARMY_MOVE < 5) ||
      rover_value(ARMY_XLOC, ARMY_YLOC)) return;

  /* set the movement mode appropriately */
  if (unit_flight(ARMY_STAT)) {
    movemode = MOVE_FLYARMY;
  } else {
    movemode = MOVE_ARMY;
  }

  /* check if they needed to back out */
  /* if (npc_retreat_army(army_ptr)) return; */

  /* determine the best location */
  rv_maxtotal = 0;
  rv_maxneighbors = 0;
  rv_mntotal = 0;
  map_loop(ARMY_XLOC, ARMY_YLOC, 1, rove_loopfunc);

  /* random movement if nothing found */
  if ((rv_maxtotal == 0) &&
      (rv_maxneighbors == 0)) {
    /* relocate to a random neighboring sector */
    do {
      rand_sector(ARMY_XLOC, ARMY_YLOC, 1, TRUE, FALSE);
    } while (!npc_movearmy(global_int, global_long) &&
	     (count++ < 100));
  } else if ((rv_maxtotal == 0) ||
	     (rv_maxtotal == rv_mntotal) ||
	     (rand_val(100) < (100 * rv_mntotal) / (rv_mntotal +
						    rv_maxtotal))) {
    /* go to the one with the highest neighboring total */
    npc_movearmy(rv_axloc, rv_ayloc);
  } else {
    /* go to the one with the highest total */
    npc_movearmy(rv_xloc, rv_yloc);
  }
}
