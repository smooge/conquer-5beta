/*
 * navyX.h - Naval System Definitions and Fleet Management Configuration
 *
 * This header defines the complete naval system for the Conquer strategy game,
 * including ship type classifications, fleet composition, cargo management,
 * and naval operations. It provides the foundation for maritime combat,
 * transportation, trade, and amphibious operations throughout the game engine.
 *
 * Key Components:
 *   - Naval ship type classifications and specifications system
 *   - NINFO_STRUCT: Complete ship type information and capabilities
 *   - Fleet composition encoding with efficient bit-packed storage
 *   - Accessor macros for navy data structure field access
 *   - Cargo counting and capacity management systems
 *   - Optional code generation system for debugging and replay
 *
 * Integration Notes:
 *   - Used by: All naval-related modules (navyX.c, navyG.c, combatA.c, etc.)
 *   - Depends on: header.h for basic types, itemX.h for cargo definitions
 *   - Provides: Complete naval fleet system and maritime operation framework
 *
 * Modernization Considerations:
 *   - Ship type constants could benefit from enum-based type safety
 *   - Bit-packed ship storage may need 64-bit considerations for large fleets
 *   - Macro definitions could be converted to inline functions for type safety
 *   - Magic numbers in bit manipulation need clear documentation
 */

/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* ============================================================================
 * NAVAL SHIP TYPE CLASSIFICATIONS - Maritime Vessel Categories
 * ============================================================================
 * Purpose: Define ship type categories for fleet composition and naval operations
 * Usage: Ship type indices for ninfo_list[] array and fleet composition encoding
 * Notes: NSHP_NUMBER constant defined in dataX.h specifies total number of ship types
 */

/*
 * Ship Type Classification Constants - Naval Vessel Categories
 *
 * Defines the strategic categories for naval vessels in the maritime system.
 * Each ship type has distinct capabilities, costs, and tactical roles in
 * naval operations including combat, transportation, and amphibious assaults.
 *
 * Usage Patterns:
 *   - Index into ninfo_list[] array for ship specifications
 *   - Fleet composition encoding in bit-packed ship storage
 *   - Naval combat and movement calculations
 *   - Shipbuilding and cost determination systems
 *
 * Integration:
 *   - NSHP_NUMBER in dataX.h defines total ship type count
 *   - Used by bit-packing macros for fleet composition storage
 *   - Referenced throughout naval system for type-specific behavior
 */
#define NSHP_WARSHIPS	0	/* Combat vessels specialized for naval warfare and fleet protection */
#define NSHP_MERCHANTS	1	/* Trade vessels optimized for cargo capacity and economic transport */
#define NSHP_GALLEYS	2	/* Fast vessels with enhanced speed and maneuverability for scouting */
#define NSHP_BARGES	3	/* Heavy transport vessels for bulk cargo and mass troop movement */

/* ============================================================================
 * NAVAL SHIP TYPE INFORMATION STRUCTURE - Complete Ship Specifications
 * ============================================================================
 * Purpose: Store complete specifications for each naval ship type in the game
 * Usage: Indexed by ship type number to provide all ship characteristics
 * Notes: Forms the foundation of the naval system - all ship behavior derives from this
 */

/*
 * struct s_ninfo (NINFO_STRUCT) - Comprehensive naval ship type definition
 *
 * Contains complete specifications for a single naval ship type including
 * identification, performance characteristics, economic costs, and combat
 * properties. This structure defines every aspect of how a ship type
 * behaves in naval operations, construction, combat, and cargo transport.
 *
 * Usage Patterns:
 *   - Ship construction and naval recruitment system parameter source
 *   - Naval movement and speed calculation input
 *   - Economic system cost calculations for shipbuilding
 *   - Combat system damage and capture probability calculations
 *   - UI display and player information systems
 *
 * Relationships:
 *   - Array of structures (ninfo_list[]) indexed by ship type number
 *   - Ship type determines tactical role and operational capabilities
 *   - Cost fields integrate with economic and resource management systems
 *   - Speed and capacity fields drive movement and cargo mechanics
 *
 * Memory Layout:
 *   - String pointers reference static data in naval data files
 *   - Numeric fields provide direct access for calculations
 *   - itemtype fields integrate with item/resource system
 *   - Damage chance provides combat system integration
 */
typedef struct s_ninfo {
  char *name;		/* Full descriptive ship class name for display and identification */
  char *sname;		/* Abbreviated ship name for space-constrained UI elements */
  uns_char speed;	/* Base movement speed for this ship class in naval operations */
  long capacity;	/* Cargo carrying capacity per ship hold for transport operations */
  itemtype taloncost;	/* Talon currency cost required for constructing one ship of this class */
  itemtype woodcost;	/* Wood resource cost required for constructing one ship of this class */
  int dam_chance;	/* Percentage probability of taking damage rather than capture in combat */
} NINFO_STRUCT, *NINFO_PTR;

