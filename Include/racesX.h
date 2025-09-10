/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * racesX.h - Race System Definitions and Species Characteristics
 *
 * This header defines the complete race system for the Conquer strategy game,
 * providing racial type constants, trait definitions, behavioral characteristics,
 * and comprehensive data structures for managing different species and their
 * unique capabilities, limitations, and game mechanics interactions.
 *
 * The race system enables diverse gameplay by providing distinct species with
 * varying magical abilities, combat skills, environmental adaptations, economic
 * preferences, and strategic advantages that affect nation development, unit
 * effectiveness, and diplomatic interactions throughout the game world.
 *
 * Key Components:
 * - Race Type Constants: Numeric identifiers for different species
 * - Racial Traits System: Bitfield flags defining species characteristics
 * - Trait Testing Macros: Convenient access to species capabilities
 * - Race Data Structure: Comprehensive species configuration and statistics
 * - External Data Interface: Global race information array declaration
 *
 * Usage Context:
 * The race system integrates with nation creation, magic systems, combat
 * mechanics, economic calculations, population management, and environmental
 * interactions to provide balanced and diverse strategic options for players
 * while maintaining game balance and thematic consistency.
 */

/*
 * Race Type Constants - Species Identification System
 *
 * Defines numeric identifiers for different playable and non-playable races
 * in the game world. These constants are used throughout the game engine
 * for race-specific logic, behavioral modifications, and statistical tracking.
 *
 * Note: RACE_NUMBER (total number of races) is defined in dataX.h and
 * determines array sizing for race-related data structures. Additional
 * race types (0-4) may be defined elsewhere in the codebase.
 */
#define LIZARD		5	/* Lizardmen - Reptilian species with unique traits */
#define PIRATE		6	/* Pirates - Seafaring raiders and merchants */
#define SAVAGE		7	/* Savages - Primitive tribal peoples */
#define NOMAD		8	/* Nomads - Wandering desert/steppe peoples */
#define TUNKNOWN	9	/* Unknown/Mysterious race - Special/NPC race */
/* RACE_NUMBER defined in dataX.h - Total number of races in game */

/*
 * Racial Traits System - Species Characteristic Bitfield Flags
 *
 * Defines behavioral and capability flags that determine racial advantages,
 * limitations, and special abilities. These bitfield constants are combined
 * in the RACE_STRUCT.traits field to create unique racial profiles affecting
 * game mechanics, unit effectiveness, and strategic options.
 *
 * Environmental Adaptations:
 * - Winter survival and terrain handling capabilities
 * - Special movement and environmental interaction bonuses
 *
 * Magical Inclinations:
 * - Magic skill levels from complete aversion to wizardly mastery
 * - Affects spell learning, mana costs, and magical research
 *
 * Economic and Military Specializations:
 * - Trade, mining, and resource management bonuses
 * - Combat effectiveness and military unit preferences
 * - Monster handling and creature summoning abilities
 *
 * Player Selection Flags:
 * - Determines which races are available for player nation creation
 * - Some races may be NPC-only or special scenario races
 */

/* Environmental and Survival Traits */
#define RT_WOODWINTER	0x00000001	/* Race can survive winter in wooded terrain without shelter */
#define RT_MOUNTAINEER	0x00000002	/* Race has improved movement and survival in mountainous regions */

/* Magical Affinity Traits (mutually exclusive extremes) */
#define RT_MAGICSKILL	0x00000004	/* Race is naturally proficient in magical arts and spellcasting */
#define RT_ANTIMAGIC	0x00000008	/* Race culturally shuns magic use and has penalties to magical activities */
#define RT_WIZARDLY	0x00000100	/* Race possesses exceptional magical mastery and advanced spellcasting */

/* Economic and Resource Specializations */
#define RT_MINING	0x00000010	/* Race excels at mining operations and material extraction */
#define RT_TRADERS	0x00000040	/* Race has superior trading skills and commercial advantages */

/* Combat and Creature Specializations */
#define RT_MONSTERSKILL	0x00000020	/* Race is skilled at handling, taming, and commanding monsters */
#define RT_FIGHTERS	0x00000200	/* Race produces superlative warriors with enhanced combat abilities */
#define RT_ANTIMILITARY	0x00000080	/* Race culturally opposes military expansion and warfare */

/* Game Mechanics and Availability */
#define RT_NOTRACE	0x00000400	/* Race is not available for player selection (NPC/special use only) */

/*
 * Racial Trait Testing Macros - Species Capability Queries
 *
 * Provides convenient boolean-style macros for testing racial characteristics
 * by combining race index lookup with bitfield trait testing. These macros
 * access the global race_info[] array and test specific trait flags.
 *
 * Parameter: x - Race index (0 to RACE_NUMBER-1)
 * Returns: Non-zero if race has the specified trait, zero otherwise
 *
 * Usage Examples:
 *   if (r_magicskill(player_race)) { apply_magic_bonus }
 *   if (r_fighters(army_race)) { enhanced_combat_effectiveness }
 *   if (r_traders(nation_race)) { improved_trade_prices }
 *
 * Note: These macros provide read-only access to racial traits and should
 * not be used for trait modification. Direct manipulation of race_info[]
 * array should be done through appropriate race management functions.
 */

/* Environmental and Survival Capability Tests */
#define r_woodwinter(x)	(race_info[x].traits & RT_WOODWINTER)	/* Winter forest survival */
#define r_mountaineer(x)	(race_info[x].traits & RT_MOUNTAINEER)	/* Mountain terrain bonus */

