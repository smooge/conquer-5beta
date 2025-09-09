/*
 * butesX.h - National Attributes System Definitions
 *
 * This header defines the comprehensive national attributes framework that forms
 * the core of the game's civilization and nation management systems. It provides
 * a unified attribute system for tracking and managing all aspects of national
 * development, from economic policies to magical capabilities and population
 * characteristics.
 *
 * The attributes system encompasses the complete spectrum of national properties:
 * - Economic attributes: taxation, currency, inflation, charitable giving
 * - Social attributes: health, morale, popularity, reputation, terror
 * - Industrial attributes: mining, metalwork, jewelwork capabilities
 * - Magical attributes: wizard skill, spell points, magical knowledge
 * - Governance attributes: communication range, spoilage rates, consumption
 *
 * Key Components:
 *   - Comprehensive attribute enumeration covering all national characteristics
 *   - Attribute metadata structure with ranges, modifiers, and behavior settings
 *   - Global attribute database providing centralized attribute management
 *   - Dynamic attribute system with update cycles and automatic adjustments
 *
 * Integration Notes:
 *   - Used by: Nation management, economic systems, UI display, turn processing
 *   - Depends on: dataX.h (for BUTE_NUMBER), header.h (basic definitions)
 *   - Provides: Attribute enumeration, metadata access, national development framework
 *
 * Modernization Considerations:
 *   - Well-designed enum provides type safety for attribute identification
 *   - Consider const qualifiers for read-only string data
 *   - Add validation for attribute value ranges and update cycles
 *   - Consider floating-point types for more precise attribute calculations
 */

/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* ============================================================================
 * NATIONAL ATTRIBUTES FRAMEWORK - Civilization Development System
 * ============================================================================
 * Purpose: Define comprehensive national characteristics and development metrics
 * Usage: Include guard and attribute enumeration for national management
 * Notes: Forms the foundation of civilization advancement and strategic gameplay
 */

#ifndef __butesX_h__
#define __butesX_h__

/* ============================================================================
 * NATIONAL ATTRIBUTE ENUMERATION - Complete Civilization Characteristics
 * ============================================================================
 * Purpose: Type-safe enumeration of all national development attributes
 * Usage: Attribute identification and array indexing for national properties
 * Notes: BUTE_NUMBER constant defined in dataX.h provides total count
 */

/*
 * enum butetype - National attribute type enumeration
 *
 * Defines the complete set of national attributes that characterize a
 * civilization's development, capabilities, and current state. This enumeration
 * provides type-safe access to all national characteristics that can be
 * tracked, modified, and displayed throughout the game.
 *
 * The attributes cover all major aspects of national development:
 * - Economic policy and financial management
 * - Social conditions and population welfare
 * - Industrial capabilities and resource processing
 * - Magical development and wizard training
 * - Administrative efficiency and governance quality
 *
 * Attribute Categories:
 *   Economic: CHARITY, CURRENCY, INFLATION, TAXRATE
 *   Social: HEALTH, MORALE, POPULARITY, REPUTATION, TERROR
 *   Industrial: JEWELWORK, METALWORK, MINING
 *   Magical: KNOWLEDGE, SPELLPTS, WIZSKILL
 *   Administrative: COMMRANGE, EATRATE, MERCREP, SPOILRATE
 *
 * Usage Patterns:
 *   - Array indexing: bute_info[BUTE_HEALTH] for attribute metadata
 *   - Type checking: Function parameters use Butetype for validation
 *   - UI display: Iterate through attributes for comprehensive nation status
 *   - Game logic: Attribute-specific calculations and modifications
 */
