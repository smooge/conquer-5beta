/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * statusX.h - Military Unit Status System Definitions
 *
 * This header defines the comprehensive status system for military units (armies, navies, caravans)
 * including base tactical statuses, extended behavioral flags, speed control, and special abilities.
 * The status system controls unit behavior in combat, movement, supply operations, and tactical
 * positioning, providing the foundation for strategic military gameplay.
 *
 * The system uses a bit-packed encoding scheme combining:
 * - 6-bit base status (0-21): Primary tactical stance and operational mode
 * - Extended flags: Special abilities, magical effects, leadership, and movement capabilities
 * - 2-bit speed encoding: Movement speed from slow to march with special states
 * - 26 special behavior flags: Combat modifiers, restrictions, and tactical capabilities
 *
 * Key Design Features:
 * - Efficient bit-packing for memory optimization in large-scale military simulations
 * - Comprehensive macro interface for status testing and modification operations
 * - Flexible special behavior system supporting complex military tactical rules
 * - Unit type classification (army/navy/caravan) with appropriate status restrictions
 * - Integration with combat, supply, siege, and movement systems throughout the game
 */

/* Base Unit Statuses - Primary Tactical Stance and Operational Mode (0-21) */
/*
 * Base Status Constants - Primary tactical stance determining unit behavior in combat,
 * movement, and interaction with other units. These form the 6-bit base status field
 * that defines the unit's primary operational mode and tactical positioning.
 */
#define ST_SORTIE	0    /* Offensive sortie from fortified position - aggressive stance with sally attacks */
#define ST_AMBUSH	1    /* Hidden ambush position - concealed until combat, surprise attack bonus */
#define ST_ATTACK	2    /* Active attack mode - seeking combat, will initiate battles with enemies */
#define ST_ENGAGE	3    /* Engaged in combat - currently fighting, locked in battle until resolution */
#define ST_DEFEND	4    /* Defensive stance - protecting territory, fortification combat bonuses */
#define ST_GARRISON	5    /* Garrison duty - defending fixed position, enhanced territorial control */
#define ST_ONBOARD	6    /* Embarked on transport - loaded on ship/caravan, cannot act independently */
#define ST_SIEGE	7    /* Besieging enemy position - attacking fortified locations, siege warfare */
#define ST_GROUPED	8    /* Grouped with other units - combined operations, shared command structure */
#define ST_SIEGED	9    /* Under siege - trapped in fortification, supply restrictions, sortie options */
#define ST_RESERVE	10   /* Strategic reserve - held back from combat, rapid deployment capability */
#define ST_SWEEP	11   /* Sweep operation - partial sector control, reconnaissance and harassment */
#define ST_WORKCREW	12  /* Construction crew - building infrastructure, non-combat engineering role */
#define ST_TRADED	13   /* Trade mission - commercial operation, reduced combat capability */
#define ST_CARRY	14   /* Carrying supplies - logistics operation, supply transport and delivery */
#define ST_DECOY	15   /* Decoy unit - misdirection tactics, false target to confuse enemies */
#define ST_SUPPLY	16   /* Supply depot - providing logistics support to nearby units */
#define ST_REPAIR	17   /* Repair operations - maintaining equipment and infrastructure */
#define ST_ONBSPLY	18  /* Onboard supply - embarked supply unit, ship-based logistics support */
#define ST_SSUPPLY	19  /* Self-supply - independent logistics, enhanced operational autonomy */
#define ST_ONBSSPLY	20 /* Onboard self-supply - embarked independent logistics, ship-based autonomy */
#define ST_ROVER	21   /* Roving patrol - autonomous territorial acquisition, independent operations */

/* Extended Status Flags - Special Abilities and Temporary Effects */
/*
 * Extended Status Indicators - Bit flags for special abilities, magical effects,
 * leadership, flight capability, and speed encoding. These combine with base status
 * to provide comprehensive unit state tracking and capability management.
 */
