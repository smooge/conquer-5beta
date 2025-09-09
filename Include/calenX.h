/*
 * calenX.h - Calendar System Definitions and Time Management
 *
 * Complete calendar framework providing game time representation, seasonal
 * calculations, and temporal formatting for the Conquer game system. This
 * header defines the temporal infrastructure used throughout the game for
 * tracking game progression, seasonal effects, and time-based mechanics.
 *
 * Key Components:
 *   - Time conversion macros (year, month, season calculations)
 *   - Season identification constants and classification system
 *   - String formatting for temporal display and user interface
 *   - Calendar arithmetic for game progression and event timing
 *
 * Integration Notes:
 *   - Used by: Game engine, UI display, event scheduling, seasonal effects
 *   - Depends on: String arrays (monthstr[], seasonstr[])
 *   - Provides: Complete temporal calculation and formatting framework
 *
 * Modernization Considerations:
 *   - Replace macro arithmetic with inline functions for type safety
 *   - Add bounds checking for calendar calculations and array access
 *   - Consider struct-based time representation for clarity
 *   - Add const qualifiers to string arrays for read-only data
 *
 * Copyright (c) 1992 by Ed Barlow and Adam Bryant
 * Please see the copyright notice located in the header.h file.
 */

/* ============================================================================
 * TEMPORAL CALCULATION MACROS
 * ============================================================================
 * Purpose: Core time conversion and calendar arithmetic for game progression
 * Usage: Year/month/season calculations from game turn numbers
 * Notes: 12-month year system with 4 seasons (3 months each)
 */

/*
 * YEAR(x) - Calculate game year from turn number
 *
 * Converts a game turn number into the corresponding game year using
 * a 12-month calendar system. The calculation includes an 11-turn offset
 * to handle year boundary transitions and ensure proper year numbering.
 *
 * Parameters: x - Game turn number (1-based counting)
 * Returns: Game year number (integer, 1-based)
 * Formula: (turn + 11) / 12 using integer division
 * 
 * Usage Examples:
 *   YEAR(1) = 1   (first turn of first year)
 *   YEAR(12) = 1  (last turn of first year)
 *   YEAR(13) = 2  (first turn of second year)
 *
 * Notes: The +11 offset ensures proper year boundaries and prevents
 *        year 0 in the game calendar system
 */
#define YEAR(x)		((int)((x+11)/12))

/*
 * MONTH(x) - Calculate month number from turn number
 *
 * Extracts the month component from a game turn number using modular
 * arithmetic. Returns a 0-based month index for internal calculations
 * and array indexing operations.
 *
 * Parameters: x - Game turn number (1-based counting)
 * Returns: Month index (0-11, zero-based for array access)
 * Formula: (turn - 1) % 12 using modular arithmetic
 *
 * Usage Examples:
 *   MONTH(1) = 0   (January, first month)
 *   MONTH(12) = 11 (December, last month)
 *   MONTH(13) = 0  (January of next year)
 *
 * Notes: Zero-based indexing for direct use with monthstr[] array
 *        Subtract 1 to convert from 1-based turns to 0-based months
 */
#define MONTH(x)	((x-1)%12)

/*
 * PMONTH(x) - Get printable month name from turn number
 *
 * Retrieves the human-readable month name string corresponding to
 * the given game turn number. Uses MONTH() macro for index calculation
 * and accesses the monthstr[] array for string lookup.
 *
 * Parameters: x - Game turn number (1-based counting)
 * Returns: Pointer to month name string (char*)
 * Dependencies: monthstr[] array must be properly initialized
 *
 * Usage Examples:
 *   PMONTH(1) = "January"   (or localized equivalent)
 *   PMONTH(6) = "June"      (mid-year)
 *   PMONTH(12) = "December" (end of year)
 *
 * Notes: Direct array access requires monthstr[] to have 12 valid entries
 *        No bounds checking performed - ensure valid turn numbers
 */
#define PMONTH(x)	monthstr[(x-1)%12]

/*
 * SEASON(x) - Calculate season number from turn number
 *
 * Determines the seasonal classification for a given game turn using
 * the month calculation and integer division by 3. Creates a 4-season
 * system with 3 months per season for seasonal game mechanics.
 *
 * Parameters: x - Game turn number (1-based counting)
 * Returns: Season index (0-3, corresponding to seasonal constants)
 * Formula: MONTH(x) / 3 using integer division
 *
 * Season Mapping:
 *   Months 0-2 (Jan-Mar) = Season 0 (Spring)
 *   Months 3-5 (Apr-Jun) = Season 1 (Summer)
 *   Months 6-8 (Jul-Sep) = Season 2 (Fall)
 *   Months 9-11 (Oct-Dec) = Season 3 (Winter)
 *
 * Notes: Used for seasonal effects, weather patterns, resource availability
 */
#define SEASON(x)	(MONTH(x)/3)

