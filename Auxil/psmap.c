
/*
 * A program to convert conquer-maps to postscript Feel free to hack'n slash
 * Comments should be sent to  d8forma@dtek.chalmers.se
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "psmap.h"

char buffer[BUFSIZ];
int c, xsize, ysize;
int xmin, ymin, xmax, ymax, centx, centy;
FILE *infile, *outfile, *fh;
int grid = TRUE, sqsize = 9, maptype = SIMPLE, onepage = FALSE;
int verbose = FALSE, coords = TRUE, center = TRUE, note = FALSE;
int pagewidth, pageheight, xoffset, yoffset;
char title[81] = "", foot[81], fontname[81];
char progname[80];

/*
 * parsepagesize - Parse page size string to numeric identifier
 *
 * Converts page size strings to internal numeric constants for page
 * configuration. Supports case-insensitive matching of standard paper sizes.
 *
 * Parameters:
 *   buf - String containing page size name to parse
 *
 * Returns:
 *   1 for A4 paper size (case insensitive)
 *   2 for LETTER paper size (case insensitive)
 *   0 for unknown/unsupported page size (defaults to OTHER)
 *
 * Side Effects:
 *   None - pure string parsing function
 *
 * Notes:
 *   - Case insensitive comparison (A4/a4, LETTER/letter both work)
 *   - Used for command line option parsing and environment variable processing
 *   - Return values correspond to DEFAULTPAGE constants in psmap.h
 *   - Unknown strings return 0, which maps to OTHER page size
 */
int
parsepagesize(buf)
  char *buf;
{
  if (!strcmp(buf, "A4"))
    return (1);
  if (!strcmp(buf, "a4"))
    return (1);
  if (!strcmp(buf, "LETTER"))
    return (2);
  if (!strcmp(buf, "letter"))
    return (2);
  return (0);
}

/*
 * setpagesize - Configure page dimensions and offsets for given page type
 *
 * Sets global page configuration variables based on predefined page size
 * constants. Updates page dimensions, offsets, and printer-specific settings
 * for PostScript output generation.
 *
 * Parameters:
 *   defpag - Page size identifier (1=A4, 2=LETTER, other=OTHER)
 *
 * Returns:
 *   void - configuration stored in global variables
 *
 * Side Effects:
 *   - Sets global pagewidth variable (page width in PostScript points)
 *   - Sets global pageheight variable (page height in PostScript points)
 *   - Sets global xoffset variable (horizontal margin offset)
 *   - Sets global yoffset variable (vertical margin offset)
 *
 * Notes:
 *   - A4: 540x820 points with 30x10 offsets (European standard)
 *   - LETTER: 575x760 points with 15x10 offsets (US standard)
 *   - OTHER: 450x700 points with 40x30 offsets (generic/custom)
 *   - Values optimized for typical PostScript printers
 *   - Coordinates in PostScript points (1/72 inch)
 */
void
setpagesize(defpag)
  int defpag;
{
  switch (defpag) {
  case 1:
    pagewidth = PAGEWIDTH_A4;
    pageheight = PAGEHEIGHT_A4;
    xoffset = XOFFSET_A4;
    yoffset = YOFFSET_A4;
    break;
  case 2:
    pagewidth = PAGEWIDTH_LETTER;
    pageheight = PAGEHEIGHT_LETTER;
    xoffset = XOFFSET_LETTER;
    yoffset = YOFFSET_LETTER;
    break;
  default:
    pagewidth = PAGEWIDTH_OTHER;
    pageheight = PAGEHEIGHT_OTHER;
    xoffset = XOFFSET_OTHER;
    yoffset = YOFFSET_OTHER;
    break;
  }
}

