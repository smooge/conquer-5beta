/*
 * itemX.h - Item System Access and Adjustment Framework
 *
 * This header defines the comprehensive item system interface providing efficient
 * access to item properties, construction project management, and debugging/logging
 * capabilities. The system supports all game items including buildings, ships,
 * improvements, and equipment through a unified access framework and optional
 * execution tracking system.
 *
 * The item system provides multiple access patterns:
 * - Direct field access macros for efficient property retrieval
 * - Dual pointer system supporting primary and temporary item references
 * - Optional execution logging for debugging and replay functionality
 * - Integration with army, navy, and caravan management systems
 * - Material resource tracking for construction and crafting operations
 *
 * Key Components:
 *   - Item property access macros for all item data fields
 *   - Dual pointer system (item_ptr and item_tptr) for complex operations
 *   - Optional execution logging framework for debugging and analysis
 *   - Integration points with military units and transportation systems
 *   - Material resource management for construction projects
 *
 * Integration Notes:
 *   - Used by: Construction systems, unit management, UI display, save/load
 *   - Depends on: executeX.h (optional logging), ITEM structure definitions
 *   - Provides: Item access macros, execution logging, debugging support
 *
 * Modernization Considerations:
 *   - Consider replacing macros with inline functions for type safety
 *   - Add bounds checking for array access operations
 *   - Implement const-correctness for read-only item access
 *   - Consider structured logging instead of fprintf-based debugging
 */

/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* ============================================================================
 * PRIMARY ITEM ACCESS MACROS - Direct Property Access Interface
 * ============================================================================
 * Purpose: Efficient direct access to item structure fields via item_ptr
 * Usage: Primary item access for current/active item operations
 * Notes: Uses global item_ptr for consistent item context throughout operations
 */

/*
 * ITEM_ID - Item identification number access
 *
 * Provides direct access to the unique item identifier used throughout
 * the game systems for item tracking, referencing, and management.
 * Essential for item database operations and cross-system references.
 *
 * Expands to: item_ptr->itemid
 * Usage: Item identification, database lookup, reference tracking
 * Type: Typically integer or long identifier
 * Context: Primary item (item_ptr) identifier access
 */
#define ITEM_ID		item_ptr->itemid

/*
 * ITEM_TYPE - Item type classification access
 *
 * Accesses the item type field that determines the category and behavior
 * of the item within the game systems. Item type affects construction
 * requirements, capabilities, and interaction rules.
 *
 * Expands to: item_ptr->type
 * Usage: Item classification, behavior determination, type-specific logic
 * Type: Item type enumeration or constant value
 * Context: Primary item type identification and classification
 */
#define ITEM_TYPE	item_ptr->type

/*
 * ITEM_INFO - Item information flags and attributes access
 *
 * Provides access to the comprehensive item information field containing
 * status flags, attributes, and behavioral modifiers that control item
 * functionality and interaction with other game systems.
 *
 * Expands to: item_ptr->iteminfo
 * Usage: Status checking, attribute modification, behavior control
 * Type: Bit-packed flags or structured information field
 * Context: Primary item status and attribute management
 */
#define ITEM_INFO	item_ptr->iteminfo

/*
 * ITEM_MEN - Item workforce/personnel requirement access
 *
 * Accesses the personnel requirement field that determines how many
 * people are needed to operate, construct, or maintain the item.
 * Critical for workforce management and construction planning.
 *
 * Expands to: item_ptr->menforjob
 * Usage: Workforce allocation, construction planning, resource management
 * Type: Personnel count (integer or long)
 * Context: Primary item personnel requirements and workforce allocation
 */
#define ITEM_MEN	item_ptr->menforjob

/*
 * ITEM_ARMY - Associated army unit identifier access
 *
 * Provides access to the army unit identifier associated with this item,
 * enabling integration between items and military units for combined
 * operations, construction projects, and tactical coordination.
 *
 * Expands to: item_ptr->armyid
 * Usage: Military integration, construction assignments, tactical coordination
 * Type: Army unit identifier (integer or reference)
 * Context: Primary item army unit association and military integration
 */
#define ITEM_ARMY	item_ptr->armyid

/*
 * ITEM_NAVY - Associated navy unit identifier access
 *
 * Accesses the navy unit identifier associated with this item, supporting
 * naval construction projects, ship-based operations, and maritime
 * military integration for coastal and naval items.
 *
 * Expands to: item_ptr->navyid
 * Usage: Naval integration, ship construction, maritime operations
 * Type: Navy unit identifier (integer or reference)
 * Context: Primary item navy unit association and naval integration
 */
