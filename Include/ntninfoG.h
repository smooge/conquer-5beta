/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * ntninfoG.h - Nation Information Screen Framework
 *
 * This header defines the complete nation information display system for the
 * game interface. It provides a comprehensive framework for displaying and
 * editing nation statistics, attributes, and administrative data through a
 * structured screen interface with sophisticated permission and formatting
 * controls.
 *
 * Key Components:
 *   - Screen layout constants defining positioning and spacing
 *   - Permission flags controlling visibility and editability
 *   - Nation information types covering all game statistics
 *   - Flexible data union supporting multiple value types
 *   - Item structure linking data to interface functionality
 *   - Screen structure managing display positioning
 *
 * Architecture Overview:
 *   The system uses a data-driven approach where each displayable nation
 *   attribute is defined as an NI_ITEM structure containing the data pointer,
 *   formatting information, permission flags, and associated input/output
 *   functions. The NI_SCREEN structures manage the actual screen positioning
 *   and layout of these items.
 *
 * Permission System:
 *   - NI_GODSEE: Items visible only to god-level users
 *   - NI_GOD/NI_PLAYER: Edit permissions for different user levels
 *   - Dynamic visibility and editability checking through macros
 *
 * Data Type Support:
 *   - String data (nation names, descriptions)
 *   - Integer values (statistics, counts)
 *   - Item type references (specialized game objects)
 *   - Short integers (efficiency values)
 *   - Long integers (large numeric values)
 *
 * Integration Notes:
 *   - Used by: ntninfoG.c for nation information display interface
 *   - Depends on: header.h for basic types and LINES constant
 *   - Provides: Complete nation information screen framework
 *   - Interfaces: Integrates with user permission system and data structures
 *
 * Modernization Considerations:
 *   - Function pointer typedef uses legacy conditional compilation
 *   - Data union could benefit from tagged union pattern
 *   - Screen positioning system could use dynamic layout
 *   - Permission flags could be converted to enum for type safety
 *   - Item array management could use dynamic allocation
 */

/* ============================================================================
 * SCREEN LAYOUT CONSTANTS - Nation Information Display Positioning
 * ============================================================================
 * Purpose: Define screen geometry and spacing for nation information display
 * Usage: Used by display functions to calculate screen positions and layout
 * Notes: Layout assumes standard terminal size with LINES rows available
 */

/*
 * NI_BOTTOM - Bottom margin reserved space
 *
 * Number of lines reserved at the bottom of the screen for status messages,
 * command prompts, or other interface elements that should not be overwritten
 * by the nation information display.
 *
 * Value: 3 lines
 * Usage: Subtracted from total screen height to determine usable display area
 * Notes: Provides space for command prompt and status information
 */
#define NI_BOTTOM	3

/*
 * NI_TITLE - Title area reserved space
 *
 * Number of lines reserved at the top of the screen for the nation information
 * screen title and header information.
 *
 * Value: 2 lines
 * Usage: Added to line calculations to position content below title area
 * Notes: Accommodates title text and decorative elements
 */
#define NI_TITLE	2

/*
 * NI_INDENT - Horizontal indentation for content
 *
 * Number of columns to indent nation information content from the left edge
 * of the screen for visual formatting and readability.
 *
 * Value: 1 column
 * Usage: Added to column positions to provide left margin
 * Notes: Minimal indentation for clean left alignment
 */
#define NI_INDENT	1

/*
 * NI_SPACING - Vertical spacing between content lines
 *
 * Number of lines to space between different nation information items for
 * visual separation and improved readability.
 *
 * Value: 1 line
 * Usage: Added between displayed items to create visual separation
 * Notes: Provides clean separation without excessive whitespace
 */
#define NI_SPACING	1

/*
 * NI_MAXHEIGHT - Maximum usable display height
 *
 * Calculated maximum number of lines available for displaying nation
 * information content, accounting for reserved screen areas.
 *
 * Value: (LINES - NI_BOTTOM - NI_TITLE)
 * Usage: Used to limit content display and implement scrolling if needed
 * Notes: Dynamically calculated based on terminal size
 */
#define NI_MAXHEIGHT	(LINES - NI_BOTTOM - NI_TITLE)

