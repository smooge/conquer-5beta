/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * olddataX.h - Legacy Data Structure Definitions for Version Compatibility
 *
 * This header provides critical backward compatibility infrastructure for the
 * Conquer game system, preserving data structure definitions from historical
 * versions (patches 24-28) to support game data migration and file format
 * conversion. These structures ensure that saved games from older versions
 * can be loaded and upgraded to current data formats.
 *
 * Key Components:
 *   - Patch 28 world/city structures (s27_world, s27_city)
 *   - Patch 26-27 compatibility structures (s26_world, s25_nation)
 *   - Patch 24-25 legacy definitions (s24_nation, s24_world)
 *   - Historical constant definitions for status values and attributes
 *   - Version-specific data layout preservation for safe migration
 *
 * Integration Notes:
 *   - Used by: Data conversion routines, save/load compatibility functions
 *   - Depends on: header.h (for current type definitions and constants)
 *   - Provides: Legacy structure layouts for version upgrade paths
 *
 * Modernization Considerations:
 *   - Preserve exact memory layouts for binary compatibility
 *   - Document migration paths to current data structures
 *   - Consider deprecation timeline after full modernization
 *   - Validate against historical save file formats during testing
 *
 * Version Evolution Summary:
 *   - Patch 24: Basic nation/world structures with limited attributes
 *   - Patch 25: Added mapping information and expanded nation data
 *   - Patch 26: Enhanced navy/caravan status system and unit types
 *   - Patch 27: Added diplomacy adjustment capabilities (maxdipadj)
 *   - Patch 28: Introduced "late login" flags and growth rate mechanics
 */

/* ============================================================================
 * PATCH 28 COMPATIBILITY STRUCTURES (Latest Legacy Version)
 * ============================================================================
 * Purpose: Support for the most recent legacy format before current version
 * Features: Late login flags, growth rate mechanics, expanded city structures
 * Migration: Direct upgrade path to current data structures
 */

/*
 * struct s27_world - Patch 27 World Data Structure (Pre-Patch 28)
 *
 * Represents the complete game world state from patch 27, including all
 * global configuration parameters, nation management, and game mechanics
 * settings. This structure preserves the exact layout needed for loading
 * saved games from patch 27 installations.
 *
 * Usage Patterns:
 *   - Loaded during save game compatibility processing
 *   - Converted to current world structure format during game initialization
 *   - Validated for data integrity before migration
 *
 * Migration Notes:
 *   - Missing maxdipadj field compared to patch 28 (added in s26_world)
 *   - Direct field-by-field copy possible for most members
 *   - Special handling needed for new diplomacy features
 *
 * Memory Layout:
 *   - Exact binary compatibility required for save file loading
 *   - Field order and padding must match historical implementation
 *   - Total size affects save file validation checksums
 */
struct s27_world
{
  /* World Geometry and Nation Management */
  maptype mapx, mapy;	/* World dimensions in map coordinates */
  ntntype nations;	/* Maximum nations allowed in game */
  ntntype npcs;		/* Count of computer-controlled nations */
  ntntype active_ntns;	/* Count of currently active nations */
  NTN_PTR np[ABSMAXNTN];	/* Array of nation pointers for all players */
  
  /* Game Turn and Timeline Control */
  short turn;		/* Current game turn counter */
  short start_turn;	/* Turn when campaign was initialized */
  
  /* Administrative Access and Security */
  char demigod[NAMELTH+1];	/* Game master character name */
  char passwd[PASSLTH+1];	/* Game master authentication password */
  char addpwd[PASSLTH+1];	/* Password for new nation registration */
  
  /* Game Master Privilege Controls */
  char demibuild;	/* Allow game master to rebuild nations */
  char demilimit;	/* Restrict game master intervention powers */
  
  /* Player Interface and Display Options */
  char relative_map;	/* Enable relative coordinate display confusion */
  char hexmap;		/* Use hexagonal map display mode */
  char verify_login;	/* Require additional login verification */
  char hide_scores;	/* Conceal nation scores from players */
  char hide_login;	/* Hide nation owner login names */
  
  /* Nation Placement and Combat Configuration */
  char builddist;	/* Minimum distance between starting nations */
  short fleet_cbval;	/* Base combat value bonus for naval units */
  short wagon_cbval;	/* Base combat value bonus for caravan units */
  
  /* Combat Mechanics and Damage System */
  char num_dice;	/* Number of dice rolled for combat resolution */
  char avg_damage;	/* Average damage percentage per combat round */
  char damage_limit;	/* Damage threshold to terminate combat */
  char overmatch_adj;	/* Damage adjustment for relative unit sizes */
  char pmindamage;	/* Minimum damage as percentage of roll */
  