#define ITEM_NAVY	item_ptr->navyid

/*
 * ITEM_CVN - Associated caravan unit identifier access
 *
 * Provides access to the caravan unit identifier associated with this item,
 * enabling integration with trade caravans for transportation, supply
 * operations, and mobile construction projects.
 *
 * Expands to: item_ptr->cvnid
 * Usage: Caravan integration, supply logistics, mobile construction
 * Type: Caravan unit identifier (integer or reference)
 * Context: Primary item caravan association and logistics integration
 */
#define ITEM_CVN	item_ptr->cvnid

/*
 * ITEM_XLOC - Item X-coordinate location access
 *
 * Accesses the X-coordinate of the item's location on the game map,
 * essential for positioning, distance calculations, and spatial
 * relationships with other game entities and terrain features.
 *
 * Expands to: item_ptr->xloc
 * Usage: Position tracking, distance calculation, spatial relationships
 * Type: Map coordinate (integer)
 * Context: Primary item horizontal position on game map
 */
#define ITEM_XLOC	item_ptr->xloc

/*
 * ITEM_YLOC - Item Y-coordinate location access
 *
 * Accesses the Y-coordinate of the item's location on the game map,
 * completing the positional information needed for full spatial
 * awareness and geographic calculations within the game world.
 *
 * Expands to: item_ptr->yloc
 * Usage: Position tracking, distance calculation, spatial relationships
 * Type: Map coordinate (integer)
 * Context: Primary item vertical position on game map
 */
#define ITEM_YLOC	item_ptr->yloc

/*
 * ITEM_MTRLS - Item materials array access
 *
 * Provides access to the materials array containing resource requirements
 * or storage for the item. Critical for construction projects, resource
 * management, and economic calculations throughout the game systems.
 *
 * Expands to: item_ptr->mtrls
 * Usage: Resource management, construction materials, economic calculations
 * Type: Materials array (indexed by material type)
 * Context: Primary item material resources and requirements
 */
#define ITEM_MTRLS	item_ptr->mtrls

/* ============================================================================
 * TEMPORARY ITEM ACCESS MACROS - Secondary Item Reference Interface
 * ============================================================================
 * Purpose: Access to secondary item structure fields via item_tptr
 * Usage: Temporary item operations, comparisons, and complex item manipulations
 * Notes: Uses global item_tptr for secondary item context in dual-item operations
 */

/*
 * ITEMT_ID - Temporary item identification number access
 *
 * Provides access to the unique identifier of the temporary item referenced
 * by item_tptr. Used in operations requiring two item references such as
 * comparisons, transfers, or complex construction operations.
 *
 * Expands to: item_tptr->itemid
 * Usage: Dual-item operations, comparisons, complex item management
 * Type: Item identifier matching ITEM_ID type
 * Context: Temporary/secondary item identifier access
 */
#define ITEMT_ID	item_tptr->itemid

/*
 * ITEMT_TYPE - Temporary item type classification access
 *
 * Accesses the item type of the temporary item, enabling type-based
 * operations and comparisons between primary and temporary items
 * for compatibility checking and operational validation.
 *
 * Expands to: item_tptr->type
 * Usage: Type comparisons, compatibility checking, dual-item validation
 * Type: Item type enumeration matching ITEM_TYPE
 * Context: Temporary item type classification and comparison
 */
#define ITEMT_TYPE	item_tptr->type

/*
 * ITEMT_INFO - Temporary item information flags access
 *
 * Note: This macro has an inconsistency (uses item_ptr instead of item_tptr)
 * Provides access to item information flags for temporary item operations.
 *
 * Expands to: item_ptr->iteminfo (Note: Potential bug - should be item_tptr)
 * Usage: Temporary item status checking, attribute comparison
 * Type: Information flags matching ITEM_INFO type
 * Context: Temporary item information access (with pointer inconsistency)
 */
#define ITEMT_INFO	item_ptr->iteminfo

/*
 * ITEMT_MEN - Temporary item workforce requirement access
 *
 * Note: This macro has an inconsistency (uses item_ptr instead of item_tptr)
 * Accesses personnel requirements for temporary item operations.
 *
 * Expands to: item_ptr->menforjob (Note: Potential bug - should be item_tptr)
 * Usage: Workforce comparison, resource planning, dual-item operations
 * Type: Personnel count matching ITEM_MEN type
 * Context: Temporary item workforce access (with pointer inconsistency)
 */