/*
 * PSEASON(x) - Get printable season name from turn number
 *
 * Retrieves the human-readable season name string corresponding to
 * the given game turn number. Uses SEASON() macro for index calculation
 * and accesses the seasonstr[] array for string lookup.
 *
 * Parameters: x - Game turn number (1-based counting)
 * Returns: Pointer to season name string (char*)
 * Dependencies: seasonstr[] array must be properly initialized
 *
 * Usage Examples:
 *   PSEASON(2) = "Spring"  (early year)
 *   PSEASON(6) = "Summer"  (mid year)
 *   PSEASON(10) = "Fall"   (late year)
 *   PSEASON(12) = "Winter" (end of year)
 *
 * Notes: Direct array access requires seasonstr[] to have 4 valid entries
 *        Used for UI display and seasonal effect descriptions
 */
#define PSEASON(x)	seasonstr[MONTH(x)/3]

/* ============================================================================
 * SEASONAL CLASSIFICATION CONSTANTS
 * ============================================================================
 * Purpose: Season identification constants for game mechanics and logic
 * Usage: Seasonal effects, weather patterns, resource availability
 * Notes: 4-season system with 3 months per season (quarter-year periods)
 */

/*
 * SPRING_SEASON - Spring season identifier
 *
 * Constant representing the spring season in the game calendar system.
 * Corresponds to the first quarter of the game year (months 0-2) and
 * is used for seasonal mechanics, weather effects, and resource systems.
 *
 * Value: 0 (first season index)
 * Months: January, February, March (months 0-2)
 * Usage: Seasonal effect calculations, weather pattern determination
 * Notes: Growth season for agriculture, moderate weather conditions
 */
#define SPRING_SEASON	0

/*
 * SUMMER_SEASON - Summer season identifier
 *
 * Constant representing the summer season in the game calendar system.
 * Corresponds to the second quarter of the game year (months 3-5) and
 * is used for peak activity periods and optimal resource gathering.
 *
 * Value: 1 (second season index)
 * Months: April, May, June (months 3-5)
 * Usage: Peak production periods, optimal travel conditions
 * Notes: Prime season for campaigns, trade, and resource exploitation
 */
#define SUMMER_SEASON	1

/*
 * FALL_SEASON - Fall/Autumn season identifier
 *
 * Constant representing the fall season in the game calendar system.
 * Corresponds to the third quarter of the game year (months 6-8) and
 * is used for harvest mechanics and preparation for winter conditions.
 *
 * Value: 2 (third season index)
 * Months: July, August, September (months 6-8)
 * Usage: Harvest mechanics, resource stockpiling, winter preparation
 * Notes: Transition season with declining activity and resource gathering
 */
#define FALL_SEASON	2

/*
 * WINTER_SEASON - Winter season identifier
 *
 * Constant representing the winter season in the game calendar system.
 * Corresponds to the fourth quarter of the game year (months 9-11) and
 * is used for harsh weather effects and reduced activity periods.
 *
 * Value: 3 (fourth season index)
 * Months: October, November, December (months 9-11)
 * Usage: Harsh weather effects, reduced movement, survival mechanics
 * Notes: Challenging season with limited resources and movement penalties
 */
#define WINTER_SEASON  	3

/* ============================================================================
 * CALENDAR STRING ARRAYS
 * ============================================================================
 * Purpose: External string arrays for month and season name display
 * Usage: UI formatting, temporal display, user-readable time representation
 * Notes: Must be defined elsewhere with proper initialization
 */

/*
 * seasonstr[] - Array of season name strings for display formatting
 *
 * External array containing human-readable season names used by the
 * PSEASON() macro for temporal display and user interface formatting.
 * Must contain exactly 4 entries corresponding to the season constants.
 *
 * Expected Content:
 *   seasonstr[0] = "Spring" (SPRING_SEASON)
 *   seasonstr[1] = "Summer" (SUMMER_SEASON)
 *   seasonstr[2] = "Fall"   (FALL_SEASON)
 *   seasonstr[3] = "Winter" (WINTER_SEASON)
 *
 * Usage: PSEASON() macro, seasonal effect descriptions, UI display
 * Notes: Implementation must ensure 4 valid string pointers
 *        Consider localization support for international versions
 */

/*
 * monthstr[] - Array of month name strings for display formatting
 *
 * External array containing human-readable month names used by the
 * PMONTH() macro for temporal display and user interface formatting.
 * Must contain exactly 12 entries corresponding to calendar months.
 *
 * Expected Content:
 *   monthstr[0] = "January"   through monthstr[11] = "December"
 *   Standard calendar month names in chronological order
 *
 * Usage: PMONTH() macro, date display, temporal formatting, UI elements
 * Notes: Implementation must ensure 12 valid string pointers
 *        Consider localization support for different calendar systems
 *        Zero-based indexing matches MONTH() macro output
 */
extern char *seasonstr[], *monthstr[];