/* ============================================================================
 * FLEET COMPOSITION BIT-PACKING SYSTEM - Efficient Ship Storage
 * ============================================================================
 * Purpose: Encode fleet composition using bit-packed storage for memory efficiency
 * Usage: Store multiple ship type counts in compact integer representations
 * Notes: Uses 5-bit fields to store ship counts, enabling up to 31 ships per type
 */

/*
 * Naval Weight Classifications - Fleet Load Categories
 *
 * Defines fleet weight categories that affect movement speed and tactical
 * capabilities. These classifications determine speed bonuses and penalties
 * based on the overall composition and load of the naval fleet.
 *
 * Usage: Fleet speed calculations and movement point determination
 * Integration: Combined with ship counts to determine overall fleet performance
 */
#define N_LIGHT		0	/* Light fleet configuration with speed bonus for minimal load */
#define N_MEDIUM	1	/* Medium fleet configuration with standard movement capabilities */
#define N_HEAVY		2	/* Heavy fleet configuration with reduced speed due to cargo load */

/*
 * Bit-Packing Configuration Constants - Ship Count Storage System
 *
 * Defines the bit manipulation parameters for efficiently storing ship counts
 * of different types within integer values. This system allows compact storage
 * of fleet composition while maintaining fast access to individual ship counts.
 *
 * Bit Layout: Each ship type uses N_BITSIZE bits (5 bits = 0-31 ships max)
 * Storage: Multiple ship types packed into single integer values
 * Access: N_MASK and bit shifting operations extract individual counts
 */
#define N_BITSIZE	5	/* Number of bits allocated per ship type count (5 bits = 0-31 range) */
#define N_MASK		((unsigned short) 0x001f)	/* Bit mask for extracting single ship type count */

/*
 * Fleet Speed Configuration Constants - Movement Calculations
 *
 * Defines speed-related constants for naval movement calculations including
 * base speeds and bonuses based on fleet composition and load factors.
 *
 * Speed System: Lighter fleets receive speed bonuses, heavier fleets penalties
 * Integration: Used with ship type speeds and fleet weight classifications
 */
#define N_NOSPD		0	/* Base speed when no ships present in fleet */
#define N_SIZESPD	3	/* Speed bonus points awarded to lighter fleet configurations */

/* ============================================================================
 * SHIP COUNT EXTRACTION MACROS - Bit-Packed Fleet Composition Access
 * ============================================================================
 * Purpose: Extract individual ship type counts from bit-packed fleet storage
 * Usage: Decode ship counts for fleet analysis and naval operations
 * Notes: Uses bit shifting and masking to retrieve counts from packed integers
 */

/*
 * Ship Count Extraction Macros - Fleet Composition Decoding
 *
 * These macros extract individual ship type counts from the bit-packed
 * fleet composition storage system. They enable efficient access to
 * specific ship counts while maintaining compact storage representation.
 *
 * Bit Extraction Algorithm:
 *   1. Calculate bit position: (ship_type * N_BITSIZE) 
 *   2. Create shifted mask: (N_MASK << bit_position)
 *   3. Apply mask and shift back: ((value & mask) >> bit_position)
 *
 * Usage Patterns:
 *   - Fleet composition analysis and display
 *   - Naval movement speed calculations
 *   - Combat strength determination
 *   - Cargo capacity calculations
 */
#define	N_CNTSHIPS(x,y)	(short)( ((x)&(N_MASK<<((y)*N_BITSIZE))) >> ((y)*N_BITSIZE) )
	/* Extract ship count for type (y) from packed value (x) */

#define NAVY_CNTSHIPS(x,y)	N_CNTSHIPS(NAVY_SHIPS[x], y)
	/* Extract ship count for type (y) from navy fleet ship array at index (x) */

/* ============================================================================
 * NAVY DATA STRUCTURE ACCESSOR MACROS - Field Access Convenience
 * ============================================================================
 * Purpose: Provide convenient access to navy structure fields through global pointers
 * Usage: Simplified field access throughout naval manipulation code
 * Notes: Assumes navy_ptr and navy_tptr global variables point to valid NAVY_STRUCT
 */

/*
 * NAVY_* Macros - Primary navy pointer field access
 *
 * Provides convenient access to fields of the navy structure pointed to by
 * the global navy_ptr variable. These macros simplify code readability and
 * provide a consistent interface for naval data manipulation throughout
 * the maritime system modules.
 *
 * Usage Context:
 *   - Naval fleet manipulation functions throughout the codebase
 *   - Movement and position tracking operations
 *   - Cargo and crew management systems
 *   - Naval combat and efficiency calculations
 *
 * Pointer Dependency:
 *   - Requires navy_ptr global variable to point to valid NAVY_STRUCT
 *   - No bounds checking or null pointer validation
 *   - Assumes proper initialization before macro usage
 */
