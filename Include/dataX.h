/*
 * dataX.h - Extended Data Structures and Core Game Constants
 *
 * This header file defines the foundational data structures, constants, and
 * global variables used throughout the Conquer game system. It serves as the
 * central hub for both the Administrative Interface (conqrun) and User Interface
 * (conquer), providing core game entities, world configuration, and system-wide
 * constants that drive game mechanics and functionality.
 *
 * Key Components:
 *   - Core data structures (SCT_STRUCT, ARMY_STRUCT, NAVY_STRUCT, etc.)
 *   - Game balance constants (population limits, economic parameters, combat values)
 *   - World configuration structure (map size, nations, game parameters)
 *   - String length limits and buffer size constants
 *   - System operation macros and utility functions
 *   - Global state variables and file handles
 *
 * Integration Notes:
 *   - Used by: Both administrative and user interface programs
 *   - Depends on: header.h (core constants), sysconf.h (system config), paramX.h (parameters)
 *   - Provides: Game entity structures, world state, operational constants
 *   - Includes: Conditional compilation for data declarations and function prototypes
 *
 * Modernization Considerations:
 *   - Replace fixed-size arrays with dynamic allocation where appropriate
 *   - Update integer types for 64-bit portability (long→size_t for counts)
 *   - Add const qualifiers for read-only data structures
 *   - Consider thread safety for global state variables
 *   - Replace legacy function pointers with modern callback patterns
 *   - Update file I/O operations for modern systems
 *   - Add bounds checking for all array access operations
 *   - Consider using enums instead of #define constants for type safety
 *
 * Architecture Role:
 *   - Provides unified data model for entire game system
 *   - Enables shared data structures between UI and administrative components
 *   - Defines game world representation and entity relationships
 *   - Establishes system-wide constants for consistent behavior
 *   - Supports both real-time gameplay and batch processing modes
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

/* ============================================================================
 * HEADER DEPENDENCIES AND COMPILATION CONTROL
 * ============================================================================
 * Purpose: Define required headers and prevent multiple inclusion
 * Usage: Foundation includes for all game system components
 * Notes: Central header that should remain stable across system modifications
 */

/* ::: THIS FILE SHOULD NOT NEED TO BE ALTERED ::: */
#include "header.h"    /* Core system constants and basic definitions */
#include "sysconf.h"   /* System configuration and platform-specific settings */
#include "paramX.h"    /* Game parameter definitions and configuration values */

/* ============================================================================
 * INCLUSION GUARD AND SCOPE DEFINITION
 * ============================================================================
 * Purpose: Prevent multiple inclusion and define shared program scope
 * Usage: Included by both administrative and user interface programs
 * Notes: BIGINT serves as inclusion guard and overflow detection constant
 */

/* check for inclusion */
#ifndef BIGINT

/*
 * SHARED PROGRAM SCOPE DEFINITION
 *
 * This file is to be included by all files composing either the Administrative
 * Interface (conqrun) or the User Interface (conquer). Only those definitions
 * used in both programs should be set here.
 *
 * Scope Boundaries:
 *   - Shared between: Administrative Interface (conqrun) and User Interface (conquer)
 *   - Excludes: Program-specific definitions (use dataA.h or dataG.h instead)
 *   - Includes: Core game entities, world state, system constants
 *   - Purpose: Unified data model and constants for entire game system
 */

/* ============================================================================
 * SYSTEM OPERATION CONSTANTS
 * ============================================================================
 * Purpose: Define standard return values and logical constants
 * Usage: Function return values, program exit codes, conditional logic
 * Notes: Standard Unix conventions for success/failure indication
 */

/*
 * FAIL - Standard failure exit status
 *
 * Standard Unix exit status indicating program failure or error condition.
 * Used throughout the system for error reporting and program termination.
 *
 * Value: 1 (standard Unix failure code)
 * Units: Exit status code
 * Usage: Program termination, error indication, function return values
 * Notes: Follows Unix convention where non-zero indicates failure
 */
#define FAIL	1

/*
 * SUCCESS - Standard success exit status
 *
 * Standard Unix exit status indicating successful program completion or
 * operation. Used throughout the system for success reporting.
 *
 * Value: 0 (standard Unix success code)
 * Units: Exit status code
 * Usage: Program termination, success indication, function return values
 * Notes: Follows Unix convention where zero indicates success
 */
#define SUCCESS	0

/* ============================================================================
 * LOGICAL CONSTANTS (Conditional Definition)
 * ============================================================================
 * Purpose: Define boolean logic constants if not already defined
 * Usage: Conditional logic, boolean operations, flag variables
 * Notes: Protected by inclusion guard to prevent redefinition conflicts
 */

/*
 * Standard Logical Definitions
 * Purpose: Provide boolean constants for logical operations
 * Usage: Boolean variables, conditional expressions, flag settings
 * Notes: Only defined if not already available from system headers
 */
#ifndef TRUE
#define TRUE	1    /* Boolean true value (non-zero) */
#define FALSE	0    /* Boolean false value (zero) */
#endif /*TRUE*/

/* ============================================================================
 * CORE SYSTEM CONSTANTS
 * ============================================================================
 * Purpose: Define fundamental system limits and configuration values
 * Usage: Security, overflow detection, string processing, game balance
 * Notes: Critical values that affect system behavior and security
 */

/*
 * SALT - Cryptographic salt for password hashing
 *
 * Simple salt string used with crypt() function for password hashing.
 * Provides basic protection against rainbow table attacks on stored passwords.
 *
 * Value: "az" (two-character salt)
 * Units: String literal
 * Usage: Password encryption, user authentication
 * Notes: Legacy DES crypt salt, consider modern hashing (bcrypt, scrypt)
 * Security: Minimal protection, should be replaced with random salts
 */
#define SALT		"az"

/*
 * BIGINT - Integer overflow detection threshold
 *
 * Large integer value used for overflow detection and range checking in
 * numerical calculations. Also serves as inclusion guard for this header.
 *
 * Value: 500000000L (500 million)
 * Units: Long integer
 * Usage: Overflow detection, bounds checking, inclusion guard
 * Notes: Chosen to be large but safely within 32-bit signed range
 * Modernization: Consider using LONG_MAX or SIZE_MAX for portability
 */
#define BIGINT	500000000L

/*
 * BIGDOUBLE - Floating point overflow detection threshold
 *
 * Large floating point value used for overflow detection in double-precision
 * calculations. Provides range checking for floating point operations.
 *
 * Value: 3.0E99 (3 × 10^99)
 * Units: Double precision floating point
 * Usage: Floating point overflow detection, bounds checking
 * Notes: Well within double precision range on modern systems
 * Modernization: Consider using DBL_MAX from float.h
 */
#define BIGDOUBLE	3.0E99

/*
 * BREAKJIHAD - Cost to break diplomatic confederacy or jihad
 *
 * Economic cost in game currency required to break confederacy or jihad
 * diplomatic relationships. Controls game balance for diplomatic actions.
 *
 * Value: 200000L (200,000 currency units)
 * Units: Game currency (talons)
 * Usage: Diplomatic system, economic balance, confederation mechanics
 * Notes: High cost reflects serious nature of breaking alliances
 */
#define BREAKJIHAD	200000L

/* ============================================================================
 * STRING LENGTH CONSTANTS
 * ============================================================================
 * Purpose: Define maximum string lengths for various game elements
 * Usage: Buffer allocation, input validation, data structure sizing
 * Notes: Critical for preventing buffer overflows and memory management
 */

/*
 * PASSLTH - Maximum password length
 *
 * Maximum number of characters allowed in user passwords. Used for buffer
 * allocation and input validation in authentication systems.
 *
 * Value: 8 characters
 * Units: Character count
 * Usage: Password buffers, authentication, input validation
 * Notes: Very short by modern standards, consider increasing to 64+ chars
 * Security: Insufficient length for strong passwords
 */
#define PASSLTH		8

/*
 * NAMELTH - Maximum nation name length
 *
 * Maximum number of characters allowed in nation names. Used throughout
 * the system for nation identification and display.
 *
 * Value: 10 characters
 * Units: Character count
 * Usage: Nation name buffers, display formatting, data structures
 * Notes: Quite short, may limit creative nation naming
 */
#define NAMELTH		10

/*
 * LEADERLTH - Maximum leader name length
 *
 * Maximum number of characters allowed in leader names or titles.
 * Used for displaying nation leadership information.
 *
 * Value: 10 characters
 * Units: Character count
 * Usage: Leader name buffers, display formatting, nation data
 * Notes: Short length may limit descriptive leader titles
 */
#define LEADERLTH	10

/*
 * DISPLAYLTH - Maximum display name length
 *
 * Maximum number of characters for display names and interface labels.
 * Used for UI elements and formatted output.
 *
 * Value: 40 characters
 * Units: Character count
 * Usage: Display formatting, UI labels, output strings
 * Notes: Reasonable length for most display purposes
 */
#define DISPLAYLTH	40

/*
 * LINELTH - Generic string buffer length
 *
 * Standard buffer size for general-purpose string operations. Used
 * throughout the system for temporary string storage and processing.
 *
 * Value: 100 characters
 * Units: Character count
 * Usage: String buffers, temporary storage, general string operations
 * Notes: Good balance between memory usage and functionality
 */
#define LINELTH		100

/*
 * FILELTH - Maximum file name length
 *
 * Maximum number of characters allowed in file names and paths.
 * Used for file I/O operations and path management.
 *
 * Value: 255 characters
 * Units: Character count
 * Usage: File path buffers, I/O operations, path validation
 * Notes: Matches traditional Unix file name limits
 * Modernization: Consider PATH_MAX from limits.h for portability
 */
#define FILELTH		255

/*
 * BIGLTH - Large string buffer length
 *
 * Buffer size for large string operations requiring extra space.
 * Used for complex string processing and large text handling.
 *
 * Value: 500 characters
 * Units: Character count
 * Usage: Large string buffers, text processing, formatted output
 * Notes: Suitable for multi-line text and complex formatting
 */
#define BIGLTH		500

/*
 * OPT_LTH - Maximum option name length
 *
 * Maximum number of characters allowed in configuration option names.
 * Used for option parsing and configuration management.
 *
 * Value: 40 characters
 * Units: Character count
 * Usage: Option parsing, configuration, command line arguments
 * Notes: Reasonable length for most configuration options
 */
#define OPT_LTH		40

/*
 * MOTDLINES - Maximum message of the day lines
 *
 * Maximum number of lines allowed in the message of the day display.
 * Controls MOTD formatting and buffer allocation.
 *
 * Value: 4 lines
 * Units: Text lines
 * Usage: MOTD display, welcome messages, server announcements
 * Notes: Compact size keeps MOTD concise and manageable
 */
