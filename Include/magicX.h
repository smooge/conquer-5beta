/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * magicX.h - Magical Powers and Abilities System
 *
 * This header defines the complete magical system framework for the strategy game,
 * including three distinct classes of magical powers: Military, Civilian, and Wizardry.
 * Each power provides specific bonuses to national attributes and capabilities.
 *
 * The magic system uses bit flags for efficient power storage and checking, allowing
 * nations to possess multiple powers simultaneously within each class. Powers have
 * prerequisites and affect national attributes (butes) defined in butesX.h.
 *
 * Magic Classes:
 *   - Military Magic (MM_*): Combat and warfare-related powers
 *   - Civilian Magic (MC_*): Economic, social, and infrastructure powers  
 *   - Wizardry Magic (MW_*): Mystical and supernatural abilities
 *
 * Integration:
 *   - Works with butesX.h for national attribute modifications
 *   - Data tables defined in datamagX.c for power descriptions and effects
 *   - Command execution system integration via executeX.h (optional)
 */
#ifndef MAG_MILITARY
#include "butesX.h"

/*
 * ============================================================================
 * MAGICAL POWER BIT FLAG DEFINITIONS
 * ============================================================================
 * 
 * Each magic power is defined as a unique bit flag using long integers.
 * This allows efficient storage and checking of multiple powers using bitwise
 * operations. Nations can possess any combination of powers within each class.
 */

/*
 * Military Magic Powers (MM_*) - Combat and Warfare Abilities
 * 
 * These powers enhance military capabilities, unit effectiveness, and combat
 * strategies. Military magic focuses on direct warfare improvements and
 * specialized military unit types. Data and descriptions in datamagX.c.
 */
#define MM_ARCHERY	0x00000001L	/* Enhanced ranged combat effectiveness */
#define MM_ARMOR	0x00000002L	/* Improved defensive capabilities and protection */
#define MM_AVIAN	0x00000004L	/* Flying unit creation and aerial combat */
#define MM_CAPTAIN	0x00000008L	/* Leadership bonuses and command effectiveness */
#define MM_DRAGON	0x00000010L	/* Powerful dragon units and fire-based attacks */
#define MM_EQUINE	0x00000020L	/* Cavalry units and mounted combat bonuses */
#define MM_NINJA	0x00000040L	/* Stealth operations and infiltration abilities */
#define MM_OGRE		0x00000080L	/* Giant humanoid units with massive strength */
#define MM_ORC		0x00000100L	/* Orcish units and brutal combat tactics */
#define MM_SAPPER	0x00000200L	/* Siege warfare and fortification destruction */
#define MM_WARLORD	0x00000400L	/* Advanced military leadership and strategy */
#define MM_WARRIOR	0x00000800L	/* Enhanced basic combat unit effectiveness */

/*
 * Civilian Magic Powers (MC_*) - Economic and Infrastructure Abilities
 * 
 * These powers enhance economic development, population management, infrastructure,
 * and civilian capabilities. Civilian magic focuses on nation-building, resource
 * management, and societal advancement. Data and descriptions in datamagX.c.
 */
#define MC_ACCOUNTANT	0x00000001L	/* Enhanced financial management and taxation */
#define MC_AMPHIBIAN	0x00000002L	/* Aquatic units and underwater operations */
#define MC_ARCHITECT	0x00000004L	/* Advanced construction and building efficiency */
#define MC_BOTANY	0x00000008L	/* Agricultural improvements and plant cultivation */
#define MC_BREEDER	0x00000010L	/* Animal husbandry and livestock management */
#define MC_DEMOCRACY	0x00000020L	/* Democratic governance and civic participation */
#define MC_DERVISH	0x00000040L	/* Religious zealotry and spiritual warfare */
#define MC_FARMING	0x00000080L	/* Enhanced agricultural production and food supply */
#define MC_JEWELER	0x00000100L	/* Luxury goods production and wealth generation */
#define MC_MARINE	0x00000200L	/* Naval infantry and amphibious assault capabilities */
#define MC_METALCRAFT	0x00000400L	/* Advanced metallurgy and weapon/tool crafting */
#define MC_MINER	0x00000800L	/* Enhanced mining operations and resource extraction */
#define MC_RELIGION	0x00001000L	/* Religious influence and spiritual authority */
#define MC_ROADS	0x00002000L	/* Transportation infrastructure and trade routes */
#define MC_SAILOR	0x00004000L	/* Naval expertise and maritime commerce */
#define MC_SLAVER	0x00008000L	/* Forced labor systems and population control */
#define MC_SOCIALISM	0x00010000L	/* Collective governance and resource sharing */
#define MC_URBAN	0x00020000L	/* City development and urban planning expertise */
#define MC_WOODCRAFT	0x00040000L	/* Forestry, carpentry, and wood-based construction */