/* ============================================================================
 * ITEM ATTRIBUTE FLAGS - Nation Information Display and Permission Control
 * ============================================================================
 * Purpose: Bit flags controlling item visibility, editability, and formatting
 * Usage: Combined in NI_ITEM.stats field to specify item behavior
 * Notes: Uses bit shifting for efficient flag combination and testing
 */

/*
 * NI_GOD - God-level edit permission
 *
 * Flag indicating that this nation information item can be edited by
 * users with god-level privileges. Items with this flag allow administrative
 * modification of game parameters.
 *
 * Value: (1<<1) = 0x02
 * Usage: Combined with other flags in stats field for permission checking
 * Notes: Provides administrative control over critical game parameters
 */
#define NI_GOD		(1<<1)

/*
 * NI_PLAYER - Player-level edit permission
 *
 * Flag indicating that this nation information item can be edited by
 * regular players. Items with this flag allow player customization of
 * their nation's editable attributes.
 *
 * Value: (1<<2) = 0x04
 * Usage: Combined with other flags for user permission validation
 * Notes: Enables player control over appropriate nation characteristics
 */
#define NI_PLAYER	(1<<2)

/*
 * NI_GODSEE - God-only visibility
 *
 * Flag indicating that this nation information item is visible only to
 * users with god-level privileges. Hidden information for administrative
 * oversight and debugging purposes.
 *
 * Value: (1<<3) = 0x08
 * Usage: Checked by visibility macros to hide sensitive information
 * Notes: Provides administrative transparency for game management
 */
#define NI_GODSEE	(1<<3)

/*
 * NI_PERCENT - Percentage value formatting
 *
 * Flag indicating that this numeric value should be displayed with a
 * trailing percent character (%) for percentage values and rates.
 *
 * Value: (1<<4) = 0x10
 * Usage: Used by output functions to append percentage formatting
 * Notes: Automatically formats decimal values as percentages
 */
#define NI_PERCENT	(1<<4)

/*
 * NI_DOLLAR - Currency value formatting
 *
 * Flag indicating that this numeric value should be displayed with a
 * leading dollar character ($) for monetary values and prices.
 *
 * Value: (1<<5) = 0x20
 * Usage: Used by output functions to prepend currency formatting
 * Notes: Formats values as currency amounts for economic displays
 */
#define NI_DOLLAR	(1<<5)

/*
 * NI_BOLD - Emphasized display formatting
 *
 * Flag indicating that this nation information item should be displayed
 * with emphasis (bold or highlighted) to draw attention to important
 * values or critical information.
 *
 * Value: (1<<6) = 0x40
 * Usage: Used by display functions to apply visual emphasis
 * Notes: Highlights critical or notable nation characteristics
 */
#define NI_BOLD		(1<<6)

/*
 * NI_EMPTY - Blank line spacer
 *
 * Flag indicating that this item represents an empty line used for
 * visual spacing and organization in the nation information display.
 *
 * Value: (1<<7) = 0x80
 * Usage: Recognized by display functions to render blank spacing lines
 * Notes: Provides visual organization and content grouping
 */
#define NI_EMPTY	(1<<7)

/*
 * NI_NOSPACE - Space-restricted string
 *
 * Flag indicating that string values for this item cannot contain
 * spaces and should be validated accordingly during input.
 *
 * Value: (1<<8) = 0x100
 * Usage: Used by input validation to reject space characters
 * Notes: Enforces formatting constraints for identifiers and codes
 */
#define NI_NOSPACE	(1<<8)

/*
 * NI_BONUS - Special bonus value handling
 *
 * Flag indicating that this item requires special processing for
 * bonus values, modifiers, or derived calculations.
 *
 * Value: (1<<9) = 0x200
 * Usage: Triggers special calculation or display logic for bonus values
 * Notes: Handles complex derived values and modifier calculations
 */
#define NI_BONUS	(1<<9)

/*
 * NI_BOUNDARY - Map boundary coordinate handling
 *
 * Flag indicating that this item represents map boundary coordinates
 * and requires special formatting and validation for geographic edges.
 *
 * Value: (1<<10) = 0x400
 * Usage: Triggers boundary-specific validation and display formatting
 * Notes: Handles world map edge coordinates and geographic constraints
 */
