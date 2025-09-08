/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * dataioX.h - Data File Input/Output System Definitions
 *
 * Comprehensive data serialization and file I/O system providing type-safe
 * and structure-aware data persistence for the Conquer game engine. This
 * header defines the core infrastructure for saving and loading game data
 * to/from disk files with consistent format handling.
 *
 * The system provides a unified approach to data serialization that handles
 * different primitive types, custom game types, and complex data structures
 * through standardized type identifiers and size definitions. This enables
 * platform-independent data file formats and version-safe game state persistence.
 *
 * Key Components:
 *   - DIO_STRUCT: Universal data element descriptor structure
 *   - Type Size Definitions: Standardized type identifiers for serialization
 *   - Structure Size Definitions: Game entity structure identifiers
 *   - Serialization Framework: Type-safe data persistence infrastructure
 *
 * Integration Notes:
 *   - Used by: All data persistence modules (ioX.c, iodataX.c, etc.)
 *   - Depends on: Core data structures from dataA.h, dataG.h, dataX.h
 *   - Provides: Standardized serialization interface for all game data
 *
 * Modernization Considerations:
 *   - Type definitions are critical for backward compatibility
 *   - Size constants must remain stable for save file compatibility
 *   - Consider adding version information for future format changes
 *   - Thread safety may be needed for concurrent file operations
 */

/* ============================================================================
 * DATA SERIALIZATION INFRASTRUCTURE
 * ============================================================================
 * Purpose: Core data structure for describing serializable data elements
 * Usage: Provides metadata for type-safe data file I/O operations
 * Notes: Central to the game's data persistence and serialization system
 */

/*
 * struct s_dioinfo - Data I/O Element Information Descriptor
 *
 * Core data structure that describes individual data elements for serialization
 * and deserialization operations. Each instance represents a single piece of
 * data (field, array element, or structure member) with its metadata required
 * for proper file I/O handling.
 *
 * This structure enables the data I/O system to handle different data types
 * uniformly while preserving type information and size constraints. It supports
 * both primitive types (int, char, double) and complex game-specific types
 * (nations, armies, cities) through standardized descriptors.
 *
 * Usage Patterns:
 *   - Created for each field during structure serialization setup
 *   - Used in arrays to describe complete data layouts
 *   - Referenced during both save and load operations for validation
 *   - Enables automatic data format conversion and validation
 *
 * Relationships:
 *   - Used by all data I/O functions in ioX.c and iodataX.c
 *   - References type constants defined in this header
 *   - Works with structure size definitions for complete serialization
 *   - Integrates with file format versioning and compatibility systems
 *
 * Fields:
 *   name - Human-readable identifier for debugging and validation
 *   size - Byte size of the data element for memory/file operations
 */
typedef struct s_dioinfo {
    char *name;     /* Element name for identification and debugging purposes */
    int size;       /* Size in bytes of the data element for I/O operations */
} DIO_STRUCT, *DIO_PTR;

/* ============================================================================
 * DATA TYPE IDENTIFIERS FOR SERIALIZATION
 * ============================================================================
 * Purpose: Standardized type codes for data serialization and file I/O
 * Usage: Used in DIO_STRUCT to identify data types during save/load operations
 * Notes: CRITICAL - These values must NEVER change for save file compatibility
 */

/*
 * DIO_BITS - Bit field data type identifier
 *
 * Used for serializing bit fields and boolean flags that are packed into
 * integer storage. Enables efficient storage of multiple boolean values
 * in a single data element during file I/O operations.
 *
 * Value: 0
 * Usage: Bit field arrays, boolean flags, compressed data
 * Notes: Special handling required for bit-level operations
 */
#define DIO_BITS        0

/*
 * DIO_CHAR - Signed character type identifier
 *
 * Standard signed 8-bit character data type for text and small integer
 * values. Used for character strings, small numeric values, and text
 * data that requires sign preservation.
 *
 * Value: 1
 * Units: 8-bit signed integer (-128 to +127)
 * Usage: Character data, small signed integers, text processing
 * Notes: Platform-dependent sign behavior on some systems
 */
#define DIO_CHAR        1

/*
 * DIO_UNSCHAR - Unsigned character type identifier
 *
 * Unsigned 8-bit character data type for binary data and small positive
 * integer values. Used for raw byte data, small counters, and binary
 * content that must remain unsigned.
 *
 * Value: 2
 * Units: 8-bit unsigned integer (0 to 255)
 * Usage: Binary data, byte arrays, small unsigned counters
 * Notes: Preferred for binary data to avoid sign extension issues
 */