  /* World Generation Parameters */
  char smoothings;	/* Terrain smoothing algorithm iterations */
  char land_range;	/* Water displacement check radius */
  char preject_land;	/* Land generation rejection percentage */
  char pwater;		/* Water coverage percentage of world */
  char pmount;		/* Mountain coverage percentage of land */
  
  /* Economic and Military Balance */
  char pmercval;	/* Percentage of disbanded units becoming mercenaries */
  char prevolt;		/* Annual revolt probability percentage */
  char pscout;		/* Scout capture probability percentage */
  char pmonster;	/* Annual monster reproduction rate percentage */
  char pmercmonst;	/* Percentage of monsters joining mercenary pool */
  
  /* Trade and Resource Distribution */
  char ptrade;		/* Percentage of sectors containing trade goods */
  char ptgmetal;	/* Metal trade good distribution percentage */
  char ptgjewel;	/* Jewel trade good distribution percentage */
  char ptgspell;	/* Magic trade good distribution percentage */
  
  /* Logistics and Supply Chain */
  char supplylimit;	/* Maximum supply capacity per unit */
  char nvsplydist;	/* Supply range for naval and overland units */
  char cityxfdist;	/* Maximum transfer distance for cities */
  
  /* Game Balance and Scaling */
  uns_char exposelvl;	/* Exposure level scaling factor (10=normal) */
  uns_char maxpts;	/* Maximum build points per nation per turn */
  
  /* NPC Nation Generation */
  uns_char lizards;	/* Starting count of lizard NPC nations */
  uns_char savages;	/* Starting count of savage NPC nations */
  uns_char nomads;	/* Starting count of nomad NPC nations */
  uns_char pirates;	/* Starting count of pirate NPC nations */
  
  /* Random Event Generation */
  char numrandom;	/* Random events generated per turn */
  char numweather;	/* Weather events generated per turn */
  
  /* Mercenary Market Configuration */
  long m_mil;		/* Total mercenary units available globally */
  short m_aplus;	/* Attack bonus for mercenary units */
  short m_dplus;	/* Defense bonus for mercenary units */
  itemtype bribelevel;	/* Base value for diplomatic bribery */
  
  /* Global Resource Tracking */
  itemtype w_mtrls[MTRLS_NUMBER];	/* Total material resources in world */
  long w_civ;		/* Total civilian population across all nations */
  long w_mil;		/* Total military units across all nations */
  long w_sctrs;		/* Total owned sectors across all nations */
  long w_score;		/* Combined score of all nations */
  
  /* Communication System State */
  off_t mbox_size;	/* Game master mailbox size at last read */
  off_t news_size;	/* Global news file size at last update */
  
  /* Unit Numbering System */
  int num_unum;		/* Count of default unit numbering schemes */
  UNUM_PTR dflt_unum;	/* Default unit autonumbering configuration */
  
  /* Diplomacy System (Added in Patch 27) */
  uns_char maxdipadj;	/* Maximum diplomacy relationship adjustment */
};

/*
 * struct s27_city - Patch 27 City Data Structure
 *
 * Represents individual city settlement data from patch 27, including location,
 * resources, population, and defensive capabilities. Cities serve as economic
 * and strategic centers for nations, providing resource production, population
 * growth, and military fortification points.
 *
 * Usage Patterns:
 *   - Linked list node in nation's city_list
 *   - Loaded/saved during game persistence operations
 *   - Migrated to current city structure during version upgrades
 *
 * Migration Notes:
 *   - Direct field mapping to current city structure
 *   - Resource array sizes must match current MTRLS_NUMBER
 *   - Linked list structure preserved in current format
 *
 * Economic Role:
 *   - Resource production based on efficiency and materials
 *   - Population growth center for civilian expansion
 *   - Trade good distribution and storage point
 *   - Economic weight affects resource distribution algorithms
 */
typedef struct s27_city {
  /* Geographic Position */
  maptype xloc, yloc;	/* Map coordinates of city location */
  
  /* City Identity and Population */
  char name[NAMELTH+1];	/* Human-readable city name */
  short i_people;	/* Initial civilian population when founded */
  
  /* Economic and Production Factors */
  uns_char efficiency;	/* Production efficiency percentage (0-100) */
  uns_char weight;	/* Economic importance for resource distribution */
  
  /* Resource Storage and Management */
  itemtype c_mtrls[MTRLS_NUMBER];	/* Current material stockpiles */
  itemtype i_mtrls[MTRLS_NUMBER];	/* Initial material distributions */
  itemtype s_talons;	/* Starting monetary reserves (game currency) */
  
  /* Military and Defense */
  uns_char fortress;	/* Defensive fortification level (0-255) */
  
  /* Data Structure Management */
  struct s27_city *next;	/* Linked list pointer to next city */
} C27_STRUCT, *C27_PTR;

