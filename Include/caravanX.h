/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * caravanX.h - Caravan System Data Access Macros and Command Processing
 *
 * Provides comprehensive macro-based accessor system for caravan (trade expedition)
 * data structures, enabling efficient field access, modification tracking, and
 * command execution logging for overland commerce and resource transportation.
 *
 * The caravan system represents mobile commercial units that transport goods,
 * materials, and personnel between settlements across overland trade routes.
 * Caravans operate independently of cities and navies, providing flexible
 * commerce capabilities with their own crew, cargo capacity, and movement systems.
 *
 * Key Components:
 *   - CVN_* accessor macros for primary caravan pointer (cvn_ptr)
 *   - CVNT_* accessor macros for temporary caravan pointer (cvn_tptr)
 *   - VADJ* command logging macros for state change tracking
 *   - Integration with executeX.h command processing system
 *
 * Integration Notes:
 *   - Used by: caravanG.c, xferG.c, moveG.c, and other caravan interfaces
 *   - Depends on: executeX.h for command codes, core data structures
 *   - Provides: Unified field access and modification logging interface
 *
 * Modernization Considerations:
 *   - Replace macros with inline functions for type safety
 *   - Add bounds checking for array operations (materials)
 *   - Consider const qualifiers for read-only access patterns
 *   - Evaluate thread safety for multi-user caravan operations
 */
/* ============================================================================
 * PRIMARY CARAVAN DATA ACCESS MACROS (CVN_*)
 * ============================================================================
 * Purpose: Direct field access for the primary caravan pointer (cvn_ptr)
 * Usage: Standard read/write operations on current caravan entity
 * Notes: No bounds checking - unsafe for array operations without validation
 */

/*
 * CVN_ID - Unique caravan identification number
 *
 * Provides access to the caravan's unique identifier used for tracking,
 * command processing, and cross-referencing with other game systems.
 * Essential for all caravan operations and database consistency.
 *
 * Value: Positive integer uniquely identifying this caravan
 * Usage: Primary key for caravan operations and command logging
 * Notes: Must remain constant throughout caravan lifetime
 */
#define CVN_ID		cvn_ptr->cvnid

/*
 * CVN_XLOC - Current X map coordinate
 *
 * Horizontal position of the caravan on the game world map, representing
 * the current location for movement, encounters, and trade operations.
 * Coordinates are in game world units.
 *
 * Value: Integer map coordinate (0 to world width-1)
 * Usage: Movement validation, encounter resolution, trade route planning
 * Notes: Must be validated against world boundaries
 */
#define CVN_XLOC	cvn_ptr->xloc

/*
 * CVN_YLOC - Current Y map coordinate
 *
 * Vertical position of the caravan on the game world map, complementing
 * CVN_XLOC to provide complete 2D positioning for all caravan operations.
 * Essential for pathfinding and territorial interactions.
 *
 * Value: Integer map coordinate (0 to world height-1)
 * Usage: Movement validation, territorial checks, resource discovery
 * Notes: Coordinate system may be inverted depending on display mode
 */
#define CVN_YLOC	cvn_ptr->yloc

/*
 * CVN_LASTX - Previous X map coordinate
 *
 * Stores the caravan's previous horizontal position for movement tracking,
 * route history, and potential movement reversal operations. Critical for
 * implementing movement validation and retreat capabilities.
 *
 * Value: Integer map coordinate from previous turn or movement
 * Usage: Movement history, retreat operations, path optimization
 * Notes: Updated automatically during movement processing
 */
#define CVN_LASTX	cvn_ptr->lastx

/*
 * CVN_LASTY - Previous Y map coordinate
 *
 * Stores the caravan's previous vertical position, enabling complete movement
 * history tracking and supporting complex navigation behaviors including
 * backtracking and route optimization algorithms.
 *
 * Value: Integer map coordinate from previous turn or movement
 * Usage: Movement validation, tactical retreat, navigation assistance
 * Notes: Must be updated consistently with CVN_LASTX
 */
#define CVN_LASTY	cvn_ptr->lasty

