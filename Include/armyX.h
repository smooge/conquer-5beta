/*
 * armyX.h - Army Unit Type Definitions and Military System Configuration
 *
 * This header defines the complete military unit system for the Conquer strategy game,
 * including unit type classifications, properties, statistics, and behavioral macros.
 * It provides the foundation for army management, combat calculations, unit recruitment,
 * and military operations throughout the game engine.
 *
 * Key Components:
 *   - Army unit type classifications and enumeration system
 *   - AINFO_STRUCT: Complete unit type information and statistics
 *   - Unit property flags defining special abilities and behaviors
 *   - Accessor macros for army data structure field access
 *   - Unit classification and property testing macros
 *   - Optional code generation system for debugging and replay
 *
 * Integration Notes:
 *   - Used by: All military-related modules (armyX.c, armyG.c, combatA.c, etc.)
 *   - Depends on: header.h for basic types, datamilX.c for unit data tables
 *   - Provides: Complete military unit type system and property framework
 *
 * Modernization Considerations:
 *   - Enum types could benefit from explicit sizing and namespace protection
 *   - Property flags use long int - consider standardized bit field types
 *   - Macro definitions could be converted to inline functions for type safety
 *   - Magic numbers embedded in constants need documentation and validation
 */

/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* ============================================================================
 * DEFAULT UNIT TYPE CONSTANTS - Critical System Configuration
 * ============================================================================
 * Purpose: Define default unit types for automatic army creation and fallbacks
 * Usage: Used when no specific unit type is requested or for system defaults
 * Notes: These values are indexes into the ainfo_list[] array and must match
 *        the actual unit definitions in datamilX.c
 */

/*
 * DEFAULT_ARMYTYPE - Default infantry unit type for standard army creation
 *
 * Specifies the unit type index used when creating armies without explicit
 * type specification. This must correspond to a basic infantry unit that
 * serves as the standard military unit for most operations.
 *
 * Value: 38 (infantry unit index in ainfo_list[])
 * Usage: Default army creation, fallback unit selection
 * Notes: CRITICAL - Must match infantry definition in datamilX.c unit tables
 *        Changing this value affects game balance and default army behavior
 */
#define DEFAULT_ARMYTYPE	38

/*
 * DEFAULT_RULERTYPE - Default ruler unit type for nation leadership
 *
 * Specifies the unit type index for the primary ruler/king unit type.
 * This unit type represents the nation's primary leader with full
 * administrative and command capabilities.
 *
 * Value: 0 (ruler unit index in ainfo_list[])
 * Usage: Nation creation, ruler spawn, leadership determination
 * Notes: Must have UP_RULER property flag set in unit definition
 */
#define DEFAULT_RULERTYPE	0

/*
 * DEFAULT_LEADERTYPE - Default leader unit type for military command
 *
 * Specifies the unit type index for standard military leaders below
 * the ruler level. These units provide command bonuses and tactical
 * leadership for army groups.
 *
 * Value: 1 (leader unit index in ainfo_list[])
 * Usage: Leader creation, military command structure
 * Notes: Must have AC_LEADER classification in unit definition
 */
#define DEFAULT_LEADERTYPE	1

/* ============================================================================
 * ARMY UNIT CLASSIFICATION SYSTEM - Strategic Unit Categories
 * ============================================================================
 * Purpose: Categorize military units by role, capability, and tactical function
 * Usage: Unit classification affects AI behavior, combat mechanics, and recruitment
 * Notes: Classifications form hierarchy - special units (leaders/magic) vs normal units
 */

/*
 * Aclasstype - Army unit classification enumeration
 *
 * Defines strategic categories for all military unit types in the game.
 * Classifications determine unit capabilities, AI behavior patterns,
 * combat mechanics, and recruitment restrictions. The enumeration is
 * organized hierarchically with special units first, then normal units.
 *
 * Classification Hierarchy:
 *   Special Units (0-3): Leaders, spellcasters, monsters - unique capabilities
 *   Normal Units (4+): Standard military forces with specialized roles
 *
 * Usage Patterns:
 *   - Unit AI behavior and tactical decisions
 *   - Combat bonus/penalty calculations
 *   - Recruitment system restrictions and costs
 *   - Leadership and command structure determination
 *
 * Integration:
 *   - String names stored in datamilX.c ainfo_clist[] array
 *   - Used by classification macros (a_isleader, a_isnormal, etc.)
 *   - Affects unit property interpretation and game mechanics
 */
