/*
 * activeX.h - Nation Activity Status and Alignment System
 *
 * This header defines the comprehensive nation activity classification system
 * that governs all aspects of nation behavior, AI control, and gameplay
 * mechanics. It provides a unified framework for distinguishing between player
 * nations, computer-controlled nations, deceased nations, and various types
 * of AI-controlled faction behaviors.
 *
 * The activity system encompasses multiple layers of classification:
 * - Basic activity levels: from inactive to highly aggressive behaviors
 * - Nation types: player-controlled vs. computer-controlled entities
 * - Alignment system: good, neutral, and evil moral orientations
 * - Faction types: lizards, peasants, savages, nomads, pirates with distinct behaviors
 * - Activity status: active, deceased, or inactive nation states
 *
 * Key Components:
 *   - Activity level constants defining behavioral intensity (0-5 scale)
 *   - Alignment constants for moral orientation classification
 *   - Nation status values combining activity, alignment, and faction type
 *   - Comprehensive macro system for efficient nation classification
 *   - Mathematical encoding supporting complex behavioral combinations
 *
 * Integration Notes:
 *   - Used by: AI systems, turn processing, diplomacy, combat, UI display
 *   - Depends on: header.h (basic definitions), game logic systems
 *   - Provides: Nation classification, behavior determination, status queries
 *
 * Modernization Considerations:
 *   - Consider enum types for better type safety and readability
 *   - Add validation for activity value ranges and combinations
 *   - Consider bitfield structures for more efficient storage
 *   - Implement bounds checking for mathematical operations
 */

/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* ============================================================================
 * BASIC ACTIVITY LEVEL DEFINITIONS - Behavioral Intensity Classification
 * ============================================================================
 * Purpose: Define fundamental activity levels governing nation behavior intensity
 * Usage: Base activity classification from passive to highly aggressive
 * Notes: Forms the foundation for more complex activity status encoding
 */

/*
 * ACT_NOMOVE - No movement or aggressive action (passive behavior)
 *
 * Represents completely passive behavior with no movement or aggressive
 * actions. Nations with this activity level will not initiate attacks,
 * expand territory, or engage in active diplomacy.
 *
 * Value: 0 (baseline activity level)
 * Behavior: Completely passive, defensive only
 * Usage: Peaceful nations, inactive players, defensive positions
 * AI Impact: No autonomous movement or aggressive actions
 */
#define ACT_NOMOVE	0

/*
 * ACT_STATIC - Static defensive position with limited activity
 *
 * Represents defensive behavior with minimal movement but some limited
 * activity. Nations may respond to threats but will not initiate
 * expansionist or aggressive actions against neutral parties.
 *
 * Value: 1 (minimal activity level)
 * Behavior: Defensive responses, limited local activity
 * Usage: Defensive nations, cautious players, fortified positions
 * AI Impact: Responds to threats but doesn't initiate expansion
 */
#define ACT_STATIC	1

/*
 * ACT_ENFORCE - Enforcement and patrol activities within territory
 *
 * Represents moderate activity focused on maintaining control within
 * existing territory. Nations will patrol borders, suppress rebellion,
 * and maintain order but limit expansion activities.
 *
 * Value: 2 (moderate defensive activity)
 * Behavior: Territory maintenance, border patrol, internal security
 * Usage: Stable nations, border security, maintaining control
 * AI Impact: Active territory management and security enforcement
 */
#define ACT_ENFORCE	2

/*
 * ACT_OVERT - Open aggressive activity and expansion
 *
 * Represents openly aggressive behavior with active expansion efforts.
 * Nations will engage in territorial expansion, diplomatic pressure,
 * and overt military actions against neighboring territories.
 *
 * Value: 3 (moderate aggressive activity)
 * Behavior: Active expansion, diplomatic pressure, territorial growth
 * Usage: Expansionist nations, aggressive players, territorial conquest
 * AI Impact: Active expansion and diplomatic manipulation
 */
#define ACT_OVERT	3

