/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * weightX.h - Weight System Definitions and Strategic Value Framework
 *
 * This header defines the strategic weight system for the Conquer strategy game,
 * providing settlement value classifications and strategic importance calculations
 * used by AI decision-making systems, combat target prioritization, economic
 * evaluation algorithms, and strategic planning modules throughout the game engine.
 *
 * The weight system establishes a hierarchical value framework for different
 * settlement types, enabling sophisticated AI behavior, resource allocation
 * decisions, and strategic target assessment based on settlement importance.
 * Values range from 0 to 255, creating a comprehensive scale for strategic
 * evaluation and priority-based decision making.
 *
 * Key Weight System Components:
 * - Settlement hierarchy: Exponential importance scaling from stockades to capitals
 * - Strategic value: Numerical framework for AI target prioritization and evaluation
 * - Decision support: Algorithmic foundation for resource allocation and planning
 * - Combat targeting: Priority assessment for military operations and siege planning
 * - Economic evaluation: Settlement value calculations for trade and development
 *
 * Integration Points:
 * - AI systems: Strategic decision-making and target prioritization algorithms
 * - Combat engine: Target selection and tactical priority assessment
 * - Economic systems: Settlement value calculations and resource allocation
 * - Map evaluation: Territory assessment and strategic importance analysis
 * - Diplomatic relations: Settlement value considerations in negotiations
 *
 * Design Philosophy:
 * The weight system uses powers of 2 progression (16, 32, 64, 128) to create
 * clear hierarchical distinctions between settlement types, enabling sophisticated
 * AI decision trees and strategic evaluation algorithms with consistent scaling
 * and predictable value relationships across different settlement categories.
 */

/*
 * Settlement Weight Classifications
 *
 * Strategic value constants defining the hierarchical importance of different
 * settlement types for AI decision-making, combat targeting, economic evaluation,
 * and strategic planning systems. Values use powers-of-2 progression to create
 * clear distinction levels and enable efficient binary operations.
 *
 * Weight Range: 0-255 (8-bit value space)
 * Progression: Powers of 2 for hierarchical scaling
 * Usage: AI systems, combat targeting, economic evaluation, strategic planning
 */

/*
 * WEIGHT_CAPITAL - Capital city strategic weight
 *
 * Defines the highest strategic value for capital cities, representing the
 * most important settlement type in terms of political control, economic
 * significance, and military value. Capitals serve as administrative centers,
 * economic hubs, and strategic focal points for national power projection.
 *
 * Strategic Significance:
 * - Political: Administrative center and seat of government power
 * - Economic: Major trade hub and resource concentration point
 * - Military: Primary strategic target and defensive stronghold
 * - Diplomatic: Capital control affects negotiation leverage and status
 * - Victory: Often represents critical victory conditions and campaign objectives
 *
 * AI Behavior Impact:
 * - Highest priority for attack planning and siege operations
 * - Maximum defensive resource allocation and protection priority
 * - Primary target for strategic bombing and disruption campaigns
 * - Critical factor in territorial control and expansion decisions
 * - Major influence on diplomatic relations and alliance considerations
 *
 * Value: 128 (50% of maximum weight scale, leaving room for super-priority targets)
 */
#define WEIGHT_CAPITAL	128

/*
 * WEIGHT_CITY - Major city strategic weight
 *
 * Defines strategic value for major cities, representing significant urban
 * centers with substantial economic, military, and population importance.
 * Cities serve as regional hubs, military production centers, and economic
 * engines supporting national power and territorial control.
 *
 * Strategic Significance:
 * - Economic: Regional trade centers and industrial production hubs
 * - Military: Unit recruitment centers and defensive strongpoints
 * - Population: Major population centers affecting national strength
 * - Infrastructure: Transportation hubs and resource processing centers
 * - Territory: Regional control points affecting surrounding area influence
 *
 * AI Behavior Impact:
 * - High priority for siege operations and territorial expansion
 * - Significant defensive resource allocation and fortification priority
 * - Important targets for economic disruption and resource denial
 * - Major factors in regional control and expansion planning
 * - Substantial influence on military recruitment and unit production
 *
 * Value: 64 (Half of capital weight, double town weight - clear hierarchical progression)
 */