#define MOTDLINES	4

/*
 * FULLPCT - Maximum percentage value for national statistics
 *
 * Maximum percentage value for various national statistics and settings.
 * Used for validation and display of percentage-based values.
 *
 * Value: 100 (representing 100%)
 * Units: Percentage
 * Usage: National statistics, percentage validation, display formatting
 * Notes: Standard percentage maximum for various game mechanics
 */
#define FULLPCT		100

/* ============================================================================
 * ENVIRONMENT VARIABLE CONFIGURATION
 * ============================================================================ 
 * Purpose: Define environment variable names for system configuration
 * Usage: Runtime configuration, path discovery, user customization
 * Notes: Allows external configuration without recompilation
 */

/*
 * ENVIRON_OPTS - Environment variable for game options
 *
 * Environment variable name used to store default game options and
 * preferences. Allows users to customize behavior without command line args.
 *
 * Value: "CONQ_OPTS"
 * Usage: Option parsing, user preferences, default settings
 * Notes: Checked at startup for default option values
 */
#define ENVIRON_OPTS	"CONQ_OPTS"

/*
 * ENVIRON_DFLT - Environment variable for default directory
 *
 * Environment variable name for specifying the default data directory.
 * Used to locate game data files when not specified explicitly.
 *
 * Value: "CONQ_DEFAULTDIR" 
 * Usage: File path resolution, data directory location
 * Notes: Overrides compiled-in default data directory path
 */
#define ENVIRON_DFLT	"CONQ_DEFAULTDIR"

/*
 * ENVIRON_HELP - Environment variable for help directory
 *
 * Environment variable name for specifying the help files directory.
 * Used to locate documentation and help files.
 *
 * Value: "CONQ_HELPDIR"
 * Usage: Help system, documentation display, file location
 * Notes: Allows custom help file installation locations
 */
#define ENVIRON_HELP	"CONQ_HELPDIR"

/*
 * ENVIRON_EXEC - Environment variable for executable directory
 *
 * Environment variable name for specifying the executable directory.
 * Used to locate supporting programs and utilities.
 *
 * Value: "CONQ_EXEDIR"
 * Usage: Program execution, utility location, path resolution
 * Notes: Supports non-standard installation directories
 */
#define ENVIRON_EXEC	"CONQ_EXEDIR"

/*
 * CONQRC_FILE - User configuration file name
 *
 * Standard name for user configuration files containing personal
 * preferences and customizations.
 *
 * Value: ".conqrc"
 * Usage: Configuration loading, user preferences, customization
 * Notes: Hidden file following Unix convention (leading dot)
 */
#define CONQRC_FILE	".conqrc"

/*
 * CQRC_TAG - Configuration file tag (duplicate definition)
 *
 * Duplicate definition of configuration file name. May be legacy
 * or used in different contexts.
 *
 * Value: ".conqrc"
 * Usage: Configuration file identification
 * Notes: Duplicate of CONQRC_FILE, consider consolidation
 */
#define CQRC_TAG	".conqrc"

/* ============================================================================
 * GAME STATE CONSTANTS
 * ============================================================================
 * Purpose: Define special values for game state and ownership
 * Usage: Land ownership, communication system, state identification
 * Notes: Special values that have semantic meaning in game logic
 */

/*
 * UNOWNED - Unowned land/sector indicator
 *
 * Special value indicating that a sector or land area is not owned by
 * any nation. Used throughout the ownership and territorial systems.
 *
 * Value: 0 (no nation owns this)
 * Units: Nation identifier
 * Usage: Sector ownership, territorial control, map display
 * Notes: Zero value allows easy testing for ownership (if owner != UNOWNED)
 */
#define UNOWNED		0

/*
 * NEWSPAPER - News mailing system identifier
 *
 * Special identifier used for the news mailing system to distinguish
 * news messages from regular mail communications.
 *
 * Value: -1 (negative nation ID)
 * Units: Nation/sender identifier
 * Usage: Mail system, news distribution, message routing
 * Notes: Negative value distinguishes from valid nation IDs
 */
#define NEWSPAPER	-1

/* ============================================================================
 * ARRAY SIZE CONSTANTS
 * ============================================================================
 * Purpose: Define array dimensions for various game data structures
 * Usage: Array declarations, loop bounds, memory allocation
 * Notes: Critical for data structure sizing and bounds checking
 */

/*
 * MTRLS_NUMBER - Number of material types
 *
 * Total number of different material/resource types in the game.
 * Used for resource arrays and economic calculations.
 *
 * Value: 5 material types
 * Usage: Resource arrays, economic systems, material storage
 * Notes: Core economic system parameter affecting resource complexity
 */
#define MTRLS_NUMBER	5

/*
 * MIN_NUMBER - Number of mineral types
 *
 * Total number of different mineral types available in the game.
 * Used for mineral-related arrays and geological systems.
 *
 * Value: 12 mineral types
 * Usage: Mineral arrays, geology, sector resources
 * Notes: Affects geological diversity and mining complexity
 */
#define MIN_NUMBER	12

/*
 * MAJ_NUMBER - Number of major designation types
 *
 * Total number of major sector designation types (cities, farms, etc.).
 * Used for sector classification and economic organization.
 *
 * Value: 16 major designation types
 * Usage: Sector designations, economic calculations, city types
 * Notes: Core parameter affecting sector type diversity
 */
#define MAJ_NUMBER	16

/*
 * ELE_NUMBER - Number of elevation types
 *
 * Total number of different elevation/terrain height categories.
 * Used for terrain generation and tactical calculations.
 *
 * Value: 6 elevation types
 * Usage: Terrain generation, elevation arrays, tactical bonuses
 * Notes: Affects terrain complexity and combat calculations
 */
#define ELE_NUMBER	6

/*
 * VEG_NUMBER - Number of vegetation types
 *
 * Total number of different vegetation/climate types.
 * Used for terrain generation and environmental systems.
 *
 * Value: 12 vegetation types
 * Usage: Terrain generation, vegetation arrays, environmental effects
 * Notes: Affects terrain diversity and environmental modeling
 */
#define VEG_NUMBER	12

/*
 * RACE_NUMBER - Number of race types
 *
 * Total number of different races available for nations.
 * Used for race-specific arrays and cultural systems.
 *
 * Value: 10 race types
 * Usage: Race arrays, cultural bonuses, nation characteristics
 * Notes: Core parameter affecting cultural diversity and bonuses
 */
#define RACE_NUMBER	10

/*
 * MAG_NUMBER - Number of magic/power categories
 *
 * Total number of different magic power categories or schools.
 * Used for magic system arrays and spell organization.
 *
 * Value: 3 magic categories
 * Usage: Magic arrays, spell systems, power calculations
 * Notes: Affects magic system complexity and spell organization
 */
#define MAG_NUMBER	3

/*
 * BUTE_NUMBER - Number of attribute types
 *
 * Total number of different national attributes or characteristics.
 * Used for attribute arrays and nation customization.
 *
 * Value: 19 attribute types
 * Usage: Attribute arrays, nation characteristics, bonuses
 * Notes: Affects nation customization depth and characteristic variety
 */
#define BUTE_NUMBER	19

/*
 * AU_NUMBER - Number of automatic unit types
 *
 * Total number of different automatic unit numbering categories.
 * Used for unit numbering and organization systems.
 *
 * Value: 11 automatic unit types
 * Usage: Unit numbering, organizational systems, ID management
 * Notes: Affects unit organization and automatic numbering complexity
 */
#define AU_NUMBER	11

/*
 * NSHP_NUMBER - Number of naval ship types
 *
 * Total number of different naval ship types in the fleet system.
 * Used for naval arrays and fleet composition.
 *
 * Value: 4 ship types
 * Usage: Naval arrays, fleet composition, ship classification
 * Notes: Core parameter affecting naval complexity and fleet diversity
 */
#define NSHP_NUMBER	4

/*
 * ZOOM_NUMBER - Number of zoom levels
 *
 * Total number of different map zoom levels available.
 * Used for display systems and map scaling.
 *
 * Value: 3 zoom levels
 * Usage: Display systems, map scaling, zoom arrays
 * Notes: Affects map display flexibility and detail levels
 */
#define ZOOM_NUMBER	3

/*
 * HXPOS_NUMBER - Number of hexagon position types
 *
 * Total number of different position types in hexagonal map system.
 * Used for hexagonal map calculations and positioning.
 *
 * Value: 4 position types
 * Usage: Hexagonal mapping, position arrays, spatial calculations
 * Notes: Core parameter for hexagonal map system geometry
 */
#define HXPOS_NUMBER	4

/*
 * SEASON_NUMBER - Number of seasons
 *
 * Total number of seasons in the game calendar system.
 * Used for seasonal effects and time-based calculations.
 *
 * Value: 4 seasons (spring, summer, autumn, winter)
 * Usage: Calendar systems, seasonal effects, time calculations
 * Notes: Standard four-season model for temporal game mechanics
 */
#define SEASON_NUMBER	4	/* how many seasons are there, anyway? */

/* ============================================================================
 * FUNCTION POINTER TYPES
 * ============================================================================
 * Purpose: Define function pointer types for callback and dynamic dispatch
 * Usage: Event handling, callback systems, dynamic function calls
 * Notes: Legacy function pointer definitions with ANSI C compatibility
 */

/*
 * FNCI - Integer-returning function pointer type
 *
 * Function pointer type for functions that return integer values.
 * Used for callback systems and dynamic function dispatch where
 * integer return values are expected.
 *
 * Return Type: int
 * Parameters: Unspecified (legacy K&R style)
 * Usage: Callback registration, event handlers, dynamic dispatch
 * Notes: Legacy definition without parameter specification
 * Modernization: Consider specific parameter types for type safety
 */
typedef int (*FNCI)();

/*
 * FNCV2 - Void function pointer with coordinate parameters
 *
 * Function pointer type for functions that accept two integer coordinates
 * and return void. Used for coordinate-based operations and map functions.
 * Includes ANSI C compatibility checking for parameter specification.
 *
 * Return Type: void
 * Parameters: int x, int y (coordinates) - when __STDC__ is defined
 * Parameters: Unspecified - for pre-ANSI C compatibility
 * Usage: Map operations, coordinate callbacks, position-based functions
 * Notes: Conditional parameter specification for compiler compatibility
 * Modernization: Always use ANSI C parameter specification
 */
#ifdef __STDC__
typedef void (*FNCV2)(int x, int y);    /* ANSI C: specific parameters */
#else /* FNCV */
typedef void (*FNCV2)();               /* K&R C: unspecified parameters */
#endif /* FNCV */