#define ITEMT_MEN	item_ptr->menforjob

/*
 * ITEMT_ARMY - Temporary item army association access
 *
 * Note: This macro has an inconsistency (uses item_ptr instead of item_tptr)
 * Provides access to army unit association for temporary item.
 *
 * Expands to: item_ptr->armyid (Note: Potential bug - should be item_tptr)
 * Usage: Military integration comparison, unit coordination
 * Type: Army identifier matching ITEM_ARMY type
 * Context: Temporary item army association (with pointer inconsistency)
 */
#define ITEMT_ARMY	item_ptr->armyid

/*
 * ITEMT_NAVY - Temporary item navy association access
 *
 * Note: This macro has an inconsistency (uses item_ptr instead of item_tptr)
 * Provides access to navy unit association for temporary item.
 *
 * Expands to: item_ptr->navyid (Note: Potential bug - should be item_tptr)
 * Usage: Naval integration comparison, maritime coordination
 * Type: Navy identifier matching ITEM_NAVY type
 * Context: Temporary item navy association (with pointer inconsistency)
 */
#define ITEMT_NAVY	item_ptr->navyid

/*
 * ITEMT_CVN - Temporary item caravan association access
 *
 * Note: This macro has an inconsistency (uses item_ptr instead of item_tptr)
 * Provides access to caravan unit association for temporary item.
 *
 * Expands to: item_ptr->cvnid (Note: Potential bug - should be item_tptr)
 * Usage: Caravan integration comparison, logistics coordination
 * Type: Caravan identifier matching ITEM_CVN type
 * Context: Temporary item caravan association (with pointer inconsistency)
 */
#define ITEMT_CVN	item_ptr->cvnid

/*
 * ITEMT_XLOC - Temporary item X-coordinate access
 *
 * Accesses the X-coordinate of the temporary item's location, enabling
 * position comparisons, distance calculations, and spatial operations
 * between primary and temporary items.
 *
 * Expands to: item_tptr->xloc
 * Usage: Position comparison, distance calculation, spatial operations
 * Type: Map coordinate matching ITEM_XLOC type
 * Context: Temporary item horizontal position access
 */
#define ITEMT_XLOC	item_tptr->xloc

/*
 * ITEMT_YLOC - Temporary item Y-coordinate access
 *
 * Accesses the Y-coordinate of the temporary item's location, completing
 * the positional information needed for comprehensive spatial operations
 * and geographic calculations involving both items.
 *
 * Expands to: item_tptr->yloc
 * Usage: Position comparison, distance calculation, spatial operations
 * Type: Map coordinate matching ITEM_YLOC type
 * Context: Temporary item vertical position access
 */
#define ITEMT_YLOC	item_tptr->yloc

/*
 * ITEMT_MTRLS - Temporary item materials array access
 *
 * Provides access to the materials array of the temporary item, enabling
 * resource comparisons, transfer operations, and complex resource
 * management involving multiple items.
 *
 * Expands to: item_tptr->mtrls
 * Usage: Resource comparison, material transfers, complex resource operations
 * Type: Materials array matching ITEM_MTRLS type
 * Context: Temporary item material resources access
 */
#define ITEMT_MTRLS	item_tptr->mtrls

/* ============================================================================
 * EXECUTION LOGGING FRAMEWORK - Debugging and Analysis Support
 * ============================================================================
 * Purpose: Optional execution logging for debugging, analysis, and replay
 * Usage: Conditional compilation for development and testing builds
 * Notes: Provides detailed item operation tracking for debugging purposes
 */

#ifdef USE_CODES

#include "executeX.h"

/*
 * Execution Range Definition - Item Operation Boundaries
 *
 * Defines the range of execution codes used for item operations,
 * providing boundaries for item-related logging and analysis.
 * Used to categorize and filter item operations in debug logs.
 */

/*
 * EX_ITEMBEGIN - Start of item execution codes range
 *
 * Defines the beginning of the item operation execution code range,
 * marking the first code used for item-related operations in the
 * execution logging system.
 *
 * Value: EX_ITEMLOC (location adjustment operation code)
 * Usage: Range definition, code categorization, filtering
 * Context: Execution logging system boundary definition
 */
#define EX_ITEMBEGIN	EX_ITEMLOC