/* ============================================================================
 * PATCH 26-27 COMPATIBILITY STRUCTURES
 * ============================================================================
 * Purpose: Support for patch 26 save files (missing maxdipadj field)
 * Features: Pre-diplomacy enhancement versions
 * Migration: Add default maxdipadj value during conversion
 */

/*
 * struct s26_world - Patch 26 World Data Structure (Pre-Diplomacy Enhancement)
 *
 * Represents the complete game world state from patch 26, identical to patch 27
 * except for the missing maxdipadj field. This structure enables loading of
 * patch 26 save files and converting them to later formats with appropriate
 * default values for new diplomacy features.
 *
 * Key Differences from s27_world:
 *   - Missing maxdipadj field (diplomacy adjustment limit)
 *   - Otherwise identical field layout and functionality
 *   - Requires default diplomacy settings during migration
 *
 * Migration Strategy:
 *   - Copy all existing fields to s27_world structure
 *   - Initialize maxdipadj to reasonable default value
 *   - Validate all percentage and limit fields
 */
struct s26_world
{
  /* 
   * NOTE: Field layout identical to s27_world except missing:
   * - uns_char maxdipadj (diplomacy adjustment limit)
   * All other fields have same meaning and usage as documented in s27_world.
   */
  
  maptype mapx, mapy;	/* World dimensions in map coordinates */
  ntntype nations;	/* Maximum nations allowed in game */
  ntntype npcs;		/* Count of computer-controlled nations */
  ntntype active_ntns;	/* Count of currently active nations */
  NTN_PTR np[ABSMAXNTN];	/* Array of nation pointers for all players */
  short turn;		/* Current game turn counter */
  short start_turn;	/* Turn when campaign was initialized */
  char demigod[NAMELTH+1];	/* Game master character name */
  char passwd[PASSLTH+1];	/* Game master authentication password */
  char addpwd[PASSLTH+1];	/* Password for new nation registration */
  char demibuild;	/* Allow game master to rebuild nations */
  char demilimit;	/* Restrict game master intervention powers */
  char relative_map;	/* Enable relative coordinate display confusion */
  char hexmap;		/* Use hexagonal map display mode */
  char verify_login;	/* Require additional login verification */
  char hide_scores;	/* Conceal nation scores from players */
  char hide_login;	/* Hide nation owner login names */
  char builddist;	/* Minimum distance between starting nations */
  short fleet_cbval;	/* Base combat value bonus for naval units */
  short wagon_cbval;	/* Base combat value bonus for caravan units */
  char num_dice;	/* Number of dice rolled for combat resolution */
  char avg_damage;	/* Average damage percentage per combat round */
  char damage_limit;	/* Damage threshold to terminate combat */
  char overmatch_adj;	/* Damage adjustment for relative unit sizes */
  char pmindamage;	/* Minimum damage as percentage of roll */
  char smoothings;	/* Terrain smoothing algorithm iterations */
  char land_range;	/* Water displacement check radius */
  char preject_land;	/* Land generation rejection percentage */
  char pwater;		/* Water coverage percentage of world */
  char pmount;		/* Mountain coverage percentage of land */
  char pmercval;	/* Percentage of disbanded units becoming mercenaries */
  char prevolt;		/* Annual revolt probability percentage */
  char pscout;		/* Scout capture probability percentage */
  char pmonster;	/* Annual monster reproduction rate percentage */
  char pmercmonst;	/* Percentage of monsters joining mercenary pool */
  char ptrade;		/* Percentage of sectors containing trade goods */
  char ptgmetal;	/* Metal trade good distribution percentage */
  char ptgjewel;	/* Jewel trade good distribution percentage */
  char ptgspell;	/* Magic trade good distribution percentage */
  char supplylimit;	/* Maximum supply capacity per unit */
  char nvsplydist;	/* Supply range for naval and overland units */
  char cityxfdist;	/* Maximum transfer distance for cities */
  uns_char exposelvl;	/* Exposure level scaling factor (10=normal) */
  uns_char maxpts;	/* Maximum build points per nation per turn */
  uns_char lizards;	/* Starting count of lizard NPC nations */
  uns_char savages;	/* Starting count of savage NPC nations */
  uns_char nomads;	/* Starting count of nomad NPC nations */
  uns_char pirates;	/* Starting count of pirate NPC nations */
  char numrandom;	/* Random events generated per turn */
  char numweather;	/* Weather events generated per turn */
  long m_mil;		/* Total mercenary units available globally */
  short m_aplus;	/* Attack bonus for mercenary units */
  short m_dplus;	/* Defense bonus for mercenary units */
  itemtype bribelevel;	/* Base value for diplomatic bribery */
  itemtype w_mtrls[MTRLS_NUMBER];	/* Total material resources in world */
  long w_civ;		/* Total civilian population across all nations */
  long w_mil;		/* Total military units across all nations */
  long w_sctrs;		/* Total owned sectors across all nations */
  long w_score;		/* Combined score of all nations */
  off_t mbox_size;	/* Game master mailbox size at last read */
  off_t news_size;	/* Global news file size at last update */
  int num_unum;		/* Count of default unit numbering schemes */
  UNUM_PTR dflt_unum;	/* Default unit autonumbering configuration */
  /* Missing from patch 27: uns_char maxdipadj - added in patch 27 */
};

