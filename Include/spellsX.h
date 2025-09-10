/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * spellsX.h - Spell System Definitions and Magic Framework
 *
 * This header defines the complete spell system for the Conquer strategy game,
 * providing spell data structures, magical effect types, casting requirements,
 * and comprehensive magic framework for implementing diverse magical abilities,
 * spell effects, and magical unit capabilities throughout the game world.
 *
 * The spell system enables strategic magical gameplay through diverse spell
 * categories, casting restrictions, power requirements, success rates, and
 * strength costs that balance magical abilities against resource consumption
 * and tactical considerations in combat, exploration, and empire management.
 *
 * Key Components:
 * - Spell Data Structure: Complete spell definition with costs, effects, descriptions
 * - Spell Type Enumeration: Categorized magical effects and spell classifications
 * - Caster Requirements: Spell information bits defining casting restrictions
 * - Testing Macros: Convenient access to spell casting capabilities and limitations
 * - Strength Limitations: Minimum requirements and safety thresholds for spell casting
 * - External Data Interface: Global spell database and configuration declarations
 *
 * Usage Context:
 * The spell system integrates with magical units, nation power levels, combat
 * mechanics, exploration systems, and strategic resource management to provide
 * balanced and engaging magical gameplay options while maintaining game balance
 * and preventing magical dominance over conventional strategic approaches.
 */

#ifndef SPL_STR_LIM

/*
 * SPELL_STRUCT - Comprehensive Spell Definition Data Structure
 *
 * Defines complete spell characteristics including presentation, mechanics,
 * costs, requirements, and effects for implementing magical abilities in
 * the game. This structure contains all data necessary for spell casting
 * validation, success calculation, resource consumption, and player feedback.
 *
 * The structure integrates with multiple game systems:
 * - User Interface: Name, prompt, and description presentation
 * - Magic System: Power requirements and spell classification
 * - Resource Management: Casting costs and strength consumption
 * - Game Balance: Success rates and caster restrictions
 * - Feedback System: Success/failure message presentation
 *
 * Usage Context:
 * Populated in spell data tables and accessed via spell_list[] global array.
 * Used by spell casting interface, magic validation, combat spell effects,
 * exploration spells, and strategic magical abilities throughout game engine.
 */
typedef struct s_spells {
  /* User Interface and Presentation */
  char *name;		/* Display name of the spell (e.g., "Fireball", "Teleport") */
  char *prompt;		/* Interactive prompt text for spell selection interface */
  char *descript;	/* Detailed description of spell effects and capabilities */
  char *good_str;	/* Success message displayed when spell casting succeeds */
  char *bad_str;	/* Failure message displayed when spell casting fails */
  int keypos;		/* Character position in prompt for keyboard spell selection */

  /* Spell Classification and Mechanics */
  char class;		/* Spell category/school classification (combat, utility, etc.) */
  long info;		/* Spell information bitfield (SI_* flags for casting restrictions) */

  /* Resource Requirements and Costs */
  int cost;		/* Mana/spell points required to attempt casting this spell */
  int drain;		/* Strength points consumed from caster when spell is cast */
  int rate;		/* Base percentage chance of successful spell casting (0-100) */

  /* Magical Power Prerequisites */
  long pow_need[MAG_NUMBER];	/* Required magical power levels in each school for casting */
} SPELL_STRUCT, *SPELL_PTR;

/*
 * Spelltype Enumeration - Magical Effect Categories
 *
 * Defines standardized spell type constants for categorizing magical effects
 * and abilities throughout the game. These enumerated values provide type-safe
 * spell identification and enable systematic organization of magical capabilities
 * by function, target, and strategic application.
 *
 * Spell Categories:
 * - Combat Magic: Direct damage and battlefield manipulation
 * - Movement Magic: Transportation and positioning abilities
 * - Healing Magic: Restoration and recovery effects
 * - Environmental Magic: Terrain and world manipulation
 * - Communication Magic: Information transfer and coordination
 * - Psychological Magic: Morale and fear effects
 * - Summoning Magic: Creature creation and control
 * - Transformation Magic: Object and resource manipulation
 * - Strategic Magic: Large-scale effects and resource management
 *
 * Implementation Note:
 * Spell data is typically stored in datamagX.c with complete spell definitions
 * referencing these type constants for classification and system integration.
 */
typedef enum spelltype {
  SPL_COMBAT,	/* Combat spells - direct damage, battlefield effects, offensive magic */
  SPL_FLIGHT,	/* Flight spells - aerial movement, transportation, positioning magic */
  SPL_HEAL,	/* Healing spells - restoration, recovery, regeneration magic */
  SPL_QUAKE,	/* Earthquake spells - terrain manipulation, environmental destruction */
  SPL_SENDING,	/* Sending spells - communication, message delivery, information magic */
  SPL_SCARE,	/* Scare spells - fear effects, morale manipulation, psychological magic */
  SPL_SUMMON,	/* Summon spells - creature creation, monster control, entity magic */
  SPL_TPORT,	/* Teleport spells - instant transportation, spatial manipulation magic */
  SPL_TRANSMUTE,/* Transmute spells - object transformation, resource conversion magic */
  SPL_TRANSFER	/* Transfer spells - resource movement, strategic repositioning magic */
} Spelltype;

