/*
 * dataG.h - User Interface Global Data Structures and Constants
 *
 * This header file defines the core data structures, constants, and global
 * variables specifically used by the Conquer game's user interface system.
 * It provides the foundational definitions for UI rendering, command processing,
 * mail systems, display modes, and user interaction patterns.
 *
 * Key Components:
 *   - Command processing constants (MAXCOMMANDS, key parsing limits)
 *   - Display and pager system configuration (screen dimensions, redraw modes)
 *   - Mail system infrastructure (statuses, editing modes, indicators)
 *   - File perusal structures (LINE_STRUCT for text display)
 *   - Jump navigation system (JUMP_CAP, JUMP_PICK, JUMP_SAVE)
 *   - Global UI state variables (locations, mail status, display modes)
 *
 * Integration Notes:
 *   - Used by: All user interface source files (mainG.c, displayG.c, infoG.c, etc.)
 *   - Depends on: dataX.h (extended data structures), keybindG.h (key bindings)
 *   - Provides: UI-specific constants, mail system types, display structures
 *   - Includes: Conditional compilation for SYSMAIL and REGEXP features
 *
 * Modernization Considerations:
 *   - Replace magic numbers with named constants for better maintainability
 *   - Consider const qualifiers for read-only string arrays
 *   - Evaluate thread safety for global UI state variables
 *   - Update conditional compilation to use feature detection
 *   - Add bounds checking constants for array access safety
 *   - Consider replacing hardcoded screen dimensions with dynamic calculation
 *
 * Architecture Role:
 *   - Provides UI abstraction layer over core game data structures
 *   - Enables separation between game logic (dataA.h) and interface presentation
 *   - Supports multiple UI modes and display configurations
 *   - Facilitates mail system integration with external messaging
 */

/* This file declares global data for the user interface */
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

/* Include the global definitions file */
#include "dataX.h"
#include "keybindG.h"

/* ============================================================================
 * USER INTERFACE SCOPE DEFINITION
 * ============================================================================
 * Purpose: Define the scope and usage boundaries for UI-specific definitions
 * Usage: Included by all user interface source files only
 * Notes: Separates UI layer from core game engine (dataA.h) and extensions (dataX.h)
 */

/*
 * This file is to be included by all files composing only the Conquer User
 * Interface (conquer). Only those definitions used within all of the conquer
 * files and only in the conquer files should be set here.
 */

/* ============================================================================
 * COMMAND PROCESSING CONSTANTS
 * ============================================================================
 * Purpose: Define limits and constraints for user command input processing
 * Usage: Command parser, key binding system, input validation
 * Notes: Critical for preventing buffer overflows in command processing
 */

/*
 * MAXCOMMANDS - Maximum number of command keys that can be parsed
 *
 * Defines the upper limit for command key parsing to prevent buffer overflows
 * and ensure reasonable memory usage. This constant affects the size of
 * command parsing buffers and validation logic.
 *
 * Value: 255 (maximum unsigned char value)
 * Units: Number of command keys
 * Usage: Command parser buffer allocation, input validation loops
 * Notes: Chosen as maximum unsigned char value for efficient storage
 * Modernization: Consider using SIZE_T_MAX or dynamic allocation for flexibility
 */
#define MAXCOMMANDS	255

/* ============================================================================
 * PAGER SYSTEM CONFIGURATION
 * ============================================================================
 * Purpose: Define display dimensions for built-in help and text pager system
 * Usage: Help display, file viewing, text pagination calculations
 * Notes: Hardcoded values may not work well on all terminal sizes
 */

/*
 * MAX_PAGERHELP - Number of text lines displayable in pager help mode
 *
 * Defines the maximum number of lines that can be displayed simultaneously
 * in the pager help system. This controls scrolling behavior and screen
 * real estate usage for help text display.
 *
 * Value: 21 lines
 * Units: Text lines on screen
 * Usage: Help text pagination, screen layout calculations
 * Notes: Assumes standard 24-25 line terminal with header/footer space
 * Modernization: Should be dynamic based on actual terminal size (LINES)
 */
#define MAX_PAGERHELP	21

/*
 * WIDTH_PAGERHELP - Character width for pager help text display
 *
 * Defines the maximum character width for help text lines to ensure proper
 * formatting and prevent line wrapping issues. Controls text layout and
 * formatting in help display mode.
 *
 * Value: 42 characters
 * Units: Characters per line
 * Usage: Help text formatting, line wrapping calculations
 * Notes: Conservative width to work on 80-column terminals with margins
 * Modernization: Should be dynamic based on actual terminal width (COLS)
 */
