/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * Include/buildA.h - World Creation and Configuration System
 *
 * This header defines the comprehensive world creation and game configuration
 * framework for the Conquer game system. It provides indexed constants for
 * accessing configuration parameters stored in configuration arrays, enabling
 * systematic world generation with customizable game balance settings.
 *
 * The build system supports:
 * - Administrative Settings: Game access, passwords, and player management
 * - World Geography: Map dimensions, terrain distribution, and world composition
 * - Combat Mechanics: Battle calculations, damage systems, and military balance
 * - Economic Systems: Trade goods distribution, mercenary systems, and resource allocation
 * - Population Dynamics: NPC nations, monster populations, and faction distributions
 * - Event Systems: Random events, weather patterns, and diplomatic mechanics
 *
 * All constants serve as array indices into configuration data structures,
 * providing a flexible and extensible configuration management system.
 */

/* ============================================================================
 * WORLD CONFIGURATION ENTRY INDICES
 * ============================================================================
 * Purpose: Array indices for accessing world configuration parameters
 * Usage: These constants index into configuration arrays for world generation
 * Notes: Each constant represents a specific configurable game parameter
 * Architecture: Indexed access enables dynamic configuration management
 */

/* === Administrative and Security Settings === */
#define BLD_DEMIGOD	0    /* Demigod player privileges - administrative access and special powers */
#define BLD_PASSWD	1    /* Global password requirement - world access security setting */
#define BLD_ADDPWD	2    /* Additional password layer - enhanced security for administrative functions */
#define BLD_DEMILIMIT	3    /* Demigod limitation setting - restrictions on administrative power usage */
#define BLD_DEMIBUILD	4    /* Demigod build privileges - world creation and modification permissions */
#define BLD_VRFYLOGIN	7    /* Login verification mode - player authentication and security validation */
#define BLD_HIDESCORES	8    /* Score hiding option - competitive balance and information visibility */
#define BLD_HIDELOGIN	9    /* Login hiding option - player privacy and security settings */
#define BLD_LATESTART	10   /* Late start compensation - catch-up mechanics for new players */

/* === World Geography and Map Configuration === */
#define BLD_MAPTYPE	5    /* Map generation type - world layout algorithm and geographical style */
#define BLD_RELATIVE	6    /* Relative positioning mode - coordinate system and spatial relationships */
#define BLD_XSIZE	11   /* World width dimension - horizontal map size in sectors */
#define BLD_YSIZE	12   /* World height dimension - vertical map size in sectors */
#define BLD_BUILDDIST	14   /* Minimum nation separation - starting position distribution algorithm */
#define BLD_PWATER	24   /* Water percentage - ocean and sea coverage of total world area */
#define BLD_PMOUNT	25   /* Mountain percentage - highland terrain distribution across landmasses */
#define BLD_SMOOTH	27   /* Terrain smoothing iterations - geographical feature refinement algorithm */
#define BLD_LRANGE	28   /* Location range checking - geographical validation and placement constraints */
#define BLD_PREJLAND	29   /* Land rejection probability - terrain generation algorithm refinement */

/* === Combat and Military Systems === */
#define BLD_POINTS	13   /* Nation building points - starting resources for nation development */
#define BLD_FLEETCBVAL	15   /* Naval combat bonus value - fleet tactical advantage in naval battles */
#define BLD_WAGONCBVAL	16   /* Caravan combat bonus value - trade unit defensive capabilities */
#define BLD_DICE	17   /* Combat dice count - randomness factor in battle resolution */
#define BLD_AVGDAM	18   /* Average damage calculation - expected combat damage per attack */
#define BLD_DAMLIM	19   /* Damage threshold limit - minimum damage required to stop attacks */
#define BLD_OVERMATCH	20   /* Overmatch damage bonus - superior force advantage in combat */
#define BLD_PMINDAM	21   /* Minimum damage percentage - guaranteed minimum damage per attack */
#define BLD_EXPOSURE	26   /* Unit exposure factor - vulnerability calculation for environmental effects */