/* ============================================================================
 * CORE GAME DATA STRUCTURES
 * ============================================================================
 * Purpose: Define primary data structures for game entities and systems
 * Usage: Game state representation, entity storage, economic calculations
 * Notes: Central structures that define the game world model
 */

/*
 * struct s_sheet - Regional economic and demographic summary
 *
 * Comprehensive data structure representing economic and demographic
 * information for a regional area. Used for statistical analysis,
 * economic calculations, and administrative reporting. Aggregates
 * population, resources, military forces, and economic costs.
 *
 * Usage Patterns:
 *   - Economic analysis and resource planning
 *   - Regional summaries and statistical reports
 *   - Administrative interface calculations
 *   - Resource distribution and cost analysis
 *
 * Relationships:
 *   - Aggregates data from multiple sectors within a region
 *   - Connected to nation-level economic calculations
 *   - Used by administrative programs for economic modeling
 *   - Provides input for resource distribution algorithms
 *
 * Fields:
 *   people - Total civilian population across all sectors in region
 *   mtrls[] - Raw material quantities by type (MTRLS_NUMBER elements)
 *   sectors - Number of sectors included in this regional summary
 *   type_people[] - Population breakdown by major sector designation
 *   type_talons[] - Economic value breakdown by major sector designation
 *   type_sectors[] - Count of sectors by major designation type
 *   army_men - Total military personnel stationed in region
 *   monst_troops - Number of monster/NPC troops present
 *   ship_holds - Total naval cargo capacity available in region
 *   caravan_wagons - Total caravan transport capacity in region
 *   army_cost - Economic cost to maintain military forces
 *   monst_jewels - Tribute required for monster/NPC units
 *   navy_cost - Economic cost to maintain naval forces
 *   cvn_cost - Economic cost to maintain caravan transport
 *
 * Memory Management:
 *   - Typically allocated as single instances for regional calculations
 *   - Arrays use fixed sizes based on game constants
 *   - No dynamic memory allocation within structure
 *
 * Thread Safety:
 *   - Not thread-safe without external synchronization
 *   - Economic calculations require atomic updates
 *   - Statistical aggregation needs coordination
 */
typedef struct s_sheet {
    long people;                        /* Total population across region */
    itemtype mtrls[MTRLS_NUMBER];      /* Raw materials by type (food, iron, etc.) */
    short sectors;                      /* Number of sectors in this regional summary */
    long type_people[MAJ_NUMBER];      /* Population by major designation (city, farm, etc.) */
    itemtype type_talons[MAJ_NUMBER];  /* Economic value by major designation */
    int type_sectors[MAJ_NUMBER];      /* Sector count by major designation type */
    long army_men;                     /* Military personnel count in region */
    int monst_troops;                  /* Monster/NPC troop count */
    int ship_holds;                    /* Total naval cargo capacity (holds) */
    int caravan_wagons;                /* Total caravan transport capacity (wagons) */
    itemtype army_cost;                /* Economic cost for military maintenance */
    itemtype monst_jewels;             /* Tribute cost for monster/NPC units */
    itemtype navy_cost;                /* Economic cost for naval maintenance */
    itemtype cvn_cost;                 /* Economic cost for caravan maintenance */
} SHEET_STRUCT, *SHEET_PTR;

/*
 * struct s_sector - Individual map sector data structure
 *
 * Fundamental data structure representing a single map sector (hex or square)
 * in the game world. Contains all information about terrain, ownership,
 * development, population, and resources for one map location. This is the
 * basic building block of the game world map.
 *
 * Usage Patterns:
 *   - Game world map representation and storage
 *   - Economic production calculations per sector
 *   - Territorial ownership and control tracking
 *   - Resource extraction and trade good generation
 *   - Population growth and demographic modeling
 *
 * Relationships:
 *   - Owned by nations (referenced by owner field)
 *   - Contains civilians who contribute to economy
 *   - Produces resources based on designation and efficiency
 *   - Aggregated into regional summaries (SHEET_STRUCT)
 *   - Connected to combat and movement systems
 *
 * Fields:
 *   designation - Sector type/purpose (city, farm, mine, etc.)
 *   altitude - Terrain elevation category affecting movement and combat
 *   vegetation - Climate/vegetation type affecting production
 *   owner - Nation ID that controls this sector (UNOWNED if neutral)
 *   efficiency - Development level affecting productivity (0-100%)
 *   people - Civilian population living in this sector
 *   minerals - Geological wealth value for mining operations
 *   tradegood - Exotic trade goods available for economic exploitation
 *   region - [Commented out] Regional grouping index for administration
 *
 * Memory Management:
 *   - Typically allocated as large 2D arrays for entire world map
 *   - Fixed-size structure for efficient array access
 *   - No internal dynamic allocation
 *
 * Thread Safety:
 *   - Not thread-safe without external synchronization
 *   - Concurrent updates during turns require coordination
 *   - Population and efficiency changes need atomic operations
 *
 * Performance Notes:
 *   - Hot path structure accessed frequently during game updates
 *   - Compact layout important for cache performance
 *   - Array indexing used for fast map coordinate access
 */
typedef struct s_sector {
    uns_short designation;      /* Sector type/purpose (city, farm, mine, fortress, etc.) */
    uns_char altitude;          /* Elevation category (affects movement, combat, visibility) */
    uns_char vegetation;        /* Climate/vegetation type (affects production, movement) */
    ntntype owner;              /* Owning nation ID (UNOWNED if neutral territory) */
    uns_char efficiency;        /* Development level 0-100% (affects productivity) */
    long people;                /* Civilian population (contributes to economy, recruitment) */
    uns_char minerals;          /* Geological wealth 0-255 (mining potential) */
    uns_char tradegood;         /* Exotic trade goods 0-255 (special economic value) */
    /* uns_char region;          [Unused] Regional grouping index for administration */
} SCT_STRUCT, *SCT_PTR;

/*
 * struct s_army - Military unit data structure
 *
 * Represents a single military unit (army) in the game, containing all
 * information needed for combat, movement, command hierarchy, and resource
 * management. Army units are the primary military forces of nations and
 * can move across the map, engage in combat, and carry out strategic operations.
 *
 * Usage Patterns:
 *   - Military combat and warfare operations
 *   - Strategic movement and positioning on game map
 *   - Resource consumption (supply, provisions) calculations
 *   - Command hierarchy and leadership chain management
 *   - Magic system integration through leader spell capabilities
 *
 * Relationships:
 *   - Owned by nations (managed through nation army lists)
 *   - Located on map sectors (SCT_STRUCT coordinates)
 *   - May be led by other units (recursive leadership hierarchy)
 *   - Can be transported by naval units (NAVY_STRUCT integration)
 *   - Consumes materials from nation stockpiles for supply
 *
 * Fields:
 *   armyid - Unique identifier for this army unit across entire game
 *   unittype - Military unit classification (infantry, cavalry, etc.)
 *   max_eff - Maximum possible efficiency (100% when at full strength)
 *   efficiency - Current operational efficiency (0-100%, affects performance)
 *   xloc, yloc - Current map coordinates where unit is positioned
 *   targx, targy - Target destination coordinates for movement orders
 *   lastx, lasty - Previous location (for movement tracking and retreat)
 *   strength - Number of individual soldiers/troops in this unit
 *   umove - Movement points available per turn (terrain dependent)
 *   status - Operational status flags (combat readiness, orders, morale)
 *   spellpts - Magical power points if unit has spellcasting leaders
 *   supply - Food supplies per soldier (affects movement and combat)
 *   leader - Unit ID of commanding officer/leader (0 if no leader)
 *   next - Linked list pointer to next army in nation's army roster
 *   nrby - Linked list pointer to other armies in same map sector
 *
 * Memory Management:
 *   - Dynamically allocated as linked lists per nation
 *   - Nation structures maintain head pointers to army lists
 *   - Next/nrby pointers require careful maintenance during operations
 *   - Cleanup needed when units are destroyed or disbanded
 *
 * Thread Safety:
 *   - Not thread-safe without external synchronization
 *   - Combat operations require atomic updates to prevent conflicts
 *   - Movement updates need coordination to prevent invalid states
 *   - Status changes during simultaneous operations need protection
 *
 * Performance Notes:
 *   - Linked list traversal for operations affecting multiple armies
 *   - Location-based operations optimized through nrby list grouping
 *   - Frequent access during combat and movement phases
 *   - Supply calculations performed each turn for all active units
 */
typedef struct s_army {
    idtype armyid;          /* Unique army identifier across entire game world */
    uns_char unittype;      /* Military unit classification (infantry, cavalry, siege, etc.) */
    uns_char max_eff;       /* Maximum efficiency percentage (100% at full strength) */
    uns_char efficiency;    /* Current operational efficiency (0-100%, affects combat/movement) */
    maptype xloc, yloc;     /* Current map coordinates of army position */
    maptype targx, targy;   /* Target destination coordinates for movement orders */
    maptype lastx, lasty;   /* Previous location coordinates (retreat, movement tracking) */
    long strength;          /* Number of individual soldiers/troops in unit */
    uns_char umove;         /* Movement points per turn (terrain and efficiency dependent) */
    long status;            /* Operational status flags (combat, orders, morale, special states) */
    uns_char spellpts;      /* Magical power points available for spellcasting operations */
    uns_char supply;        /* Food supply per soldier (days of provisions available) */
    idtype leader;          /* Unit ID of commanding leader (0 if independent command) */
    struct s_army *next;    /* Linked list: next army in nation's military roster */
    struct s_army *nrby;    /* Linked list: other armies in same map sector (spatial grouping) */
} ARMY_STRUCT, *ARMY_PTR;

