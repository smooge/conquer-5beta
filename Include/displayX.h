/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * displayX.h - Extended Display System Definitions
 *
 * This header defines the comprehensive display system for the game's user
 * interface, providing constants and modes for map visualization, tactical
 * overlays, and user interface presentation. The system supports multiple
 * display modes, highlighting schemes, and zoom levels for strategic gameplay.
 *
 * The display system consists of three main components:
 * 1. Display Modes: What information to show (terrain, resources, units, etc.)
 * 2. Highlighting Modes: How to visually emphasize strategic information
 * 3. Positioning and Zoom: Where and how detailed to present the information
 *
 * Technical Features:
 * - 21 distinct display modes for different strategic views
 * - 17 highlighting modes for tactical overlays
 * - 4 hexagonal map positioning modes for optimal display
 * - 3 zoom levels for varying levels of detail
 * - Extensible design for additional display modes
 *
 * Integration Notes:
 * - Works with hlightX.h for sector-level highlighting
 * - Integrates with map display rendering system
 * - Supports both strategic overview and tactical detail views
 * - Designed for real-time display updates during gameplay
 *
 * Performance Considerations:
 * - Constants designed for fast switch/case operations
 * - Sequential numbering for efficient array indexing
 * - Minimal memory overhead for display state tracking
 */

/* This file declares the data structure for display operations */

/*
 * Display Mode Constants
 *
 * These constants define what type of information should be displayed on
 * the map. Each mode provides a different strategic view of the game world,
 * allowing players to focus on specific aspects of their empire or military
 * operations.
 */

/*
 * DI_BLANK - Blank Display Mode
 *
 * Displays minimal or no information overlay on the map.
 * Shows basic terrain without additional strategic information.
 * Useful for clean map viewing or screenshot purposes.
 *
 * Value: 0
 * Usage: Default/clean display state
 */
#define DI_BLANK	0

/*
 * DI_CONT - Continent Display Mode
 *
 * Displays continent boundaries and geographical regions.
 * Shows major landmasses and geographical divisions.
 * Useful for understanding large-scale geography and strategic positioning.
 *
 * Value: 1  
 * Usage: Geographic overview and strategic planning
 */
#define DI_CONT		1

/*
 * DI_DESG - Designation Display Mode
 *
 * Displays sector designations and classifications.
 * Shows how sectors are categorized (cities, fortresses, farms, etc.).
 * Essential for understanding territorial development and infrastructure.
 *
 * Value: 2
 * Usage: Infrastructure planning and territorial management
 */
#define DI_DESG		2

/*
 * Resource Display Modes
 * These modes show the distribution of various resources across the map.
 */

/*
 * DI_FOOD - Food Resource Display Mode
 *
 * Displays food production and availability across sectors.
 * Shows agricultural capacity and food surplus/deficit areas.
 * Critical for population management and supply planning.
 *
 * Value: 3
 * Usage: Agricultural planning and food security analysis
 */
#define DI_FOOD		3

/*
 * DI_JEWEL - Jewel Resource Display Mode
 *
 * Displays jewel deposits and luxury resource availability.
 * Shows high-value trade goods and wealth generation potential.
 * Important for economic strategy and trade route planning.
 *
 * Value: 4
 * Usage: Economic planning and luxury trade management
 */
#define DI_JEWEL	4

/*
 * DI_METAL - Metal Resource Display Mode
 *
 * Displays metal deposits and industrial resource availability.
 * Shows raw materials for military equipment and construction.
 * Essential for military production and industrial development.
 *
 * Value: 5
 * Usage: Industrial planning and military supply management
 */
#define DI_METAL	5

/*
 * Political and Demographic Display Modes
 */

/*
 * DI_NATN - Nation Display Mode
 *
 * Displays national boundaries and territorial control.
 * Shows which nation controls each sector of the map.
 * Fundamental for diplomatic analysis and territorial disputes.
 *
 * Value: 6
 * Usage: Political analysis and territorial management
 */
#define DI_NATN		6