#define NI_BOUNDARY	(1<<10)

/* ============================================================================
 * NATION INFORMATION ITEM TYPES - Complete Display Element Catalog
 * ============================================================================
 * Purpose: Unique identifiers for each displayable nation information element
 * Usage: Used as item identifiers in NI_ITEM structures and display arrays
 * Notes: Sequential numbering with gaps for logical grouping and blank lines
 */

/* Basic Nation Identity Section */
#define NIT_BLANK	0	/* Empty line for visual spacing */
#define NIT_NATION	1	/* Nation name and title */
#define NIT_MARK	2	/* Nation mark/symbol identifier */
#define NIT_LEADER	3	/* Current leader name */
#define NIT_RACE	4	/* Nation's racial type */
#define NIT_ALIGN	5	/* Alignment (good/neutral/evil) */
#define NIT_CLASS	6	/* Nation class type */
#define NIT_AGGRESS	7	/* Aggression level setting */
#define NIT_NPCSTAT	8	/* NPC status and control */
#define NIT_LOGIN	9	/* Last login timestamp */
/* Element 10 reserved for blank line */

/* Economic Policy Section */
#define NIT_TAXRATE	11	/* Current taxation rate */
#define NIT_CHARITY	12	/* Charity spending level */
#define NIT_CURRENCY	13	/* Available currency amount */
#define NIT_INFLATION	14	/* Current inflation rate */
/* Element 15 reserved for attributes title */

/* Nation Attributes Section */
#define NIT_COMMRANGE	16	/* Communication range capability */
#define NIT_EATRATE	17	/* Food consumption rate */
#define NIT_HEALTH	18	/* Overall national health */
#define NIT_JEWELWORK	19	/* Jewelry crafting skill */
#define NIT_KNOWLEDGE	20	/* Scientific knowledge level */
#define NIT_MERCREP	21	/* Merchant reputation rating */
#define NIT_METALWORK	22	/* Metalworking skill level */
#define NIT_MINING	23	/* Mining capability and efficiency */
#define NIT_MORALE	24	/* National morale level */
#define NIT_POPULARITY	25	/* Popular support rating */
#define NIT_REPUTATION	26	/* Overall diplomatic reputation */
#define NIT_SPELLPTS	27	/* Available spell points */
#define NIT_SPOILRATE	28	/* Food spoilage rate */
#define NIT_TERROR	29	/* Terror/fear level */
#define NIT_WIZSKILL	30	/* Wizard skill proficiency */
/* Element 31 reserved for blank line */

/* Military Capabilities Section */
#define NIT_ATTACK	32	/* Military attack strength */
#define NIT_DEFENSE	33	/* Defensive capability rating */
#define NIT_REPRO	34	/* Unit reproduction rate */
#define NIT_MOVE	35	/* Movement speed modifier */
/* Element 36 reserved for blank line */

/* Resource Inventory Section */
#define NIT_TALONS	37	/* Currency/talon reserves */
#define NIT_JEWELS	38	/* Jewel inventory count */
#define NIT_METALS	39	/* Metal resource stockpile */
#define NIT_FOOD	40	/* Food supply reserves */
#define NIT_WOOD	41	/* Timber resource inventory */
/* Element 42 reserved for blank line */

/* Population and Assets Section */
#define NIT_LEADERS	43	/* Number of leaders available */
#define NIT_SOLDIERS	44	/* Total military personnel */
#define NIT_CIVILIANS	45	/* Civilian population count */
#define NIT_MONSTERS	46	/* Monster units controlled */
#define NIT_SHIPS	47	/* Naval fleet size */
#define NIT_WAGONS	48	/* Caravan wagon count */
#define NIT_SECTORS	49	/* Controlled territory sectors */
#define NIT_SCORE	50	/* Overall nation score */
/* Element 51 reserved for god title */

/* Administrative/Geographic Section (God-only) */
#define NIT_CAPCOORD	52	/* Capital city coordinates */
#define NIT_CENTERCOORD	53	/* Nation center coordinates */
#define NIT_LEFTEDGE	54	/* Western boundary coordinate */
#define NIT_RIGHTEDGE	55	/* Eastern boundary coordinate */
#define NIT_TOPEDGE	56	/* Northern boundary coordinate */
#define NIT_BOTTOMEDGE	57	/* Southern boundary coordinate */

