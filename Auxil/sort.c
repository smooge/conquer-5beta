/*
 * sort.c
 *
 * Written by Adam Bryant as a replacement for 'sort'
 * for use with the conquer program.  Note that it
 * is initially written with this program in mind and
 * will not have many features useful elsewhere.
 *
 * VARIOUS METHODS OF USE:
 *
 *   standard in to standard out:
 *          'cat foo | conqsort > foonew'
 *   file input to standard output:
 *          'conqsort foo > foonew'
 *   file input to file output:
 *          'conqsort foo foonew'
 *
 * OR MAY EVEN OVERWRITE INITIAL FILE VIA:
 *
 *          'conqsort foo foo'
 *
 * Design Notes:
 *   - all of the file will have to be placed into memory;
 *   - to allow for any line length, space will be malloced()
 *      as it comes in.
 *
 * Initial Revision:		(adb@bucsf.bu.edu)
 *     Tuesday March 21th, 1989 - Began the program at 23:26 EST
 *     Wednesday March 22nd, 1989 - Finished the initial version 11:29 EST
 *     Wednesday July 5th, 1989 - Stopped use of sysexits.h 7:22 EST
 *     Friday April 5th, 1991 - Added in some system ULTRIX tests 2:56 PM EST
 *     February, 1992 (adb@csa.bu.edu === bucsf dead) ;-)
 *     Friday April 24th, 1992 - Fixed "repeats" code which was given
 *		another user (who?). 4:25 PM EST
 *     Thursday May 7th, 1992 - Make "1 more times" -> "1 more time".
 *     Saturday October 10th, 1992 - Make sure blank lines aren't messed up.
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* function checker - using standard library headers */

/* system definitions just in case */
#ifndef FALSE
#define FALSE (0)
#define TRUE  (1)
#endif /* FALSE */

/* system exit definitions from sysexits.h on BSD machines */
#ifndef EX_OK
#define EX_OK             0   /* successful termination */
#define EX_USAGE          64  /* invalid command line format */
#define EX_NOINPUT        66  /* could not open input file */
#define EX_SOFTWARE       70  /* software error; couldn't malloc */
#define EX_CANTCREAT      73  /* could not create output file */
#endif /* EX_OK */

/* number of characters to compare by default */
#define DEFAULT_COMP 2
#define MAX_STR      200

/* structure for holding a line of information */
typedef struct holder {
  char *line;
  struct holder *next;
} L_DATA, *L_PTR;

/* pointer to head of list of lines */
L_PTR head;

/* usage string */
static char usage[]="Usage: %s [-num -h] [infile] [outfile]\n\
	-h		display this screen\n\
	-i		treat upper and lower case characters as equivalent\n\
	-num		Sort on first num characters\n";

/* number of characters for comparison */
int compnum = DEFAULT_COMP;

/* flag for folding upper and lowercase characters */
int iflag = FALSE;

/* file pointers to infile and outfile */
FILE *infile, *outfile;

/* macros */
#define UPPER(x) ((islower(x))?(toupper(x)):(x))

/* external system funtions */
#ifdef BSD
#include <strings.h>
#else
#include <string.h>
#endif /* BSD */
/* fprintf provided by stdio.h */
/* Standard library functions provided by system headers */

/*
 * main - Conquer-specific sorting utility with duplicate detection
 *
 * A custom replacement for Unix 'sort' written specifically for the Conquer
 * game system. Provides in-memory sorting with configurable comparison length,
 * case-insensitive mode, and special duplicate line handling with repetition
 * counting. Designed for processing game log files and data exports.
 *
 * Command Line Usage:
 *   conqsort [-num] [-i] [-h] [infile] [outfile]
 *   Examples:
 *     cat foo | conqsort > foonew        # stdin to stdout
 *     conqsort foo > foonew              # file to stdout  
 *     conqsort foo foonew                # file to file
 *     conqsort foo foo                   # in-place sorting
 *
 * Parameters:
 *   argc - Command line argument count
 *   argv - Command line argument array
 *          -num: Sort on first 'num' characters (default: 2)
 *          -i: Case-insensitive comparison (fold upper/lowercase)
 *          -h: Display help and exit
 *
 * Returns:
 *   EX_OK (0) on successful completion
 *   EX_USAGE (64) on invalid command line format
 *   EX_NOINPUT (66) if input file cannot be opened
 *   EX_CANTCREAT (73) if output file cannot be created
 *   EX_SOFTWARE (70) on memory allocation failure
 *
 * Side Effects:
 *   - Reads entire input file into memory using linked list
 *   - Opens input/output files as specified or uses stdin/stdout
 *   - Allocates dynamic memory for each line (freed on exit)
 *   - Modifies global variables: compnum, iflag, infile, outfile
 *
 * Notes:
 *   - Uses insertion sort algorithm for in-memory line sorting
 *   - Special handling for lines starting with '5' (no duplicate detection)
 *   - Filters input to printable characters, spaces, and tabs only
 *   - Memory efficient: only allocates space needed for each line
 *   - Processes files of any size (limited only by available memory)
 */