/*
 * Wizardry Magic Powers (MW_*) - Mystical and Supernatural Abilities
 * 
 * These powers provide magical and supernatural capabilities including elemental
 * control, divination, illusion, and mystical combat abilities. Wizardry magic
 * represents the most exotic and powerful magical disciplines. Data in datamagX.c.
 */
#define MW_ALCHEMY	0x00000001L	/* Transmutation of materials and magical chemistry */
#define MW_AIR		0x00000002L	/* Control over wind, storms, and atmospheric forces */
#define MW_DESTROYER	0x00000004L	/* Devastating destructive magical attacks */
#define MW_DRUIDISM	0x00000008L	/* Nature magic and communion with natural forces */
#define MW_EARTH	0x00000010L	/* Control over stone, soil, and geological forces */
#define MW_FIRE		0x00000020L	/* Pyromancy and thermal magical manipulation */
#define MW_HIDDEN	0x00000040L	/* Concealment magic and invisibility powers */
#define MW_ILLUSION	0x00000080L	/* Mind magic and reality distortion abilities */
#define MW_KNOWALL	0x00000100L	/* Omniscience and perfect information gathering */
#define MW_SEEALL	0x00000200L	/* Clairvoyance and remote viewing capabilities */
#define MW_SENDING	0x00000400L	/* Magical communication and message transmission */
#define MW_SORCERER	0x00000800L	/* General sorcerous abilities and spell mastery */
#define MW_SUMMON	0x00001000L	/* Creature summoning and binding magic */
#define MW_THEVOID	0x00002000L	/* Void magic and reality manipulation */
#define MW_VAMPIRE	0x00004000L	/* Undead powers and life-draining abilities */
#define MW_VISION	0x00008000L	/* Prophetic sight and future divination */
#define MW_WATER	0x00010000L	/* Hydromancy and aquatic magical control */
#define MW_WEATHER	0x00020000L	/* Meteorological manipulation and climate control */
#define MW_WYZARD	0x00040000L	/* Master wizard abilities and arcane mastery */

/*
 * ============================================================================
 * MAGIC CLASS SYSTEM DEFINITIONS
 * ============================================================================
 */

/*
 * Magic Class Identifiers
 * 
 * These constants define the three major categories of magical powers.
 * Used as array indices for power storage and processing. MAG_NUMBER
 * (total number of classes) is defined in dataX.h.
 */
#define MAG_MILITARY	0	/* Military/Combat magic class index */
#define MAG_CIVILIAN	1	/* Civilian/Economic magic class index */
#define MAG_WIZARDRY	2	/* Wizardry/Mystical magic class index */

/*
 * Magic Power Manipulation Macros
 * 
 * These macros provide efficient bit manipulation for magical power testing,
 * addition, and removal. They operate on the long integer bit flags used
 * to store multiple powers within each magic class.
 */

/*
 * MAGIC - Test if a nation possesses specific magical power(s)
 * Parameters:
 *   x - Nation's power flags (long integer)
 *   y - Power bit flag(s) to test for
 * Returns: true if nation has ALL specified powers, false otherwise
 */
#define MAGIC(x,y)	(((x)&(y)) == (y))

/*
 * ADDMAGIC - Grant magical power(s) to a nation
 * Parameters:
 *   x - Nation's power flags (modified in place)
 *   y - Power bit flag(s) to add
 * Side Effects: Modifies x by adding the specified powers
 */
#define ADDMAGIC(x,y)	x |= (y)

/*
 * KILLMAGIC - Remove magical power(s) from a nation
 * Parameters:
 *   x - Nation's power flags (modified in place)  
 *   y - Power bit flag(s) to remove
 * Side Effects: Modifies x by removing the specified powers
 */
#define KILLMAGIC(x,y)	x &= ~(y)

/*
 * Nation-Specific Magic Testing Macros
 * 
 * These convenience macros test magical powers for the current nation
 * pointed to by ntn_ptr. They access the nation's powers array using
 * the appropriate magic class index.
 */
#define MIL_MAGIC(x)	MAGIC(ntn_ptr->powers[MAG_MILITARY], x)	/* Test military magic */
#define CIV_MAGIC(x)	MAGIC(ntn_ptr->powers[MAG_CIVILIAN], x)	/* Test civilian magic */
#define WIZ_MAGIC(x)	MAGIC(ntn_ptr->powers[MAG_WIZARDRY], x)	/* Test wizardry magic */