/*
 * CVN_SIZE - Caravan cargo capacity and size rating
 *
 * Determines the total cargo capacity of the caravan for materials,
 * supplies, and trade goods. Affects movement speed, resource consumption,
 * and maximum load calculations for trade operations.
 *
 * Value: Positive integer representing cargo capacity units
 * Usage: Load calculations, movement speed modifiers, trade limits
 * Notes: May affect terrain traversal and encounter resolution
 */
#define CVN_SIZE	cvn_ptr->size

/*
 * CVN_CREW - Professional caravan crew members
 *
 * Number of trained personnel responsible for caravan operations including
 * navigation, security, maintenance, and trade activities. Affects caravan
 * efficiency, security rating, and operational capabilities.
 *
 * Value: Non-negative integer count of crew personnel
 * Usage: Efficiency calculations, security ratings, operational limits
 * Notes: Crew may have different skill levels and specializations
 */
#define CVN_CREW	cvn_ptr->crew

/*
 * CVN_PEOP - Civilian passengers and personnel
 *
 * Count of non-crew individuals traveling with the caravan including
 * merchants, settlers, refugees, or other passengers. Affects resource
 * consumption and may provide special capabilities or trade opportunities.
 *
 * Value: Non-negative integer count of civilian personnel
 * Usage: Resource consumption, encounter resolution, special abilities
 * Notes: Civilians may have different effects than crew members
 */
#define CVN_PEOP	cvn_ptr->people

/*
 * CVN_STAT - Caravan operational status flags
 *
 * Bitfield or status code representing the caravan's current operational
 * state including activity mode, special conditions, and temporary states
 * that affect movement, trading, and interaction capabilities.
 *
 * Value: Status bitfield or enumerated status code
 * Usage: Operational validation, activity restrictions, UI display
 * Notes: Status changes may trigger automatic behaviors or restrictions
 */
#define CVN_STAT	cvn_ptr->status

/*
 * CVN_MOVE - Movement points or mobility rating
 *
 * Available movement capacity for the current turn, determining how far
 * the caravan can travel and what terrain it can traverse. Affected by
 * size, crew efficiency, cargo load, and terrain conditions.
 *
 * Value: Non-negative integer representing movement points
 * Usage: Movement validation, pathfinding, turn planning
 * Notes: May be modified by terrain, weather, or operational conditions
 */
#define CVN_MOVE	cvn_ptr->umove

/*
 * CVN_MTRLS - Materials and trade goods array
 *
 * Multi-dimensional array storing quantities of different materials,
 * trade goods, and resources carried by the caravan. Central to commerce
 * operations and resource transportation between settlements.
 *
 * Value: Array of material quantities indexed by material type
 * Usage: Trade operations, resource management, cargo calculations
 * Notes: UNSAFE - array access requires bounds checking
 */
#define CVN_MTRLS	cvn_ptr->mtrls

/*
 * CVN_SPLY - Supply level and provisions
 *
 * Current supply level for caravan operations including food, equipment,
 * and maintenance materials necessary for extended travel and operations.
 * Critical for determining operational range and survival capabilities.
 *
 * Value: Non-negative supply level (higher values indicate better supply)
 * Usage: Range calculations, survival checks, operational efficiency
 * Notes: Depletes over time and must be replenished at settlements
 */
#define CVN_SPLY	cvn_ptr->supply

/*
 * CVN_EFF - Operational efficiency rating
 *
 * Overall effectiveness of caravan operations including crew performance,
 * equipment condition, and organizational efficiency. Affects movement
 * speed, trade success rates, and resource consumption.
 *
 * Value: Efficiency percentage (0-100 typical, higher values possible)
 * Usage: Performance calculations, movement modifiers, trade bonuses
 * Notes: Can be improved through crew training and equipment upgrades
 */
#define CVN_EFF		cvn_ptr->efficiency

/* ============================================================================
 * TEMPORARY CARAVAN DATA ACCESS MACROS (CVNT_*)
 * ============================================================================
 * Purpose: Direct field access for the temporary caravan pointer (cvn_tptr)
 * Usage: Comparison operations, batch processing, and temporary calculations
 * Notes: Mirrors CVN_* macros but operates on secondary caravan pointer
 */