/* === Economic and Trade Systems === */
#define BLD_PTRADE	30   /* Trade goods percentage - commercial resource distribution across sectors */
#define BLD_PTGMETAL	31   /* Metal trade goods percentage - mineral resource distribution */
#define BLD_PTGJEWEL	32   /* Jewel trade goods percentage - luxury resource distribution */
#define BLD_PTGSPELL	33   /* Magical trade goods percentage - arcane resource distribution */
#define BLD_GROWTH	34   /* Economic growth rate - population and resource development speed */
#define BLD_MERCMEN	45   /* Mercenary population - available hired military forces */
#define BLD_MERCATT	46   /* Mercenary attack bonus - hired unit combat effectiveness */
#define BLD_MERCDEF	47   /* Mercenary defense bonus - hired unit defensive capabilities */
#define BLD_PMERCVAL	48   /* Mercenary value percentage - disbanded troop conversion to mercenaries */
#define BLD_PMERCMONST	49   /* Monster mercenary percentage - creature recruitment into mercenary forces */

/* === Population and Faction Systems === */
#define BLD_NPCS	35   /* Non-player character count - AI-controlled nations in world */
#define BLD_PMONSTER	36   /* Monster reproduction rate - creature population growth mechanics */
#define BLD_LIZARD	37   /* Lizard faction count - reptilian civilization starting population */
#define BLD_SAVAGE	38   /* Savage faction count - primitive civilization starting forces */
#define BLD_NOMAD	39   /* Nomad faction count - wandering civilization starting armies */
#define BLD_PIRATE	40   /* Pirate faction count - maritime raider starting bases */
#define BLD_PREVOLT	41   /* Revolt probability modifier - population unrest and rebellion frequency */
#define BLD_PSCOUT	42   /* Scout capture probability - reconnaissance unit vulnerability */

/* === Event and Temporal Systems === */
#define BLD_YEAR	22   /* Starting year - world chronology and temporal reference point */
#define BLD_MONTH	23   /* Starting month - seasonal timing and calendar initialization */
#define BLD_NUMRANDOM	43   /* Random event frequency - unexpected occurrence rate per turn */
#define BLD_NUMWEATHER	44   /* Weather event frequency - environmental effect occurrence rate per turn */

/* === Diplomatic and Administrative Systems === */
#define BLD_MAXDIPADJ	50   /* Maximum diplomatic adjustment - relationship change limits per turn */
#define BLD_BRIBE	51   /* Bribery base cost - diplomatic influence and corruption mechanics */

/* === Supply and Logistics Systems === */
#define BLD_MAXSUPPLIES	52   /* Maximum supply capacity - logistical carrying limits for units */
#define BLD_NVSPLYDIST	53   /* Naval supply distance - fleet logistical support range */
#define BLD_CITYDIST	54   /* City interaction distance - urban influence and transfer range */

/* === System Configuration === */
#define BLD_OPTIONS	55   /* Configuration option count - total number of build system parameters */

/* ============================================================================
 * BUILD SYSTEM OPERATIONAL CONSTANTS
 * ============================================================================
 * Purpose: Core operational parameters for world generation algorithms
 * Usage: Control world creation algorithms and sector composition analysis
 * Notes: These constants are used internally by world generation systems
 */

#define BLD_START	2    /* Build process start phase - initialization stage for world generation */

/* === World Generation Grid System === */
#define MAXX		(MAPX / 8)  /* Area grid width - horizontal subdivision for generation algorithms */
#define MAXY		(MAPY / 8)  /* Area grid height - vertical subdivision for generation algorithms */
#define NUMAREAS	(MAXX * MAXY)  /* Total area count - number of generation areas for world algorithms */
#define NUMSECTS	(MAPX * MAPY)  /* Total sector count - complete world size in individual sectors */

/* === Sector Composition Classifications === */
#define WORLD_WATER	1    /* Water sector type - oceanic and aquatic terrain classification */
#define WORLD_HALF	2    /* Mixed sector type - coastal and transitional terrain classification */
#define WORLD_LAND	3    /* Land sector type - terrestrial and continental terrain classification */

/* ============================================================================
 * DEFAULT WORLD CONFIGURATION VALUES
 * ============================================================================
 * Purpose: Standard default values for world generation and game balance
 * Usage: Initial configuration settings for new world creation
 * Notes: These defaults provide balanced gameplay for typical game scenarios
 * Architecture: Values designed for moderate difficulty and strategic depth
 */

/* === Nation Development Defaults === */
#define DFLT_MAXPTS 80		/* Default nation building points - starting resource allocation for new nations */
#define DFLT_LATESTART 5	/* Late start password requirement - turns after game start requiring password */
#define DFLT_BUILDDIST 10	/* Minimum nation separation distance - spatial distribution for starting positions */