/* ============================================================================
 * PATCH 25-26 COMPATIBILITY STRUCTURES AND CONSTANTS
 * ============================================================================
 * Purpose: Support for pre-patch 26 naval/caravan status systems
 * Features: Legacy unit status codes and nation structure without mapping info
 * Migration: Convert old status codes to new system values
 */

/*
 * Legacy Naval and Caravan Status Definitions (Pre-Patch 26)
 *
 * These constants define the operational status codes used for naval units
 * and caravans in versions prior to patch 26. The status system was
 * reorganized in patch 26 to add new unit types and capabilities.
 *
 * Migration Strategy:
 *   - Map old status codes to corresponding new status values
 *   - Validate status transitions during data conversion
 *   - Preserve essential operational states during upgrade
 *
 * Usage in Legacy Code:
 *   - Set in unit->status field for naval units and caravans
 *   - Controls unit behavior and display in interface
 *   - Affects movement restrictions and combat participation
 */
#define OST_CARRY       0	/* Unit carrying cargo or passengers */
#define OST_DECOY       1	/* Unit acting as combat decoy */
#define OST_SUPPLY      2	/* Unit providing supply support */
#define OST_ENGAGE      3	/* Unit actively engaged in combat */
#define OST_REPAIR      4	/* Unit undergoing repairs */
#define OST_SIEGED      5	/* Unit trapped in siege conditions */
#define OST_FORSALE     6	/* Unit offered for sale/trade */
#define OST_ONBOARD     7	/* Unit boarded on transport vessel */
#define OST_ONBSPLY     8	/* Unit boarded for supply operations */
#define OST_WORKCREW    9	/* Unit performing work/construction */

/*
 * struct s25_nation - Patch 25 Nation Data Structure (Pre-Mapping Information)
 *
 * Represents individual nation state from patch 25, containing all player and
 * NPC nation data except for the mapping information features added in patch 26.
 * This structure preserves the exact layout needed for loading nations from
 * patch 25 save files.
 *
 * Key Differences from Current Nation Structure:
 *   - Missing mapping information fields (added in patch 26)
 *   - Uses BUTE_NUMBER instead of current attribute system
 *   - Contains deprecated autonumbering fields
 *
 * Migration Notes:
 *   - Most fields map directly to current nation structure
 *   - Mapping information must be initialized with defaults
 *   - Attribute array size changed in later patches
 *   - Autonumbering system was restructured
 */
typedef struct s25_nation {
  /* Nation Identity and Player Information */
  char name[NAMELTH+1];		/* Nation name displayed to players */
  char login[NAMELTH+1];	/* Player login name for authentication */
  char passwd[PASSLTH+1];	/* Player password for nation access */
  char leader[LEADERLTH+1];	/* Nation leader title/name */
  
  /* Basic Nation Properties */
  short repro;			/* Population reproduction rate percentage */
  short race;			/* National race identifier (affects bonuses) */
  char mark;			/* Single character nation identifier */
  char location;		/* Starting location quality rating */
  
  /* Geographic Position and Territory */
  maptype capx;			/* Capital city X coordinate */
  maptype capy;			/* Capital city Y coordinate */
  maptype centerx;		/* Relative nation center X coordinate */
  maptype centery;		/* Relative nation center Y coordinate */
  short leftedge;		/* Westernmost edge of nation territory */
  short rightedge;		/* Easternmost edge of nation territory */
  short topedge;		/* Northernmost edge of nation territory */
  short bottomedge;		/* Southernmost edge of nation territory */
  
  /* Nation Class and Military Capabilities */
  short class;			/* National class (affects abilities) */
  short aplus;			/* Attack bonus for all military units */
  short dplus;			/* Defense bonus for all military units */
  long score;			/* Current nation score for ranking */
  short active;			/* Nation activity status and AI strategy */
  short maxmove;		/* Maximum movement points for military units */
  
  /* Entity Count Tracking (for save/load) */
  idtype num_army;		/* Count of army units for serialization */
  idtype num_navy;		/* Count of naval units for serialization */
  idtype num_city;		/* Count of city structures for serialization */
  idtype num_item;		/* Count of item structures for serialization */
  idtype num_cvn;		/* Count of caravan units for serialization */
  idtype num_unum;		/* Count of autonumbering schemes */
  
  /* Entity Lists (Linked List Heads) */
  ARMY_PTR army_list;		/* Head of army units linked list */
  NAVY_PTR navy_list;		/* Head of naval units linked list */
  CITY_PTR city_list;		/* Head of cities linked list */
  ITEM_PTR item_list;		/* Head of national commodities linked list */
  CVN_PTR cvn_list;		/* Head of trading caravans linked list */
  UNUM_PTR unum_list;		/* Head of autonumbering schemes linked list */
  
  /* Diplomacy and International Relations */
  char dstatus[ABSMAXNTN];	/* Diplomatic status with each nation */
  
  /* Resource Management */
  itemtype mtrls[MTRLS_NUMBER];	/* Current total materials owned */
  itemtype m_new[MTRLS_NUMBER];	/* Materials produced this turn */
  
  /* Territory and Unit Statistics */
  short tsctrs;			/* Total number of controlled sectors */
  short tunsctrs;		/* Total number of unsupported sectors */
  short tships;			/* Total number of naval vessels */
  short twagons;		/* Total number of wagon/caravan units */
  long tmonst;			/* Total number of monster units */
  long tleaders;		/* Total number of leader units */
  long tmil;			/* Total military unit count */
  long tciv;			/* Total civilian population */
  
  /* Magic and Special Abilities */
  long powers[MAG_NUMBER];	/* Nation's magical power levels */
  short attribute[BUTE_NUMBER];	/* Nation attributes (legacy system) */
  
  /* Communication System State */
  off_t mbox_size;		/* Conquer mail file size at last read */
  off_t news_size;		/* Conquer news file size at last read */
} N25_STRUCT, *N25_PTR;