/*
 * DI_PEOP - Population Display Mode
 *
 * Displays population density and demographic information.
 * Shows civilian population distribution across territories.
 * Important for taxation, recruitment, and economic planning.
 *
 * Value: 7
 * Usage: Demographic analysis and population management
 */
#define DI_PEOP		7

/*
 * DI_RACE - Race Display Mode
 *
 * Displays racial or ethnic composition of populations.
 * Shows demographic diversity and cultural boundaries.
 * Relevant for diplomatic relations and cultural management.
 *
 * Value: 8
 * Usage: Cultural analysis and diplomatic planning
 */
#define DI_RACE		8

/*
 * Terrain and Environmental Display Modes
 */

/*
 * DI_VEGE - Vegetation Display Mode
 *
 * Displays vegetation types and terrain characteristics.
 * Shows forests, grasslands, deserts, and other terrain features.
 * Important for movement planning and resource identification.
 *
 * Value: 9
 * Usage: Terrain analysis and movement planning
 */
#define DI_VEGE		9

/*
 * DI_WOOD - Wood Resource Display Mode
 *
 * Displays wood resources and forestry potential.
 * Shows timber availability for construction and shipbuilding.
 * Essential for naval development and construction projects.
 *
 * Value: 10
 * Usage: Construction planning and naval development
 */
#define DI_WOOD		10

/*
 * Advanced Strategic Display Modes
 */

/*
 * DI_YDESG - Yield Designation Display Mode
 *
 * Displays productivity and yield information for sectors.
 * Shows economic output and production efficiency.
 * Critical for optimizing territorial development.
 *
 * Value: 11
 * Usage: Economic optimization and productivity analysis
 */
#define DI_YDESG	11

/*
 * Military Movement Display Modes
 */

/*
 * DI_AMOVE - Army Movement Display Mode
 *
 * Displays army movement capabilities and restrictions.
 * Shows where land forces can move and movement costs.
 * Essential for military planning and tactical positioning.
 *
 * Value: 12
 * Usage: Land military planning and troop deployment
 */
#define DI_AMOVE	12

/*
 * DI_DEFENSE - Defense Display Mode
 *
 * Displays defensive capabilities and fortification levels.
 * Shows sector defense values and military fortifications.
 * Important for defensive planning and siege warfare.
 *
 * Value: 13
 * Usage: Defensive planning and fortification management
 */
#define DI_DEFENSE	13

/*
 * DI_FMOVE - Fleet Movement Display Mode
 *
 * Displays naval movement capabilities and sea lanes.
 * Shows where fleets can move and naval accessibility.
 * Critical for naval operations and maritime strategy.
 *
 * Value: 14
 * Usage: Naval planning and maritime operations
 */
#define DI_FMOVE	14

/*
 * DI_MAGIC - Magic Display Mode
 *
 * Displays magical resources, ley lines, or supernatural elements.
 * Shows mystical aspects of the game world if applicable.
 * Relevant for fantasy/magical gameplay elements.
 *
 * Value: 15
 * Usage: Magical resource management and supernatural planning
 */
#define DI_MAGIC	15

/*
 * DI_NMOVE - Naval Movement Display Mode
 *
 * Alternative naval movement display with different parameters.
 * May show different naval movement aspects than DI_FMOVE.
 * Provides additional naval strategic information.
 *
 * Value: 16
 * Usage: Advanced naval planning and maritime strategy
 */
#define DI_NMOVE	16

/*
 * Economic and Trade Display Modes
 */

/*
 * DI_TGDESGS - Trade Goods Designations Display Mode
 *
 * Displays trade goods and commercial designations.
 * Shows commercial sectors and trade route potential.
 * Important for economic development and trade planning.
 *
 * Value: 17
 * Usage: Trade route planning and commercial development
 */
#define DI_TGDESGS	17

/*
 * DI_VALUES - Values Display Mode
 *
 * Displays economic values and sector worth.
 * Shows monetary value or strategic importance of sectors.
 * Critical for investment decisions and prioritization.
 *
 * Value: 18
 * Usage: Economic analysis and investment planning
 */