#define WIDTH_PAGERHELP	42

/* ============================================================================
 * FILE DISPLAY DATA STRUCTURES
 * ============================================================================
 * Purpose: Define structures for text file display and perusal functionality
 * Usage: File viewers, help system, mail reader, text display components
 * Notes: Supports syntax highlighting and line-by-line text processing
 */

/*
 * struct s_line - Text line structure for file perusal and display
 *
 * Represents a single line of text in the file perusal system, supporting
 * both the text content and associated display attributes. This structure
 * enables the UI system to display text files with highlighting, formatting,
 * and visual emphasis for better user experience.
 *
 * Usage Patterns:
 *   - Array of LINE_STRUCT represents complete file or text buffer
 *   - Line-by-line processing for pagination and scrolling
 *   - Highlight field controls visual emphasis and color coding
 *   - Memory managed through malloc/free for dynamic text loading
 *
 * Relationships:
 *   - Used by pager system (pagerG.c) for file viewing
 *   - Integrated with mail system for message display
 *   - Connected to help system for documentation viewing
 *   - Supports syntax highlighting in various contexts
 *
 * Fields:
 *   line - Pointer to null-terminated text string (dynamically allocated)
 *   highlight - Visual emphasis flags and color coding information
 *
 * Memory Management:
 *   - line pointer must be allocated with malloc() and freed with free()
 *   - Structure itself typically allocated as array for complete files
 *   - Careful cleanup required to prevent memory leaks
 *
 * Thread Safety:
 *   - Not thread-safe without external synchronization
 *   - Concurrent access to highlight field requires locking
 *   - Line pointer modification needs atomic operations
 */
typedef struct s_line {
    char *line;         /* Text content pointer (null-terminated, dynamically allocated) */
    uns_char highlight; /* Visual emphasis flags (color, bold, underline, etc.) */
} LINE_STRUCT;

/* ============================================================================
 * MAIL SYSTEM STATUS CONSTANTS
 * ============================================================================
 * Purpose: Define operational states for the integrated mail system
 * Usage: Mail interface, message processing, user interaction state tracking
 * Notes: Controls mail system workflow and user interface behavior
 */

/*
 * MAIL_DONE - Mail system idle state indicator
 *
 * Indicates that the mail system is not currently active and no mail
 * operations are in progress. This is the default state when users are
 * not interacting with the mail system.
 *
 * Value: 0 (false/inactive state)
 * Units: State enumeration
 * Usage: Mail system state machine, UI mode switching
 * Notes: Default state for mail system initialization
 */
#define MAIL_DONE	0

/*
 * MAIL_READING - Mail reading mode active indicator
 *
 * Indicates that the user is currently reading mail messages. The mail
 * system interface is active in read mode, displaying message content
 * and providing navigation controls for message browsing.
 *
 * Value: 1 (active reading state)
 * Units: State enumeration
 * Usage: Mail reader interface, message display logic
 * Notes: Enables read-only mail operations and navigation
 */
#define MAIL_READING	1

/*
 * MAIL_SENDING - Mail composition/sending mode indicator
 *
 * Indicates that the user is currently composing or sending mail messages.
 * The mail system interface is active in write mode, providing editing
 * capabilities and message transmission functionality.
 *
 * Value: 2 (active sending state)
 * Units: State enumeration
 * Usage: Mail composer interface, message editing logic
 * Notes: Enables mail composition, editing, and transmission
 */
#define MAIL_SENDING	2

/* ============================================================================
 * MAIL EDITING MODE FLAGS
 * ============================================================================
 * Purpose: Define editing behavior and interface style for mail composition
 * Usage: Mail editor, text input processing, editing mode control
 * Notes: Bit flags for combining multiple editing modes simultaneously
 */

/*
 * EM_M_NORMAL - Normal mail editing mode (default)
 *
 * Standard mail editing mode with default text input behavior. Provides
 * basic text editing functionality without special input processing or
 * visual enhancements.
 *
 * Value: 0x00 (no special flags set)
 * Units: Bit flag
 * Usage: Mail editor initialization, mode reset operations
 * Notes: Default editing mode, can be combined with other flags
 */
#define EM_M_NORMAL	0x00