/* === Combat System Defaults === */
#define DFLT_NUMDICE 9		/* Combat dice count - randomness factor in battle resolution calculations */
#define DFLT_AVGDAMAGE 50	/* Average combat damage - expected damage output per successful attack */
#define DFLT_PMINDAMAGE 5	/* Minimum damage percentage - guaranteed damage floor as percentage of damage roll */
#define DFLT_FLEETCBVAL	-25	/* Naval combat penalty - fleet tactical disadvantage in land-based combat */
#define DFLT_WAGONCBVAL	-50	/* Caravan combat penalty - trade unit vulnerability in military engagements */
#define DFLT_DAMLIMIT 50	/* Attack stopping threshold - damage required to halt continued combat */
#define DFLT_OVERMATCH 10	/* Overmatch damage bonus - additional damage for superior military force */
#define DFLT_EXPOSURE 10	/* Unit exposure factor - environmental vulnerability coefficient (1.0 ratio) */

/* === World Geography Defaults === */
#define DFLT_XSIZE 72		/* Default world width - horizontal map dimension in sectors */
#define DFLT_YSIZE 120		/* Default world height - vertical map dimension in sectors */
#define DFLT_PWATER 50		/* Water coverage percentage - oceanic and aquatic terrain distribution */
#define DFLT_PMOUNT 70		/* Mountain coverage percentage - highland terrain distribution on landmasses */
#define DFLT_SMOOTH 6		/* Terrain smoothing iterations - geographical feature refinement passes */
#define DFLT_LRANGE 2		/* Location checking range - 8-sector block validation radius for placement */
#define DFLT_PREJLAND 100	/* Land rejection probability - terrain generation algorithm strictness */

/* === Economic System Defaults === */
#define DFLT_GROWTH 10		/* Economic growth rate - population and resource development speed */
#define DFLT_PTRADE 80		/* Trade goods coverage - percentage of land sectors containing commercial resources */
#define DFLT_PTGMETAL 25	/* Metal trade goods percentage - mineral resource distribution among trade goods */
#define DFLT_PTGJEWEL 25	/* Jewel trade goods percentage - luxury resource distribution among trade goods */
#define DFLT_PTGSPELL 15	/* Magical trade goods percentage - arcane resource distribution among trade goods */
#define DFLT_BRIBE 200000L	/* Diplomatic bribery base cost - influence and corruption mechanics (long integer) */

/* === Mercenary System Defaults === */
#define DFLT_MERCMEN (500*ABSMAXNTN)	/* Initial mercenary population - available hired forces scaled by maximum nations */
#define DFLT_MERCATT 40		/* Mercenary attack bonus - hired unit offensive combat effectiveness */
#define DFLT_MERCDEF 40		/* Mercenary defense bonus - hired unit defensive combat capabilities */
#define DFLT_PMERCVAL 15	/* Mercenary conversion rate - percentage of disbanded troops becoming mercenaries */
#define DFLT_PMERCMONST 6	/* Monster mercenary rate - percentage of creatures becoming hired forces */

/* === NPC and Faction Defaults === */
#define DFLT_NPCS 8		/* Non-player character count - AI-controlled nations in generated world */
#define DFLT_PMONSTER 12	/* Monster reproduction rate - creature population growth percentage */
#define DFLT_LIZARD 20		/* Lizard faction forts - reptilian civilization starting fortress count */
#define DFLT_SAVAGE 40		/* Savage faction forces - primitive civilization starting infantry and monsters */
#define DFLT_NOMAD 30		/* Nomad faction armies - wandering civilization starting military units */
#define DFLT_PIRATE 15		/* Pirate faction bases - maritime raider starting stronghold count */
#define DFLT_PREVOLT 25		/* Revolt probability modifier - population unrest and rebellion frequency */
#define DFLT_PSCOUT 25		/* Scout capture probability - reconnaissance unit vulnerability percentage */

/* === Event System Defaults === */
#define DFLT_NUMRANDOM 5	/* Random event frequency - unexpected occurrences per game turn */
#define DFLT_NUMWEATHER 4	/* Weather event frequency - environmental effects per game turn */
#define DFLT_MAXDIPADJ 2	/* Maximum diplomatic adjustment - relationship change limits per turn */

/* === Supply and Logistics Defaults === */
#define DFLT_SUPPLIES 2		/* Maximum supply capacity - logistical carrying limits for military units */
#define DFLT_NVSPLYDIST 1	/* Naval supply range - fleet and caravan logistical support distance */
#define DFLT_CITYXFDIST 2	/* City transfer range - maximum distance for inter-urban resource transfers */