/*
 * struct s_navy - Naval fleet data structure
 *
 * Represents a naval fleet consisting of multiple ship types, capable of
 * movement on water sectors, combat operations, and transportation of armies,
 * caravans, and materials. Naval units provide strategic mobility and control
 * of sea routes, harbors, and coastal territories.
 *
 * Usage Patterns:
 *   - Naval combat and coastal warfare operations
 *   - Transportation of armies and caravans across water
 *   - Materials transport and trade route protection
 *   - Harbor control and blockade operations
 *   - Coastal raiding and amphibious assault support
 *
 * Relationships:
 *   - Owned by nations (managed through nation navy lists)
 *   - Located on water/coastal map sectors
 *   - Can transport army units (ARMY_STRUCT) across water
 *   - Can transport caravan units (CVN_STRUCT) for trade
 *   - Carries materials for supply and trade operations
 *
 * Fields:
 *   navyid - Unique identifier for this naval fleet across entire game
 *   ships[] - Array of ship counts by type (warships, merchants, galleys, etc.)
 *   efficiency[] - Operational efficiency per ship type (0-100% per type)
 *   xloc, yloc - Current map coordinates where fleet is positioned
 *   targx, targy - Target destination coordinates for movement orders
 *   lastx, lasty - Previous location (for movement tracking and retreat)
 *   umove - Movement points available per turn (weather and damage dependent)
 *   status - Operational status flags (combat readiness, orders, condition)
 *   crew - Percentage of optimal crew manning the fleet (0-100%)
 *   people - Civilian passengers carried per galley hold capacity
 *   supply - Food supplies per crew member (affects operational range)
 *   armynum - Army unit ID being transported (0 if no army aboard)
 *   cvnnum - Caravan unit ID being transported (0 if no caravan aboard)
 *   mtrls[] - Materials cargo carried by fleet (trade goods, supplies)
 *   next - Linked list pointer to next navy in nation's naval roster
 *
 * Memory Management:
 *   - Dynamically allocated as linked lists per nation
 *   - Nation structures maintain head pointers to navy lists
 *   - Ship type arrays use fixed sizes based on NSHP_NUMBER constant
 *   - Materials arrays use fixed sizes based on MTRLS_NUMBER constant
 *
 * Thread Safety:
 *   - Not thread-safe without external synchronization
 *   - Naval combat requires atomic updates to prevent state corruption
 *   - Transportation operations need coordination with army/caravan units
 *   - Material transfers require synchronized access with nation stockpiles
 *
 * Performance Notes:
 *   - Multiple ship type arrays require iteration for fleet calculations
 *   - Cargo capacity calculations involve summing across ship types
 *   - Combat effectiveness depends on combined ship type strengths
 *   - Movement calculations consider worst-case ship type limitations
 */
typedef struct s_navy {
    idtype navyid;                      /* Unique naval fleet identifier across game world */
    uns_short ships[NSHP_NUMBER];       /* Ship counts by type (warships, merchants, galleys, etc.) */
    uns_char efficiency[NSHP_NUMBER];   /* Operational efficiency per ship type (0-100% each) */
    maptype xloc, yloc;                 /* Current map coordinates of fleet position */
    maptype targx, targy;               /* Target destination coordinates for movement orders */
    maptype lastx, lasty;               /* Previous location coordinates (retreat, movement tracking) */
    uns_char umove;                     /* Movement points per turn (weather, damage, and crew dependent) */
    long status;                        /* Operational status flags (combat, orders, condition, special states) */
    uns_char crew;                      /* Crew manning percentage (0-100% of optimal crew levels) */
    uns_char people;                    /* Civilian passengers carried per galley hold capacity */
    uns_char supply;                    /* Food supplies per crew member (days of provisions available) */
    idtype armynum;                     /* Army unit ID currently being transported (0 if empty) */
    idtype cvnnum;                      /* Caravan unit ID currently being transported (0 if empty) */
    itemtype mtrls[MTRLS_NUMBER];       /* Materials cargo by type (trade goods, supplies, equipment) */
    struct s_navy *next;                /* Linked list: next navy in nation's naval roster */
} NAVY_STRUCT, *NAVY_PTR;

/*
 * struct s_caravan - Trade caravan data structure
 *
 * Represents a trading caravan consisting of wagons, crew, and trade goods
 * that can move across land to transport materials and people between sectors.
 * Caravans are essential for overland commerce, resource distribution, and
 * economic development in territories without naval access.
 *
 * Usage Patterns:
 *   - Overland trade and commerce operations
 *   - Material transport between cities and sectors
 *   - Economic development of inland territories
 *   - Civilian population transport and migration
 *   - Resource distribution for national infrastructure
 *
 * Relationships:
 *   - Owned by nations (managed through nation caravan lists)
 *   - Moves across land sectors following trade routes
 *   - Can be transported by naval units for overseas trade
 *   - Connects cities and economic centers for resource flow
 *   - Supports national economy through material distribution
 *
 * Fields:
 *   cvnid - Unique identifier for this caravan across entire game
 *   efficiency - Operational efficiency (0-100%, affects capacity and speed)
 *   xloc, yloc - Current map coordinates where caravan is positioned
 *   targx, targy - Target destination coordinates for movement orders
 *   lastx, lasty - Previous location (for movement tracking and retreat)
 *   size - Number of wagons in caravan (determines total capacity)
 *   umove - Movement points available per turn (terrain and load dependent)
 *   status - Operational status flags (loaded, moving, trading, damaged)
 *   crew - Percentage of optimal crew manning the caravan (0-100%)
 *   people - Civilian passengers carried per wagon capacity
 *   supply - Food supplies per crew member (affects operational range)
 *   mtrls[] - Materials cargo carried by caravan (trade goods, resources)
 *   next - Linked list pointer to next caravan in nation's roster
 *
 * Memory Management:
 *   - Dynamically allocated as linked lists per nation
 *   - Nation structures maintain head pointers to caravan lists
 *   - Materials arrays use fixed sizes based on MTRLS_NUMBER constant
 *   - Cleanup needed when caravans are destroyed or disbanded
 *
 * Thread Safety:
 *   - Not thread-safe without external synchronization
 *   - Trade operations require atomic updates to prevent duplication
 *   - Movement updates need coordination to prevent invalid states
 *   - Material transfers require synchronized access with nation stockpiles
 *
 * Performance Notes:
 *   - Cargo capacity calculations based on wagon count and efficiency
 *   - Movement speed affected by total load and terrain conditions
 *   - Trade route optimization considers multiple caravan coordination
 *   - Economic calculations aggregate all caravan contributions
 */
typedef struct s_caravan {
    idtype cvnid;                   /* Unique caravan identifier across game world */
    uns_char efficiency;            /* Operational efficiency (0-100%, affects capacity and speed) */
    maptype xloc, yloc;             /* Current map coordinates of caravan position */
    maptype targx, targy;           /* Target destination coordinates for movement orders */
    maptype lastx, lasty;           /* Previous location coordinates (retreat, movement tracking) */
    uns_char size;                  /* Number of wagons in caravan (determines total capacity) */
    uns_char umove;                 /* Movement points per turn (terrain, load, and crew dependent) */
    long status;                    /* Operational status flags (loaded, moving, trading, damaged) */
    uns_char crew;                  /* Crew manning percentage (0-100% of optimal crew levels) */
    uns_char people;                /* Civilian passengers carried per wagon capacity */
    uns_char supply;                /* Food supplies per crew member (days of provisions available) */
    itemtype mtrls[MTRLS_NUMBER];   /* Materials cargo by type (trade goods, resources, equipment) */
    struct s_caravan *next;         /* Linked list: next caravan in nation's trade roster */
} CVN_STRUCT, *CVN_PTR;

/*
 * struct s_city - Urban settlement data structure
 *
 * Represents a major urban settlement or city within a nation, containing
 * population, resources, economic distribution systems, and defensive
 * fortifications. Cities serve as economic centers, military command posts,
 * and resource distribution hubs for surrounding territories.
 *
 * Usage Patterns:
 *   - Economic resource distribution and trade management
 *   - Population centers for recruitment and civilian growth
 *   - Military command and control coordination
 *   - Fortified defensive positions during warfare
 *   - Resource storage and processing facilities
 *
 * Relationships:
 *   - Located on specific map sectors (usually developed urban areas)
 *   - Owned by nations (managed through nation city lists)
 *   - Connected to economic production and resource flows
 *   - Serves as command centers for naval and caravan operations
 *   - Integrates with national material distribution networks
 *
 * Fields:
 *   xloc, yloc - Map coordinates where city is established
 *   name - Human-readable city name for identification and display
 *   cityid - Unique internal identifier for this city across game
 *   i_people - Initial civilian population when city was founded
 *   efficiency - Urban development efficiency (0-100%, affects productivity)
 *   weight - Economic distribution priority weight for resource allocation
 *   c_mtrls[] - Current materials stockpiled in city warehouses
 *   i_mtrls[] - Distribution materials for automatic resource management
 *   s_talons - Starting economic wealth (talons) available in city treasury
 *   m_mtrls[] - Minimum materials to maintain in city reserves
 *   auto_flags[] - Automatic distribution settings per material type
 *   cmd_flag - Command and control flags for naval/caravan coordination
 *   fortress - Defensive fortification level (affects combat protection)
 *   next - Linked list pointer to next city in nation's urban roster
 *
 * Memory Management:
 *   - Dynamically allocated as linked lists per nation
 *   - Nation structures maintain head pointers to city lists
 *   - Material arrays use fixed sizes based on MTRLS_NUMBER constant
 *   - Name strings require null termination and length validation
 *
 * Thread Safety:
 *   - Not thread-safe without external synchronization
 *   - Resource distribution requires atomic updates across multiple arrays
 *   - Economic calculations need coordination to prevent inconsistencies
 *   - Command operations require synchronized access to prevent conflicts
 *
 * Performance Notes:
 *   - Resource distribution calculations iterate across material arrays
 *   - Economic weight affects priority in national resource allocation
 *   - Fortress calculations impact combat performance for defending forces
 *   - Command coordination affects efficiency of naval/caravan operations
 */
typedef struct s_city {
    maptype xloc, yloc;                 /* Map coordinates where city is established */
    char name[NAMELTH+1];               /* Human-readable city name (null-terminated string) */
    uns_short cityid;                   /* Unique internal identifier across entire game */
    short i_people;                     /* Initial civilian population at city founding */
    uns_char efficiency;                /* Urban development efficiency (0-100%, affects productivity) */
    uns_char weight;                    /* Economic distribution priority weight for resource allocation */
    itemtype c_mtrls[MTRLS_NUMBER];     /* Current materials stockpiled in city warehouses */
    itemtype i_mtrls[MTRLS_NUMBER];     /* Distribution materials for automatic resource management */
    itemtype s_talons;                  /* Starting economic wealth (talons) in city treasury */
    itemtype m_mtrls[MTRLS_NUMBER];     /* Minimum materials to maintain in city reserves */
    long auto_flags[MTRLS_NUMBER];      /* Automatic distribution settings per material type */
    long cmd_flag;                      /* Command/control flags for naval and caravan coordination */
    uns_char fortress;                  /* Defensive fortification level (affects combat protection) */
    struct s_city *next;                /* Linked list: next city in nation's urban roster */
} CITY_STRUCT, *CITY_PTR;

