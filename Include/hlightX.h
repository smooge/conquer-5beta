/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * hlightX.h - Sector Highlighting and Visibility System
 *
 * This header defines the game's sector highlighting and visibility tracking
 * system, providing efficient mechanisms for managing what players can see
 * and how different map sectors are visually highlighted based on various
 * game criteria.
 *
 * The system combines two key functionalities:
 * 1. Sector Visibility: Tracks what parts of the map each player has explored
 * 2. Highlighting System: Allows dynamic visual highlighting of sectors based
 *    on strategic, diplomatic, or gameplay criteria
 *
 * Technical Implementation:
 * - Uses bit-packed data structure for memory efficiency
 * - Lower 4 bits store visibility level (0-4 scale)
 * - Upper 28 bits available for highlighting flags
 * - Single long integer per map sector for all visibility/highlight data
 *
 * Performance Considerations:
 * - Efficient bit manipulation for fast visibility checks
 * - Single array lookup for both visibility and highlighting
 * - Minimal memory footprint despite rich functionality
 *
 * Integration Notes:
 * - Integrates with map display system for visual rendering
 * - Supports strategic overlay displays for tactical planning
 * - Extensible highlighting system for future gameplay features
 */

/* This file contains the mechanisms to implement the highlights and */
/* sector visibility [old HAS_SEEN code] functionality.              */

/*
 * _VD_ - Visibility Data Array Access Macro
 *
 * Provides efficient access to the visibility data array using 2D coordinates.
 * Converts (x,y) map coordinates to linear array index for storage access.
 *
 * Parameters:
 *   x - X coordinate on the map (0 to MAPX-1)
 *   y - Y coordinate on the map (0 to MAPY-1)
 *
 * Returns:
 *   Reference to the long integer containing visibility and highlighting data
 *   for the specified map sector
 *
 * Implementation:
 *   Uses row-major ordering: index = x + MAPX * y
 *   Assumes MAPX is defined elsewhere as the map width
 *
 * Usage:
 *   Access: long data = _VD_(10, 5);
 *   Modify: _VD_(10, 5) = new_value;
 */
#define _VD_(x,y)	visibility_data[(x) + MAPX * (y)]

/*
 * Bit Manipulation Constants for Visibility and Highlighting Data
 *
 * These constants define the bit layout within each visibility data entry:
 * - Lower 4 bits (0-3): Visibility level (0-15, but only 0-4 used)
 * - Upper 28 bits (4-31): Highlighting flags for various display modes
 */

/*
 * SCTR_SKIP - Bit Offset for Highlighting Flags
 *
 * Defines the number of bits to skip when accessing highlighting flags.
 * The lower 4 bits are reserved for visibility level data.
 *
 * Value: 4 (highlighting flags start at bit position 4)
 */
#define SCTR_SKIP	4

/*
 * SCTR_MASK - Sector Highlighting Bits Mask
 *
 * Bit mask to isolate the highlighting flags portion of the visibility data.
 * Preserves upper 28 bits while clearing lower 4 visibility bits.
 *
 * Value: 0xFFFFFFF0 (binary: 11111111111111111111111111110000)
 * Usage: Used when updating visibility while preserving highlighting flags
 */
#define SCTR_MASK	0xFFFFFFF0

/*
 * SCTR_NMASK - Sector Visibility Bits Mask
 *
 * Bit mask to isolate the visibility level portion of the data.
 * Preserves lower 4 bits while clearing upper 28 highlighting bits.
 *
 * Value: 0x0000000F (binary: 00000000000000000000000000001111)
 * Usage: Used when reading visibility level without highlighting data
 */
#define SCTR_NMASK	0x0000000F

/*
 * Sector Visibility Access Macros
 *
 * These macros provide the interface for reading and updating the visibility
 * level of map sectors, implementing the "fog of war" and exploration system.
 */

/*
 * VIS_CHECK - Read Sector Visibility Level
 *
 * Retrieves the current visibility level for a map sector without affecting
 * any highlighting flags that may be set.
 *
 * Parameters:
 *   x - X coordinate of the sector
 *   y - Y coordinate of the sector
 *
 * Returns:
 *   Integer visibility level (0-4):
 *   - 0 (HS_NOSEE): Sector never seen
 *   - 1 (HS_SEEPART): Partially visible
 *   - 2 (HS_SEEMOST): Mostly visible
 *   - 3 (HS_SEEFULL): Fully visible
 *   - 4 (HS_SEEALL): Complete visibility
 *
 * Implementation:
 *   Uses bitwise AND with SCTR_NMASK to isolate visibility bits
 */
