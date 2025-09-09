/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * dstatusX.h - Diplomacy Status System Definitions and Nation Relationship Framework
 *
 * This header defines the comprehensive diplomacy status system that governs
 * relationships between nations in the game world. The system provides a
 * hierarchical scale of diplomatic relations from complete hostility to full
 * alliance, with corresponding behavioral and mechanical implications.
 *
 * Key Components:
 *   - Diplotype enum: Complete diplomatic relationship enumeration (10 levels)
 *   - dipname array: Human-readable string representations for each status
 *   - dstatus_number: System configuration and validation support
 *
 * Integration Notes:
 *   - Used by: Nation management, combat systems, trade mechanics, AI behavior
 *   - Depends on: dataX.c for string array definitions and system constants
 *   - Provides: Core diplomatic framework for all inter-nation interactions
 *
 * Diplomatic Hierarchy:
 *   The system implements a 10-level diplomatic spectrum from DIP_UNMET (unknown
 *   nations) through various levels of cooperation and conflict to DIP_JIHAD
 *   (total war). Each level affects trade access, military cooperation, territory
 *   passage, information sharing, and combat behavior.
 *
 * Modernization Considerations:
 *   - Enum values could benefit from explicit integer assignments for save compatibility
 *   - String array dependency on dataX.c creates coupling - consider const char*
 *   - Additional diplomatic states may be needed for complex alliance systems
 *   - Bitfield representation could enable compound diplomatic modifiers
 */
/* ============================================================================
 * DIPLOMACY STATUS ENUMERATION
 * ============================================================================
 * Purpose: Complete diplomatic relationship spectrum for inter-nation relations
 * Usage: Used throughout game systems to determine interaction behavior
 * Notes: 10-level hierarchy from unknown to total war with behavioral implications
 */

/*
 * enum diplotype - Diplomatic relationship status enumeration
 *
 * Defines the complete spectrum of diplomatic relationships between nations,
 * from initial contact through various levels of cooperation and conflict.
 * Each status level determines available interactions, trade access, military
 * cooperation, territory passage rights, and AI behavioral patterns.
 *
 * Diplomatic Spectrum (10 levels):
 *   DIP_UNMET     - Nations have not yet established contact (unknown status)
 *   DIP_ALLIED    - Full military and economic alliance (highest cooperation)
 *   DIP_TREATY    - Formal treaty with mutual obligations and benefits
 *   DIP_FRIENDLY  - Positive relations with trade and cooperation bonuses
 *   DIP_PEACEFUL  - Non-aggression pact with limited cooperation
 *   DIP_NEUTRAL   - Default neutral status with standard interactions
 *   DIP_HOSTILE   - Negative relations with trade restrictions and tension
 *   DIP_BELLICOSE - Pre-war state with military posturing and preparations
 *   DIP_WAR       - Active warfare state with combat and territorial conflict
 *   DIP_JIHAD     - Total war with complete resource commitment (highest conflict)
 *
 * Game Mechanical Effects:
 *   - Trade access and pricing modifiers based on relationship level
 *   - Military cooperation and unit sharing for allied/treaty nations
 *   - Territory passage rights and movement restrictions
 *   - Information sharing and intelligence cooperation levels
 *   - AI behavioral patterns and diplomatic action preferences
 *   - Combat bonuses/penalties when fighting alongside or against nations
 *
 * Usage Patterns:
 *   - Stored in nation data structures for bilateral relationships
 *   - Used by trade systems to determine access and pricing
 *   - Referenced by combat systems for alliance behavior
 *   - Consulted by AI systems for diplomatic decision making
 *   - Used by UI systems for relationship display and color coding
 *
 * Modernization Notes:
 *   - Consider explicit integer values for save file compatibility
 *   - Enum size affects memory usage in nation relationship matrices
 *   - Additional states may be needed for complex multi-party alliances
 */
typedef enum diplotype {
  DIP_UNMET,     /* Nations have not established contact */
  DIP_ALLIED,    /* Full military and economic alliance */
  DIP_TREATY,    /* Formal treaty with mutual obligations */
  DIP_FRIENDLY,  /* Positive relations with cooperation bonuses */
  DIP_PEACEFUL,  /* Non-aggression pact with limited cooperation */
  DIP_NEUTRAL,   /* Default neutral status with standard interactions */
  DIP_HOSTILE,   /* Negative relations with trade restrictions */
  DIP_BELLICOSE, /* Pre-war state with military preparations */
  DIP_WAR,       /* Active warfare state with territorial conflict */
  DIP_JIHAD      /* Total war with complete resource commitment */
} Diplotype;

/* ============================================================================
 * DIPLOMACY SYSTEM SUPPORT VARIABLES
 * ============================================================================
 * Purpose: Runtime support for diplomacy status display and validation
 * Usage: Referenced by UI systems and diplomacy management functions
 * Notes: String array provides human-readable names, counter enables validation
 */

/*
 * Global Diplomacy Support Variables
 *
 * These variables provide runtime support for the diplomacy status system,
 * enabling user interface display, configuration validation, and system
 * maintenance operations.
 *
 * Variable Definitions:
 *   dipname[]       - String array containing human-readable diplomatic status names
 *   dstatus_number  - Count of diplomatic status levels for validation and iteration
 *
 * Usage Patterns:
 *   - dipname[] used by UI systems to display current diplomatic relationships
 *   - Referenced by report generation for diplomatic status summaries
 *   - Used by configuration interfaces for diplomatic option selection
 *   - dstatus_number used for array bounds checking and validation
 *   - Enables iteration over all diplomatic status levels for system operations
 *
 * Implementation Notes:
 *   - dipname[] array defined in dataX.c with string constants
 *   - Array size must match the number of Diplotype enum values (10)
 *   - dstatus_number should equal the count of diplomatic status levels
 *   - Used for validation of diplomatic status values in save files and user input
 *
 * Memory Management:
 *   - String array uses static storage with program lifetime
 *   - No dynamic allocation required for diplomacy status names
 *   - Strings are read-only and shared across all diplomatic operations
 *
 * Modernization Considerations:
 *   - Consider const char* declaration for string array immutability
 *   - Array size validation should use compile-time checks
 *   - String localization support may require dynamic lookup tables
 */

/*
 * dipname - Array of human-readable diplomatic status names
 *
 * Contains string representations for each diplomatic status level,
 * used by user interfaces, reports, and configuration systems to
 * display current diplomatic relationships in readable form.
 *
 * Array Contents: Corresponds 1:1 with Diplotype enum values
 * Definition: Implemented in dataX.c as static string array
 * Usage: Referenced by UI display and reporting functions
 * Size: Must contain exactly 10 strings matching Diplotype enum
 */
extern char *dipname[];

/*
 * dstatus_number - Count of diplomatic status levels
 *
 * Provides the total number of diplomatic status levels for validation,
 * bounds checking, and iteration operations throughout the diplomacy system.
 *
 * Value: Should equal 10 (number of Diplotype enum values)
 * Usage: Used for array bounds checking and validation operations
 * Validation: Ensures diplomatic status values are within valid range
 * Iteration: Enables loops over all diplomatic status levels
 */
extern int dstatus_number;