/*
 * struct s_item - Construction and trade item data structure
 *
 * Represents items that can be constructed, purchased, or traded within the
 * game world. Items include military units, infrastructure projects, trade
 * goods, and special constructions that require materials and workforce to
 * complete. They serve as the foundation for economic development and military
 * expansion.
 *
 * Usage Patterns:
 *   - Construction projects for cities and infrastructure
 *   - Military unit recruitment and equipment manufacturing
 *   - Trade goods production and commercial transactions
 *   - Resource transformation and value-added production
 *   - Special projects requiring coordinated resource investment
 *
 * Relationships:
 *   - Located on specific map sectors for construction/production
 *   - Owned by nations (managed through nation item lists)
 *   - Connected to armies, navies, or caravans for unit construction
 *   - Requires materials from national stockpiles for completion
 *   - Affects sector development and economic productivity
 *
 * Fields:
 *   xloc, yloc - Map coordinates where item construction/production occurs
 *   itemid - Unique identifier for this item across entire game
 *   type - Item classification (construction, military, trade, infrastructure)
 *   iteminfo - Detailed information about item specifications and progress
 *   menforjob - Workforce required to complete item construction/production
 *   armyid - Army unit ID if item is military unit construction (0 if N/A)
 *   navyid - Navy unit ID if item is naval unit construction (0 if N/A)
 *   cvnid - Caravan unit ID if item is trade unit construction (0 if N/A)
 *   mtrls[] - Raw materials required for item completion
 *   next - Linked list pointer to next item in nation's production queue
 *
 * Memory Management:
 *   - Dynamically allocated as linked lists per nation
 *   - Nation structures maintain head pointers to item lists
 *   - Material arrays use fixed sizes based on MTRLS_NUMBER constant
 *   - Cleanup needed when items are completed or cancelled
 *
 * Thread Safety:
 *   - Not thread-safe without external synchronization
 *   - Construction progress requires atomic updates to prevent corruption
 *   - Material consumption needs coordination with nation stockpiles
 *   - Unit creation requires synchronized access to military rosters
 *
 * Performance Notes:
 *   - Production calculations iterate across material requirements
 *   - Workforce allocation affects completion time and efficiency
 *   - Economic impact calculations consider resource investment costs
 *   - Unit construction affects national military and economic capacity
 */
typedef struct s_item {
    maptype xloc, yloc;             /* Map coordinates where item construction/production occurs */
    idtype itemid;                  /* Unique identifier for this item across entire game */
    unsigned char type;             /* Item classification (construction, military, trade, infrastructure) */
    long iteminfo;                  /* Detailed specifications, progress, and configuration data */
    long menforjob;                 /* Workforce required to complete item construction/production */
    idtype armyid;                  /* Army unit ID for military construction (0 if not applicable) */
    idtype navyid;                  /* Navy unit ID for naval construction (0 if not applicable) */
    idtype cvnid;                   /* Caravan unit ID for trade construction (0 if not applicable) */
    itemtype mtrls[MTRLS_NUMBER];   /* Raw materials required for item completion */
    struct s_item *next;            /* Linked list: next item in nation's production queue */
} ITEM_STRUCT, *ITEM_PTR;

/*
 * struct s_unitnum - Automatic unit numbering system
 *
 * Provides automatic numbering schemes for various game entities to ensure
 * unique identifiers and organized unit designation. This system maintains
 * consistent numbering across different unit types, classes, and categories
 * to support efficient identification and management of game objects.
 *
 * Usage Patterns:
 *   - Automatic ID assignment for newly created units
 *   - Consistent numbering schemes across unit types
 *   - Organizational numbering for military formations
 *   - Sequential identifier generation for various game entities
 *   - Customizable numbering systems per nation or unit class
 *
 * Relationships:
 *   - Used by nations for unit creation and identification
 *   - Applies to armies, navies, caravans, cities, and other entities
 *   - Supports multiple numbering schemes per nation
 *   - Integrates with unit creation and management systems
 *
 * Fields:
 *   number - Current number in sequence (next number to assign)
 *   ident - Identifier of the unit class, type, or category being numbered
 *   type - Classification of the numbering scheme (unit type, purpose)
 *   next - Linked list pointer to next numbering scheme in chain
 *
 * Memory Management:
 *   - Dynamically allocated as linked lists per nation
 *   - Small structures with minimal memory overhead
 *   - Cleanup needed when numbering schemes are removed
 *
 * Thread Safety:
 *   - Not thread-safe without external synchronization
 *   - Number assignment requires atomic increment operations
 *   - Scheme modification needs coordination to prevent conflicts
 *
 * Performance Notes:
 *   - Fast sequential number generation for unit creation
 *   - Minimal memory footprint for numbering state
 *   - Efficient linked list traversal for scheme lookup
 */
typedef struct s_unitnum {
    int number;                 /* Current number in sequence (next number to assign) */
    int ident;                  /* Identifier of unit class, type, or category being numbered */
    int type;                   /* Classification of numbering scheme (unit type, purpose) */
    struct s_unitnum *next;     /* Linked list: next numbering scheme in chain */
} UNITNUM, *UNUM_PTR;

/*
 * struct s_map - Map visibility and reconnaissance data structure
 *
 * Represents areas of the map that have been explored, scouted, or are under
 * surveillance by a nation. This structure tracks visibility ranges, reconnaissance
 * strength, and the persistence of map knowledge for strategic planning and
 * situational awareness during gameplay.
 *
 * Usage Patterns:
 *   - Track explored territories and reconnaissance information
 *   - Manage fog of war and visibility systems
 *   - Store intelligence gathering and surveillance data
 *   - Support strategic planning with known map information
 *   - Implement scouting and exploration mechanics
 *
 * Relationships:
 *   - Owned by nations (managed through nation map lists)
 *   - References specific map coordinates for visibility
 *   - Integrates with movement and exploration systems
 *   - Supports combat intelligence and strategic awareness
 *   - Connected to scouting units and reconnaissance operations
 *
 * Fields:
 *   xloc, yloc - Map coordinates of the area under surveillance/known
 *   range - Distance visible from this location (visibility radius)
 *   strength - Intensity of visibility/reconnaissance (clarity, accuracy)
 *   lifetime - Duration of visibility in turns (0 = permanent knowledge)
 *   next - Linked list pointer to next map knowledge entry
 *
 * Memory Management:
 *   - Dynamically allocated as linked lists per nation
 *   - Nation structures maintain head pointers to map lists
 *   - Temporary entries cleaned up when lifetime expires
 *   - Permanent entries persist throughout game
 *
 * Thread Safety:
 *   - Not thread-safe without external synchronization
 *   - Visibility updates require atomic operations during exploration
 *   - Lifetime decrements need coordination during turn processing
 *   - Map knowledge queries need consistent read access
 *
 * Performance Notes:
 *   - Frequent access during movement and combat operations
 *   - Range calculations for visibility determination
 *   - Lifetime management requires periodic cleanup operations
 *   - Strategic AI relies heavily on map knowledge for decisions
 */
typedef struct s_map {
    maptype xloc, yloc;     /* Map coordinates of the area under surveillance/known */
    uns_char range;         /* Distance visible from this location (visibility radius) */
    uns_char strength;      /* Intensity of visibility/reconnaissance (clarity, accuracy) */
    uns_char lifetime;      /* Duration of visibility in turns (0 = permanent knowledge) */
    struct s_map *next;     /* Linked list: next map knowledge entry in nation's intel */
} MAP_STRUCT, *MAP_PTR;

/*
 * struct s_nation - Complete nation data structure
 *
 * Represents a complete player nation in the game, containing all information
 * about leadership, territory, military forces, economy, diplomacy, and
 * strategic attributes. This is the central data structure that defines each
 * player's civilization and tracks all aspects of their empire.
 *
 * Usage Patterns:
 *   - Complete player nation representation and state management
 *   - Economic calculations and resource management
 *   - Military command and unit coordination
 *   - Diplomatic relations and international politics
 *   - Strategic AI decision-making and planning
 *
 * Relationships:
 *   - Contains linked lists of all national assets (armies, navies, cities)
 *   - References map sectors through ownership and territorial control
 *   - Maintains diplomatic relations with other nations
 *   - Integrates with game world for resource production and consumption
 *   - Connected to user authentication and session management
 *
 * Fields:
 *   name - Nation name displayed to other players
 *   login - User login name for authentication
 *   passwd - Encrypted password for secure access
 *   leader - Ruler/leader title for roleplay and identification
 *   repro - Population reproduction rate modifier
 *   race - National race affecting characteristics and abilities
 *   mark - Unique single-character identifier for map display
 *   location - Initial placement quality score on world creation
 *   capx, capy - Capital city coordinates (center of government)
 *   centerx, centery - Geographic center of national territory
 *   leftedge, rightedge, topedge, bottomedge - Territorial boundaries
 *   class - National classification affecting abilities and strategies
 *   aplus, dplus - Combat bonuses for all national military units
 *   score - Overall nation score for ranking and victory conditions
 *   active - Activity status and AI strategy type
 *   maxmove - Maximum movement points for national units
 *   num_* - Entity counts for save/load serialization
 *   *_list - Linked list heads for all national assets
 *   dstatus[] - Diplomatic relations with all other nations
 *   mtrls[] - Total national material stockpiles
 *   m_new[] - Materials produced this turn
 *   t* - Total counts of various national assets and population
 *   powers[] - Magical abilities and special national powers
 *   attribute[] - National characteristics affecting various game mechanics
 *   mbox_size, news_size - Communication system state tracking
 *
 * Memory Management:
 *   - Central structure allocated once per nation
 *   - Contains head pointers to multiple linked lists
 *   - Arrays use fixed sizes based on game constants
 *   - String fields require null termination and validation
 *
 * Thread Safety:
 *   - Not thread-safe without external synchronization
 *   - Economic calculations require atomic updates across multiple fields
 *   - Military operations need coordination to prevent state corruption
 *   - Diplomatic changes require synchronized access to prevent conflicts
 *
 * Performance Notes:
 *   - Central structure accessed frequently during all game operations
 *   - Economic calculations iterate across material arrays
 *   - Military operations traverse linked lists for unit management
 *   - AI decision-making relies heavily on statistical totals
 */