/*
 * ACT_MOBILE - High mobility and active military operations
 *
 * Represents highly active military behavior with extensive movement
 * and coordinated operations. Nations engage in complex military
 * campaigns, multi-front operations, and sophisticated tactical maneuvers.
 *
 * Value: 4 (high military activity)
 * Behavior: Coordinated campaigns, multi-front operations, tactical mobility
 * Usage: Military powers, advanced players, complex warfare
 * AI Impact: Sophisticated military coordination and strategic planning
 */
#define ACT_MOBILE	4

/*
 * ACT_KILLER - Maximum aggression and elimination-focused behavior
 *
 * Represents the highest level of aggressive behavior focused on
 * elimination of enemies and total dominance. Nations prioritize
 * destruction of opponents over territorial or economic gains.
 *
 * Value: 5 (maximum aggressive activity)
 * Behavior: Elimination-focused, total warfare, dominance seeking
 * Usage: Warlike nations, elimination strategies, total conquest
 * AI Impact: Ruthless elimination tactics and overwhelming aggression
 */
#define ACT_KILLER	5

/*
 * ACT_NUMBER - Total count of basic activity levels
 *
 * Provides the total number of basic activity levels for array sizing,
 * validation, and iteration purposes throughout the game systems.
 *
 * Value: 6 (count of activity levels 0-5)
 * Usage: Array sizing, validation bounds, iteration limits
 * Purpose: System configuration and bounds checking
 */
#define ACT_NUMBER	6

/*
 * ACT_PCMODULUS - Player character activity range modulus
 *
 * Defines the modulus value that separates player-controlled nations
 * from computer-controlled nations in the activity encoding system.
 * Values below this threshold represent player nations.
 *
 * Value: 24 (separation threshold for PC vs NPC)
 * Usage: Player/computer distinction, activity range calculation
 * Purpose: Mathematical encoding of nation control type
 */
#define ACT_PCMODULUS	24

/*
 * ACT_MAXIMUM - Maximum valid activity status value
 *
 * Defines the highest valid activity status value in the entire system,
 * providing bounds checking and validation capabilities for all
 * activity-related calculations and assignments.
 *
 * Value: 47 (maximum valid activity status)
 * Usage: Bounds checking, validation, system limits
 * Purpose: System integrity and input validation
 */
#define ACT_MAXIMUM	47

/* ============================================================================
 * MORAL ALIGNMENT SYSTEM - Ethical Orientation Classification
 * ============================================================================
 * Purpose: Define moral alignment categories for diplomatic and behavior systems
 * Usage: Alignment-based diplomacy, AI behavior modification, conflict resolution
 * Notes: Affects diplomatic relations, AI decision making, and player interactions
 */

/*
 * ALIGN_GOOD - Good moral alignment (altruistic behavior)
 *
 * Represents good moral alignment with altruistic tendencies, cooperative
 * behavior, and preference for peaceful solutions. Good nations favor
 * diplomacy, trade, and mutual benefit over conquest and exploitation.
 *
 * Value: 1 (good alignment identifier)
 * Behavior: Cooperative, altruistic, diplomatic preference
 * Diplomacy: Favors alliances, trade agreements, peaceful resolution
 * AI Impact: Diplomatic solutions preferred over military action
 */
#define ALIGN_GOOD	1

/*
 * ALIGN_NEUTRAL - Neutral moral alignment (pragmatic behavior)
 *
 * Represents neutral moral alignment with pragmatic decision-making
 * based on practical considerations rather than moral imperatives.
 * Neutral nations adapt their behavior based on circumstances and advantage.
 *
 * Value: 2 (neutral alignment identifier)
 * Behavior: Pragmatic, adaptable, situational decision-making
 * Diplomacy: Opportunistic alliances, practical agreements, flexible relations
 * AI Impact: Decision-making based on practical advantage and opportunity
 */
#define ALIGN_NEUTRAL	2

