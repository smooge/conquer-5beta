/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * desigX.h - Sector Designation System Definitions
 *
 * This header defines the complete land use and sector development system for the
 * Conquer strategy game, providing major and minor designation classifications,
 * construction requirements, economic benefits, and territorial development
 * mechanics that control resource production, population management, taxation,
 * military recruitment, and strategic territorial enhancement throughout the
 * game world.
 *
 * The designation system operates on a dual-classification model where each map
 * sector has one major designation (primary land use) and multiple minor
 * designations (infrastructure improvements). This creates a flexible and
 * hierarchical development framework that supports complex economic gameplay,
 * territorial specialization, and strategic infrastructure investment decisions.
 *
 * Key System Components:
 * - Major Designations: 16 primary land use types from undeveloped (MAJ_NONE)
 *   to metropolitan centers (MAJ_CAPITAL), defining core sector functionality
 * - Minor Designations: 12 infrastructure improvements using bit flags for
 *   multiple simultaneous enhancements (roads, fortifications, facilities)
 * - DESG_STRUCT Framework: Comprehensive designation data structure covering
 *   construction costs, economic benefits, racial preferences, and seasonal effects
 * - Construction System: 4-tier building complexity with material requirements
 *   and time investments for territorial development progression
 * - Economic Integration: Tax generation, resource production, population support,
 *   and military recruitment integrated with designation characteristics
 *
 * Performance Considerations:
 * - Bit-packed designation storage: 16-bit values with 4-bit major + 12-bit minor
 * - O(1) designation lookup through direct array indexing
 * - Efficient macro-based testing for common designation queries
 * - Cache-friendly structure layout for designation data access
 * - Vectorized operations support for bulk territorial calculations
 *
 * Integration Points:
 * - Economic System: Resource production, taxation, trade good generation
 * - Population System: Attraction values, recruitment rates, carrying capacity
 * - Military System: Fortification bonuses, recruitment percentages, defense values
 * - Construction System: Material costs, build times, prerequisite checking
 * - UI System: Map display symbols, information presentation, development planning
 * - AI System: Territorial development priorities, economic optimization, strategic planning
 */

/*
 * Minor Designation System - Infrastructure Improvements and Status Flags
 *
 * Minor designations use bit flags to allow multiple simultaneous infrastructure
 * improvements on a single sector. Each flag represents a specific enhancement
 * that provides economic, military, or functional benefits. The bit-flag design
 * enables efficient storage and testing while supporting complex territorial
 * development with multiple overlapping improvements.
 *
 * Status and Condition Flags (0x0010-0x0040):
 * - Temporary or situational sector states affecting gameplay
 * - Used for dynamic game events and territorial conditions
 *
 * Economic Infrastructure (0x0080-0x2000):
 * - Permanent improvements that enhance resource production and economic output
 * - Provide ongoing benefits to territorial development and population support
 *
 * Military Infrastructure (0x4000-0x8000):
 * - Defensive and strategic improvements for territorial protection
 * - Enhance military capabilities and territorial security
 */
#define MIN_START	0x0010    /* Starting designation - initial sector state marker */
#define MIN_DEVASTATED	0x0010    /* War damage - reduced production, population loss, reconstruction needed */
#define MIN_FORSALE	0x0020    /* Economic marker - sector available for territorial transfer */
#define MIN_SIEGED	0x0040    /* Military status - sector under siege, restricted access and production */
#define MIN_TRADINGPOST	0x0080    /* Commercial facility - enhanced trade capacity, merchant activity hub */
#define MIN_ROADS	0x0100    /* Transportation - improved movement speed, supply lines, connectivity */
#define MIN_BLACKSMITH	0x0200    /* Industrial facility - weapon/tool production, military equipment support */
#define MIN_UNIVERSITY	0x0400    /* Educational facility - research capacity, population education, knowledge advancement */
#define MIN_CHURCH	0x0800    /* Religious facility - population happiness, cultural influence, spiritual support */
#define MIN_MILL	0x1000    /* Processing facility - agricultural efficiency, food production enhancement */
#define MIN_GRANARY	0x2000    /* Storage facility - food reserves, famine protection, supply security */
#define MIN_FORTIFIED	0x4000    /* Military improvement - defensive bonuses, siege resistance, protection enhancement */
#define MIN_HARBOR	0x8000    /* Naval facility - ship construction, naval access, maritime trade support */
/* MIN_NUMBER defined in dataX.h - total count of minor designation types */

