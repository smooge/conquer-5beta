/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * xferG.h - Transfer System GUI Definitions and Resource Movement Framework
 *
 * Comprehensive transfer system for moving resources, personnel, and military
 * units between different storage locations (cities, navies, caravans, sectors).
 * This header defines the GUI interface framework, transfer site classifications,
 * capacity management, and data structures for safe resource movement operations.
 *
 * Key Components:
 *   - Transfer site type definitions (cities, navies, caravans, sectors)
 *   - Transfer quantity control constants (unlimited, joined operations)
 *   - GUI positioning and layout constants for transfer interface
 *   - XFER_STRUCT data structure for transfer operation management
 *
 * Integration Notes:
 *   - Used by: xferG.c (transfer interface), resource management modules
 *   - Depends on: dataX.h (basic types), item system, map system
 *   - Provides: Resource transfer framework, GUI layout, capacity management
 *
 * Modernization Considerations:
 *   - Transfer limits may need adjustment for larger economies
 *   - GUI positioning could benefit from dynamic layout calculation
 *   - Structure could use const qualifiers for read-only fields
 *   - Transfer validation needs bounds checking and error handling
 */

/* ============================================================================
 * TRANSFER SITE TYPE DEFINITIONS
 * ============================================================================
 * Purpose: Classification of storage locations for resource transfer operations
 * Usage: Identifies source and destination types in transfer operations
 * Notes: Each site type has different capacity limits and transfer rules
 */

/*
 * XFER_CITY - City transfer site classification
 *
 * Identifies cities as transfer locations for resource storage and
 * distribution operations. Cities typically have large storage capacity
 * and serve as major economic hubs for resource management.
 *
 * Value: 0 (city site type)
 * Units: N/A (site classification)
 * Usage: site_class field in XFER_STRUCT for city identification
 * Notes: Cities have highest storage capacity and broadest transfer capabilities
 */
#define XFER_CITY	0

/*
 * XFER_NAVY - Naval fleet transfer site classification
 *
 * Identifies naval fleets as mobile transfer locations for maritime
 * resource transportation and supply operations. Navies provide
 * waterborne logistics and strategic resource positioning.
 *
 * Value: 1 (naval site type)
 * Units: N/A (site classification)
 * Usage: site_class field in XFER_STRUCT for naval fleet identification
 * Notes: Mobile storage with moderate capacity, enables water-based logistics
 */
#define XFER_NAVY	1

/*
 * XFER_CARAVAN - Trade caravan transfer site classification
 *
 * Identifies trade caravans as mobile transfer locations for overland
 * commerce and resource transportation. Caravans enable flexible
 * trade routes and economic distribution networks.
 *
 * Value: 2 (caravan site type)
 * Units: N/A (site classification)
 * Usage: site_class field in XFER_STRUCT for caravan identification
 * Notes: Mobile storage with specialized trade capabilities and route flexibility
 */
#define XFER_CARAVAN	2

/*
 * XFER_SECTOR - Map sector transfer site classification
 *
 * Identifies map sectors as transfer locations for territorial
 * resource deposits and field storage operations. Sectors provide
 * distributed storage and resource extraction points.
 *
 * Value: 3 (sector site type)
 * Units: N/A (site classification)
 * Usage: site_class field in XFER_STRUCT for sector identification
 * Notes: Distributed storage with location-specific access and capacity limits
 */
#define XFER_SECTOR	3

/* ============================================================================
 * TRANSFER QUANTITY CONTROL CONSTANTS
 * ============================================================================
 * Purpose: Special values for transfer quantity and operation control
 * Usage: Controls transfer limits and operational modes
 * Notes: Negative values indicate special transfer conditions
 */

/*
 * XF_NOLIMIT - Unlimited transfer quantity indicator
 *
 * Indicates that transfer operations should not be limited by
 * quantity restrictions, allowing maximum possible resource
 * movement based on available capacity and supply.
 *
 * Value: -1 (unlimited transfer)
 * Units: N/A (special transfer mode)
 * Usage: Transfer quantity parameter for unlimited operations
 * Notes: Subject to capacity limits but not arbitrary quantity restrictions
 */
#define XF_NOLIMIT	-1

/*
 * XF_JOINED - Joined transfer operation indicator
 *
 * Indicates that transfer operations involve coordinated movement
 * between multiple entities or locations, requiring synchronized
 * resource management and capacity coordination.
 *
 * Value: -2 (joined operation mode)
 * Units: N/A (special transfer mode)
 * Usage: Transfer operation parameter for coordinated transfers
 * Notes: Enables complex multi-site transfer operations with synchronization
 */
#define XF_JOINED	-2

