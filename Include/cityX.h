/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * cityX.h - City System Accessor Macros and Urban Management Interface
 *
 * This header defines the complete city system interface for the Conquer strategy game,
 * providing convenient accessor macros for city data manipulation, urban settlement
 * management, and optional code generation for debugging and replay functionality.
 *
 * The city system forms a core component of the economic and territorial control
 * mechanics, managing urban settlements including population, resources, fortifications,
 * and economic production. Cities serve as population centers, resource collection
 * points, and defensive strongholds within the strategic gameplay framework.
 *
 * Key Components:
 * - City Data Accessor Macros: Convenient field access for city structures
 * - Type-Safe Access Patterns: Both direct (city_ptr) and template (city_tptr) access
 * - Code Generation System: Optional debugging and replay functionality
 * - Urban Management Interface: Complete city manipulation and monitoring system
 *
 * Integration:
 * - Used by cityA.c for city management operations
 * - Integrated with dataX.h CITY_STRUCT definitions
 * - Connected to economic system via material and talon management
 * - Linked to military system through fortress and defensive capabilities
 * - Supports territorial control and population management mechanics
 *
 * Modernization Notes:
 * - Consider converting macros to inline functions for type safety
 * - Evaluate const-correctness for read-only operations
 * - Review pointer access patterns for null safety
 * - Assess numeric type safety for population and resource values
 */

/*
 * =============================================================================
 * CITY DATA ACCESSOR MACROS - Urban Settlement Field Access Interface
 * =============================================================================
 *
 * These macros provide convenient access to city structure fields via global
 * city_ptr and city_tptr pointers. The dual pointer system supports both
 * direct city manipulation (city_ptr) and template-based operations (city_tptr).
 *
 * Accessor Pattern:
 * - CITY_* macros: Access via city_ptr for current city operations
 * - CITYT_* macros: Access via city_tptr for template/comparison operations
 *
 * City Structure Fields:
 * - name: City identification string
 * - xloc/yloc: Map coordinates for city location
 * - c_mtrls: Construction materials array
 * - i_mtrls: Industrial materials array  
 * - s_talons: Silver talons (currency) treasury
 * - fortress: Fortification level for defensive strength
 * - i_people: Population count
 * - weight: Total carrying capacity/encumbrance
 */

/* City identification and location access - Primary city_ptr operations */
#define CITY_NAME	city_ptr->name      /* City name string identifier */
#define CITY_XLOC	city_ptr->xloc      /* City X coordinate on world map */
#define CITY_YLOC	city_ptr->yloc      /* City Y coordinate on world map */

/* Economic resource management - Construction and industrial materials */
#define CITY_MTRLS	city_ptr->c_mtrls   /* Construction materials array */
#define CITY_IMTRLS	city_ptr->i_mtrls   /* Industrial materials array */
#define CITY_STALONS	city_ptr->s_talons  /* Silver talons treasury */

/* Urban infrastructure and population */
#define CITY_FORT	city_ptr->fortress  /* Fortification defensive level */
#define CITY_PEOPLE	city_ptr->i_people  /* Population count */
#define CITY_WEIGHT	city_ptr->weight    /* Total carrying capacity */

/* Template city access - Secondary city_tptr operations for comparisons */
#define CITYT_NAME	city_tptr->name     /* Template city name */
#define CITYT_XLOC	city_tptr->xloc     /* Template city X coordinate */
#define CITYT_YLOC	city_tptr->yloc     /* Template city Y coordinate */
#define CITYT_MTRLS	city_tptr->c_mtrls  /* Template construction materials */
#define CITYT_IMTRLS	city_tptr->i_mtrls  /* Template industrial materials */
#define CITYT_STALONS	city_tptr->s_talons /* Template silver talons */
#define CITYT_FORT	city_tptr->fortress /* Template fortification level */
#define CITYT_PEOPLE	city_tptr->i_people /* Template population count */
#define CITYT_WEIGHT	city_tptr->weight   /* Template carrying capacity */

/*
 * =============================================================================
 * OPTIONAL CODE GENERATION SYSTEM - City Operations Debugging and Replay
 * =============================================================================
 *
 * When USE_CODES is defined, this section provides structured logging
 * functionality for city operations, enabling debugging, replay analysis,
 * and game state tracking. All city modifications are logged with standardized
 * format for external analysis tools.
 *
 * Code Generation Features:
 * - City location tracking with coordinate logging
 * - Population and economic resource change monitoring
 * - Fortification and infrastructure modification logging
 * - City creation and destruction event recording
 * - Structured output format for automated analysis
 */