/*
 * Spell Information Bits - Caster Requirement Flags
 *
 * Defines bitfield constants for specifying spell casting restrictions and
 * requirements. These flags are combined in the SPELL_STRUCT.info field to
 * control which units can attempt to cast specific spells, ensuring appropriate
 * magical balance and preventing inappropriate spell access.
 *
 * Caster Hierarchy (from least to most restrictive):
 * 1. SI_ANYCAST: Any unit with spell points (basic magic users)
 * 2. SI_SPELLCASTER: Dedicated magical units (trained wizards)
 * 3. SI_FULLCASTER: Master magicians (advanced spellcasters)
 *
 * Restriction Categories:
 * - Unit Type Restrictions: Based on magical training and capability level
 * - Monster Limitations: Preventing AI creatures from accessing certain spells
 * - Power Level Requirements: Ensuring appropriate spell access progression
 *
 * Usage Context:
 * These flags provide fine-grained control over spell accessibility, supporting
 * game balance by restricting powerful spells to appropriate caster types and
 * preventing magical dominance through unrestricted spell access.
 */

/* Caster Capability Requirements (progressive restriction levels) */
#define SI_ANYCAST	0x00000001L	/* Any unit possessing spell points may attempt casting */
#define SI_SPELLCASTER	0x00000002L	/* Requires dedicated spellcaster unit classification */
#define SI_FULLCASTER	0x00000004L	/* Requires master magician with full magical training */

/* Unit Type and Access Restrictions */
#define SI_NONMONSTER	0x00000008L	/* Monster units prohibited from casting this spell */

/*
 * Spell Information Testing Macros - Caster Requirement Queries
 *
 * Provides convenient boolean-style macros for testing spell casting restrictions
 * by combining spell index lookup with bitfield flag testing. These macros
 * access the global spell_list[] array and test specific requirement flags.
 *
 * Parameter: x - Spell index (0 to spell_number-1)
 * Returns: Non-zero if spell has the specified requirement, zero otherwise
 *
 * Usage Examples:
 *   if (si_needcaster(spell_id)) { verify_unit_is_spellcaster }
 *   if (si_needfull(spell_id)) { verify_unit_is_full_magician }
 *   if (si_nonmonster(spell_id) && is_monster(unit)) { deny_casting }
 *
 * Note: These macros provide read-only access to spell requirements and should
 * be used for casting validation. Spell modification should be done through
 * appropriate spell management functions and data table updates.
 */

/* Caster Capability Requirement Tests */
#define si_anycaster(x)	(spell_list[x].info & SI_ANYCAST)	/* Basic spell point requirement */
#define si_needcaster(x)	(spell_list[x].info & SI_SPELLCASTER)	/* Spellcaster unit required */
#define si_needfull(x)	(spell_list[x].info & SI_FULLCASTER)	/* Full magician required */

/* Unit Type and Access Restriction Tests */
#define si_nonmonster(x)	(spell_list[x].info & SI_NONMONSTER)	/* Monster casting prohibited */

/*
 * Spell Strength Limitations - Caster Safety and Balance Constants
 *
 * Defines critical strength thresholds for spell casting to ensure game balance,
 * prevent spell abuse, and maintain caster safety through minimum strength
 * requirements and post-casting protection limits.
 *
 * Strength Management System:
 * - Pre-casting validation ensures casters have sufficient strength
 * - Post-casting protection prevents casters from reaching dangerous weakness
 * - Balance mechanism prevents unlimited spell casting through strength depletion
 *
 * Game Balance Impact:
 * These constants create strategic resource management where magical power
 * must be balanced against caster safety and operational capability, preventing
 * magical dominance while maintaining tactical spell usage viability.
 */

/* Spell Casting Strength Requirements and Safety Limits */
#define SPL_STR_LIM	50	/* Minimum strength required to attempt spell casting */
#define SPL_STR_MIN	20	/* Minimum strength caster must retain after spell casting */

/*
 * Global Spell Database Interface - Magic System Data Access
 *
 * External declarations for the global spell database providing complete
 * access to all spell definitions, characteristics, and magical system
 * configuration throughout the game engine.
 *
 * Data Structure Access:
 * - spell_list[]: Array of SPELL_STRUCT containing complete spell data
 * - spell_number: Total count of spells available in the magic system
 *
 * Array Organization:
 * - Index Range: 0 to spell_number-1 (spell identifier constants)
 * - Content: Fully initialized SPELL_STRUCT entries for each magical ability
 * - Access Pattern: Direct indexing via spell type constants and IDs
 *
 * Usage Examples:
 *   char *spell_name = spell_list[spell_id].name;
 *   int casting_cost = spell_list[SPL_FIREBALL].cost;
 *   bool can_cast = si_needcaster(spell_id) && is_spellcaster(unit);
 *
 * Implementation Note:
 * This database is typically initialized in datamagX.c with complete spell
 * definitions including costs, requirements, effects, and textual descriptions
 * for all magical abilities available throughout the game world.
 */
extern SPELL_STRUCT spell_list[];	/* Global array of complete spell definitions */
extern int spell_number;		/* Total number of spells in magic system */

/*
 * Modernization Analysis for spellsX.h:
 *
 * Magic System Enhancement Opportunities:
 * 1. Type Safety: Convert spell constants to enum for compile-time checking
 * 2. String Management: Use const char* for read-only spell text fields
 * 3. Validation: Add bounds checking macros for spell index validation
 * 4. Memory Safety: Consider string length limits and buffer overflow protection
 * 5. Extensibility: Design framework for custom spells and mod support
 * 6. Performance: Consider spell caching and lookup optimization for hot paths
 * 7. Localization: Plan for multi-language spell names and descriptions
 * 8. Magic Schools: Enhance MAG_NUMBER integration with school-specific requirements
 * 9. Effect System: Consider separating spell effects from spell definitions
 * 10. Balance Framework: Add configuration system for dynamic spell balancing
 */

#endif /* SPL_STR_LIM */
