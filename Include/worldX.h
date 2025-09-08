/* conquer : Copyright (c) 1991 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * worldX.h - World System Configuration and Global Game Parameter Interface
 *
 * This header defines the complete world system configuration for the Conquer strategy game,
 * providing convenient accessor macros for global game parameters, world generation settings,
 * economic factors, combat mechanics, and optional code generation for debugging and replay
 * functionality.
 *
 * The world system forms the foundation of the entire game simulation, controlling fundamental
 * parameters that govern world generation, economic systems, combat mechanics, diplomatic
 * relations, and strategic gameplay balance. These parameters determine the character and
 * difficulty of each game world, affecting everything from terrain generation to trade values.
 *
 * Key Components:
 * - World Generation Parameters: Terrain, geography, and resource distribution
 * - Economic Configuration: Trade goods, mercenary costs, and resource values
 * - Combat System Settings: Damage calculation, mercenary statistics, and battle mechanics
 * - Diplomatic Parameters: Relationship limits and international interaction rules
 * - Infrastructure Settings: Building distances, supply limits, and logistical constraints
 * - Nation Management: Score calculation, attribute limits, and player empire controls
 * - Code Generation System: Optional debugging and replay functionality for world events
 *
 * Integration:
 * - Used by world generation modules for terrain and resource creation
 * - Integrated with economic systems for trade and resource valuation
 * - Connected to combat systems for damage calculations and mercenary management
 * - Linked to diplomatic systems through relationship and interaction parameters
 * - Supports administrative tools through nation and sector manipulation interfaces
 * - Enables debugging and analysis through comprehensive logging capabilities
 *
 * Modernization Notes:
 * - Consider converting macros to const variables for type safety
 * - Evaluate configuration system for runtime parameter adjustment
 * - Review global world structure access patterns for encapsulation
 * - Assess parameter validation and bounds checking requirements
 * - Consider configuration file loading for modifiable game parameters
 */

/*
 * =============================================================================
 * WORLD GENERATION AND TERRAIN PARAMETERS - Geographic Configuration Interface
 * =============================================================================
 *
 * These macros provide access to fundamental world generation parameters that
 * control terrain distribution, geographic features, and environmental factors
 * during world creation and ongoing simulation.
 *
 * Terrain Generation:
 * - PWATER: Percentage of water tiles in world generation
 * - PMOUNT: Percentage of mountain tiles for terrain variation
 * - EXPOSURE: Exposure level affecting weather and environmental factors
 * - GROWTH: Growth rate for population and economic development
 *
 * Monster and Event Distribution:
 * - PSCOUT: Probability of scout encounters and reconnaissance events
 * - PMONSTER: Monster generation probability for wilderness encounters
 * - PREVOLT: Revolt probability affecting political stability
 * - NUMRANDOM: Random event frequency for world dynamics
 * - NUMWEATHER: Weather pattern frequency and intensity
 */
/* World generation and terrain distribution parameters */
#define PWATER		world.pwater        /* Percentage of water tiles in world generation */
#define PMOUNT		world.pmount        /* Percentage of mountain tiles for terrain variation */
#define EXPOSURE	world.exposelvl     /* Exposure level affecting weather and environmental factors */
#define GROWTH		world.growthrate    /* Growth rate for population and economic development */

/* Monster and event probability parameters */
#define PSCOUT		world.pscout        /* Probability of scout encounters and reconnaissance events */
#define PMONSTER	world.pmonster      /* Monster generation probability for wilderness encounters */
#define PREVOLT		world.prevolt       /* Revolt probability affecting political stability */
#define NUMRANDOM	world.numrandom     /* Random event frequency for world dynamics */
#define NUMWEATHER	world.numweather    /* Weather pattern frequency and intensity */
#define BRIBELEVEL	world.bribelevel    /* Base bribery cost and corruption resistance level */

/*
 * =============================================================================
 * ECONOMIC SYSTEM PARAMETERS - Trade, Commerce, and Resource Configuration
 * =============================================================================
 *
 * Economic parameters controlling trade values, mercenary costs, resource
 * distribution, and commercial activity throughout the game world.
 */

/* Mercenary and military economic parameters */
#define PMERCVAL	world.pmercval      /* Base mercenary unit cost multiplier */
#define PMERCMONST	world.pmercmonst    /* Mercenary vs monster encounter probability */
#define	MERCMEN		world.m_mil        /* Mercenary military strength rating */
#define	MERCATT		world.m_aplus      /* Mercenary attack bonus modifier */
#define	MERCDEF		world.m_dplus      /* Mercenary defense bonus modifier */