#define DI_VALUES	18

/*
 * DI_WEIGHTS - Weights Display Mode
 *
 * Displays weight-related information for logistics.
 * Shows carrying capacity or logistical burden information.
 * Important for supply chain and transportation planning.
 *
 * Value: 19
 * Usage: Logistics planning and supply chain management
 */
#define DI_WEIGHTS	19

/*
 * DI_KEEP - Keep Display Mode
 *
 * Displays fortress and stronghold information.
 * Shows defensive structures and fortified positions.
 * Essential for military defense and strategic control.
 *
 * Value: 20
 * Usage: Fortress management and defensive planning
 */
#define DI_KEEP		20

/*
 * DI_MAXIMUM - Maximum Display Mode Constant
 *
 * Defines the highest valid display mode number.
 * Used for bounds checking and array dimensioning.
 * Must be updated when new display modes are added.
 *
 * Value: 20 (currently matches DI_KEEP)
 * Usage: Array bounds and validation checks
 */
#define DI_MAXIMUM	20

/*
 * Highlighting Mode Constants
 *
 * These constants define tactical overlay highlighting modes that can be
 * applied to the map display to emphasize strategic information. Unlike
 * display modes which control what information is shown, highlighting modes
 * control how that information is visually emphasized.
 *
 * Integration Note: These constants work with the highlighting system
 * defined in hlightX.h for sector-level highlighting control.
 */

/*
 * HI_NONE - No Highlighting
 *
 * Disables all tactical highlighting overlays.
 * Displays information without additional visual emphasis.
 * Provides clean, uncluttered map presentation.
 *
 * Value: 0
 * Usage: Default state, clean display mode
 */
#define HI_NONE		0

/*
 * Diplomatic Relationship Highlighting
 */

/*
 * HI_ALLIED - Allied Territory Highlighting
 *
 * Highlights sectors controlled by allied nations.
 * Shows friendly territories for diplomatic and strategic planning.
 * Useful for coordinating military operations with allies.
 *
 * Value: 1
 * Usage: Diplomatic analysis and alliance coordination
 */
#define HI_ALLIED	1

/*
 * HI_ENEMY - Enemy Territory Highlighting
 *
 * Highlights sectors controlled by hostile nations.
 * Shows enemy territories for military planning and threat assessment.
 * Critical for identifying targets and defensive priorities.
 *
 * Value: 3
 * Usage: Military planning and threat analysis
 */
#define HI_ENEMY	3

/*
 * HI_NEUTRAL - Neutral Territory Highlighting
 *
 * Highlights sectors controlled by neutral nations.
 * Shows non-aligned territories for diplomatic opportunities.
 * Important for expansion planning and diplomatic strategy.
 *
 * Value: 5
 * Usage: Diplomatic strategy and expansion planning
 */
#define HI_NEUTRAL	5

/*
 * HI_OWN - Own Territory Highlighting
 *
 * Highlights sectors controlled by the player's nation.
 * Shows domestic territories for internal management.
 * Essential for administrative and development planning.
 *
 * Value: 6
 * Usage: Domestic territory management and development
 */
#define HI_OWN		6

/*
 * Strategic Infrastructure Highlighting
 */

/*
 * HI_MAJDESG - Major Designation Highlighting
 *
 * Highlights sectors with major strategic designations.
 * Shows important cities, capitals, and major strategic points.
 * Critical for identifying key objectives and control points.
 *
 * Value: 2
 * Usage: Strategic planning and objective identification
 */
#define HI_MAJDESG	2

/*
 * HI_MINDESG - Minor Designation Highlighting
 *
 * Highlights sectors with minor strategic designations.
 * Shows secondary settlements and minor strategic points.
 * Useful for detailed tactical planning and resource management.
 *
 * Value: 4
 * Usage: Tactical planning and resource management
 */
#define HI_MINDESG	4

/*
 * HI_KEEP - Fortress Highlighting
 *
 * Highlights fortified positions and defensive structures.
 * Shows castles, fortresses, and other defensive installations.
 * Essential for defensive planning and siege warfare.
 *
 * Value: 16
 * Usage: Defensive planning and siege strategy
 */