/* ============================================================================
 * GUI POSITIONING AND LAYOUT CONSTANTS
 * ============================================================================
 * Purpose: User interface layout parameters for transfer display
 * Usage: Controls visual positioning and spacing in transfer interface
 * Notes: Fixed layout values for consistent transfer GUI presentation
 */

/*
 * XF_INDENT - Transfer interface indentation spacing
 *
 * Horizontal spacing for transfer interface elements, providing
 * visual hierarchy and alignment for transfer site listings
 * and resource quantity displays.
 *
 * Value: 11 (character positions)
 * Units: Character columns (text interface spacing)
 * Usage: GUI layout calculation for transfer interface alignment
 * Notes: Ensures consistent visual hierarchy in transfer displays
 */
#define XF_INDENT	11

/*
 * XF_SPACING - Transfer interface vertical spacing
 *
 * Vertical spacing between transfer interface elements, controlling
 * line spacing for transfer site listings and resource quantity
 * displays in the transfer GUI.
 *
 * Value: 8 (line spacing units)
 * Units: Text lines (vertical interface spacing)
 * Usage: GUI layout calculation for transfer interface line spacing
 * Notes: Balances information density with visual clarity
 */
#define XF_SPACING	8

/*
 * XF_USEDLINES - Transfer interface reserved line count
 *
 * Number of display lines reserved for transfer interface elements,
 * ensuring adequate space for transfer site information and
 * resource quantity displays within the GUI layout.
 *
 * Value: 10 (reserved lines)
 * Units: Text lines (interface space allocation)
 * Usage: GUI layout calculation for transfer interface space reservation
 * Notes: Prevents interface overflow and ensures complete transfer data display
 */
#define XF_USEDLINES	10

/* ============================================================================
 * TRANSFER OPERATION DATA STRUCTURE
 * ============================================================================
 * Purpose: Complete transfer state management and capacity tracking
 * Usage: Stores all transfer operation data and constraints
 * Notes: Central data structure for transfer interface and operations
 */

/*
 * struct s_xfer - Transfer operation data structure
 *
 * Complete data structure for managing transfer operations between
 * storage sites, tracking resources, personnel, capacity limits,
 * and operational constraints for safe and efficient transfers.
 *
 * Usage Patterns:
 *   - Transfer operation initialization and state management
 *   - Capacity validation and resource movement coordination
 *   - GUI display data for transfer interface presentation
 *
 * Relationships:
 *   - Integrates with item system (MTRLS_NUMBER materials)
 *   - Connects to map system (xloc, yloc positioning)
 *   - Links to entity system (army and caravan ID tracking)
 *
 * Fields:
 *   description - Human-readable site identification and status
 *   site_class - Transfer site type (XFER_CITY through XFER_SECTOR)
 *   xloc, yloc - Map coordinates for site location and positioning
 *   mtrls - Material inventory array for resource storage tracking
 *   civies - Civilian population count for personnel transfers
 *   crews - Crew member count for operational personnel transfers
 *   onb_id - Army entity ID for military unit transport tracking
 *   onb_cid - Caravan entity ID for trade unit transport tracking
 *   mtrl_max - Maximum material storage capacity limitation
 *   civ_max - Maximum civilian population capacity limitation
 *   onb_max - Maximum military unit transport capacity limitation
 *   onb_cmax - Maximum caravan transport capacity limitation
 *   crew_max - Maximum crew member capacity limitation
 *   civ_div - Atomic transfer unit size for civilian operations
 *   crew_div - Atomic transfer unit size for crew operations
 */
typedef struct s_xfer {
    char description[LINELTH];    /* Site identification and status description */
    uns_char site_class;          /* Transfer site type classification (XFER_*) */
    maptype xloc, yloc;           /* Map coordinates for site positioning */
    itemtype mtrls[MTRLS_NUMBER]; /* Material storage array for resource tracking */
    long civies;                  /* Civilian population count for personnel transfers */
    long crews;                   /* Crew member count for operational personnel */
    idtype onb_id;                /* Army entity ID for military unit transport */
    idtype onb_cid;               /* Caravan entity ID for trade unit transport */
    long mtrl_max;                /* Maximum material storage capacity limit */
    long civ_max;                 /* Maximum civilian population capacity limit */
    long onb_max;                 /* Maximum military unit transport capacity */
    long onb_cmax;                /* Maximum caravan transport capacity limit */
    long crew_max;                /* Maximum crew member capacity limitation */
    int civ_div;                  /* Atomic transfer unit size for civilians */
    int crew_div;                 /* Atomic transfer unit size for crew */
} XFER_STRUCT, *XFER_PTR;
