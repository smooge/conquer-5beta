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

/* integer function pointer */
typedef int (*FNCI)();

/* void function pointer */
#ifdef __STDC__
typedef void (*FNCV2)(int x, int y);
#else /* FNCV */
typedef void (*FNCV2)();
#endif /* FNCV */

/* Spread Sheet Data Structure */
typedef struct s_sheet {
  long people;			/* total population		*/
  itemtype mtrls[MTRLS_NUMBER];	/* the raw materials in region	*/
  short sectors;		/* sectors covered by structure	*/
  long type_people[MAJ_NUMBER];		/* people by major_desg	*/
  itemtype type_talons[MAJ_NUMBER];	/* money by major_desg	*/
  int type_sectors[MAJ_NUMBER];	/* sectors by major designation	*/
  long army_men;		/* number of army men in region */
  int monst_troops;		/* number of monster troops	*/
  int ship_holds;		/* ship holds in region		*/
  int caravan_wagons;		/* caravan wagons in region	*/
  itemtype army_cost;		/* support cost for troops	*/
  itemtype monst_jewels;	/* tribute to monster units	*/
  itemtype navy_cost;		/* support cost for fleets	*/
  itemtype cvn_cost;		/* support cost for wagons	*/
} SHEET_STRUCT, *SHEET_PTR;

/* Sector Data Structure */
typedef struct s_sector {
  uns_short designation;	/* design of a sector	*/
  uns_char altitude;	/* sector altitude		*/
  uns_char vegetation;	/* sector vegetation		*/
  ntntype owner;	/* nation id of owner		*/
  uns_char efficiency;	/* the efficiency of the sector	*/
  long people;		/* civilians in sector		*/
  uns_char minerals;	/* jewel / metal value		*/
  uns_char tradegood;	/* exotic trade goods in sector	*/
  /* uns_char region;	 index of region		*/
} SCT_STRUCT, *SCT_PTR;

/* Army Data Structure */
typedef struct s_army {
  idtype armyid;	/* unique identifier for unit	*/
  uns_char unittype;	/* type of the army unit	*/
  uns_char max_eff;	/* the efficiency of the unit	*/
  uns_char efficiency;	/* the efficiency of the unit	*/
  maptype xloc, yloc;	/* location of the army unit	*/
  maptype targx, targy;	/* target sector for the unit	*/
  maptype lastx, lasty;	/* prior location of the unit	*/
  long strength;	/* number of soldiers		*/
  uns_char umove;	/* movement ability of the unit	*/
  long status;		/* the status of the unit	*/
  uns_char spellpts;	/* magic strength of leaders	*/
  uns_char supply;	/* food supply per man		*/
  idtype leader;	/* unit the army is lead by	*/
  struct s_army *next;	/* to next army			*/
  struct s_army *nrby;	/* to armies in same sector	*/
} ARMY_STRUCT, *ARMY_PTR;

/* Navy Data Structure */
typedef struct s_navy {
  idtype navyid;	/* unique identifier		*/
  uns_short ships[NSHP_NUMBER];	/* warships, merchants galleys */
  uns_char efficiency[NSHP_NUMBER];	/* the efficiency of the fleet	*/
  maptype xloc, yloc;	/* location of the ships	*/
  maptype targx, targy;	/* target sector for the fleet	*/
  maptype lastx, lasty;	/* prior location of the ships	*/
  uns_char umove;	/* movement ability of ships	*/
  long status;		/* the status of the unit	*/
  uns_char crew;	/* percent of crew on the ships	*/
  uns_char people;	/* people carried per ghold	*/
  uns_char supply;	/* supplies per crew member	*/
  idtype armynum;	/* army unit carried		*/
  idtype cvnnum;	/* caravan unit carried		*/
  itemtype mtrls[MTRLS_NUMBER];	/* materials carried	*/
  struct s_navy *next;	/* next navy unit		*/
} NAVY_STRUCT, *NAVY_PTR;

/* Caravan Data Structure */
typedef struct s_caravan {
  idtype cvnid;		/* unique identifier		*/
  uns_char efficiency;	/* the efficiency of the cvan	*/
  maptype xloc, yloc;	/* location of the caravan	*/
  maptype targx, targy;	/* target sector for the cvan	*/
  maptype lastx, lasty;	/* last location of the caravan	*/
  uns_char size;	/* how many wagons in caravan	*/
  uns_char umove;	/* movement ability of caravan	*/
  long status;		/* the status of the unit	*/
  uns_char crew;	/* percent of caravan crew	*/
  uns_char people;	/* people carried per wagon	*/
  uns_char supply;	/* supplies per crew member	*/
  itemtype mtrls[MTRLS_NUMBER];	/* materials carried	*/
  struct s_caravan *next;	/* next caravan unit	*/
} CVN_STRUCT, *CVN_PTR;

