/*
 * dataA.c - Core Game Data Definitions for Conquer
 *
 * This file contains global data structures and variables that define
 * core game state and content for the Conquer strategy game. All data
 * defined here is used exclusively during conqrun (game execution) and
 * provides the foundation for name generation, world state management,
 * and spell processing.
 *
 * Key Components:
 *   - Fantasy name generation syllables (mild_begin array)
 *   - Global game state variables (remake, scenario)
 *   - Spell processing queue management (upd_spl_list)
 *
 * Data Visibility:
 *   - Uses DATA_DECLARE macro to control external visibility
 *   - Coordinates with dataA.h for proper declaration/definition
 *   - Provides actual storage for variables declared extern elsewhere
 *
 * Dependencies:
 *   - dataA.h: Primary header with structure definitions
 *   - magicX.h: Spell system structures and constants
 *   - adduserA.h: User management definitions
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
#define DATA_DECLARE
#include "dataA.h"
#undef DATA_DECLARE
#include "magicX.h"
#include "adduserA.h"

/*
 * mild_begin - Fantasy name prefix syllables for procedural name generation
 *
 * Array of string prefixes used to generate fantasy-style names for 
 * characters, locations, or other game entities in the Conquer game.
 * These syllables are designed to create pronounceable, fantasy-themed
 * names when combined with suffix syllables.
 *
 * Usage:
 *   - Combined with suffix arrays to create full names
 *   - Used by random name generation functions
 *   - Provides consistent fantasy naming convention
 *
 * Notes:
 *   - Contains 77 prefix syllables
 *   - Uses apostrophes and accents for fantasy flavor
 *   - Designed to sound elvish/fantasy-like
 *   - Array is null-terminated string array
 *   - Referenced in dataA.h definitions
 */
char *mild_begin[] = {
  "A'", "Ael", "Aer", "Bai", "Bae", "B`", "Be", "Ben", "Cae", "Cai",
  "D`", "Dae", "De", "Dol", "E`", "Ee'", "Ea", "Ear", "Eir", "Eal",
  "Fa", "Fie", "Feo", "Gan", "Gin", "Hei", "Heo", "Ia", "Ier", "Ii",
  "Iia", "J`", "J'", "Jae", "Je", "Joa", "Kae", "Lae", "Lei", "Lei",
  "M'", "Mae", "Mea", "Mia", "Nae", "Nia", "Oa", "Oae", "Oai", "Oer",
  "Ole", "On", "Pae", "Pai", "Pao", "Q'", "Q`", "Qai", "Qe'", "Qei",
  "Que", "Qui", "Ra", "Rae", "Rai", "Sae", "Sai", "Soi", "Tea", "Tei",
  "Ua", "Ua'", "Uai", "Vae", "Vi", "W'", "Wai", "Wae", "Wa'", "Xai",
  "Xae", "Xea", "Yae", "Yai", "Yea", "Zae", "Zea"
};

/*
 * Global Variables - Runtime State Management
 *
 * These variables maintain global state during game execution
 * and are used across multiple modules for game coordination.
 */

/*
 * remake - World recreation flag
 *
 * Controls whether the game world should be completely regenerated
 * on the next update cycle. When set to non-zero, triggers world
 * reconstruction with new terrain, placement, and initial conditions.
 *
 * Values:
 *   0 - Normal operation, preserve existing world
 *   1 - Recreate world on next update cycle
 *
 * Usage:
 *   - Set by administrative commands
 *   - Checked during world update processing
 *   - Resets to 0 after world recreation completes
 */
int remake;

/*
 * scenario - Current game scenario name
 *
 * Stores the name of the currently active game scenario or campaign.
 * Used to identify which rule set, map, and configuration files to
 * use for the current game session.
 *
 * Size: NAMELTH+1 characters (includes null terminator)
 *
 * Usage:
 *   - Set during game initialization
 *   - Used to load scenario-specific data files
 *   - Displayed in game status and save files
 *   - Must be null-terminated string
 */
char scenario[NAMELTH+1];

/*
 * upd_spl_list - Spell update list pointer
 *
 * Points to the head of a linked list containing spells that need
 * to be processed during the current update cycle. Used to queue
 * spell effects, delays, and ongoing magical processes.
 *
 * Type: SPLINFO_PTR (pointer to spell information structure)
 *
 * Usage:
 *   - Managed by spell processing routines
 *   - Traversed during update cycles
 *   - NULL when no spells are pending
 *   - Dynamically allocated and freed
 */
SPLINFO_PTR upd_spl_list;