int
main(argc, argv)
  int argc;
  char *argv[];
{
  /* declare temporary variables and functions */
  int i, j, num_args = 0, l, innum = 0,outnum = 0;
  int get_line(char data[]);
  void place(char data[]), send_out(void);

  /* input string */
  char data[MAX_STR];

#ifndef VAXC
  infile = stdin;
  outfile = stdout;
#endif

  /* parse command line arguments */
  for (i = 1; i < argc; i++) {

    /* process non-switch statements */
    if (argv[i][0] != '-') {
      num_args++;
      switch(num_args) {
      case 1:
	/* do not open unless arguments are valid */
	innum = i;
	break;
      case 2:
	/* do not open unless arguments are valid */
	outnum = i;
	break;
      default:
	fprintf(stderr, "%s: Too many parameters\n", argv[0]);
	fprintf(stderr, usage, argv[0]);
	exit(EX_USAGE);
	break;
      }
      continue;
    }
    
    /* process switch statements */
    l = strlen(argv[i]);
    for (j = 1; j < l; j++) {
      switch(argv[i][j]) {
      case 'h':
	fprintf(stderr, usage, argv[0]);
	exit(EX_OK);
	break;
      case 'i':
	iflag = TRUE;
	break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
	/* assign new comparison number */
	compnum = 0;
	for(; j < l && argv[i][j] >= '0' && argv[i][j] <= '9'; j++) {
	  compnum *= 10;
	  compnum += argv[i][j] - '0';
	}
	j--;
	break;
      default:
	fprintf(stderr, "%s: invalid option '%c' in <%s>\n",
		argv[0], argv[i][j], argv[i]);
	fprintf(stderr, usage, argv[0]);
	exit(EX_USAGE);
	break;
      }
    }
  }
  
  /* open input file now */
  if (innum != 0) {
    if ((infile = fopen(argv[innum], "r")) == (FILE *)NULL) {
      fprintf(stderr, "%s: can't open file <%s> for input\n",
	      argv[0], argv[innum]);
      exit(EX_NOINPUT);
    }
  }

  /* ======= main processing loop ======== */

  while(!feof(infile)) {

    /* remove any lines less than sort characters */
    if (get_line(data) >= compnum) {
      place(data);
    }

  }

  /* ==== end of main processing loop ==== */

  /* close input file if not stdin */
  if (infile != stdin) {
    (void) fclose(infile);
  }
  
  /* open output file now */
  if(outnum != 0) {
    if ((outfile = fopen(argv[outnum], "w")) == (FILE *)NULL) {
      fprintf(stderr, "%s: cannot open file <%s> for output\n",
	      argv[0], argv[outnum]);
      exit(EX_CANTCREAT);
    }
  }
  send_out();

  /* close output file if not stdout */
  if (outfile != stdout) {
    (void) fclose(outfile);
  }

  exit(EX_OK);
  return(0);
}

/*
 * get_line - Read and filter one line from input with character validation
 *
 * Reads characters from the global input file stream until newline or EOF,
 * filtering input to only include printable characters, spaces, and tabs.
 * Provides bounds checking to prevent buffer overflow and handles line
 * truncation for exceptionally long lines.
 *
 * Input Processing:
 *   - Accepts: spaces, tabs, printable ASCII characters (0x20-0x7E)
 *   - Rejects: control characters, extended ASCII, non-printable characters
 *   - Truncates: lines longer than MAX_STR-1 characters (199 chars + null)
 *   - Terminates: strings with null terminator for safe string handling
 *
 * Parameters:
 *   data - Character buffer to store the filtered line (must be MAX_STR size)
 *          Buffer will be null-terminated regardless of input length
 *
 * Returns:
 *   Number of valid characters stored in data buffer (0 to MAX_STR-1)
 *   0 for empty lines or lines with no valid characters
 *   Does not include the null terminator in the count
 *
 * Side Effects:
 *   - Advances global infile stream position to next line
 *   - Modifies data buffer with filtered line content
 *   - Discards characters beyond MAX_STR-1 limit
 *   - Skips invalid characters without error reporting
 *
 * Notes:
 *   - Used by main processing loop to read game data files
 *   - Character filtering ensures consistent data format for sorting
 *   - Buffer overflow protection prevents memory corruption
 *   - Compatible with various text file formats and encodings
 */
