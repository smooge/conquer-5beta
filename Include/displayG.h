/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * Include/displayG.h - Display System Definitions and Screen Management
 *
 * This header defines the complete display and user interface system including
 * screen layout management, display mode configurations, sector status tracking,
 * and coordinate translation for hexagonal and rectangular map displays.
 *
 * Key Systems:
 * - Multi-zoom display modes with configurable screen layouts
 * - Hexagonal and rectangular map display support  
 * - Sector status tracking for unit and movement visualization
 * - Dynamic screen sizing based on terminal capabilities
 * - Information mode vs. game mode display switching
 * - Coordinate translation and offset management
 *
 * Display Architecture:
 * - Screen structure defines display characteristics per zoom level
 * - Base display modes provide 21 different visualization options
 * - Sector status system tracks troops, movement, and reconnaissance
 * - Coordinate macros provide efficient map access and status updates
 *
 * Performance Notes: Display operations are frequently called during map
 * updates and user interaction, requiring efficient coordinate calculations
 * and status checking.
 *
 * Modernization: Consider GPU acceleration for large maps and modern GUI
 * framework integration for enhanced user interface capabilities.
 */
#include "displayX.h"

/*
 * Display System Data Structures
 */

/*
 * SCREEN_STRUCT - Screen layout and display characteristics
 *
 * Defines the visual layout characteristics for different display modes and
 * zoom levels. This structure controls how map sectors are rendered on screen,
 * including spacing, sizing, and visual alignment for both hexagonal and
 * rectangular map display modes.
 *
 * Fields:
 *   xshift     - Horizontal character offset between adjacent sectors
 *   yshift     - Vertical character offset between adjacent sectors  
 *   xsize      - Display width (in characters) of a single map sector
 *   ysize      - Display height (in characters) of a single map sector
 *   oddlift    - Vertical offset for odd columns in hexagonal displays
 *   has_border - Flag indicating if this display mode includes borders
 *
 * Display Types:
 * - Hexagonal maps use oddlift for proper hex grid alignment
 * - Rectangular maps use standard grid positioning
 * - Different zoom levels require different spacing and sizing
 * - Border modes add visual separation between sectors
 *
 * Usage: Screen rendering, coordinate translation, display mode switching
 * Integration: Used with base_modes[] and zoom level management
 * Performance: Critical for real-time map display updates
 * Modernization: Consider vector graphics for scalable display modes
 */
typedef struct s_screen {
  int xshift;		/* characters shifted when going horizonally */
  int yshift;		/* characters shifted when going vertically */
  int xsize;		/* width of one sector on the display */
  int ysize;		/* height of one sector on the display */
  int oddlift;		/* y offset for odd columns */
  int has_border;	/* is there a border on this setting? */
} SCREEN_STRUCT, *SCREEN_PTR;

/*
 * Screen Layout and Sizing Constants
 *
 * These constants define the screen real estate allocation for different
 * interface modes and components, enabling dynamic layout based on 
 * information display requirements and terminal capabilities.
 */

/*
 * SCREEN_RIGHT - Right panel width in normal game mode
 *
 * Defines the width of the right information panel when operating in
 * standard game mode (not information mode). Used for status displays,
 * unit information, and command interfaces.
 *
 * Value: 20 characters for right panel
 * Usage: Normal game mode layout calculations
 */
#define SCREEN_RIGHT	20

/*
 * SCREEN_MIDDLE - Right panel width in information mode  
 *
 * Expanded width for the right information panel when operating in
 * detailed information mode, providing more space for comprehensive
 * data display and analysis.
 *
 * Value: 50 characters for expanded information panel
 * Usage: Information mode layout calculations
 */
#define SCREEN_MIDDLE	50

/*
 * SCREEN_BOTTOM - Bottom panel height in normal game mode
 *
 * Defines the height of the bottom status and command panel during
 * standard game operations, providing space for essential information
 * without overwhelming the map display.
 *
 * Value: 5 lines for bottom panel  
 * Usage: Normal game mode vertical layout
 */
#define SCREEN_BOTTOM	5

/*
 * SCREEN_EXTBTM - Extended bottom panel height in information mode
 *
 * Expanded height for the bottom panel when operating in information
 * mode, allowing for detailed status displays, command histories,
 * and comprehensive game state information.
 *
 * Value: 12 lines for expanded bottom panel
 * Usage: Information mode vertical layout
 */
#define SCREEN_EXTBTM	12

