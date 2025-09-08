/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * Include/combatA.h - Combat System Definitions and Data Structures
 *
 * This header defines the complete combat system including battle mechanics,
 * unit combat roles, damage calculations, and multi-sided battle coordination.
 * The combat system supports complex tactical scenarios with up to 15 different
 * combatants, distance attacks, fortification effects, and specialized unit roles.
 *
 * Key Systems:
 * - Multi-sided combat with diplomatic relationships
 * - Unit grouping by tactical role (sweeper, attacker, defender, etc.)
 * - Damage and usage tracking for combat persistence
 * - Distance attack coordination for ranged combat
 * - Fortification and cover bonus calculations
 * - Spy reconnaissance during battles
 *
 * Combat Flow:
 * 1. Units enter combat and are organized by tactical groups
 * 2. Combat calculations consider unit bonuses, cover, and fortifications
 * 3. Damage is applied and tracked for unit persistence across turns
 * 4. Battle results update unit status and territorial control
 *
 * Thread Safety: Combat structures require synchronization for concurrent
 * access during multi-player battles and automated combat resolution.
 *
 * Performance Notes: Combat calculations are CPU-intensive and may benefit
 * from optimization for large-scale battles with many units.
 */
#include "dstatusX.h"

/*
 * Combat System Constants
 */

/*
 * MAX_COMBAT - Maximum number of combatant sides in a single battle
 *
 * Defines the upper limit for different nations or factions that can
 * participate simultaneously in a single combat engagement. This supports
 * complex multi-party battles with intricate diplomatic relationships.
 *
 * Value: 15 combatant sides maximum
 * Usage: Array sizing for combat structures, diplomatic matrices
 * Considerations: Increasing this value affects memory usage exponentially
 *                for diplomatic relationship matrices (MAX_COMBAT²)
 * Modernization: Consider dynamic allocation for very large battles
 */
#define MAX_COMBAT	15	/* can change w/out invalidating data */

/*
 * Combat Unit Type Classifications
 */

/*
 * Ucombattype - Combat unit type enumeration for battle participation
 *
 * Defines the different types of military units that can engage in combat.
 * Each type has distinct combat characteristics, movement capabilities,
 * and tactical roles within the battle system.
 *
 * Values:
 *   UNIT_NONE - No unit type (placeholder or error state)
 *   UNIT_ARMY - Land-based military forces with ground combat capabilities
 *   UNIT_NAVY - Naval forces for sea combat and coastal bombardment
 *   UNIT_CVN  - Caravan units with limited defensive combat ability
 *
 * Usage: Combat system unit classification, tactical role assignment
 * Integration: Links with ARMY_STRUCT, NAVY_STRUCT, CVN_STRUCT data
 * Modernization: Consider adding air units or specialized forces
 */
typedef enum ucombattype {
  UNIT_NONE, UNIT_ARMY, UNIT_NAVY, UNIT_CVN
} Ucombattype;

/*
 * Combat Usage Values (Legacy - Currently Disabled)
 *
 * These constants were designed to track unit usage levels during combat
 * to implement fatigue, tactical withdrawal, and combat completion states.
 * Currently disabled but preserved for potential future implementation.
 *
 * Intended Values:
 *   CUSE_NONE     - Unit not yet engaged in combat
 *   CUSE_FULL     - Unit fully committed to battle (100% usage)
 *   CUSE_SKIP     - Unit skipping current combat round
 *   CUSE_FINISHED - Unit has completed combat participation
 *
 * Legacy Status: Disabled pending combat system refinement
 * Modernization: Could be reimplemented with better state management
 */
#ifdef NOTUSED
#define CUSE_NONE	0
#define CUSE_FULL	100
#define CUSE_SKIP	101
#define CUSE_FINISHED	102
#endif /* NOTUSED */

/*
 * Combat Sector Status Flags
 *
 * These flags mark sectors based on their current combat involvement,
 * enabling the system to track active battle locations and manage
 * combat state across the game world map.
 */

/*
 * CBSCT_NONE - No combat activity in sector
 *
 * Indicates a peaceful sector with no active combat engagement.
 * Used as the default state for sectors without military conflict.
 *
 * Value: 0 (neutral/inactive state)
 * Usage: Default sector combat status, peaceful territory marking
 */
#define CBSCT_NONE	0

/*
 * CBSCT_BATTLE - Active combat in progress in sector
 *
 * Marks a sector as having an ongoing battle with units engaged in combat.
 * This flag triggers combat resolution routines and prevents normal
 * movement through the sector during battle.
 *
 * Value: 1 (active combat state)
 * Usage: Battle zone identification, movement restriction, combat processing
 * Effects: Blocks civilian movement, triggers combat calculations
 */