/*
 * CVNT_ID - Temporary caravan unique identification number
 *
 * Provides access to the temporary caravan's unique identifier, typically
 * used for comparison operations, batch processing, or when working with
 * multiple caravan entities simultaneously during complex operations.
 *
 * Value: Positive integer uniquely identifying the temporary caravan
 * Usage: Comparison operations, batch processing, multi-caravan operations
 * Notes: Parallel to CVN_ID but operates on cvn_tptr instead of cvn_ptr
 */
#define CVNT_ID		cvn_tptr->cvnid

/*
 * CVNT_XLOC - Temporary caravan X map coordinate
 *
 * Horizontal position access for the temporary caravan pointer, enabling
 * comparison of positions, route calculations, and distance computations
 * between multiple caravan entities or operational states.
 *
 * Value: Integer map coordinate for temporary caravan
 * Usage: Position comparison, route planning, distance calculations
 * Notes: Used alongside CVN_XLOC for multi-caravan operations
 */
#define CVNT_XLOC	cvn_tptr->xloc

/*
 * CVNT_YLOC - Temporary caravan Y map coordinate
 *
 * Vertical position access for the temporary caravan pointer, complementing
 * CVNT_XLOC to provide complete 2D positioning for comparison and calculation
 * operations involving multiple caravan entities.
 *
 * Value: Integer map coordinate for temporary caravan
 * Usage: Position comparison, 2D distance calculation, route optimization
 * Notes: Essential for geometric calculations involving multiple caravans
 */
#define CVNT_YLOC	cvn_tptr->yloc

/*
 * CVNT_SIZE - Temporary caravan cargo capacity
 *
 * Cargo capacity access for the temporary caravan, enabling size comparisons,
 * load balancing operations, and capacity planning when coordinating multiple
 * caravan operations or transfer activities.
 *
 * Value: Positive integer representing cargo capacity of temporary caravan
 * Usage: Capacity comparison, load balancing, transfer planning
 * Notes: Used for comparing caravan capabilities and optimizing operations
 */
#define CVNT_SIZE	cvn_tptr->size

/*
 * CVNT_CREW - Temporary caravan crew members
 *
 * Professional crew count access for the temporary caravan, supporting
 * crew comparison operations, resource allocation planning, and efficiency
 * calculations when coordinating multiple caravan entities.
 *
 * Value: Non-negative integer count of crew in temporary caravan
 * Usage: Crew comparison, resource planning, efficiency calculations
 * Notes: Enables crew transfer and optimization between caravans
 */
#define CVNT_CREW	cvn_tptr->crew

/*
 * CVNT_PEOP - Temporary caravan civilian personnel
 *
 * Civilian count access for the temporary caravan, enabling passenger
 * comparison, transfer planning, and resource consumption calculations
 * when managing multiple caravan operations simultaneously.
 *
 * Value: Non-negative integer count of civilians in temporary caravan
 * Usage: Passenger management, transfer operations, resource planning
 * Notes: Supports civilian evacuation and relocation operations
 */
#define CVNT_PEOP	cvn_tptr->people

/*
 * CVNT_STAT - Temporary caravan operational status
 *
 * Status access for the temporary caravan, enabling status comparison,
 * conditional operations, and state-based processing when working with
 * multiple caravan entities or operational scenarios.
 *
 * Value: Status bitfield or code for temporary caravan
 * Usage: Status comparison, conditional processing, multi-caravan coordination
 * Notes: Critical for determining operational compatibility between caravans
 */
#define CVNT_STAT	cvn_tptr->status

/*
 * CVNT_MOVE - Temporary caravan movement capacity
 *
 * Movement points access for the temporary caravan, supporting movement
 * comparison, route optimization, and coordination planning when managing
 * multiple caravan movements or formation operations.
 *
 * Value: Non-negative integer movement points for temporary caravan
 * Usage: Movement comparison, route coordination, formation planning
 * Notes: Essential for synchronized movement and tactical formations
 */
#define CVNT_MOVE	cvn_tptr->umove