typedef struct s_nation {
    char name[NAMELTH+1];               /* Nation name displayed to other players */
    char login[NAMELTH+1];              /* User login name for authentication */
    char passwd[PASSLTH+1];             /* Encrypted password for secure access */
    char leader[LEADERLTH+1];           /* Ruler/leader title for roleplay identification */
    short repro;                        /* Population reproduction rate modifier */
    short race;                         /* National race affecting characteristics and abilities */
    char mark;                          /* Unique single-character identifier for map display */
    char location;                      /* Initial placement quality score on world creation */
    maptype capx;                       /* Capital city X coordinate (center of government) */
    maptype capy;                       /* Capital city Y coordinate (center of government) */
    maptype centerx;                    /* Geographic center X of national territory */
    maptype centery;                    /* Geographic center Y of national territory */
    short leftedge;                     /* Western boundary of national territory */
    short rightedge;                    /* Eastern boundary of national territory */
    short topedge;                      /* Northern boundary of national territory */
    short bottomedge;                   /* Southern boundary of national territory */
    short class;                        /* National classification affecting abilities/strategies */
    short aplus;                        /* Combat attack bonus for all national military units */
    short dplus;                        /* Combat defense bonus for all national military units */
    long score;                         /* Overall nation score for ranking and victory conditions */
    short active;                       /* Activity status and AI strategy type */
    short maxmove;                      /* Maximum movement points for national units */
    idtype num_army;                    /* Number of armies (for save/load serialization) */
    idtype num_navy;                    /* Number of navies (for save/load serialization) */
    idtype num_city;                    /* Number of cities (for save/load serialization) */
    idtype num_item;                    /* Number of items (for save/load serialization) */
    idtype num_cvn;                     /* Number of caravans (for save/load serialization) */
    idtype num_unum;                    /* Number of autonumbering schemes (for serialization) */
    idtype num_maps;                    /* Number of map entries (for save/load serialization) */
    ARMY_PTR army_list;                 /* Linked list head: all army units of the nation */
    NAVY_PTR navy_list;                 /* Linked list head: all navy units of the nation */
    CITY_PTR city_list;                 /* Linked list head: all city information */
    ITEM_PTR item_list;                 /* Linked list head: all national commodities/projects */
    CVN_PTR cvn_list;                   /* Linked list head: all trading caravans */
    UNUM_PTR unum_list;                 /* Linked list head: all autonumbering schemes */
    MAP_PTR map_list;                   /* Linked list head: mapping/reconnaissance of sectors */
    char dstatus[ABSMAXNTN];            /* Diplomatic status with all other nations */
    itemtype mtrls[MTRLS_NUMBER];       /* Total national material stockpiles by type */
    itemtype m_new[MTRLS_NUMBER];       /* Materials produced this turn by type */
    short tsctrs;                       /* Total number of sectors owned by nation */
    short tunsctrs;                     /* Total number of unsupported sectors */
    short tships;                       /* Total number of ships across all navies */
    short twagons;                      /* Total number of wagons across all caravans */
    long tmonst;                        /* Total number of monster/NPC units */
    long tleaders;                      /* Total number of military leaders */
    long tmil;                          /* Total military population across all forces */
    long tciv;                          /* Total civilian population across all sectors */
    long powers[MAG_NUMBER];            /* Magical abilities and special national powers */
    short attribute[BUTE_NUMBER];       /* National characteristics affecting game mechanics */
    off_t mbox_size;                    /* Last known size of nation's mail file */
    off_t news_size;                    /* Last known size of nation's news file */
} NTN_STRUCT, *NTN_PTR;

/*
 * struct s_world - Global game world configuration and state
 *
 * Contains all global game parameters, world generation settings, combat rules,
 * economic factors, and administrative configuration for the entire game world.
 * This structure defines the fundamental rules and current state of the game
 * universe, affecting all players and game mechanics.
 *
 * Usage Patterns:
 *   - Global game configuration and rule enforcement
 *   - World generation parameters and terrain distribution
 *   - Combat system configuration and damage calculations
 *   - Economic and production rate management
 *   - Administrative functions and game master controls
 *
 * Relationships:
 *   - Contains array of all nations in the game world
 *   - Defines rules affecting all players equally
 *   - Controls random event generation and world dynamics
 *   - Manages global resource pools and mercenary availability
 *   - Integrates with all game systems for consistent rule application
 *
 * Key Sections:
 *   World Dimensions: mapx, mapy define the game world size
 *   Nation Management: nations, npcs, active_ntns, np[] array
 *   Time Management: turn, start_turn for game progression
 *   Administrative: demigod controls, passwords, security flags
 *   Combat System: dice, damage, combat bonuses configuration
 *   World Generation: terrain percentages, smoothing algorithms
 *   Economic Factors: production rates, trade good distribution
 *   Random Events: event frequencies, NPC generation rates
 *   Global Statistics: world totals for population, military, resources
 *
 * Memory Management:
 *   - Single global instance allocated at game startup
 *   - Contains large nation pointer array
 *   - Material arrays use fixed sizes based on constants
 *   - String fields require null termination and validation
 *
 * Thread Safety:
 *   - Global structure requiring synchronized access
 *   - Turn progression requires atomic updates
 *   - Statistical totals need coordination during calculations
 *   - Administrative changes require exclusive access
 *
 * Performance Notes:
 *   - Central structure accessed by all game operations
 *   - Nation array requires efficient indexing and bounds checking
 *   - Global statistics recalculated frequently during gameplay
 *   - Configuration parameters affect computational complexity
 */
struct s_world
{
    maptype mapx, mapy;                 /* World dimensions (width and height in sectors) */
    ntntype nations;                    /* Total number of nations configured in world */
    ntntype npcs;                       /* Number of NPC nations at world creation */
    ntntype active_ntns;                /* Number of currently active player nations */
    NTN_PTR np[ABSMAXNTN];             /* Array of pointers to all nations in world */
    short turn;                         /* Current game turn counter */
    short start_turn;                   /* Turn number when campaign began (offset) */
    char demigod[NAMELTH+1];           /* Name of the game master/administrator */
    char passwd[PASSLTH+1];            /* Administrator password for god-level access */
    char addpwd[PASSLTH+1];            /* Password required to add new nations */
    char demibuild;                     /* Flag: allows demigod to rebuild world */
    char demilimit;                     /* Flag: limits demigod powers for fairness */
    char relative_map;                  /* Flag: enables relative map confusion */
    char hexmap;                        /* Flag: indicates hexagonal map mode */
    char verify_login;                  /* Flag: increases login security requirements */
    char hide_scores;                   /* Flag: hides nation scores from players */
    char hide_login;                    /* Flag: hides nation owner names */
    char builddist;                     /* Minimum distance between new nations */
    short fleet_cbval;                  /* Base combat bonus for naval fleets */
    short wagon_cbval;                  /* Base combat bonus for caravan units */
    char num_dice;                      /* Number of dice rolled for combat resolution */
    char avg_damage;                    /* Average damage percentage during combat */
    char damage_limit;                  /* Damage threshold to stop attack progression */
    char overmatch_adj;                 /* Damage adjustment for relative unit sizes */
    char pmindamage;                    /* Minimum damage percentage of dice roll */
    char smoothings;                    /* Terrain smoothing algorithm iterations */
    char land_range;                    /* Water displacement check radius */
    char preject_land;                  /* Percentage chance to reject land placement */
    char pwater;                        /* Percentage of world that is water */
    char pmount;                        /* Percentage of land that is mountainous */
    char pmercval;                      /* Percentage of disbanded units joining mercenaries */
    char prevolt;                       /* Percentage yearly revolt probability */
    char pscout;                        /* Percentage chance to capture enemy scouts */
    char pmonster;                      /* Percentage yearly monster reproduction rate */
    char pmercmonst;                    /* Percentage of monsters joining mercenaries */
    char ptrade;                        /* Percentage of sectors with trade goods */
    char ptgmetal;                      /* Metal trade goods percentage distribution */
    char ptgjewel;                      /* Jewel trade goods percentage distribution */
    char ptgspell;                      /* Magic trade goods percentage distribution */
    char supplylimit;                   /* Maximum supply capacity per unit */
    char nvsplydist;                    /* Supply distribution distance for navies/caravans */
    char cityxfdist;                    /* Maximum distance for city resource transfers */
    uns_char exposelvl;                 /* Combat exposure level (10 = normal) */
    uns_char maxpts;                    /* Maximum build points per construction */
    uns_char lizards;                   /* Number of lizard NPCs created at start */
    uns_char savages;                   /* Number of savage NPCs created at start */
    uns_char nomads;                    /* Number of nomad NPCs created at start */
    uns_char pirates;                   /* Number of pirate NPCs created at start */
    char numrandom;                     /* Number of random events per turn */
    char numweather;                    /* Number of weather events per turn */
    long m_mil;                         /* Global mercenary pool size */
    short m_aplus;                      /* Mercenary attack bonus modifier */
    short m_dplus;                      /* Mercenary defense bonus modifier */
    itemtype bribelevel;                /* Standard bribery value threshold */
    itemtype w_mtrls[MTRLS_NUMBER];     /* Global resource totals across all nations */
    long w_civ;                         /* Total world civilian population */
    long w_mil;                         /* Total world military population */
    long w_sctrs;                       /* Total owned sectors across all nations */
    long w_score;                       /* Combined score total of all nations */
    off_t mbox_size;                    /* Size of administrator's mailbox */
    off_t news_size;                    /* Size of global news file */
    int num_unum;                       /* Number of default unit numbering schemes */
    UNUM_PTR dflt_unum;                 /* Default world unit numbering system */
    uns_char maxdipadj;                 /* Maximum diplomatic adjustment per turn */
    uns_char latestart;                 /* Grace period turns before password enforcement */
    uns_char growthrate;                /* Global population growth rate modifier */
};

/*
 * struct s_display - User interface display configuration
 *
 * Defines the configuration for a specific display mode in the hexagonal
 * map interface, controlling how information is presented in different
 * quadrants of the screen and what visual highlighting is applied to
 * map elements and game entities.
 *
 * Usage Patterns:
 *   - User interface customization and display preferences
 *   - Multi-quadrant map display configuration
 *   - Visual highlighting and emphasis control
 *   - Display mode switching and management
 *   - Screen layout and information presentation
 *
 * Relationships:
 *   - Used by display system for map rendering
 *   - Supports hexagonal map interface customization
 *   - Integrates with highlighting and visual emphasis systems
 *   - Connected to user preference and configuration management
 *
 * Fields:
 *   name - Human-readable name for this display configuration
 *   focus - Quadrant number that receives primary user focus
 *   style[] - Display style selection for each map quadrant
 *   highlight[] - Highlighting option active in each quadrant
 *   target[] - Numeric target values for highlighting systems
 *
 * Memory Management:
 *   - Small structure with fixed-size arrays
 *   - Name string requires null termination
 *   - Arrays sized based on HXPOS_NUMBER constant
 *
 * Thread Safety:
 *   - Display configurations typically read-only during rendering
 *   - Modification requires coordination with display system
 *   - User preference changes need synchronized updates
 *
 * Performance Notes:
 *   - Accessed frequently during map rendering operations
 *   - Array indexing for quadrant-specific display settings
 *   - Highlighting calculations may impact rendering performance
 */