#define CBSCT_BATTLE	1

/*
 * Combat Group System - Tactical Role Classifications
 *
 * The combat system organizes units into tactical groups based on their
 * role in battle. Each group has distinct characteristics affecting
 * combat calculations, positioning, and damage resolution.
 */

/*
 * CGRP_NUMBER - Total number of tactical groups in combat
 *
 * Defines the number of different tactical roles units can assume
 * during battle. Used for array sizing and group iteration.
 *
 * Value: 5 tactical groups total
 * Usage: Array dimensions, combat role validation, group processing loops
 */
#define CGRP_NUMBER	5

/*
 * Cgrptype - Combat group tactical role enumeration
 *
 * Defines the different tactical roles units can assume in battle.
 * Each role has specific combat characteristics, positioning rules,
 * and damage calculations that affect battle outcomes.
 *
 * Tactical Roles:
 *   CGRP_SWEEPER   - Fast attack units for reconnaissance and harassment
 *   CGRP_ATTACKER  - Primary assault forces for offensive operations
 *   CGRP_DEFENDER  - Defensive units protecting key positions
 *   CGRP_FORTIFIED - Units with defensive bonuses from fortifications
 *   CGRP_PROTECTED - Special units with enhanced protection or support
 *
 * Usage: Unit tactical assignment, combat calculation modifiers
 * Integration: Used with CSIDE_STRUCT for group-based combat statistics
 * Strategy: Different groups have varying combat effectiveness and survivability
 */
typedef enum cgrptype {
  CGRP_SWEEPER, CGRP_ATTACKER, CGRP_DEFENDER, CGRP_FORTIFIED,
  CGRP_PROTECTED
} Cgrptype;

/*
 * Combat Data Structures
 */

/*
 * CUNIT_STRUCT - Individual combat unit representation
 *
 * Represents a single military unit participating in combat with complete
 * state tracking for damage, usage, bonuses, and tactical capabilities.
 * This structure maintains both combat-specific data and references to
 * the original unit data for persistent state management.
 *
 * Fields:
 *   owner           - Nation that controls this combat unit (ntntype)
 *   type            - Unit classification: army, navy, or caravan (Ucombattype)
 *   usage           - Combat fatigue/usage level (0-255 range)
 *   damage          - Accumulated battle damage (0-255 range)
 *   rel_size        - Relative combat strength/size of the unit
 *   adjustment      - Combat bonus/penalty modifier level
 *   destruct_value  - Capability to destroy fortifications and structures
 *   ui              - Union pointer to actual unit data (army/navy/caravan)
 *   next            - Linked list pointer for combat unit organization
 *
 * Usage: Combat calculations, damage tracking, unit state management
 * Integration: Links to ARMY_STRUCT, NAVY_STRUCT, CVN_STRUCT via union
 * Memory: Organized as linked lists within tactical groups
 * Modernization: Consider atomic operations for concurrent damage updates
 */
typedef struct s_cunit {
  ntntype owner;	/* owner of the unit in combat		*/
  Ucombattype type;	/* army, navy or caravan indicator	*/
  uns_char usage;	/* how much the unit has been used	*/
  uns_char damage;	/* how much the unit has been damaged	*/
  long rel_size;	/* relative size of the unit		*/
  int adjustment;	/* bonus level of the unit		*/
  int destruct_value;	/* potential for destroying fortif..	*/
  union {
    ARMY_PTR army_p;	/* pointer to any real army unit	*/
    NAVY_PTR navy_p;	/* pointer to any real navy unit	*/
    CVN_PTR cvn_p;	/* pointer to any real caravan unit	*/
  } ui;
  struct s_cunit *next;
} CUNIT_STRUCT, *CUNIT_PTR;

/*
 * CSIDE_STRUCT - Combat side/faction data and statistics
 *
 * Represents a complete combatant faction in battle, containing all units,
 * tactical statistics, diplomatic relationships, and combat bonuses for
 * a single nation or alliance participating in the engagement.
 *
 * This structure aggregates combat data across all tactical groups and
 * manages the complex interactions between different unit types, special
 * abilities, and diplomatic relationships during battle resolution.
 *
 * Fields:
 *   owner          - Nation controlling this combat faction (ntntype)
 *   attack_cover   - Defensive bonuses from specialized assault troops
 *   fort_cover     - Additional protection from fortification specialists
 *   has_spys       - Intelligence capability flag for battle observation
 *   zombie_mkr[]   - Undead creation potential per tactical group
 *   sum_size[]     - Total combat strength aggregated by tactical group
 *   avg_bonus[]    - Average unit bonus levels per tactical group
 *   rstat[][]      - Diplomatic relationship matrix with other combatants
 *   units[]        - Linked lists of combat units organized by tactical group
 *
 * Combat Flow:
 * 1. Units are organized into tactical groups (sweeper, attacker, etc.)
 * 2. Statistics are calculated for each group (size, bonuses, capabilities)
 * 3. Diplomatic relationships determine targeting and alliance behavior
 * 4. Special abilities (spies, zombies) provide tactical advantages
 *
 * Usage: Battle resolution, diplomatic calculations, unit coordination
 * Integration: Contains CUNIT_PTR arrays linking to individual units
 * Performance: Critical structure for combat calculations - optimize access
 * Modernization: Consider cache-friendly layout for combat-heavy operations
 */