/*
 * get_pagesize - Initialize page size from environment or default
 *
 * Reads page size configuration from environment variable and sets up
 * global page dimensions. Falls back to compile-time default if environment
 * variable is not set or contains invalid value.
 *
 * Parameters:
 *   None - reads from CONQ_PSMAPDEFAULTPAGE environment variable
 *
 * Returns:
 *   void - page configuration stored in global variables
 *
 * Side Effects:
 *   - Calls setpagesize() to configure global page variables
 *   - Reads CONQ_PSMAPDEFAULTPAGE environment variable
 *
 * Notes:
 *   - Environment variable format: "A4", "a4", "LETTER", "letter"
 *   - Invalid or missing environment variable uses DEFAULTPAGE constant
 *   - DEFAULTPAGE is set at compile time via preprocessor defines
 *   - Provides runtime page size configuration flexibility
 */
void
get_pagesize()
{
  char *buf;
  int defpag = DEFAULTPAGE;
  
  buf = (char *) getenv("CONQ_PSMAPDEFAULTPAGE");
  if (buf != NULL)
    defpag = parsepagesize(buf);
  setpagesize(defpag);
}

/*
 * psstring - Write PostScript-escaped string to output file
 *
 * Converts a C string to PostScript string format with proper escaping
 * of special characters. Wraps the string in PostScript parentheses and
 * escapes characters that have special meaning in PostScript.
 *
 * Parameters:
 *   fh - File handle to write PostScript string to
 *   str - C string to convert and output
 *
 * Returns:
 *   void - output written directly to file
 *
 * Side Effects:
 *   - Writes to specified file handle
 *   - Outputs opening and closing parentheses
 *   - Escapes special PostScript characters with backslashes
 *
 * Character Escaping:
 *   - '(' becomes '\\('
 *   - ')' becomes '\\)'  
 *   - '\\' becomes '\\\\\\\\'
 *   - '\\n' and '\\f' are filtered out (not written)
 *   - All other characters written as-is
 *
 * Notes:
 *   - PostScript strings are enclosed in parentheses: (string)
 *   - Essential for safe PostScript generation from user input
 *   - Prevents PostScript syntax errors from special characters
 *   - Used for titles, footers, and other text elements
 */
void
psstring(fh, str)
  FILE *fh;
  char *str;
{
  fprintf(fh, "(");
  while (*str != NULL) {
    switch (*str) {
    case ('('):
      fprintf(fh, "\\(");
      break;
    case (')'):
      fprintf(fh, "\\)");
      break;
    case ('\\'):
      fprintf(fh, "\\\\");
      break;
    case ('\n'):
    case ('\f'):
      break;
    default:
      fputc(*str, fh);
      break;
    }
    str++;
  }
  fprintf(fh, ")");
}

/*
 * isinstr - Search for substring within string (case sensitive)
 *
 * Performs substring search to determine if a word appears anywhere within
 * a larger string. Uses character-by-character comparison with length
 * optimization to avoid unnecessary checks.
 *
 * Parameters:
 *   string - Source string to search within
 *   word - Substring to search for
 *
 * Returns:
 *   TRUE if word is found within string
 *   FALSE if word is not found or string is shorter than word
 *
 * Side Effects:
 *   None - pure string search function
 *
 * Algorithm:
 *   1. Early exit if string is shorter than search word
 *   2. Iterate through each position in string
 *   3. Stop early when remaining string is shorter than word
 *   4. Compare first character, then full substring if match
 *   5. Return TRUE on first complete match found
 *
 * Notes:
 *   - Case sensitive comparison (uses strncmp)
 *   - More efficient than naive substring search due to length checks
 *   - Used for map type detection from file headers
 *   - Could be replaced with standard strstr() function
 */
int
isinstr(string, word)
  char *string, *word;
{
  int i,l1=strlen(string),l2=strlen(word);
  
  if (l1 < l2) return(FALSE);
  for(i = 0; i < l1; i++ ) {
    if (l1-i<l2) break;
    if (string[i] == word[0]) {
      if (strncmp(string+i,word,l2)==0) return(TRUE);
    }
  }
  return(FALSE);
}