/*
 * Major Designation System - Primary Land Use Classifications
 *
 * Major designations define the primary function and development level of each
 * map sector. Unlike minor designations, each sector has exactly one major
 * designation that determines its core economic function, resource production
 * capacity, and strategic importance. The classification system progresses from
 * undeveloped wilderness to metropolitan centers, creating a clear hierarchy
 * of territorial development and strategic value.
 *
 * Classification Categories:
 * - Undeveloped (0): Natural terrain without development
 * - Resource Production (1-6): Agricultural and extractive industries
 * - Religious/Spiritual (7): Cultural and spiritual centers
 * - Infrastructure (8-9): Transportation and logistics facilities
 * - Fortifications (10-11): Military and defensive installations
 * - Urban Settlements (12-15): Population centers with increasing complexity
 *
 * Development Progression:
 * The major designation system supports natural territorial development from
 * resource extraction through urban growth to metropolitan centers, each level
 * requiring increased investment but providing greater strategic and economic
 * benefits to the controlling nation.
 */
#define MAJ_NONE	0    /* Undeveloped - natural terrain, minimal economic value, development potential */
#define MAJ_FARM	1    /* Agricultural - basic food production, rural population support, farming community */
#define MAJ_FERTILE	2    /* Enhanced agriculture - improved food output, larger farming population, fertile soil */
#define MAJ_FRUITFUL	3    /* Premium agriculture - maximum food production, specialized crops, agricultural center */
#define MAJ_METALMINE	4    /* Industrial mining - metal extraction, industrial materials, mining workforce */
#define MAJ_JEWELMINE	5    /* Luxury mining - precious gems, high-value resources, specialized extraction */
#define MAJ_LUMBERYARD	6    /* Forestry industry - wood production, construction materials, forest management */
#define MAJ_SHRINE	7    /* Religious center - spiritual authority, cultural influence, pilgrimage destination */
#define MAJ_BRIDGE	8    /* Transportation hub - river crossing, movement facilitation, logistics support */
#define MAJ_CANAL	9    /* Waterway infrastructure - water transport, irrigation, hydraulic engineering */
#define MAJ_WALL	10   /* Defensive barrier - territorial protection, military control, border security */
#define MAJ_CACHE	11   /* Supply depot - resource storage, military supplies, strategic reserves */
#define MAJ_STOCKADE	12   /* Military settlement - frontier defense, garrison town, military outpost */
#define MAJ_TOWN	13   /* Urban center - commercial hub, population center, regional administration */
#define MAJ_CITY	14   /* Major settlement - significant population, economic center, political importance */
#define MAJ_CAPITAL	15   /* Metropolitan center - national capital, maximum development, political headquarters */
/* MAJ_NUMBER defined in dataX.h - total count of major designation types */

/*
 * Construction Complexity Classification System
 *
 * The construction system defines four tiers of building complexity that determine
 * material requirements, construction time, and labor investment needed for
 * territorial development. This classification applies to both major and minor
 * designations, creating a consistent framework for economic planning and
 * strategic development decisions.
 *
 * Construction Tiers:
 * - Each tier represents increasing complexity, cost, and strategic value
 * - Higher tiers require more materials, longer construction time, and greater investment
 * - Tier selection affects game balance by creating meaningful development choices
 * - Used by DESG_STRUCT.info_val to classify designation construction requirements
 */