/*
 * CVNT_MTRLS - Temporary caravan materials array
 *
 * Materials array access for the temporary caravan, enabling cargo comparison,
 * transfer calculations, and resource redistribution operations between
 * multiple caravan entities or operational states.
 *
 * Value: Array of material quantities in temporary caravan
 * Usage: Cargo comparison, transfer operations, resource optimization
 * Notes: UNSAFE - requires bounds checking for array operations
 */
#define CVNT_MTRLS	cvn_tptr->mtrls

/*
 * CVNT_SPLY - Temporary caravan supply level
 *
 * Supply level access for the temporary caravan, supporting supply comparison,
 * redistribution planning, and resource management when coordinating multiple
 * caravan operations or emergency supply transfers.
 *
 * Value: Non-negative supply level for temporary caravan
 * Usage: Supply comparison, redistribution planning, emergency operations
 * Notes: Critical for supply chain optimization and emergency response
 */
#define CVNT_SPLY	cvn_tptr->supply

/*
 * CVNT_EFF - Temporary caravan efficiency rating
 *
 * Efficiency access for the temporary caravan, enabling performance comparison,
 * optimization planning, and resource allocation decisions when managing
 * multiple caravan entities or operational improvements.
 *
 * Value: Efficiency percentage for temporary caravan
 * Usage: Performance comparison, optimization planning, resource allocation
 * Notes: Supports efficiency-based decision making and caravan selection
 */
#define CVNT_EFF	cvn_tptr->efficiency

/* ============================================================================
 * CARAVAN COMMAND PROCESSING AND LOGGING SYSTEM
 * ============================================================================
 * Purpose: Integration with command execution and state change logging
 * Usage: Compile-time optional system for tracking caravan modifications
 * Notes: Requires USE_CODES compilation flag and executeX.h integration
 */

#ifdef USE_CODES

#include "executeX.h"

/*
 * EX_CVNBEGIN - Caravan command range start marker
 *
 * Defines the beginning of the caravan command code range within the
 * broader executeX.h command system. Used for command validation and
 * range checking to ensure caravan commands fall within allocated space.
 *
 * Value: EX_CVNLOC (first caravan command code)
 * Usage: Command range validation, system initialization
 * Notes: Must align with executeX.h command code allocation
 */
#define EX_CVNBEGIN	EX_CVNLOC

/*
 * EX_CVNEND - Caravan command range end marker
 *
 * Defines the end of the caravan command code range, providing boundaries
 * for command validation and ensuring caravan commands don't overlap with
 * other system command codes in the executeX.h framework.
 *
 * Value: EX_CVNOLOC (last caravan command code)
 * Usage: Command range validation, boundary checking
 * Notes: Inclusive end marker for caravan command code range
 */
#define EX_CVNEND	EX_CVNOLOC

/* ============================================================================
 * CARAVAN STATE CHANGE LOGGING MACROS (VADJ/VMTRLS/VCREATE/VDESTROY)
 * ============================================================================
 * Purpose: Detailed logging of caravan modifications for replay and debugging
 * Usage: Conditional compilation for development and debugging builds
 * Notes: Requires both USE_CODES and PRINT_CODES compilation flags
 */

#ifdef PRINT_CODES

/*
 * VADJLOC - Log caravan location change
 *
 * Records caravan movement to the current position, logging the caravan ID
 * along with new coordinates for movement tracking, replay systems, and
 * debugging caravan positioning and pathfinding algorithms.
 *
 * Output: "V_LOC" command with caravan ID and new X,Y coordinates
 * Usage: Called automatically when caravan position changes
 * Notes: Essential for movement validation and replay functionality
 */
#define VADJLOC	fprintf(fexe, "V_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_CVNLOC, (int)CVN_ID, (int)CVN_XLOC, (int)CVN_YLOC)

/*
 * VADJOLOC - Log caravan previous location change
 *
 * Records caravan's previous position update, maintaining movement history
 * for retreat operations, undo functionality, and tracking complete movement
 * sequences for tactical analysis and debugging.
 *
 * Output: "V_LOC" command with caravan ID and previous X,Y coordinates
 * Usage: Called when updating caravan's last known position
 * Notes: Critical for implementing movement history and retreat capabilities
 */