/*
 * struct s25_world - Patch 25 World Data Structure (Pre-Exposure Level)
 *
 * Represents the complete game world state from patch 25, missing the
 * exposelvl field that was added in patch 26. This structure enables
 * loading of patch 25 save files with appropriate default values for
 * the new exposure level scaling system.
 *
 * Key Differences from Later Patches:
 *   - Missing uns_char exposelvl field (scaling factor)
 *   - Missing uns_char maxdipadj field (added in patch 27)
 *   - Uses N25_PTR nation pointers instead of current format
 *
 * Migration Strategy:
 *   - Copy all existing fields to current world structure
 *   - Initialize exposelvl to default value (10 = normal)
 *   - Initialize maxdipadj to reasonable default value
 *   - Convert N25_PTR nations to current format
 */
struct s25_world
{
  /* 
   * NOTE: Field layout similar to s26_world except missing:
   * - uns_char exposelvl (exposure level scaling factor)
   * - uns_char maxdipadj (diplomacy adjustment limit)
   * Uses N25_PTR instead of current NTN_PTR for nation array.
   */
  
  maptype mapx, mapy;	/* World dimensions in map coordinates */
  ntntype nations;	/* Maximum nations allowed in game */
  ntntype npcs;		/* Count of computer-controlled nations */
  ntntype active_ntns;	/* Count of currently active nations */
  N25_PTR np[ABSMAXNTN];	/* Array of patch 25 nation pointers */
  short turn;		/* Current game turn counter */
  short start_turn;	/* Turn when campaign was initialized */
  char demigod[NAMELTH+1];	/* Game master character name */
  char passwd[PASSLTH+1];	/* Game master authentication password */
  char addpwd[PASSLTH+1];	/* Password for new nation registration */
  char demibuild;	/* Allow game master to rebuild nations */
  char demilimit;	/* Restrict game master intervention powers */
  char relative_map;	/* Enable relative coordinate display confusion */
  char hexmap;		/* Use hexagonal map display mode */
  char verify_login;	/* Require additional login verification */
  char hide_scores;	/* Conceal nation scores from players */
  char hide_login;	/* Hide nation owner login names */
  char builddist;	/* Minimum distance between starting nations */
  short fleet_cbval;	/* Base combat value bonus for naval units */
  short wagon_cbval;	/* Base combat value bonus for caravan units */
  char num_dice;	/* Number of dice rolled for combat resolution */
  char avg_damage;	/* Average damage percentage per combat round */
  char damage_limit;	/* Damage threshold to terminate combat */
  char overmatch_adj;	/* Damage adjustment for relative unit sizes */
  char pmindamage;	/* Minimum damage as percentage of roll */
  char smoothings;	/* Terrain smoothing algorithm iterations */
  char land_range;	/* Water displacement check radius */
  char preject_land;	/* Land generation rejection percentage */
  char pwater;		/* Water coverage percentage of world */
  char pmount;		/* Mountain coverage percentage of land */
  char pmercval;	/* Percentage of disbanded units becoming mercenaries */
  char prevolt;		/* Annual revolt probability percentage */
  char pscout;		/* Scout capture probability percentage */
  char pmonster;	/* Annual monster reproduction rate percentage */
  char pmercmonst;	/* Percentage of monsters joining mercenary pool */
  char ptrade;		/* Percentage of sectors containing trade goods */
  char ptgmetal;	/* Metal trade good distribution percentage */
  char ptgjewel;	/* Jewel trade good distribution percentage */
  char ptgspell;	/* Magic trade good distribution percentage */
  char supplylimit;	/* Maximum supply capacity per unit */
  char nvsplydist;	/* Supply range for naval and overland units */
  char cityxfdist;	/* Maximum transfer distance for cities */
  uns_char maxpts;	/* Maximum build points per nation per turn */
  uns_char lizards;	/* Starting count of lizard NPC nations */
  uns_char savages;	/* Starting count of savage NPC nations */
  uns_char nomads;	/* Starting count of nomad NPC nations */
  uns_char pirates;	/* Starting count of pirate NPC nations */
  char numrandom;	/* Random events generated per turn */
  char numweather;	/* Weather events generated per turn */
  long m_mil;		/* Total mercenary units available globally */
  short m_aplus;	/* Attack bonus for mercenary units */
  short m_dplus;	/* Defense bonus for mercenary units */
  itemtype bribelevel;	/* Base value for diplomatic bribery */
  itemtype w_mtrls[MTRLS_NUMBER];	/* Total material resources in world */
  long w_civ;		/* Total civilian population across all nations */
  long w_mil;		/* Total military units across all nations */
  long w_sctrs;		/* Total owned sectors across all nations */
  long w_score;		/* Combined score of all nations */
  off_t mbox_size;	/* Game master mailbox size at last read */
  off_t news_size;	/* Global news file size at last update */
  int num_unum;		/* Count of default unit numbering schemes */
  UNUM_PTR dflt_unum;	/* Default unit autonumbering configuration */
  /* Missing from patch 26: uns_char exposelvl - added in patch 26 */
  /* Missing from patch 27: uns_char maxdipadj - added in patch 27 */
};

