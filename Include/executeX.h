/*
 * executeX.h - Command Execution System Definitions
 *
 * This header defines the core command execution type system for the Conquer game
 * engine. It provides a comprehensive enumeration of all possible entity adjustment
 * operations that can be performed during game execution, including modifications
 * to armies, navies, caravans, cities, items, nations, sectors, magic, and 
 * resource distribution.
 *
 * The execution system serves as the foundation for the game's command processing
 * architecture, allowing systematic tracking and application of state changes
 * across all game entities. Each execution type represents a specific modification
 * operation that can be applied to game objects during turn processing, combat
 * resolution, economic updates, or player commands.
 *
 * Key Components:
 *   - Exectype enum: Complete enumeration of all possible entity modifications
 *   - Army Operations: Location, type, status, movement, and attribute changes
 *   - Naval Operations: Ship modifications, crew management, cargo handling
 *   - Caravan Operations: Trade route management and cargo operations
 *   - City Operations: Population, materials, fortification, and infrastructure
 *   - Item Operations: Equipment management, creation, and distribution
 *   - National Operations: Diplomatic, administrative, and strategic changes
 *   - Sector Operations: Terrain, resources, and territorial modifications
 *   - Magic Operations: Spell casting, magical effects, and enchantments
 *   - Resource Operations: Material distribution and economic transactions
 *
 * Integration Notes:
 *   - Used by: executeX.c (command execution engine), game state processors
 *   - Depends on: header.h (basic definitions), dataX.h (MTRLS_NUMBER constant)
 *   - Provides: Standardized command type enumeration for all game operations
 *
 * Modernization Considerations:
 *   - Enum values use legacy spacing patterns that could be regularized
 *   - Some value ranges (20, 30, 50, etc.) create gaps for future expansion
 *   - Consider using named constants for range boundaries to improve clarity
 *   - Thread safety: Read-only enum values are inherently thread-safe
 *   - The MTRLS_NUMBER dependency creates tight coupling with materials system
 */

/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

#ifndef __executeX_h__
#define __executeX_h__

/*
 * enum exectype - Game Entity Modification Operation Types
 *
 * Comprehensive enumeration defining all possible modification operations that
 * can be performed on game entities during command execution. This enum serves
 * as the foundation for the game's command processing system, providing a
 * standardized way to represent and track all state changes across different
 * entity types.
 *
 * The enumeration is organized into logical groups with reserved value ranges
 * to allow for future expansion while maintaining backward compatibility. Each
 * group handles a specific category of game entities and their associated
 * modification operations.
 *
 * Value Ranges:
 *   0-19: Army operations (individual unit management)
 *   20-29: Army group operations (formation management)
 *   30-49: Naval operations (fleet and ship management)
 *   50-69: Caravan operations (trade route management)
 *   70-89: City operations (settlement management)
 *   90-109: Item operations (equipment and artifact management)
 *   110-149: National operations (diplomatic and administrative)
 *   150-159: Sector operations (territorial management)
 *   160-180: Magic operations (spell and enchantment system)
 *   181+: Resource distribution (economic transactions)
 *
 * Usage Patterns:
 *   - Command processors use these types to identify operation categories
 *   - State change systems apply modifications based on execution type
 *   - Validation systems check operation legality using type information
 *   - Logging and debugging systems categorize operations by type
 *
 * Design Notes:
 *   - Gaps in numbering allow for future expansion without breaking compatibility
 *   - Resource operations use dynamic ranges based on MTRLS_NUMBER constant
 *   - Each operation type maps to specific data structure fields and validation rules
 */