/*
 * getmaptype - Determine map type from header string analysis
 *
 * Analyzes the map file header string to automatically detect the type
 * of Conquer map data for appropriate PostScript rendering. Different
 * map types require different visual representations and color schemes.
 *
 * Parameters:
 *   string - Map file header string to analyze for type keywords
 *
 * Returns:
 *   ALTITUDES if "Altitude" found in string
 *   DESIGNATIONS if "Designation" found in string  
 *   NATIONS if "Nation" found in string
 *   VEGETATIONS if "Vegetation" found in string
 *   SIMPLE if no specific type keywords found (default/fallback)
 *
 * Side Effects:
 *   None - pure string analysis function
 *
 * Map Type Descriptions:
 *   - ALTITUDES: Terrain elevation data (mountains, plains, water)
 *   - DESIGNATIONS: Land use/building types (cities, roads, farms)
 *   - NATIONS: Political boundaries and ownership
 *   - VEGETATIONS: Flora and natural terrain types
 *   - SIMPLE: Basic character map without special rendering
 *
 * Notes:
 *   - Case sensitive substring matching via isinstr()
 *   - First match wins (order: Altitude, Designation, Nation, Vegetation)
 *   - Map type affects PostScript rendering algorithms and color choices
 *   - Used for automatic map type detection from file metadata
 */
int
getmaptype(string)
  char *string;
{
  if (isinstr(string, "Altitude"))
    return (ALTITUDES);
  if (isinstr(string, "Designation"))
    return (DESIGNATIONS);
  if (isinstr(string, "Nation"))
    return (NATIONS);
  if (isinstr(string, "Vegetation"))
    return (VEGETATIONS);
  return (SIMPLE);
}

/*
 * readmap - Parse map file and convert to PostScript array format
 *
 * Reads the Conquer map data character by character, analyzes dimensions,
 * calculates visible area bounds, and converts to PostScript array format.
 * Performs real-time bounds detection and validates map structure consistency.
 *
 * Parameters:
 *   None - reads from global infile, writes to global outfile
 *
 * Returns:
 *   void - map data written to output, bounds stored in global variables
 *
 * Side Effects:
 *   - Sets global variables: xsize, ysize (map dimensions)
 *   - Sets global variables: xmin, xmax, ymin, ymax (visible bounds)
 *   - Writes PostScript array definition to output file
 *   - May call exit(1) on map format errors or empty files
 *   - Outputs verbose statistics if verbose mode enabled
 *
 * Map Processing Algorithm:
 *   1. Initialize bounds tracking with sentinel values
 *   2. Character-by-character input processing
 *   3. Newlines: Validate line length consistency, update row count
 *   4. Spaces: Track position, output to PostScript (part of map data)
 *   5. Map characters: Update bounds, track visible area, output to PS
 *   6. PostScript output: Wrapped in parentheses for string arrays
 *   7. Bounds calculation: Track actual content area vs full grid
 *
 * Validation Rules:
 *   - All map lines must have identical length (exits on mismatch)
 *   - Map must contain at least one non-space character (exits if empty)
 *   - Handles arbitrary map sizes dynamically
 *
 * PostScript Output Format:
 *   /Map [
 *     (line1data)
 *     (line2data)
 *     ...
 *   ] def
 *
 * Notes:
 *   - xmin/xmax track leftmost/rightmost non-space characters
 *   - ymin/ymax track topmost/bottommost rows with content
 *   - Space characters are preserved in output (represent empty terrain)
 *   - Bounds used later for page layout and centering calculations
 */