/* ============================================================================
 * PATCH 24-25 COMPATIBILITY STRUCTURES AND CONSTANTS
 * ============================================================================
 * Purpose: Support for pre-patch 25 attribute system and autonumbering
 * Features: Legacy attribute definitions and simplified nation structure
 * Migration: Convert old attribute indices to new system
 */

/*
 * Legacy National Attribute Definitions (Pre-Patch 25)
 *
 * These constants define the attribute system used in patch 24 and earlier,
 * before the attribute reorganization that occurred in patch 25. The attribute
 * system controls various nation bonuses, penalties, and special capabilities.
 *
 * Migration Strategy:
 *   - Map old attribute indices to corresponding new attribute system
 *   - Preserve attribute values during conversion where applicable
 *   - Initialize new attributes with reasonable defaults
 *   - Handle missing attributes (gaps in numbering)
 *
 * Attribute Functions:
 *   - Economic: CHARITY, INFLATION, TAXRATE, WEALTH
 *   - Production: EATRATE, JEWELWORK, METALWORK, MINING, SPOILRATE
 *   - Military/Social: HEALTH, POPULARITY, TERROR
 *   - Communication: COMMRANGE
 *   - Magic: KNOWLEDGE, SPELLPTS
 *
 * Note: Some indices are missing (3, 11-14, 20) indicating removed or
 * deprecated attributes in the legacy system.
 */
#define OBUTE_CHARITY	0	/* Nation charity/generosity level */
#define OBUTE_COMMRANGE	1	/* Communication range for diplomacy */
#define OBUTE_EATRATE	2	/* Food consumption rate modifier */
/* Gap: Index 3 unused in legacy system */
#define OBUTE_HEALTH	4	/* Population health/disease resistance */
#define OBUTE_INFLATION	5	/* Economic inflation rate */
#define OBUTE_JEWELWORK	6	/* Jewelry production bonus */
#define OBUTE_KNOWLEDGE	7	/* Magical knowledge and research */
#define OBUTE_METALWORK	8	/* Metal working and weapons production */
#define OBUTE_MINING	9	/* Mining efficiency and output */
#define OBUTE_POPULARITY	10	/* Ruler popularity and stability */
/* Gap: Indices 11-14 unused in legacy system */
#define OBUTE_SPELLPTS	15	/* Spell point generation rate */
#define OBUTE_SPOILRATE	16	/* Resource spoilage/decay rate */
#define OBUTE_TAXRATE	17	/* Tax collection efficiency */
#define OBUTE_TERROR	18	/* Fear/intimidation level */
#define OBUTE_WEALTH	19	/* Economic wealth accumulation */
/* Gap: Index 20 unused in legacy system */
#define OBUTE_NUMBER	21	/* Total number of legacy attributes */