#define HI_KEEP		16

/*
 * Military and Operational Highlighting
 */

/*
 * HI_RANGE - Range Analysis Highlighting
 *
 * Highlights sectors within operational range of selected units.
 * Shows movement range, attack range, or supply range.
 * Critical for tactical positioning and operational planning.
 *
 * Value: 7
 * Usage: Tactical positioning and range analysis
 */
#define HI_RANGE	7

/*
 * HI_SCOUT - Reconnaissance Highlighting
 *
 * Highlights sectors for scouting and reconnaissance operations.
 * Shows areas of interest for intelligence gathering.
 * Important for information warfare and exploration.
 *
 * Value: 8
 * Usage: Intelligence gathering and reconnaissance planning
 */
#define HI_SCOUT	8

/*
 * Unit Type Highlighting
 */

/*
 * HI_UNITS - General Unit Highlighting
 *
 * Highlights sectors containing military units.
 * Shows military presence regardless of unit type.
 * Useful for overall force distribution analysis.
 *
 * Value: 9
 * Usage: Force distribution analysis and military overview
 */
#define HI_UNITS	9

/*
 * HI_YUNITS - Your Units Highlighting
 *
 * Highlights sectors containing the player's military units.
 * Shows domestic force deployment and positioning.
 * Essential for command and control operations.
 *
 * Value: 10
 * Usage: Force management and command operations
 */
#define HI_YUNITS	10

/*
 * HI_MOVEABLE - Moveable Units Highlighting
 *
 * Highlights sectors containing units that can currently move.
 * Shows available forces for immediate deployment.
 * Critical for real-time tactical decisions.
 *
 * Value: 11
 * Usage: Real-time tactical planning and unit activation
 */
#define HI_MOVEABLE	11

/*
 * Regional and Logistical Highlighting
 */

/*
 * HI_REGION - Regional Highlighting
 *
 * Highlights specific geographical or administrative regions.
 * Shows regional boundaries and administrative divisions.
 * Useful for regional management and planning.
 *
 * Value: 12
 * Usage: Regional management and administrative planning
 */
#define HI_REGION	12

/*
 * HI_SUPPORTED - Logistical Support Highlighting
 *
 * Highlights sectors with adequate logistical support.
 * Shows well-supplied areas and supply line coverage.
 * Critical for sustained military operations.
 *
 * Value: 13
 * Usage: Logistics planning and supply line management
 */
#define HI_SUPPORTED	13

/*
 * HI_UNSUPPORTED - Unsupported Areas Highlighting
 *
 * Highlights sectors lacking adequate logistical support.
 * Shows vulnerable areas and supply line gaps.
 * Important for identifying logistical vulnerabilities.
 *
 * Value: 15
 * Usage: Vulnerability analysis and supply planning
 */
#define HI_UNSUPPORTED	15

/*
 * Economic Highlighting
 */

/*
 * HI_TGOODS - Trade Goods Highlighting
 *
 * Highlights sectors containing valuable trade goods.
 * Shows economic resources and trade opportunities.
 * Essential for economic strategy and trade planning.
 *
 * Value: 14
 * Usage: Economic planning and trade route development
 */
#define HI_TGOODS	14

/*
 * HI_MAXIMUM - Maximum Highlighting Mode Constant
 *
 * Defines the highest valid highlighting mode number.
 * Used for bounds checking and array dimensioning.
 * Must be updated when new highlighting modes are added.
 *
 * Value: 16 (currently matches HI_KEEP)
 * Usage: Array bounds and validation checks
 */
#define HI_MAXIMUM	16

/*
 * Hexagonal Map Positioning Mode Constants
 *
 * These constants define how the hexagonal map display should be positioned
 * and oriented. The game uses a hexagonal grid system for tactical map
 * representation, and these modes control the display positioning relative
 * to the screen or viewport.
 */