/*
 * SCREEN_BTMLIM - Dynamic bottom panel height based on mode
 *
 * Macro that selects appropriate bottom panel height based on current
 * information mode state, enabling dynamic interface layout switching.
 *
 * Logic: Extended height in info mode, normal height otherwise
 * Usage: Dynamic vertical layout calculations
 */
#define SCREEN_BTMLIM	(conq_infomode?SCREEN_EXTBTM:SCREEN_BOTTOM)

/*
 * SCREEN_RGTLIM - Dynamic right panel width based on mode
 *
 * Macro that selects appropriate right panel width based on current
 * information mode state, enabling dynamic interface layout switching.
 *
 * Logic: Extended width in info mode, normal width + 1 otherwise
 * Usage: Dynamic horizontal layout calculations
 */
#define SCREEN_RGTLIM	(conq_infomode?SCREEN_MIDDLE:(SCREEN_RIGHT+1))

/*
 * SCREEN_X_SIZE - Available map display width
 *
 * Calculates the actual horizontal screen space available for map display
 * after accounting for right panel space and interface borders.
 *
 * Calculation: Total columns minus right panel and border space
 * Usage: Map display area determination, horizontal scrolling limits
 */
#define SCREEN_X_SIZE	(COLS - (SCREEN_RGTLIM + 1))

/*
 * SCREEN_Y_SIZE - Available map display height  
 *
 * Calculates the actual vertical screen space available for map display
 * after accounting for bottom panel space and interface borders.
 *
 * Calculation: Total lines minus bottom panel space
 * Usage: Map display area determination, vertical scrolling limits
 */
#define SCREEN_Y_SIZE	(LINES - SCREEN_BTMLIM)

/*
 * Display Mode System Constants
 */

/*
 * DMODE_NUMBER - Total number of base display modes available
 *
 * Defines the total number of different display visualization modes
 * supported by the system, each providing different map representations
 * for various tactical and strategic analysis needs.
 *
 * Value: 21 different display modes available
 * Usage: Display mode array sizing, mode validation, mode cycling
 * Integration: Used with base_modes[] array for mode management
 */
#define DMODE_NUMBER	21

/*
 * Change View Options - Map Visualization Types
 *
 * These constants define different map visualization overlays that can
 * be applied to change how terrain and strategic information is displayed.
 * Each view type emphasizes different aspects of the game world.
 */

/*
 * CV_CONTOUR - Topographic contour display mode
 *
 * Displays elevation and terrain height information using contour lines
 * or color coding, useful for tactical movement planning and strategic
 * positioning based on terrain advantages.
 *
 * Value: 0 (contour visualization)
 * Usage: Terrain analysis, movement planning, tactical positioning
 */
#define CV_CONTOUR	0

/*
 * CV_DESIGNATION - Territory designation display mode
 *
 * Shows political boundaries, territorial control, and administrative
 * designations, useful for understanding political control and planning
 * expansion or diplomatic strategies.
 *
 * Value: 1 (designation visualization)  
 * Usage: Political analysis, territorial planning, diplomatic strategy
 */
#define CV_DESIGNATION	1

/*
 * CV_VEGETATION - Vegetation and resource display mode
 *
 * Displays natural resources, vegetation types, and environmental
 * information, useful for economic planning and resource management
 * strategies.
 *
 * Value: 2 (vegetation visualization)
 * Usage: Resource planning, economic strategy, environmental analysis
 */
#define CV_VEGETATION	2

/*
 * CV_NUMBER - Total number of change view options
 *
 * Defines the total number of different view visualization modes
 * available for map display customization.
 *
 * Value: 3 different change view options
 * Usage: View mode validation, cycling through visualization options
 */
#define CV_NUMBER	3

/*
 * Sector Status Information Flags
 *
 * These bit flags track the status of units and activities within each
 * map sector for efficient display updates and user interface feedback.
 * Multiple flags can be combined using bitwise operations.
 */

/*
 * SSTAT_TROOPS - Sector contains military units
 *
 * Indicates that the sector contains one or more military units
 * (armies, navies, or caravans), triggering appropriate display
 * indicators and interaction options.
 *
 * Value: 0x01 (bit 0)
 * Usage: Unit presence detection, display highlighting, command availability
 */
#define SSTAT_TROOPS	0x01