/*
 * struct s24_nation - Patch 24 Nation Data Structure (Pre-Autonumbering Enhancement)
 *
 * Represents individual nation state from patch 24, the earliest supported
 * legacy format. This structure lacks the autonumbering enhancements added
 * in patch 25 and uses the original attribute system defined by OBUTE_* constants.
 *
 * Key Differences from Later Patches:
 *   - Missing num_unum and unum_list fields (autonumbering added in patch 25)
 *   - Uses OBUTE_NUMBER (21) for attribute array size
 *   - Simplified structure without mapping information
 *
 * Migration Notes:
 *   - Most fields map directly to current nation structure
 *   - Autonumbering fields must be initialized as empty/NULL
 *   - Attribute array must be converted from legacy system
 *   - Communication and messaging fields preserved
 */
typedef struct s24_nation {
  /* Nation Identity and Player Information */
  char name[NAMELTH+1];		/* Nation name displayed to players */
  char login[NAMELTH+1];	/* Player login name for authentication */
  char passwd[PASSLTH+1];	/* Player password for nation access */
  char leader[LEADERLTH+1];	/* Nation leader title/name */
  
  /* Basic Nation Properties */
  short repro;			/* Population reproduction rate percentage */
  short race;			/* National race identifier (affects bonuses) */
  char mark;			/* Single character nation identifier */
  char location;		/* Starting location quality rating */
  
  /* Geographic Position and Territory */
  maptype capx;			/* Capital city X coordinate */
  maptype capy;			/* Capital city Y coordinate */
  maptype centerx;		/* Relative nation center X coordinate */
  maptype centery;		/* Relative nation center Y coordinate */
  short leftedge;		/* Westernmost edge of nation territory */
  short rightedge;		/* Easternmost edge of nation territory */
  short topedge;		/* Northernmost edge of nation territory */
  short bottomedge;		/* Southernmost edge of nation territory */
  
  /* Nation Class and Military Capabilities */
  short class;			/* National class (affects abilities) */
  short aplus;			/* Attack bonus for all military units */
  short dplus;			/* Defense bonus for all military units */
  long score;			/* Current nation score for ranking */
  short active;			/* Nation activity status and AI strategy */
  short maxmove;		/* Maximum movement points for military units */
  
  /* Entity Count Tracking (for save/load) */
  idtype num_army;		/* Count of army units for serialization */
  idtype num_navy;		/* Count of naval units for serialization */
  idtype num_city;		/* Count of city structures for serialization */
  idtype num_item;		/* Count of item structures for serialization */
  idtype num_cvn;		/* Count of caravan units for serialization */
  /* Missing from patch 25: idtype num_unum - autonumbering added later */
  
  /* Entity Lists (Linked List Heads) */
  ARMY_PTR army_list;		/* Head of army units linked list */
  NAVY_PTR navy_list;		/* Head of naval units linked list */
  CITY_PTR city_list;		/* Head of cities linked list */
  ITEM_PTR item_list;		/* Head of national commodities linked list */
  CVN_PTR cvn_list;		/* Head of trading caravans linked list */
  /* Missing from patch 25: UNUM_PTR unum_list - autonumbering added later */
  
  /* Diplomacy and International Relations */
  char dstatus[ABSMAXNTN];	/* Diplomatic status with each nation */
  
  /* Resource Management */
  itemtype mtrls[MTRLS_NUMBER];	/* Current total materials owned */
  itemtype m_new[MTRLS_NUMBER];	/* Materials produced this turn */
  
  /* Territory and Unit Statistics */
  short tsctrs;			/* Total number of controlled sectors */
  short tunsctrs;		/* Total number of unsupported sectors */
  short tships;			/* Total number of naval vessels */
  short twagons;		/* Total number of wagon/caravan units */
  long tmonst;			/* Total number of monster units */
  long tleaders;		/* Total number of leader units */
  long tmil;			/* Total military unit count */
  long tciv;			/* Total civilian population */
  
  /* Magic and Special Abilities */
  long powers[MAG_NUMBER];	/* Nation's magical power levels */
  short attribute[OBUTE_NUMBER];	/* Nation attributes (legacy system with 21 slots) */
  
  /* Communication System State */
  off_t mbox_size;		/* Conquer mail file size at last read */
  off_t news_size;		/* Conquer news file size at last read */
} N24_STRUCT, *N24_PTR;

/*
 * struct s24_world - Patch 24 World Data Structure (Minimal Legacy Format)
 *
 * Represents the earliest supported world state format from patch 24,
 * missing several features that were added in subsequent patches. This
 * structure provides the absolute minimum backward compatibility for
 * the oldest supported save file format.
 *
 * Key Differences from Later Patches:
 *   - Missing num_unum and dflt_unum fields (autonumbering added in patch 25)
 *   - Missing uns_char exposelvl field (scaling added in patch 26)
 *   - Missing uns_char maxdipadj field (diplomacy added in patch 27)
 *   - Uses N24_PTR nation pointers with legacy attribute system
 *
 * Migration Strategy:
 *   - Copy all existing fields to current world structure
 *   - Initialize missing autonumbering fields as empty
 *   - Set exposelvl to default value (10 = normal)
 *   - Set maxdipadj to reasonable default value
 *   - Convert N24_PTR nations with legacy attributes
 */