/*
 * NIT_NUMBER - Total count of nation information elements
 *
 * Total number of defined nation information display elements, used for
 * array sizing and bounds checking in display and processing functions.
 *
 * Value: 58 elements
 * Usage: Array allocation and iteration bounds for nation info processing
 * Notes: Includes both data elements and blank spacer lines
 */
#define NIT_NUMBER	58

/* ============================================================================
 * VISIBILITY AND PERMISSION MACROS - Dynamic Access Control
 * ============================================================================
 * Purpose: Runtime checking of item visibility and edit permissions
 * Usage: Used throughout nation info interface for access control
 * Notes: Depends on global is_god flag and item permission flags
 */

/*
 * NI_CANSEE(x) - Check if current user can see screen item
 *
 * Macro that determines whether the current user has permission to view
 * a specific nation information screen item based on their privilege level
 * and the item's visibility flags.
 *
 * Parameters:
 *   x - Screen item index to check visibility for
 *
 * Returns:
 *   TRUE if item is visible to current user, FALSE if hidden
 *
 * Logic:
 *   - God users can see all items regardless of flags
 *   - Regular users cannot see items marked with NI_GODSEE
 *
 * Usage: if (NI_CANSEE(item_index)) { display_item(); }
 * Notes: Accesses ni_screen array and requires valid screen item index
 */
#define NI_CANSEE(x) (is_god ? TRUE : !(ni_screen[x].item->stats & NI_GODSEE))

/*
 * NI_ICANSEE(x) - Check if current user can see raw item
 *
 * Macro that determines whether the current user has permission to view
 * a specific nation information item directly from the items array,
 * bypassing the screen structure.
 *
 * Parameters:
 *   x - Item array index to check visibility for
 *
 * Returns:
 *   TRUE if item is visible to current user, FALSE if hidden
 *
 * Logic:
 *   - God users can see all items regardless of flags
 *   - Regular users cannot see items marked with NI_GODSEE
 *
 * Usage: if (NI_ICANSEE(item_id)) { process_item(); }
 * Notes: Accesses items array directly, requires valid item index
 */
#define NI_ICANSEE(x) (is_god ? TRUE : !(items[x].stats & NI_GODSEE))

/*
 * NI_EDITABLE(x) - Check if current user can edit screen item
 *
 * Comprehensive macro that determines whether the current user has permission
 * to edit a specific nation information screen item, including bounds checking,
 * visibility verification, and edit permission validation.
 *
 * Parameters:
 *   x - Screen item index to check edit permissions for
 *
 * Returns:
 *   TRUE if item is editable by current user, FALSE if read-only or hidden
 *
 * Logic:
 *   1. Bounds check: x must be valid array index (0 <= x < numitems)
 *   2. Visibility check: User must be able to see the item (NI_CANSEE)
 *   3. Permission check: Item must have appropriate edit flags
 *      - God users: Can edit items with NI_GOD or NI_PLAYER flags
 *      - Regular users: Can only edit items with NI_PLAYER flag
 *
 * Usage: if (NI_EDITABLE(item_index)) { allow_editing(); }
 * Notes: Comprehensive safety check combining bounds, visibility, and permissions
 */
#define NI_EDITABLE(x)	(((x >= 0) && (x < numitems) && NI_CANSEE(x)) ? \
			 (is_god ? \
			  (ni_screen[x].item->stats & (NI_GOD | NI_PLAYER)):\
			  (ni_screen[x].item->stats & NI_PLAYER)) : FALSE)

/* ============================================================================
 * FUNCTION POINTER TYPES - Input/Output Function Interface
 * ============================================================================
 * Purpose: Define function pointer types for item input and output handlers
 * Usage: Used in NI_ITEM structure for input/output function callbacks
 * Notes: Conditional compilation for ANSI C compatibility
 */