#define VIS_CHECK(x,y)	(_VD_(x,y) & SCTR_NMASK)

/*
 * VIS_STORE - Update Sector Visibility Level
 *
 * Sets the visibility level for a map sector while preserving all existing
 * highlighting flags. This is the primary mechanism for updating fog of war
 * as players explore the map.
 *
 * Parameters:
 *   x - X coordinate of the sector
 *   y - Y coordinate of the sector
 *   N - New visibility level (0-4, should use HS_* constants)
 *
 * Side Effects:
 *   Updates the visibility data for the specified sector
 *   Preserves all highlighting flags in upper bits
 *
 * Implementation:
 *   1. Masks out old visibility bits with SCTR_MASK
 *   2. ORs in new visibility level N
 *   3. Stores result back to visibility data array
 *
 * Safety Notes:
 *   - N should be masked to 4 bits to avoid corrupting highlighting flags
 *   - No bounds checking on coordinates
 */
#define VIS_STORE(x,y,N)	_VD_(x,y) = ((_VD_(x,y) & SCTR_MASK) | N)

/*
 * Sector Highlighting Management Macros
 *
 * These macros provide the interface for managing dynamic highlighting of
 * map sectors based on various game criteria such as diplomacy, resources,
 * military units, or strategic overlays.
 */

/*
 * HIGH_CHECK - Test Sector Highlighting Flag
 *
 * Checks whether a specific highlighting flag is set for a map sector.
 * Used to determine if a sector should be visually highlighted in the
 * display based on current overlay settings.
 *
 * Parameters:
 *   x - X coordinate of the sector
 *   y - Y coordinate of the sector
 *   T - Highlighting type/flag number (0-27, corresponds to HI_* constants)
 *
 * Returns:
 *   Non-zero if the highlighting flag is set, zero if not set
 *
 * Implementation:
 *   1. Calculates bit position: T + SCTR_SKIP (T + 4)
 *   2. Creates bit mask: 1 << bit_position
 *   3. Tests bit with bitwise AND operation
 *
 * Usage Example:
 *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { sector highlighted }
 */
#define HIGH_CHECK(x,y,T)	(_VD_(x,y) & (1 << (T + SCTR_SKIP)))

/*
 * HIGH_SETON - Enable Sector Highlighting Flag
 *
 * Sets a specific highlighting flag for a map sector, causing it to be
 * visually highlighted when the corresponding display overlay is active.
 *
 * Parameters:
 *   x - X coordinate of the sector
 *   y - Y coordinate of the sector
 *   T - Highlighting type/flag number (0-27, corresponds to HI_* constants)
 *
 * Side Effects:
 *   Sets the specified highlighting bit in the visibility data
 *   Does not affect visibility level or other highlighting flags
 *
 * Implementation:
 *   1. Calculates bit position: T + SCTR_SKIP (T + 4)
 *   2. Creates bit mask: 1 << bit_position
 *   3. Sets bit with bitwise OR operation
 *
 * Usage Example:
 *   HIGH_SETON(x, y, HI_DIPLOMACY); // Mark sector for diplomatic highlighting
 */
#define HIGH_SETON(x,y,T)	_VD_(x,y) |= (1 << (T + SCTR_SKIP))

/*
 * HIGH_SETOFF - Disable Sector Highlighting Flag
 *
 * Clears a specific highlighting flag for a map sector, removing it from
 * visual highlighting when the corresponding display overlay is active.
 *
 * Parameters:
 *   x - X coordinate of the sector
 *   y - Y coordinate of the sector
 *   T - Highlighting type/flag number (0-27, corresponds to HI_* constants)
 *
 * Side Effects:
 *   Clears the specified highlighting bit in the visibility data
 *   Does not affect visibility level or other highlighting flags
 *
 * Implementation:
 *   1. Calculates bit position: T + SCTR_SKIP (T + 4)
 *   2. Creates bit mask: 1 << bit_position
 *   3. Inverts mask: ~bit_mask
 *   4. Clears bit with bitwise AND operation
 *
 * Usage Example:
 *   HIGH_SETOFF(x, y, HI_DIPLOMACY); // Remove diplomatic highlighting
 */
