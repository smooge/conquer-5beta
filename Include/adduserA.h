/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * adduserA.h - User Addition System Definitions and Nation Creation Constants
 *
 * Comprehensive configuration system for new player nation creation, defining
 * starting resource allocation, attribute distribution, and initial placement
 * parameters. This header provides the framework for balanced nation creation
 * ensuring fair starting conditions while allowing for strategic variety.
 *
 * Key Components:
 *   - Nation attribute configuration indices (AU_PEOPLE through AU_RAWGOODS)
 *   - Starting resource value constants (wood, jewels, metals)
 *   - Starting location quality classifications (random through excellent)
 *   - Resource adjustment direction controls (addition/subtraction)
 *
 * Integration Notes:
 *   - Used by: adduserA.c (nation creation system), configuration modules
 *   - Depends on: dataX.h (AU_NUMBER total count), nation system headers
 *   - Provides: Nation creation framework, resource allocation, placement system
 *
 * Modernization Considerations:
 *   - Resource values may need balancing for economic changes
 *   - Location system could benefit from enum type safety
 *   - Addition/subtraction system needs clearer semantics
 *   - Configuration system could support runtime adjustment
 */

/* ============================================================================
 * NATION ATTRIBUTE CONFIGURATION INDICES
 * ============================================================================
 * Purpose: Array indices for AU_values[] configuration of starting attributes
 * Usage: Index into attribute configuration arrays for nation creation
 * Notes: Total count AU_NUMBER defined in dataX.h, must maintain array bounds
 */

/*
 * AU_PEOPLE - Population starting value configuration index
 *
 * Controls the initial civilian population size for new nations,
 * affecting economic capacity, recruitment potential, and territorial
 * development capabilities from game start.
 *
 * Value: 0 (first configuration index)
 * Units: Population count (civilians)
 * Usage: AU_values[AU_PEOPLE] for population initialization
 * Notes: Foundation for economic and military development potential
 */
#define AU_PEOPLE	0

/*
 * AU_TREASURY - Starting silver talon treasury configuration index
 *
 * Determines initial monetary resources available for immediate
 * purchases, unit recruitment, construction projects, and early
 * game strategic investments.
 *
 * Value: 1 (treasury configuration index)
 * Units: Silver talons (game currency)
 * Usage: AU_values[AU_TREASURY] for financial initialization
 * Notes: Critical for early game flexibility and strategic options
 */
#define AU_TREASURY	1

/*
 * AU_LOCATE - Starting location quality configuration index
 *
 * Controls the quality of initial nation placement, affecting
 * access to resources, defensive positioning, expansion opportunities,
 * and long-term strategic advantages.
 *
 * Value: 2 (location quality index)
 * Units: Quality level (AU_RANDOM through AU_EXCELLENT)
 * Usage: AU_values[AU_LOCATE] for placement algorithm selection
 * Notes: Balances fairness with strategic variety in starting positions
 */
#define AU_LOCATE	2

/*
 * AU_SOLDIERS - Starting military unit count configuration index
 *
 * Determines initial armed forces strength for immediate defense,
 * early expansion, territory control, and military operations
 * from nation establishment.
 *
 * Value: 3 (military strength index)
 * Units: Soldier count (military units)
 * Usage: AU_values[AU_SOLDIERS] for military initialization
 * Notes: Balance between early security and peaceful development focus
 */
#define AU_SOLDIERS	3

/*
 * AU_ATTACK - Attack attribute starting value configuration index
 *
 * Sets initial offensive military capability affecting combat
 * effectiveness, siege success rates, and aggressive expansion
 * potential for military-focused strategies.
 *
 * Value: 4 (attack capability index)
 * Units: Attribute points (military effectiveness)
 * Usage: AU_values[AU_ATTACK] for combat attribute initialization
 * Notes: Core component of military doctrine and expansion strategy
 */
#define AU_ATTACK	4

