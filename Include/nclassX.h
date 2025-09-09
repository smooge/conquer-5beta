/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * Include/nclassX.h - Nation Class System Definitions
 *
 * This header defines the nation class system that provides civilization types
 * and government structures for player nations. The class system determines
 * starting conditions, leadership structures, magical powers, and national
 * characteristics that shape gameplay and strategic options.
 *
 * The nation class framework supports:
 * - Civilization Types: Different government and social structures
 * - Leadership Hierarchies: Ruler types and minor leader classifications
 * - Racial Compatibility: Race restrictions and preferences for each class
 * - Magical Powers: Starting magical abilities and power distributions
 * - Economic Balance: Point costs for accessing different class advantages
 * - Strategic Variety: Diverse playstyles through class-specific benefits
 *
 * Integration with game systems:
 * - Magic System: Power allocation across MAG_NUMBER schools (3 magic types)
 * - Race System: Racial compatibility and preference matching
 * - Leadership System: Ruler and minor leader type definitions
 * - Economy System: Point costs for class selection and advancement
 * - UI System: Class selection and information display
 */

/* ============================================================================
 * NATION CLASS SYSTEM CONSTANTS
 * ============================================================================
 * Purpose: Core constants for nation class selection and management
 * Usage: Default class settings and system configuration
 * Notes: Provides fallback values and standard class identification
 */

#define NC_DEFAULT	1    /* Default nation class index - standard civilization type for new players */

/* ============================================================================
 * NATION CLASS DATA STRUCTURE
 * ============================================================================
 * Purpose: Complete nation class definition with all characteristics
 * Usage: Stores class information for player selection and game mechanics
 * Notes: Comprehensive structure supporting all aspects of nation classification
 * Architecture: String-based descriptions with numerical balance parameters
 */

/*
 * NCLASS_STRUCT - Nation Class Information Structure
 *
 * Comprehensive data structure defining all characteristics of a nation class
 * including leadership types, racial compatibility, magical powers, and costs.
 * Each class provides a unique gameplay experience with different advantages,
 * limitations, and strategic focus areas.
 *
 * String Fields:
 * - Class identification and display names for UI presentation
 * - Leadership hierarchy definitions for ruler and minor leader types
 * - Racial compatibility lists specifying allowed or preferred races
 * - Power descriptions for player understanding of class benefits
 *
 * Numerical Fields:
 * - Point costs for economic balance and class selection restrictions
 * - Leadership counts for starting military and administrative capacity
 * - Magical power arrays for spell system integration and class advantages
 *
 * Usage Context:
 * - Nation creation and class selection during game initialization
 * - Class-specific bonuses and restrictions during gameplay
 * - UI display of class information and selection options
 * - Game balance through cost and benefit distribution
 */
typedef struct s_nclass {
  char *name;		/* Class name - civilization type identifier for UI display and player selection */
  char *rulertype;	/* Ruler designation - leadership title and authority level for nation head */
  char *minleadtype;	/* Minor leader type - subordinate leadership roles and administrative structure */
  char *racetype;	/* Racial compatibility - list of allowed or preferred races for this class */
  char *powname;	/* Power description - textual explanation of class benefits and special abilities */
  int cost;		/* Point cost - additional expense for selecting this class during nation creation */
  int leadnum;		/* Starting leaders - number of initial leadership units for military and administration */
  long pow_given[MAG_NUMBER];	/* Magical powers - starting power levels across all magic schools (indexed by school type) */
} NCLASS_STRUCT, *NCLASS_PTR;

/* ============================================================================
 * GLOBAL NATION CLASS DATA
 * ============================================================================
 * Purpose: External declaration for nation class information array
 * Usage: Access to complete list of available nation classes
 * Notes: Implementation provides all class definitions for game system
 * Architecture: Array-based storage enabling indexed access to class data
 */

extern NCLASS_STRUCT nclass_list[];  /* Complete nation class database - array of all available civilization types */

