/*
 * stringX.h - String Processing Type Definitions and Classification System
 *
 * This header defines string classification constants used throughout the
 * Conquer game engine for type-safe string processing, input validation,
 * and context-specific string handling. These constants enable the string
 * processing system to apply appropriate validation, formatting, and
 * expansion rules based on the intended use of each string.
 *
 * Purpose:
 *   - String type classification for input validation systems
 *   - Context-aware string processing and formatting
 *   - Type-safe string expansion and completion
 *   - Input field validation for user interface elements
 *   - String processing behavior control
 *
 * Usage Context:
 *   - Used by: String processing functions (convertX.c)
 *   - Used by: User interface input validation (ioX.c)
 *   - Used by: Command parsing and expansion systems
 *   - Used by: Text formatting and display functions
 *   - Provides: Type safety for string processing operations
 *
 * Integration:
 *   - Called by get_string() function for input validation
 *   - Used in string expansion and completion systems
 *   - Applied in user interface field validation
 *   - Enables context-specific string processing rules
 *
 * String Processing Framework:
 *   Each string class defines specific validation rules, allowed characters,
 *   formatting requirements, and expansion behavior for different types of
 *   game data input. This enables robust input validation and consistent
 *   string handling across the entire game interface.
 *
 * Modernization Notes:
 *   - These constants should be converted to enum for type safety
 *   - Consider namespace protection to avoid naming conflicts
 *   - String validation should use modern secure practices
 *   - Input validation should include bounds checking
 *   - Consider Unicode support for internationalization
 *
 * Historical Context:
 *   - Original string classification system from 1992
 *   - Designed for terminal-based user interface validation
 *   - Provides foundation for all string input processing
 *   - Critical for maintaining data integrity in user input
 */

/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* ============================================================================
 * STRING CLASSIFICATION CONSTANTS
 * ============================================================================
 * Purpose: Define string types for context-specific input validation
 * Usage: Enable type-safe string processing with appropriate validation rules
 * Total: 19 string classification types for comprehensive input handling
 */

/*
 * STR_NORM - Normal/Generic String Input
 *
 * Standard string input with basic validation. Allows most printable
 * characters with minimal restrictions. Used for general text input
 * where no specific format requirements apply.
 *
 * Validation Rules:
 *   - Allows alphanumeric characters and common punctuation
 *   - Basic length limits apply
 *   - No special expansion or completion behavior
 *
 * Usage: Default string input, comments, descriptions, general text
 */
#define STR_NORM	0

/*
 * STR_NAME - Player/Nation Name Input
 *
 * String validation for player names and nation names. Enforces naming
 * conventions and character restrictions appropriate for identifiers
 * in the game system.
 *
 * Validation Rules:
 *   - Alphanumeric characters plus limited punctuation
 *   - Length restrictions for display compatibility
 *   - May include automatic capitalization
 *   - Prevents duplicate names in validation context
 *
 * Usage: Player names, nation names, character identifiers
 */
#define STR_NAME	1

/*
 * STR_XNAME - Extended Name Input
 *
 * Extended naming validation with broader character set than STR_NAME.
 * Allows more flexible naming conventions while maintaining basic
 * validation requirements.
 *
 * Validation Rules:
 *   - Expanded character set including spaces and extended punctuation
 *   - Longer length limits than STR_NAME
 *   - Maintains uniqueness checking where appropriate
 *
 * Usage: Extended names, titles, descriptive identifiers
 */
#define STR_XNAME	2

/*
 * STR_CITY - City Name Input and Selection
 *
 * String processing for city names with expansion and completion
 * capabilities. Enables partial name matching and automatic completion
 * from existing city lists.
 *
 * Validation Rules:
 *   - City naming conventions (capitals, length limits)
 *   - Automatic completion from existing city names
 *   - Uniqueness validation within nation/region
 *   - Geographic naming pattern support
 *
 * Usage: City naming, city selection, urban area identification
 */
#define STR_CITY	3

/*
 * STR_ARMY - Army Unit Name and Selection
 *
 * String processing for army unit identification with automatic
 * expansion and unit number completion. Supports both numeric
 * and named unit identification.
 *
 * Validation Rules:
 *   - Army naming conventions and numbering systems
 *   - Automatic completion from existing army units
 *   - Unit type and class validation
 *   - Military hierarchy naming support
 *
 * Usage: Army unit selection, military unit naming, unit commands
 */
#define STR_ARMY	4

/*
 * STR_SPACE - Coordinate and Spatial Input
 *
 * String processing for spatial coordinates, map locations, and
 * geographic references. Handles coordinate parsing and validation
 * for map-based operations.
 *
 * Validation Rules:
 *   - Coordinate format validation (x,y or x:y patterns)
 *   - Map boundary checking and validation
 *   - Relative and absolute coordinate support
 *   - Sector reference format compliance
 *
 * Usage: Map coordinates, location input, spatial references
 */
#define STR_SPACE	5

/*
 * STR_STATUS - Unit Status and State Input
 *
 * String processing for unit status values and state designations.
 * Enables selection and validation of unit operational states
 * and status conditions.
 *
 * Validation Rules:
 *   - Valid status code enumeration
 *   - Status compatibility checking
 *   - Automatic expansion from status keywords
 *   - Context-appropriate status validation
 *
 * Usage: Unit status setting, operational state selection
 */
#define STR_STATUS	6

