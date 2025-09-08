/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * infoG.h - Information Display System Definitions
 *
 * This header defines the information display management system for the Conquer
 * game's user interface. It provides enumerated types and constants that control
 * how different game entities and their detailed information are presented to
 * the player through various information screens and displays.
 *
 * Key Components:
 *   - Information screen type definitions for different game entities
 *   - City information display mode specifications
 *   - Information screen management constants and limits
 *   - Display mode selection and navigation support
 *
 * Integration Notes:
 *   - Used by: infoG.c (information display implementation)
 *   - Used by: displayG.c (main display coordination)
 *   - Used by: mainG.c (UI command processing and screen navigation)
 *   - Depends on: header.h (basic type definitions)
 *   - Provides: Information display type system for UI management
 *
 * Modernization Considerations:
 *   - Enum values could benefit from explicit numbering for save compatibility
 *   - INFO_MAX constant enables compile-time array sizing and bounds checking
 *   - Display modes support extensible information presentation
 *   - Clean separation of display types enables modular UI development
 */

/* ============================================================================
 * INFORMATION SCREEN TYPE SYSTEM
 * ============================================================================
 * Purpose: Define the different types of detailed information screens available
 * Usage: Screen selection, display routing, and UI navigation control
 * Notes: Each type corresponds to a major game entity with detailed displays
 */

/*
 * INFO_MAX - Maximum number of information screen types
 *
 * Defines the upper limit for information screen types in the game's UI system.
 * This constant enables compile-time array allocation for information screen
 * management, display routing tables, and UI navigation structures.
 *
 * Value: 7 information screen types supported
 * Usage: Array sizing, bounds checking, UI navigation limits
 * Notes: Should be updated if new information screen types are added
 */
#define INFO_MAX	7

/*
 * enum infotype - Information Screen Type Definitions
 *
 * Defines the different types of detailed information screens available in the
 * game's user interface. Each type represents a major game entity that has
 * comprehensive information displays with multiple data views, statistics,
 * and management options for player review and decision-making.
 *
 * Usage Patterns:
 *   - Screen selection and routing in UI navigation
 *   - Display mode determination and content switching
 *   - Information presentation coordination between display modules
 *
 * Relationships:
 *   - Used by infoG.c for information display implementation
 *   - Coordinates with displayG.h display management system
 *   - Integrates with selectG.h entity selection system
 *
 * Values:
 *   INFO_ARMY   - Individual army unit detailed information screen
 *   INFO_GRPARM - Army group collective information and management screen
 *   INFO_NAVY   - Individual naval unit detailed information screen
 *   INFO_CVN    - Caravan detailed information and trade management screen
 *   INFO_CITY   - City detailed information with economic and resource data
 *   INFO_ITEM   - Item detailed information with properties and usage data
 *   INFO_NTN    - Nation comprehensive information and diplomatic status
 *   INFO_DIP    - Diplomatic information and international relations screen
 */
typedef enum infotype {
  INFO_ARMY,    /* Individual army unit information: stats, equipment, orders */
  INFO_GRPARM,  /* Army group information: collective stats, coordination */
  INFO_NAVY,    /* Naval unit information: ship stats, cargo, naval orders */
  INFO_CVN,     /* Caravan information: trade goods, routes, profit analysis */
  INFO_CITY,    /* City information: population, production, economy, defenses */
  INFO_ITEM,    /* Item information: properties, effects, usage requirements */
  INFO_NTN,     /* Nation information: overview, statistics, global status */
  INFO_DIP      /* Diplomatic information: relations, treaties, negotiations */
} Infotype;

/* ============================================================================
 * CITY INFORMATION DISPLAY MODES
 * ============================================================================
 * Purpose: Define specific display modes for city information screens
 * Usage: City information screen content selection and data presentation
 * Notes: Enables detailed city management through specialized information views
 */

/*
 * enum cinfotype - City Information Display Mode Definitions
 *
 * Defines the specific display modes available for city information screens,
 * allowing players to view different aspects of city management and status
 * through specialized information presentations. Each mode focuses on specific
 * city management areas with detailed data and analysis.
 *
 * Usage Patterns:
 *   - City information screen mode selection and switching
 *   - Content filtering and data presentation control
 *   - City management workflow organization and navigation
 *
 * Relationships:
 *   - Used within INFO_CITY information screen type
 *   - Coordinates with city management systems in cityX.h
 *   - Integrates with economic computation systems
 *
 * Values:
 *   CINFO_RESOURCE - Resource production, consumption, and availability
 *   CINFO_ECONOMY  - Economic analysis, trade, taxation, and financial status
 *   CINFO_SUMMARY  - General city overview with key statistics and status
 */
typedef enum cinfotype {
  CINFO_RESOURCE, /* Resource view: production, stockpiles, supply/demand */
  CINFO_ECONOMY,  /* Economic view: taxation, trade, financial analysis */
  CINFO_SUMMARY   /* Summary view: general overview, key stats, status */
} Cinfotype;
