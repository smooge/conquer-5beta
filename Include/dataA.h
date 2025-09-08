/*
 * dataA.h - Administrative Program Global Data Structures and Constants
 *
 * This header file declares the core data structures, constants, and global
 * variables used by the administrative components of the Conquer game system.
 * It provides the fundamental data types for spell management, sector status
 * tracking, and administrative program coordination.
 *
 * Key Components:
 *   - Spell casting information structures and linked list management
 *   - Sector status bit flags for siege, damage, and devastation states
 *   - Global variable declarations for administrative program coordination
 *   - Integration with extended data structures via dataX.h inclusion
 *
 * Integration Notes:
 *   - Used by: Administrative programs, spell processing, sector management
 *   - Depends on: dataX.h for extended data type definitions
 *   - Provides: Core administrative data structures and status management
 *
 * Modernization Considerations:
 *   - Spell structure could benefit from type safety improvements
 *   - Coordinate fields should use appropriate integer types for large maps
 *   - Global variable declarations should be better organized and documented
 *   - Status bit flags could use modern enum or const definitions
 *   - Linked list management could use modern container structures
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
 * DESIGN STABILITY NOTICE
 * ============================================================================
 * Note: Original comment indicates this file should not need alteration.
 * This reflects the stable nature of core administrative data structures
 * and their importance to system compatibility and data file formats.
 */

/* ::: THIS FILE SHOULD NOT NEED TO BE ALTERED ::: */

/* ============================================================================
 * DEPENDENCY INCLUSION
 * ============================================================================
 * Purpose: Include extended data type definitions required for administrative structures
 * Usage: Provides fundamental data types (ntntype, coordinate types, etc.)
 * Notes: dataX.h must be included before any structure definitions in this file
 */

#include "dataX.h"

/* ============================================================================
 * SPELL MANAGEMENT DATA STRUCTURES
 * ============================================================================
 * Purpose: Track and manage magical spell effects and their state in the game world
 * Usage: Maintains linked list of active spells for processing and persistence
 * Notes: Critical for magic system state management and world update processing
 */

/*
 * struct s_spell - Spell casting information structure
 *
 * This structure contains all necessary information about a spell that has been
 * cast in the game world. It tracks the caster, spell type, location, and
 * maintains a linked list structure for efficient management of multiple spells.
 * Used primarily during world updates to process spell effects and maintain
 * magical state across game turns.
 *
 * Usage Patterns:
 *   - Created when spells are cast during player turns
 *   - Linked into global spell list for world update processing
 *   - Processed and potentially removed during world updates
 *   - Persisted in save games to maintain magical state
 *
 * Relationships:
 *   - Links to other spells via next pointer for list management
 *   - References nation data via caster field
 *   - References world map coordinates via xloc/yloc fields
 *   - Connects to spell type definitions and effect processing
 *
 * Fields:
 *   caster - Nation that cast the spell (references nation array index)
 *   type   - Spell type identifier (references spell definition tables)
 *   xloc   - X coordinate where spell was cast (world map position)
 *   yloc   - Y coordinate where spell was cast (world map position)
 *   next   - Pointer to next spell in linked list (NULL if last)
 *
 * Modernization Notes:
 *   - Consider using standard container instead of manual linked list
 *   - Coordinate fields should use size_t for large map compatibility
 *   - Add validation for coordinate bounds and spell type validity
 *   - Consider adding timestamp or duration fields for spell effects
 *   - Type field could use enum for better type safety
 */
typedef struct s_spell {
  ntntype caster;	/* nation who cast the spell	*/
  int type;		/* type of spell that was cast	*/
  int xloc;		/* x position of spell casting	*/
  int yloc;		/* y position of spell casting	*/
  struct s_spell *next; /* pointer to next spell in linked list */
} SPLINFO_STRUCT, *SPLINFO_PTR;

/* ============================================================================
 * SECTOR STATUS BIT FLAGS
 * ============================================================================
 * Purpose: Define bit flag constants for tracking sector conditions and states
 * Usage: Used with bitwise operations to set/check multiple sector conditions
 * Notes: Each flag represents a specific sector condition that affects gameplay
 */