int
get_line(data)
  char data[];
{
  int in,ch;

  /* discard exceptionally long lines */
  for (in = 0;
      !feof(infile) && (in < MAX_STR - 1) && ((ch = getc(infile)) != '\n');
      ) {
    /* copy valid input into data */
    if ((ch == '\t') || (ch == ' ') || ((ch >= ' ') && (ch <= '~'))) {
      data[in] = ch;
      in++;
    }
  }

  /* end string */
  data[in] = '\0';

  return(in);
}

/*
 * send_out - Output sorted lines with duplicate detection and compression
 *
 * Traverses the sorted linked list and outputs lines to the global output file,
 * with special processing for duplicate lines. Consecutive identical lines are
 * compressed into a single line followed by a repetition count message, except
 * for lines beginning with '5' which are always output individually.
 *
 * Duplicate Detection Algorithm:
 *   - Compares each line with the previous line using strcmp()
 *   - Counts consecutive duplicates and outputs summary message
 *   - Special case: lines starting with '5' bypass duplicate detection
 *   - Format: "Last message repeated N more time(s)." (grammatically correct)
 *
 * Output Format:
 *   Normal lines: output as-is with newline
 *   Duplicate summary: "prefix\t\tLast message repeated N more time(s)."
 *   Where prefix is truncated to compnum characters for alignment
 *
 * Parameters:
 *   None (operates on global head pointer and outfile stream)
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Writes formatted output to global outfile stream
 *   - Traverses and effectively consumes the linked list data
 *   - Uses printf formatting for duplicate count messages
 *   - Memory remains allocated (cleanup handled by program exit)
 *
 * Notes:
 *   - Assumes sorted linked list exists (head != NULL)
 *   - Duplicate detection based on full line comparison, not just sort key
 *   - Lines starting with '5' likely represent special game events
 *   - Tab formatting aligns duplicate messages with sorted data
 *   - Grammatical correctness: "1 more time" vs "N more times"
 */
void
send_out()
{
  L_PTR temp = head, prev;
  int cnt = 0;

  fprintf(outfile, "%s\n", temp->line);
  prev = temp;
  temp = temp->next;
  
  while (temp != (L_PTR)NULL) {
    if ((strcmp(temp->line, prev->line) == 0) &&
	(prev->line[0] != '5')) {
      cnt++;
    } else if (cnt != 0) {
      fprintf(outfile, "%-*.*s\t\tLast message repeated %d more %s.\n",
 	      compnum, compnum, prev->line, cnt,
	      (cnt == 1) ? "time" : "times");
      cnt = 0;
      fprintf(outfile, "%s\n", temp->line);
    } else {
      fprintf(outfile, "%s\n", temp->line);
    }
    prev = temp;
    temp = temp->next;
  }
}

/*
 * place - Insert line into sorted linked list using insertion sort algorithm
 *
 * Maintains a sorted linked list by inserting each new line in its proper
 * position based on the custom comparison function. Uses insertion sort
 * algorithm with optimizations for common cases (empty list, insert at head).
 * The sorting order is determined by the first 'compnum' characters of each line.
 *
 * Insertion Algorithm:
 *   1. Empty list: Create first node as head
 *   2. Insert at head: New line sorts before current head
 *   3. Insert in middle/end: Find proper position and insert
 *   Uses comp_line() function for character-by-character comparison
 *
 * Parameters:
 *   data - Null-terminated string containing the line to insert
 *          Must be valid string (handled by get_line() filtering)
 *          Length determined dynamically for memory allocation
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Modifies global head pointer to maintain sorted list
 *   - Allocates memory for new node via build_node()
 *   - Updates next pointers to maintain list structure
 *   - Maintains sort order throughout the insertion process
 *
 * Notes:
 *   - Uses insertion sort: O(n) for each insertion, O(n²) overall
 *   - Efficient for small to medium datasets typical in game files
 *   - Memory allocated per node, allowing variable line lengths
 *   - List remains sorted after each insertion for immediate output
 *   - Depends on comp_line() for comparison logic and sorting criteria
 */