typedef struct s_display {
    char name[DISPLAYLTH + 1];      /* Human-readable name for this display configuration */
    int focus;                      /* Quadrant number that receives primary user focus */
    int style[HXPOS_NUMBER];        /* Display style selection for each map quadrant */
    int highlight[HXPOS_NUMBER];    /* Highlighting option active in each quadrant */
    int target[HXPOS_NUMBER];       /* Numeric target values for highlighting systems */
} DISPLAY_STRUCT, *DISPLAY_PTR;

/*
 * struct s_dmode - Display mode linked list node
 *
 * Provides linked list management for multiple display configurations,
 * allowing users to maintain several different display modes and switch
 * between them as needed. This structure wraps the display configuration
 * with list management capabilities.
 *
 * Usage Patterns:
 *   - Multiple display mode management
 *   - User preference persistence and switching
 *   - Display configuration list traversal
 *   - Dynamic display mode creation and deletion
 *
 * Relationships:
 *   - Contains complete display configuration
 *   - Managed as linked lists for user preference systems
 *   - Supports display mode switching and customization
 *
 * Fields:
 *   d - Complete display configuration structure
 *   next - Linked list pointer to next display mode
 *
 * Memory Management:
 *   - Dynamically allocated as linked lists
 *   - Contains embedded display structure
 *   - Cleanup needed when modes are deleted
 *
 * Thread Safety:
 *   - List modifications require synchronization
 *   - Display configuration access needs coordination
 *   - Mode switching requires atomic updates
 *
 * Performance Notes:
 *   - Linked list traversal for mode selection
 *   - Embedded structure accessed during rendering
 *   - Mode switching involves configuration copying
 */
typedef struct s_dmode {
    DISPLAY_STRUCT d;           /* Complete display configuration structure */
    struct s_dmode *next;       /* Linked list: next display mode in user's list */
} DMODE_STRUCT, *DMODE_PTR;

/* Everything below this point is not used by dataX.c */
#ifndef DATA_DECLARE

/* ============================================================================
 * MAIL SYSTEM CONFIGURATION
 * ============================================================================
 * Purpose: Define mail system identification for automated messages
 * Usage: Message headers, mail processing, system communications
 * Notes: Used for automated game update notifications and system messages
 */

/*
 * CQ_MAIL_NAME - Mail sender name for automated messages
 *
 * Standard sender name used for automated game update messages and system
 * communications. Provides consistent identification for mail from the game.
 *
 * Value: "Conquer"
 * Usage: Mail headers, message identification, system communications
 * Notes: Displayed to users in mail listings and message headers
 */
#define CQ_MAIL_NAME	"Conquer"

/*
 * CQ_MAIL_NICK - Mail sender nickname for automated messages
 *
 * Descriptive nickname used for automated game messages, providing a more
 * atmospheric and immersive identification for system communications.
 *
 * Value: "The Grand Overseer"
 * Usage: Mail headers, roleplay atmosphere, system message identification
 * Notes: Adds personality to automated game communications
 */
#define CQ_MAIL_NICK	"The Grand Overseer"

/* ============================================================================
 * GENERIC UTILITY MACROS
 * ============================================================================
 * Purpose: Provide standard utility functions and error handling
 * Usage: Mathematical operations, bounds checking, error management
 * Notes: Include guards prevent redefinition conflicts with system libraries
 */

/*
 * min - Return minimum of two values
 *
 * Standard minimum function macro that safely compares two values and
 * returns the smaller one. Protected with include guards to prevent
 * conflicts with system definitions.
 *
 * Parameters:
 *   a - First value to compare
 *   b - Second value to compare
 *
 * Returns: The smaller of the two input values
 * Notes: Double evaluation of parameters - avoid side effects
 */
#ifndef min
#define min(a,b)	((b) < (a) ? (b) : (a))
#endif /* min */

/*
 * max - Return maximum of two values
 *
 * Standard maximum function macro that safely compares two values and
 * returns the larger one. Protected with include guards to prevent
 * conflicts with system definitions.
 *
 * Parameters:
 *   a - First value to compare
 *   b - Second value to compare
 *
 * Returns: The larger of the two input values
 * Notes: Double evaluation of parameters - avoid side effects
 */
#ifndef max
#define max(a,b)	((b) > (a) ? (b) : (a))
#endif /* max */

/*
 * abs - Return absolute value
 *
 * Standard absolute value function macro that returns the positive
 * representation of a signed value. Protected with include guards to
 * prevent conflicts with system definitions.
 *
 * Parameters:
 *   a - Value to convert to absolute value
 *
 * Returns: Non-negative absolute value of input
 * Notes: Double evaluation of parameter - avoid side effects
 */
#ifndef abs
#define abs(a)		((a) < 0 ? -(a) : (a))
#endif /* abs */

/*
 * abrt - Emergency abort with cleanup and error reporting
 *
 * Comprehensive emergency termination macro that performs necessary cleanup
 * operations before aborting the program. Handles curses interface cleanup,
 * file closure, communication cleanup, and provides diagnostic information.
 *
 * Usage: Critical error conditions requiring immediate program termination
 * Side Effects:
 *   - Resets curses interface if active
 *   - Prints diagnostic error message with file and line information
 *   - Closes update file if open and not stderr
 *   - Performs communication hangup if needed
 *   - Calls abort() for immediate termination
 *
 * Notes: Should only be used for unrecoverable errors
 */
#define	abrt() { \
if (in_curses) cq_reset(); \
fprintf(stderr,"\nSerious Error (File %s, Line %d) - Aborting\n",__FILE__,__LINE__); \
if ((fupdate != stderr) && (fupdate != NULL)) fclose(fupdate); \
if (need_hangup) hangup(0); \
abort(); \
}

/*
 * beep - Audio notification for user attention
 *
 * Conditional audio beep that respects user preferences for sound
 * notifications. Outputs ASCII bell character if beeper is enabled.
 *
 * Usage: User attention, error notifications, completion alerts
 * Side Effects: Outputs bell character (ASCII 7) to stderr if enabled
 * Notes: Respects conq_beeper global preference setting
 */
#define beep()	if (conq_beeper) putc('\007', stderr)

/* ============================================================================
 * MAP COORDINATE VALIDATION MACROS
 * ============================================================================
 * Purpose: Validate map coordinates and bounds checking
 * Usage: Movement validation, array bounds checking, coordinate verification
 * Notes: Critical for preventing array overruns and invalid map access
 */

/*
 * XY_ONMAP - Validate map coordinates within valid bounds
 *
 * Complete bounds checking for map coordinates, ensuring both X and Y
 * coordinates are within valid map boundaries (>= 0 and < maximum).
 *
 * Parameters:
 *   x - X coordinate to validate
 *   y - Y coordinate to validate
 *
 * Returns: True if coordinates are valid, false otherwise
 * Usage: Movement validation, array access, coordinate verification
 * Notes: Preferred for complete coordinate validation
 */
#define XY_ONMAP(x,y)	(((x) >= 0) && ((y) >= 0) && ((x) < MAPX) && ((y) < MAPY))

/*
 * XY_INMAP - Validate coordinates within map dimensions (no negative check)
 *
 * Simplified bounds checking that only verifies coordinates are less than
 * maximum dimensions. Does not check for negative values.
 *
 * Parameters:
 *   x - X coordinate to validate
 *   y - Y coordinate to validate
 *
 * Returns: True if coordinates are within dimensions, false otherwise
 * Usage: Array indexing where negative values are already handled
 * Notes: Use XY_ONMAP for complete validation
 */
#define XY_INMAP(x,y)	(((x) < MAPX) && ((y) < MAPY))

/*
 * X_ONMAP - Validate X coordinate within valid bounds
 *
 * Single-axis bounds checking for X coordinates, ensuring value is
 * within valid horizontal map boundaries.
 *
 * Parameters:
 *   x - X coordinate to validate
 *
 * Returns: True if X coordinate is valid, false otherwise
 * Usage: Horizontal movement validation, column access verification
 */
#define X_ONMAP(x)	(((x) >= 0) && ((x) < MAPX))

/*
 * Y_ONMAP - Validate Y coordinate within valid bounds
 *
 * Single-axis bounds checking for Y coordinates, ensuring value is
 * within valid vertical map boundaries.
 *
 * Parameters:
 *   y - Y coordinate to validate
 *
 * Returns: True if Y coordinate is valid, false otherwise
 * Usage: Vertical movement validation, row access verification
 */
#define Y_ONMAP(y)	(((y) >= 0) && ((y) < MAPY))

/* ============================================================================
 * WORLD ACCESS MACROS
 * ============================================================================
 * Purpose: Convenient access to global world configuration and state
 * Usage: Map dimensions, turn tracking, nation limits
 * Notes: Provides abstraction layer for world structure access
 */

/*
 * MAPX - World map width dimension
 *
 * Convenient access to the world map width, automatically converting
 * from zero-based internal representation to one-based size.
 *
 * Value: world.mapx + 1 (converted to int)
 * Usage: Loop bounds, array sizing, coordinate validation
 * Notes: Adds 1 to convert from max index to dimension size
 */
#define MAPX		((int) world.mapx + 1)	/* map size */

/*
 * MAPY - World map height dimension
 *
 * Convenient access to the world map height, automatically converting
 * from zero-based internal representation to one-based size.
 *
 * Value: world.mapy + 1 (converted to int)
 * Usage: Loop bounds, array sizing, coordinate validation
 * Notes: Adds 1 to convert from max index to dimension size
 */
#define MAPY		((int) world.mapy + 1)

/*
 * TURN - Current game turn number
 *
 * Direct access to the current game turn counter for time-based
 * calculations and turn processing.
 *
 * Value: world.turn
 * Usage: Turn-based calculations, time tracking, scheduling
 */
#define TURN		world.turn		/* game turn storage loc */

/*
 * START_TURN - Campaign starting turn offset
 *
 * Access to the turn number when the campaign began, used for
 * calculating relative turn numbers and campaign duration.
 *
 * Value: world.start_turn
 * Usage: Campaign duration, relative turn calculations
 */
#define START_TURN	world.start_turn	/* relative starting point */

/*
 * MAXNTN - Maximum number of nations
 *
 * Access to the total number of nations configured in the world,
 * used for nation array bounds and iteration limits.
 *
 * Value: world.nations
 * Usage: Nation array bounds, iteration limits, allocation sizing
 */
#define MAXNTN		world.nations

/* ============================================================================
 * NATION BOUNDARY ACCESS MACROS
 * ============================================================================
 * Purpose: Convenient access to current nation's territorial boundaries
 * Usage: Territory calculations, boundary checks, nation sizing
 * Notes: Assumes ntn_ptr points to current nation
 */