#define SX_MAGIC	0x00100L  /* Unit affected by magic - enhanced abilities, spell effects active */
#define SX_LEADER	0x00200L  /* Unit has leadership capability - command bonus, unit coordination */
#define SX_FLIGHT	0x00400L  /* Unit has flight ability - aerial movement, terrain independence */
#define SX_HEALED	0x00800L  /* Unit recently healed - medical treatment applied, recovery state */
#define SX_SPEED	0x000C0L  /* Speed encoding mask - 2-bit field for movement speed state */
#define SX_SHOWSTAT	0x0003FL  /* Base status display mask - 6-bit field for primary status (0-63) */
#define SX_SPDSLOT	6         /* Speed bit shift position - speed field starts at bit 6 */

/* Unit Speed Classifications - Movement Rate Control System */
/*
 * Movement Speed Constants - Define unit movement rates affecting tactical positioning,
 * strategic mobility, and operational tempo. Speed is encoded in a 2-bit field within
 * the extended status flags, providing 4 primary speeds plus special states.
 */
#define SPD_SLOW	0  /* Slow movement - cautious advance, enhanced stealth, reduced detection */
#define SPD_NORMAL	1  /* Normal movement - standard operational speed, balanced mobility */
#define SPD_MARCH	2  /* Forced march - rapid movement, extended range, increased fatigue */
#define SPD_STUCK	3  /* Immobilized - cannot move, pinned down, mechanical failure */
#define SPD_PATROL	4  /* Patrol speed - systematic area coverage, reconnaissance operations */
#define SPD_MINIMUM	0  /* Minimum speed constant - lower bound for speed validation */
#define SPD_NUMBER	5  /* Total speed count - number of defined speed states */

/* Status Manipulation Macros - Efficient Bit Operations for Status Management */
/*
 * Status Modifier Macros - Provide efficient bit manipulation operations for setting
 * and clearing status flags. These macros handle the bit-packed encoding safely
 * while preserving other status information during updates.
 */
#define set_speed(x,y)	x = ((x & ~SX_SPEED) | ((y) << SX_SPDSLOT))  /* Set unit movement speed */
#define set_status(x,y)	x = ((x & ~SX_SHOWSTAT) | (y))              /* Set base tactical status */
#define set_leading(x)	x |= SX_LEADER      /* Grant leadership capability to unit */
#define set_unlead(x)	x &= ~SX_LEADER     /* Remove leadership capability from unit */
#define set_spelled(x)	x |= SX_MAGIC       /* Apply magical effects to unit */
#define set_unspell(x)	x &= ~SX_MAGIC      /* Remove magical effects from unit */
#define set_flight(x)	x |= SX_FLIGHT      /* Grant flight ability to unit */
#define set_unflight(x)	x &= ~SX_FLIGHT     /* Remove flight ability from unit */
#define set_healed(x)	x |= SX_HEALED      /* Mark unit as recently healed */
#define set_unhealed(x)	x &= ~SX_HEALED     /* Clear healed status from unit */

/*
 * Status Query Macros - Extract specific status information from bit-packed fields.
 * These macros provide safe read access to status components without modifying state.
 */
#define unit_speed(x)	(((x) & SX_SPEED) >> SX_SPDSLOT)  /* Extract movement speed value */
#define unit_status(x)	((x) & SX_SHOWSTAT)               /* Extract base tactical status */
#define unit_leading(x)	((x) & SX_LEADER)                 /* Test if unit has leadership */
#define unit_spelled(x)	((x) & SX_MAGIC)                  /* Test if unit has magic effects */
#define unit_flight(x)	((x) & SX_FLIGHT)                 /* Test if unit has flight ability */
#define unit_healed(x)	((x) & SX_HEALED)                 /* Test if unit recently healed */
#define unit_built(x)	((x) & SX_BUILT)                  /* Test if unit involved in construction */
#define unit_orig(x)	((x) & SX_ORIGSTAT)               /* Extract original status information */

/* Status Information Structure - Complete Status Definition and Behavior */
/*
 * STATUS_STRUCT - Comprehensive status type definition providing complete behavioral
 * specification for each military unit status. This structure defines the display
 * names, user interface elements, combat modifiers, and special behavioral flags
 * that control how units with this status interact with the game world.
 *
 * The structure supports the full status system including UI presentation,
 * combat calculations, and special rule enforcement for tactical gameplay.
 */