/* Trade goods and commercial activity parameters */
#define PTRADE		world.ptrade        /* Overall trade activity probability and frequency */
#define PTGMETAL	world.ptgmetal      /* Metal trade goods generation probability */
#define PTGJEWEL	world.ptgjewel      /* Jewel trade goods generation probability */  
#define PTGSPELL	world.ptgspell      /* Spell components trade goods generation probability */

/*
 * =============================================================================
 * INFRASTRUCTURE AND LOGISTICS PARAMETERS - Distance and Supply Configuration
 * =============================================================================
 *
 * Infrastructure parameters controlling building placement, supply lines,
 * transportation networks, and logistical constraints across the game world.
 */

/* Building and construction distance limits */
#define BUILDDIST	world.builddist     /* Maximum distance for building placement from cities */
#define CITYXFDIST	world.cityxfdist    /* City transfer distance for resource exchange */
#define NVSPLYDIST	world.nvsplydist    /* Naval supply line maximum effective distance */

/* Supply and logistics constraints */
#define MAXPTS		world.maxpts        /* Maximum action points per turn for nations */
#define MAXSUPPLIES	world.supplylimit   /* Supply limit for military units and operations */
#define MAXDIPADJ	world.maxdipadj     /* Maximum diplomatic adjustment per turn */

/* Transportation and cargo values */
#define FLEET_CBVAL	world.fleet_cbval   /* Fleet cargo carrying capacity base value */
#define WAGON_CBVAL	world.wagon_cbval   /* Wagon cargo carrying capacity base value */

/*
 * =============================================================================
 * COMBAT SYSTEM PARAMETERS - Battle Mechanics and Damage Configuration
 * =============================================================================
 *
 * Combat parameters controlling battle resolution, damage calculation,
 * and military engagement mechanics throughout the strategic game system.
 */

/* Combat resolution and damage calculation */
#define NUMDICE		world.num_dice      /* Number of dice used in combat resolution */
#define AVG_DAMAGE	world.avg_damage    /* Average damage per successful combat round */
#define DAMAGE_LIMIT	world.damage_limit  /* Maximum damage possible in single combat round */
#define OVERMATCH_ADJ	world.overmatch_adj /* Damage adjustment for overwhelming force advantage */
#define PMINDAMAGE	world.pmindamage    /* Minimum damage percentage for successful attacks */

/*
 * =============================================================================
 * WORLD STATISTICS AND GLOBAL STATE - Administrative and Scoring Parameters
 * =============================================================================
 *
 * Global world state tracking parameters for administrative functions,
 * scoring systems, and overall game world management and analysis.
 */

/* Global world resource and civilization tracking */
#define	WORLDMTRLS	world.w_mtrls       /* Total world materials available across all sectors */
#define	WORLDSCORE	world.w_score       /* Combined world score for all active nations */
#define	WORLDCIV	world.w_civ         /* Overall civilization level across the world */
#define	WORLDSCT	world.w_sctrs       /* Total number of sectors in the world map */
#define	WORLDMIL	world.w_mil         /* Total military units across all nations */
#define	WORLDNTN	world.active_ntns   /* Number of active nations currently in the game */

/*
 * =============================================================================
 * DISTANCE AND TRANSFER CALCULATION CONSTANTS - Logistics Configuration
 * =============================================================================
 *
 * Special constants for distance calculations and resource transfer operations,
 * providing conditional logic for different transfer distance scenarios.
 */

/* Distance calculation and transfer limit constants */
#define NTN_DISTIND	50                  /* Nation distance indicator threshold value */
#define MAX_TRANSFER	((CITYXFDIST == NTN_DISTIND)?COMM_I_RANGE:CITYXFDIST)  /* Conditional max transfer distance */

/*
 * =============================================================================
 * OPTIONAL CODE GENERATION SYSTEM - World Operations Debugging and Replay
 * =============================================================================
 *
 * When USE_CODES is defined, this section provides structured logging
 * functionality for world operations, nation management, sector modifications,
 * and resource transfers, enabling debugging, replay analysis, and comprehensive
 * game state tracking.
 *
 * Code Generation Features:
 * - Nation management operations with complete state tracking
 * - Sector modification logging with coordinate and property changes
 * - Resource transfer operations for economic transaction analysis
 * - Administrative command logging for game management actions
 * - Structured output format for automated analysis and replay tools
 */
#ifdef USE_CODES

#include "executeX.h"

/*
 * Command Range Definitions - Operation Code Boundaries
 *
 * These definitions establish the boundaries for different categories of
 * operations in the code generation system, enabling organized logging
 * and efficient command parsing for replay and analysis tools.
 */

/* Nation management command range */
#define EX_NTNBEGIN	EX_NTNNAME      /* First nation operation code */
#define EX_NTNEND	EX_UNUMDEFAULT  /* Last nation operation code */

