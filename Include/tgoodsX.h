/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * tgoodsX.h - Trade Goods System Definitions and Economic Item Framework
 *
 * This header defines the complete trade goods and economic system for the Conquer
 * strategy game, providing comprehensive economic item definitions, trade good
 * classifications, national attribute effects, and resource management framework
 * for implementing diverse economic activities, trade relationships, and resource-based
 * national development throughout the game world.
 *
 * The trade goods system forms the economic foundation of the game, with each
 * tradeable item belonging to specific classifications that affect different
 * aspects of national development, population support, and strategic capabilities.
 * Trade goods drive economic growth, support population centers, enable
 * technological advancement, and provide strategic resources for military and
 * magical operations.
 *
 * Key Components:
 * - Trade Good Classifications (Tgctype): 14 economic categories affecting
 *   different national attributes and capabilities
 * - Individual Trade Good Data (TGOOD_STRUCT): Complete item definitions with
 *   economic value, production requirements, and classification
 * - Trade Good Class Information (TGCLASS_STRUCT): Category metadata with
 *   descriptions and national attribute effects
 * - Classification Testing Macros: Convenient boolean-style queries for
 *   trade good category identification and economic planning
 *
 * Economic Integration:
 * - Population Support: Trade goods provide civilian population sustenance
 * - National Attributes: Different categories enhance specific national capabilities
 * - Production Requirements: Designation-specific production dependencies
 * - Strategic Resources: Military, magical, and technological advancement materials
 * - Economic Balance: Value systems and production frequency controls
 */
#include "butesX.h"

/*
 * Trade Good Classification System (Tgctype)
 *
 * Defines the 14 fundamental categories of trade goods, each affecting different
 * aspects of national development and strategic capabilities. These classifications
 * determine how individual trade goods impact national attributes, population
 * support, and strategic resource availability.
 *
 * Economic Categories:
 * - Basic Resources: Food production, lumber, metals for infrastructure
 * - Population Services: Health, communication, knowledge for civilian development
 * - Strategic Resources: Spells, jewels, terror for military/magical capabilities
 * - Special Effects: Spoilage reduction, popularity enhancement, fishing/farming bonuses
 *
 * National Integration:
 * - Each category maps to specific national attributes (defined via Butetype)
 * - Categories determine production requirements and population effects
 * - Strategic planning relies on category-based resource management
 * - Economic balance achieved through category weighting and availability
 */
typedef enum tgctype {
  TG_NONE,        /* No classification - placeholder or invalid trade good */
  TG_POPULARITY,  /* Popularity enhancers - luxury goods improving civilian morale and national reputation */
  TG_COMMUNICATE, /* Communication goods - items enabling information transfer and diplomatic relations */
  TG_FISHING,     /* Fishing resources - maritime food production and coastal economic development */
  TG_FARMING,     /* Agricultural goods - land-based food production and rural economic foundation */
  TG_SPOILRATE,   /* Preservation goods - items reducing spoilage and extending resource storage life */
  TG_LUMBER,      /* Forestry products - construction materials and infrastructure development resources */
  TG_KNOWLEDGE,   /* Educational resources - books, scrolls, and intellectual advancement materials */
  TG_EATRATE,     /* Food consumption goods - dietary staples and population sustenance resources */
  TG_HEALTH,      /* Medical supplies - healing items and population health maintenance resources */
  TG_TERROR,      /* Terror weapons - fear-inducing items and psychological warfare materials */
  TG_SPELLS,      /* Magical components - spell reagents and magical research materials */
  TG_METALS,      /* Metallic resources - raw materials for weapons, tools, and construction */
  TG_JEWELS       /* Precious materials - valuable gems and luxury trade goods for wealth */
} Tgctype;

/*
 * Individual Trade Good Definition (TGOOD_STRUCT)
 *
 * Defines the complete specification for a single trade good item, including
 * economic value, production requirements, classification, and population effects.
 * This structure forms the foundation for the game's economic system, enabling
 * diverse trade goods with distinct characteristics and strategic implications.
 *
 * Economic Framework:
 * - Value determines trade worth and economic impact
 * - Production rate controls availability and scarcity
 * - Classification links to national attribute effects
 * - Production requirements tie to map designations
 * - Population support defines civilian sustenance capacity
 *
 * Strategic Integration:
 * - Trade goods drive economic development and resource management
 * - Different goods support different population levels and activities
 * - Production constraints create strategic dependencies on terrain control
 * - Economic value enables trade relationships and resource accumulation
 */
typedef struct s_tgood {
  char *name;            /* Trade good display name - user-visible identifier for economic items */
  int value;             /* Economic value - base trade worth and transaction price for economic calculations */
  int rate;              /* Production frequency - relative availability within classification for balance control */
  Tgctype class;         /* Economic classification - category determining national attribute effects and strategic role */
  uns_short need_desg;   /* Production requirement - major map designation necessary for trade good production */
  uns_short pop_support; /* Population capacity - number of civilian population units this trade good can sustain */
} TGOOD_STRUCT, *TGOOD_PTR;

/*
 * Trade Good Class Metadata (TGCLASS_STRUCT)
 *
 * Defines comprehensive information about each trade good classification category,
 * including user interface descriptions, national attribute mappings, and strategic
 * effects. This structure provides the metadata framework that connects trade good
 * categories to their gameplay impacts and national development effects.
 *
 * Metadata Framework:
 * - User interface information for category presentation and player understanding
 * - National attribute integration linking economic categories to strategic capabilities
 * - Effect descriptions explaining gameplay impacts and strategic implications
 * - Weighting systems for balanced economic development and category importance
 *
 * Integration Points:
 * - Links trade good categories to national attribute systems (via Butetype)
 * - Provides user interface descriptions for economic planning and education
 * - Defines strategic importance through weighting systems
 * - Enables dynamic economic balance through category-level adjustments
 */