/*
 * FNCV_NI - Nation information function pointer type
 *
 * Function pointer type definition for input and output handler functions
 * used in the nation information system. These functions handle the
 * specialized input validation and output formatting for different types
 * of nation information items.
 *
 * Function Signature:
 *   - Takes no parameters (void)
 *   - Returns no value (void)
 *   - Uses global state and context for operation
 *
 * Usage Pattern:
 *   - inp_func: Handles user input validation and processing
 *   - out_func: Handles formatted output display generation
 *
 * Implementation Notes:
 *   - ANSI C version: void (*FNCV_NI)(void) - explicit void parameters
 *   - K&R C version: void (*FNCV_NI)() - implicit parameters
 *   - Functions access global variables for context and data
 *
 * Modernization Notes:
 *   - Could be enhanced with context parameters for thread safety
 *   - Consider separate types for input vs output functions
 *   - Modern versions could use more specific parameter types
 */
#ifdef __STDC__
typedef void (*FNCV_NI)(void);		/* ANSI C: explicit void parameters */
#else
typedef void (*FNCV_NI)();		/* K&R C: implicit parameter list */
#endif /* FNCV_NI */

/* ============================================================================
 * DATA UNION TYPE - Flexible Nation Information Data Storage
 * ============================================================================
 * Purpose: Union for storing pointers to different data types in nation items
 * Usage: Used in NI_ITEM structure to provide flexible data access
 * Notes: All members are pointers to allow dynamic data binding
 */

/*
 * union u_ndata - Flexible data pointer union for nation information
 *
 * Union that provides a flexible way to store pointers to different data
 * types used in the nation information system. This allows a single item
 * structure to handle different types of nation data (strings, integers,
 * special types) through a common interface.
 *
 * Design Pattern:
 *   The union stores pointers rather than values directly, allowing the
 *   nation information items to reference actual game data structures
 *   without copying. This provides live data binding where changes to
 *   the underlying game state are immediately reflected in the display.
 *
 * Data Type Support:
 *   - String data: Nation names, descriptions, text fields
 *   - Integer data: Counts, statistics, numeric values
 *   - Item types: Special game objects with complex data
 *   - Short integers: Efficiency values, small numeric ranges
 *   - Long integers: Large values like population, resources
 *
 * Memory Management:
 *   Union does not own the pointed-to data; lifetime management is handled
 *   by the game's data structures. The union provides access without
 *   creating additional memory allocation responsibilities.
 *
 * Fields:
 *   p_string - Pointer to character string data (names, descriptions)
 *   p_int - Pointer to integer value (counts, statistics)
 *   p_itemv - Pointer to itemtype structure (complex game objects)
 *   p_short - Pointer to short integer (efficiency values, small ranges)
 *   p_longint - Pointer to long integer (large values, populations)
 *
 * Modernization Considerations:
 *   - Could use tagged union pattern for type safety
 *   - Consider generic void* with separate type indicator
 *   - Modern C could use _Generic for type-safe access
 */
typedef union u_ndata {
  char *p_string;	/* Pointer to string data (nation names, descriptions) */
  int *p_int;		/* Pointer to integer data (counts, basic statistics) */
  itemtype *p_itemv;	/* Pointer to item type data (complex game objects) */
  short *p_short;	/* Pointer to short integer data (efficiency values) */
  long *p_longint;	/* Pointer to long integer data (populations, resources) */
} NI_UNION, *NI_UNPTR;

/* ============================================================================
 * NATION INFORMATION ITEM STRUCTURE - Complete Item Definition
 * ============================================================================
 * Purpose: Complete definition for displayable/editable nation information items
 * Usage: Array of these structures defines the entire nation info interface
 * Notes: Links display metadata with actual game data through flexible design
 */