#define DIO_UNSCHAR     2

/*
 * DIO_SHORT - Signed short integer type identifier
 *
 * Standard signed 16-bit integer data type for medium-range numeric
 * values. Used for coordinates, small counters, and numeric data that
 * requires more range than character types but less than full integers.
 *
 * Value: 3
 * Units: 16-bit signed integer (-32,768 to +32,767)
 * Usage: Coordinates, medium-range counters, array indices
 * Notes: Size guaranteed to be at least 16 bits on all platforms
 */
#define DIO_SHORT       3

/*
 * DIO_UNSSHORT - Unsigned short integer type identifier
 *
 * Unsigned 16-bit integer data type for medium-range positive values.
 * Used for array sizes, positive counters, and numeric data that
 * benefits from the extended positive range of unsigned types.
 *
 * Value: 4
 * Units: 16-bit unsigned integer (0 to 65,535)
 * Usage: Array sizes, positive counters, resource quantities
 * Notes: Provides double the positive range of signed short
 */
#define DIO_UNSSHORT    4

/*
 * DIO_INT - Signed integer type identifier
 *
 * Standard signed integer data type for general-purpose numeric values.
 * Used for most game calculations, entity IDs, and numeric data that
 * requires the platform's natural integer size and performance.
 *
 * Value: 5
 * Units: Platform-dependent signed integer (typically 32-bit)
 * Usage: General numeric data, entity IDs, calculation results
 * Notes: Size varies by platform (16/32/64-bit) - use carefully
 */
#define DIO_INT         5

/*
 * DIO_LONG - Signed long integer type identifier
 *
 * Extended signed integer data type for large numeric values and
 * calculations requiring extended range. Used for large counters,
 * accumulated values, and numeric data exceeding standard int range.
 *
 * Value: 6
 * Units: Platform-dependent long integer (typically 32 or 64-bit)
 * Usage: Large counters, accumulated values, extended calculations
 * Notes: Size varies significantly by platform (32-bit vs 64-bit)
 */
#define DIO_LONG        6

/*
 * DIO_DOUBLE - Double-precision floating point type identifier
 *
 * Standard double-precision floating-point data type for calculations
 * requiring decimal precision. Used for mathematical calculations,
 * ratios, percentages, and any numeric data requiring fractional values.
 *
 * Value: 7
 * Units: IEEE 754 double-precision (64-bit) floating point
 * Usage: Mathematical calculations, ratios, decimal values
 * Notes: Platform-independent IEEE 754 format on modern systems
 */
#define DIO_DOUBLE      7

/*
 * DIO_IDTYPE - Game entity ID type identifier
 *
 * Custom game-specific data type for entity identification values.
 * Used for unique identifiers of game objects (nations, armies, cities)
 * that require special handling during serialization and cross-references.
 *
 * Value: 8
 * Usage: Entity IDs, unique identifiers, cross-reference keys
 * Notes: Custom game type requiring special serialization handling
 */
#define DIO_IDTYPE      8

/*
 * DIO_MAPTYPE - Map coordinate type identifier
 *
 * Custom game-specific data type for map coordinates and spatial
 * data. Used for hexagonal map positions, territorial boundaries,
 * and geographic data requiring special coordinate handling.
 *
 * Value: 9
 * Usage: Map coordinates, spatial data, geographic references
 * Notes: Custom type for hexagonal coordinate system
 */
#define DIO_MAPTYPE     9

/*
 * DIO_ITEMTYPE - Game item type identifier
 *
 * Custom game-specific data type for item and resource identification.
 * Used for trade goods, military equipment, magical items, and other
 * game objects that require special item-specific serialization.
 *
 * Value: 10
 * Usage: Item IDs, resource types, equipment classification
 * Notes: Custom type for game item system integration
 */
#define DIO_ITEMTYPE    10

/*
 * DIO_NTNTYPE - Nation data type identifier
 *
 * Custom game-specific data type for nation-related data structures.
 * Used for diplomatic relationships, nation-specific attributes,
 * and political data requiring special nation-aware serialization.
 *
 * Value: 11
 * Usage: Nation data, diplomatic information, political attributes
 * Notes: Custom type for nation and diplomacy system integration
 */
#define DIO_NTNTYPE     11