typedef enum butetype {
    /*
     * BUTE_CHARITY - Charitable giving and social welfare attribute
     *
     * Represents the nation's commitment to charitable activities and social
     * welfare programs. Higher charity levels improve population morale and
     * international reputation but reduce available treasury funds.
     *
     * Strategic Impact: Affects population loyalty, international diplomacy
     * Economic Effect: Reduces available funds but improves social stability
     * Gameplay Role: Balance between treasury management and population happiness
     */
    BUTE_CHARITY,

    /*
     * BUTE_COMMRANGE - Communication range and administrative efficiency
     *
     * Defines the effective range of national communication systems and
     * administrative control. Higher values allow better coordination of
     * distant territories and more effective governance of large empires.
     *
     * Strategic Impact: Enables larger empire management and better coordination
     * Administrative Effect: Improves efficiency of distant territory control
     * Gameplay Role: Essential for expansion and large-scale empire management
     */
    BUTE_COMMRANGE,

    /*
     * BUTE_CURRENCY - Currency stability and monetary system strength
     *
     * Measures the stability and strength of the national currency system.
     * Strong currency facilitates trade, reduces transaction costs, and
     * provides economic advantages in international commerce.
     *
     * Strategic Impact: Affects trade efficiency and economic stability
     * Economic Effect: Influences trade costs and international commerce
     * Gameplay Role: Foundation of economic policy and trade relationships
     */
    BUTE_CURRENCY,

    /*
     * BUTE_EATRATE - Food consumption rate and dietary efficiency
     *
     * Determines how efficiently the population consumes food resources.
     * Lower eat rates indicate better food distribution, preservation, or
     * dietary efficiency, reducing the burden on agricultural production.
     *
     * Strategic Impact: Affects food supply sustainability and population capacity
     * Resource Effect: Determines food resource consumption efficiency
     * Gameplay Role: Critical for population growth and resource management
     */
    BUTE_EATRATE,

    /*
     * BUTE_HEALTH - Population health and medical system quality
     *
     * Represents the overall health status of the population and the
     * effectiveness of medical systems. Higher health levels support
     * larger populations and improve military recruitment quality.
     *
     * Strategic Impact: Enables larger populations and better military units
     * Social Effect: Improves population growth and survival rates
     * Gameplay Role: Foundation for demographic advantage and military strength
     */
    BUTE_HEALTH,

    /*
     * BUTE_INFLATION - Economic inflation rate and price stability
     *
     * Measures the rate of inflation and overall price stability within
     * the national economy. Lower inflation provides economic predictability
     * and protects citizen purchasing power and savings.
     *
     * Strategic Impact: Affects economic planning and citizen satisfaction
     * Economic Effect: Influences cost of goods and economic stability
     * Gameplay Role: Key factor in economic policy and long-term planning
     */
    BUTE_INFLATION,

    /*
     * BUTE_JEWELWORK - Jewelry crafting and luxury goods production
     *
     * Represents the nation's skill and capacity in jewelry crafting and
     * luxury goods production. Higher levels enable better trade goods,
     * diplomatic gifts, and magical item creation capabilities.
     *
     * Strategic Impact: Improves diplomatic options and trade opportunities
     * Economic Effect: Enables high-value luxury goods production
     * Gameplay Role: Supports diplomacy, trade, and magical item crafting
     */
    BUTE_JEWELWORK,

    /*
     * BUTE_KNOWLEDGE - Scientific knowledge and educational development
     *
     * Measures the accumulation of scientific knowledge, educational systems,
     * and intellectual development within the nation. Higher knowledge levels
     * support technological advancement and magical research.
     *
     * Strategic Impact: Enables technological and magical advancement
     * Development Effect: Supports research and innovation capabilities
     * Gameplay Role: Foundation for technological and magical progression
     */
    BUTE_KNOWLEDGE,

    /*
     * BUTE_MERCREP - Merchant reputation and commercial standing
     *
     * Represents the nation's reputation in commercial circles and the
     * trustworthiness of its merchants. Higher merchant reputation improves
     * trade relationships and enables better commercial opportunities.
     *
     * Strategic Impact: Affects trade efficiency and commercial relationships
     * Economic Effect: Influences trade costs and partnership opportunities
     * Gameplay Role: Critical for trade-based economic strategies
     */
    BUTE_MERCREP,

    /*
     * BUTE_METALWORK - Metalworking and industrial manufacturing capability
     *
     * Defines the nation's skill and capacity in metalworking and industrial
     * manufacturing. Higher levels enable better weapons, tools, and
     * infrastructure development for military and economic advantage.
     *
     * Strategic Impact: Improves military equipment and infrastructure quality
     * Industrial Effect: Enables advanced manufacturing and tool production
     * Gameplay Role: Essential for military strength and industrial development
     */
    BUTE_METALWORK,

    /*
     * BUTE_MINING - Mining efficiency and resource extraction capability
     *
     * Represents the nation's expertise in mining operations and resource
     * extraction. Higher mining levels increase resource yields and enable
     * access to rare materials for advanced crafting and construction.
     *
     * Strategic Impact: Improves resource availability and economic foundation
     * Resource Effect: Increases efficiency of resource extraction operations
     * Gameplay Role: Foundation for resource-based economic and military strategies
     */
    BUTE_MINING,

    /*
     * BUTE_MORALE - Population morale and social cohesion
     *
     * Measures the overall morale and social cohesion of the population.
     * Higher morale improves productivity, military effectiveness, and
     * resistance to enemy influence and internal dissent.
     *
     * Strategic Impact: Affects productivity and military performance
     * Social Effect: Influences population loyalty and effectiveness
     * Gameplay Role: Critical for maintaining stable and productive society
     */
    BUTE_MORALE,

    /*
     * BUTE_POPULARITY - Government popularity and citizen approval
     *
     * Represents the popularity of the current government and leadership
     * among the population. Higher popularity reduces rebellion risk and
     * improves cooperation with government policies and military service.
     *
     * Strategic Impact: Affects political stability and policy effectiveness
     * Governance Effect: Influences citizen cooperation and rebellion risk
     * Gameplay Role: Essential for maintaining political control and stability
     */
    BUTE_POPULARITY,

    /*
     * BUTE_REPUTATION - International reputation and diplomatic standing
     *
     * Measures the nation's reputation and standing in international
     * relations. Higher reputation improves diplomatic opportunities,
     * alliance possibilities, and international trade relationships.
     *
     * Strategic Impact: Affects diplomatic options and international relations
     * Diplomatic Effect: Influences alliance opportunities and trade partnerships
     * Gameplay Role: Critical for diplomatic strategies and international cooperation
     */
    BUTE_REPUTATION,

    /*
     * BUTE_SPELLPTS - Magical spell points and magical energy capacity
     *
     * Represents the nation's magical energy reserves and spell-casting
     * capacity. Higher spell points enable more powerful magical effects,
     * enhanced magical research, and stronger magical defenses.
     *
     * Strategic Impact: Determines magical capabilities and magical warfare potential
     * Magical Effect: Provides energy for spell casting and magical operations
     * Gameplay Role: Foundation for magical strategies and supernatural abilities
     */
    BUTE_SPELLPTS,

    /*
     * BUTE_SPOILRATE - Resource spoilage rate and preservation efficiency
     *
     * Determines the rate at which stored resources spoil or degrade over
     * time. Lower spoilage rates indicate better preservation methods,
     * storage systems, or resource management practices.
     *
     * Strategic Impact: Affects long-term resource storage and supply security
     * Resource Effect: Influences efficiency of resource stockpiling
     * Gameplay Role: Important for strategic resource management and siege preparation
     */
    BUTE_SPOILRATE,

    /*
     * BUTE_TAXRATE - Taxation rate and revenue collection efficiency
     *
     * Represents the rate and efficiency of tax collection within the nation.
     * Higher tax rates provide more revenue but may reduce population
     * satisfaction and economic growth if set too high.
     *
     * Strategic Impact: Balances government revenue with population satisfaction
     * Economic Effect: Determines government income and economic burden
     * Gameplay Role: Critical for balancing treasury needs with population happiness
     */
    BUTE_TAXRATE,

    /*
     * BUTE_TERROR - Fear level and authoritarian control
     *
     * Measures the level of fear and authoritarian control within the nation.
     * Higher terror levels may prevent rebellion but can reduce productivity,
     * innovation, and international reputation.
     *
     * Strategic Impact: Affects population control vs. productivity balance
     * Social Effect: Influences population behavior and international perception
     * Gameplay Role: Authoritarian control strategy with trade-offs
     */
    BUTE_TERROR,

    /*
     * BUTE_WIZSKILL - Wizard skill level and magical expertise
     *
     * Represents the skill level and magical expertise of the nation's
     * wizards and magical practitioners. Higher wizard skill enables more
     * powerful spells, better magical research, and advanced magical capabilities.
     *
     * Strategic Impact: Determines magical power and research capabilities
     * Magical Effect: Enables advanced spells and magical operations
     * Gameplay Role: Foundation for magical development and supernatural warfare
     */
    BUTE_WIZSKILL
} Butetype;