typedef enum aclasstype {
  AC_LEADER,		/* Command unit with leadership capabilities and tactical bonuses */
  AC_SPELLCASTER,	/* Magical unit capable of casting spells and magical combat */
  AC_MONSTER,		/* Special creature unit with unique properties and abilities */
  AC_NORMAL,		/* Threshold marker: all classifications >= this are standard units */
  AC_SCOUT,		/* Reconnaissance unit with enhanced movement and vision capabilities */
  AC_AGENT,		/* Infiltration unit recruited from enemy forces with special access */
  AC_MERCS,		/* Mercenary unit with enhanced combat ability but higher costs */
  AC_CAVALRY,		/* Mounted unit with superior speed and charge attack capabilities */
  AC_SAILORS,		/* Naval-capable unit able to operate on ships and coastal areas */
  AC_ORCISH,		/* Orcish racial unit with species-specific properties and bonuses */
  AC_ARCHERS,		/* Ranged combat unit specializing in projectile weapons */
  AC_UNIQUE		/* Special one-of-a-kind unit with completely unique characteristics */
} Aclasstype;

/* ============================================================================
 * ARMY UNIT TYPE INFORMATION STRUCTURE - Complete Unit Specification
 * ============================================================================
 * Purpose: Store complete specifications for each army unit type in the game
 * Usage: Indexed by unit type number to provide all unit characteristics
 * Notes: Forms the foundation of the unit system - all unit behavior derives from this
 */

/*
 * struct s_ainfo (AINFO_STRUCT) - Comprehensive army unit type definition
 *
 * Contains complete specifications for a single army unit type including
 * identification, combat statistics, economic costs, special properties,
 * and magical requirements. This structure defines every aspect of how
 * a unit type behaves in combat, recruitment, maintenance, and special operations.
 *
 * Usage Patterns:
 *   - Unit creation and recruitment system parameter source
 *   - Combat calculation input (attack/defense bonuses, properties)
 *   - Economic system cost and maintenance calculations
 *   - Magic system integration (spell requirements and casting abilities)
 *   - UI display and player information systems
 *
 * Relationships:
 *   - Array of structures (ainfo_list[]) indexed by unit type number
 *   - Classification determines AI behavior and game rule application
 *   - Properties field controls special abilities and restrictions
 *   - Magical power requirements link to spell system
 *
 * Memory Layout:
 *   - String pointers reference static data in datamilX.c
 *   - Integer fields provide direct access for calculations
 *   - Properties use bit flags for efficient capability testing
 *   - Power array provides magic system integration
 */
typedef struct s_ainfo {
  char *name;		/* Full descriptive unit type name for display and identification */
  char *sname;		/* Abbreviated unit name for space-constrained UI elements */
  char *select;		/* Selection key string used in recruitment interface commands */
  char *descript;	/* Detailed unit description for player information and help */
  long properties;	/* Bit field of unit properties (UP_* flags) defining special abilities */
  Aclasstype class;	/* Unit classification determining tactical role and AI behavior */
  int minsth;		/* Minimum unit strength threshold for effectiveness and bonuses */
  int att_bonus;	/* Offensive combat modifier added to attack calculations */
  int def_bonus;	/* Defensive combat modifier added to defense calculations */
  uns_char speed;	/* Relative movement speed multiplied by 10 (allows decimal precision) */
  long ore_enlist;	/* Metal and jewel resource cost required for unit recruitment */
  long tal_enlist;	/* Talon currency or spell points required for unit recruitment */
  long maint;		/* Economic maintenance cost per turn; leader birth rate for special units */
  int capt_val;		/* Land capture effectiveness value for territorial control operations */
  int work_val;		/* Construction and labor effectiveness value for building projects */
  long pow_need[MAG_NUMBER];	/* Array of magical power requirements indexed by magic school */
} AINFO_STRUCT, *AINFO_PTR;