/*
 * ============================================================================
 * MAGIC SYSTEM DATA STRUCTURES
 * ============================================================================
 */

/*
 * MAGIC_STRUCT - Individual Magical Power Definition
 * 
 * This structure defines a single magical power including its identity,
 * effects, prerequisites, and game impact. Each power modifies specific
 * national attributes and may require other powers as prerequisites.
 */
typedef struct s_magic {
  char *name;			/* String identifier for the magic power */
  char *descript;		/* Brief description of what the power represents */
  char *info;			/* Detailed description of power's effects and mechanics */
  Butetype bute_num;		/* National attribute (bute) affected by this power */
  int bute_val;			/* Adjustment amount applied to the attribute */
  long stats;			/* Additional power statistics (usage uncertain) */
  long pow_need[MAG_NUMBER];	/* Prerequisites: required powers in each magic class */
} MAGIC_STRUCT, *MAGIC_PTR;

/*
 * MCLASS_STRUCT - Magical Class Definition
 * 
 * This structure defines a complete class of magical powers, organizing
 * related powers into coherent categories (Military, Civilian, Wizardry).
 * Each class has its own power list and capacity limits.
 */
typedef struct s_mclass {
  char *name;			/* Human-readable name of the magical class */
  int maxval;			/* Maximum number of different powers in this class */
  MAGIC_PTR pow_list;		/* Array of power definitions for this class */
} MCLASS_STRUCT, *MCLASS_PTR;

/*
 * ============================================================================
 * COMMAND EXECUTION SYSTEM INTEGRATION (OPTIONAL)
 * ============================================================================
 * 
 * This section provides integration with the command execution system for
 * logging and replaying magical actions. Only compiled when USE_CODES is
 * defined, enabling command recording and playback functionality.
 */
#ifdef USE_CODES

#include "executeX.h"

/*
 * Magic Command Execution Range
 * 
 * These constants define the range of execution codes used for magical
 * operations, establishing boundaries for magic-related command processing.
 */
#define EX_MGKBEGIN	EX_MGK_ADJ	/* First magic execution code */
#define EX_MGKEND	EX_MGKSENDING	/* Last magic execution code */

#ifdef PRINT_CODES
/*
 * Magic Command Logging Macros
 * 
 * These macros write magic-related commands to the execution log file
 * for replay and debugging purposes. They format magic operations into
 * standardized command records.
 */

/*
 * MGKADJ - Log magical power adjustment command
 * Records changes to a nation's magical power levels
 */
#define MGKADJ	fprintf(fexe, "L_MGKADJ\t%d\t%d\t%ld\t%ld\tnull\tnull\n", EX_MGK_ADJ, global_int, ntn_ptr->powers[global_int], global_long)

/*
 * MGKSPELL - Log spell casting command  
 * Records magical spell usage at specific map coordinates
 */
#define MGKSPELL	fprintf(fexe, "X_MGKSPELL\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_MGKSPELL, spellnum, XREAL, YREAL)

/*
 * MGKSEND - Log magical sending/summoning command
 * Records magical creature sending or summoning operations
 */
#define MGKSEND	fprintf(fexe, "X_MGKSEND\t%d\t%d\t%d\t0\tnull\tnull\n", EX_MGKSENDING, monst_val, ntn_num)

#endif /* PRINT_CODES */
#endif /* USE_CODES */

/*
 * ============================================================================
 * EXTERNAL DATA DECLARATIONS
 * ============================================================================
 * 
 * These declarations reference the magical power data tables defined in
 * datamagX.c. They provide access to the complete magical system data
 * including power lists for each class and class management structures.
 */

/*
 * Magical Power Data Tables
 * 
 * These arrays contain the complete definitions for all magical powers
 * in each class, including names, descriptions, effects, and prerequisites.
 * Actual data is defined in datamagX.c.
 */
extern MAGIC_STRUCT milpow_list[];	/* Military magic power definitions */
extern MAGIC_STRUCT civpow_list[];	/* Civilian magic power definitions */  
extern MAGIC_STRUCT wizpow_list[];	/* Wizardry magic power definitions */

/*
 * Magic Class Management Array
 * 
 * This array contains the class definitions for all three magic types,
 * providing access to class names, limits, and power lists. Indexed by
 * the MAG_* constants (MAG_MILITARY, MAG_CIVILIAN, MAG_WIZARDRY).
 */
extern MCLASS_STRUCT mclass_list[MAG_NUMBER];

#endif /* MAG_MILITARY */
