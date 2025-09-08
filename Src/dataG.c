/*
 * dataG.c - User Interface Global Data Declarations
 *
 * This file contains global variable declarations and static data arrays
 * for the user interface system. No functions are defined here - this is
 * purely a data definition file that provides:
 *
 * - Display mode configurations and presets
 * - User interface option lists and arrays
 * - Screen layout definitions for different zoom levels
 * - Global state variables for UI components
 * - Default values and configuration templates
 *
 * Data Categories:
 * - Message of the day default content
 * - Display and highlight option lists
 * - Predefined display mode templates (21 modes)
 * - Screen structure definitions for hexagonal and rectangular maps
 * - UI state variables (email mode, screen pointers, etc.)
 * - File paths and system status variables
 *
 * Usage:
 *   This file is included via #define DATA_DECLARE to instantiate
 *   global variables that are declared as extern in dataG.h
 */

/* Declaration of global variables in the user interface */
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
#define DATA_DECLARE
#include "dataG.h"
#undef DATA_DECLARE
#include "infoG.h"
#include "displayG.h"

/*
 * dflt_motd - Default Message of the Day Content
 *
 * Contains the default message lines displayed to users when they log in.
 * This array provides fallback content when no custom MOTD is configured.
 * Each string represents one line of the message, with the last entry being
 * an empty string to mark the end of the message content.
 *
 * Array Size: MOTDLINES (defined in header)
 * Format: Array of string pointers, null-terminated content
 * Usage: Displayed in login sequence and game startup
 */
char *dflt_motd[MOTDLINES] = {
  "New movement system installed that has greater accuracy",
  "Expanded and updated documentation system",
  "Many bugs fixed",
  ""
};

/*
 * display_list - Map Display Options Array
 *
 * Defines the available display modes for map visualization. Each string
 * corresponds to a different way of displaying information on the map hexes.
 * These options control what data is shown in each hex position on the game map.
 *
 * Display Categories:
 * - Terrain: blank, contour, vegetation, designation
 * - Resources: food, jewel, metal, wood, values
 * - Political: nation mark, your desg, people, race
 * - Movement: Army mcost, Navy mcost, Flight mcost, Weights
 * - Special: Defense, Magic value, Tgood desgs, Keep
 *
 * Usage: Used in display mode selection menus and configuration
 * Array Size: 20 elements plus null terminator
 */
char *display_list[] = {
  "blank", "contour", "designation", "food", "jewel", "metal",
  "nation mark", "people", "race", "vegetation", "wood", "your desg",
  "Army mcost", "Defense", "Flight mcost", "Magic value", "Navy mcost",
  "Tgood desgs", "Values", "Weights", "Keep"
};

/*
 * highl_list - Map Highlighting Options Array
 *
 * Defines the available highlighting modes for emphasizing specific map elements.
 * These options control which hexes or units are visually highlighted on the map
 * to help players identify important information quickly.
 *
 * Highlight Categories:
 * - Political: allies, enemies, neutrals, owned
 * - Terrain: designation, minor desgs, region
 * - Units: scouts, units, your units, movable, supported, unsupported
 * - Special: range, tradegoods, none, keep
 *
 * Usage: Used in highlight mode selection and map visualization
 * Array Size: 16 elements plus null terminator
 * Special: "Keep" preserves current setting, "None" disables highlighting
 */
char *highl_list[] = {
  "None", "allies", "designation", "enemies", "minor desgs",
  "neutrals", "owned", "range", "scouts", "units", "your units",
  "Movable", "Region", "Supported", "Tradegoods", "Unsupported",
  "Keep"
};

/*
 * display_mode - Current Active Display Mode Configuration
 *
 * Contains the currently active display mode settings that control how the
 * game map is rendered. This structure is modified during gameplay as users
 * change display settings. Initialized to match the "Standard" preset below.
 *
 * Structure Fields:
 * - name: Display mode name ("Standard")
 * - position: Hex position type (HXPOS_LOWLEFT)
 * - display[4]: Four display layer settings (designation, blank, blank, contour)
 * - highlight[4]: Four highlight layer settings (owned for all)
 * - nations[4]: Nation filtering settings (unowned for all)
 *
 * Usage: Active configuration modified by display mode changes
 * Default: Mirrors "Standard" mode from base_modes array
 */
