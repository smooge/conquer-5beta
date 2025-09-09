/*
 * mtrlsX.h - Raw Materials System Definitions
 *
 * This header defines the comprehensive raw materials framework for the game's
 * economic and resource management systems. It provides a unified type system
 * for the five fundamental resources that drive all economic activity, crafting,
 * and transmutation within the game world.
 *
 * The materials system forms the foundation of the game's economy, supporting:
 * - Resource production and consumption by sectors and units
 * - Economic trading and market systems between nations
 * - Magical transmutation and alchemical processes
 * - Weight-based encumbrance and transportation logistics
 * - Strategic resource scarcity and diplomatic considerations
 *
 * Key Components:
 *   - Material type constants defining the five core resources
 *   - Comprehensive material information structure with metadata
 *   - Global material database providing centralized resource definitions
 *   - Weight and transmutation value systems for gameplay balance
 *
 * Integration Notes:
 *   - Used by: Economic systems, inventory management, crafting, magic
 *   - Depends on: dataX.h (for MTRLS_NUMBER), header.h (basic definitions)
 *   - Provides: Material type enumeration, metadata access, resource framework
 *
 * Modernization Considerations:
 *   - Consider enum for type safety instead of #define constants
 *   - Add const qualifiers for read-only string data
 *   - Implement bounds checking for material array access
 *   - Consider extensible design for mod support or additional materials
 */

/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* ============================================================================
 * MATERIAL TYPE CONSTANTS - Core Resource Enumeration
 * ============================================================================
 * Purpose: Define the five fundamental raw materials in the game economy
 * Usage: Array indices and type identification for material operations
 * Notes: Order matches initialization data in dataX.c material tables
 */

/*
 * MTRLS_TALONS - Currency and precious metal resource identifier
 *
 * Represents the primary monetary resource in the game economy. Talons serve
 * as both currency for transactions and a valuable material for crafting and
 * transmutation. This dual nature makes talons central to all economic activity.
 *
 * Value: 0 (primary array index for material system)
 * Usage: Currency transactions, economic calculations, treasure storage
 * Weight: Variable based on economic configuration
 * Transmutation: High value for magical conversion processes
 * Strategic Role: Essential for maintaining armies, purchasing goods, diplomacy
 */
#define MTRLS_TALONS	0

/*
 * MTRLS_JEWELS - Luxury goods and magical component identifier
 *
 * Represents precious gems and jewelry used for luxury trade and magical
 * enhancement. Jewels have high value-to-weight ratios and are often used
 * in diplomatic gifts and magical item creation processes.
 *
 * Value: 1 (secondary array index for material system)
 * Usage: Luxury trading, magical components, diplomatic exchanges
 * Weight: Low relative to value (high-value density)
 * Transmutation: Moderate to high value for specialized magical processes
 * Strategic Role: Diplomatic tool, magical enhancement, high-value storage
 */
#define MTRLS_JEWELS	1

/*
 * MTRLS_METALS - Industrial materials and weapon crafting identifier
 *
 * Represents iron, steel, and other metallic resources essential for military
 * equipment, tools, and infrastructure development. Metals form the backbone
 * of industrial and military capabilities within the game world.
 *
 * Value: 2 (tertiary array index for material system)
 * Usage: Weapon crafting, tool creation, infrastructure development
 * Weight: High (reflects realistic material density)
 * Transmutation: Moderate value for transformation processes
 * Strategic Role: Military equipment, industrial capacity, defensive structures
 */
#define MTRLS_METALS	2

/*
 * MTRLS_FOOD - Sustenance and population support identifier
 *
 * Represents agricultural products and food supplies necessary for population
 * maintenance and army logistics. Food scarcity directly affects population
 * growth, military effectiveness, and overall nation stability.
 *
 * Value: 3 (quaternary array index for material system)
 * Usage: Population feeding, army supply, emergency reserves
 * Weight: Moderate (bulk commodity with preservation considerations)
 * Transmutation: Low value (basic biological material)
 * Strategic Role: Population support, military logistics, siege warfare
 */
#define MTRLS_FOOD	3