/* City Structure */
typedef struct s_city {
  maptype xloc, yloc;	/* location of the city		*/
  char name[NAMELTH+1];	/* name	of the city		*/
  uns_short cityid;	/* internal identification	*/
  short i_people;	/* initial civilians in sector	*/
  uns_char efficiency;	/* the efficiency of the region	*/
  uns_char weight;	/* distribution value of a city	*/
  itemtype c_mtrls[MTRLS_NUMBER];	/* materials	*/
  itemtype i_mtrls[MTRLS_NUMBER];	/* dist mtrls	*/
  itemtype s_talons;	/* starting talons in city	*/
  itemtype m_mtrls[MTRLS_NUMBER];	/* min keep	*/
  long auto_flags[MTRLS_NUMBER];	/* dist info	*/
  long cmd_flag;	/* navy/cvn command information	*/
  uns_char fortress;	/* fortification value of city	*/
  struct s_city *next;	/* next city storage structure	*/
} CITY_STRUCT, *CITY_PTR;

/* Item Structure */
typedef struct s_item {
  maptype xloc, yloc;	/* location of the item		*/
  idtype itemid;	/* unique identifier of item	*/
  unsigned char type;	/* the task of this item	*/
  long iteminfo;	/* information about item use	*/
  long menforjob;	/* the number of men needed	*/
  idtype armyid;	/* army id (sale or construct)	*/
  idtype navyid;	/* navy id (sale or construct)	*/
  idtype cvnid;		/* caravan id (for sale)	*/
  itemtype mtrls[MTRLS_NUMBER];	/* the raw materials	*/
  struct s_item *next;	/* pointer to next item struct	*/
} ITEM_STRUCT, *ITEM_PTR;

/* Automatic Unit Numbering */
typedef struct s_unitnum {
  int number;			/* number to start from */
  int ident;			/* identifier of the class/unit/etc. */
  int type;			/* type of the numeric */
  struct s_unitnum *next;	/* to next element in chain */
} UNITNUM, *UNUM_PTR;

/* Mapping Structure */
typedef struct s_map {
  maptype xloc, yloc;		/* location of the mapping element	*/
  uns_char range;		/* distance visible from this locale	*/
  uns_char strength;		/* the strength of the visibility	*/
  uns_char lifetime;		/* duration of visibility -- 0 = perm	*/
  struct s_map *next;
} MAP_STRUCT, *MAP_PTR;

/* Nation Data Structure */
typedef struct s_nation {
  char name[NAMELTH+1];		/* name				*/
  char login[NAMELTH+1];	/* login name of user		*/
  char passwd[PASSLTH+1];	/* password			*/
  char leader[LEADERLTH+1];	/* leader title			*/
  short repro;			/* reproduction rate of nation	*/
  short race;			/* national race		*/
  char mark;			/* unique mark for nation	*/
  char location;		/* how well placed on creation	*/
  maptype capx;			/* Capital x coordinate		*/
  maptype capy;			/* Capital y coordinate		*/
  maptype centerx;		/* Relative central X location	*/
  maptype centery;		/* Relative central Y location	*/
  short leftedge;		/* Left Edge of Nation		*/
  short rightedge;		/* Right Edge of Nation		*/
  short topedge;		/* Top Edge of Nation		*/
  short bottomedge;		/* Bottom Edge of Nation	*/
  short class;			/* national class		*/
  short aplus;			/* attack plus of all soldiers	*/
  short dplus;			/* defense plus of all soldiers	*/
  long score;			/* score			*/
  short active;			/* nation type and strategy	*/
  short maxmove;		/* maximum movement of soldiers */
  idtype num_army;		/* for saves; number of armies	*/
  idtype num_navy;		/* for saves; number of navies	*/
  idtype num_city;		/* for saves; # of city structs	*/
  idtype num_item;		/* for saves; # of item structs	*/
  idtype num_cvn;		/* for saves; # of caravans	*/
  idtype num_unum;		/* for saves; # of autonum itms	*/
  idtype num_maps;		/* for saves; # of map structs	*/
  ARMY_PTR army_list;		/* the army units of the nation	*/
  NAVY_PTR navy_list;		/* the navy units of the nation	*/
  CITY_PTR city_list;		/* the city information		*/
  ITEM_PTR item_list;		/* national commodities		*/
  CVN_PTR cvn_list;		/* trading caravans		*/
  UNUM_PTR unum_list;		/* autonumbering schemes	*/
  MAP_PTR map_list;		/* mapping of sectors		*/
  char dstatus[ABSMAXNTN];	/* diplomatic status		*/
  itemtype mtrls[MTRLS_NUMBER];	/* sum of materials in nation	*/
  itemtype m_new[MTRLS_NUMBER];	/* sum of produced materials	*/
  short tsctrs;			/* total number sectors		*/
  short tunsctrs;		/* total number unsupported	*/
  short tships;			/* number of ships		*/
  short twagons;		/* number of wagons		*/
  long tmonst;			/* total number of monsters	*/
  long tleaders;		/* total number of leaders	*/
  long tmil;			/* total military		*/
  long tciv;			/* total civilians		*/
  long powers[MAG_NUMBER];	/* lists of national powers	*/
  short attribute[BUTE_NUMBER];	/* national attributes		*/
  off_t mbox_size;		/* last known size of conq mail	*/
  off_t news_size;		/* last known size of conq news	*/
} NTN_STRUCT, *NTN_PTR;