void
readmap()
{
  int x, none;
  
    /*
     * Now we read the map. Anybody got an idea how big it is ? I don't, but
     * who cares ?
     */
  
  x = 0;
  xsize = 0;
  ysize = 0;
  xmin = 0xffffff;
  xmax = (-1);
  ymin = (-1);
  ymax = 0;
  none = TRUE;
  
  fprintf(outfile, "/Map [\n");
  
  while ((c = fgetc(infile)) != EOF) {
    switch (c) {
    case ('\n'):
      if (xsize == 0)
	xsize = x;
      if (xsize != x) {
	fprintf(stderr, "Error in map-file. Lines different length\n");
	exit(1);
      }
      x = 0;
      if ((xmax > -1) && (ymin == -1))
	ymin = ysize;
      ysize++;
      fprintf(outfile, ")\n");
      none = TRUE;
      break;
    case (' '):
      x++;
      if (none) {
	fprintf(outfile, "(");
	none = FALSE;
      }
      fprintf(outfile, " ");
      break;
    default:
      if (x < xmin)
	xmin = x;
      if (x > xmax)
	xmax = x;
      if (ysize > ymax)
	ymax = ysize;
      x++;
      if (none) {
	fprintf(outfile, "(");
	none = FALSE;
      }
      fprintf(outfile, "%c", c);
      break;
    }
  }
  
  fprintf(outfile, "] def\n\n");
  
  /* Did I get anything ? */
  if (xmax == -1) {
    fprintf(stderr, "Empty input file\n");
    exit(1);
  }
  if (verbose) {
    fprintf(stderr, "Map is %d * %d  squares\n", xsize, ysize);
    fprintf(stderr, "You can see %d * %d  squares\n",
	    xmax - xmin + 1, ymax - ymin + 1);
  }
}

/*
 * buildps - Generate PostScript output with layout and page definitions
 *
 * Creates the complete PostScript program by writing configuration variables,
 * including PostScript library code, calculating page layout, and generating
 * page rendering commands. Handles both multi-page and single-page output modes.
 *
 * Parameters:
 *   None - uses global variables for configuration and map data
 *
 * Returns:
 *   void - complete PostScript program written to output file
 *
 * Side Effects:
 *   - Writes extensive PostScript code to output file
 *   - Opens and reads PSFILE (PostScript library code)
 *   - May call exit(-10) if PSFILE cannot be opened
 *   - Outputs verbose page count information if enabled
 *
 * PostScript Generation Process:
 *   1. Calculate page count based on map size and square size
 *   2. Write PostScript variable definitions for all parameters
 *   3. Include PostScript library code from external PSFILE
 *   4. Calculate layout geometry (squares per page, centering)
 *   5. Generate DoPage commands for each page or single page
 *
 * Layout Calculations:
 *   - xpages/ypages: Number of pages needed horizontally/vertically
 *   - xnumb/ynumb: Number of map squares per page dimension
 *   - xcorr/ycorr: Centering corrections for better layout
 *   - Page boundaries respect margins and square size constraints
 *
 * Output Modes:
 *   - Multi-page: Generates DoPage commands for each page tile
 *   - Single-page: Centers specified coordinates on one page
 *   - Pages numbered starting from 1, include map coordinates
 *
 * PostScript Variables Written:
 *   Map bounds (xmin, ymin, xmax, ymax), page layout (xpages, ypages),
 *   rendering options (sqsize, grid, coords, maptype), page setup
 *   (pagewidth, pageheight, margins, offsets), text elements (title, foot)
 *
 * Notes:
 *   - PSFILE contains PostScript functions for map rendering
 *   - DoPage function signature: page_x page_y map_x squares_x map_y squares_y
 *   - Centering calculations ensure optimal map placement on pages
 *   - Single-page mode uses centx/centy from command line options
 */
