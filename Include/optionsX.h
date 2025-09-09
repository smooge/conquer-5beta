/*
 * optionsX.h - User Configuration Options System
 *
 * This header defines the comprehensive user preference and configuration system
 * that allows players to customize their game experience. It provides a unified
 * framework for interface behavior, display settings, key bindings, and user
 * interaction preferences that persist across game sessions.
 *
 * The options system enables extensive customization of:
 * - User interface appearance and behavior settings
 * - Key binding configuration and input method preferences
 * - Display formatting and information presentation modes
 * - Paging and scrolling behavior for text interfaces
 * - Expert mode settings for advanced users
 * - Mail and notification system preferences
 *
 * Key Components:
 *   - Option identifier constants for all configurable settings
 *   - Interface customization options (display, layout, formatting)
 *   - Input and navigation preference settings
 *   - Expert mode and advanced user configuration
 *   - Notification and alert system preferences
 *
 * Integration Notes:
 *   - Used by: User interface systems, configuration management, key binding
 *   - Depends on: dataX.c (option string definitions), header.h (basic types)
 *   - Provides: Option enumeration, configuration constants, preference framework
 *
 * Modernization Considerations:
 *   - Consider enum for type safety instead of #define constants
 *   - Add validation for option value ranges and types
 *   - Implement structured configuration with categories
 *   - Consider JSON or similar format for complex configuration data
 */

/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* ============================================================================
 * USER CONFIGURATION OPTIONS - Interface and Behavior Customization
 * ============================================================================
 * Purpose: Define all configurable user preference settings
 * Usage: Array indices for option data structures and configuration access
 * Notes: Option strings and default values defined in dataX.c
 */

/*
 * OPT_ALLBLANKS - Display formatting option for blank line handling
 *
 * Controls whether empty lines and blank spaces are displayed in various
 * interface contexts. This setting affects information display density
 * and can be used to create more compact or more spacious layouts.
 *
 * Value: 0 (primary option array index)
 * Usage: Information display, report formatting, interface layout
 * Effect: Toggles blank line display in text output and reports
 * User Benefit: Customizable information density for reading preferences
 */
#define OPT_ALLBLANKS	0

/*
 * OPT_BINDKEY - Key binding configuration option
 *
 * Enables or controls the key binding system that allows users to customize
 * keyboard shortcuts and input mappings. This setting determines whether
 * custom key bindings are active and how they are processed.
 *
 * Value: 1 (secondary option array index)
 * Usage: Input processing, key binding system, user interface control
 * Effect: Enables/disables custom key binding functionality
 * User Benefit: Personalized keyboard shortcuts for efficient gameplay
 */
#define OPT_BINDKEY	1

/*
 * OPT_BOTTOMLINES - Bottom status line display configuration
 *
 * Controls the display and behavior of status lines at the bottom of the
 * screen interface. This setting affects how many lines are reserved for
 * status information and how they are formatted and updated.
 *
 * Value: 2 (tertiary option array index)
 * Usage: Screen layout, status display, interface organization
 * Effect: Configures bottom status line count and behavior
 * User Benefit: Customizable status information display area
 */
#define OPT_BOTTOMLINES	2

/*
 * OPT_CHECKKEYS - Input validation and key checking option
 *
 * Enables or configures input validation for keyboard commands and shortcuts.
 * This setting determines whether the system performs additional checking
 * on user input to prevent invalid commands or accidental actions.
 *
 * Value: 3 (quaternary option array index)
 * Usage: Input processing, command validation, error prevention
 * Effect: Enables/disables enhanced input checking and validation
 * User Benefit: Prevents accidental commands and provides input feedback
 */
#define OPT_CHECKKEYS	3

/*
 * OPT_EXPERT - Expert mode configuration setting
 *
 * Enables expert mode which modifies the interface to provide advanced
 * functionality, reduced prompting, and more efficient operation for
 * experienced users. Expert mode typically removes confirmation dialogs
 * and provides more direct access to advanced features.
 *
 * Value: 4 (fifth option array index)
 * Usage: Interface behavior, prompt display, advanced feature access
 * Effect: Toggles expert mode interface and reduced confirmation prompts
 * User Benefit: Streamlined interface for experienced players
 */