/*
 * AU_DEFEND - Defense attribute starting value configuration index
 *
 * Establishes initial defensive military capability affecting
 * resistance to attacks, fortification effectiveness, and
 * territorial security for defensive strategies.
 *
 * Value: 5 (defense capability index)
 * Units: Attribute points (defensive effectiveness)
 * Usage: AU_values[AU_DEFEND] for defensive attribute initialization
 * Notes: Essential for territorial security and survival strategies
 */
#define AU_DEFEND	5

/*
 * AU_REPRO - Reproduction rate starting value configuration index
 *
 * Controls initial population growth rate affecting long-term
 * demographic expansion, economic development capacity, and
 * sustainable territorial growth potential.
 *
 * Value: 6 (reproduction rate index)
 * Units: Growth rate modifier (demographic expansion)
 * Usage: AU_values[AU_REPRO] for population growth initialization
 * Notes: Foundation for long-term economic and territorial expansion
 */
#define AU_REPRO	6

/*
 * AU_MOVEMENT - Movement capability starting value configuration index
 *
 * Determines initial transportation and logistics capability
 * affecting exploration speed, military mobility, trade efficiency,
 * and territorial expansion rate.
 *
 * Value: 7 (movement capability index)
 * Units: Movement points or efficiency modifier
 * Usage: AU_values[AU_MOVEMENT] for mobility initialization
 * Notes: Critical for exploration, expansion, and strategic flexibility
 */
#define AU_MOVEMENT	7

/*
 * AU_MAGIC - Magical power starting value configuration index
 *
 * Sets initial magical capabilities across all schools of magic,
 * affecting spell access, magical research potential, and
 * supernatural strategic options available to the nation.
 *
 * Value: 8 (magical power index)
 * Units: Magic power points (spell system access)
 * Usage: AU_values[AU_MAGIC] for magical system initialization
 * Notes: Enables access to spell system and magical strategies
 */
#define AU_MAGIC	8

/*
 * AU_LEADERS - Leadership capacity starting value configuration index
 *
 * Controls initial command structure and administrative capacity
 * affecting army organization, territorial management, and
 * coordinated military operations capability.
 *
 * Value: 9 (leadership capacity index)
 * Units: Leader count or command capacity
 * Usage: AU_values[AU_LEADERS] for command structure initialization
 * Notes: Essential for large-scale military and territorial coordination
 */
#define AU_LEADERS	9

/*
 * AU_RAWGOODS - Raw material starting value configuration index
 *
 * Determines initial stockpile of basic production materials
 * affecting construction capability, economic development, and
 * infrastructure building potential from game start.
 *
 * Value: 10 (raw materials index)
 * Units: Material quantity (construction resources)
 * Usage: AU_values[AU_RAWGOODS] for resource initialization
 * Notes: Foundation for construction and economic development
 */
#define AU_RAWGOODS	10

/* Note: Total configuration count AU_NUMBER defined in dataX.h for array bounds */

/* ============================================================================
 * STARTING RESOURCE VALUE CONSTANTS
 * ============================================================================
 * Purpose: Fixed resource quantities for specific trade good initialization
 * Usage: Default starting amounts for non-configurable trade goods
 * Notes: Food configured via AU_values array, these are fixed constants
 */

/*
 * AU_WOOD - Starting wood resource quantity
 *
 * Fixed initial allocation of lumber resources for construction,
 * fortification building, infrastructure development, and trade
 * activities from nation establishment.
 *
 * Value: 15000L (15,000 units)
 * Units: Wood units (construction material)
 * Usage: Direct assignment during nation creation process
 * Notes: Sufficient for early construction projects and development
 */
#define AU_WOOD		15000L

/*
 * AU_JEWELS - Starting jewelry resource quantity
 *
 * Fixed initial allocation of precious gems for luxury trade,
 * magical component usage, high-value commerce, and strategic
 * resource stockpiling from game start.
 *
 * Value: 15000L (15,000 units)
 * Units: Jewelry units (luxury trade goods)
 * Usage: Direct assignment during nation creation process
 * Notes: Enables participation in luxury trade and magical activities
 */
#define AU_JEWELS	15000L