/*
 * SET_SIEGE - Sector under siege flag
 *
 * Indicates that a sector is currently under siege by enemy forces.
 * This affects combat calculations, resource flow, and sector accessibility.
 * When set, the sector experiences siege-related penalties and restrictions.
 *
 * Value: 0x01 (bit 0 - binary: 00000001)
 * Usage: Bitwise operations with sector status arrays
 * Notes: Affects movement, resource flow, and combat in the sector
 * Modernization: Consider using named enum for better type safety
 */
#define SET_SIEGE	0x01

/*
 * SET_DAMAGE - Sector damage flag
 *
 * Indicates that a sector has sustained damage from combat, spells, or
 * other destructive events. Damaged sectors may have reduced productivity
 * or other penalties until repaired.
 *
 * Value: 0x02 (bit 1 - binary: 00000010)
 * Usage: Bitwise operations with sector status arrays
 * Notes: Affects sector productivity and may require repair actions
 * Modernization: Consider using named enum for better type safety
 */
#define SET_DAMAGE	0x02

/*
 * SET_DEVASTATE - Sector devastation flag
 *
 * Indicates that a sector has been devastated, representing severe damage
 * or destruction that significantly impacts the sector's functionality.
 * Devastated sectors typically require extensive resources to restore.
 *
 * Value: 0x04 (bit 2 - binary: 00000100)
 * Usage: Bitwise operations with sector status arrays
 * Notes: Severe condition requiring significant resources to repair
 * Modernization: Consider using named enum for better type safety
 */
#define SET_DEVASTATE	0x04

/* ============================================================================
 * CONDITIONAL COMPILATION AND EXTERNAL DECLARATIONS
 * ============================================================================
 * Purpose: Provide external declarations when not compiling dataA.c itself
 * Usage: Prevents multiple definition errors while providing access to globals
 * Notes: Uses conditional compilation to control declaration vs definition
 */

#ifndef DATA_DECLARE
/* ============================================================================
 * ADMINISTRATIVE PROGRAM CONSTANTS
 * ============================================================================ */

/*
 * OOPS - Error indicator constant
 *
 * Constant used to indicate error conditions in nation placement and
 * other administrative operations. Typically returned when operations
 * fail or invalid conditions are encountered.
 *
 * Value: -1 (standard error return value)
 * Usage: Error checking and validation in administrative functions
 * Notes: Standard practice for indicating failure conditions
 * Modernization: Consider using more descriptive error codes or enums
 */
#define OOPS	(-1)

/* ============================================================================
 * SECTOR STATUS ACCESS MACRO
 * ============================================================================ */

/*
 * SCT_STATUS(x, y) - Sector status access macro
 *
 * Provides convenient access to sector status information by converting
 * 2D map coordinates (x, y) into a linear array index. This macro is
 * essential for efficiently accessing sector status data in the game world.
 *
 * Parameters:
 *   x - X coordinate of the sector (0 to MAPX-1)
 *   y - Y coordinate of the sector (0 to MAPY-1)
 *
 * Returns:
 *   Reference to the status byte for the specified sector
 *
 * Usage: Status checking and modification for world map sectors
 * Notes: Assumes row-major storage order (x + y * MAPX)
 * Modernization: Consider bounds checking and safer array access methods
 */
#define SCT_STATUS(x, y)	sct_status[(x) + (y) * MAPX]

/* ============================================================================
 * GLOBAL VARIABLE DECLARATIONS
 * ============================================================================
 * Purpose: Declare global variables used across administrative programs
 * Usage: Shared state for administrative program coordination and world management
 * Notes: These variables are defined in dataA.c and used throughout the system
 */