/*
 * SSTAT_UNMOVED - Sector contains units that haven't moved this turn
 *
 * Flags sectors containing units that still have movement points
 * available and haven't been moved during the current turn, helping
 * players identify units requiring attention.
 *
 * Value: 0x02 (bit 1)
 * Usage: Turn management, movement planning, unit status tracking
 */
#define SSTAT_UNMOVED	0x02

/*
 * SSTAT_MOVABLE - Sector contains units capable of movement
 *
 * Indicates that the sector contains units that are physically
 * capable of movement (not damaged, not in construction, etc.),
 * distinguishing from immobilized or busy units.
 *
 * Value: 0x04 (bit 2)
 * Usage: Movement validation, command filtering, tactical planning
 */
#define SSTAT_MOVABLE	0x04

/*
 * SSTAT_SCOUTS - Sector contains reconnaissance units
 *
 * Flags sectors with units capable of scouting and reconnaissance
 * operations, enabling special intelligence-gathering commands
 * and tactical options.
 *
 * Value: 0x08 (bit 3)
 * Usage: Intelligence operations, reconnaissance planning, tactical commands
 */
#define SSTAT_SCOUTS	0x08

/*
 * Sector Status Access Macros
 *
 * These macros provide efficient 2D coordinate access to sector status
 * arrays, converting (x,y) coordinates to linear array indices for
 * optimal memory access patterns.
 */

/*
 * SECTSTAT(x,y) - Access sector status flags at coordinates (x,y)
 *
 * Converts 2D map coordinates to linear array index for accessing
 * sector status information. Uses row-major order storage with
 * MAPX as the row width.
 *
 * Parameters: x,y - Map coordinates (0-based)
 * Returns: Reference to sector status flags at specified location
 * Usage: Status checking, flag manipulation, display updates
 */
#define SECTSTAT(x,y)	sectstat[(x) + (y) * MAPX]

/*
 * TROOPLOCS(x,y) - Access troop location data at coordinates (x,y)
 *
 * Converts 2D map coordinates to linear array index for accessing
 * troop location information and unit tracking data.
 *
 * Parameters: x,y - Map coordinates (0-based)
 * Returns: Reference to troop location data at specified coordinates
 * Usage: Unit tracking, location validation, troop management
 */
#define TROOPLOCS(x,y)	trooplocs[(x) + (y) * MAPX]

/*
 * Sector Status Query Macros
 *
 * These macros provide convenient boolean tests for sector status
 * flags, enabling efficient conditional logic for display updates
 * and user interface behavior.
 */

/*
 * has_troops(x,y) - Check if sector contains military units
 *
 * Tests whether the specified sector contains any military units
 * by checking the SSTAT_TROOPS flag.
 *
 * Parameters: x,y - Map coordinates to check
 * Returns: Non-zero if sector has troops, 0 otherwise
 * Usage: Display highlighting, command availability, tactical analysis
 */
#define has_troops(x,y)	(SECTSTAT(x,y) & SSTAT_TROOPS)

/*
 * has_unmoved(x,y) - Check if sector contains unmoved units
 *
 * Tests whether the specified sector contains units that haven't
 * moved during the current turn.
 *
 * Parameters: x,y - Map coordinates to check
 * Returns: Non-zero if sector has unmoved units, 0 otherwise
 * Usage: Turn management, movement planning, status indicators
 */
#define has_unmoved(x,y)	(SECTSTAT(x,y) & SSTAT_UNMOVED)

/*
 * has_movable(x,y) - Check if sector contains movable units
 *
 * Tests whether the specified sector contains units that are
 * physically capable of movement operations.
 *
 * Parameters: x,y - Map coordinates to check
 * Returns: Non-zero if sector has movable units, 0 otherwise
 * Usage: Movement validation, command filtering, interface logic
 */
#define has_movable(x,y)	(SECTSTAT(x,y) & SSTAT_MOVABLE)

/*
 * has_scouts(x,y) - Check if sector contains reconnaissance units
 *
 * Tests whether the specified sector contains units capable of
 * scouting and intelligence operations.
 *
 * Parameters: x,y - Map coordinates to check
 * Returns: Non-zero if sector has scouts, 0 otherwise
 * Usage: Intelligence planning, reconnaissance commands, tactical options
 */
#define has_scouts(x,y)	(SECTSTAT(x,y) & SSTAT_SCOUTS)

/*
 * Sector Status Setting Macros
 *
 * These macros provide convenient flag setting operations for
 * updating sector status information during game state changes.
 * Use bitwise OR to preserve existing flags while adding new ones.
 */