/*
 * ALIGN_EVIL - Evil moral alignment (exploitative behavior)
 *
 * Represents evil moral alignment with exploitative tendencies, aggressive
 * behavior, and preference for domination over cooperation. Evil nations
 * favor conquest, manipulation, and exploitation of weakness.
 *
 * Value: 3 (evil alignment identifier)
 * Behavior: Exploitative, aggressive, domination-focused
 * Diplomacy: Manipulative alliances, exploitative agreements, betrayal prone
 * AI Impact: Aggressive expansion and exploitation of opportunities
 */
#define ALIGN_EVIL	3

/* ============================================================================
 * NATION STATUS VALUES - Complete Activity and Type Classification
 * ============================================================================
 * Purpose: Comprehensive nation status encoding combining activity, alignment, and type
 * Usage: Complete nation classification for AI behavior, UI display, and game logic
 * Notes: Mathematical encoding allows efficient extraction of multiple classification dimensions
 */

/*
 * INACTIVE - Completely inactive nation status
 *
 * Represents nations that are completely inactive with no AI control,
 * player control, or autonomous behavior. Used for empty nation slots,
 * placeholder entries, or nations awaiting activation.
 *
 * Value: 0 (complete inactivity)
 * Status: No control, no behavior, placeholder
 * Usage: Empty slots, unassigned nations, system placeholders
 * Behavior: No autonomous actions or responses
 */
#define INACTIVE	0

/* Dead Nation Status Values - Deceased computer-controlled factions */

/*
 * DEAD_LIZARD - Deceased lizard faction nation
 *
 * Represents a lizard faction nation that has been eliminated but may
 * still have remnant forces or influence in the game world. Dead nations
 * may have limited automated cleanup or final actions.
 *
 * Value: 4 (deceased lizard faction)
 * Status: Eliminated lizard nation with potential remnant activity
 * Faction: Lizard people - typically swamp/marsh dwelling civilization
 * Behavior: Limited automated cleanup or final defensive actions
 */
#define DEAD_LIZARD	4

/*
 * DEAD_PEASANT - Deceased peasant faction nation
 *
 * Represents a peasant faction nation that has been eliminated but may
 * still have remnant agricultural operations or population centers that
 * require automated management during the cleanup process.
 *
 * Value: 8 (deceased peasant faction)
 * Status: Eliminated peasant nation with potential remnant activity
 * Faction: Peasant communities - agricultural and settlement-based civilization
 * Behavior: Limited automated resource management and population cleanup
 */
#define DEAD_PEASANT	8

/*
 * DEAD_SAVAGE - Deceased savage faction nation
 *
 * Represents a savage faction nation that has been eliminated but may
 * still have remnant tribal warriors or territorial claims that require
 * automated resolution and cleanup processing.
 *
 * Value: 12 (deceased savage faction)
 * Status: Eliminated savage nation with potential remnant activity
 * Faction: Savage tribes - nomadic and warrior-based civilization
 * Behavior: Limited automated tribal territory cleanup and warrior disbanding
 */
#define DEAD_SAVAGE	12

/*
 * DEAD_NOMAD - Deceased nomad faction nation
 *
 * Represents a nomad faction nation that has been eliminated but may
 * still have remnant caravans or trade routes that require automated
 * management and dissolution procedures.
 *
 * Value: 16 (deceased nomad faction)
 * Status: Eliminated nomad nation with potential remnant activity
 * Faction: Nomadic traders - mobile and commerce-based civilization
 * Behavior: Limited automated caravan management and trade route cleanup
 */
#define DEAD_NOMAD	16

/*
 * DEAD_PIRATE - Deceased pirate faction nation
 *
 * Represents a pirate faction nation that has been eliminated but may
 * still have remnant ships or coastal strongholds that require automated
 * management and naval cleanup operations.
 *
 * Value: 20 (deceased pirate faction)
 * Status: Eliminated pirate nation with potential remnant activity
 * Faction: Pirate fleets - naval and raiding-based civilization
 * Behavior: Limited automated fleet management and coastal cleanup
 */
#define DEAD_PIRATE	20

/* Active NPC Status Values - Living computer-controlled factions */