/*
 * mild_begin - Mild scenario beginning messages
 * sct_status - Sector status array for the entire world map
 *
 * mild_begin: Array of strings containing introductory messages for mild scenarios
 * sct_status: Character array storing status flags for each sector in the world
 *
 * Usage: Scenario introduction and world map status tracking
 * Notes: sct_status size depends on MAPX and MAPY world dimensions
 * Modernization: Consider using std::vector or similar container for dynamic sizing
 */
extern char *mild_begin[], *sct_status;

/*
 * spent - Resource expenditure tracking array
 * remake - World recreation flag
 *
 * spent: Array tracking resource expenditures by various game entities
 * remake: Flag indicating whether world should be recreated/rebuilt
 *
 * Usage: Economic tracking and world management decisions
 * Notes: Array sizing and indexing depends on game configuration
 * Modernization: Better type definitions and clearer variable purposes needed
 */
extern int spent[], remake;

/*
 * scenario - Current scenario name
 *
 * String containing the name of the currently active game scenario.
 * Used for scenario identification, file naming, and player information.
 *
 * Value: Character array with NAMELTH+1 capacity (null-terminated)
 * Usage: Scenario identification and file management
 * Notes: Size limited by NAMELTH constant from dataX.h
 * Modernization: Consider using std::string for dynamic sizing and safety
 */
extern char scenario[NAMELTH+1];

/* ============================================================================
 * SPELL SYSTEM GLOBAL VARIABLES
 * ============================================================================ */

/*
 * upd_spl_list - World update spell list
 *
 * Pointer to the head of the linked list containing all spells that need
 * to be processed during world updates. This list manages active magical
 * effects and their application to the game world.
 *
 * Type: SPLINFO_PTR (pointer to SPLINFO_STRUCT)
 * Usage: Spell processing during world updates and magical effect management
 * Notes: Linked list structure for dynamic spell effect management
 * Modernization: Consider using std::list or similar container for better memory management
 */
extern SPLINFO_PTR upd_spl_list;

/* ============================================================================
 * ADMINISTRATIVE HEADER INCLUSION
 * ============================================================================ * Purpose: Include additional administrative header definitions
 * Usage: Provides file management and additional administrative functionality
 * Notes: Conditional compilation prevents issues with code extraction tools
 */

/*
 * fileA.h inclusion - Administrative file management
 *
 * Includes definitions for file operations, data persistence, and administrative
 * file management functions. Conditionally compiled to prevent conflicts with
 * code extraction and analysis tools.
 *
 * Condition: Not included when __CEXTRACT__ is defined
 * Purpose: Administrative file operations and data management
 * Usage: File I/O, data persistence, and administrative program support
 */
#ifndef __CEXTRACT__
#include "fileA.h"
#endif /* __CEXTRACT__ */