void
buildps()
{
  int xbeg, ybeg, xnumb, ynumb, x, y, xpages, ypages, xcorr, ycorr;
  
  /*
   * The map should be dumped to the outfile now. Maybe we should tell the
   * printer what to do with it?
   */
  
  xpages = 1 + (xmax - xmin) * sqsize / (pagewidth - XMARGINS);
  ypages = 1 + (ymax - ymin) * sqsize / (pageheight - YMARGINS);
  
  if (verbose)
    fprintf(stderr, "The map will be %d * %d pages\n", xpages, ypages);
  
  fprintf(outfile, "/xmin %d def\n", xmin);
  fprintf(outfile, "/ymin %d def\n", ymin);
  fprintf(outfile, "/xmax %d def\n", xmax);
  fprintf(outfile, "/ymax %d def\n", ymax);
  fprintf(outfile, "/xpages %d def\n", xpages);
  fprintf(outfile, "/ypages %d def\n", ypages);
  fprintf(outfile, "/sqsize %d def\n", sqsize);
  fprintf(outfile, "/title ");
  psstring(outfile, title);
  fprintf(outfile, " def\n");
  fprintf(outfile, "/foot ");
  psstring(outfile, foot);
  fprintf(outfile, " def\n");
  fprintf(outfile, "/foot2 (ConqPS Version %s) def\n", VERSION);
  fprintf(outfile, "/PAGEWIDTH %d def\n", pagewidth);
  fprintf(outfile, "/PAGEHEIGHT %d def\n", pageheight);
  fprintf(outfile, "/XMARGINS %d def\n", XMARGINS);
  fprintf(outfile, "/YMARGINS %d def\n", YMARGINS);
  fprintf(outfile, "/XOFFSET %d def\n", xoffset);
  fprintf(outfile, "/YOFFSET %d def\n", yoffset);
  fprintf(outfile, "/maptype %d def\n", maptype);
  fprintf(outfile, "/font /%s def\n", fontname);
  fprintf(outfile, "/grid ");
  if (grid) {
    fprintf(outfile, "true def\n");
  } else fprintf(outfile, "false def\n");
  fprintf(outfile, "/coords ");
  if (coords) {
    fprintf(outfile, "true def\n");
  } else fprintf(outfile, "false def\n");
  fprintf(outfile, "/noteq ");
  if (note) {
    fprintf(outfile, "true def\n");
  } else fprintf(outfile, "false def\n");
  
  fprintf(outfile, "\n\n");

  fh = fopen(PSFILE, "r");
  
  if (fh == NULL) {
    perror(progname);
    exit(-10);
  }
  while ((c = fgetc(fh)) != EOF)
    fputc(c, outfile);

  fclose(fh);

  fprintf(outfile, "\n %% Here we start the magic\n");

  xnumb = (int) ((pagewidth - XMARGINS) / sqsize);
  ynumb = (int) ((pageheight - YMARGINS) / sqsize);

  if (center) {
    xcorr = (int) ((xpages * xnumb - (xmax - xmin)) / 2);
    ycorr = (int) ((ypages * ynumb - (ymax - ymin)) / 2);
  } else {
    xcorr = 0;
    ycorr = 0;
  }

  if (onepage) {
    fprintf(outfile, "%d %d %d %d %d %d DoPage\n",
		1, 1, centx - xnumb / 2, xnumb, centy - ynumb / 2, ynumb);
  } else for (x = 0; x < xpages; x++)
    for (y = 0; y < ypages; y++) {
      xbeg = x * xnumb + x + xmin - xcorr;
      ybeg = y * ynumb + y + ymin - ycorr;
      fprintf(outfile, "%d %d %d %d %d %d DoPage\n",
	      x + 1, y + 1, xbeg, xnumb, ybeg, ynumb);
    }
}