/* ============================================================================
 * ATTRIBUTE METADATA STRUCTURE - National Development Configuration
 * ============================================================================
 * Purpose: Complete attribute properties and behavior configuration
 * Usage: Centralized metadata for all attribute calculations and display
 * Notes: Supports dynamic attribute system with automatic updates and validation
 */

/*
 * struct s_bute - National attribute information and configuration structure
 *
 * Defines the complete metadata and configuration for each national attribute
 * including display information, value ranges, update behavior, and mathematical
 * properties. This structure provides all necessary information for attribute
 * calculations, validation, display, and automatic update processing.
 *
 * The structure supports the game's attribute management systems by providing:
 * - Display information for user interface presentation
 * - Value ranges for validation and balance enforcement
 * - Mathematical properties for calculation precision
 * - Update behavior for automatic attribute adjustments
 *
 * Usage Patterns:
 *   - UI display uses name and description for attribute presentation
 *   - Game logic uses ranges (min_base, max_base) for validation
 *   - Mathematical calculations use divisor for precision control
 *   - Update system uses modifier and clear_it for automatic adjustments
 *
 * Relationships:
 *   - Indexed by Butetype enumeration for type-safe access
 *   - Used by nation management functions for attribute operations
 *   - Referenced by UI systems for consistent attribute display
 *   - Supports turn processing for automatic attribute updates
 *
 * Modernization Notes:
 *   - Consider const char* for read-only string data
 *   - Add validation for positive ranges and reasonable values
 *   - Consider floating-point types for more precise calculations
 *   - Potential for additional fields (categories, dependencies, etc.)
 *
 * Fields:
 *   name - Display name for user interface and reports
 *   description - Detailed explanation of attribute purpose and effects
 *   start_val - Initial value for new nations
 *   min_base - Minimum allowed value for game balance
 *   max_base - Maximum allowed value for game balance
 *   divisor - Mathematical divisor for precision calculations
 *   modifier - Standard adjustment amount for positive influences
 *   clear_it - Flag indicating whether to reset during update cycles
 */