typedef enum exectype {

  /* ============================================================================
   * ARMY OPERATIONS (Range: 0-19)
   * ============================================================================
   * Purpose: Individual army unit management and modification operations
   * Usage: Applied to single army entities for tactical and strategic changes
   * Notes: Covers all aspects of army state from location to combat effectiveness
   */

  EX_ARMYLOC,      /* Army location - Change army's current map position */
  EX_ARMYTYPE,     /* Army type - Modify army's unit composition or classification */
  EX_ARMYSTAT,     /* Army status - Update army's operational state (active, reserve, etc.) */
  EX_ARMYMOVE,     /* Army movement - Process army movement orders and pathfinding */
  EX_ARMYLEAD,     /* Army leader - Assign or change army's commanding officer */
  EX_ARMYSIZE,     /* Army size - Modify army's total unit count or strength */
  EX_ARMYID,       /* Army ID - Change army's unique identifier (rare operation) */
  EX_ARMYSPLY,     /* Army supply - Update army's supply level and logistics state */
  EX_ARMYSPTS,     /* Army spell points - Modify army's magical energy reserves */
  EX_ARMYEFF,      /* Army efficiency - Change army's current combat effectiveness */
  EX_ARMYMAXEFF,   /* Army max efficiency - Set army's maximum potential effectiveness */
  EX_ARMYCREATE,   /* Army creation - Initialize new army entity with default values */
  EX_ARMYDESTROY,  /* Army destruction - Remove army entity from game state */
  EX_ARMYOLOC,     /* Army old location - Track army's previous position for validation */
  EX_ARMYMERCS,    /* Army mercenaries - Manage mercenary units within army */

  /* ============================================================================
   * ARMY GROUP OPERATIONS (Range: 20-29)
   * ============================================================================
   * Purpose: Formation and group-level army management operations
   * Usage: Applied to army formations, coordinated movements, and group tactics
   * Notes: Handles multiple armies acting as coordinated units
   */

  EX_GRPLOC = 20,  /* Group location - Manage formation positioning and coordination */
  EX_GRPMOVE,      /* Group movement - Process coordinated movement of army formations */

  /* ============================================================================
   * NAVAL OPERATIONS (Range: 30-49)
   * ============================================================================
   * Purpose: Fleet, ship, and naval unit management operations
   * Usage: Applied to naval entities for maritime operations and fleet management
   * Notes: Covers ships, crews, cargo, and naval combat systems
   */

  EX_NAVYLOC = 30, /* Navy location - Change fleet's current maritime position */
  EX_NAVYSHIP,     /* Navy ships - Modify fleet's ship composition and types */
  EX_NAVYSTAT,     /* Navy status - Update fleet's operational state and readiness */
  EX_NAVYMOVE,     /* Navy movement - Process fleet movement and navigation orders */
  EX_NAVYID,       /* Navy ID - Change fleet's unique identifier (administrative) */
  EX_NAVYCREW,     /* Navy crew - Manage crew size, experience, and assignments */
  EX_NAVYPEOP,     /* Navy people - Handle civilian passengers and personnel */
  EX_NAVYMTRLS,    /* Navy materials - Manage ship construction and repair materials */
  EX_NAVYSPLY,     /* Navy supply - Update fleet's provisions and logistics */
  EX_NAVYARMY,     /* Navy army - Manage embarked land forces and marines */
  EX_NAVYEFF,      /* Navy efficiency - Change fleet's current operational effectiveness */
  EX_NAVYCREATE,   /* Navy creation - Initialize new fleet entity with default state */
  EX_NAVYDESTROY,  /* Navy destruction - Remove fleet entity from game state */
  EX_NAVYOLOC,     /* Navy old location - Track fleet's previous position for validation */
  EX_NAVYCVN,      /* Navy caravan - Handle naval transport of trade caravans */

  /* ============================================================================
   * CARAVAN OPERATIONS (Range: 50-69)
   * ============================================================================
   * Purpose: Trade caravan and commercial transport management operations
   * Usage: Applied to trade routes, merchant expeditions, and economic transport
   * Notes: Handles trade goods, routes, protection, and commercial activities
   */

  EX_CVNLOC = 50,  /* Caravan location - Change caravan's current position on trade routes */
  EX_CVNSIZE,      /* Caravan size - Modify caravan's cargo capacity and scale */
  EX_CVNSTAT,      /* Caravan status - Update caravan's operational state and condition */
  EX_CVNMOVE,      /* Caravan movement - Process caravan movement along trade routes */
  EX_CVNID,        /* Caravan ID - Change caravan's unique identifier (administrative) */
  EX_CVNCREW,      /* Caravan crew - Manage caravan personnel, guards, and traders */
  EX_CVNSPLY,      /* Caravan supply - Update caravan's provisions and maintenance needs */
  EX_CVNPEOP,      /* Caravan people - Handle passengers and additional personnel */
  EX_CVNEFF,       /* Caravan efficiency - Change caravan's operational effectiveness */
  EX_CVNMTRLS,     /* Caravan materials - Manage trade goods and cargo inventory */
  EX_CVNCREATE,    /* Caravan creation - Initialize new caravan entity with defaults */
  EX_CVNDESTROY,   /* Caravan destruction - Remove caravan entity from game state */
  EX_CVNOLOC,      /* Caravan old location - Track caravan's previous position */

  /* ============================================================================
   * CITY OPERATIONS (Range: 70-89)
   * ============================================================================
   * Purpose: Settlement, city, and supply center management operations
   * Usage: Applied to urban centers for infrastructure, population, and production
   * Notes: Handles cities, towns, fortifications, and economic centers
   */

  EX_CITYLOC = 70, /* City location - Change city's map position (rare operation) */
  EX_CITYNAME,     /* City name - Modify city's display name and identification */
  EX_CITYPEOP,     /* City people - Manage city's population size and demographics */
  EX_CITYMTRLS,    /* City materials - Update city's production materials and resources */
  EX_CITYIMTRLS,   /* City imported materials - Handle trade imports and acquisitions */
  EX_CITYFORT,     /* City fortification - Modify city's defensive structures and walls */
  EX_CITYSTALONS,  /* City stalons - Manage city's elite guard units or special forces */
  EX_CITYCREATE,   /* City creation - Initialize new city entity with default infrastructure */
  EX_CITYDESTROY,  /* City destruction - Remove city entity and handle consequences */
  EX_CITYWEIGHT,   /* City weight - Manage city's strategic importance and influence */

  /* ============================================================================
   * ITEM OPERATIONS (Range: 90-109)
   * ============================================================================
   * Purpose: Equipment, artifact, and item management operations
   * Usage: Applied to weapons, armor, magical items, and equipment systems
   * Notes: Handles item creation, distribution, assignment, and special properties
   */

  EX_ITEMLOC = 90, /* Item location - Change item's current position or container */
  EX_ITEMID,       /* Item ID - Modify item's unique identifier and classification */
  EX_ITEMMTRLS,    /* Item materials - Handle item's material composition and requirements */
  EX_ITEMCREATE,   /* Item creation - Initialize new item entity with base properties */
  EX_ITEMDESTROY,  /* Item destruction - Remove item entity and handle dependencies */
  EX_ITEMTYPE,     /* Item type - Change item's classification and category */
  EX_ITEMINFO,     /* Item info - Update item's descriptive information and properties */
  EX_ITEMMEN,      /* Item men - Handle item's effect on or assignment to personnel */
  EX_ITEMARMY,     /* Item army - Manage item's assignment to or effect on armies */
  EX_ITEMNAVY,     /* Item navy - Handle item's assignment to or effect on naval units */
  EX_ITEMCVN,      /* Item caravan - Manage item's transport via or effect on caravans */

  /* ============================================================================
   * NATIONAL OPERATIONS (Range: 110-149)
   * ============================================================================
   * Purpose: Nation-level administration, diplomacy, and strategic management
   * Usage: Applied to countries, players, and high-level political entities
   * Notes: Handles national identity, diplomacy, leadership, and strategic resources
   */

  EX_NTNNAME = 110, /* Nation name - Change nation's official name and identity */
  EX_NTNLOGIN,      /* Nation login - Modify nation's player access credentials */
  EX_NTNPASSWD,     /* Nation password - Update nation's authentication information */
  EX_NTNLEADER,     /* Nation leader - Change nation's ruling authority or government */
  EX_NTNLOC,        /* Nation location - Modify nation's capital or primary territory */
  EX_NTNAPLUS,      /* Nation attack plus - Adjust nation's offensive military bonuses */
  EX_NTNDPLUS,      /* Nation defense plus - Adjust nation's defensive military bonuses */
  EX_NTNACTIVE,     /* Nation active - Change nation's participation status in game */
  EX_NTNDIPLO,      /* Nation diplomacy - Update nation's diplomatic relations and treaties */
  EX_NTNRLOC,       /* Nation ruler location - Track nation's leadership position */
  EX_NTNREPRO,      /* Nation reproduction - Manage nation's population growth factors */
  EX_NTNRACE,       /* Nation race - Modify nation's racial characteristics and bonuses */
  EX_NTNMARK,       /* Nation mark - Update nation's special designation or status */
  EX_NTNLEDGE,      /* Nation left edge - Manage nation's western territorial boundary */
  EX_NTNREDGE,      /* Nation right edge - Manage nation's eastern territorial boundary */
  EX_NTNTEDGE,      /* Nation top edge - Manage nation's northern territorial boundary */
  EX_NTNBEDGE,      /* Nation bottom edge - Manage nation's southern territorial boundary */
  EX_NTNCLASS,      /* Nation class - Modify nation's technological or social advancement */
  EX_NTNSCORE,      /* Nation score - Update nation's victory points and achievements */
  EX_NTNMOVE,       /* Nation move - Handle nation-level movement and expansion operations */
  EX_NTNSPTS,       /* Nation spell points - Manage nation's magical energy reserves */
  EX_NTNBUTE,       /* Nation attribute - Modify nation's special characteristics */
  EX_BRIBE,         /* Bribe operation - Handle corruption and influence transactions */
  EX_NEWSSIZE,      /* News size - Manage nation's news and information distribution */
  EX_MAILSIZE,      /* Mail size - Handle nation's communication system capacity */
  EX_NTNRENUM,      /* Nation renumber - Change nation's numerical identifier */
  EX_UNUMSLOT,      /* Unit number slot - Manage unit identification system slots */
  EX_UNUMCOPY,      /* Unit number copy - Handle unit identification duplication */
  EX_UNUMRESET,     /* Unit number reset - Reset unit identification system */
  EX_UNUMDEFAULT,   /* Unit number default - Set default unit identification values */

  /* ============================================================================
   * SECTOR OPERATIONS (Range: 150-159)
   * ============================================================================
   * Purpose: Territorial sector and terrain management operations
   * Usage: Applied to map sectors for terrain, ownership, and resource management
   * Notes: Handles individual map hexes and their properties
   */

  EX_SCTDESG = 150, /* Sector designation - Change sector's land use and development type */
  EX_SCTPEOPLE,     /* Sector people - Modify sector's population and inhabitants */
  EX_SCTOWN,        /* Sector ownership - Change sector's political control and ownership */
  EX_SCTALT,        /* Sector altitude - Modify sector's elevation and topographic features */
  EX_SCTVEG,        /* Sector vegetation - Change sector's natural vegetation and biome */
  EX_SCTTGOOD,      /* Sector trade good - Modify sector's natural resource production */
  EX_SCTMNRLS,      /* Sector minerals - Update sector's mineral deposits and resources */

  /* ============================================================================
   * MAGIC OPERATIONS (Range: 160-180)
   * ============================================================================
   * Purpose: Magical system operations including spells and enchantments
   * Usage: Applied to magical effects, spell casting, and supernatural phenomena
   * Notes: Handles the game's magic system and supernatural elements
   */

  EX_MGK_ADJ = 160, /* Magic adjustment - General magical effect modification operation */
  EX_MGKSPELL,      /* Magic spell - Cast spell or apply magical effect to targets */
  EX_MGKSENDING,    /* Magic sending - Transmit magical communications or divinations */

  /* ============================================================================
   * RESOURCE DISTRIBUTION OPERATIONS (Range: 181+)
   * ============================================================================
   * Purpose: Economic resource transfer and material distribution operations
   * Usage: Applied to material transactions, trade, and resource redistribution
   * Notes: Uses dynamic ranges based on MTRLS_NUMBER for different material types
   *
   * Range Calculation:
   *   - TAKESTART to TAKEFINISH: Resource extraction operations (181 to 182+MTRLS_NUMBER)
   *   - GIVESTART to GIVEFINISH: Resource distribution operations (191 to 192+MTRLS_NUMBER)
   *   - Each material type gets its own specific operation code within the range
   */

  EX_TAKESTART = 181,                           /* Begin resource extraction operation range */
  EX_TAKEMTRLS,                                 /* Take materials - Extract resources from sources */
  EX_TAKEFINISH = (182 + MTRLS_NUMBER),         /* End resource extraction operation range */

  EX_GIVESTART = 191,                           /* Begin resource distribution operation range */
  EX_GIVEMTRLS,                                 /* Give materials - Distribute resources to targets */
  EX_GIVEFINISH = (192 + MTRLS_NUMBER)          /* End resource distribution operation range */

} Exectype;

#endif /* __executeX_h__ */