DISPLAY_STRUCT display_mode = {
  /* Resembles the Standard Setting Below */
  "Standard", HXPOS_LOWLEFT,
  DI_DESG, DI_BLANK, DI_BLANK, DI_CONT,
  HI_OWN, HI_OWN, HI_OWN, HI_OWN,
  UNOWNED, UNOWNED, UNOWNED, UNOWNED
};

/*
 * base_modes - Predefined Display Mode Templates Array
 *
 * Contains all 21 predefined display mode configurations that users can select.
 * Each mode defines a complete set of display and highlight settings optimized
 * for viewing specific types of game information. These serve as templates that
 * can be copied to the active display_mode when selected.
 *
 * Array Contents (21 modes):
 * 0. Standard - General purpose display with designations and contours
 * 1. Designation - Focus on sector designations
 * 2. Own Designation - Show only player's designated sectors
 * 3. Nation Marks - Display nation ownership markers
 * 4. Race - Show racial information in sectors
 * 5. Population - Display population data
 * 6. Contour - Emphasize terrain elevation
 * 7. Vegetation - Show vegetation types
 * 8. Tradegoods - Display tradegood information
 * 9. Jewels - Focus on jewel mine locations
 * 10. Magics - Show magic-related information
 * 11. Metals - Display metal mine locations
 * 12. Values - Show sector values
 * 13. Food - Display food production
 * 14. Wood - Show lumber production
 * 15. Defense - Display defensive capabilities
 * 16. Army Move - Show army movement costs
 * 17. Navy Move - Show naval movement costs
 * 18. Flight Move - Show flight movement costs
 * 19. Weights - Display weight/capacity information
 * 20. Blank - Clear display mode with no overlays
 *
 * Usage: Template source for user display mode selection
 * Array Size: DMODE_NUMBER (21 elements)
 */
DISPLAY_STRUCT base_modes[DMODE_NUMBER] = {
  { "Standard",
      HXPOS_LOWLEFT,
      DI_DESG, DI_BLANK, DI_BLANK, DI_CONT,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Designation",
      HXPOS_LOWLEFT,
      DI_DESG, DI_KEEP, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Own Designation",
      HXPOS_LOWLEFT,
      DI_YDESG, DI_KEEP, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Nation Marks",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_NATN, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Race",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_RACE, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Population",
      HXPOS_LOWRIGHT,
      DI_KEEP, DI_PEOP, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Contour",
      HXPOS_UPRIGHT,
      DI_KEEP, DI_KEEP, DI_KEEP, DI_CONT,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Vegetation",
      HXPOS_UPRIGHT,
      DI_KEEP, DI_KEEP, DI_KEEP, DI_VEGE,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Tradegoods",
      HXPOS_LOWRIGHT,
      DI_DESG, DI_TGDESGS, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Jewels",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_JEWEL, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Magics",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_MAGIC, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Metals",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_METAL, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Values",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_VALUES, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Food",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_FOOD, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Wood",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_WOOD, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Defense",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_DEFENSE, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Army Move",
      HXPOS_LOWRIGHT,
      DI_KEEP, DI_AMOVE, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Navy Move",
      HXPOS_LOWRIGHT,
      DI_KEEP, DI_NMOVE, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Flight Move",
      HXPOS_LOWRIGHT,
      DI_KEEP, DI_FMOVE, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Weights",
      HXPOS_LOWRIGHT,
      DI_KEEP, DI_WEIGHTS, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Blank",
      HXPOS_LOWLEFT,
      DI_BLANK, DI_BLANK, DI_BLANK, DI_BLANK,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 }
};

