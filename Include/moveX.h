/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * Include/moveX.h - Movement System Definitions and Types
 *
 * This header defines the movement system types, constants, and error codes
 * used throughout the Conquer game for unit movement and pathfinding.
 * The movement system handles various unit types (army, navy, air) and
 * validates movement legality based on terrain, diplomatic relations,
 * and game rules.
 *
 * Key Components:
 * - Movement type enumeration for different unit movement modes
 * - Movement error codes for validation and user feedback
 * - Movement indicator constants for UI selection display
 *
 * Usage Context:
 * - Core movement validation and execution system
 * - User interface movement command processing
 * - Pathfinding and route calculation algorithms
 * - Diplomatic and territorial access control
 */

/*
 * enum movetype - Unit Movement Type Definitions
 *
 * Defines the different types of movement commands available for units
 * in the game. Each movement type has specific rules, restrictions,
 * and behaviors that affect how units can traverse the game world.
 *
 * Movement types control both the method of movement (land, sea, air)
 * and special movement behaviors (teleport, patrol, one-way missions).
 *
 * Values:
 *   MOVE_CANCEL   (-2) - Cancel current movement command, return to idle
 *   MOVE_NOMOVE   (-1) - No movement planned, unit remains stationary
 *   MOVE_ARMY     (0)  - Standard land-based army movement across terrain
 *   MOVE_NAVY     (1)  - Naval movement restricted to water sectors
 *   MOVE_CVN      (2)  - Aircraft carrier movement with special rules
 *   MOVE_PEOPLE   (3)  - Civilian population movement and migration
 *   MOVE_TELEPORT (4)  - Instant movement ignoring terrain and distance
 *   MOVE_FLYARMY  (5)  - Air transport of army units over obstacles
 *   MOVE_FLYCVN   (6)  - Air transport of carrier units (helicopter carriers)
 *   MOVE_PATROL   (7)  - Automated patrol movement along defined routes
 *   MOVE_ONEWAY   (8)  - One-way mission movement (suicide missions)
 *
 * Notes:
 * - Negative values indicate special states or cancellation commands
 * - Movement validation depends on unit type, terrain, and diplomatic status
 * - Air movement types can bypass certain terrain restrictions
 * - Patrol and one-way movements have automated behavior components
 */
typedef enum movetype {
  MOVE_CANCEL = (-2), /* Cancel movement command, return unit to idle state */
  MOVE_NOMOVE,        /* No movement planned, unit remains at current position */
  MOVE_ARMY,          /* Standard army movement across land terrain */
  MOVE_NAVY,          /* Naval movement restricted to water sectors */
  MOVE_CVN,           /* Aircraft carrier movement with landing capabilities */
  MOVE_PEOPLE,        /* Civilian population movement and migration */
  MOVE_TELEPORT,      /* Instant teleportation ignoring terrain restrictions */
  MOVE_FLYARMY,       /* Air transport of army units over terrain obstacles */
  MOVE_FLYCVN,        /* Air transport of carrier units (helicopter operations) */
  MOVE_PATROL,        /* Automated patrol movement along predefined routes */
  MOVE_ONEWAY         /* One-way mission movement (typically suicide missions) */
} Movetype;