/* Magical Affinity and Ability Tests */
#define r_magicskill(x)	(race_info[x].traits & RT_MAGICSKILL)	/* Magic proficiency */
#define r_antimagic(x)	(race_info[x].traits & RT_ANTIMAGIC)	/* Magic aversion */
#define r_wizardly(x)	(race_info[x].traits & RT_WIZARDLY)	/* Exceptional magic mastery */

/* Economic and Resource Specialization Tests */
#define r_mining(x)	(race_info[x].traits & RT_MINING)	/* Mining expertise */
#define r_traders(x)	(race_info[x].traits & RT_TRADERS)	/* Trading skills */

/* Combat and Creature Management Tests */
#define r_monsterly(x)	(race_info[x].traits & RT_MONSTERSKILL)	/* Monster handling ability */
#define r_fighters(x)	(race_info[x].traits & RT_FIGHTERS)	/* Superior warrior capability */
#define r_antiwar(x)	(race_info[x].traits & RT_ANTIMILITARY)	/* Military aversion */

/* Game Mechanics and Availability Tests */
#define r_notrace(x)	(race_info[x].traits & RT_NOTRACE)	/* Non-player race flag */

/*
 * RACE_STRUCT - Comprehensive Race Data Structure
 *
 * Defines complete racial characteristics, capabilities, limitations, and
 * game balance parameters for each species in the game. This structure
 * contains all data necessary to implement racial differences in magic,
 * combat, economics, population dynamics, and strategic gameplay.
 *
 * The structure integrates with multiple game systems:
 * - Magic System: Power limits, costs, and starting abilities
 * - Population System: Growth rates and inter-racial interactions
 * - Economics: Nation attribute costs and effectiveness
 * - Combat: Racial bonuses and unit effectiveness modifiers
 * - Diplomacy: Inter-racial capture and population control mechanics
 *
 * Usage Context:
 * Populated in race data tables and accessed via race_info[] global array.
 * Used by nation creation, magic calculations, population growth, combat
 * resolution, and economic systems throughout the game engine.
 */
typedef struct s_race {
  /* Basic Race Identity and Presentation */
  char *name;			/* Display name of the race (e.g., "Elves", "Dwarves") */
  char *description;		/* Detailed description for player race selection interface */

  /* Magical System Integration */
  long pow_start[MAG_NUMBER];	/* Starting magical power levels for new nations of this race */
  long pow_limit[MAG_NUMBER];	/* Maximum magical power levels this race can achieve (racial caps) */
  long cost_base[MAG_NUMBER];	/* Base cost multipliers for magical power advancement */

  /* Inter-Racial Population Dynamics */
  long take_value[RACE_NUMBER];	/* Racial adjustments when capturing sectors (effectiveness modifiers) */
  long kill_pop[RACE_NUMBER];	/* Percentage of enemy population killed during conquest by race */
  long keep_pop[RACE_NUMBER];	/* Percentage of enemy population assimilated during conquest by race */

  /* Population and Growth Characteristics */
  int repro_limit;		/* Maximum reproduction potential (population growth rate cap) */

  /* Behavioral and Capability Traits */
  long traits;			/* Bitfield of racial characteristics (RT_* flags combined) */

  /* Nation Attribute System (AU_* attributes) */
  int au_maxval[AU_NUMBER];	/* Maximum attainable values for each nation attribute by race */
  int au_start[AU_NUMBER];	/* Starting values for nation attributes for new races */
  int au_costs[AU_NUMBER];	/* Cost in points to improve each attribute (racial efficiency) */
  int au_units[AU_NUMBER];	/* Number of units gained per point spent on each attribute */
} RACE_STRUCT, *RACE_PTR;

/*
 * Global Race Information Array - Species Database Interface
 *
 * External declaration for the global array containing complete race data
 * for all species in the game. This array is indexed by race constants
 * (0 to RACE_NUMBER-1) and provides access to all racial characteristics,
 * traits, and game balance parameters.
 *
 * Array Size: RACE_NUMBER elements (defined in dataX.h)
 * Index Range: 0 to RACE_NUMBER-1 (race type constants)
 * Content: Fully initialized RACE_STRUCT entries for each species
 *
 * Usage Examples:
 *   char *race_name = race_info[player_race].name;
 *   int magic_limit = race_info[nation_race].pow_limit[MAG_FIRE];
 *   bool can_trade = r_traders(current_race);
 *
 * Implementation Note:
 * This array is typically initialized in a separate data file (e.g., raceA.c)
 * with complete race definitions, trait assignments, and balance parameters
 * for all playable and non-playable species in the game world.
 */
extern RACE_STRUCT race_info[RACE_NUMBER];

/*
 * Modernization Analysis for racesX.h:
 *
 * Race System Enhancement Opportunities:
 * 1. Enum Type Safety: Convert race constants to enum for type checking
 * 2. Trait System: Consider enum flags or bitfield struct for traits
 * 3. String Management: Use const char* for read-only strings
 * 4. Null Safety: Add validation macros for race index bounds checking
 * 5. Magic Integration: Ensure MAG_NUMBER and AU_NUMBER are properly defined
 * 6. Documentation: Consider adding race balance documentation and examples
 * 7. Extensibility: Design system for mod support and custom races
 * 8. Performance: Consider cache-friendly data layout for hot paths
 * 9. Localization: Plan for multi-language race names and descriptions
 * 10. Validation: Add compile-time assertions for array size consistency
 */