/* ============================================================================
 * SYSTEM INTEGRATION AND USAGE NOTES
 * ============================================================================
 * 
 * Nation Class System Architecture:
 * The nation class system provides a flexible framework for defining different
 * civilization types with unique characteristics, advantages, and limitations.
 * Classes affect starting conditions, available strategies, and long-term
 * development paths for player nations.
 * 
 * Class Selection Workflow:
 * 1. Class Browsing: Players review available classes with descriptions and costs
 * 2. Compatibility Check: System validates racial compatibility with selected class
 * 3. Cost Calculation: Point costs are applied during nation creation process
 * 4. Power Allocation: Magical powers are granted according to class definitions
 * 5. Leadership Assignment: Starting leaders are created based on class parameters
 * 
 * System Dependencies:
 * - Magic System (spellsX.h): MAG_NUMBER schools and power level integration
 * - Race System (racesX.h): Racial compatibility checking and preference matching
 * - Leadership System (armyX.h): Leader creation and type assignment
 * - Economy System (dataX.h): Point cost validation and nation creation balance
 * - UI System (displayG.h): Class selection interface and information presentation
 * - Nation System (dataX.h): Integration with NTN_STRUCT for nation characteristics
 * 
 * Magical Power Integration:
 * - pow_given[MAG_NUMBER]: Starting magical abilities across all magic schools
 * - Power School Mapping: Each array index corresponds to a specific magic type
 * - Balance Considerations: Higher magical powers typically require higher point costs
 * - Racial Synergy: Class powers should complement racial magical inclinations
 * 
 * Leadership Structure Integration:
 * - rulertype: Defines the title and authority level of the nation's primary leader
 * - minleadtype: Specifies subordinate leadership roles and administrative structure
 * - leadnum: Determines starting leadership capacity for military and civilian management
 * - Leadership Hierarchy: Classes may emphasize different leadership models
 * 
 * Performance Characteristics:
 * - O(1) Class Access: Array-based storage enables direct indexed access
 * - String Storage: Text descriptions require careful memory management
 * - Configuration Loading: Class definitions typically loaded at game initialization
 * - Memory Efficiency: Structure packing considerations for class database
 * 
 * ============================================================================
 * MODERNIZATION ANALYSIS AND RECOMMENDATIONS
 * ============================================================================
 * 
 * C2023 Enhancement Opportunities:
 * 
 * 1. **Enum Type Safety** - Define nation class indices as strongly-typed enum:
 *    ```c
 *    typedef enum {
 *        NC_FEUDAL = 0,
 *        NC_REPUBLIC = 1,
 *        NC_EMPIRE = 2,
 *        // ... other class types
 *        NC_CLASS_COUNT
 *    } nation_class_t;
 *    ```
 * 
 * 2. **String Management** - Replace char* with safer string handling:
 *    ```c
 *    typedef struct {
 *        char name[64];          // Fixed-size strings
 *        char rulertype[32];     // Bounded string storage
 *        char minleadtype[32];   // Prevents buffer overflows
 *        char racetype[128];     // Adequate space for race lists
 *        char powname[256];      // Comprehensive power descriptions
 *    } nclass_strings_t;
 *    ```
 * 
 * 3. **Const Correctness** - Make class data immutable:
 *    ```c
 *    typedef struct {
 *        const char * const name;
 *        const char * const rulertype;
 *        const char * const minleadtype;
 *        const char * const racetype;
 *        const char * const powname;
 *        const int cost;
 *        const int leadnum;
 *        const long pow_given[MAG_NUMBER];
 *    } nclass_immutable_t;
 *    ```
 * 
 * 4. **Magic Power Enhancement** - Improve magical power system:
 *    ```c
 *    typedef struct {
 *        long fire_magic;        // Direct named access
 *        long earth_magic;       // Better than array indexing
 *        long air_magic;         // Self-documenting code
 *    } magic_powers_t;
 *    
 *    // Or using C2023 designated initializers:
 *    magic_powers_t powers = {
 *        .fire_magic = 100,
 *        .earth_magic = 50,
 *        .air_magic = 25
 *    };
 *    ```
 * 
 * 5. **Validation Functions** - Add class validation and utilities:
 *    ```c
 *    bool is_valid_class_index(nation_class_t class_id);
 *    bool is_race_compatible(nation_class_t class_id, race_type_t race);
 *    int calculate_class_cost(nation_class_t class_id, race_type_t race);
 *    const char *get_class_description(nation_class_t class_id);
 *    ```
 * 
 * 6. **Race Compatibility System** - Structured race matching:
 *    ```c
 *    typedef struct {
 *        race_type_t allowed_races[MAX_RACES_PER_CLASS];
 *        race_type_t preferred_races[MAX_PREFERRED_RACES];
 *        int race_cost_modifiers[RACE_COUNT];
 *    } race_compatibility_t;
 *    ```
 * 
 * 7. **Configuration System** - External class definition support:
 *    ```c
 *    int load_nation_classes(const char *config_file);
 *    bool save_nation_classes(const char *config_file);
 *    void reset_classes_to_default(void);
 *    ```
 * 
 * 8. **Error Handling** - Comprehensive validation and error reporting:
 *    ```c
 *    typedef enum {
 *        CLASS_ERROR_NONE = 0,
 *        CLASS_ERROR_INVALID_INDEX,
 *        CLASS_ERROR_RACE_INCOMPATIBLE,
 *        CLASS_ERROR_INSUFFICIENT_POINTS,
 *        CLASS_ERROR_INVALID_CONFIG
 *    } class_error_t;
 *    ```
 * 
 * Security Considerations:
 * - Input validation for all class selection and configuration
 * - Bounds checking on array access and string operations
 * - Secure parsing of class configuration files
 * - Protection against buffer overflows in string fields
 * 
 * Performance Optimizations:
 * - Cache-friendly structure layout with hot fields first
 * - String interning for repeated text values
 * - Compile-time class validation where possible
 * - Efficient class lookup and comparison operations
 * 
 * Extensibility Improvements:
 * - Dynamic class loading and registration system
 * - Plugin-based class definition modules
 * - Runtime class modification capabilities
 * - Backward compatibility for class definition changes
 * 
 * Integration Enhancements:
 * - Tighter integration with race and magic systems
 * - Class-specific AI behavior modifications
 * - Dynamic class balance adjustments
 * - Class achievement and progression systems
 */