typedef struct s_status {
  char *name;		/* Full descriptive name - complete status description for detailed displays */
  char *nickname;	/* Short display name - abbreviated status for compact UI elements */
  char *input;		/* Input prompt text - user instruction for status selection interface */
  int key_char;		/* Keyboard shortcut - quick selection character for rapid status changes */
  long special;		/* Special behavior flags - SPST_* bit flags defining tactical rules */
  int attackval;	/* Attack combat modifier - bonus/penalty applied during offensive combat */
  int defendval;	/* Defense combat modifier - bonus/penalty applied during defensive combat */
} STATUS_STRUCT, *STATUS_PTR;

/* Special Status Behavior Flags - Tactical Rules and Behavioral Modifiers */
/*
 * Special Status Handlers - Comprehensive bit flag system defining the tactical
 * rules, restrictions, and special abilities for each status type. These flags
 * control combat behavior, movement restrictions, supply operations, unit
 * interactions, and strategic capabilities.
 *
 * The flags are organized into functional categories:
 * - Combat Behavior: Attack patterns, combat initiation, risk assessment
 * - Territory Control: Land capture, protection, fortification usage
 * - Unit Interaction: Combination rules, cooperation restrictions
 * - Movement/Operational: Movement limitations, status transitions
 * - Supply Operations: Logistics support, supply requirements
 * - Special Abilities: Concealment, deception, unique capabilities
 * - Unit Type Classification: Army, navy, caravan applicability
 */

/* Combat Behavior Flags - Define offensive and defensive combat patterns */
#define SPST_ATT	0x00000001L	/* Aggressive combat - will initiate battles when encountering enemies */
#define SPST_CAPT	0x00000002L	/* Territory capture - may seize and control enemy land during combat */
#define SPST_DIST	0x00000004L	/* Ranged combat - can engage targets at distance without adjacency */
#define SPST_INVERSE	0x00000008L	/* Distance inverse - combat effectiveness increases with range */
#define SPST_RISKY	0x00100000L	/* High-risk target - dangerous to attack, potential heavy casualties */
#define SPST_SWEEP	0x00400000L	/* Partial engagement - limited sector attack, reconnaissance combat */

/* Concealment and Tactical Deception */
#define SPST_HIDDEN	0x00000010L	/* Concealed unit - invisible to enemy reconnaissance, ambush capability */
#define SPST_DECOY	0x00020000L	/* Deception unit - false target, misdirection tactics, feint operations */

/* Fortification and Defensive Positioning */
#define SPST_FORT	0x00000020L	/* Fortification bonus - utilizes defensive structures, enhanced protection */
#define SPST_GROUNDED	0x00008000L	/* Terrain advantage - gains combat bonus from favorable ground */

/* Unit Combination and Cooperation Rules */
#define SPST_UNCOMB	0x00000040L	/* Isolation restriction - cannot combine with units of different status */
#define SPST_NOLIKE	0x00000080L	/* Exclusivity restriction - cannot combine with units of same status */

/* Status Transition and Movement Restrictions */
#define SPST_UNSWITCH	0x00000100L	/* Status locked - cannot voluntarily change to different status */
#define SPST_UNSPLIT	0x00000200L	/* Indivisible unit - cannot split into smaller formations */
#define SPST_UNMOVE	0x00000400L	/* Immobilized - movement prohibited, fixed position operations */
#define SPST_SIEGED	0x00000800L	/* Siege vulnerable - status can transition to besieged state */
#define SPST_SORTIE	0x00001000L	/* Sortie capable - can transition to offensive sortie operations */

/* Territory Control and Protection Capabilities */
#define SPST_NOHOLD	0x00002000L	/* No territorial control - cannot protect or claim territory */
#define SPST_HOLDIT	0x00004000L	/* Enhanced control - superior territorial protection, double effectiveness */

/* Supply and Logistics Operations */
#define SPST_SUPPLY	0x00010000L	/* Supply center - provides logistics support to nearby units */
#define SPST_SSUPPLY	0x20000000L	/* Self-supply - independent logistics, supplies own operations */
#define SPST_SPLYINSECT	0x00040000L	/* Local supply - must be in same sector to provide supply support */

