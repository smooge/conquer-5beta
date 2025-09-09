/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * elevegX.h - Elevation and Vegetation System Definitions
 *
 * This header defines the complete terrain system for the Conquer strategy game,
 * providing elevation classifications, vegetation types, and comprehensive terrain
 * characteristics that control movement, resource generation, seasonal effects,
 * racial preferences, and strategic gameplay mechanics throughout the world map.
 *
 * The terrain system forms the foundation of strategic gameplay by determining:
 * - Unit movement costs and accessibility by terrain type and race
 * - Resource generation rates (food and wood production) per terrain
 * - Seasonal exposure effects and environmental hazards
 * - Racial attraction values affecting settlement preferences
 * - Flight costs for aerial movement and magical transportation
 * - Map display symbols for visual terrain identification
 *
 * The dual elevation/vegetation system enables sophisticated terrain representation
 * where each map sector combines an elevation level (water to mountain peak) with
 * a vegetation type (desert to jungle), creating rich environmental diversity and
 * strategic depth through terrain-based gameplay mechanics.
 */

/*
 * Elevation System Definitions - Topographical Terrain Classification
 *
 * The elevation system provides a 6-level terrain height classification from
 * sea level to mountain peaks, controlling movement accessibility, defensive
 * advantages, resource availability, and strategic positioning in the game world.
 * Each elevation level represents distinct topographical features with specific
 * gameplay mechanics and tactical implications.
 *
 * Elevation affects:
 * - Movement costs and accessibility for ground units
 * - Defensive bonuses and tactical positioning advantages
 * - Resource generation potential and settlement viability
 * - Line of sight and reconnaissance capabilities
 * - Weather exposure and seasonal effect severity
 * - Construction requirements and infrastructure costs
 */

#define ELE_WATER	0    /* Water bodies - impassable to ground units, naval access */
#define ELE_VALLEY	1    /* Low valleys - easy movement, protected from weather */
#define ELE_CLEAR	2    /* Open plains - standard movement costs, balanced terrain */
#define ELE_HILL	3     /* Rolling hills - moderate movement penalty, some defense */
#define ELE_MOUNTAIN	4 /* Mountain ranges - difficult movement, strong defense */
#define ELE_PEAK	5    /* Mountain peaks - severe movement penalty, maximum defense */
/* ELE_NUMBER defined in dataX.h - total number of elevation types */

/*
 * Vegetation System Definitions - Environmental Biome Classification
 *
 * The vegetation system provides a 12-type environmental classification system
 * representing diverse biomes and climatic conditions across the game world.
 * Each vegetation type defines distinct resource generation capabilities, movement
 * characteristics, seasonal effects, and racial preferences that create strategic
 * diversity and environmental challenges.
 *
 * Vegetation types range from hostile environments (volcanoes, deserts, ice) to
 * resource-rich biomes (forests, jungles) and temperate zones (good vegetation).
 * The system supports complex ecological gameplay where terrain type directly
 * impacts economic development, military movement, settlement viability, and
 * racial adaptation strategies.
 *
 * Vegetation affects:
 * - Food and wood resource generation rates
 * - Movement costs and unit accessibility by race
 * - Seasonal exposure and environmental hazards
 * - Racial settlement preferences and attraction values
 * - Economic development potential and carrying capacity
 * - Strategic resource availability and territorial value
 */

#define VEG_VOLCANO	0   /* Volcanic terrain - hostile, impassable, extreme hazards */
#define VEG_DESERT	1   /* Desert regions - harsh movement, water scarcity, heat exposure */
#define VEG_TUNDRA	2   /* Arctic tundra - cold exposure, limited resources, seasonal access */
#define VEG_BARREN	3   /* Barren wasteland - poor resources, neutral movement costs */
#define VEG_LT_VEG	4   /* Light vegetation - modest resources, easy movement */
#define VEG_GOOD	5    /* Good vegetation - balanced resources, optimal for settlement */
#define VEG_WOOD	6    /* Woodland areas - wood production, moderate movement costs */
#define VEG_FOREST	7  /* Dense forest - high wood yield, movement restrictions */
#define VEG_JUNGLE	8  /* Jungle terrain - rich resources, difficult movement, concealment */
#define VEG_SWAMP	9   /* Swampland - movement penalties, disease exposure, concealment */
#define VEG_ICE		10  /* Ice fields - impassable to most units, extreme cold exposure */
#define VEG_NONE	11  /* No vegetation - special case, variable characteristics */
/* VEG_NUMBER defined in dataX.h - total number of vegetation types */