#define VADJOLOC	fprintf(fexe, "V_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_CVNOLOC, (int)CVN_ID, (int)CVN_LASTX, (int)CVN_LASTY)

/*
 * VADJSIZE - Log caravan size modification
 *
 * Records changes to caravan cargo capacity, essential for tracking capacity
 * modifications, upgrades, damage, and ensuring data consistency across
 * save/load operations and multiplayer synchronization.
 *
 * Output: "V_SIZE" command with caravan ID and new size value
 * Usage: Called when caravan capacity is modified or upgraded
 * Notes: Size changes affect movement speed and cargo calculations
 */
#define VADJSIZE	fprintf(fexe, "V_SIZE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNSIZE, (int)CVN_ID, (int)CVN_SIZE)

/*
 * VADJSTAT - Log caravan status change
 *
 * Records caravan operational status modifications including mode changes,
 * special conditions, and state transitions that affect caravan behavior
 * and interaction capabilities with other game systems.
 *
 * Output: "L_VSTAT" command with caravan ID and new status value
 * Usage: Called when caravan operational status changes
 * Notes: Status changes may trigger automatic behaviors or restrictions
 */
#define VADJSTAT	fprintf(fexe, "L_VSTAT\t%d\t%d\t%ld\t0\tnull\tnull\n", EX_CVNSTAT, (int)CVN_ID, CVN_STAT)

/*
 * VADJMOVE - Log caravan movement capacity change
 *
 * Records changes to caravan movement points or mobility rating, tracking
 * modifications due to crew changes, equipment upgrades, cargo load, or
 * terrain effects that impact caravan mobility and range.
 *
 * Output: "V_MOVE" command with caravan ID and new movement value
 * Usage: Called when caravan movement capacity is modified
 * Notes: Movement changes affect pathfinding and turn planning calculations
 */
#define VADJMOVE	fprintf(fexe, "V_MOVE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNMOVE, (int)CVN_ID, (int)CVN_MOVE)

/*
 * VADJID - Log caravan ID assignment or change
 *
 * Records caravan identification changes during creation, transfer, or
 * administrative operations. Uses global_int context for tracking ID
 * assignment operations and maintaining referential integrity.
 *
 * Output: "V_ID" command with context, global_int, and caravan ID
 * Usage: Called during caravan creation or ID management operations
 * Notes: Critical for maintaining database consistency and cross-references
 */
#define VADJID	fprintf(fexe, "V_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNID, global_int, (int)CVN_ID)

/*
 * VADJCREW - Log caravan crew change
 *
 * Records modifications to professional crew count including hiring,
 * dismissal, casualties, or transfers that affect caravan operational
 * efficiency, security rating, and specialized capabilities.
 *
 * Output: "V_CREW" command with caravan ID and new crew count
 * Usage: Called when crew numbers change through any mechanism
 * Notes: Crew changes directly impact efficiency and operational capabilities
 */
#define VADJCREW	fprintf(fexe, "V_CREW\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNCREW, (int)CVN_ID, (int)CVN_CREW)

/*
 * VADJSPLY - Log caravan supply level change
 *
 * Records modifications to caravan supply levels including consumption,
 * resupply operations, spoilage, or emergency supply usage that affects
 * operational range and survival capabilities.
 *
 * Output: "V_SPLY" command with caravan ID and new supply level
 * Usage: Called when supply levels change through consumption or resupply
 * Notes: Supply tracking is critical for range calculations and survival
 */
#define VADJSPLY	fprintf(fexe, "V_SPLY\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNSPLY, (int)CVN_ID, (int)CVN_SPLY)

/*
 * VADJPEOP - Log caravan civilian personnel change
 *
 * Records changes to civilian passenger count including embarkation,
 * disembarkation, casualties, or transfers that affect resource consumption
 * and special capabilities provided by civilian personnel.
 *
 * Output: "V_PEOP" command with caravan ID and new civilian count
 * Usage: Called when civilian passenger numbers change
 * Notes: Civilian changes affect resource consumption and special abilities
 */