/* ============================================================================
 * ARMY DATA STRUCTURE ACCESSOR MACROS - Field Access Convenience
 * ============================================================================
 * Purpose: Provide convenient access to army structure fields through global pointers
 * Usage: Simplified field access throughout army manipulation code
 * Notes: Assumes army_ptr and army_tptr global variables point to valid ARMY_STRUCT
 */

/*
 * ARMY_* Macros - Primary army pointer field access
 *
 * Provides convenient access to fields of the army structure pointed to by
 * the global army_ptr variable. These macros simplify code readability and
 * provide a consistent interface for army data manipulation throughout
 * the military system modules.
 *
 * Usage Context:
 *   - Army manipulation functions throughout the codebase
 *   - Combat calculation and unit state updates
 *   - Movement and position tracking operations
 *   - Status and efficiency management systems
 *
 * Pointer Dependency:
 *   - Requires army_ptr global variable to point to valid ARMY_STRUCT
 *   - No bounds checking or null pointer validation
 *   - Assumes proper initialization before macro usage
 */
#define ARMY_ID		army_ptr->armyid	/* Unique army unit identifier */
#define ARMY_TYPE	army_ptr->unittype	/* Unit type index into ainfo_list[] */
#define ARMY_XLOC	army_ptr->xloc		/* Current X coordinate position */
#define ARMY_YLOC	army_ptr->yloc		/* Current Y coordinate position */
#define ARMY_LASTX	army_ptr->lastx		/* Previous X coordinate position */
#define ARMY_LASTY	army_ptr->lasty		/* Previous Y coordinate position */
#define ARMY_SIZE	army_ptr->strength	/* Current unit strength/population */
#define ARMY_MOVE	army_ptr->umove		/* Remaining movement points this turn */
#define ARMY_STAT	army_ptr->status	/* Unit status flags and state information */
#define ARMY_LEAD	army_ptr->leader	/* Leadership value and command effectiveness */
#define ARMY_SPLY	army_ptr->supply	/* Supply level for unit maintenance */
#define ARMY_SPTS	army_ptr->spellpts	/* Available spell points for magic units */
#define ARMY_EFF	army_ptr->efficiency	/* Current operational efficiency percentage */
#define ARMY_MAXEFF	army_ptr->max_eff	/* Maximum achievable efficiency for this unit */

/*
 * ARMYT_* Macros - Temporary army pointer field access
 *
 * Provides convenient access to fields of the army structure pointed to by
 * the global army_tptr (temporary) variable. Used in situations where two
 * army structures need to be accessed simultaneously, such as combat
 * calculations, unit transfers, or comparison operations.
 *
 * Usage Context:
 *   - Combat systems requiring attacker and defender access
 *   - Unit transfer and merge operations
 *   - Comparison and analysis functions
 *   - Temporary storage during complex operations
 *
 * Parallel Structure:
 *   - Mirrors ARMY_* macros but uses army_tptr instead of army_ptr
 *   - Enables dual army access patterns throughout the system
 *   - Supports complex military operations requiring multiple unit references
 */
#define ARMYT_ID	army_tptr->armyid	/* Temporary army unique identifier */
#define ARMYT_TYPE	army_tptr->unittype	/* Temporary army unit type index */
#define ARMYT_XLOC	army_tptr->xloc		/* Temporary army X coordinate position */
#define ARMYT_YLOC	army_tptr->yloc		/* Temporary army Y coordinate position */
#define ARMYT_LASTX	army_tptr->lastx	/* Temporary army previous X position */
#define ARMYT_LASTY	army_tptr->lasty	/* Temporary army previous Y position */
#define ARMYT_SIZE	army_tptr->strength	/* Temporary army unit strength */
#define ARMYT_MOVE	army_tptr->umove	/* Temporary army movement points */
#define ARMYT_STAT	army_tptr->status	/* Temporary army status flags */
#define ARMYT_LEAD	army_tptr->leader	/* Temporary army leadership value */
#define ARMYT_SPLY	army_tptr->supply	/* Temporary army supply level */
#define ARMYT_SPTS	army_tptr->spellpts	/* Temporary army spell points */
#define ARMYT_EFF	army_tptr->efficiency	/* Temporary army efficiency */
#define ARMYT_MAXEFF	army_tptr->max_eff	/* Temporary army maximum efficiency */