void
place(data)
  char data[];
{
  L_PTR temp, build_node();
  int comp_line();

  /* find location for placing input */
  if (head == (L_PTR)NULL) {
    /* begin list properly */
    head = build_node(data,(L_PTR)NULL);
  } else if (comp_line(head->line, data) == 1) {
    /* add to beginning of list */
    head = build_node(data,head);
  } else {
    /* otherwise add in proper position */
    temp = head;
    while ((temp->next != (L_PTR)NULL) &&
	   (comp_line(temp->next->line, data) != 1)) {
      temp = temp->next;
    }
    temp->next = build_node(data, temp->next);
  }
}

/*
 * comp_line - Compare two strings for sorting with configurable length and case handling
 *
 * Performs character-by-character comparison of two strings up to 'compnum'
 * characters, with optional case-insensitive mode controlled by global 'iflag'.
 * Used by the insertion sort algorithm to determine proper ordering of lines
 * in the sorted output. Handles variable-length strings safely.
 *
 * Comparison Logic:
 *   - Compares up to 'compnum' characters (global variable)
 *   - Case-sensitive mode: direct character comparison (a[i] vs b[i])
 *   - Case-insensitive mode: UPPER() macro converts to uppercase first
 *   - Early termination on null character encounter
 *   - Standard lexicographic ordering (dictionary-style sorting)
 *
 * Parameters:
 *   a - First string for comparison (null-terminated)
 *   b - Second string for comparison (null-terminated)
 *       Both strings must be valid (filtered by get_line())
 *
 * Returns:
 *   -1 if string 'a' should precede string 'b' in sorted order
 *    0 if strings are equal (within compnum characters)
 *    1 if string 'a' should follow string 'b' in sorted order
 *
 * Side Effects:
 *   None (pure comparison function, no global state modification)
 *
 * Notes:
 *   - Uses global 'compnum' variable for comparison length limit
 *   - Uses global 'iflag' variable for case-insensitive mode control
 *   - UPPER() macro handles case conversion safely for ASCII characters
 *   - Stops at null terminator even if compnum not reached
 *   - Essential for maintaining proper sort order in linked list
 */
int
comp_line(a, b)
  char *a, *b;
{
  int i;

  for(i = 0; i < compnum; i++) {
    /* do checks */
    if (iflag) {
      if (UPPER(a[i]) < UPPER(b[i])) return(-1);
      if (UPPER(b[i]) < UPPER(a[i])) return(1);
    } else {
      if (a[i] < b[i]) return(-1);
      if (b[i] < a[i]) return(1);
    }
    if (a[i] == '\0') return(0);
  }
  return(0);
}

/*
 * build_node - Create and initialize linked list node with dynamic memory allocation
 *
 * Allocates memory for a new linked list node containing a copy of the input
 * string data. Performs dynamic memory allocation for both the node structure
 * and the string data, allowing efficient storage of variable-length lines.
 * Handles memory allocation failures with proper error reporting and program exit.
 *
 * Memory Allocation:
 *   - Node structure: sizeof(L_DATA) bytes for the node itself
 *   - String data: strlen(data) + 1 bytes for null-terminated string copy
 *   - Dynamic sizing: each line uses only the memory it needs
 *   - Error handling: exits program if allocation fails
 *
 * Parameters:
 *   data - Null-terminated string to store in the new node
 *          Must be valid string (typically from get_line())
 *          String is copied, original data can be reused
 *   nptr - Pointer to next node in linked list (may be NULL)
 *          Allows insertion at any position in the list
 *
 * Returns:
 *   L_PTR pointing to newly allocated and initialized node
 *   Never returns NULL (program exits on allocation failure)
 *
 * Side Effects:
 *   - Allocates dynamic memory that persists until program exit
 *   - Exits program with EX_SOFTWARE on allocation failure
 *   - Prints error message to stderr before exit
 *   - Copies string data using strcpy()
 *
 * Notes:
 *   - Memory is not freed during program execution (cleanup on exit)
 *   - Essential for building the sorted linked list structure
 *   - Uses system malloc() for memory allocation
 *   - Error messages indicate specific allocation failure type
 */
L_PTR
build_node(data, nptr)
  char data[];
  L_PTR nptr;
{
  L_PTR temp;
  char *strcpy();
  
  /* build the memory space */
  if ((temp = (L_PTR)malloc(sizeof(L_DATA))) == (L_PTR)NULL) {
    fprintf(stderr, "Error in creating structure memory!\n");
    exit(EX_SOFTWARE);
  }
  if ((temp->line = (char *)malloc((strlen(data) + 1) *
				   sizeof(char))) == NULL) {
    fprintf(stderr, "Error in creating data memory!\n");
    exit(EX_SOFTWARE);
  }

  /* assign the values */
  (void) strcpy(temp->line, data);
  temp->next = nptr;
  return(temp);
}