#ifdef USE_CODES

#include "executeX.h"

/*
 * City execution command range definitions
 * EX_CITYBEGIN: Start of city command range
 * EX_CITYEND: End of city command range
 */
#define EX_CITYBEGIN	EX_CITYLOC      /* First city operation code */
#define EX_CITYEND	EX_CITYWEIGHT   /* Last city operation code */

/*
 * City Information Adjustment Commands - Structured Logging Macros
 *
 * These macros generate formatted output for city operations when PRINT_CODES
 * is enabled. Each macro logs specific city modifications with standardized
 * tab-separated format for external processing and replay functionality.
 *
 * Output Format: COMMAND\tCODE\tPARAM1\tPARAM2\tPARAM3\tNAME\tEXTRA
 * - COMMAND: Operation type identifier
 * - CODE: Execution code number from executeX.h
 * - PARAM1-3: Operation-specific parameters
 * - NAME: City name for identification
 * - EXTRA: Additional data or "null"
 */
#ifdef PRINT_CODES

/* City location and identification logging */
#define CADJLOC	fprintf(fexe, "C_LOC\t%d\t0\t%d\t%d\t%12s\tnull\n", EX_CITYLOC, (int)CITY_XLOC, (int)CITY_YLOC, CITY_NAME)
#define CADJNAME	fprintf(fexe, "C_NAME\t%d\t0\t0\t0\t%12s\t%12s\n", EX_CITYNAME, CITY_NAME, newnamestr)

/* Population and infrastructure modification logging */
#define CADJPEOP	fprintf(fexe, "C_PEOP\t%d\t0\t%d\t0\t%12s\tnull\n", EX_CITYPEOP, (int)CITY_PEOPLE, CITY_NAME)
#define CADJWEIGHT	fprintf(fexe, "C_WEIGHT\t%d\t0\t%d\t0\t%12s\tnull\n", EX_CITYWEIGHT, (int)CITY_WEIGHT, CITY_NAME)
#define CADJFORT	fprintf(fexe, "C_FORT\t%d\t0\t%d\t0\t%12s\tnull\n", EX_CITYFORT, (int)CITY_FORT, CITY_NAME)

/* Economic resource modification logging */
#define CADJMTRLS(x)	fprintf(fexe, "D_CMTRLS\t%d\t%d\t%f\t0\t%12s\tnull\n", EX_CITYMTRLS, (x), (double)CITY_MTRLS[x], CITY_NAME)
#define CADJSTALONS	fprintf(fexe, "D_CTLNS\t%d\t0\t%f\t0\t%12s\tnull\n", EX_CITYSTALONS, (double)CITY_STALONS, CITY_NAME)
#define CADJIMTRLS(x)	fprintf(fexe, "D_CMTRLS\t%d\t%d\t%f\t0\t%12s\tnull\n", EX_CITYIMTRLS, (x), (double)CITY_IMTRLS[x], CITY_NAME)

/* City lifecycle event logging */
#define CCREATE	fprintf(fexe, "C_CREATE\t%d\t0\t0\t0\t%12s\tnull\n", EX_CITYCREATE, CITY_NAME)
#define CDESTROY	fprintf(fexe, "C_DESTROY\t%d\t0\t0\t0\t%12s\tnull\n", EX_CITYDESTROY, CITY_NAME)

#endif /* PRINT_CODES */
#endif /* USE_CODES */

/*
 * =============================================================================
 * GLOBAL VARIABLE DECLARATIONS - City System Pointer Interface
 * =============================================================================
 *
 * External declarations for the global city pointers used throughout the
 * city management system. These pointers provide the foundation for all
 * city accessor macros and operations.
 *
 * Global Pointers:
 * - city_ptr: Primary city pointer for current city operations
 * - city_tptr: Template city pointer for comparison and batch operations
 *
 * Usage Pattern:
 * 1. Set city_ptr to target city structure
 * 2. Use CITY_* macros for field access and modification
 * 3. Set city_tptr for template operations using CITYT_* macros
 * 4. Perform city comparisons, transfers, or batch updates
 *
 * Memory Management:
 * - Pointers reference existing city structures (no allocation)
 * - Caller responsible for ensuring valid pointer assignments
 * - Thread safety considerations apply for multi-threaded access
 *
 * Integration Notes:
 * - Defined in cityA.c or main initialization module
 * - Used by all city manipulation functions
 * - Required for city accessor macro functionality
 * - Connected to city structure arrays and dynamic allocation
 */
extern CITY_PTR city_ptr, city_tptr;