/* Special Protection and Priority Systems */
#define SPST_DIESLAST	0x00080000L	/* Protected unit - other units absorb damage first, command priority */

/* User Interface and Control Flags */
#define SPST_USABLE	0x00200000L	/* Player selectable - status can be directly assigned by players */

/* Transportation and Embarked Operations */
#define SPST_ONSHIP	0x00800000L	/* Embarked unit - loaded on naval transport, ship-based operations */

/* Construction and Economic Operations */
#define SPST_ITEMINFO	0x01000000L	/* Construction role - unit involved in building/item creation */

/* Magical Restrictions and Capabilities */
#define SPST_NOCAST	0x02000000L	/* Magic prohibited - cannot cast spells or use magical abilities */

/* Unit Type Classification and Applicability */
#define SPST_ASTATUS	0x04000000L	/* Army status - applicable to land-based military units */
#define SPST_NSTATUS	0x08000000L	/* Navy status - applicable to naval fleet units */
#define SPST_VSTATUS	0x10000000L	/* Caravan status - applicable to trade and transport units */

/* Autonomous and Strategic Operations */
#define SPST_ROVER	0x40000000L	/* Autonomous patrol - independent territorial acquisition, roaming operations */

/* Status Capability Testing Macros - Efficient Boolean-Style Queries */
/*
 * Status Testing Macros - Provide convenient boolean-style testing of unit status
 * capabilities by extracting the base status and checking the corresponding special
 * behavior flags. These macros access the global stat_info[] array to determine
 * if a unit's current status has specific tactical capabilities or restrictions.
 *
 * Usage Pattern: These macros take a unit's complete status value and return
 * non-zero if the capability is present, zero if not present, enabling direct
 * use in conditional statements and logical expressions.
 */

/* User Interface and Control Testing */
#define usable_stat(x)	(stat_info[unit_status(x)].special & SPST_USABLE)      /* Player-assignable status */

/* Combat Capability and Risk Assessment */
#define risky_stat(x)	(stat_info[unit_status(x)].special & SPST_RISKY)       /* High-risk target */
#define sweep_stat(x)	(stat_info[unit_status(x)].special & SPST_SWEEP)       /* Partial attack capability */
#define attack_stat(x)	(stat_info[unit_status(x)].special & SPST_ATT)         /* Aggressive combat mode */
#define capture_stat(x)	(stat_info[unit_status(x)].special & SPST_CAPT)        /* Territory capture ability */
#define distant_stat(x)	(stat_info[unit_status(x)].special & SPST_DIST)        /* Ranged combat capability */
#define inverse_stat(x)	(stat_info[unit_status(x)].special & SPST_INVERSE)     /* Distance effectiveness inverse */

/* Concealment and Tactical Deception */
#define hidden_stat(x)	(stat_info[unit_status(x)].special & SPST_HIDDEN)      /* Concealment capability */
#define decoy_stat(x)	(stat_info[unit_status(x)].special & SPST_DECOY)       /* Deception operations */

/* Defensive and Fortification Capabilities */
#define fort_stat(x)	(stat_info[unit_status(x)].special & SPST_FORT)        /* Fortification utilization */
#define grounded_stat(x)	(stat_info[unit_status(x)].special & SPST_GROUNDED)    /* Terrain combat bonus */

/* Unit Interaction and Combination Rules */
#define nocomb_stat(x)	(stat_info[unit_status(x)].special & SPST_UNCOMB)      /* No unlike combination */
#define nolike_stat(x)	(stat_info[unit_status(x)].special & SPST_NOLIKE)      /* No like combination */

/* Movement and Status Transition Restrictions */
#define nochange_stat(x)	(stat_info[unit_status(x)].special & SPST_UNSWITCH)    /* Status change locked */
#define nosplit_stat(x)	(stat_info[unit_status(x)].special & SPST_UNSPLIT)     /* Unit splitting prohibited */
#define nomove_stat(x)	(stat_info[unit_status(x)].special & SPST_UNMOVE)      /* Movement prohibited */
#define siege_stat(x)	(stat_info[unit_status(x)].special & SPST_SIEGED)      /* Siege transition capable */
#define sortie_stat(x)	(stat_info[unit_status(x)].special & SPST_SORTIE)      /* Sortie transition capable */