/*
 * hexmap_screen - Hexagonal Map Screen Layout Definitions
 *
 * Defines screen layout parameters for hexagonal map display at different
 * zoom levels. Each zoom level has different spacing and sizing parameters
 * to optimize display density and readability.
 *
 * Structure Fields (per zoom level):
 * - x_spacing: Horizontal spacing between hex centers
 * - y_spacing: Vertical spacing between hex rows
 * - x_size: Horizontal size of each hex
 * - y_size: Vertical size of each hex
 * - hex_offset: Offset for hex positioning (1 for hex maps)
 * - show_text: Whether to show text in hexes
 *
 * Zoom Levels:
 * 0. Close (3,2,4,2,1,TRUE) - Large hexes with text
 * 1. Medium (2,2,1,1,1,FALSE) - Medium hexes without text
 * 2. Far (1,2,1,1,1,FALSE) - Small hexes without text
 *
 * Usage: Selected based on current zoom level for hex map rendering
 */
SCREEN_STRUCT hexmap_screen[ZOOM_NUMBER] = {
  { 3, 2, 4, 2, 1, TRUE },
  { 2, 2, 1, 1, 1, FALSE },
  { 1, 2, 1, 1, 1, FALSE }
};

/*
 * rectmap_screen - Rectangular Map Screen Layout Definitions
 *
 * Defines screen layout parameters for rectangular map display at different
 * zoom levels. Similar to hexmap_screen but with rectangular grid positioning
 * and no hex offset required.
 *
 * Structure Fields (per zoom level):
 * - x_spacing: Horizontal spacing between cells
 * - y_spacing: Vertical spacing between rows
 * - x_size: Horizontal size of each cell
 * - y_size: Vertical size of each cell
 * - hex_offset: Offset for positioning (0 for rectangular)
 * - show_text: Whether to show text in cells
 *
 * Zoom Levels:
 * 0. Close (3,2,4,2,0,TRUE) - Large cells with text
 * 1. Medium (2,1,1,1,0,FALSE) - Medium cells without text
 * 2. Far (1,1,1,1,0,FALSE) - Small cells without text
 *
 * Usage: Alternative layout for non-hexagonal map viewing
 */
SCREEN_STRUCT rectmap_screen[ZOOM_NUMBER] = {
  { 3, 2, 4, 2, 0, TRUE },
  { 2, 1, 1, 1, 0, FALSE },
  { 1, 1, 1, 1, 0, FALSE }
};

/*
 * cv_name - View Change Category Names
 *
 * Array of strings representing the different categories of map data that
 * can be cycled through when using view change commands. These correspond
 * to major map data types that have multiple subtypes for detailed viewing.
 *
 * Categories:
 * - "Contour": Terrain elevation levels
 * - "Designation": Sector designation types  
 * - "Vegetation": Vegetation and biome types
 *
 * Usage: Used in conjunction with cv_max for cycling through data subtypes
 * Array Size: 3 categories
 */
char *cv_name[] = {
  "Contour", "Designation", "Vegetation"
};

/*
 * cv_max - View Change Maximum Values
 *
 * Array of integers defining the maximum number of subtypes available for
 * each view change category. Used to determine cycling limits when users
 * browse through different display variations.
 *
 * Values:
 * - ELE_NUMBER: Maximum elevation/contour levels
 * - MAJ_NUMBER: Maximum major designation types
 * - VEG_NUMBER: Maximum vegetation types
 *
 * Usage: Paired with cv_name for view change cycling logic
 * Array Size: 3 values (matches cv_name array)
 */
int cv_max[] = { ELE_NUMBER, MAJ_NUMBER, VEG_NUMBER };

/*
 * shortspeed - Speed Indicator Symbols
 *
 * Array of single-character strings used to display unit speed status in
 * the sidebar and unit information displays. Provides quick visual feedback
 * about unit movement capabilities.
 *
 * Speed Indicators:
 * - "<": Below normal speed
 * - "=": Normal/standard speed
 * - ">": Above normal speed
 * - "-": Reduced speed (penalties)
 * - "+": Enhanced speed (bonuses)
 *
 * Usage: Unit status displays and movement information
 * Array Size: 5 speed states
 */
char *shortspeed[] = { "<", "=", ">", "-", "+" };