/*
 * NPC_INACTIVE - Inactive computer-controlled nation placeholder
 *
 * Represents a computer-controlled nation slot that is currently inactive
 * but reserved for potential NPC spawning or activation. Distinguished
 * from completely inactive slots by being in the NPC range.
 *
 * Value: 24 (inactive NPC placeholder)
 * Status: Reserved for NPC activation, currently inactive
 * Control: Computer-controlled when activated
 * Behavior: No current behavior, awaiting activation
 */
#define NPC_INACTIVE	24

/*
 * NPC_LIZARD - Active lizard faction computer-controlled nation
 *
 * Represents an active lizard faction nation under computer control.
 * Lizard nations typically focus on swamp and marsh territories with
 * defensive strategies and territorial control emphasis.
 *
 * Value: 28 (active lizard NPC)
 * Status: Active computer-controlled lizard nation
 * Faction: Lizard people - swamp/marsh dwelling with defensive focus
 * Behavior: Territorial defense, swamp expansion, cautious diplomacy
 */
#define NPC_LIZARD	28

/*
 * NPC_PEASANT - Active peasant faction computer-controlled nation
 *
 * Represents an active peasant faction nation under computer control.
 * Peasant nations emphasize agricultural development, peaceful growth,
 * and defensive strategies with focus on economic development.
 *
 * Value: 32 (active peasant NPC)
 * Status: Active computer-controlled peasant nation
 * Faction: Peasant communities - agricultural focus with peaceful growth
 * Behavior: Economic development, peaceful expansion, defensive diplomacy
 */
#define NPC_PEASANT	32

/*
 * NPC_SAVAGE - Active savage faction computer-controlled nation
 *
 * Represents an active savage faction nation under computer control.
 * Savage nations emphasize warrior culture, tribal organization, and
 * aggressive expansion through military conquest and raiding.
 *
 * Value: 36 (active savage NPC)
 * Status: Active computer-controlled savage nation
 * Faction: Savage tribes - warrior culture with aggressive expansion
 * Behavior: Military conquest, raiding, tribal warfare, aggressive diplomacy
 */
#define NPC_SAVAGE	36

/*
 * NPC_NOMAD - Active nomad faction computer-controlled nation
 *
 * Represents an active nomad faction nation under computer control.
 * Nomad nations emphasize trade, mobility, and economic strategies with
 * focus on commerce and diplomatic relations for mutual benefit.
 *
 * Value: 40 (active nomad NPC)
 * Status: Active computer-controlled nomad nation
 * Faction: Nomadic traders - mobile commerce with diplomatic focus
 * Behavior: Trade expansion, diplomatic relations, economic strategies
 */
#define NPC_NOMAD	40

/*
 * NPC_PIRATE - Active pirate faction computer-controlled nation
 *
 * Represents an active pirate faction nation under computer control.
 * Pirate nations emphasize naval operations, raiding, and coastal
 * control with focus on opportunistic expansion and resource acquisition.
 *
 * Value: 44 (active pirate NPC)
 * Status: Active computer-controlled pirate nation
 * Faction: Pirate fleets - naval focus with raiding and opportunistic expansion
 * Behavior: Naval raiding, coastal expansion, opportunistic attacks
 */
#define NPC_PIRATE	44

/* ============================================================================
 * NATION CLASSIFICATION MACROS - Efficient Status Determination
 * ============================================================================
 * Purpose: Provide efficient mathematical extraction of nation characteristics
 * Usage: Fast nation classification for AI, UI, and game logic systems
 * Notes: Mathematical encoding enables multiple properties from single value
 */

/*
 * n_alignment - Extract moral alignment from nation activity status
 *
 * Extracts the moral alignment (good, neutral, evil) from the encoded
 * nation activity status value using modular arithmetic. Alignment
 * affects diplomatic relations and AI behavior patterns.
 *
 * Formula: (status % 4) yields 0=inactive, 1=good, 2=neutral, 3=evil
 * Usage: Diplomatic calculations, AI behavior modification, conflict resolution
 * Returns: ALIGN_GOOD, ALIGN_NEUTRAL, ALIGN_EVIL, or 0 for inactive
 */