/*
 * DIO_NUMBER - Maximum type identifier value
 *
 * Defines the upper bound for data type identifiers, providing space
 * for future expansion of the type system. Used for bounds checking
 * and array sizing in type validation and dispatch systems.
 *
 * Value: 20 (allows expansion room for types 12-19)
 * Usage: Array sizing, bounds checking, type validation
 * Notes: Provides expansion space for future data types
 */
#define DIO_NUMBER      20      /* Allow expansion space for future types */

/* ============================================================================
 * GAME STRUCTURE IDENTIFIERS FOR SERIALIZATION
 * ============================================================================
 * Purpose: Standardized structure codes for game entity serialization
 * Usage: Used in data I/O system to identify complete structure types
 * Notes: CRITICAL - These values must NEVER change for save file compatibility
 */

/*
 * DIOS_WORLD - World/global data structure identifier
 *
 * Identifies the main world data structure containing global game state,
 * world parameters, and overall game configuration. Used for serializing
 * the complete world state including global settings and parameters.
 *
 * Value: 0
 * Structure: Contains global game state and world-wide parameters
 * Usage: World file I/O, global state persistence, game initialization
 * Notes: Largest and most critical data structure for save files
 */
#define DIOS_WORLD      0

/*
 * DIOS_SECTOR - Map sector data structure identifier
 *
 * Identifies individual map sector (hex) data structures containing
 * terrain information, ownership, improvements, and local properties.
 * Used for serializing the complete map state sector by sector.
 *
 * Value: 1
 * Structure: Individual hexagonal map sector with terrain and ownership
 * Usage: Map file I/O, terrain persistence, sector-based operations
 * Notes: Large arrays of these structures represent the complete game map
 */
#define DIOS_SECTOR     1

/*
 * DIOS_NATION - Nation data structure identifier
 *
 * Identifies nation data structures containing all information about
 * a player's empire including resources, technology, diplomatic status,
 * and national characteristics. Core structure for player data persistence.
 *
 * Value: 2
 * Structure: Complete nation/player state and characteristics
 * Usage: Player file I/O, nation persistence, diplomatic data
 * Notes: Critical for multiplayer game state and player progression
 */
#define DIOS_NATION     2

/*
 * DIOS_CITY - City data structure identifier
 *
 * Identifies city data structures containing urban settlement information
 * including population, production, improvements, and economic data.
 * Used for serializing all city-related game state and urban development.
 *
 * Value: 3
 * Structure: Urban settlement with population and production data
 * Usage: City file I/O, urban development, economic calculations
 * Notes: Key economic and population centers in the game world
 */
#define DIOS_CITY       3

/*
 * DIOS_ARMY - Army unit data structure identifier
 *
 * Identifies army unit data structures containing military force
 * information including unit composition, location, status, and
 * combat capabilities. Used for serializing all land-based military units.
 *
 * Value: 4
 * Structure: Military unit with composition and combat statistics
 * Usage: Military unit I/O, combat system, unit management
 * Notes: Core military game mechanic requiring precise state preservation
 */
#define DIOS_ARMY       4

/*
 * DIOS_NAVY - Navy unit data structure identifier
 *
 * Identifies naval unit data structures containing fleet information
 * including ship composition, cargo, location, and naval capabilities.
 * Used for serializing all sea-based military and transport units.
 *
 * Value: 5
 * Structure: Naval unit with fleet composition and cargo capacity
 * Usage: Naval unit I/O, maritime operations, naval combat
 * Notes: Specialized military units for water-based operations
 */
#define DIOS_NAVY       5

/*
 * DIOS_CARAVAN - Caravan data structure identifier
 *
 * Identifies caravan data structures containing trade convoy information
 * including cargo, route, destination, and commercial data. Used for
 * serializing all trade-related units and economic transport.
 *
 * Value: 6
 * Structure: Trade unit with cargo and route information
 * Usage: Trade unit I/O, economic system, caravan management
 * Notes: Economic units essential for trade and resource distribution
 */
#define DIOS_CARAVAN    6

/*
 * DIOS_NUMBER - Maximum structure identifier value
 *
 * Defines the upper bound for structure type identifiers, providing
 * space for future expansion of the game entity system. Used for
 * bounds checking and array sizing in structure dispatch systems.
 *
 * Value: 15 (allows expansion room for structures 7-14)
 * Usage: Array sizing, bounds checking, structure validation
 * Notes: Provides expansion space for future game entity types
 */
#define DIOS_NUMBER     15      /* Allow expansion space for future structures */