/*
 * shortdir - Direction Abbreviations
 *
 * Array of direction names used for compact direction displays in the UI.
 * Covers all 8 compass directions plus the current location indicator.
 *
 * Directions:
 * - "Here": Current location (no movement)
 * - "N": North
 * - "NE": Northeast  
 * - "E": East
 * - "SE": Southeast
 * - "S": South
 * - "SW": Southwest
 * - "W": West
 * - "NW": Northwest
 *
 * Usage: Movement commands, unit facing, and navigation displays
 * Array Size: 9 directions (including "Here")
 */
char *shortdir[] = { "Here", "N", "NE", "E", "SE", "S", "SW", "W", "NW" };

/*
 * shipsize - Naval Unit Size Classifications
 *
 * Array of ship size category names used for naval unit classification
 * and display. Determines ship capabilities, costs, and strategic roles.
 *
 * Ship Classes:
 * - "Light": Fast, low-cost vessels (scouts, raiders)
 * - "Medium": Balanced ships (standard warships)
 * - "Heavy": Powerful, expensive vessels (battleships, carriers)
 *
 * Usage: Naval unit information, construction menus, fleet composition
 * Array Size: 3 ship size categories
 */
char *shipsize[] = { "Light", "Medium", "Heavy" };

/*
 * desg_selects - Major Designation Selection Options
 *
 * Array of major designation types available for sector development.
 * Each entry includes both a selection key and descriptive name for
 * designation assignment menus. The first character serves as the
 * keyboard shortcut for quick selection.
 *
 * Designation Types:
 * - "XNone": No designation (clear)
 * - "FFarm": Agricultural designation for food production
 * - "GFertile": Enhanced agricultural designation
 * - "HFruitful": Premium agricultural designation
 * - "mMetal mine": Metal extraction facility
 * - "$Jewel mine": Precious stone extraction facility
 * - ":Lumberyard": Wood production facility
 * - "@Shrine": Religious/magical facility
 * - "[Bridge": Transportation infrastructure
 * - "=Canal": Water transportation infrastructure
 * - "|Wall": Defensive fortification
 * - "&Cache": Storage facility
 * - "sStockade": Basic defensive structure
 * - "tTown": Urban settlement
 * - "cCity": Large urban center
 * - "CCapital": Nation capital city
 *
 * Usage: Designation assignment menus and sector development
 * Array Size: 16 major designation types
 */
char *desg_selects[] = {
  "XNone", "FFarm", "GFertile", "HFruitful", "mMetal mine",
  "$Jewel mine", ":Lumberyard", "@Shrine", "[Bridge", "=Canal",
  "|Wall", "&Cache", "sStockade", "tTown", "cCity", "CCapital"
};

/*
 * min_desg_selects - Minor Designation Selection Options
 *
 * Array of minor designation types that provide additional sector attributes
 * and special properties. These can be applied in addition to major
 * designations to add specialized functionality or temporary status.
 *
 * Minor Designation Types:
 * - "Xdevastate": Sector has been devastated (damaged/destroyed)
 * - "Y forsale": Sector is available for purchase/transfer
 * - "Z Sieged": Sector is under siege conditions
 * - "Trading Post": Commercial trading facility
 * - "Roads": Transportation infrastructure improvement
 * - "Blacksmith": Metalworking and equipment facility
 * - "School": Education and research facility
 * - "Church": Religious facility
 * - "Mill": Processing facility for goods
 * - "Granary": Food storage facility
 * - "Fortifications": Defensive improvements
 * - "Harbor": Naval facility and port
 *
 * Usage: Secondary designation assignment and sector enhancement
 * Array Size: 12 minor designation types
 */
char *min_desg_selects[] = {
  "Xdevastate", "Y forsale", "Z Sieged", "Trading Post", "Roads",
  "Blacksmith", "School", "Church", "Mill", "Granary",
  "Fortifications", "Harbor"
};

/*
 * email_mode - Default Email Edit Mode
 *
 * Global variable defining the default editing mode for the email system.
 * Controls the initial behavior and interface style when users compose
 * or edit email messages.
 *
 * Value: EM_M_NORMAL (normal editing mode)
 * Usage: Email composition and editing system initialization
 */
int email_mode = EM_M_NORMAL;