/* Sector modification command range */
#define EX_SCTBEGIN	EX_SCTDESG      /* First sector operation code */
#define EX_SCTEND	EX_SCTMNRLS     /* Last sector operation code */

/* Resource transfer command ranges */
#define EX_TAKEBEGIN	EX_TAKESTART    /* First resource take operation code */
#define EX_TAKEEND	EX_TAKEFINISH   /* Last resource take operation code */
#define EX_GIVEBEGIN	EX_GIVESTART    /* First resource give operation code */
#define EX_GIVEEND	EX_GIVEFINISH   /* Last resource give operation code */

/*
 * Nation Management Logging Macros - Complete Empire State Tracking
 *
 * These macros generate formatted output for nation management operations
 * when PRINT_CODES is enabled. Each macro logs specific nation modifications
 * with standardized tab-separated format for external processing and replay.
 *
 * Output Format: COMMAND\tCODE\tPARAM1\tPARAM2\tPARAM3\tNAME\tEXTRA
 * - COMMAND: Operation type identifier
 * - CODE: Execution code number from executeX.h
 * - PARAM1-3: Operation-specific parameters
 * - NAME: Nation name or identifier
 * - EXTRA: Additional data or "null"
 */
#ifdef PRINT_CODES

/* Nation identification and leadership logging */
#define XADJNAME	fprintf(fexe, "X_NAME\t%d\t0\t0\t0\t%12s\tnull\n", EX_NTNNAME, ntn_ptr->name)
#define XADJLOGIN	fprintf(fexe, "X_LOGIN\t%d\t0\t0\t0\t%12s\tnull\n", EX_NTNLOGIN, ntn_ptr->login)
#define XADJPASSWD	fprintf(fexe, "X_PASSWD\t%d\t0\t0\t0\t%12s\tnull\n", EX_NTNPASSWD, ntn_ptr->passwd)
#define XADJLEADER	fprintf(fexe, "X_LEADER\t%d\t0\t0\t0\t%12s\tnull\n", EX_NTNLEADER, ntn_ptr->leader)

/* Nation location and geographical boundaries */
#define XADJLOC	fprintf(fexe, "X_LOC\t%d\t0\t%d\t%d\tnull\tnull\n", EX_NTNLOC, (int)ntn_ptr->capx, (int)ntn_ptr->capy)
#define XADJRLOC	fprintf(fexe, "X_RLOC\t%d\t0\t%d\t%d\tnull\tnull\n", EX_NTNRLOC, (int)ntn_ptr->centerx, (int)ntn_ptr->centery)
#define XADJLEDGE	fprintf(fexe, "X_LEDGE\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNLEDGE, (int)ntn_ptr->leftedge)
#define XADJREDGE	fprintf(fexe, "X_REDGE\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNREDGE, (int)ntn_ptr->rightedge)
#define XADJTEDGE	fprintf(fexe, "X_TEDGE\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNTEDGE, (int)ntn_ptr->topedge)
#define XADJBEDGE	fprintf(fexe, "X_BEDGE\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNBEDGE, (int)ntn_ptr->bottomedge)

/* Military capabilities and combat statistics */
#define XADJAPLUS	fprintf(fexe, "X_APLUS\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNAPLUS, (int)ntn_ptr->aplus)
#define XADJDPLUS	fprintf(fexe, "X_DPLUS\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNDPLUS, (int)ntn_ptr->dplus)
#define XADJMOVE	fprintf(fexe, "X_MOVE\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNMOVE, (int)ntn_ptr->maxmove)

/* Nation status and administrative properties */
#define XADJACT	fprintf(fexe, "X_ACTIVE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NTNACTIVE, country, (int)ntn_ptr->active)
#define XADJCLASS	fprintf(fexe, "X_CLASS\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNCLASS, (int)ntn_ptr->class)
#define XADJRACE	fprintf(fexe, "X_RACE\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNRACE, (int)ntn_ptr->race)
#define XADJREPRO	fprintf(fexe, "X_REPRO\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNLOC, (int)ntn_ptr->repro)
#define XADJMARK	fprintf(fexe, "X_MARK\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNMARK, (int)ntn_ptr->mark)

/* Diplomatic relations and international status */
#define XADJDIPLO	fprintf(fexe, "X_DIPLO\t%d\t0\t%d\t%d\tnull\tnull\n", EX_NTNDIPLO, global_int, ntn_ptr->dstatus[global_int])
#define XADJBUTE	fprintf(fexe, "X_BUTE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NTNBUTE, global_int, (int)ntn_ptr->attribute[global_int])

/* Scoring, magic, and special capabilities */
#define XADJSCORE	fprintf(fexe, "L_SCORE\t%d\t0\t%ld\t0\tnull\tnull\n", EX_NTNSCORE, ntn_ptr->score)
#define XADJSPTS	fprintf(fexe, "X_SPELL\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNSPTS, (int)ntn_ptr->spellpts)