/*
 * main - PostScript map converter main program with command line processing
 *
 * Comprehensive command line tool for converting Conquer game map files to
 * PostScript format for printing. Handles extensive configuration options,
 * file validation, environment variable processing, and coordinates the
 * entire map conversion pipeline.
 *
 * Parameters:
 *   argc - Command line argument count
 *   argv - Command line argument vector
 *
 * Returns:
 *   0 on successful conversion
 *   1 on command line errors or file format problems
 *   -10 on file I/O errors
 *
 * Side Effects:
 *   - Reads input map file (stdin if not specified)
 *   - Writes PostScript output file (stdout if not specified)
 *   - May output verbose information to stderr
 *   - Processes environment variables (CONQ_PSFONT, CONQ_PSMAPDEFAULTPAGE)
 *
 * Command Line Options:
 *   -c: Turn off coordinate display
 *   -f font: Set PostScript font name
 *   -g: Turn off grid lines
 *   -h: Show help text and exit
 *   -l: Enable large map mode (note flag)
 *   -L height: Set custom page height
 *   -n: Turn off map centering
 *   -o x,y: Single page mode centered on coordinates
 *   -p size: Set page size (A4, LETTER, OTHER)
 *   -s size: Set square size in points (default varies)
 *   -t title: Set map title text
 *   -u: Force simple map output (override auto-detection)
 *   -v: Verbose mode with diagnostic output
 *   -W width: Set custom page width
 *   -X offset: Set horizontal page offset
 *   -Y offset: Set vertical page offset
 *
 * File Processing Pipeline:
 *   1. Initialize defaults and process environment variables
 *   2. Parse command line options and validate arguments
 *   3. Open input/output files with error checking
 *   4. Validate map file format and extract metadata
 *   5. Determine map type (altitude, designation, nation, vegetation)
 *   6. Parse map data and calculate bounds (readmap)
 *   7. Generate PostScript output with layout (buildps)
 *   8. Clean up and exit
 *
 * Map File Validation:
 *   - Must start with MATCHSTRING ("Conquer Version")
 *   - Header format: "version:title" where version becomes footer
 *   - Auto-detects map type from title keywords
 *   - Extracts title for PostScript output if not overridden
 *
 * Environment Variables:
 *   - CONQ_PSFONT: Default PostScript font (fallback: Times-Roman)
 *   - CONQ_PSMAPDEFAULTPAGE: Default page size (A4/LETTER/OTHER)
 *
 * Error Handling:
 *   - Invalid command line: Usage message and exit(1)
 *   - File errors: perror() and exit(-10)
 *   - Format errors: Error message and exit(1)
 *   - Coordinate parsing: Error message and exit(1)
 *
 * Notes:
 *   - Supports stdin/stdout for pipeline processing
 *   - Optimized output buffering for large maps
 *   - Extensive help system with current defaults
 *   - Author attribution: MaF (d8forma@dtek.chalmers.se)
 */