/*
 * curmap_screen - Current Map Screen Layout Pointer
 *
 * Pointer to the currently active screen layout structure. Points to either
 * hexmap_screen or rectmap_screen array based on current map display mode
 * and zoom level. NULL indicates no screen layout is currently selected.
 *
 * Type: SCREEN_PTR (pointer to SCREEN_STRUCT)
 * Default: NULL (uninitialized)
 * Usage: Map rendering and screen layout calculations
 */
SCREEN_PTR curmap_screen = NULL;

/*
 * session_random - Session Random Number Seed
 *
 * Random number seed value used for the current game session. Provides
 * the basis for all random number generation during gameplay to ensure
 * consistent and reproducible results within a session.
 *
 * Type: long integer
 * Usage: Random number generation and game mechanics
 */
long session_random;

/*
 * fison - Current Player Data File Path
 * conqmail - Mail System File Path
 *
 * File path strings for the current player's data file and mail system.
 * These paths are constructed based on player ID and game configuration
 * to locate the appropriate data files for the session.
 *
 * Size: FILELTH characters each
 * Usage: File I/O operations for player data and mail
 */
char fison[FILELTH], conqmail[FILELTH];

/*
 * sectstat - Sector Status Display Buffer
 * trooplocs - Troop Location Display Buffer
 *
 * Dynamic string buffers used for building sector status and troop location
 * information displays. Allocated as needed and freed when no longer required.
 * NULL when not in use.
 *
 * Type: char* (dynamically allocated)
 * Default: NULL
 * Usage: Status display generation and UI information
 */
char *sectstat = NULL, *trooplocs = NULL;

/*
 * redraw - Screen Redraw Flag
 * xloc_mark, yloc_mark - Map Location Markers
 * lockfilenum - File Lock Number
 * mail_ind - Mail Status Indicator
 *
 * Global state variables for UI and system management:
 * - redraw: Whether screen needs complete redraw
 * - xloc_mark, yloc_mark: Marked location coordinates on map
 * - lockfilenum: Current file lock identifier
 * - mail_ind: Mail system status (MAIL_DONE when no mail activity)
 *
 * Usage: UI state management and file coordination
 */
int redraw, xloc_mark, yloc_mark, lockfilenum, mail_ind = MAIL_DONE;

/*
 * pager - Paging System State
 * selector - Selection System State
 *
 * State variables for UI navigation and selection systems:
 * - pager: Current page or paging system status
 * - selector: Current selection or selector system status
 *
 * Usage: UI navigation and item selection
 */
int pager, selector;

/*
 * conq_news_status - News System Status
 * conq_mail_status - Mail System Status  
 * god_browsing - God Mode Browsing Flag
 *
 * System status flags for various game subsystems:
 * - conq_news_status: Status of news/announcement system
 * - conq_mail_status: Status of mail system operations
 * - god_browsing: Whether user is browsing in god/admin mode (FALSE by default)
 *
 * Usage: System state tracking and administrative functions
 */
int conq_news_status, conq_mail_status, god_browsing = FALSE;

/*
 * city_mode - Current City Information Display Mode
 *
 * Defines which type of city information is currently being displayed
 * when viewing city details. Controls the active information panel
 * in city interface screens.
 *
 * Type: Cinfotype enumeration
 * Default: CINFO_ECONOMY (economic information)
 * Usage: City information display system
 */
Cinfotype city_mode = CINFO_ECONOMY;

/*
 * command_cost - Current Command Cost
 *
 * Tracks the resource cost of the currently executing or planned command.
 * Used for displaying command costs to users and validating resource
 * availability before command execution.
 *
 * Type: itemtype (likely a resource/cost structure)
 * Usage: Command cost calculation and resource management
 */
itemtype command_cost;

#ifdef SYSMAIL
/*
 * sys_mail_status - System Mail Status
 * sysmail - System Mail File Path
 *
 * System mail functionality variables (compiled conditionally):
 * - sys_mail_status: Status of system-wide mail operations
 * - sysmail: File path for system mail storage
 *
 * Size: sysmail is FILELTH characters
 * Usage: System-level mail operations and notifications
 * Conditional: Only available when SYSMAIL is defined
 */
int sys_mail_status;
char sysmail[FILELTH];
#endif /*SYSMAIL*/