/*
 * struct s_nitem - Complete nation information item specification
 *
 * Structure that defines a complete nation information display item, including
 * all metadata needed for display formatting, user interaction, data validation,
 * and connection to the underlying game data. This structure forms the core
 * of the data-driven nation information interface system.
 *
 * Design Philosophy:
 *   Each item encapsulates all information needed to display and edit a
 *   specific piece of nation data. The structure separates presentation
 *   logic (labels, formatting) from data access (union pointer) and
 *   behavior (function pointers), enabling flexible and maintainable
 *   interface construction.
 *
 * Data Flow:
 *   1. Display: out_func reads data via it.* pointer and formats using metadata
 *   2. Input: inp_func validates user input and updates data via it.* pointer
 *   3. Validation: minval/maxval provide bounds checking for numeric inputs
 *   4. Permission: stats flags control visibility and editability
 *
 * Usage Pattern:
 *   Items are typically organized in arrays corresponding to screen layout,
 *   with each item fully self-contained for its display and editing behavior.
 *
 * Fields:
 *   label - Display label shown to user on screen
 *   ident - Unique identifier matching NIT_* constants
 *   desc - Detailed description for help or documentation
 *   prompt - User prompt text for input requests
 *   emsg - Error message text for validation failures
 *   stats - Bit flags controlling permissions and formatting (NI_* flags)
 *   minval - Minimum acceptable value or string length for validation
 *   maxval - Maximum acceptable value or string length for validation
 *   modifier - Additional numeric modifier for calculations or adjustments
 *   inp_func - Function pointer for handling user input and validation
 *   out_func - Function pointer for generating formatted display output
 *   it - Data union pointing to actual game data for this item
 *
 * Modernization Considerations:
 *   - Could use const char* for read-only string fields
 *   - Function pointers could include context parameters
 *   - Stats field could use enum flags for type safety
 *   - Could separate input/output into different structure types
 */
typedef struct s_nitem {
  char *label;		/* Display label text shown on screen */
  int ident;		/* Unique identifier (matches NIT_* constants) */
  char *desc;		/* Detailed description for help/documentation */
  char *prompt;		/* User prompt message for input requests */
  char *emsg;		/* Error message text for validation failures */
  long stats;		/* Permission and formatting flags (NI_* bit flags) */
  int minval;		/* Minimum acceptable value or string length */
  int maxval;		/* Maximum acceptable value or string length */
  int modifier;		/* Additional numeric modifier for calculations */
  FNCV_NI inp_func;	/* Input handler function pointer */
  FNCV_NI out_func;	/* Output formatter function pointer */
  NI_UNION it;		/* Data union pointing to actual game data */
} NI_ITEM, *NI_ITEMPTR;

/* ============================================================================
 * SCREEN POSITION STRUCTURE - Display Layout Management
 * ============================================================================
 * Purpose: Manages screen positioning and layout for nation information items
 * Usage: Array of these structures defines the actual screen layout
 * Notes: Links display items to specific screen coordinates
 */

/*
 * struct s_niscr - Screen position and item reference structure
 *
 * Structure that manages the positioning and layout of nation information
 * items on the display screen. Each structure associates a specific item
 * with its screen coordinates, enabling flexible layout management and
 * cursor navigation through the nation information interface.
 *
 * Design Purpose:
 *   Separates logical item definitions (NI_ITEM) from physical screen
 *   layout (NI_SCREEN), allowing the same items to be arranged in
 *   different layouts or screen configurations without modifying the
 *   item definitions themselves.
 *
 * Layout Management:
 *   The screen structures form an array that defines the complete visual
 *   layout of the nation information display. Items can be positioned
 *   at arbitrary screen coordinates, enabling multi-column layouts,
 *   grouped sections, and flexible spacing arrangements.
 *
 * Navigation Support:
 *   Screen coordinates enable cursor movement and input focus management,
 *   allowing users to navigate between editable items using keyboard
 *   navigation or mouse interaction.
 *
 * Usage Pattern:
 *   1. Item array defines what data is available and how it behaves
 *   2. Screen array defines where each item appears on the display
 *   3. Display functions iterate through screen array for rendering
 *   4. Input functions use coordinates for focus and cursor management
 *
 * Fields:
 *   col - Horizontal screen column position (0-based)
 *   line - Vertical screen line position (0-based, adjusted for title area)
 *   item - Pointer to the associated nation information item structure
 *
 * Coordinate System:
 *   - Column 0 is leftmost screen position
 *   - Line 0 is adjusted for title area (actual screen line = line + NI_TITLE)
 *   - Coordinates are in character cell units for text-mode display
 *
 * Modernization Considerations:
 *   - Could include width/height for multi-cell items
 *   - Might benefit from separate layout calculation functions
 *   - Could support dynamic layouts with percentage-based positioning
 */
typedef struct s_niscr {
  int col, line;	/* Screen position: column and line coordinates */
  NI_ITEMPTR item;	/* Pointer to the nation information item for this position */
} NI_SCREEN, *NI_SCRPTR;