/* World Data Structure */
struct s_world
{
  maptype mapx, mapy;	/* size of world		*/
  ntntype nations;	/* total number of nations	*/
  ntntype npcs;		/* number of beginning npcs	*/
  ntntype active_ntns;	/* number of active nations	*/
  NTN_PTR np[ABSMAXNTN];	/* nations of the world */
  short turn;		/* count of game turn		*/
  short start_turn;	/* turn on which campaign began	*/
  char demigod[NAMELTH+1];	/* name of the demi-god	*/
  char passwd[PASSLTH+1];	/* god's password	*/
  char addpwd[PASSLTH+1];	/* add nation password	*/
  char demibuild;	/* flag allows demigod rebuild	*/
  char demilimit;	/* flag limits demigod powers	*/
  char relative_map;	/* flag for messing up players	*/
  char hexmap;		/* flag to indicate hexmap mode	*/
  char verify_login;	/* flag to increase security	*/
  char hide_scores;	/* flag to hide nation scores	*/
  char hide_login;	/* flag to hide nation owners	*/
  char builddist;	/* min dist between nations	*/
  short fleet_cbval;	/* base combat bonus of navies	*/
  short wagon_cbval;	/* base combat bonus of cvns	*/
  char num_dice;	/* number of combat roll dice	*/
  char avg_damage;	/* average damage during combat	*/
  char damage_limit;	/* damage needed to stop attack	*/
  char overmatch_adj;	/* adjust to dam for rel_size	*/
  char pmindamage;	/* min damage level (% of roll)	*/
  char smoothings;	/* smoothing algorithm repeats	*/
  char land_range;	/* displacement check for water	*/
  char preject_land;	/* % chance rejection for land	*/
  char pwater;		/* percentage of water		*/
  char pmount;		/* % of land that is mountains	*/
  char pmercval;	/* % of disbanded to mercs	*/
  char prevolt;		/* % yearly revolts level	*/
  char pscout;		/* % chance to capture scouts	*/
  char pmonster;	/* % per year for monster repro	*/
  char pmercmonst;	/* % of monsters who goto mercs	*/
  char ptrade;		/* % of sectors with tradegoods	*/
  char ptgmetal;	/* metal tradegood percentage	*/
  char ptgjewel;	/* jewel tradegood percentage	*/
  char ptgspell;	/* magic tradegood percentage	*/
  char supplylimit;	/* limit on supply carrying	*/
  char nvsplydist;	/* distance of supply for n & v	*/
  char cityxfdist;	/* max dist for city transfers	*/
  uns_char exposelvl;	/* the exposure level: 10=norm	*/
  uns_char maxpts;	/* maximum number of build pts	*/
  uns_char lizards;	/* how many lizards are created	*/
  uns_char savages;	/* how many savages are created	*/
  uns_char nomads;	/* how many nomads are created	*/
  uns_char pirates;	/* how many pirates are created	*/
  char numrandom;	/* number of random events/turn	*/
  char numweather;	/* number of weather/turn	*/
  long m_mil;		/* number of mercs available	*/
  short m_aplus;	/* mercenary attack bonus	*/
  short m_dplus;	/* mercenary defense bonus	*/
  itemtype bribelevel;	/* value level of bribes	*/
  itemtype w_mtrls[MTRLS_NUMBER];	/* all resource	*/
  long w_civ;		/* world population		*/
  long w_mil;		/* world military		*/
  long w_sctrs;		/* owned sectors in world	*/
  long w_score;		/* world score total		*/
  off_t mbox_size;	/* size of god's mbox when read	*/
  off_t news_size;	/* gods knowledge of news size	*/
  int num_unum;		/* how many default unit nums	*/
  UNUM_PTR dflt_unum;	/* default world unit numbering	*/
  uns_char maxdipadj;	/* maximum diplomacy adjustment	*/
  uns_char latestart;	/* turns before password check	*/
  uns_char growthrate;	/* speed of world reproduction	*/
};