/*
 * STR_SPEED - Speed and Movement Input
 *
 * String processing for movement speed values and mobility settings.
 * Handles numeric speed input with range validation and unit-specific
 * speed limit checking.
 *
 * Validation Rules:
 *   - Numeric speed value validation
 *   - Unit-specific speed limit enforcement
 *   - Movement type compatibility checking
 *   - Speed increment and granularity rules
 *
 * Usage: Unit speed setting, movement configuration, mobility control
 */
#define STR_SPEED	7

/*
 * STR_MONTH - Month and Time Period Input
 *
 * String processing for month values and time period specifications.
 * Supports both numeric and named month input with calendar
 * validation and date processing.
 *
 * Validation Rules:
 *   - Month name expansion (Jan -> January)
 *   - Numeric month validation (1-12)
 *   - Calendar system compatibility
 *   - Time period format compliance
 *
 * Usage: Date input, calendar operations, time period specification
 */
#define STR_MONTH	8

/*
 * STR_DESG - Sector Designation Input
 *
 * String processing for sector designation selection and validation.
 * Handles terrain type input, land use designation, and sector
 * classification with automatic expansion and validation.
 *
 * Validation Rules:
 *   - Valid designation type enumeration
 *   - Terrain compatibility checking
 *   - Designation abbreviation expansion
 *   - Cost and requirement validation
 *
 * Usage: Sector designation, terrain modification, land use planning
 */
#define STR_DESG	9

/*
 * STR_DMODES - Display Mode Selection
 *
 * String processing for display mode configuration and selection.
 * Enables selection of map display modes, information views, and
 * interface configuration options.
 *
 * Validation Rules:
 *   - Valid display mode enumeration
 *   - Mode compatibility checking
 *   - Display capability validation
 *   - Mode abbreviation expansion
 *
 * Usage: Display configuration, map mode selection, interface setup
 */
#define STR_DMODES	10

/*
 * STR_ALT - Alternative/Alternate Input Processing
 *
 * String processing for alternative input methods and special
 * processing modes. Provides alternative validation and expansion
 * rules for context-specific input requirements.
 *
 * Validation Rules:
 *   - Context-dependent validation rules
 *   - Alternative character set support
 *   - Special expansion behavior
 *   - Flexible input format acceptance
 *
 * Usage: Alternative input modes, special contexts, flexible parsing
 */
#define STR_ALT		11

/*
 * STR_TGCLASS - Trade Good Class Selection
 *
 * String processing for trade good class identification and selection.
 * Handles economic category input with automatic expansion and
 * trade system validation.
 *
 * Validation Rules:
 *   - Valid trade good class enumeration
 *   - Economic category validation
 *   - Class abbreviation expansion
 *   - Trade system compatibility checking
 *
 * Usage: Trade good classification, economic planning, resource management
 */
#define STR_TGCLASS	12

/*
 * STR_VEG - Vegetation and Resource Input
 *
 * String processing for vegetation types and natural resource
 * designation. Handles environmental and ecological input with
 * resource system validation.
 *
 * Validation Rules:
 *   - Valid vegetation type enumeration
 *   - Resource compatibility checking
 *   - Environmental constraint validation
 *   - Ecological system compliance
 *
 * Usage: Environmental designation, resource specification, ecological planning
 */
#define STR_VEG		13

/*
 * STR_TGOOD - Trade Good Item Selection
 *
 * String processing for specific trade good items and commodities.
 * Enables selection and validation of individual trade items with
 * economic system integration.
 *
 * Validation Rules:
 *   - Valid trade good item enumeration
 *   - Item availability validation
 *   - Economic value checking
 *   - Trade route compatibility
 *
 * Usage: Trade good selection, commodity trading, economic transactions
 */
#define STR_TGOOD	14

/*
 * STR_OPTION - Configuration Option Input
 *
 * String processing for configuration options and system settings.
 * Handles option selection and validation for game configuration
 * and user preference management.
 *
 * Validation Rules:
 *   - Valid configuration option enumeration
 *   - Option value range validation
 *   - Setting compatibility checking
 *   - Configuration constraint compliance
 *
 * Usage: Game configuration, user preferences, system settings
 */
#define STR_OPTION	15

/*
 * STR_FUNCS - Function and Command Input
 *
 * String processing for function names and command input. Enables
 * command expansion, function selection, and interactive command
 * processing with validation and completion.
 *
 * Validation Rules:
 *   - Valid function/command enumeration
 *   - Command syntax validation
 *   - Function availability checking
 *   - Parameter compatibility validation
 *
 * Usage: Command input, function selection, interactive command processing
 */
#define STR_FUNCS	16

/*
 * STR_MINDESG - Minor Designation Input
 *
 * String processing for minor sector designations and supplementary
 * land use classifications. Handles secondary designation input with
 * compatibility validation.
 *
 * Validation Rules:
 *   - Valid minor designation enumeration
 *   - Primary designation compatibility
 *   - Resource requirement validation
 *   - Construction feasibility checking
 *
 * Usage: Minor designations, supplementary land use, sector enhancement
 */
#define STR_MINDESG	17

/*
 * STR_ACLASS - Army Class Selection
 *
 * String processing for army class identification and military unit
 * classification. Handles unit class selection with military system
 * validation and expansion capabilities.
 *
 * Validation Rules:
 *   - Valid army class enumeration
 *   - Military classification compatibility
 *   - Unit capability validation
 *   - Class abbreviation expansion
 *
 * Usage: Army classification, military unit selection, unit type specification
 */
#define STR_ACLASS	18