struct s24_world
{
  /* 
   * NOTE: Most minimal legacy format. Missing several fields:
   * - int num_unum, UNUM_PTR dflt_unum (autonumbering - patch 25)
   * - uns_char exposelvl (scaling factor - patch 26)
   * - uns_char maxdipadj (diplomacy limit - patch 27)
   * Uses N24_PTR with legacy OBUTE_* attribute system.
   */
  
  maptype mapx, mapy;	/* World dimensions in map coordinates */
  ntntype nations;	/* Maximum nations allowed in game */
  ntntype npcs;		/* Count of computer-controlled nations */
  ntntype active_ntns;	/* Count of currently active nations */
  N24_PTR np[ABSMAXNTN];	/* Array of patch 24 nation pointers */
  short turn;		/* Current game turn counter */
  short start_turn;	/* Turn when campaign was initialized */
  char demigod[NAMELTH+1];	/* Game master character name */
  char passwd[PASSLTH+1];	/* Game master authentication password */
  char addpwd[PASSLTH+1];	/* Password for new nation registration */
  char demibuild;	/* Allow game master to rebuild nations */
  char demilimit;	/* Restrict game master intervention powers */
  char relative_map;	/* Enable relative coordinate display confusion */
  char hexmap;		/* Use hexagonal map display mode */
  char verify_login;	/* Require additional login verification */
  char hide_scores;	/* Conceal nation scores from players */
  char hide_login;	/* Hide nation owner login names */
  char builddist;	/* Minimum distance between starting nations */
  short fleet_cbval;	/* Base combat value bonus for naval units */
  short wagon_cbval;	/* Base combat value bonus for caravan units */
  char num_dice;	/* Number of dice rolled for combat resolution */
  char avg_damage;	/* Average damage percentage per combat round */
  char damage_limit;	/* Damage threshold to terminate combat */
  char overmatch_adj;	/* Damage adjustment for relative unit sizes */
  char pmindamage;	/* Minimum damage as percentage of roll */
  char smoothings;	/* Terrain smoothing algorithm iterations */
  char land_range;	/* Water displacement check radius */
  char preject_land;	/* Land generation rejection percentage */
  char pwater;		/* Water coverage percentage of world */
  char pmount;		/* Mountain coverage percentage of land */
  char pmercval;	/* Percentage of disbanded units becoming mercenaries */
  char prevolt;		/* Annual revolt probability percentage */
  char pscout;		/* Scout capture probability percentage */
  char pmonster;	/* Annual monster reproduction rate percentage */
  char pmercmonst;	/* Percentage of monsters joining mercenary pool */
  char ptrade;		/* Percentage of sectors containing trade goods */
  char ptgmetal;	/* Metal trade good distribution percentage */
  char ptgjewel;	/* Jewel trade good distribution percentage */
  char ptgspell;	/* Magic trade good distribution percentage */
  char supplylimit;	/* Maximum supply capacity per unit */
  char nvsplydist;	/* Supply range for naval and overland units */
  char cityxfdist;	/* Maximum transfer distance for cities */
  uns_char maxpts;	/* Maximum build points per nation per turn */
  uns_char lizards;	/* Starting count of lizard NPC nations */
  uns_char savages;	/* Starting count of savage NPC nations */
  uns_char nomads;	/* Starting count of nomad NPC nations */
  uns_char pirates;	/* Starting count of pirate NPC nations */
  char numrandom;	/* Random events generated per turn */
  char numweather;	/* Weather events generated per turn */
  long m_mil;		/* Total mercenary units available globally */
  short m_aplus;	/* Attack bonus for mercenary units */
  short m_dplus;	/* Defense bonus for mercenary units */
  itemtype bribelevel;	/* Base value for diplomatic bribery */
  itemtype w_mtrls[MTRLS_NUMBER];	/* Total material resources in world */
  long w_civ;		/* Total civilian population across all nations */
  long w_mil;		/* Total military units across all nations */
  long w_sctrs;		/* Total owned sectors across all nations */
  long w_score;		/* Combined score of all nations */
  off_t mbox_size;	/* Game master mailbox size at last read */
  off_t news_size;	/* Global news file size at last update */
  /* Missing from patch 25: int num_unum, UNUM_PTR dflt_unum */
  /* Missing from patch 26: uns_char exposelvl */
  /* Missing from patch 27: uns_char maxdipadj */
};
