/*
 * psmap.h - PostScript Map Generation Configuration Header
 *
 * Header file for the Conquer PostScript map generation utility (conqps).
 * Defines page layout constants, map type classifications, and printer-specific
 * parameters for converting Conquer game maps to PostScript format suitable
 * for printing on various paper sizes and printer types.
 *
 * Author: MaF (d8forma@dtek.chalmers.se)
 * Purpose: Configuration and constants for PostScript map printing system
 * Usage: Include in psmap.c and related PostScript generation utilities
 *
 * Design Notes:
 *   - Supports multiple page sizes: A4, LETTER, and custom OTHER format
 *   - Page dimensions specified in PostScript points (1/72 inch)
 *   - Printer-specific offsets accommodate different printer margins
 *   - Map type constants enable different rendering styles for game data
 *   - Compile-time page size selection via preprocessor definitions
 */

/*
 * Page Size Selection - Compile-time Configuration
 *
 * These preprocessor conditionals set the default page size based on
 * compile-time definitions. Only one should be defined during compilation
 * to select the appropriate default page format for the target region.
 *
 * DEFAULTPAGE Values:
 *   0 = OTHER  - Custom page size (450x700 points)
 *   1 = A4     - International standard (540x820 points) 
 *   2 = LETTER - US standard (575x760 points)
 *
 * Usage: Define ONE of A4, LETTER, or OTHER during compilation
 * Example: gcc -DA4 psmap.c -o psmap
 */
#ifdef OTHER
#define DEFAULTPAGE 0   /* Custom page size for non-standard printers */
#endif

#ifdef A4  
#define DEFAULTPAGE 1   /* International A4 paper size (210 × 297 mm) */
#endif

#ifdef LETTER
#define DEFAULTPAGE 2   /* US Letter paper size (8.5 × 11 inches) */
#endif

/*
 * A4 Page Layout Constants (International Standard)
 *
 * Page dimensions and offsets for A4 paper size (210 × 297 mm).
 * All measurements in PostScript points (1 point = 1/72 inch).
 * Designed for European and international printer compatibility.
 *
 * Layout Calculations:
 *   - Physical A4: 595 × 842 points (8.27 × 11.69 inches)
 *   - Usable area: 540 × 820 points (accounts for printer margins)
 *   - Offsets position content within printable area of typical printers
 */
#define PAGEWIDTH_A4       540  /* Usable page width in PostScript points */
#define PAGEHEIGHT_A4      820  /* Usable page height in PostScript points */
#define XOFFSET_A4         30   /* Horizontal offset from left edge (points) */
#define YOFFSET_A4         10   /* Vertical offset from bottom edge (points) */

/*
 * US Letter Page Layout Constants
 *
 * Page dimensions and offsets for US Letter paper size (8.5 × 11 inches).
 * All measurements in PostScript points (1 point = 1/72 inch).
 * Optimized for North American printer standards and margins.
 *
 * Layout Calculations:
 *   - Physical Letter: 612 × 792 points (8.5 × 11 inches exactly)
 *   - Usable area: 575 × 760 points (accounts for printer margins)
 *   - Smaller offsets take advantage of typical US printer capabilities
 */
#define PAGEWIDTH_LETTER   575  /* Usable page width in PostScript points */
#define PAGEHEIGHT_LETTER  760  /* Usable page height in PostScript points */
#define XOFFSET_LETTER     15   /* Horizontal offset from left edge (points) */
#define YOFFSET_LETTER     10   /* Vertical offset from bottom edge (points) */

/*
 * Custom Page Layout Constants (OTHER format)
 *
 * Page dimensions for custom or non-standard printer configurations.
 * Provides smaller usable area with larger margins for compatibility
 * with older printers or special printing requirements.
 *
 * Conservative Layout:
 *   - Smaller usable area ensures compatibility with various printers
 *   - Larger offsets accommodate printers with larger non-printable margins
 *   - Suitable for dot-matrix, older laser printers, or custom setups
 */
#define PAGEWIDTH_OTHER    450  /* Conservative usable width (points) */
#define PAGEHEIGHT_OTHER   700  /* Conservative usable height (points) */
#define XOFFSET_OTHER      40   /* Large horizontal offset for compatibility */
#define YOFFSET_OTHER      30   /* Large vertical offset for compatibility */

/*
 * Map Layout and Cosmetic Constants
 *
 * Additional spacing and margin controls for map appearance and readability.
 * These values fine-tune the visual presentation of maps on printed pages.
 *
 * XMARGINS: Additional horizontal spacing around map content
 * YMARGINS: Additional vertical spacing around map content
 *
 * Purpose: Ensure maps don't print too close to page edges and maintain
 * professional appearance with adequate white space borders.
 */
#define XMARGINS        30  /* Extra horizontal margins (points) */
#define YMARGINS        50  /* Extra vertical margins (points) */

/*
 * Version and Program Identification
 *
 * Program metadata and user interface strings for the PostScript map
 * generation utility. Used for help text, version reporting, and
 * input file validation.
 */
#define VERSION         "1.0"  /* Program version string */

/*
 * Command Line Usage String
 *
 * Complete usage information displayed when user requests help or
 * provides invalid command line arguments. Documents all available
 * options and their syntax for the psmap utility.
 *
 * Options Documented:
 *   c,g,h,l,n,v,u - Single character flags
 *   p pagesize    - Page size selection
 *   f font        - Font specification
 *   o x,y         - Output offset coordinates
 *   s size        - Size specification
 *   W,L,X,Y n     - Dimension parameters
 *   t title       - Map title text
 *   infile/outfile - Input and output file specifications
 */
#define USAGE           "Usage: %s [cghlnvu] [p pagesize] [f font] [o x,y] [s size]\n\t[W n] [L n] [X n] [Y n] [t title] [infile [outfile]]\n"

/*
 * Input File Validation String
 *
 * String pattern used to validate that input files are genuine Conquer
 * game map files. Prevents processing of incompatible file formats
 * and provides early error detection for invalid inputs.
 */
#define MATCHSTRING     "Conquer Version"  /* Required header in map files */

/*
 * Boolean Logic Constants
 *
 * Standard boolean value definitions for logical operations and
 * flag variables throughout the PostScript generation system.
 * Ensures consistent true/false semantics across the codebase.
 */
#define TRUE            1   /* Boolean true value */
#define FALSE           0   /* Boolean false value */

/*
 * Map Type Classification Constants
 *
 * Enumeration of different map rendering modes supported by the PostScript
 * generator. Each type produces different visual representations of the
 * same game map data, emphasizing different aspects of the game world.
 *
 * Map Types:
 *   SIMPLE       - Basic map outline without detailed rendering
 *   ALTITUDES    - Elevation/height information display  
 *   DESIGNATIONS - Terrain type and land use information
 *   NATIONS      - Political boundaries and nation ownership
 *   VEGETATIONS  - Vegetation and natural feature display
 *   FORCED       - Override automatic type detection
 *
 * Usage: Selected automatically via file content analysis or manually
 * specified via command line options to control rendering style.
 */
#define SIMPLE          0   /* Basic map rendering without detail overlays */
#define ALTITUDES       1   /* Elevation and topographical information */
#define DESIGNATIONS    2   /* Terrain types and land designations */
#define NATIONS         3   /* Political boundaries and nation colors */
#define VEGETATIONS     4   /* Natural features and vegetation patterns */
#define FORCED          5   /* Manual override of automatic type detection */