typedef struct s_cside {
  ntntype owner;		/* which nation controls this combatant	*/
  int attack_cover;		/* cover bonuses due to special troops	*/
  int fort_cover;		/* cover bonuses due to special troops	*/
  int has_spys;			/* indicates that spys can see battle	*/
  long zombie_mkr[CGRP_NUMBER];	/* how many zomies can be created	*/
  long sum_size[CGRP_NUMBER];	/* total strength of all units		*/
  double avg_bonus[CGRP_NUMBER];	/* avg bonus level of the units	*/
  Diplotype rstat[CGRP_DEFENDER][MAX_COMBAT];	/* attitude for others	*/
  CUNIT_PTR units[CGRP_NUMBER];	/* the actual units within the combat	*/
} CSIDE_STRUCT, *CSIDE_PTR;

/*
 * CDIST_STRUCT - Distance attack coordination structure
 *
 * Manages ranged combat units and their positioning for distance attacks
 * such as artillery bombardment, naval gunfire support, and siege weapons.
 * This structure enables complex tactical coordination between units at
 * different map locations participating in the same battle.
 *
 * Fields:
 *   xloc, yloc - Map coordinates of the attacking unit's position
 *   unit       - Pointer to the combat unit capable of distance attacks
 *   next       - Linked list pointer for multiple distance attackers
 *
 * Combat Applications:
 * - Naval bombardment of coastal targets
 * - Artillery support from adjacent sectors
 * - Siege weapon coordination against fortifications
 * - Long-range magical or technological attacks
 *
 * Usage: Distance combat calculations, ranged unit coordination
 * Integration: Links with CUNIT_STRUCT for unit capabilities
 * Tactical: Enables combined arms operations across multiple map sectors
 * Modernization: Consider spatial indexing for efficient range calculations
 */
typedef struct s_cdist {
  int xloc, yloc;		/* the position of the unit */
  CUNIT_PTR unit;		/* the unit under consideration */
  struct s_cdist *next;		/* the next distance element */
} CDIST_STRUCT, *CDIST_PTR;

/*
 * COMBAT_STRUCT - Complete battle management structure
 *
 * Represents a complete combat engagement at a specific map location,
 * coordinating all participating factions, their units, and the overall
 * battle state. This is the top-level structure that manages complex
 * multi-sided battles with diplomatic relationships and tactical coordination.
 *
 * Fields:
 *   xloc, yloc - World map coordinates where the battle is taking place
 *   cside[]    - Array of combat factions participating in the battle
 *   next       - Linked list pointer for multiple battles in same sector
 *
 * Battle Management:
 * - Coordinates up to MAX_COMBAT (15) different factions
 * - Manages diplomatic relationships between all participants
 * - Tracks tactical groups and unit assignments for each faction
 * - Handles complex alliance and enemy interactions
 * - Supports simultaneous battles in the same sector
 *
 * Combat Resolution Process:
 * 1. All factions organize their units into tactical groups
 * 2. Diplomatic relationships determine targeting priorities
 * 3. Combat calculations process damage and casualties
 * 4. Battle results update unit status and territorial control
 * 5. Multiple battles can occur simultaneously in large sectors
 *
 * Usage: Top-level battle coordination, multi-faction combat management
 * Integration: Contains CSIDE_PTR array for all participating factions
 * Performance: Central structure for combat system - optimize for efficiency
 * Scalability: Linked list allows multiple simultaneous battles per sector
 * Modernization: Consider concurrent access protection for multiplayer battles
 */
typedef struct s_combat {
  maptype xloc, yloc;		/* the position of the sector in world	*/
  CSIDE_PTR cside[MAX_COMBAT];	/* the actual list of combatents	*/
  struct s_combat *next;	/* the next among those in the sector	*/
} COMBAT_STRUCT, *COMBAT_PTR;