/* ============================================================================
 * UNIT PROPERTY FLAGS - Special Abilities and Behavioral Characteristics
 * ============================================================================
 * Purpose: Define special unit capabilities and behavioral modifiers
 * Usage: Bit flags stored in AINFO_STRUCT.properties field for capability testing
 * Notes: String descriptions stored in datamilX.c traits_list[] array
 */

/* Leadership and Command Properties */
#define UP_RULER	0x00000001L	/* Nation ruler with administrative and military command authority */

/* Movement and Tactical Properties */
#define UP_SLIPPERY	0x00000002L	/* Can move through enemy units without engaging in combat */
#define UP_FLIGHT	0x00000004L	/* Innate flight ability - can move over water and obstacles */
#define UP_ANTIAIR	0x00000010L	/* Can force flying units to land and engage in ground combat */
#define UP_UNLOAD	0x00000040L	/* May disembark from ships in unowned neutral territory */
#define UP_NAVALTAKE	0x00000080L	/* May conduct amphibious assaults in enemy territory */

/* Combat and Weapon Properties */
#define UP_BALLISTICS	0x00000020L	/* Equipped with projectile weapons for ranged combat */
#define UP_FORTDAMAGE	0x00000400L	/* Can cause structural damage to fortifications and defenses */
#define UP_ARROWWEAK	0x00000800L	/* Vulnerable to projectile weapons - reduced defense against archers */
#define UP_COVERBONUS	0x00002000L	/* Provides defensive bonus to all friendly units in same location */
#define UP_SAPPERUNIT	0x02000000L	/* Engineering unit capable of siege work and fortification attacks */

/* Special Unit Types and Origins */
#define UP_UNDEAD	0x00000008L	/* Undead creature - absorbs fallen enemies and immune to morale */
#define UP_FIRETYPE	0x00020000L	/* Elemental fire origin - special resistances and vulnerabilities */
#define UP_WATERTYPE	0x00040000L	/* Elemental water origin - aquatic capabilities and properties */
#define UP_EARTHTYPE	0x00080000L	/* Elemental earth origin - enhanced fortification and terrain bonuses */

/* Recruitment and Training Properties */
#define UP_HALFRECRUIT	0x00008000L	/* Uses only half normal recruitment population from sectors */
#define UP_SPCLTRAIN	0x00010000L	/* Requires specialized training facilities and enhanced recruitment */
#define UP_ENLISTAWAY	0x01000000L	/* Can be recruited from enemy ranks through diplomatic or covert means */
#define UP_NODRAFT	0x08000000L	/* Cannot be recruited directly - special acquisition methods required */

/* Operational and Maintenance Properties */
#define UP_SIGHT	0x00000100L	/* Enhanced reconnaissance ability - extended vision range */
#define UP_NEEDMIN	0x00000200L	/* Must maintain minimum strength threshold for effectiveness bonuses */
#define UP_PAYOFF	0x00001000L	/* Requires severance payment when disbanded - mercenary-type units */
#define UP_ANYDISB	0x00004000L	/* May disband in any sector regardless of ownership or control */
#define UP_FREESUPPLY	0x00100000L	/* Does not require supply lines - self-sufficient unit */
#define UP_DECAY	0x00200000L	/* Gradually loses strength over time - requires constant reinforcement */
#define UP_MAPPING	0x04000000L	/* Can survey and claim uncharted territory for territorial expansion */