/*
 * EX_ITEMEND - End of item execution codes range
 *
 * Defines the end of the item operation execution code range,
 * marking the last code used for item-related operations in the
 * execution logging system.
 *
 * Value: EX_ITEMCVN (caravan association operation code)
 * Usage: Range definition, code categorization, filtering
 * Context: Execution logging system boundary definition
 */
#define EX_ITEMEND	EX_ITEMCVN

/* ============================================================================
 * ITEM ADJUSTMENT LOGGING MACROS - Detailed Operation Tracking
 * ============================================================================
 * Purpose: Comprehensive logging of item modifications for debugging
 * Usage: Conditional compilation for detailed operation analysis
 * Notes: Provides formatted output for item state changes and operations
 */

#ifdef PRINT_CODES

/*
 * IADJLOC - Item location adjustment logging
 *
 * Logs changes to item location coordinates, providing detailed tracking
 * of item movement and positioning operations for debugging and analysis.
 * Records both item identifier and new coordinate values.
 *
 * Output Format: "I_LOC\t[code]\t[item_id]\t[x_loc]\t[y_loc]\tnull\tnull\n"
 * Usage: Position change tracking, movement debugging, spatial analysis
 * Context: Item location modification logging
 */
#define IADJLOC	fprintf(fexe, "I_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_ITEMLOC, (int)ITEM_ID, (int)ITEM_XLOC, (int)ITEM_YLOC)

/*
 * IADJID - Item identifier adjustment logging
 *
 * Logs changes to item identifiers, tracking item ID modifications
 * and providing audit trail for item identification operations.
 * Uses global_int for the new identifier value.
 *
 * Output Format: "I_ID\t[code]\t[global_int]\t[item_id]\t0\tnull\tnull\n"
 * Usage: ID change tracking, identifier debugging, audit trails
 * Context: Item identifier modification logging
 */
#define IADJID	fprintf(fexe, "I_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ITEMID, global_int, (int)ITEM_ID)

/*
 * IARMY - Item army association logging
 *
 * Logs changes to item-army associations, tracking military unit
 * assignments and providing debugging support for military integration
 * operations and unit coordination.
 *
 * Output Format: "I_ID\t[code]\t[item_id]\t[army_id]\t0\tnull\tnull\n"
 * Usage: Military integration tracking, unit assignment debugging
 * Context: Item-army association modification logging
 */
#define IARMY	fprintf(fexe, "I_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ITEMARMY, (int)ITEM_ID, (int)ITEM_ARMY)

/*
 * INAVY - Item navy association logging
 *
 * Logs changes to item-navy associations, tracking naval unit
 * assignments and providing debugging support for maritime integration
 * operations and fleet coordination.
 *
 * Output Format: "I_ID\t[code]\t[item_id]\t[navy_id]\t0\tnull\tnull\n"
 * Usage: Naval integration tracking, fleet assignment debugging
 * Context: Item-navy association modification logging
 */
#define INAVY	fprintf(fexe, "I_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ITEMNAVY, (int)ITEM_ID, (int)ITEM_NAVY)

/*
 * ICVN - Item caravan association logging
 *
 * Logs changes to item-caravan associations, tracking trade unit
 * assignments and providing debugging support for logistics integration
 * operations and caravan coordination.
 *
 * Output Format: "I_ID\t[code]\t[item_id]\t[caravan_id]\t0\tnull\tnull\n"
 * Usage: Caravan integration tracking, logistics assignment debugging
 * Context: Item-caravan association modification logging
 */
#define ICVN	fprintf(fexe, "I_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ITEMCVN, (int)ITEM_ID, (int)ITEM_CVN)

/*
 * ITYPE - Item type modification logging
 *
 * Logs changes to item types, tracking item classification modifications
 * and providing debugging support for item type changes and category
 * reassignments throughout the game systems.
 *
 * Output Format: "I_TYPE\t[code]\t[item_id]\t[item_type]\t0\tnull\tnull\n"
 * Usage: Type change tracking, classification debugging, category analysis
 * Context: Item type modification logging
 */
#define ITYPE	fprintf(fexe, "I_TYPE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ITEMTYPE, (int)ITEM_ID, (int)ITEM_TYPE)

/*
 * IINFO - Item information flags modification logging
 *
 * Logs changes to item information flags, tracking status and attribute
 * modifications with long integer precision for comprehensive flag
 * tracking and state change analysis.
 *
 * Output Format: "L_IINFO\t[code]\t[item_id]\t[item_info]\t0\tnull\tnull\n"
 * Usage: Status change tracking, attribute debugging, flag analysis
 * Context: Item information flags modification logging
 */