#define n_alignment(x)	((x) % 4)

/*
 * n_aggression - Extract aggression level from nation activity status
 *
 * Extracts the aggression level (0-5 scale) from the encoded nation
 * activity status value. Aggression level determines autonomous behavior
 * intensity and military action likelihood.
 *
 * Formula: ((status % 24) / 4) yields 0-5 aggression scale
 * Usage: AI behavior intensity, military action probability, expansion rate
 * Returns: ACT_NOMOVE through ACT_KILLER (0-5 scale)
 */
#define n_aggression(x)	(((x) % ACT_PCMODULUS) / 4)

/*
 * n_isactive - Test if nation has any active status
 *
 * Determines whether a nation has any form of active status, including
 * player control, computer control, or remnant activity from deceased
 * nations requiring cleanup processing.
 *
 * Usage: General activity checking, slot allocation, system processing
 * Returns: Non-zero for any active status, 0 for completely inactive
 */
#define n_isactive(x)	((x) != INACTIVE)

/*
 * n_notactive - Test if nation is completely inactive
 *
 * Determines whether a nation is completely inactive with no player
 * control, computer control, or remnant activity. Used for empty
 * slot detection and system resource management.
 *
 * Usage: Empty slot detection, resource cleanup, system optimization
 * Returns: Non-zero for completely inactive, 0 for any active status
 */
#define n_notactive(x)	((x) == INACTIVE)

/*
 * n_ismonster - Test if nation represents monster/creature forces
 *
 * Determines whether a nation represents monster or creature forces
 * rather than organized civilization. Monsters have different behavioral
 * patterns and diplomatic limitations compared to standard nations.
 *
 * Usage: AI behavior selection, diplomatic restrictions, special rules
 * Returns: Non-zero for monster forces, 0 for organized civilizations
 */
#define n_ismonster(x)	(n_alignment(x) == INACTIVE)

/* Faction Type Identification Macros */

/*
 * n_islizard - Test if nation belongs to lizard faction
 *
 * Determines whether a nation belongs to the lizard faction, including
 * both active and deceased lizard nations. Lizard factions have distinct
 * behavioral patterns and territorial preferences.
 *
 * Usage: Faction-specific AI behavior, diplomatic relations, territorial preferences
 * Returns: Non-zero for lizard faction nations (active or deceased)
 */
#define n_islizard(x)	(((x) == NPC_LIZARD) || ((x) == DEAD_LIZARD))

/*
 * n_ispeasant - Test if nation belongs to peasant faction
 *
 * Determines whether a nation belongs to the peasant faction, including
 * both active and deceased peasant nations. Peasant factions emphasize
 * agricultural development and peaceful expansion strategies.
 *
 * Usage: Faction-specific AI behavior, economic strategies, diplomatic preferences
 * Returns: Non-zero for peasant faction nations (active or deceased)
 */
#define n_ispeasant(x)	(((x) == NPC_PEASANT) || ((x) == DEAD_PEASANT))

/*
 * n_issavage - Test if nation belongs to savage faction
 *
 * Determines whether a nation belongs to the savage faction, including
 * both active and deceased savage nations. Savage factions emphasize
 * warrior culture and aggressive military expansion.
 *
 * Usage: Faction-specific AI behavior, military strategies, aggressive diplomacy
 * Returns: Non-zero for savage faction nations (active or deceased)
 */
#define n_issavage(x)	(((x) == NPC_SAVAGE) || ((x) == DEAD_SAVAGE))

/*
 * n_isnomad - Test if nation belongs to nomad faction
 *
 * Determines whether a nation belongs to the nomad faction, including
 * both active and deceased nomad nations. Nomad factions emphasize
 * trade, mobility, and economic diplomatic strategies.
 *
 * Usage: Faction-specific AI behavior, trade strategies, economic diplomacy
 * Returns: Non-zero for nomad faction nations (active or deceased)
 */