typedef struct s_bute {
    char *name;             /* Display name for user interface presentation and reports */
    char *description;      /* Detailed explanation of attribute purpose and strategic effects */
    short start_val;        /* Initial attribute value for new nations and game start */
    short min_base;         /* Minimum allowed value for game balance and validation */
    short max_base;         /* Maximum allowed value for game balance and limit enforcement */
    short divisor;          /* Mathematical divisor for floating-point precision in calculations */
    short modifier;         /* Standard adjustment amount when attribute is positively influenced */
    short clear_it;         /* Flag indicating whether attribute should be reset during update cycles */
} BUTE_STRUCT, *BUTE_PTR;

/* ============================================================================
 * GLOBAL ATTRIBUTE DATABASE - Centralized National Development Data
 * ============================================================================
 * Purpose: Master database of all national attribute types and their properties
 * Usage: Global access point for attribute metadata throughout the system
 * Notes: Initialized in dataX.c with complete attribute specifications
 */

/*
 * bute_info - Global national attribute information database array
 *
 * Provides centralized access to attribute properties for all game systems
 * dealing with national development and civilization management. This array
 * contains complete metadata for each national attribute, indexed by the
 * Butetype enumeration values defined above.
 *
 * The database supports consistent attribute handling across all game systems:
 * - Nation management functions reference this for attribute operations
 * - User interface systems access names and descriptions for display
 * - Game logic uses ranges and modifiers for calculations and validation
 * - Turn processing systems use update flags for automatic adjustments
 *
 * Array Structure:
 *   [BUTE_CHARITY] - Charitable giving and social welfare properties
 *   [BUTE_COMMRANGE] - Communication range and administrative efficiency
 *   [BUTE_CURRENCY] - Currency stability and monetary system strength
 *   [BUTE_EATRATE] - Food consumption rate and dietary efficiency
 *   [BUTE_HEALTH] - Population health and medical system quality
 *   [BUTE_INFLATION] - Economic inflation rate and price stability
 *   [BUTE_JEWELWORK] - Jewelry crafting and luxury goods production
 *   [BUTE_KNOWLEDGE] - Scientific knowledge and educational development
 *   [BUTE_MERCREP] - Merchant reputation and commercial standing
 *   [BUTE_METALWORK] - Metalworking and industrial manufacturing capability
 *   [BUTE_MINING] - Mining efficiency and resource extraction capability
 *   [BUTE_MORALE] - Population morale and social cohesion
 *   [BUTE_POPULARITY] - Government popularity and citizen approval
 *   [BUTE_REPUTATION] - International reputation and diplomatic standing
 *   [BUTE_SPELLPTS] - Magical spell points and magical energy capacity
 *   [BUTE_SPOILRATE] - Resource spoilage rate and preservation efficiency
 *   [BUTE_TAXRATE] - Taxation rate and revenue collection efficiency
 *   [BUTE_TERROR] - Fear level and authoritarian control
 *   [BUTE_WIZSKILL] - Wizard skill level and magical expertise
 *
 * Initialization:
 *   - Complete attribute data defined in dataX.c
 *   - Names, descriptions, ranges, and behavior flags set during startup
 *   - Read-only after initialization for game balance consistency
 *
 * Access Patterns:
 *   - bute_info[attribute_type].name for display purposes
 *   - bute_info[attribute_type].min_base/max_base for validation
 *   - bute_info[attribute_type].modifier for standard adjustments
 *   - Bounds checking recommended using BUTE_NUMBER from dataX.h
 *
 * Thread Safety:
 *   - Read-only after initialization, safe for concurrent access
 *   - No modification expected during normal game operation
 *   - Consider const declaration for additional safety
 */
extern BUTE_STRUCT bute_info[BUTE_NUMBER];

#endif /* __butesX_h__ */