#define IINFO	fprintf(fexe, "L_IINFO\t%d\t%d\t%ld\t0\tnull\tnull\n", EX_ITEMINFO, (int)ITEM_ID, ITEM_INFO)

/*
 * IMEN - Item personnel requirement modification logging
 *
 * Logs changes to item personnel requirements, tracking workforce
 * adjustments with long integer precision for detailed personnel
 * management and workforce allocation debugging.
 *
 * Output Format: "L_IMEN\t[code]\t[item_id]\t[men_count]\t0\tnull\tnull\n"
 * Usage: Workforce tracking, personnel debugging, allocation analysis
 * Context: Item personnel requirement modification logging
 */
#define IMEN	fprintf(fexe, "L_IMEN\t%d\t%d\t%ld\t0\tnull\tnull\n", EX_ITEMMEN, (int)ITEM_ID, ITEM_MEN)

/*
 * ICREATE - Item creation logging
 *
 * Logs item creation events, providing audit trail for new item
 * instantiation and tracking item lifecycle from creation through
 * destruction for comprehensive system analysis.
 *
 * Output Format: "I_CREATE\t[code]\t[item_id]\t0\t0\tnull\tnull\n"
 * Usage: Creation tracking, lifecycle analysis, audit trails
 * Context: Item creation event logging
 */
#define ICREATE	fprintf(fexe, "I_CREATE\t%d\t%d\t0\t0\tnull\tnull\n", EX_ITEMCREATE, (int)ITEM_ID)

/*
 * IMTRLS - Item materials modification logging (parameterized)
 *
 * Logs changes to specific material quantities in item materials array,
 * providing detailed resource tracking with floating-point precision
 * for accurate economic and resource management analysis.
 *
 * Parameters: x - Material type index for materials array access
 * Output Format: "D_IMTRLS\t[code]\t[item_id]\t[material_type]\t[amount]\tnull\tnull\n"
 * Usage: Resource tracking, material debugging, economic analysis
 * Context: Item materials array modification logging
 */
#define IMTRLS(x)	fprintf(fexe, "D_IMTRLS\t%d\t%d\t%d\t%f\tnull\tnull\n", EX_ITEMMTRLS, (int)ITEM_ID, (x), (double)ITEM_MTRLS[x])

/*
 * IDESTROY - Item destruction logging
 *
 * Logs item destruction events, completing the item lifecycle audit
 * trail and providing tracking for item removal and cleanup operations
 * throughout the game systems.
 *
 * Output Format: "I_DESTROY\t[code]\t[item_id]\t0\t0\tnull\tnull\n"
 * Usage: Destruction tracking, lifecycle completion, cleanup analysis
 * Context: Item destruction event logging
 */
#define IDESTROY	fprintf(fexe, "I_DESTROY\t%d\t%d\t0\t0\tnull\tnull\n", EX_ITEMDESTROY, (int)ITEM_ID)

#endif /* PRINT_CODES - End of logging macro definitions */

#endif /* USE_CODES - End of conditional execution logging framework */

/* ============================================================================
 * GLOBAL ITEM POINTER VARIABLES - Item Context Management
 * ============================================================================
 * Purpose: Global item reference variables for system-wide item access
 * Usage: Primary and temporary item context throughout game operations
 * Notes: Enables consistent item access patterns across all game systems
 */

/*
 * Global Item Pointer Variables - System-Wide Item Context
 *
 * These global variables provide the foundation for the item access macro
 * system, enabling consistent item context management throughout the game.
 * The dual-pointer system supports complex operations requiring multiple
 * item references while maintaining efficient access patterns.
 *
 * item_ptr: Primary item context for standard operations and modifications
 * item_tptr: Temporary item context for comparisons and complex operations
 *
 * Thread Safety: These global variables require careful management in
 * multi-threaded environments and should be considered for thread-local
 * storage or parameter-based access patterns in modernized implementations.
 *
 * Modernization Notes:
 * - Consider thread-local storage for multi-threaded safety
 * - Evaluate parameter-based access instead of global state
 * - Add const-correctness for read-only operations
 * - Implement bounds checking and validation for pointer operations
 * - Consider smart pointer or reference-counted systems for memory safety
 */
extern ITEM_PTR item_ptr,      /* Primary item pointer - main item context for operations */
               item_tptr;      /* Temporary item pointer - secondary item context for comparisons */