/*
 * set_has_troops(x,y) - Mark sector as containing military units
 *
 * Sets the SSTAT_TROOPS flag for the specified sector to indicate
 * the presence of military units.
 *
 * Parameters: x,y - Map coordinates to update
 * Effect: Sets troops flag, preserves other flags
 * Usage: Unit placement, movement tracking, status updates
 */
#define set_has_troops(x,y)	SECTSTAT(x,y) |= SSTAT_TROOPS

/*
 * set_has_unmoved(x,y) - Mark sector as containing unmoved units
 *
 * Sets the SSTAT_UNMOVED flag for the specified sector to indicate
 * units that haven't moved this turn.
 *
 * Parameters: x,y - Map coordinates to update
 * Effect: Sets unmoved flag, preserves other flags
 * Usage: Turn initialization, movement tracking, status management
 */
#define set_has_unmoved(x,y)	SECTSTAT(x,y) |= SSTAT_UNMOVED

/*
 * set_has_movable(x,y) - Mark sector as containing movable units
 *
 * Sets the SSTAT_MOVABLE flag for the specified sector to indicate
 * units capable of movement operations.
 *
 * Parameters: x,y - Map coordinates to update
 * Effect: Sets movable flag, preserves other flags
 * Usage: Unit status updates, movement validation, interface state
 */
#define set_has_movable(x,y)	SECTSTAT(x,y) |= SSTAT_MOVABLE

/*
 * set_has_scouts(x,y) - Mark sector as containing reconnaissance units
 *
 * Sets the SSTAT_SCOUTS flag for the specified sector to indicate
 * units capable of intelligence operations.
 *
 * Parameters: x,y - Map coordinates to update
 * Effect: Sets scouts flag, preserves other flags
 * Usage: Unit capability tracking, intelligence operations, command availability
 */
#define set_has_scouts(x,y)	SECTSTAT(x,y) |= SSTAT_SCOUTS

/*
 * Display System Global Variables
 *
 * These global variables manage the current display state, screen
 * configurations, and display mode data for the user interface system.
 */

/*
 * zoom_level - Current map zoom level setting
 *
 * Tracks the active zoom level for map display, affecting the level
 * of detail shown and the scale of map sectors. Different zoom levels
 * provide different tactical and strategic perspectives.
 *
 * Range: 0 to ZOOM_NUMBER-1
 * Usage: Display scaling, detail level control, user interface state
 * Integration: Used with screen configuration arrays for layout
 */
extern int zoom_level;

/*
 * base_modes[] - Array of available display modes
 *
 * Contains the complete set of display mode configurations, each
 * defining different visualization styles, color schemes, and
 * information display options for tactical analysis.
 *
 * Size: DMODE_NUMBER (21) display mode configurations
 * Usage: Display mode switching, visualization customization
 * Integration: Each mode defines colors, symbols, and layout options
 */
extern DISPLAY_STRUCT base_modes[DMODE_NUMBER];

/*
 * hexmap_screen[] - Hexagonal map screen configurations
 *
 * Array of screen layout configurations optimized for hexagonal
 * map display at different zoom levels. Handles the complex
 * positioning required for proper hexagonal grid alignment.
 *
 * Size: ZOOM_NUMBER configurations for different zoom levels
 * Usage: Hexagonal map rendering, coordinate translation, layout
 * Features: Includes oddlift calculations for hex grid alignment
 */
extern SCREEN_STRUCT hexmap_screen[ZOOM_NUMBER];

/*
 * rectmap_screen[] - Rectangular map screen configurations
 *
 * Array of screen layout configurations optimized for rectangular
 * (square grid) map display at different zoom levels. Provides
 * simpler positioning for traditional grid-based maps.
 *
 * Size: ZOOM_NUMBER configurations for different zoom levels
 * Usage: Rectangular map rendering, coordinate translation, layout
 * Features: Standard grid positioning without hex complications
 */
extern SCREEN_STRUCT rectmap_screen[ZOOM_NUMBER];

/*
 * curmap_screen - Current active screen configuration
 *
 * Pointer to the currently active screen configuration structure,
 * which may point to either a hexagonal or rectangular map
 * configuration based on current map type and zoom level.
 *
 * Points to: Active element from hexmap_screen[] or rectmap_screen[]
 * Usage: Current display calculations, coordinate translation
 * Performance: Direct pointer access avoids array lookups during rendering
 */
extern SCREEN_PTR curmap_screen;