#define OPT_EXPERT	4

/*
 * OPT_GAUDY - Visual style and decoration option
 *
 * Controls the visual style and decorative elements of the user interface.
 * This setting affects the use of visual enhancements, colors, borders,
 * and other aesthetic elements that can make the interface more visually
 * appealing or more minimalist based on user preference.
 *
 * Value: 5 (sixth option array index)
 * Usage: Visual display, interface styling, aesthetic preferences
 * Effect: Toggles decorative visual elements and enhanced styling
 * User Benefit: Customizable visual appearance for personal preference
 */
#define OPT_GAUDY	5

/*
 * OPT_HEADERS - Header display configuration option
 *
 * Controls the display of headers in various interface contexts such as
 * reports, lists, and information displays. This setting determines whether
 * column headers, section titles, and other organizational headers are shown.
 *
 * Value: 6 (seventh option array index)
 * Usage: Report formatting, list display, information organization
 * Effect: Toggles header display in reports and information screens
 * User Benefit: Customizable information organization and readability
 */
#define OPT_HEADERS	6

/*
 * OPT_INFOMODE - Information display mode configuration
 *
 * Configures the mode and format for information display throughout the
 * interface. This setting affects how detailed information is presented,
 * the level of detail shown, and the formatting style used for data display.
 *
 * Value: 7 (eighth option array index)
 * Usage: Information display, detail level, formatting preferences
 * Effect: Controls information display mode and detail level
 * User Benefit: Customizable information detail and presentation style
 */
#define OPT_INFOMODE	7

/*
 * OPT_MAILCHECK - Mail notification and checking option
 *
 * Controls the automatic checking and notification system for in-game mail
 * and messages. This setting determines whether the system automatically
 * checks for new mail and how notifications are displayed to the user.
 *
 * Value: 8 (ninth option array index)
 * Usage: Mail system, notification display, automatic checking
 * Effect: Enables/disables automatic mail checking and notifications
 * User Benefit: Customizable mail notification preferences
 */
#define OPT_MAILCHECK	8

/*
 * OPT_PAGEOFFSET - Page display offset configuration
 *
 * Controls the starting position or offset for paged display of information.
 * This setting affects where the display begins when showing multi-page
 * content and can be used to adjust for different terminal or display sizes.
 *
 * Value: 9 (tenth option array index)
 * Usage: Paging system, display positioning, screen layout
 * Effect: Configures starting offset for paged information display
 * User Benefit: Customizable page positioning for optimal viewing
 */
#define OPT_PAGEOFFSET	9

/*
 * OPT_PAGESCROLL - Page scrolling behavior option
 *
 * Configures how scrolling behaves when navigating through multi-page
 * content. This setting controls scroll speed, direction, and the number
 * of lines scrolled per operation for optimal navigation experience.
 *
 * Value: 10 (eleventh option array index)
 * Usage: Navigation, scrolling behavior, page movement
 * Effect: Controls page scrolling speed and behavior
 * User Benefit: Customizable scrolling for comfortable navigation
 */
#define OPT_PAGESCROLL	10

/*
 * OPT_PAGETAB - Page tabulation and formatting option
 *
 * Controls tabulation and column formatting in paged displays. This setting
 * affects how columns are aligned, tab stops are set, and text is formatted
 * in tabular displays and reports throughout the interface.
 *
 * Value: 11 (twelfth option array index)
 * Usage: Text formatting, column alignment, tabular display
 * Effect: Configures tab stops and column formatting in paged content
 * User Benefit: Customizable text formatting for improved readability
 */
#define OPT_PAGETAB	11

/*
 * OPT_READ - Reading mode and text display configuration
 *
 * Configures reading mode settings that affect how text is displayed and
 * formatted for optimal reading. This setting can control line spacing,
 * text wrapping, and other factors that improve text readability.
 *
 * Value: 12 (thirteenth option array index)
 * Usage: Text display, reading optimization, formatting preferences
 * Effect: Controls text display formatting for improved readability
 * User Benefit: Optimized text display for comfortable reading
 */