/*
 * HXPOS_MINIMAL - Minimal Positioning Mode
 *
 * Uses minimal screen space for map display positioning.
 * Compact layout that maximizes map area relative to UI elements.
 * Useful for small screens or when maximum map visibility is needed.
 *
 * Value: -1
 * Usage: Compact display layouts, small screens
 */
#define HXPOS_MINIMAL	-1

/*
 * HXPOS_LOWLEFT - Lower Left Positioning Mode
 *
 * Positions the hexagonal map with emphasis on the lower-left quadrant.
 * Traditional map positioning with south-west orientation priority.
 * Follows conventional cartographic presentation standards.
 *
 * Value: 0
 * Usage: Traditional map display, strategic overview
 */
#define HXPOS_LOWLEFT	0

/*
 * HXPOS_LOWRIGHT - Lower Right Positioning Mode
 *
 * Positions the hexagonal map with emphasis on the lower-right quadrant.
 * South-east orientation priority for different strategic perspectives.
 * Alternative positioning for varied tactical viewpoints.
 *
 * Value: 1
 * Usage: Alternative strategic perspectives, tactical planning
 */
#define HXPOS_LOWRIGHT	1

/*
 * HXPOS_UPLEFT - Upper Left Positioning Mode
 *
 * Positions the hexagonal map with emphasis on the upper-left quadrant.
 * North-west orientation priority for specific strategic needs.
 * Useful for campaigns focusing on northern or western territories.
 *
 * Value: 2
 * Usage: Northern/western campaign focus, alternative viewpoints
 */
#define HXPOS_UPLEFT	2

/*
 * HXPOS_UPRIGHT - Upper Right Positioning Mode
 *
 * Positions the hexagonal map with emphasis on the upper-right quadrant.
 * North-east orientation priority for comprehensive map coverage.
 * Provides different strategic perspective for tactical analysis.
 *
 * Value: 3
 * Usage: Northern/eastern campaign focus, comprehensive coverage
 */
#define HXPOS_UPRIGHT	3

/*
 * Map Zoom Level Constants
 *
 * These constants define different levels of map magnification and detail.
 * Each zoom level provides a different balance between area coverage and
 * detail visibility, allowing players to choose the appropriate level
 * for their current strategic or tactical needs.
 *
 * Integration Note: ZOOM_NUMBER is defined in dataX.h and specifies
 * the total number of available zoom levels in the system.
 */

/*
 * ZOOM_DETAIL - Detailed Zoom Level
 *
 * Highest magnification level showing maximum detail.
 * Displays individual sector details, unit specifics, and fine terrain.
 * Best for tactical planning and precise unit positioning.
 * Limited area coverage but maximum information density.
 *
 * Value: 0
 * Usage: Tactical planning, detailed unit management, precise positioning
 */
#define ZOOM_DETAIL	0

/*
 * ZOOM_MEDIUM - Medium Zoom Level  
 *
 * Balanced magnification level providing moderate detail and coverage.
 * Shows regional information while maintaining readability.
 * Good compromise between strategic overview and tactical detail.
 * Suitable for operational-level planning and regional management.
 *
 * Value: 1
 * Usage: Operational planning, regional management, balanced view
 */
#define ZOOM_MEDIUM	1

/*
 * ZOOM_LARGE - Large Area Zoom Level
 *
 * Lowest magnification level showing maximum area coverage.
 * Displays strategic overview with minimal detail per sector.
 * Best for grand strategy planning and empire-wide perspectives.
 * Maximum area coverage but reduced information density.
 *
 * Value: 2
 * Usage: Strategic overview, empire management, grand strategy planning
 */
#define ZOOM_LARGE	2

/*
 * Note: ZOOM_NUMBER Constant
 *
 * The total number of available zoom levels is defined in dataX.h as
 * ZOOM_NUMBER. This constant should be used for array dimensioning
 * and bounds checking when working with zoom level arrays or functions.
 *
 * Reference: dataX.h for ZOOM_NUMBER definition
 * Usage: Array bounds, validation, iteration limits
 */
/* ZOOM_NUMBER set in dataX.h */