/* ============================================================================
 * SYSTEM INTEGRATION AND USAGE NOTES
 * ============================================================================
 * 
 * Configuration Array Integration:
 * The BLD_* constants serve as indices into configuration arrays that store
 * the actual values for world generation and game balance. This indexed approach
 * enables dynamic configuration management and runtime parameter adjustment.
 * 
 * World Generation Workflow:
 * 1. Configuration Loading: BLD_* indices access stored configuration values
 * 2. Geography Generation: DFLT_* defaults initialize world creation parameters
 * 3. Terrain Processing: WORLD_* constants classify sector composition
 * 4. Grid Subdivision: MAXX/MAXY organize generation into manageable areas
 * 5. Validation: Range and constraint checking ensure valid world parameters
 * 
 * System Dependencies:
 * - World System (worldX.h): Global configuration and parameter management
 * - Map System (header.h): MAPX/MAPY definitions for world dimensions
 * - Nation System (dataX.h): ABSMAXNTN for mercenary population scaling
 * - Designation System (desigX.h): Construction and development integration
 * - File System (fileA.h): Configuration persistence and world saving
 * 
 * Performance Characteristics:
 * - O(1) Configuration Access: Direct array indexing for parameter retrieval
 * - Efficient Grid Processing: 8x8 subdivision reduces algorithmic complexity
 * - Scalable World Size: NUMSECTS calculation handles arbitrary map dimensions
 * - Memory Optimization: Constants enable compile-time optimization
 * 
 * ============================================================================
 * MODERNIZATION ANALYSIS AND RECOMMENDATIONS
 * ============================================================================
 * 
 * C2023 Enhancement Opportunities:
 * 
 * 1. **Enum Type Safety** - Convert BLD_* constants to strongly-typed enums:
 *    ```c
 *    enum build_config_index {
 *        BLD_DEMIGOD = 0,
 *        BLD_PASSWD = 1,
 *        // ... rest of BLD_* constants
 *        BLD_CONFIG_COUNT = BLD_OPTIONS
 *    };
 *    ```
 * 
 * 2. **Configuration Structure** - Replace indexed arrays with structured config:
 *    ```c
 *    typedef struct {
 *        int demigod_privileges;
 *        int password_required;
 *        int additional_password;
 *        // ... all configuration parameters
 *    } world_config_t;
 *    ```
 * 
 * 3. **Const Correctness** - Make default values immutable:
 *    ```c
 *    static const int DFLT_MAXPTS = 80;
 *    static const int DFLT_NUMDICE = 9;
 *    // ... all DFLT_* constants
 *    ```
 * 
 * 4. **Range Validation** - Add configuration bounds checking:
 *    ```c
 *    #define MIN_WORLD_SIZE 24
 *    #define MAX_WORLD_SIZE 1000
 *    _Static_assert(DFLT_XSIZE >= MIN_WORLD_SIZE, "X size too small");
 *    _Static_assert(DFLT_YSIZE <= MAX_WORLD_SIZE, "Y size too large");
 *    ```
 * 
 * 5. **Configuration Management** - Implement validation and bounds checking:
 *    ```c
 *    bool validate_world_config(const world_config_t *config);
 *    void reset_to_defaults(world_config_t *config);
 *    int save_world_config(const char *filename, const world_config_t *config);
 *    ```
 * 
 * 6. **Documentation Integration** - Add configuration metadata:
 *    ```c
 *    typedef struct {
 *        const char *name;
 *        const char *description;
 *        int min_value;
 *        int max_value;
 *        int default_value;
 *    } config_param_info_t;
 *    ```
 * 
 * 7. **Build System Integration** - CMake configuration generation:
 *    - Generate config.h from CMake variables
 *    - Runtime configuration file support
 *    - Platform-specific default adjustments
 * 
 * 8. **Error Handling** - Configuration validation with detailed error reporting:
 *    - Parameter range checking with descriptive error messages
 *    - Configuration file format validation
 *    - Graceful degradation for invalid parameters
 * 
 * Security Considerations:
 * - Input validation for all configuration parameters
 * - Bounds checking to prevent buffer overflows
 * - Secure configuration file parsing
 * - Administrative privilege validation
 * 
 * Performance Optimizations:
 * - Compile-time constant folding where possible
 * - Cache-friendly configuration structure layout
 * - Efficient parameter access patterns
 * - Reduced indirection through direct structure access
 * 
 * Extensibility Improvements:
 * - Plugin-based configuration modules
 * - Dynamic parameter registration
 * - Configuration versioning and migration
 * - Backward compatibility maintenance
 */