/*
 * MTRLS_WOOD - Construction and fuel resource identifier
 *
 * Represents timber and wooden materials used for construction, shipbuilding,
 * and fuel production. Wood is essential for infrastructure development and
 * provides the foundation for expansion and technological advancement.
 *
 * Value: 4 (final array index for material system)
 * Usage: Construction projects, shipbuilding, fuel production
 * Weight: Moderate to high (bulk construction material)
 * Transmutation: Low to moderate value for basic transformation
 * Strategic Role: Infrastructure development, naval construction, settlement expansion
 */
#define MTRLS_WOOD	4

/* MTRLS_NUMBER constant defined in dataX.h - Total count of material types */

/* ============================================================================
 * MATERIAL INFORMATION STRUCTURE - Resource Metadata Framework
 * ============================================================================
 * Purpose: Comprehensive material properties and game balance parameters
 * Usage: Central database for all material-related calculations and display
 * Notes: Supports both gameplay mechanics and user interface presentation
 */

/*
 * struct s_mtrls - Raw material information and properties structure
 *
 * Defines the complete metadata framework for each raw material type in the
 * game economy. This structure provides all necessary information for economic
 * calculations, user interface display, weight calculations, and magical
 * transmutation processes.
 *
 * The structure supports the game's resource management systems by providing
 * centralized access to material properties, enabling consistent behavior
 * across all economic, magical, and interface systems.
 *
 * Usage Patterns:
 *   - Economic calculations use weight for transportation costs
 *   - Magic systems use transval for transmutation ratios
 *   - Interface systems use name/lname for display consistency
 *   - Inventory systems combine weight and value for capacity planning
 *
 * Relationships:
 *   - Indexed by MTRLS_* constants for type-safe material access
 *   - Used by economic computation functions in computeX.c
 *   - Referenced by display functions for consistent material naming
 *   - Supports magic system transmutation calculations
 *
 * Modernization Notes:
 *   - Consider const char* for read-only string data
 *   - Add validation for positive weight and transval values
 *   - Consider decimal/floating point for more precise balance
 *   - Potential for additional fields (rarity, availability, etc.)
 *
 * Fields:
 *   name - Display name for user interface and reports
 *   lname - Lowercase version for case-insensitive operations
 *   weight - Physical weight per unit for encumbrance calculations
 *   transval - Magical transmutation value for conversion ratios
 */
typedef struct s_mtrls {
    char *name;         /* Display name for user interface presentation and reports */
    char *lname;        /* Lowercase version for case-insensitive parsing and comparison */
    int weight;         /* Weight per unit for encumbrance and transportation calculations */
    int transval;       /* Relative transmutation value for magical conversion processes */
} MTRLS_STRUCT, *MTRLS_PTR;

/* ============================================================================
 * GLOBAL MATERIAL DATABASE - Centralized Resource Information
 * ============================================================================
 * Purpose: Master database of all material types and their properties
 * Usage: Global access point for material metadata throughout the system
 * Notes: Initialized in dataX.c with complete material specifications
 */

/*
 * mtrls_info - Global material information database array
 *
 * Provides centralized access to material properties for all game systems.
 * This array contains complete metadata for each of the five fundamental
 * raw materials, indexed by the MTRLS_* constants defined above.
 *
 * The database supports consistent material handling across all game systems:
 * - Economic calculations reference this for weight and value computations
 * - Magic systems use transmutation values for conversion ratios
 * - User interface systems access names for consistent display
 * - Inventory systems combine multiple properties for complex operations
 *
 * Array Structure:
 *   [MTRLS_TALONS] - Currency and precious metal properties
 *   [MTRLS_JEWELS] - Luxury goods and magical component properties  
 *   [MTRLS_METALS] - Industrial materials and crafting properties
 *   [MTRLS_FOOD]   - Sustenance and population support properties
 *   [MTRLS_WOOD]   - Construction and fuel resource properties
 *
 * Initialization:
 *   - Complete material data defined in dataX.c
 *   - Names, weights, and transmutation values set during startup
 *   - Read-only after initialization for game balance consistency
 *
 * Access Patterns:
 *   - mtrls_info[material_type].name for display purposes
 *   - mtrls_info[material_type].weight for encumbrance calculations
 *   - mtrls_info[material_type].transval for magical operations
 *   - Bounds checking recommended using MTRLS_NUMBER from dataX.h
 *
 * Thread Safety:
 *   - Read-only after initialization, safe for concurrent access
 *   - No modification expected during normal game operation
 *   - Consider const declaration for additional safety
 */
extern MTRLS_STRUCT mtrls_info[MTRLS_NUMBER];