/* ============================================================================
 * SYSTEM INTEGRATION ANALYSIS
 * ============================================================================
 *
 * This header file provides the core administrative data structures and
 * global variable declarations for the Conquer game system. It serves as
 * the central hub for administrative program coordination and state management.
 *
 * CRITICAL DEPENDENCIES:
 * - dataX.h: Provides fundamental data types (ntntype, MAPX, MAPY, NAMELTH)
 * - fileA.h: Administrative file operations (conditionally included)
 * - Used by all administrative programs and world management components
 * - Required for spell system, sector status tracking, and scenario management
 *
 * DATA STRUCTURE RELATIONSHIPS:
 * 1. SPLINFO_STRUCT: Core of the spell management system
 *    - Links spell effects to world coordinates and caster nations
 *    - Forms linked lists for efficient spell processing during updates
 *    - Integrates with magic system modules for effect application
 *
 * 2. Sector Status System: Bit-flag based status tracking
 *    - SET_SIEGE, SET_DAMAGE, SET_DEVASTATE work together as bit flags
 *    - SCT_STATUS macro provides efficient 2D->1D coordinate mapping
 *    - sct_status array stores status for entire world map
 *
 * 3. Global State Management: Shared administrative state
 *    - scenario[]: Current scenario identification and file naming
 *    - spent[]: Resource tracking across game entities
 *    - remake: World recreation control flag
 *    - mild_begin[]: Scenario introduction messaging
 *
 * MODERNIZATION PRIORITY ANALYSIS:
 *
 * HIGH PRIORITY (Data Safety and Performance):
 * - Replace manual linked list with standard containers (std::list, std::vector)
 * - Add bounds checking to SCT_STATUS macro to prevent buffer overflows
 * - Use size_t for coordinate fields to support large maps properly
 * - Replace bit flag #defines with type-safe enum class
 * - Add validation for spell structure fields (coordinates, caster, type)
 *
 * MEDIUM PRIORITY (Code Organization and Maintainability):
 * - Better organization of global variables with clear ownership
 * - Replace magic numbers with named constants (OOPS -> specific error codes)
 * - Add documentation for array sizing and indexing schemes
 * - Implement proper error handling instead of OOPS constant
 * - Use std::string for scenario name to avoid buffer limitations
 *
 * LOW PRIORITY (Enhancement):
 * - Add spell duration and timestamp fields for better effect management
 * - Implement spell effect composition and interaction tracking
 * - Add sector status change logging and history
 * - Implement more sophisticated error reporting system
 * - Add compile-time validation of data structure relationships
 *
 * MEMORY MANAGEMENT CONSIDERATIONS:
 * - SPLINFO_STRUCT linked list requires careful memory management
 * - sct_status array size scales with world map dimensions (MAPX * MAPY)
 * - Global arrays (mild_begin, spent) need clear ownership and lifecycle
 * - Pointer management critical for upd_spl_list integrity
 *
 * THREAD SAFETY IMPLICATIONS:
 * - Global variables present thread safety challenges
 * - Spell list manipulation requires synchronization in multi-threaded updates
 * - Sector status array access needs protection during concurrent updates
 * - Shared scenario state requires careful access control
 *
 * INTEGRATION POINTS:
 * - combatA.c/h: Uses sector status flags for siege and damage tracking
 * - magicA.c/h: Creates and manages SPLINFO_STRUCT instances
 * - updateA.c: Processes upd_spl_list during world updates
 * - worldX.c: Uses sector status for world state management
 * - All administrative programs: Use global scenario and state variables
 *
 * BACKWARD COMPATIBILITY CONSIDERATIONS:
 * - SPLINFO_STRUCT layout affects save game format compatibility
 * - Sector status bit flag values must remain stable for data persistence
 * - Global variable declarations must match definitions in dataA.c
 * - Coordinate field sizes affect world map size limitations
 *
 * SECURITY IMPLICATIONS:
 * - SCT_STATUS macro vulnerable to buffer overflow with invalid coordinates
 * - Global variables accessible system-wide increase attack surface
 * - Linked list pointers require validation to prevent corruption
 * - Array access patterns need bounds checking for security
 *
 * PERFORMANCE IMPLICATIONS:
 * - Linear linked list traversal for spell processing (O(n) per update)
 * - Direct array access for sector status provides O(1) lookup
 * - Global variables avoid parameter passing overhead
 * - Bit flag operations provide efficient status checking
 *
 * MAINTENANCE RECOMMENDATIONS:
 * 1. Implement bounds checking for all array and coordinate access
 * 2. Replace manual memory management with modern container classes
 * 3. Add comprehensive input validation for all public interfaces
 * 4. Implement proper error handling with descriptive error codes
 * 5. Add unit tests for data structure manipulation and validation
 * 6. Document array sizing requirements and memory layout assumptions
 * 7. Implement thread-safe accessors for global state variables
 * 8. Add compile-time assertions for critical size and alignment requirements
 *
 * TESTING REQUIREMENTS:
 * - Spell list management: insertion, removal, traversal, memory leaks
 * - Sector status operations: bit manipulation, coordinate mapping, bounds
 * - Global state consistency: initialization, updates, persistence
 * - Integration testing: interaction with dependent modules
 * - Performance testing: large world maps, many active spells
 * - Memory testing: leak detection, buffer overflow protection
 */

#endif /*DATA_DECLARE*/