#define WEIGHT_CITY	64

/*
 * WEIGHT_TOWN - Town settlement strategic weight
 *
 * Defines strategic value for town settlements, representing moderate-sized
 * population centers with regional economic importance and defensive value.
 * Towns serve as local administration centers, resource gathering points,
 * and secondary military positions supporting broader territorial control.
 *
 * Strategic Significance:
 * - Economic: Local trade centers and resource collection points
 * - Military: Secondary defensive positions and unit staging areas
 * - Population: Regional population centers affecting local control
 * - Administration: Local governance and tax collection centers
 * - Infrastructure: Supply points and communication relay stations
 *
 * AI Behavior Impact:
 * - Moderate priority for expansion and territorial consolidation
 * - Balanced resource allocation for defense and development
 * - Secondary targets for economic disruption campaigns
 * - Regional control factors in expansion and consolidation planning
 * - Local military recruitment and supply chain considerations
 *
 * Value: 32 (Half of city weight, double stockade weight - maintains exponential scaling)
 */
#define WEIGHT_TOWN	32

/*
 * WEIGHT_STOCKADE - Frontier settlement strategic weight
 *
 * Defines strategic value for stockade settlements, representing the smallest
 * fortified positions with basic defensive capabilities and minimal economic
 * importance. Stockades serve as frontier outposts, border guards, and
 * territorial markers establishing initial claims and defensive perimeters.
 *
 * Strategic Significance:
 * - Military: Frontier defensive positions and early warning outposts
 * - Territory: Border markers and territorial claim establishment
 * - Economic: Minimal resource generation and basic trade facilitation
 * - Expansion: Foundation points for future settlement development
 * - Defense: Perimeter security and approach monitoring positions
 *
 * AI Behavior Impact:
 * - Low priority for direct assault unless strategically positioned
 * - Minimal defensive resource allocation and protection priority
 * - Opportunity targets for quick territorial gains and expansion
 * - Border control factors in expansion and security planning
 * - Basic military staging points for frontier operations
 *
 * Value: 16 (Minimum strategic weight for fortified positions, base level for scaling)
 */
#define WEIGHT_STOCKADE	16

/*
 * Weight System Usage Notes:
 *
 * Hierarchical Scaling:
 * The powers-of-2 progression (16 → 32 → 64 → 128) creates clear value
 * distinctions that enable sophisticated AI decision algorithms while
 * maintaining simple arithmetic relationships and efficient calculations.
 *
 * AI Integration:
 * Weight values integrate directly with AI evaluation functions, combat
 * targeting systems, economic planning algorithms, and strategic assessment
 * modules throughout the game engine for consistent decision-making.
 *
 * Extensibility:
 * The 0-255 range provides space for additional settlement types, special
 * modifiers, or dynamic weight adjustments based on game state, diplomatic
 * relations, or strategic circumstances.
 *
 * Performance Considerations:
 * Powers-of-2 values enable efficient bit operations, fast comparisons,
 * and optimized arithmetic calculations in performance-critical AI and
 * evaluation systems.
 *
 * Modernization Opportunities for C2023 Upgrade:
 * 1. Convert to enum for type safety and better debugging support
 * 2. Add weight calculation functions for dynamic strategic assessment
 * 3. Implement weight modifier system for situational adjustments
 * 4. Create weight category validation and bounds checking
 * 5. Add strategic value calculation utilities and helper functions
 * 6. Implement weight-based priority queue systems for AI decision-making
 * 7. Create settlement value assessment framework with multiple factors
 * 8. Add debugging and logging support for weight-based decisions
 * 9. Implement dynamic weight adjustment based on game state changes
 * 10. Create weight system documentation and usage guidelines
 */