#define HIGH_SETOFF(x,y,T)	_VD_(x,y) &= ~(1 << (T + SCTR_SKIP))

/*
 * Sector Visibility Level Constants
 *
 * These constants define the different levels of sector visibility in the
 * fog of war system. Higher values indicate greater visibility and more
 * detailed information available to the player.
 */

/*
 * HS_NOSEE - No Visibility
 *
 * Sector has never been explored or seen by the player.
 * Appears completely dark/hidden in fog of war.
 * No information about terrain, units, or structures available.
 *
 * Value: 0
 * Usage: Initial state for unexplored map areas
 */
#define HS_NOSEE	0

/*
 * HS_SEEPART - Partial Visibility
 *
 * Sector has been partially explored or is at the edge of vision range.
 * Basic terrain information may be visible but details are limited.
 * Unit and structure information may be incomplete or outdated.
 *
 * Value: 1
 * Usage: Edge of exploration range, limited reconnaissance
 */
#define HS_SEEPART	1

/*
 * HS_SEEMOST - Mostly Visible
 *
 * Sector is well within explored territory with good visibility.
 * Most terrain and structural information is available.
 * Recent unit activity may be visible.
 *
 * Value: 2
 * Usage: Recently explored areas with good intelligence
 */
#define HS_SEEMOST	2

/*
 * HS_SEEFULL - Full Visibility
 *
 * Sector is fully explored with complete information available.
 * All terrain, structures, and current unit positions are visible.
 * Real-time updates on changes within the sector.
 *
 * Value: 3
 * Usage: Territory under direct control or observation
 */
#define HS_SEEFULL	3

/*
 * HS_SEEALL - Complete Visibility
 *
 * Maximum visibility level with complete tactical information.
 * All details about terrain, units, structures, and activities visible.
 * May include special intelligence or omniscient view capabilities.
 *
 * Value: 4
 * Usage: Areas under direct control, debug mode, or special abilities
 */
#define HS_SEEALL	4

/*
 * Highlighting Scheme Constants (Future Implementation)
 *
 * NOTE: These highlighting schemes are currently disabled (NOTDONE ifdef)
 * but represent the planned strategic overlay system for tactical displays.
 * Maximum of 28 highlighting schemes supported by the bit layout.
 * 
 * Development Note: These definitions should be moved to displayX.h when
 * the highlighting system is fully implemented.
 *
 * Each constant represents a different tactical overlay that can be applied
 * to the map display to highlight sectors based on specific criteria.
 */

#ifdef NOTDONE

/*
 * Strategic Information Overlays
 */

/*
 * HI_POPLEVEL - Population Level Highlighting
 * Highlights sectors based on population density or demographic data.
 * Useful for economic planning and resource allocation strategies.
 */
#define HI_POPLEVEL	1

/*
 * HI_DIPLOMACY - Diplomatic Status Highlighting  
 * Highlights sectors based on diplomatic relationships and territory control.
 * Shows allied, neutral, and enemy controlled areas.
 */
#define HI_DIPLOMACY	2

/*
 * HI_MAJDESG - Major Designation Highlighting
 * Highlights sectors with major strategic designations or classifications.
 * Includes capital cities, major fortresses, or key strategic points.
 */
#define HI_MAJDESG	3

/*
 * HI_MINDESG - Minor Designation Highlighting
 * Highlights sectors with minor strategic designations or classifications.
 * Includes outposts, minor settlements, or secondary strategic points.
 */
#define HI_MINDESG	4

/*
 * Terrain and Resource Overlays
 */

/*
 * HI_VEGETATION - Vegetation Type Highlighting
 * Highlights sectors based on vegetation and terrain characteristics.
 * Useful for movement planning and resource identification.
 */
#define HI_VEGETATION	5

/*
 * HI_ELEVATION - Elevation Level Highlighting
 * Highlights sectors based on elevation and topographical features.
 * Important for tactical positioning and movement planning.
 */
#define HI_ELEVATION	6

/*
 * HI_TRADEGOOD - Trade Goods Highlighting
 * Highlights sectors containing valuable trade goods or resources.
 * Essential for economic strategy and supply chain planning.
 */
#define HI_TRADEGOOD	7

/*
 * Ownership and Control Overlays
 */

/*
 * HI_OWNEDBY - Ownership Status Highlighting
 * Highlights sectors based on current ownership or control.
 * Shows which nation or faction controls each area.
 */