#define NAVY_ID		navy_ptr->navyid	/* Unique naval fleet identifier */
#define NAVY_SHIPS	navy_ptr->ships		/* Bit-packed ship composition array */
#define NAVY_XLOC	navy_ptr->xloc		/* Current X coordinate position */
#define NAVY_YLOC	navy_ptr->yloc		/* Current Y coordinate position */
#define NAVY_LASTX	navy_ptr->lastx		/* Previous X coordinate position */
#define NAVY_LASTY	navy_ptr->lasty		/* Previous Y coordinate position */
#define NAVY_MOVE	navy_ptr->umove		/* Remaining movement points this turn */
#define NAVY_CREW	navy_ptr->crew		/* Naval crew strength and manning level */
#define NAVY_PEOP	navy_ptr->people	/* Civilian passengers and personnel count */
#define NAVY_MTRLS	navy_ptr->mtrls		/* Material cargo and resource storage array */
#define NAVY_ARMY	navy_ptr->armynum	/* Embarked army unit identifier */
#define NAVY_CVN	navy_ptr->cvnnum	/* Embarked caravan identifier */
#define NAVY_STAT	navy_ptr->status	/* Fleet status flags and operational state */
#define NAVY_SPLY	navy_ptr->supply	/* Supply level for fleet maintenance */
#define NAVY_EFF	navy_ptr->efficiency	/* Operational efficiency rating array */

/*
 * NAVYT_* Macros - Temporary navy pointer field access
 *
 * Provides convenient access to fields of the navy structure pointed to by
 * the global navy_tptr (temporary) variable. Used in situations where two
 * navy structures need to be accessed simultaneously, such as fleet transfers,
 * naval combat, or comparison operations.
 *
 * Usage Context:
 *   - Naval combat systems requiring attacker and defender access
 *   - Fleet transfer and merge operations
 *   - Comparison and analysis functions
 *   - Temporary storage during complex naval operations
 *
 * Parallel Structure:
 *   - Mirrors NAVY_* macros but uses navy_tptr instead of navy_ptr
 *   - Enables dual fleet access patterns throughout the system
 *   - Supports complex naval operations requiring multiple fleet references
 */
#define NAVYT_ID	navy_tptr->navyid	/* Temporary navy unique identifier */
#define NAVYT_SHIPS	navy_tptr->ships	/* Temporary navy ship composition array */
#define NAVYT_XLOC	navy_tptr->xloc		/* Temporary navy X coordinate position */
#define NAVYT_YLOC	navy_tptr->yloc		/* Temporary navy Y coordinate position */
#define NAVYT_LASTX	navy_tptr->lastx	/* Temporary navy previous X position */
#define NAVYT_LASTY	navy_tptr->lasty	/* Temporary navy previous Y position */
#define NAVYT_MOVE	navy_tptr->umove	/* Temporary navy movement points */
#define NAVYT_CREW	navy_tptr->crew		/* Temporary navy crew strength */
#define NAVYT_PEOP	navy_tptr->people	/* Temporary navy passenger count */
#define NAVYT_MTRLS	navy_tptr->mtrls	/* Temporary navy material storage */
#define NAVYT_ARMY	navy_tptr->armynum	/* Temporary navy embarked army */
#define NAVYT_CVN	navy_tptr->cvnnum	/* Temporary navy embarked caravan */
#define NAVYT_STAT	navy_tptr->status	/* Temporary navy status flags */
#define NAVYT_SPLY	navy_tptr->supply	/* Temporary navy supply level */
#define NAVYT_EFF	navy_tptr->efficiency	/* Temporary navy efficiency rating */

/* ============================================================================
 * OPTIONAL CODE GENERATION SYSTEM - Naval Operations Debugging Support
 * ============================================================================
 * Purpose: Generate structured output codes for naval operations debugging
 * Usage: Compile with USE_CODES to enable code generation macros
 * Notes: Creates parseable output for naval replay and debugging analysis
 */

#ifdef USE_CODES

#include "executeX.h"

/*
 * Naval Operation Code Boundaries - Command Range Definition
 *
 * Defines the range of command codes used for naval fleet operations
 * in the code generation system. These boundaries enable systematic
 * processing of navy-related commands during replay and debugging.
 *
 * Naval Commands: EX_NAVYLOC through EX_NAVYCVN
 *
 * Usage: Command range validation and systematic code processing
 * Integration: Links to executeX.h command enumeration system
 */