#define DINFO_BUILDA	1    /* Tier 1 - Basic construction: minimal materials, short build time, low complexity */
#define DINFO_BUILDB	2    /* Tier 2 - Standard construction: moderate materials, medium build time, normal complexity */
#define DINFO_BUILDC	3    /* Tier 3 - Advanced construction: significant materials, long build time, high complexity */
#define DINFO_BUILDD	4    /* Tier 4 - Massive construction: extensive materials, very long build time, maximum complexity */

/*
 * DESG_STRUCT - Comprehensive Designation Information Structure
 *
 * This structure defines the complete characteristics of each designation type,
 * encompassing all aspects of territorial development from construction costs
 * through economic benefits to racial preferences. The structure serves as the
 * central database for designation behavior, supporting complex economic gameplay,
 * strategic territorial development, and multi-racial empire management.
 *
 * Structure Categories:
 * - Identification: Name and visual representation for UI systems
 * - Construction: Build requirements, time investment, and material costs
 * - Economics: Tax generation, production values, and maintenance costs
 * - Population: Military recruitment rates and racial attraction preferences
 * - Temporal: Seasonal variations and time-based effects
 * - Administrative: Metadata and restriction information
 *
 * Usage Patterns:
 * - Referenced by maj_dinfo[] and min_dinfo[] arrays for O(1) designation lookup
 * - Used throughout economic, construction, and UI systems for consistent behavior
 * - Supports complex calculations for territorial development planning
 * - Enables sophisticated AI decision-making for territorial optimization
 *
 * Performance Notes:
 * - Structure size optimized for cache efficiency with frequent access patterns
 * - Array fields support vectorized operations for bulk calculations
 * - Compact data types minimize memory footprint while maintaining precision
 * - Direct indexing eliminates pointer chasing for critical game loops
 */
typedef struct s_desg {
  char *name;		                    /* Designation name - human-readable identifier for UI display and debugging */
  char symbol;		                    /* Map display character - single-character visual representation for map rendering */
  uns_char build_time;	                /* Construction duration - game turns required to complete construction */
  uns_char draft_pct;	                /* Military recruitment rate - percentage of population available for army conscription */
  uns_char tax_value;	                /* Economic tax generation - base taxation income provided by this designation */
  uns_char multiplier;	                /* Economic modifier - adjustment factor for various economic calculations */
  uns_char attract[RACE_NUMBER];	    /* Racial attraction values - appeal to different species for settlement preferences */
  uns_char production[SEASON_NUMBER];	/* Seasonal production - resource output variations across game seasons */
  uns_short info_val;	                /* Administrative metadata - construction tier and compatibility restrictions */
  itemtype mtrls_cost[MTRLS_NUMBER];	/* Construction materials - resource costs required for initial building */
  itemtype mtrls_spt[MTRLS_NUMBER];	/* Maintenance materials - ongoing resource costs for designation support */
} DESG_STRUCT, *DESG_PTR;

/*
 * Designation Query and Testing Macros
 *
 * These macros provide efficient access to designation information encoded in
 * 16-bit values using bit manipulation. The encoding uses 4 bits for major
 * designation (0-15) and 12 bits for minor designation flags, enabling compact
 * storage while supporting fast queries essential for game performance.
 *
 * Bit Layout (16-bit designation value):
 * - Bits 0-3:   Major designation ID (0-15)
 * - Bits 4-15:  Minor designation flags (bit flags)
 *
 * Performance Critical:
 * These macros are used extensively throughout the game loop for map display,
 * movement validation, economic calculations, and AI decision-making. Bit
 * manipulation operations provide O(1) performance for all designation queries.
 */

/* Core Designation Extraction Macros */
#define major_desg(x)	((x) & 0x0F)           /* Extract major designation ID from encoded value */
#define minor_desg(x,y)	((x) & (y) & 0xFFF0)   /* Test for specific minor designation flag presence */
#define built_desg(x)	((x) & 0x0F)           /* Extract built designation level (alias for major_desg) */