/*
 * EM_M_INSERTON - Insert mode enabled flag
 *
 * Enables insert mode for mail editing where typed characters are inserted
 * at the cursor position rather than overwriting existing text. This flag
 * modifies the text input behavior in the mail editor.
 *
 * Value: 0x04 (bit 2 set)
 * Units: Bit flag
 * Usage: Text input processing, cursor behavior control
 * Notes: Can be combined with other editing mode flags
 */
#define EM_M_INSERTON	0x04

/*
 * EM_M_VISTYLE - Vi-style editing mode flag
 *
 * Enables Vi-style (modal) editing behavior in the mail editor, providing
 * command mode and insert mode distinctions similar to the Vi text editor.
 * This affects key binding interpretation and editing workflow.
 *
 * Value: 0x08 (bit 3 set)
 * Units: Bit flag
 * Usage: Key binding processing, modal editing control
 * Notes: Provides Vi-like editing experience for experienced users
 */
#define EM_M_VISTYLE	0x08

/* ============================================================================
 * CONDITIONAL COMPILATION BOUNDARY
 * ============================================================================
 * Purpose: Separate definitions used by dataG.c from those used by other files
 * Usage: Prevents redefinition conflicts in dataG.c global data declarations
 * Notes: Everything past this point excluded when DATA_DECLARE is defined
 */

/* Everything past this point is not needed in dataG.c */
#ifndef DATA_DECLARE

/* ============================================================================
 * SCREEN REDRAW CONTROL CONSTANTS
 * ============================================================================
 * Purpose: Define display refresh modes for efficient screen updating
 * Usage: Display system, screen management, performance optimization
 * Notes: Controls trade-off between display completeness and performance
 */

/*
 * DRAW_DONE - No screen redraw required indicator
 *
 * Indicates that no screen redraw is necessary, typically used when the
 * display is already current and no visual updates are needed. This helps
 * optimize performance by avoiding unnecessary screen refresh operations.
 *
 * Value: 0 (no redraw state)
 * Units: Redraw mode enumeration
 * Usage: Display optimization, screen update control
 * Notes: Default state for screen refresh logic
 */
#define DRAW_DONE	0

/*
 * DRAW_FULL - Complete screen redraw required
 *
 * Indicates that a complete screen redraw is necessary, typically used when
 * the display has been corrupted, resized, or when switching between major
 * interface modes. This ensures complete visual consistency.
 *
 * Value: 1 (full redraw state)
 * Units: Redraw mode enumeration
 * Usage: Screen corruption recovery, mode transitions
 * Notes: Most expensive redraw operation, use sparingly
 */
#define DRAW_FULL	1

/*
 * DRAW_NOFRILLS - Minimal screen redraw mode
 *
 * Indicates that only essential screen elements should be redrawn, omitting
 * decorative elements and non-critical visual components. This provides a
 * performance-optimized display mode for slow terminals or heavy load.
 *
 * Value: 2 (minimal redraw state)
 * Units: Redraw mode enumeration
 * Usage: Performance optimization, slow terminal support
 * Notes: Balances functionality with drawing performance
 */
#define DRAW_NOFRILLS	2

/* ============================================================================
 * JUMP NAVIGATION SYSTEM CONSTANTS
 * ============================================================================
 * Purpose: Define navigation modes for map jumping and location selection
 * Usage: Map navigation, coordinate jumping, location bookmarking
 * Notes: Supports different jump behaviors for various navigation contexts
 */

/*
 * JUMP_CAP - Jump to capital/primary location
 *
 * Navigation mode for jumping to the player's capital city or primary
 * location. This provides quick access to the most important game location
 * for administrative and strategic purposes.
 *
 * Value: 0 (capital jump mode)
 * Units: Jump mode enumeration
 * Usage: Map navigation, quick location access
 * Notes: Typically jumps to nation capital or starting location
 */
#define JUMP_CAP	0

/*
 * JUMP_PICK - Interactive location picking mode
 *
 * Navigation mode that allows the user to interactively select a location
 * to jump to, typically through a selection interface or coordinate input.
 * Provides maximum flexibility for location navigation.
 *
 * Value: 1 (interactive pick mode)
 * Units: Jump mode enumeration
 * Usage: Interactive navigation, coordinate selection
 * Notes: Requires user input for target location selection
 */
#define JUMP_PICK	1