#define n_isnomad(x)	(((x) == NPC_NOMAD) || ((x) == DEAD_NOMAD))

/*
 * n_ispirate - Test if nation belongs to pirate faction
 *
 * Determines whether a nation belongs to the pirate faction, including
 * both active and deceased pirate nations. Pirate factions emphasize
 * naval operations, raiding, and opportunistic expansion.
 *
 * Usage: Faction-specific AI behavior, naval strategies, raiding patterns
 * Returns: Non-zero for pirate faction nations (active or deceased)
 */
#define n_ispirate(x)	(((x) == NPC_PIRATE) || ((x) == DEAD_PIRATE))

/* Control and Status Classification Macros */

/*
 * n_isntn - Test if nation is an organized nation
 *
 * Determines whether a nation represents an organized nation (rather than
 * monsters or inactive slots) capable of standard diplomatic relations
 * and civilization-level activities.
 *
 * Usage: Diplomatic eligibility, civilization-level interactions, standard AI
 * Returns: Non-zero for organized nations, 0 for monsters/inactive
 */
#define n_isntn(x)	(n_alignment(x) != INACTIVE)

/*
 * n_ispc - Test if nation is player-controlled
 *
 * Determines whether a nation is under player control rather than
 * computer AI control. Player nations require different processing
 * and have different capabilities and restrictions.
 *
 * Usage: Input processing, UI display, turn processing, save/load
 * Returns: Non-zero for player-controlled nations, 0 for computer/inactive
 */
#define n_ispc(x)	(((x) < ACT_PCMODULUS) && ((x) != INACTIVE))

/*
 * n_isnpc - Test if nation is computer-controlled
 *
 * Determines whether a nation is under computer AI control. Computer
 * nations require AI processing, automated decision-making, and
 * different user interface treatment.
 *
 * Usage: AI processing, automated decisions, computer turn processing
 * Returns: Non-zero for computer-controlled nations, 0 for player/inactive
 */
#define n_isnpc(x)	(n_isntn(x) && ((x) > ACT_PCMODULUS))

/* Alignment-Specific Classification Macros */

/*
 * n_isgood - Test if nation has good moral alignment
 *
 * Determines whether a nation has good moral alignment, affecting
 * diplomatic relations, AI behavior patterns, and interaction
 * possibilities with other nations and game elements.
 *
 * Usage: Diplomatic calculations, AI behavior modification, moral interactions
 * Returns: Non-zero for good-aligned nations, 0 for other alignments
 */
#define n_isgood(x)	(n_alignment(x) == ALIGN_GOOD)

/*
 * n_isneutral - Test if nation has neutral moral alignment
 *
 * Determines whether a nation has neutral moral alignment, indicating
 * pragmatic decision-making and flexible diplomatic relations based
 * on practical considerations rather than moral imperatives.
 *
 * Usage: Diplomatic flexibility, pragmatic AI decisions, adaptive behavior
 * Returns: Non-zero for neutral-aligned nations, 0 for other alignments
 */
#define n_isneutral(x)	(n_alignment(x) == ALIGN_NEUTRAL)

/*
 * n_isevil - Test if nation has evil moral alignment
 *
 * Determines whether a nation has evil moral alignment, affecting
 * diplomatic relations, AI aggression patterns, and tendency toward
 * exploitative or dominating behavior strategies.
 *
 * Usage: Aggressive AI behavior, exploitative diplomacy, domination strategies
 * Returns: Non-zero for evil-aligned nations, 0 for other alignments
 */
#define n_isevil(x)	(n_alignment(x) == ALIGN_EVIL)

/*
 * n_automove - Test if nation has autonomous movement capability
 *
 * Determines whether a nation has autonomous movement and action
 * capabilities, including both active computer-controlled nations
 * and deceased nations with remnant cleanup activities.
 *
 * Usage: Autonomous processing, AI turn execution, cleanup activities
 * Returns: Non-zero for nations with autonomous capability, 0 for inactive/player
 */
#define n_automove(x)	((x) >= DEAD_LIZARD)