#define EX_NAVYBEGIN	EX_NAVYLOC	/* First naval operation command code */
#define EX_NAVYEND	EX_NAVYCVN	/* Last naval operation command code */

/*
 * Code Generation Macros - Naval Operation Logging
 *
 * When both USE_CODES and PRINT_CODES are defined, these macros generate
 * structured log output for all naval operations. Each macro outputs a
 * tab-delimited line containing operation type, command code, navy ID,
 * and relevant parameters for debugging and replay functionality.
 *
 * Output Format: OPERATION\tCODE\tNAVY_ID\tPARAM1\tPARAM2\tnull\tnull
 * Usage: Automatic logging during naval operations for debugging
 * File Output: Written to global fexe file handle
 */

#ifdef PRINT_CODES
#define NADJLOC	fprintf(fexe, "N_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_NAVYLOC, (int)NAVY_ID, (int)NAVY_XLOC, (int)NAVY_YLOC)
#define NADJOLOC	fprintf(fexe, "N_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_NAVYOLOC, (int)NAVY_ID, (int)NAVY_LASTX, (int)NAVY_LASTY)
#define NADJSHIP	fprintf(fexe, "N_SHIP\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_NAVYSHIP, (int)NAVY_ID, global_int, (int)(NAVY_SHIPS[global_int]))
#define NADJSTAT	fprintf(fexe, "L_NSTAT\t%d\t%d\t%ld\t0\tnull\tnull\n", EX_NAVYSTAT, (int)NAVY_ID, NAVY_STAT)
#define NADJMOVE	fprintf(fexe, "N_MOVE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYMOVE, (int)NAVY_ID, (int)NAVY_MOVE)
#define NADJID	fprintf(fexe, "N_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYID, global_int, (int)NAVY_ID)
#define NADJCREW	fprintf(fexe, "N_CREW\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYCREW, (int)NAVY_ID, (int)NAVY_CREW)
#define NADJPEOP	fprintf(fexe, "N_PEOP\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYPEOP, (int)NAVY_ID, (int)NAVY_PEOP)
#define NMTRLS(x)	fprintf(fexe, "D_NMTRLS\t%d\t%d\t%d\t%f\tnull\tnull\n", EX_NAVYMTRLS, (int)NAVY_ID, (x), (double)NAVY_MTRLS[x])
#define NADJSPLY	fprintf(fexe, "N_SPLY\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYSPLY, (int)NAVY_ID, (int)NAVY_SPLY)
#define NADJARMY	fprintf(fexe, "N_ARMY\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYARMY, (int)NAVY_ID, (int)NAVY_ARMY)
#define NADJCVN	fprintf(fexe, "N_CVN\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYCVN, (int)NAVY_ID, (int)NAVY_CVN)
#define NADJEFF(x)	fprintf(fexe, "N_EFF\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_NAVYEFF, (int)NAVY_ID, (int)NAVY_EFF[x], (x))
#define NCREATE	fprintf(fexe, "N_CREATE\t%d\t%d\t0\t0\tnull\tnull\n", EX_NAVYCREATE, (int)NAVY_ID)
#define NDESTROY	fprintf(fexe, "N_DESTROY\t%d\t%d\t0\t0\tnull\tnull\n", EX_NAVYDESTROY, (int)NAVY_ID)
#endif /* PRINT_CODES */

#endif /* USE_CODES */

/* ============================================================================
 * EXTERNAL DECLARATIONS - Global Naval System Variables
 * ============================================================================
 * Purpose: Declare global variables and data structures used throughout the naval system
 * Usage: Provides access to naval data and configuration across all maritime modules
 * Notes: Actual definitions provided in naval data files and implementation modules
 */

/*
 * Global Naval Pointer Variables - Active Fleet References
 *
 * These global pointers provide system-wide access to navy structures
 * currently being processed. They enable the NAVY_* and NAVYT_* macros
 * and support operations requiring dual fleet access patterns.
 */
extern NAVY_PTR navy_ptr, navy_tptr;	/* Primary and temporary naval fleet structure pointers */

/*
 * Naval Ship Type Information System - Ship Definition Table
 *
 * The ninfo_list[] array contains complete specifications for all naval ship
 * types in the game. This is the master table that defines ship capabilities,
 * performance characteristics, construction costs, and combat properties
 * for every available ship class.
 *
 * Data Source: Definitions provided in naval data files
 * Array Size: NSHP_NUMBER elements (defined in dataX.h)
 * Index Range: 0 to NSHP_NUMBER-1 (corresponds to NSHP_* constants)
 * Usage: Accessed by ship type index throughout naval systems
 */
extern NINFO_STRUCT ninfo_list[NSHP_NUMBER];	/* Master array of ship type definitions */