/*
 * JUMP_SAVE - Jump to saved/bookmarked location
 *
 * Navigation mode for jumping to a previously saved or bookmarked location.
 * This enables quick return to important locations that have been marked
 * for future reference.
 *
 * Value: 2 (saved location mode)
 * Units: Jump mode enumeration
 * Usage: Location bookmarking, saved position navigation
 * Notes: Requires previously saved location data
 */
#define JUMP_SAVE	2

/* ============================================================================
 * MAIL STATUS INDICATORS
 * ============================================================================
 * Purpose: Define mail notification states for UI status display
 * Usage: Status line updates, mail notification, user alerts
 * Notes: Controls visual indicators for mail system status
 */

/*
 * STMAIL_NEW - New mail available indicator
 *
 * Indicates that new mail messages are available for the user. This status
 * triggers visual notifications and status line updates to alert the user
 * of pending mail requiring attention.
 *
 * Value: 0 (new mail present)
 * Units: Mail status enumeration
 * Usage: Mail notifications, status line display
 * Notes: Triggers user interface mail indicators
 */
#define STMAIL_NEW	0

/*
 * STMAIL_NONE - No new mail indicator
 *
 * Indicates that no new mail messages are available. This is the default
 * state when all mail has been read or when no mail has been received.
 *
 * Value: 1 (no new mail)
 * Units: Mail status enumeration
 * Usage: Mail status display, notification clearing
 * Notes: Default state for mail status indicators
 */
#define STMAIL_NONE	1

/* ============================================================================
 * DYNAMIC SCREEN LAYOUT MACROS
 * ============================================================================
 * Purpose: Calculate screen dimensions based on current display mode and terminal size
 * Usage: Layout calculations, window sizing, display optimization
 * Notes: Provides responsive layout that adapts to different screen configurations
 */

/*
 * SCREEN_SLIM - Usable screen height calculation
 *
 * Calculates the available screen height for content display, accounting for
 * headers, footers, and interface mode. Adapts based on whether info mode
 * is active, providing different space allocations for different UI modes.
 *
 * Value: Dynamic calculation based on LINES and conq_infomode
 * Units: Screen lines
 * Usage: Window sizing, content layout, scroll calculations
 * Notes: Conditional calculation: (LINES-5) in info mode, (LINES-14) otherwise
 * Modernization: Should use terminal capability queries instead of LINES
 */
#define SCREEN_SLIM	(conq_infomode?(LINES - 5):(LINES - 14))

/*
 * SCREEN_SIDE - Half-screen height calculation
 *
 * Calculates half of the usable screen height for split-screen layouts and
 * side-by-side display modes. Based on SCREEN_SLIM calculation to maintain
 * consistency with overall screen layout.
 *
 * Value: SCREEN_SLIM divided by 2
 * Units: Screen lines
 * Usage: Split-screen layouts, side panels, dual-pane displays
 * Notes: Dependent on SCREEN_SLIM macro for consistent layout
 * Modernization: Consider minimum size limits for usability
 */
#define SCREEN_SIDE	(SCREEN_SLIM / 2)

/* ============================================================================
 * GLOBAL USER INTERFACE STATE VARIABLES
 * ============================================================================
 * Purpose: Declare global variables for UI state management and data storage
 * Usage: Shared across all user interface modules for state coordination
 * Notes: Defined in dataG.c, declared here for external access
 * Thread Safety: Not thread-safe without external synchronization
 */

/* file: dataG.c -- global data for user interface */

/*
 * Location Marking Variables
 * Purpose: Track marked locations on the game map for navigation and reference
 */
extern int xloc_mark, yloc_mark;    /* Marked map coordinates (X, Y) for quick navigation */

/*
 * System State Variables  
 * Purpose: Track various system states and operational modes
 */
extern int lockfilenum;    /* File lock number for multi-user coordination */
extern int mail_ind;       /* Mail system indicator and status tracker */
extern int redraw;         /* Screen redraw requirement flag (DRAW_* constants) */

/*
 * File Path and Message Variables
 * Purpose: Store file paths and default message content
 */
extern char fison[];                    /* File system path or status indicator */
extern char conqmail[];                 /* Conquer mail system file path */
extern char *dflt_motd[MOTDLINES];     /* Default message of the day text array */

/*
 * Command and Resource Variables
 * Purpose: Track command costs and resource requirements
 */
extern itemtype command_cost;          /* Resource cost for current command execution */