#define HI_OWNEDBY	8

/*
 * HI_INRANGE - Range Analysis Highlighting
 * Highlights sectors within operational range of units or structures.
 * Useful for tactical planning and deployment strategies.
 */
#define HI_INRANGE	9

/*
 * HI_SUPPORTED - Support Status Highlighting
 * Highlights sectors that are logistically supported or supplied.
 * Critical for maintaining extended operations and supply lines.
 */
#define HI_SUPPORTED	10

/*
 * Military Unit Overlays
 */

/*
 * HI_MOVEMENT - Movement Capability Highlighting
 * Highlights sectors based on unit movement capabilities or restrictions.
 * Shows accessible routes and movement costs for selected units.
 */
#define HI_MOVEMENT	11

/*
 * HI_NTNARMY - National Army Highlighting
 * Highlights sectors containing national army units.
 * Distinguishes regular military forces from other unit types.
 */
#define HI_NTNARMY	12

/*
 * HI_NTNNAVY - National Navy Highlighting
 * Highlights sectors containing national naval units.
 * Shows naval presence and maritime control areas.
 */
#define HI_NTNNAVY	13

/*
 * HI_NTNCVN - National Caravan Highlighting
 * Highlights sectors containing national caravan or transport units.
 * Important for trade route monitoring and logistics planning.
 */
#define HI_NTNCVN	14

/*
 * Unit Classification Overlays
 */

/*
 * HI_UTYPE - Unit Type Highlighting
 * Highlights sectors based on specific unit types present.
 * Allows filtering display to show only certain unit categories.
 */
#define HI_UTYPE	15

/*
 * HI_UCLASS - Unit Class Highlighting
 * Highlights sectors based on unit class or combat role.
 * Distinguishes between infantry, cavalry, siege, etc.
 */
#define HI_UCLASS	16

/*
 * HI_USTATUS - Unit Status Highlighting
 * Highlights sectors based on unit operational status.
 * Shows ready, damaged, resting, or engaged units.
 */
#define HI_USTATUS	17

/*
 * HI_USIZE - Unit Size Highlighting
 * Highlights sectors based on unit size or strength.
 * Useful for force concentration analysis and threat assessment.
 */
#define HI_USIZE	18

/*
 * HI_NUMBER - Numerical Analysis Highlighting
 * Highlights sectors based on numerical data or calculations.
 * Generic overlay for custom numerical analysis display.
 */
#define HI_NUMBER	19

#endif /* NOTDONE */

/*
 * Global Visibility Data Array Declaration
 *
 * This is the main data structure that stores all visibility and highlighting
 * information for the entire game map. The array is dynamically allocated
 * and contains one long integer for each map sector.
 */

/*
 * visibility_data - Map Visibility and Highlighting Data Array
 *
 * Global array storing visibility and highlighting information for all map
 * sectors. Each element is a long integer with bit-packed data:
 * - Bits 0-3: Visibility level (HS_NOSEE through HS_SEEALL)
 * - Bits 4-31: Highlighting flags for various tactical overlays
 *
 * Array Size:
 *   MAPX * MAPY long integers (where MAPX and MAPY are map dimensions)
 *
 * Memory Layout:
 *   Row-major ordering: sector(x,y) = visibility_data[x + MAPX * y]
 *
 * Access Methods:
 *   - Direct: visibility_data[index]
 *   - Coordinate: _VD_(x,y) macro
 *   - Visibility: VIS_CHECK(x,y) and VIS_STORE(x,y,level)
 *   - Highlighting: HIGH_CHECK(x,y,type), HIGH_SETON(x,y,type), HIGH_SETOFF(x,y,type)
 *
 * Initialization:
 *   Must be allocated dynamically based on map size
 *   Should be initialized to zero (HS_NOSEE visibility, no highlighting)
 *
 * Memory Management:
 *   - Allocated during game initialization
 *   - Size: sizeof(long) * MAPX * MAPY bytes
 *   - Must be freed during game cleanup
 *
 * Thread Safety:
 *   - No inherent thread protection
 *   - Concurrent access requires external synchronization
 *   - Read operations are generally safe if map size is stable
 *
 * Performance Notes:
 *   - Cache-friendly sequential access patterns preferred
 *   - Bit operations are fast on modern processors
 *   - Memory usage: ~4-8 bytes per map sector (depending on architecture)
 */
extern long *visibility_data;