/*
 * ELEVEG_STRUCT - Comprehensive Terrain Characteristics Data Structure
 *
 * This structure defines the complete set of terrain characteristics for both
 * elevation and vegetation types, providing all gameplay mechanics, resource
 * generation, movement costs, seasonal effects, and racial preferences that
 * determine strategic value and tactical implications of terrain sectors.
 *
 * The structure serves as the foundation for terrain-based gameplay mechanics,
 * supporting complex environmental interactions, race-specific adaptations,
 * seasonal variations, and economic resource management throughout the game world.
 *
 * Key Design Features:
 * - Dual-purpose structure used for both elevation and vegetation data
 * - Race-specific arrays enabling complex racial adaptation mechanics
 * - Seasonal exposure system supporting dynamic environmental effects
 * - Resource generation framework for economic gameplay integration
 * - Movement cost system enabling tactical terrain considerations
 * - Display integration through symbol representation for map visualization
 *
 * Performance Considerations:
 * - Structure arrays (veg_info[], ele_info[]) provide O(1) terrain lookup
 * - Race-specific arrays enable efficient adaptation calculations
 * - Integer values optimize memory usage and calculation performance
 * - Pointer typedef supports efficient structure passing and referencing
 */
typedef struct s_eleveg {
  char *name;                    /* Terrain type display name for UI presentation */
  char symbol;                   /* Single-character map symbol for visual representation */
  int food_val;                  /* Food production value - agricultural/foraging potential */
  int wood_val;                  /* Wood resource generation - forestry and construction materials */
  int flight_cost;               /* Movement cost for aerial units and magical transportation */
  int move_cost[RACE_NUMBER];    /* Race-specific movement costs - terrain adaptation values */
  int exposure[SEASON_NUMBER];   /* Seasonal exposure effects - environmental hazard levels */
  int attract[RACE_NUMBER];      /* Racial attraction values - settlement preference ratings */
} ELEVEG_STRUCT, *ELEVEG_PTR;

/*
 * Global Terrain Information Arrays - Terrain System Interface
 *
 * These external array declarations provide the primary interface to the complete
 * terrain system data, enabling efficient access to all elevation and vegetation
 * characteristics throughout the game engine, AI systems, and user interface.
 *
 * Array Usage:
 * - veg_info[VEG_*]: Access vegetation type characteristics by vegetation constant
 * - ele_info[ELE_*]: Access elevation level characteristics by elevation constant
 * - Both arrays use the same ELEVEG_STRUCT format for consistent data access
 * - Array indices correspond directly to the defined terrain type constants
 *
 * Integration Points:
 * - Movement system: Query move_cost[] arrays for race-specific terrain costs
 * - Resource system: Access food_val and wood_val for economic calculations
 * - UI system: Use name and symbol fields for map display and information
 * - AI system: Reference attract[] arrays for settlement preference calculations
 * - Weather system: Utilize exposure[] arrays for seasonal effect calculations
 * - Magic system: Access flight_cost for aerial movement and teleportation
 *
 * Performance Notes:
 * - Direct array access provides O(1) terrain characteristic lookup
 * - External declarations enable efficient linking across multiple modules
 * - Structure layout optimized for cache efficiency and memory alignment
 * - Race and season arrays support vectorized calculations and bulk operations
 *
 * Modernization Opportunities:
 * - Convert to const arrays for immutable terrain data protection
 * - Add bounds checking macros for safe array access validation
 * - Consider structure padding optimization for 64-bit alignment
 * - Implement terrain data validation functions for consistency checking
 * - Add compile-time assertions for array size consistency verification
 * - Consider enum types for terrain constants to improve type safety
 * - Implement terrain combination functions for elevation+vegetation mechanics
 * - Add terrain effect calculation helper functions for common operations
 * - Consider dynamic terrain loading for mod support and customization
 * - Implement terrain data caching for performance-critical code paths
 */
extern ELEVEG_STRUCT veg_info[VEG_NUMBER];  /* Vegetation type characteristics array */
extern ELEVEG_STRUCT ele_info[ELE_NUMBER];  /* Elevation level characteristics array */