/*
 * User Interface Mode Variables
 * Purpose: Track current interface modes and user preferences
 */
extern int email_mode;     /* Email system operational mode */
extern int pager;          /* Pager system activation state */
extern int selector;       /* Selection system mode indicator */
extern int city_mode;      /* City interface mode flag */

/*
 * News and Mail Status Variables
 * Purpose: Track various communication system states
 */
extern int conq_news_status;    /* Conquer news system status */
extern int conq_mail_status;    /* Conquer mail system status */
extern int god_browsing;        /* God/administrator browsing mode flag */

/*
 * System Mail Variables (Conditional Compilation)
 * Purpose: External system mail integration when SYSMAIL is enabled
 * Usage: Only compiled when SYSMAIL preprocessor flag is defined
 */
#ifdef SYSMAIL
extern int sys_mail_status;    /* System mail status indicator */
extern char sysmail[];         /* System mail file path or buffer */
#endif /*SYSMAIL*/

/*
 * Display Configuration Arrays
 * Purpose: Store display configuration limits and constraints
 */
extern int cv_max[];              /* Maximum values for various display elements */

/*
 * Display Content String Arrays
 * Purpose: Store string arrays for various display elements and selections
 */
extern char *display_list[];      /* Display mode option strings */
extern char *highl_list[];        /* Highlighting option strings */
extern char *cv_name[];           /* Display element name strings */
extern char *desg_selects[];      /* Designation selection strings */
extern char *shortspeed[];        /* Abbreviated speed indicator strings */
extern char *shortdir[];          /* Abbreviated direction strings */
extern char *shipsize[];          /* Ship size description strings */
extern char *min_desg_selects[];  /* Minimal designation selection strings */

/*
 * Status and Location String Variables
 * Purpose: Store dynamically updated status and location information
 */
extern char *sectstat;            /* Sector status information string */
extern char *trooplocs;           /* Troop location information string */

/*
 * Display Mode Structure
 * Purpose: Current display configuration and mode settings
 */
extern DISPLAY_STRUCT display_mode;  /* Current display mode configuration structure */

/* ============================================================================
 * REGULAR EXPRESSION SUPPORT (Conditional Compilation)
 * ============================================================================
 * Purpose: Provide regular expression functionality when REGEXP is enabled
 * Usage: Pattern matching, text search, input validation
 * Notes: Legacy regex interface, consider POSIX regex replacement
 */

#ifdef REGEXP
/*
 * Regular Expression Function Declarations
 * Purpose: Provide pattern matching capabilities for text processing
 * Usage: Search operations, input validation, text filtering
 * Notes: Legacy regex interface, not POSIX-compliant
 * Modernization: Consider replacing with POSIX regex (regcomp, regexec)
 */

/*
 * re_exec - Execute regular expression match against string
 *
 * Executes a previously compiled regular expression against a target string
 * to determine if the pattern matches. This is part of the legacy regex
 * interface used for text processing and search operations.
 *
 * Returns:
 *   1 if pattern matches, 0 if no match, -1 on error
 *
 * Notes:
 *   - Requires prior call to re_comp() to compile pattern
 *   - Legacy interface, consider POSIX regex replacement
 *   - Not thread-safe due to global state
 */
extern int re_exec();

/*
 * re_comp - Compile regular expression pattern
 *
 * Compiles a regular expression pattern string into internal format for
 * subsequent matching operations. This is part of the legacy regex interface
 * used throughout the system for pattern matching.
 *
 * Returns:
 *   NULL on successful compilation, error message string on failure
 *
 * Notes:
 *   - Stores compiled pattern in global state
 *   - Legacy interface, consider POSIX regex replacement
 *   - Not thread-safe due to global state storage
 */
extern char *re_comp();
#endif /* REGEXP */

/* ============================================================================
 * FUNCTION PROTOTYPE INCLUSION
 * ============================================================================
 * Purpose: Include function prototype declarations for user interface
 * Usage: Provides function declarations for all UI-related functions
 * Notes: Conditional inclusion to support code extraction tools
 */

/*
 * Function Prototype File Inclusion
 * Purpose: Include fileG.h containing UI function prototypes
 * Usage: Provides function declarations for external linkage
 * Notes: Excluded when __CEXTRACT__ is defined to support code extraction
 */
#ifndef __CEXTRACT__
#include "fileG.h"
#endif /* __CEXTRACT__ */

#endif /*DATA_DECLARE*/