/* Magical and Spellcasting Properties */
#define UP_SPELLCAST	0x00400000L	/* Capable of casting spells and using magical abilities in combat */
#define UP_FULLCASTER	0x00800000L	/* Full spellcaster with access to complete magical spell repertoire */

/* ============================================================================
 * UNIT CLASSIFICATION AND PROPERTY TESTING MACROS - Capability Queries
 * ============================================================================
 * Purpose: Provide convenient testing of unit classifications and special properties
 * Usage: Quick boolean tests for unit capabilities throughout the codebase
 * Notes: All macros take unit type index (x) and return boolean test results
 */

/* Unit Classification Testing Macros */
#define a_isnormal(x)	(ainfo_list[x].class >= AC_NORMAL)	/* Test if unit is standard military type */
#define a_isleader(x)	(ainfo_list[x].class <= AC_SPELLCASTER)	/* Test if unit has leadership capabilities */
#define a_ismagician(x)	(ainfo_list[x].class == AC_SPELLCASTER)	/* Test if unit is dedicated spellcaster */
#define a_ismonster(x)	(ainfo_list[x].class == AC_MONSTER)	/* Test if unit is monster/creature type */
#define a_isscout(x)	((ainfo_list[x].class == AC_SCOUT)||a_isagent(x))	/* Test if unit has scouting capabilities */
#define a_isagent(x)	(ainfo_list[x].class == AC_AGENT)	/* Test if unit is infiltrator/spy type */
#define a_ismerc(x)	(ainfo_list[x].class == AC_MERCS)	/* Test if unit is mercenary type */
#define a_isarcher(x)	(ainfo_list[x].class == AC_ARCHERS)	/* Test if unit specializes in ranged combat */

/* Magical Capability Testing Macros */
#define a_castspells(x)	(ainfo_list[x].properties & UP_SPELLCAST)	/* Test if unit can cast spells */
#define a_fullcaster(x)	(ainfo_list[x].properties & UP_FULLCASTER)	/* Test if unit is full magical spellcaster */

/* Leadership and Authority Testing Macros */
#define a_isruler(x)	(ainfo_list[x].properties & UP_RULER)	/* Test if unit is nation ruler */

/* Movement and Tactical Testing Macros */
#define a_isavian(x)	(ainfo_list[x].properties & UP_FLIGHT)	/* Test if unit has flight capability */
#define a_slippery(x)	(ainfo_list[x].properties & UP_SLIPPERY)	/* Test if unit can slip through enemy lines */
#define a_mayunload(x)	(ainfo_list[x].properties & UP_UNLOAD)	/* Test if unit can disembark in neutral territory */
#define a_assault(x)	(ainfo_list[x].properties & UP_NAVALTAKE)	/* Test if unit can conduct amphibious assaults */

/* Combat Capability Testing Macros */
#define a_arrowmen(x)	(ainfo_list[x].properties & UP_BALLISTICS)	/* Test if unit has projectile weapons */
#define a_arrowweak(x)	(ainfo_list[x].properties & UP_ARROWWEAK)	/* Test if unit is vulnerable to projectiles */
#define a_antiair(x)	(ainfo_list[x].properties & UP_ANTIAIR)	/* Test if unit can force aircraft to land */
#define a_damfort(x)	(ainfo_list[x].properties & UP_FORTDAMAGE)	/* Test if unit can damage fortifications */
#define a_coverbonus(x)	(ainfo_list[x].properties & UP_COVERBONUS)	/* Test if unit provides defensive bonus */
#define a_sapper(x)	(ainfo_list[x].properties & UP_SAPPERUNIT)	/* Test if unit has engineering capabilities */

/* Special Unit Type Testing Macros */
#define a_isundead(x)	(ainfo_list[x].properties & UP_UNDEAD)	/* Test if unit is undead creature */