/*
 * AU_METALS - Starting metal resource quantity
 *
 * Fixed initial allocation of metallic materials for weapon
 * production, tool manufacturing, advanced construction, and
 * military equipment creation from nation establishment.
 *
 * Value: 15000L (15,000 units)
 * Units: Metal units (industrial materials)
 * Usage: Direct assignment during nation creation process
 * Notes: Essential for military equipment and advanced construction
 */
#define AU_METALS	15000L

/* ============================================================================
 * STARTING LOCATION QUALITY CLASSIFICATIONS
 * ============================================================================
 * Purpose: Nation placement algorithm selection for territorial assignment
 * Usage: Determines starting position quality and strategic advantages
 * Notes: Affects resource access, defensive position, expansion potential
 */

/*
 * AU_OOPS - Error condition for location assignment
 *
 * Indicates failure in location assignment algorithm, requiring
 * fallback to default placement strategy or error handling
 * during nation creation process.
 *
 * Value: 0 (error/invalid condition)
 * Units: N/A (error indicator)
 * Usage: Error checking in location assignment algorithms
 * Notes: Should trigger fallback placement or creation failure
 */
#define AU_OOPS		0

/*
 * AU_RANDOM - Random location assignment strategy
 *
 * Completely random territorial placement without quality
 * considerations, providing unpredictable starting conditions
 * and maximum strategic variety in initial positioning.
 *
 * Value: 1 (random placement)
 * Units: N/A (placement strategy)
 * Usage: Location algorithm selection for random placement
 * Notes: Highest variety, potential for both advantageous and challenging starts
 */
#define AU_RANDOM	1

/*
 * AU_FAIR - Fair quality location assignment strategy
 *
 * Balanced territorial placement ensuring reasonable access
 * to basic resources and defensive positioning without
 * significant advantages or disadvantages.
 *
 * Value: 2 (fair quality placement)
 * Units: N/A (placement strategy)
 * Usage: Location algorithm selection for balanced placement
 * Notes: Ensures playable starting conditions with moderate strategic potential
 */
#define AU_FAIR		2

/*
 * AU_GOOD - Good quality location assignment strategy
 *
 * Favorable territorial placement with enhanced resource access,
 * improved defensive positioning, and better expansion opportunities
 * compared to fair assignment.
 *
 * Value: 3 (good quality placement)
 * Units: N/A (placement strategy)
 * Usage: Location algorithm selection for advantageous placement
 * Notes: Provides strategic advantages while maintaining game balance
 */
#define AU_GOOD		3

/*
 * AU_EXCELLENT - Excellent quality location assignment strategy
 *
 * Premium territorial placement with optimal resource access,
 * superior defensive positioning, and maximum expansion potential
 * for the highest quality starting conditions.
 *
 * Value: 4 (excellent quality placement)
 * Units: N/A (placement strategy)
 * Usage: Location algorithm selection for premium placement
 * Notes: Provides significant strategic advantages, may affect game balance
 */
#define AU_EXCELLENT	4

/* ============================================================================
 * RESOURCE ADJUSTMENT DIRECTION CONTROLS
 * ============================================================================
 * Purpose: Mathematical operation selection for resource modification
 * Usage: Controls whether resource adjustments increase or decrease values
 * Notes: Used in configuration algorithms for dynamic resource adjustment
 */

/*
 * ADDITION - Additive resource adjustment operation
 *
 * Indicates that resource modification should increase the
 * target value, used in configuration algorithms for
 * positive adjustments to nation attributes or resources.
 *
 * Value: 0 (addition operation)
 * Units: N/A (mathematical operation)
 * Usage: Resource adjustment algorithms for positive modifications
 * Notes: Increases resource or attribute values during configuration
 */
#define ADDITION	0

/*
 * SUBTRACTION - Subtractive resource adjustment operation
 *
 * Indicates that resource modification should decrease the
 * target value, used in configuration algorithms for
 * negative adjustments to nation attributes or resources.
 *
 * Value: 1 (subtraction operation)
 * Units: N/A (mathematical operation)
 * Usage: Resource adjustment algorithms for negative modifications
 * Notes: Decreases resource or attribute values during configuration
 */
#define SUBTRACTION	1