typedef struct s_tgclass {
  char *name;            /* Category display name - user-visible identifier for trade good classification */
  char *description;     /* Category description - detailed explanation of classification purpose and contents */
  char *affect;          /* Strategic effects - description of national and strategic impacts from this category */
  Butetype attribute;    /* National attribute mapping - specific national characteristic affected by this category */
  int weighting;         /* Category importance - relative strategic value and priority for national development */
} TGCLASS_STRUCT, *TGCLASS_PTR;

/*
 * Trade Good Classification Testing Macros
 *
 * Provides convenient boolean-style queries for determining trade good categories,
 * enabling efficient economic planning, resource management, and strategic decision
 * making. These macros abstract the classification system for easier code maintenance
 * and clearer economic logic throughout the game systems.
 *
 * Usage Pattern:
 *   if (tg_farming(trade_good_index)) {
 *     // Handle agricultural trade good
 *     agricultural_bonus += tg_info[trade_good_index].value;
 *   }
 *
 * Performance Characteristics:
 * - Single array access and enum comparison per query
 * - Constant-time classification testing
 * - Minimal memory overhead for economic planning
 * - Cache-friendly access patterns for bulk operations
 *
 * Strategic Applications:
 * - Economic planning and resource allocation
 * - Population support calculations
 * - National attribute bonus computations
 * - Trade route optimization and cargo selection
 * - Production planning and designation requirements
 */
#define tg_popular(x)     (tg_info[x].class == TG_POPULARITY)  /* Test for popularity enhancement goods - luxury and morale items */
#define tg_communicate(x) (tg_info[x].class == TG_COMMUNICATE) /* Test for communication goods - information transfer items */
#define tg_fishing(x)     (tg_info[x].class == TG_FISHING)     /* Test for fishing resources - maritime food production items */
#define tg_farming(x)     (tg_info[x].class == TG_FARMING)     /* Test for agricultural goods - land-based food production items */
#define tg_unspoils(x)    (tg_info[x].class == TG_SPOILRATE)   /* Test for preservation goods - spoilage reduction items */
#define tg_islumber(x)    (tg_info[x].class == TG_LUMBER)      /* Test for forestry products - construction material items */
#define tg_knowledge(x)   (tg_info[x].class == TG_KNOWLEDGE)   /* Test for educational resources - intellectual advancement items */
#define tg_eatrate(x)     (tg_info[x].class == TG_EATRATE)     /* Test for food consumption goods - population sustenance items */
#define tg_healthy(x)     (tg_info[x].class == TG_HEALTH)      /* Test for medical supplies - health maintenance items */
#define tg_terror(x)      (tg_info[x].class == TG_TERROR)      /* Test for terror weapons - fear-inducing warfare items */
#define tg_isspell(x)     (tg_info[x].class == TG_SPELLS)      /* Test for magical components - spell reagent items */
#define tg_isjewel(x)     (tg_info[x].class == TG_JEWELS)      /* Test for precious materials - valuable luxury items */
#define tg_ismetal(x)     (tg_info[x].class == TG_METALS)      /* Test for metallic resources - construction and weapon materials */

/*
 * Global Trade Goods Data Arrays
 *
 * External declarations for the global trade goods information arrays that
 * contain the complete economic data for all trade goods and classifications
 * in the game. These arrays form the central repository for economic information
 * and are populated by the game's data initialization systems.
 *
 * Data Integration:
 * - tg_info[] contains complete definitions for all individual trade goods
 * - tgclass_info[] contains metadata for all trade good classifications
 * - Both arrays support the complete economic system and strategic planning
 * - Array indices correspond to trade good identifiers used throughout the game
 *
 * Usage Patterns:
 *   TGOOD_STRUCT *trade_good = &tg_info[item_index];
 *   int economic_value = trade_good->value;
 *   if (tg_farming(item_index)) {
 *     // Process agricultural trade good
 *   }
 *
 * Performance Considerations:
 * - Direct array access provides constant-time trade good lookups
 * - Classification testing macros optimize common economic queries
 * - Cache-friendly memory layout for bulk economic calculations
 * - Global scope enables efficient access from all economic systems
 */
extern TGOOD_STRUCT tg_info[];      /* Global array of individual trade good definitions and economic data */
extern TGCLASS_STRUCT tgclass_info[]; /* Global array of trade good classification metadata and strategic effects */

/*
 * Modernization Analysis - Trade Goods System Enhancement Opportunities
 *
 * C2023 Upgrade Recommendations:
 * 1. **Enum Type Safety**: Convert Tgctype to strongly-typed enum with explicit underlying type
 * 2. **String Management**: Replace char* fields with safer string handling (const char*, string literals)
 * 3. **Const Correctness**: Add const qualifiers to read-only trade good data structures
 * 4. **Array Bounds**: Add compile-time array size declarations and bounds checking
 * 5. **Type Definitions**: Use standardized integer types (uint16_t) for population and designation fields
 * 6. **Memory Safety**: Add validation for array access in classification testing macros
 * 7. **Documentation**: Expand inline documentation with usage examples and integration patterns
 * 8. **Performance**: Consider structure packing and alignment for cache efficiency
 * 9. **Extensibility**: Design framework for dynamic trade good loading and mod support
 * 10. **Integration**: Strengthen links to national attribute system and economic balance framework
 *
 * Strategic Considerations:
 * - Trade goods system forms economic foundation requiring careful balance
 * - Classification system enables diverse economic strategies and national development paths
 * - Population support mechanics tie economic development to strategic capacity
 * - Production requirements create strategic dependencies on territorial control
 * - Economic value system drives trade relationships and resource accumulation strategies
 */