/* Operational and Maintenance Testing Macros */
#define a_cansee(x)	(ainfo_list[x].properties & UP_SIGHT)	/* Test if unit has enhanced vision */
#define a_needmin(x)	(ainfo_list[x].properties & UP_NEEDMIN)	/* Test if unit requires minimum strength */
#define a_needpay(x)	(ainfo_list[x].properties & UP_PAYOFF)	/* Test if unit requires severance payment */
#define a_needtrain(x)	(ainfo_list[x].properties & UP_SPCLTRAIN)	/* Test if unit requires special training */
#define a_anydisband(x)	(ainfo_list[x].properties & UP_ANYDISB)	/* Test if unit can disband anywhere */
#define a_freesupply(x)	(ainfo_list[x].properties & UP_FREESUPPLY)	/* Test if unit is supply-independent */
#define a_decay(x)	(ainfo_list[x].properties & UP_DECAY)	/* Test if unit gradually loses strength */
#define a_mapper(x)	(ainfo_list[x].properties & UP_MAPPING)	/* Test if unit can survey territory */

/* Recruitment Testing Macros */
#define a_halfmen(x)	(ainfo_list[x].properties & UP_HALFRECRUIT)	/* Test if unit uses half recruitment */
#define a_enlistaway(x)	(ainfo_list[x].properties & UP_ENLISTAWAY)	/* Test if unit can be recruited from enemies */
#define a_nodraft(x)	(ainfo_list[x].properties & UP_NODRAFT)	/* Test if unit cannot be directly recruited */

/* ============================================================================
 * OPTIONAL CODE GENERATION SYSTEM - Debugging and Replay Support
 * ============================================================================
 * Purpose: Generate structured output codes for army operations debugging
 * Usage: Compile with USE_CODES to enable code generation macros
 * Notes: Creates parseable output for game replay and debugging analysis
 */

#ifdef USE_CODES

#include "executeX.h"

/*
 * Army Operation Code Boundaries - Command Range Definition
 *
 * Defines the range of command codes used for individual army operations
 * and army group operations in the code generation system. These boundaries
 * enable systematic processing of army-related commands during replay
 * and debugging operations.
 *
 * Individual Army Commands: EX_ARMYLOC through EX_ARMYMERCS
 * Army Group Commands: EX_GRPLOC through EX_GRPMOVE
 *
 * Usage: Command range validation and systematic code processing
 * Integration: Links to executeX.h command enumeration system
 */
#define EX_ARMYBEGIN	EX_ARMYLOC	/* First individual army command code */
#define EX_ARMYEND	EX_ARMYMERCS	/* Last individual army command code */

#define EX_GRPBEGIN	EX_GRPLOC	/* First army group command code */
#define EX_GRPEND	EX_GRPMOVE	/* Last army group command code */

/*
 * Code Generation Macros - Army Operation Logging
 *
 * When both USE_CODES and PRINT_CODES are defined, these macros generate
 * structured log output for all army operations. Each macro outputs a
 * tab-delimited line containing operation type, command code, army ID,
 * and relevant parameters for debugging and replay functionality.
 *
 * Output Format: OPERATION\tCODE\tARMY_ID\tPARAM1\tPARAM2\tnull\tnull
 * Usage: Automatic logging during army operations for debugging
 * File Output: Written to global fexe file handle
 */