/*
 * enum mverrtype - Movement Error Code Definitions
 *
 * Defines specific error conditions that can prevent or restrict unit
 * movement. These codes provide detailed feedback for movement validation
 * failures and help the UI display appropriate error messages to users.
 *
 * Error codes are arranged roughly by severity, with more serious
 * violations (acts of war) having more negative values.
 *
 * Values:
 *   MV_ACTOFWAR     (-18) - Movement would constitute an act of war
 *   MV_HOSTILE      (-17) - Movement through hostile territory prohibited
 *   MV_ONEWAY       (-16) - One-way movement restriction prevents return
 *   MV_BADBRIDGE    (-15) - Bridge infrastructure inadequate for unit type
 *   MV_UNSEEN       (-14) - Destination sector not visible (fog of war)
 *   MV_DRAG         (-13) - Unit dragging restrictions prevent movement
 *   MV_OTHNATION    (-12) - Other nation's territory, access denied
 *   MV_NOACCESS     (-11) - No access rights to destination sector
 *   MV_HOSTILESCOUT (-10) - Hostile forces detected, movement unsafe
 *   MV_UNMET        (-9)  - Unmet nation controls area, diplomacy required
 *   MV_NOTWALL      (-8)  - Wall required for this type of movement
 *   MV_TOOFAR       (-7)  - Destination exceeds maximum movement range
 *   MV_LANDING      (-6)  - Landing zone unsuitable for unit type
 *   MV_INHABITABLE  (-5)  - Sector uninhabitable for civilian units
 *   MV_WATER        (-4)  - Water obstacle blocks land unit movement
 *   MV_GROUND       (-3)  - Ground obstacle blocks naval unit movement
 *   MV_IMPASSABLE   (-2)  - Terrain completely impassable for unit type
 *   MV_OFFMAP       (-1)  - Movement destination is off the game map
 *
 * Notes:
 * - More negative values indicate more serious violations
 * - Diplomatic errors (MV_ACTOFWAR, MV_HOSTILE) may trigger game events
 * - Terrain errors (MV_WATER, MV_GROUND) are basic movement restrictions
 * - Infrastructure errors (MV_BADBRIDGE) indicate development needs
 */
typedef enum mverrtype {
  MV_ACTOFWAR = (-18),    /* Movement would constitute an act of war against another nation */
  MV_HOSTILE,             /* Movement through hostile territory is prohibited */
  MV_ONEWAY,              /* One-way movement restriction prevents return journey */
  MV_BADBRIDGE,           /* Bridge infrastructure inadequate for unit weight/type */
  MV_UNSEEN,              /* Destination sector not visible due to fog of war */
  MV_DRAG,                /* Unit dragging/towing restrictions prevent movement */
  MV_OTHNATION,           /* Other nation's territory, diplomatic access required */
  MV_NOACCESS,            /* No access rights or clearance for destination sector */
  MV_HOSTILESCOUT,        /* Hostile forces detected in area, movement unsafe */
  MV_UNMET,               /* Unmet nation controls area, first contact required */
  MV_NOTWALL,             /* Wall or fortification required for this movement type */
  MV_TOOFAR,              /* Destination exceeds unit's maximum movement range */
  MV_LANDING,             /* Landing zone unsuitable or unavailable for unit type */
  MV_INHABITABLE,         /* Sector uninhabitable or unsuitable for civilian units */
  MV_WATER,               /* Water obstacle blocks land-based unit movement */
  MV_GROUND,              /* Ground obstacle blocks naval unit movement */
  MV_IMPASSABLE,          /* Terrain completely impassable for this unit type */
  MV_OFFMAP               /* Movement destination is outside the game map boundaries */
} Mverrtype;

/*
 * Movement Selection Indicator Constants
 *
 * These constants define different movement mode indicators used by the
 * user interface to highlight valid movement options and display movement
 * capabilities for different unit types.
 *
 * Used by the movement selection system to visually distinguish between
 * normal ground movement, flight movement, and naval movement when
 * players are planning unit actions.
 *
 * Constants:
 *   MOVEIND_NORM  (0) - Normal ground-based movement indicator
 *   MOVEIND_FLY   (1) - Flight/air movement capability indicator  
 *   MOVEIND_WATER (2) - Naval/water movement capability indicator
 *
 * Notes:
 * - Used primarily for UI highlighting and movement option display
 * - Values correspond to different visual indicators or cursor modes
 * - Help players understand unit movement capabilities at a glance
 */
#define MOVEIND_NORM	0   /* Normal ground movement indicator for land units */
#define MOVEIND_FLY	1   /* Flight movement indicator for air-capable units */
#define MOVEIND_WATER	2   /* Water movement indicator for naval units */