/*
 * LEFTEDGE, RIGHTEDGE, TOPEDGE, BOTTOMEDGE - Nation boundary access
 *
 * Convenient access to the current nation's territorial boundaries
 * for territory calculations and boundary validation.
 *
 * Usage: Territory size calculations, boundary checking, region determination
 * Notes: Requires ntn_ptr to be set to current nation
 */
#define LEFTEDGE	ntn_ptr->leftedge
#define RIGHTEDGE	ntn_ptr->rightedge
#define TOPEDGE		ntn_ptr->topedge
#define BOTTOMEDGE	ntn_ptr->bottomedge

/* ============================================================================
 * NATION ATTRIBUTE ACCESS MACROS
 * ============================================================================
 * Purpose: Calculated access to nation attributes with unit conversion
 * Usage: Economic calculations, nation characteristics
 * Notes: Assumes ntn_ptr points to current nation
 */

/*
 * NTN_D_EATRATE - Nation eating rate as double precision
 *
 * Converts the nation's eating rate attribute from integer tenths to
 * double precision decimal value for precise calculations.
 *
 * Value: (ntn_ptr->attribute[BUTE_EATRATE] / 10.0)
 * Usage: Economic calculations requiring decimal precision
 * Notes: Requires ntn_ptr to be set to current nation
 */
#define NTN_D_EATRATE	(((double) ntn_ptr->attribute[BUTE_EATRATE]) / 10.0)

/*
 * NTN_I_EATRATE - Nation eating rate as integer
 *
 * Converts the nation's eating rate attribute from integer tenths to
 * integer units for calculations not requiring decimal precision.
 *
 * Value: (ntn_ptr->attribute[BUTE_EATRATE] / 10)
 * Usage: Economic calculations using integer arithmetic
 * Notes: Requires ntn_ptr to be set to current nation
 */
#define NTN_I_EATRATE	(ntn_ptr->attribute[BUTE_EATRATE] / 10)

/*
 * COMM_D_RANGE - Nation communication range as double precision
 *
 * Converts the nation's communication range attribute from integer tenths to
 * double precision decimal value for precise distance calculations.
 *
 * Value: (ntn_ptr->attribute[BUTE_COMMRANGE] / 10.0)
 * Usage: Communication range calculations requiring decimal precision
 * Notes: Requires ntn_ptr to be set to current nation
 */
#define COMM_D_RANGE	(((double) ntn_ptr->attribute[BUTE_COMMRANGE]) / 10.0)

/*
 * COMM_I_RANGE - Nation communication range as integer
 *
 * Converts the nation's communication range attribute from integer tenths to
 * integer units for calculations not requiring decimal precision.
 *
 * Value: (ntn_ptr->attribute[BUTE_COMMRANGE] / 10)
 * Usage: Communication range calculations using integer arithmetic
 * Notes: Requires ntn_ptr to be set to current nation
 */
#define COMM_I_RANGE	(ntn_ptr->attribute[BUTE_COMMRANGE] / 10)

/* ============================================================================
 * UNIT MANAGEMENT CONSTANTS
 * ============================================================================
 * Purpose: Special values for unit management and capacity tracking
 * Usage: Unit state management, capacity validation
 * Notes: Used throughout unit management systems
 */

/*
 * EMPTY_HOLD - Empty cargo hold indicator
 *
 * Standard value indicating an empty cargo hold or transport capacity.
 * Used for naval units, caravans, and other transport mechanisms.
 *
 * Value: 0 (no cargo)
 * Usage: Cargo management, transport capacity, hold validation
 * Notes: Zero value allows simple boolean testing for empty state
 */
#define EMPTY_HOLD	0

/* ============================================================================
 * NATION DISTRIBUTION CALCULATIONS
 * ============================================================================
 * Purpose: Calculate array indices for nation placement weighting
 * Usage: World generation, nation placement algorithms
 * Notes: Maps 2D coordinates to linear array indexing
 */

/*
 * SUM_WEIGHTS - Calculate array index for nation placement weights
 *
 * Converts 2D map coordinates to linear array index for accessing
 * nation placement weighting data. Uses row-major ordering.
 *
 * Parameters:
 *   x - X coordinate on map
 *   y - Y coordinate on map
 *
 * Returns: Linear array index for weight data
 * Usage: Nation placement, world generation, territory weighting
 * Notes: Assumes sum_weights array exists with proper dimensions
 */
#define SUM_WEIGHTS(x,y)	sum_weights[(x) + MAPX * (y)]

/* ============================================================================
 * DISPLAY COORDINATE CALCULATIONS
 * ============================================================================
 * Purpose: Convert cursor and offset positions to real map coordinates
 * Usage: Display system, cursor movement, coordinate translation
 * Notes: Handles map wrapping and coordinate transformation
 */

/*
 * XREAL - Calculate real X coordinate from cursor and offset
 *
 * Converts cursor position and offset to real map X coordinate,
 * handling horizontal map wrapping using modulo arithmetic.
 *
 * Value: (((xcurs + xoffset) + MAPX) % MAPX)
 * Usage: Display coordinate translation, cursor positioning
 * Notes: Handles negative offsets and map wrapping correctly
 */
#define	XREAL		(((xcurs + xoffset) + MAPX) % MAPX)

/*
 * YREAL - Calculate real Y coordinate from cursor and offset
 *
 * Converts cursor position and offset to real map Y coordinate.
 * No wrapping applied for vertical coordinates.
 *
 * Value: (ycurs + yoffset)
 * Usage: Display coordinate translation, cursor positioning
 * Notes: Simple addition, no wrapping for Y coordinates
 */
#define	YREAL		(ycurs + yoffset)

/* ============================================================================
 * GLOBAL VARIABLE DECLARATIONS
 * ============================================================================
 * Purpose: External declarations for global variables defined in dataX.c
 * Usage: Shared state, file handles, configuration, display system
 * Notes: All variables accessible to both client and server programs
 */

/* ============================================================================
 * FILE SYSTEM AND I/O HANDLES
 * ============================================================================
 */

/*
 * Global file handles for game data and communication
 *
 * fnews - News file handle for reading/writing game news and announcements
 * fexe - Executable file handle for program execution and utility access
 * fm - Mail file handle for player communication and messaging
 * fupdate - Update file handle for turn processing and game state changes
 */
extern FILE *fnews, *fexe, *fm, *fupdate;

/* ============================================================================
 * GAME STATE AND CONTROL VARIABLES
 * ============================================================================
 */

/*
 * Core game state and control flags
 *
 * country - Current nation ID being processed or displayed
 * global_int - General-purpose global integer for temporary calculations
 * no_input - Flag indicating input should be disabled or ignored
 * owneruid - User ID of the game owner/administrator
 * adjust_made - Flag indicating coordinate adjustments have been made
 */
extern int country, global_int, no_input, owneruid, adjust_made;

/*
 * System state and mode flags
 *
 * is_god - Flag indicating current user has god-level privileges
 * is_update - Flag indicating currently running update/turn processing
 * in_curses - Flag indicating curses interface is active
 * need_hangup - Flag indicating communication hangup is required
 * dosysm_check - Flag indicating system message checking is enabled
 */
extern int is_god, is_update, in_curses, need_hangup, dosysm_check;

/* ============================================================================
 * DISPLAY AND CURSOR MANAGEMENT
 * ============================================================================
 */

/*
 * Display cursor and viewport management
 *
 * xcurs, ycurs - Current cursor position in display coordinates
 * xoffset, yoffset - Display viewport offset from map origin
 * movemode - Current movement mode for cursor and unit movement
 */
extern int xcurs, ycurs, xoffset, yoffset, movemode;

/*
 * Coordinate adjustment tracking
 *
 * adjust_xloc, adjust_yloc - Location coordinates for adjustment operations
 */
extern int adjust_xloc, adjust_yloc;

/*
 * General-purpose global storage
 *
 * global_long - General-purpose global long integer for calculations
 */
extern long global_long;

/* ============================================================================
 * GAME DATA STRUCTURE POINTERS
 * ============================================================================
 */

/*
 * Sector data access pointers
 *
 * sct_ptr - Current sector pointer for operations
 * sct_tptr - Temporary sector pointer for calculations
 */
extern SCT_PTR sct_ptr, sct_tptr;

/*
 * Nation data access pointers
 *
 * ntn_ptr - Current nation pointer for operations
 * ntn_tptr - Temporary nation pointer for calculations
 */
extern NTN_PTR ntn_ptr, ntn_tptr;

/*
 * World map data structures
 *
 * sct - 2D array of sector structures representing the game world map
 * world - Global world configuration and state structure
 */
extern SCT_STRUCT **sct;
extern struct s_world world;

/* ============================================================================
 * DIRECTORY AND PATH CONFIGURATION
 * ============================================================================
 */

/*
 * Game directory and path configuration
 *
 * datadir - Primary data directory path for game files
 * loginname - Current user's login name
 * defaultdir - Default directory for data file location
 * datadirname - Name of the data directory
 */
extern char datadir[], loginname[], defaultdir[], datadirname[];

/*
 * Runtime identification and paths
 *
 * nationname - Current nation name for display and identification
 * string - General-purpose string buffer for operations
 * prog_name - Program name for identification and error messages
 * helpdir - Directory path for help and documentation files
 * progdir - Directory path for program executables
 */
extern char nationname[], string[], prog_name[], helpdir[], progdir[];

/* declaration of some global strings */
extern char rnumerals[];
extern char *aggressname[], *speedname[];
extern char *alignment[], *opt_list[];

/* miscellaneous file name constants */
extern char *help_files[], *zooms[ZOOM_NUMBER];
extern char *hex_list[HXPOS_NUMBER];
extern char *exetag, *datafile, *motdfile, *msgtag, *timefile;
extern char *newsfile, *tmptag, *isontag, *rulesfile, *npcfile, *blockfile;

/* sizings */
extern int help_number, options_number, tgoods_number, tgclass_number;
extern int nclass_number;

/* the mail formats */
extern char rmail_quote_prefix[LINELTH], rmail_from_format[LINELTH];

/* global options */
extern int conq_allblanks, conq_bottomlines, conq_gaudy, conq_beeper;
extern int conq_expert, zoom_level, conq_waterbottoms, conq_supply_level;
extern int conq_infomode, conq_mercsused, conq_mheaders;

/* pager settings */
int pager_tab, pager_scroll, pager_offset;

/* other miscellaneous declarations */
extern char *hasseen;
extern short *sum_weights;
extern DMODE_PTR dmode_list, dmode_tptr;

/* all global functions */
#ifndef __CEXTRACT__
#include "fileX.h"
#endif /* __CEXTRACT__ */

#endif /*DATA_DECLARE*/
#endif /*BIGINT*/