#define OPT_READ	12

/*
 * OPT_REBIND - Key rebinding configuration option
 *
 * Enables or controls the ability to rebind keys and modify existing key
 * bindings. This setting determines whether users can change existing
 * keyboard shortcuts and create new input mappings for game commands.
 *
 * Value: 13 (fourteenth option array index)
 * Usage: Key binding system, input customization, command mapping
 * Effect: Enables/disables key rebinding functionality
 * User Benefit: Ability to customize and modify keyboard shortcuts
 */
#define OPT_REBIND	13

/*
 * OPT_CENTER - Display centering and alignment option
 *
 * Controls centering and alignment of display elements on the screen.
 * This setting affects how content is positioned relative to the screen
 * boundaries and can be used to optimize display for different screen sizes.
 *
 * Value: 14 (fifteenth option array index)
 * Usage: Display layout, content positioning, screen alignment
 * Effect: Controls centering and alignment of interface elements
 * User Benefit: Optimized display positioning for screen size and preference
 */
#define OPT_CENTER	14

/*
 * OPT_RESETKEYS - Key binding reset configuration
 *
 * Controls the ability to reset key bindings to default values and restore
 * original keyboard shortcuts. This setting provides a way to undo
 * customizations and return to standard input configuration.
 *
 * Value: 15 (sixteenth option array index)
 * Usage: Key binding management, configuration reset, default restoration
 * Effect: Enables/disables key binding reset functionality
 * User Benefit: Ability to restore default keyboard configuration
 */
#define OPT_RESETKEYS	15

/*
 * OPT_STORE - Data storage and persistence option
 *
 * Controls how user preferences and configuration data are stored and
 * persisted across game sessions. This setting affects whether customizations
 * are saved automatically and how configuration data is managed.
 *
 * Value: 16 (seventeenth option array index)
 * Usage: Configuration persistence, data storage, preference management
 * Effect: Controls automatic storage of user preferences and settings
 * User Benefit: Persistent customization across game sessions
 */
#define OPT_STORE	16

/*
 * OPT_SUPPLY - Supply and resource display option
 *
 * Configures the display and monitoring of supply and resource information.
 * This setting affects how resource levels, supply status, and related
 * economic information are presented in the interface.
 *
 * Value: 17 (eighteenth option array index)
 * Usage: Resource display, supply monitoring, economic information
 * Effect: Controls supply and resource information display
 * User Benefit: Customizable resource monitoring and display preferences
 */
#define OPT_SUPPLY	17

/*
 * OPT_TBELL - Terminal bell and audio notification option
 *
 * Controls the use of terminal bell and audio notifications for alerts
 * and important events. This setting determines whether the system uses
 * audible alerts and how they are configured for different types of events.
 *
 * Value: 18 (nineteenth option array index)
 * Usage: Audio notifications, alert system, terminal bell control
 * Effect: Enables/disables terminal bell and audio alerts
 * User Benefit: Customizable audio notification preferences
 */
#define OPT_TBELL	18

/*
 * OPT_UNBIND - Key unbinding configuration option
 *
 * Enables the ability to unbind keys and remove existing keyboard shortcuts.
 * This setting controls whether users can disable specific key bindings
 * and remove unwanted or conflicting input mappings.
 *
 * Value: 19 (twentieth option array index)
 * Usage: Key binding management, input customization, binding removal
 * Effect: Enables/disables key unbinding functionality
 * User Benefit: Ability to remove unwanted keyboard shortcuts
 */
#define OPT_UNBIND	19

/*
 * OPT_WATER_BTM - Water display bottom configuration
 *
 * Controls the display of water and terrain information at the bottom
 * of the interface. This setting affects how geographical and terrain
 * information is presented in the status or information areas.
 *
 * Value: 20 (twenty-first option array index)
 * Usage: Terrain display, geographical information, status area
 * Effect: Controls water and terrain information display position
 * User Benefit: Customizable terrain information placement and visibility
 */
#define OPT_WATER_BTM	20