#ifdef PRINT_CODES
#define AADJLOC	fprintf(fexe, "A_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_ARMYLOC, (int)ARMY_ID, (int)ARMY_XLOC, (int)ARMY_YLOC)
#define AADJOLOC	fprintf(fexe, "A_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_ARMYOLOC, (int)ARMY_ID, (int)ARMY_LASTX, (int)ARMY_LASTY)
#define AADJTYPE	fprintf(fexe, "A_TYPE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYTYPE, (int)ARMY_ID, (int)ARMY_TYPE)
#define AADJSTAT	fprintf(fexe, "L_ASTAT\t%d\t%d\t%ld\t0\tnull\tnull\n", EX_ARMYSTAT, (int)ARMY_ID, ARMY_STAT)
#define AADJMOVE	fprintf(fexe, "A_MOVE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYMOVE, (int)ARMY_ID, (int)ARMY_MOVE)
#define AADJLEAD	fprintf(fexe, "A_LEAD\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYLEAD, (int)ARMY_ID, (int)ARMY_LEAD)
#define AADJSIZE	fprintf(fexe, "L_ASIZE\t%d\t%d\t%ld\t0\tnull\tnull\n", EX_ARMYSIZE, (int)ARMY_ID, ARMY_SIZE)
#define AADJID	fprintf(fexe, "A_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYID, global_int, (int)ARMY_ID)
#define AADJSPLY	fprintf(fexe, "A_SPLY\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYSPLY, (int)ARMY_ID, (int)ARMY_SPLY)
#define AADJSPTS	fprintf(fexe, "A_SPTS\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYSPTS, (int)ARMY_ID, (int)ARMY_SPTS)
#define AADJEFF	fprintf(fexe, "A_EFF\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYEFF, (int)ARMY_ID, (int)ARMY_EFF)
#define AADJMAXEFF	fprintf(fexe, "A_MAXEFF\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYMAXEFF, (int)ARMY_ID, (int)ARMY_MAXEFF)
#define ACREATE	fprintf(fexe, "A_CREATE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYCREATE, (int)ARMY_ID, (int)ARMY_TYPE)
#define ADESTROY	fprintf(fexe, "A_DESTROY\t%d\t%d\t0\t0\tnull\tnull\n", EX_ARMYDESTROY, (int)ARMY_ID)
#define AADJMERC	fprintf(fexe, "A_MERCS\t%d\t0\t%d\t0\tnull\tnull\n", EX_ARMYMERCS, global_int)

/* now, those for army groups */
#define GADJLOC	fprintf(fexe, "G_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_GRPLOC, (int)ARMY_ID, (int)ARMY_XLOC, (int)ARMY_YLOC)
#define GADJMOVE	fprintf(fexe, "G_MOVE\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_GRPMOVE, (int)ARMY_ID, (int)unit_speed(ARMY_STAT), (int)ARMY_MOVE)
#endif /* PRINT_CODES */

#endif /* USE_CODES */

/* ============================================================================
 * EXTERNAL DECLARATIONS - Global Army System Variables
 * ============================================================================
 * Purpose: Declare global variables and data structures used throughout the army system
 * Usage: Provides access to army data and configuration across all military modules
 * Notes: Actual definitions provided in datamilX.c and other implementation files
 */

/*
 * Global Army Pointer Variables - Active Army References
 *
 * These global pointers provide system-wide access to army structures
 * currently being processed. They enable the ARMY_* and ARMYT_* macros
 * and support operations requiring dual army access patterns.
 */
extern ARMY_PTR army_ptr, army_tptr;	/* Primary and temporary army structure pointers */

/*
 * Army Type Information System - Unit Definition Tables
 *
 * The ainfo_list[] array contains complete specifications for all army unit
 * types in the game. This is the master table that defines unit capabilities,
 * statistics, costs, and special properties for every available unit type.
 *
 * Data Source: Definitions provided in datamilX.c
 * Index Range: 0 to num_armytypes-1
 * Usage: Accessed by unit type index throughout military systems
 */
extern AINFO_STRUCT ainfo_list[];	/* Master array of unit type definitions */

/*
 * Army System String Tables - Descriptive Text Arrays
 *
 * These string arrays provide human-readable descriptions for army system
 * components including unit property traits and classification names.
 * They support UI display, debugging output, and player information systems.
 *
 * traits_list[]: Descriptive names for unit property flags (UP_* constants)
 * ainfo_clist[]: Classification names for army classes (Aclasstype values)
 */
extern char *traits_list[];		/* Array of unit property trait descriptions */
extern char *ainfo_clist[];		/* Array of unit classification names */

/*
 * Army System Configuration Counters - Array Size Information
 *
 * These global variables provide the count of elements in the various
 * army system arrays, enabling safe iteration and bounds checking
 * throughout the military system modules.
 *
 * num_armytypes: Total number of unit types defined in ainfo_list[]
 * num_atraits: Total number of property traits defined in traits_list[]
 * num_aclasses: Total number of classifications defined in ainfo_clist[]
 */
extern int num_armytypes, num_atraits, num_aclasses;