/* Economic and communication operations */
#define XBRIBE	fprintf(fexe, "D_BRIBE\t%d\t%d\t%f\t0\tnull\tnull\n", EX_BRIBE, global_int, (double)global_long)
#define XADJNEWS	fprintf(fexe, "L_NEWS\t%d\t0\t%ld\t0\tnull\tnull\n", EX_NEWSSIZE, global_long)
#define XADJMAIL	fprintf(fexe, "L_MAIL\t%d\t0\t%ld\t0\tnull\tnull\n", EX_MAILSIZE, global_long)

/* Administrative unit numbering system operations */
#define XNRENUM	fprintf(fexe, "X_NRENUM\t%d\t0\t0\t0\tnull\tnull\n", EX_NTNRENUM)
#define XUNUMCOPY	fprintf(fexe, "X_UNCPY\t%d\t0\t0\t0\tnull\tnull\n", EX_UNUMCOPY)
#define XUNUMRESET	fprintf(fexe, "X_UNRST\t%d\t0\t0\t0\tnull\tnull\n", EX_UNUMRESET)
#define XUNUMDEFAULT	fprintf(fexe, "X_UNDFLT\t%d\t0\t0\t0\tnull\tnull\n", EX_UNUMDEFAULT)
#define XUNUMSLOT	fprintf(fexe, "X_UNSLT\t%d\t%d\t%d\t%ld\tnull\tnull\n", EX_UNUMSLOT, x, global_int, global_long)

/*
 * Sector Modification Logging Macros - Terrain and Resource State Tracking
 *
 * These macros generate formatted output for sector modification operations,
 * logging changes to terrain properties, ownership, resources, and geographic
 * features with coordinate-based identification for map analysis tools.
 */

/* Sector property and ownership modification logging */
#define SADJDESG	fprintf(fexe, "S_DESG\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_SCTDESG, sct[XREAL][YREAL].designation, XREAL, YREAL)
#define SADJOWN	fprintf(fexe, "S_OWN\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_SCTOWN, sct[XREAL][YREAL].owner, XREAL, YREAL)
#define SADJPEOP	fprintf(fexe, "L_PEOP\t%d\t%d\t%ld\t%ld\tnull\tnull\n", EX_SCTPEOPLE, XREAL, (long)YREAL, sct[XREAL][YREAL].people)

/* Geographic and terrain feature modification logging */
#define SADJALT	fprintf(fexe, "S_ALT\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_SCTALT, sct[XREAL][YREAL].altitude, XREAL, YREAL)
#define SADJVEG	fprintf(fexe, "S_VEG\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_SCTVEG, sct[XREAL][YREAL].vegetation, XREAL, YREAL)

/* Economic resource and trade good modification logging */
#define SADJTGOOD	fprintf(fexe, "S_TGOOD\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_SCTTGOOD, sct[XREAL][YREAL].tradegood, XREAL, YREAL)
#define SADJMNRLS	fprintf(fexe, "S_MNRLS\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_SCTMNRLS, sct[XREAL][YREAL].minerals, XREAL, YREAL)

/*
 * Resource Transfer Operation Logging Macros - Economic Transaction Tracking
 *
 * These macros generate formatted output for resource allocation and transfer
 * operations, providing complete transaction logging for economic analysis,
 * trade monitoring, and resource flow tracking across the game world.
 */

/* Resource extraction and collection operations */
#define TAKESTART	fprintf(fexe, "T_START\t%d\t0\t%d\t%d\tnull\tnull\n", EX_TAKESTART, XREAL, YREAL)
#define TAKEMTRLS(x)	fprintf(fexe, "L_TMTRLS\t%d\t%d\t%ld\t%ld\tnull\tnull\n", EX_TAKEMTRLS + (x), XREAL, (long)YREAL, global_long)
#define TAKEFINISH(x)	fprintf(fexe, "T_FINISH\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_TAKEFINISH, (int) x, XREAL, YREAL)

/* Resource distribution and allocation operations */
#define GIVESTART	fprintf(fexe, "G_START\t%d\t0\t%d\t%d\tnull\tnull\n", EX_GIVESTART, XREAL, YREAL)
#define GIVEMTRLS(x)	fprintf(fexe, "L_GMTRLS\t%d\t%d\t%d\t%ld\tnull\tnull\n", EX_GIVEMTRLS + (x), XREAL, YREAL, global_long)
#define GIVEFINISH(x)	fprintf(fexe, "G_FINISH\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_GIVEFINISH, (int) x, XREAL, YREAL)

#endif /* PRINT_CODES */
#endif /* USE_CODES */