/* Territory Control and Protection */
#define nohold_stat(x)	(stat_info[unit_status(x)].special & SPST_NOHOLD)      /* No territorial control */
#define holdit_stat(x)	(stat_info[unit_status(x)].special & SPST_HOLDIT)      /* Enhanced territorial control */

/* Supply and Logistics Operations */
#define supply_stat(x)	(stat_info[unit_status(x)].special & (SPST_SUPPLY | SPST_SSUPPLY))  /* Any supply capability */
#define selfsply_stat(x)	(stat_info[unit_status(x)].special & SPST_SSUPPLY)     /* Self-supply capability */
#define splyinsect_stat(x)	(stat_info[unit_status(x)].special & SPST_SPLYINSECT)  /* Local supply requirement */

/* Special Protection and Priority */
#define dieslast_stat(x)	(stat_info[unit_status(x)].special & SPST_DIESLAST)    /* Protected unit priority */

/* Transportation and Embarked Operations */
#define onship_stat(x)	(stat_info[unit_status(x)].special & SPST_ONSHIP)      /* Embarked on transport */

/* Construction and Economic Operations */
#define iteminfo_stat(x)	(stat_info[unit_status(x)].special & SPST_ITEMINFO)    /* Construction involvement */

/* Magical Restrictions and Capabilities */
#define nocast_stat(x)	(stat_info[unit_status(x)].special & SPST_NOCAST)      /* Magic casting prohibited */

/* Unit Type Classification Testing */
#define aunit_stat(x)	(stat_info[unit_status(x)].special & SPST_ASTATUS)     /* Army unit status */
#define nunit_stat(x)	(stat_info[unit_status(x)].special & SPST_NSTATUS)     /* Navy unit status */
#define vunit_stat(x)	(stat_info[unit_status(x)].special & SPST_VSTATUS)     /* Caravan unit status */

/* Autonomous and Strategic Operations */
#define rover_stat(x)	(stat_info[unit_status(x)].special & SPST_ROVER)       /* Autonomous patrol capability */

/* Global Status System Variables - Complete Status Information Database */
/*
 * Global Status Variables - Provide access to the complete status system database
 * and configuration information used throughout the military unit management system.
 *
 * These variables are initialized during game startup and provide the foundational
 * data for all status-related operations, UI presentation, and tactical rule enforcement.
 */
extern STATUS_STRUCT stat_info[];  /* Status information array - complete database of all status types
                                   * with display names, combat modifiers, and behavioral flags.
                                   * Indexed by base status values (ST_* constants) to provide
                                   * O(1) lookup of status properties and capabilities. */

extern int num_statuses;           /* Total status count - number of defined status types in system.
                                   * Used for validation, iteration, and bounds checking in status
                                   * processing operations and user interface status selection. */

/*
 * Status System Integration Points:
 *
 * Combat System: stat_info[].attackval and defendval provide combat modifiers
 * User Interface: stat_info[].name, nickname, input, and key_char support UI operations
 * AI Systems: Special behavior flags (SPST_*) guide artificial intelligence decisions
 * Movement System: SPST_UNMOVE, SPST_ONSHIP flags restrict movement capabilities
 * Supply System: SPST_SUPPLY, SPST_SSUPPLY, SPST_SPLYINSECT control logistics
 * Magic System: SPST_NOCAST flag restricts magical ability usage
 * Unit Management: Status testing macros provide efficient capability queries
 *
 * Performance Characteristics:
 * - O(1) status property lookup via array indexing
 * - Efficient bit manipulation for status modification and testing
 * - Minimal memory overhead with bit-packed encoding
 * - Cache-friendly access patterns for status information queries
 *
 * Modernization Considerations for C2023 Upgrade:
 * - Convert STATUS_STRUCT to const array for immutable status definitions
 * - Replace macros with inline functions for type safety and debugging
 * - Add enum types for base status constants and special behavior flags
 * - Implement status validation functions with comprehensive error checking
 * - Consider bitfield structures for more efficient special flag encoding
 * - Add static assertions for status constant validation and array bounds
 * - Implement unit tests for all status manipulation and testing operations
 * - Add comprehensive documentation generation for status system rules
 */