int
main(argc, argv)
  int argc;
  char **argv;
{
  extern char *optarg;
  extern int optind;
  char *buf, firstline[81];
  
  strcpy(progname, argv[0]);
  infile = stdin;
  outfile = stdout;
  buf = (char *) getenv("CONQ_PSFONT");
  if (buf != NULL) {
    strncpy(fontname, buf, 80);
    fontname[79] = '\0';
  } else strcpy(fontname, "Times-Roman");
  get_pagesize();
  
  while ((c = getopt(argc, argv, "nuf:gs:t:vcho:p:lW:L:X:Y:")) != -1)
    switch (c) {
    case 'h':
      fprintf(stderr, "%s version %s\n", progname, VERSION);
      fprintf(stderr, "Default pagesize is ");
      switch (DEFAULTPAGE) {
      case 1:
	fprintf(stderr, "A4\n");
	break;
      case 2:
	fprintf(stderr, "LETTER\n");
	break;
      default:
	fprintf(stderr, "OTHER\n");
	break;
      }
      fprintf(stderr, USAGE, progname);
      fprintf(stderr, "\t-c  Turn off coordinates\n");
      fprintf(stderr, "\t-f  Set the font\n");
      fprintf(stderr, "\t-g  Turn off grid\n");
      fprintf(stderr, "\t-h  Show this text\n");
      fprintf(stderr, "\t-l  Print large maps\n");
      fprintf(stderr, "\t-L  Set the pagelength\n");
      fprintf(stderr, "\t-n  Turn off map centering\n");
      fprintf(stderr, "\t-o  Show one page centered around x,y\n");
      fprintf(stderr, "\t-p  Set pagesize (A4,LETTER or OTHER)\n");
      fprintf(stderr, "\t-s  Set size of square (default: %d)\n", sqsize);
      fprintf(stderr, "\t-t  Set the title of the map\n");
      fprintf(stderr, "\t-u  Force simple map output (just letters)\n");
      fprintf(stderr, "\t-v  Verbose mode\n");
      fprintf(stderr, "\t-W  Set the pagewidth\n");
      fprintf(stderr, "\t-X  Set the X-offset\n");
      fprintf(stderr, "\t-Y  Set the Y-offset\n");
      exit(1);
    case 'u':
      maptype = FORCED;
      break;
    case 'o':
      onepage = TRUE;	/* Mode one of onepage */
      if (2 != sscanf(optarg, "%d,%d", &centx, &centy)) {
	fprintf(stderr, "Error in coordinates to o-option\n");
	exit(1);
      }
      break;
    case 'l':
      note = TRUE;
      break;
    case 'p':
      setpagesize(parsepagesize(optarg));
      break;
    case 'n':
      center = FALSE;
      break;
    case 'g':
      grid = FALSE;
      break;
    case 's':
      sscanf(optarg, "%d", &sqsize);
      break;
    case 'f':
      strncpy(fontname, optarg, 80);
      fontname[79] = '\0';
      break;
    case 't':
      strncpy(title, optarg, 80);
      title[79] = '\0';
      break;
    case 'v':
      verbose = TRUE;
      break;
    case 'c':
      coords = FALSE;
      break;
    case 'W':
      sscanf(optarg, "%d", &pagewidth);
      break;
    case 'L':
      sscanf(optarg, "%d", &pageheight);
      break;
    case 'X':
      sscanf(optarg, "%d", &xoffset);
      break;
    case 'Y':
      sscanf(optarg, "%d", &yoffset);
      break;
    default:
      fprintf(stderr, USAGE, argv[0]);
      exit(1);
    }
  if (optind < argc)
    infile = fopen(argv[optind], "r");
  
  if (infile == NULL) {
    perror(progname);
    exit(-10);
  }
  if (++optind < argc)
    outfile = fopen(argv[optind], "w");
  
  if (outfile == NULL) {
    perror(progname);
    exit(-10);
  }
  setbuf(outfile, buffer);	/* They recomended this on the net today */
  
  if (++optind < argc) {
    fprintf(stderr, USAGE, argv[0]);
    exit(1);
  }
  /* If verbose identify program */
  
  if (verbose)
    fprintf(stderr, "Psmap version %s\n", VERSION);
  

  /* First we check if the infile is a valid conquer map file */
  
  if (NULL == fgets(firstline, 80, infile)) {
    fprintf(stderr, "Empty input file\n");
    exit(1);
  }
  if (0 != strncmp(firstline, MATCHSTRING, strlen(MATCHSTRING))) {
    fprintf(stderr, "Not a Conquer Map file\n");
    exit(1);
  }
  for (c = 0; c < strlen(firstline); c++) {
    if (firstline[c] == ':')
      break;
  }

  strncpy(foot, firstline, c);
  foot[c] = '\0';
  if (title[0] == '\0') {
    strncpy(title, &firstline[c + 1], 80);
    title[79] = '\0';
  }
  /* Find out which type of map it is */
  
  if (maptype != FORCED) {
    maptype = getmaptype(&firstline[c + 1]);
  } else maptype = SIMPLE;
  
  if (verbose) {
    fprintf(stderr, "Maptype is ");
    switch (maptype) {
    case (SIMPLE):
      fprintf(stderr, "simple\n");
      break;
    case (ALTITUDES):
      fprintf(stderr, "altitudes\n");
      break;
    case (DESIGNATIONS):
      fprintf(stderr, "designations\n");
      break;
    case (NATIONS):
      fprintf(stderr, "nations\n");
      break;
    case (VEGETATIONS):
      fprintf(stderr, "vegetations\n");
      break;
    default:
      break;
    }
  }
  /* Send the first part of the postscript file to outfile */
  
  fprintf(outfile, "%%!\n");
  fprintf(outfile, "%% Created by conqps version %s\n\n", VERSION);
  if (note)
    fprintf(outfile, "\nnote\n\n");
  fprintf(outfile, "%% Here comes the map data:\n");
  
  /* Parse the map */
  
  readmap();

  /* Build the last part of postscript file */

  buildps();

  /* Phu that was it.  */

  if (infile != NULL)
    fclose(infile);
  if (outfile != NULL)
    fclose(outfile);

  return (0);
}