#define VADJPEOP	fprintf(fexe, "V_PEOP\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNPEOP, (int)CVN_ID, (int)CVN_PEOP)

/*
 * VMTRLS(x) - Log specific material quantity change
 *
 * Records modifications to specific material quantities in the caravan's
 * cargo hold, tracking trade operations, consumption, production, and
 * transfer activities for complete resource movement auditing.
 *
 * Parameters:
 *   x - Material type index for the specific material being modified
 *
 * Output: "D_VMTRLS" command with caravan ID, material type, and new quantity
 * Usage: Called when specific material quantities change through any operation
 * Notes: UNSAFE - parameter x must be validated against material array bounds
 */
#define VMTRLS(x)	fprintf(fexe, "D_VMTRLS\t%d\t%d\t%d\t%f\tnull\tnull\n", EX_CVNMTRLS, (int)CVN_ID, (x), (double)CVN_MTRLS[x])

/*
 * VADJEFF - Log caravan efficiency change
 *
 * Records modifications to caravan operational efficiency including
 * improvements from crew training, equipment upgrades, or degradation
 * from damage, fatigue, or poor maintenance affecting overall performance.
 *
 * Output: "V_EFF" command with caravan ID and new efficiency rating
 * Usage: Called when efficiency rating changes through any mechanism
 * Notes: Efficiency affects movement speed, trade success, and resource usage
 */
#define VADJEFF	fprintf(fexe, "V_EFF\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNEFF, (int)CVN_ID, (int)CVN_EFF)

/*
 * VCREATE - Log caravan creation event
 *
 * Records caravan creation operations for tracking new caravan establishment,
 * resource allocation auditing, and maintaining complete operational history
 * of caravan lifecycle management within the game system.
 *
 * Output: "V_CREATE" command with caravan creation code and caravan ID
 * Usage: Called once during caravan creation process
 * Notes: Essential for tracking caravan lifecycle and resource allocation
 */
#define VCREATE	fprintf(fexe, "V_CREATE\t%d\t%d\t0\t0\tnull\tnull\n", EX_CVNCREATE, (int)CVN_ID)

/*
 * VDESTROY - Log caravan destruction event
 *
 * Records caravan destruction operations for tracking caravan elimination,
 * resource recovery auditing, and maintaining complete operational history
 * of caravan lifecycle management and loss accounting.
 *
 * Output: "V_DESTROY" command with caravan destruction code and caravan ID
 * Usage: Called once during caravan destruction process
 * Notes: Final lifecycle event - ensures proper resource cleanup tracking
 */
#define VDESTROY	fprintf(fexe, "V_DESTROY\t%d\t%d\t0\t0\tnull\tnull\n", EX_CVNDESTROY, (int)CVN_ID)

#endif /* PRINT_CODES */

#endif /* USE_CODES */

/* ============================================================================
 * CARAVAN POINTER DECLARATIONS
 * ============================================================================
 * Purpose: Global caravan pointer variables for system-wide caravan access
 * Usage: Primary and temporary caravan entity references
 * Notes: Requires proper initialization and synchronization in multi-user mode
 */

/*
 * cvn_ptr - Primary caravan pointer
 *
 * Global pointer to the currently active caravan entity, used throughout
 * the caravan system for primary operations including movement, trading,
 * crew management, and all standard caravan activities.
 *
 * Type: CVN_PTR (pointer to caravan structure)
 * Usage: Primary caravan operations and standard access patterns
 * Notes: Must be properly initialized before use with CVN_* macros
 */

/*
 * cvn_tptr - Temporary caravan pointer
 *
 * Global pointer to a secondary caravan entity, used for comparison
 * operations, batch processing, temporary calculations, and multi-caravan
 * operations that require working with multiple caravan entities.
 *
 * Type: CVN_PTR (pointer to caravan structure)
 * Usage: Secondary operations, comparisons, and multi-caravan processing
 * Notes: Used with CVNT_* macros for temporary or comparison operations
 */
extern CVN_PTR cvn_ptr, cvn_tptr;