/* display information structures */
typedef struct s_display {
  char name[DISPLAYLTH + 1];	/* the name of the display option */
  int focus;			/* which quadrant is the focus quadrant */
  int style[HXPOS_NUMBER];	/* display selection in each quadrant */
  int highlight[HXPOS_NUMBER];	/* highlight option in each quadrant */
  int target[HXPOS_NUMBER];	/* numeric target of highlights */
} DISPLAY_STRUCT, *DISPLAY_PTR;

/* the mode structure */
typedef struct s_dmode {
  DISPLAY_STRUCT d;		/* the actual display information */
  struct s_dmode *next;		/* the next display for linked lists */
} DMODE_STRUCT, *DMODE_PTR;

/* Everything below this point is not used by dataX.c */
#ifndef DATA_DECLARE

/* Name and nickname of update mail messages */
#define CQ_MAIL_NAME	"Conquer"
#define CQ_MAIL_NICK	"The Grand Overseer"

/* Various Generic Macros */
#ifndef min
#define min(a,b)	((b) < (a) ? (b) : (a))
#endif /* min */
#ifndef max
#define max(a,b)	((b) > (a) ? (b) : (a))
#endif /* max */
#ifndef abs
#define abs(a)		((a) < 0 ? -(a) : (a))
#endif /* abs */

#define	abrt() { \
if (in_curses) cq_reset(); \
fprintf(stderr,"\nSerious Error (File %s, Line %d) - Aborting\n",__FILE__,__LINE__); \
if ((fupdate != stderr) && (fupdate != NULL)) fclose(fupdate); \
if (need_hangup) hangup(); \
abort(); \
}

#define beep()	if (conq_beeper) putc('\007', stderr)

/* macros to determine proper sector */
#define XY_ONMAP(x,y)	(((x) >= 0) && ((y) >= 0) && ((x) < MAPX) && ((y) < MAPY))
#define XY_INMAP(x,y)	(((x) < MAPX) && ((y) < MAPY))
#define X_ONMAP(x)	(((x) >= 0) && ((x) < MAPX))
#define Y_ONMAP(y)	(((y) >= 0) && ((y) < MAPY))

/* World Definitions */
#define MAPX		((int) world.mapx + 1)	/* map size */
#define MAPY		((int) world.mapy + 1)
#define TURN		world.turn		/* game turn storage loc */
#define START_TURN	world.start_turn	/* relative starting point */
#define MAXNTN		world.nations

/* edge shortcuts */
#define LEFTEDGE	ntn_ptr->leftedge
#define RIGHTEDGE	ntn_ptr->rightedge
#define TOPEDGE		ntn_ptr->topedge
#define BOTTOMEDGE	ntn_ptr->bottomedge

/* other nation information definitions */
#define NTN_D_EATRATE	(((double) ntn_ptr->attribute[BUTE_EATRATE]) / 10.0)
#define NTN_I_EATRATE	(ntn_ptr->attribute[BUTE_EATRATE] / 10)
#define COMM_D_RANGE	(((double) ntn_ptr->attribute[BUTE_COMMRANGE]) / 10.0)
#define COMM_I_RANGE	(ntn_ptr->attribute[BUTE_COMMRANGE] / 10)

/* Army Unit Numbering Schemes */
#define EMPTY_HOLD	0

/* Nation weighting distributions */
#define SUM_WEIGHTS(x,y)	sum_weights[(x) + MAPX * (y)]

/* Sector location definitions */
#define	XREAL		(((xcurs + xoffset) + MAPX) % MAPX)
#define	YREAL		(ycurs + yoffset)

/* File: dataX.c -- global data for both programs */
extern FILE *fnews, *fexe, *fm, *fupdate;
extern int country, global_int, no_input, owneruid, adjust_made;
extern int is_god, is_update, in_curses, need_hangup, dosysm_check;
extern int xcurs, ycurs, xoffset, yoffset, movemode;
extern int adjust_xloc, adjust_yloc;
extern long global_long;
extern SCT_PTR sct_ptr, sct_tptr;
extern NTN_PTR ntn_ptr, ntn_tptr;
extern SCT_STRUCT **sct;
extern struct s_world world;
extern char datadir[], loginname[], defaultdir[], datadirname[];
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