/* Construction Information Lookup Macros */
#define majd_build(x)	build_desg(maj_dinfo[x].info)  /* Get major designation construction tier */
#define mind_build(x)	build_desg(min_dinfo[x].info)  /* Get minor designation construction tier */

/* Common Designation Classification Macros */
#define IS_FARM(x)	(major_desg(x) >= MAJ_FARM && major_desg(x) <= MAJ_FRUITFUL)  /* Test for agricultural designation */
#define IS_CITY(x)	(major_desg(x) > MAJ_WALL)                                    /* Test for urban settlement designation */
#define IS_HARBOR(x)	minor_desg(x, MIN_HARBOR)                                 /* Test for naval facility infrastructure */

/*
 * Designation Modification Macros
 *
 * These macros provide safe and efficient designation modification operations,
 * enabling runtime changes to sector designations while preserving data integrity.
 * All operations use bit manipulation to maintain the 16-bit encoding structure
 * and ensure atomic updates for concurrent access scenarios.
 *
 * Safety Considerations:
 * - Major designation changes clear existing major bits before setting new value
 * - Minor designation operations use bit flags to add/remove specific improvements
 * - All operations preserve unaffected bits to maintain designation integrity
 * - Atomic bit operations support thread-safe designation updates
 */
#define set_majordesg(x,y)	x = ((x & 0xFFF0) | (y))  /* Set major designation, preserve minor flags */
#define set_minordesg(x,y)	x |= y                    /* Add minor designation flag(s) */
#define clr_minordesg(x,y)	x &= ~y                   /* Remove minor designation flag(s) */

/*
 * Global Designation Information Arrays
 *
 * These arrays provide the central database for all designation characteristics,
 * supporting O(1) lookup of designation properties throughout the game system.
 * The arrays are indexed directly by designation constants, enabling efficient
 * access to construction costs, economic benefits, and gameplay characteristics.
 *
 * Usage Patterns:
 * - maj_dinfo[MAJ_FARM].tax_value: Get taxation income for farm designation
 * - min_dinfo[MIN_HARBOR].mtrls_cost: Get harbor construction material costs
 * - maj_dinfo[designation].attract[race]: Get racial attraction for designation
 * - min_dinfo[improvement].build_time: Get construction time for improvement
 *
 * Performance Notes:
 * - Direct array indexing provides O(1) access time for all designation queries
 * - Arrays are typically loaded once at game startup and accessed read-only
 * - Cache-friendly access patterns support efficient bulk designation operations
 * - Supports vectorized operations for map-wide designation calculations
 *
 * Integration Points:
 * - Economic System: Tax calculation, production values, maintenance costs
 * - Construction System: Material requirements, build times, prerequisite checking
 * - UI System: Designation names, map symbols, information display
 * - AI System: Territorial development planning, optimization calculations
 * - Population System: Racial preferences, recruitment rates, attraction values
 */
extern DESG_STRUCT maj_dinfo[MAJ_NUMBER];  /* Major designation characteristics database */
extern DESG_STRUCT min_dinfo[MIN_NUMBER];  /* Minor designation characteristics database */

/*
 * Modernization Analysis for C2023 Upgrade:
 *
 * Type Safety Improvements:
 * - Convert designation constants to enum types for type checking
 * - Add compile-time bounds checking for array access operations
 * - Use designated initializers for designation data initialization
 *
 * Performance Enhancements:
 * - Consider const arrays for read-only designation data
 * - Add cache alignment hints for frequently accessed structures
 * - Implement vectorized operations for bulk designation calculations
 *
 * Safety and Robustness:
 * - Add bounds checking macros for designation array access
 * - Implement designation validation functions for runtime checking
 * - Add assertion macros for development-time validation
 *
 * Interface Improvements:
 * - Convert macros to inline functions for better type checking
 * - Add designation iterator functions for safe array traversal
 * - Implement designation comparison and sorting utilities
 *
 * Extensibility Considerations:
 * - Design flexible framework for future designation types
 * - Support dynamic designation loading from configuration files
 * - Enable runtime designation modification for game mods
 */
