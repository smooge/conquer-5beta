/* This source file contains code to read and write the data file */
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * A good deal of time and effort has gone into the writing of this
 * code and it is our hope that you respect this.  We give permission
 * to alter the code, but not to redistribute modified versions of the
 * code without our explicit permission.  If you do modify the code,
 * please document the changes made, and send us a copy, so that all
 * people may have it.  The code, to the best of our knowledge, works
 * well, but there will probably always be a need for bug fixes and
 * improvements.  We disclaim any responsibility for the codes'
 * actions.  [Use at your own risk].  This notice is just our way of
 * saying, "Happy gaming!", while making an effort to not get sued in
 * the process.
 *                           Ed Barlow, Adam Bryant
 */
#include "dataX.h"
#include "armyX.h"
#include "cityX.h"
#include "navyX.h"
#include "itemX.h"
#include "butesX.h"
#include "activeX.h"
#include "statusX.h"
#include "dataioX.h"
#include "caravanX.h"
#include "olddataX.h"
#include "patchlevel.h"

/* indicator of any conversions needed */
static int convert_level = 0;

/* the data sizes of the various types -- DO NOT CHANGE NON-"unused" STUFF */
static DIO_STRUCT dio_types[DIO_NUMBER] = {
  { "bits_per_byte", NBBY },
  { "size_of_char", (int) sizeof(char) },
  { "size_of_unsigned_char", (int) sizeof(uns_char) },
  { "size_of_short", (int) sizeof(short) },
  { "size_of_unsigned_short", (int) sizeof(uns_short) },
  { "size_of_int", (int) sizeof(int) },
  { "size_of_long", (int) sizeof(long) },
  { "size_of_double", (int) sizeof(double) },
  { "size_of_idtype", (int) sizeof(idtype) },
  { "size_of_maptype", (int) sizeof(maptype) },
  { "size_of_itemtype", (int) sizeof(itemtype) },
  { "size_of_ntntype", (int) sizeof(ntntype) },
  { "unused", 0 },
  { "unused", 0 },
  { "unused", 0 },
  { "unused", 0 },
  { "unused", 0 },
  { "unused", 0 },
  { "unused", 0 },
  { "unused", 0 }
};

/* the data sizes of the structures -- DO NOT CHANGE NON-"unused" STUFF */
static DIO_STRUCT dio_structs[DIOS_NUMBER] = {
  { "size_of_world", sizeof(struct s_world) },
  { "size_of_sector", sizeof(SCT_STRUCT) },
  { "size_of_nation", sizeof(NTN_STRUCT) },
  { "size_of_city", sizeof(CITY_STRUCT) },
  { "size_of_army", sizeof(ARMY_STRUCT) },
  { "size_of_navy", sizeof(NAVY_STRUCT) },
  { "size_of_caravan", sizeof(CVN_STRUCT) },
  { "size_of_unum", sizeof(UNITNUM) },
  { "size_of_map", sizeof(MAP_STRUCT) },
  { "unused", 0 },
  { "unused", 0 },
  { "unused", 0 },
  { "unused", 0 },
  { "unused", 0 },
  { "unused", 0 }
};

/*
 * syserr_msg - Print system error message with optional perror support
 *
 * Outputs a formatted error message to the update file, optionally using
 * perror() to include system error details if available. The error message
 * is prefixed with the program name for context.
 *
 * Parameters:
 *   estr - Error message string to display (must not be NULL)
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Writes error message to fupdate file stream
 *   - Calls perror() if NO_PERROR is not defined
 *
 * Testing Notes:
 *   Category: B (Integration) - Requires global file pointer and program state
 *   Approach: Integration testing with mock file I/O and controlled globals
 *   Key Tests: Error message formatting, conditional perror() usage
 *   Dependencies: fupdate file pointer, prog_name global variable
 *   Mock Requirements: FILE* fupdate, prog_name string, perror() conditionally
 *   Complexity: Simple - Basic error reporting with platform-specific behavior
 *
 * Notes:
 *   - Function depends on global fupdate file pointer
 *   - Uses global prog_name for error message prefix
 *   - NO_PERROR compile flag controls perror() usage
 */
void
syserr_msg PARM_1(char *, estr)
{
#ifdef NO_PERROR
  fprintf(fupdate, "%s: %s\n", prog_name, estr);
#else
  fprintf(fupdate, "%s\n", estr);
  perror(prog_name);
#endif /* NO_PERROR */
}

/*
 * fput_string - Safe string output with error checking
 *
 * Writes a string to the specified file stream and checks for write errors.
 * If fputs() fails (returns EOF), prints an error message and terminates
 * the program via abrt().
 *
 * Parameters:
 *   outstr - String to write to file (must not be NULL)
 *   filep - File pointer to write to (must be valid open file)
 *
 * Returns:
 *   None (terminates program on error)
 *
 * Side Effects:
 *   - Writes string to specified file
 *   - Calls abrt() to terminate program on write failure
 *
 * Testing Notes:
 *   Category: A (Unit) - Self-contained with clear input/output contract
 *   Approach: Unit testing with mock FILE operations and error simulation
 *   Key Tests: Successful write, EOF error handling, abrt() call verification
 *   Dependencies: fputs() function, abrt() function for error termination
 *   Mock Requirements: Mock FILE* operations, stub abrt() function
 *   Complexity: Simple - Straightforward error-checked file output wrapper
 *
 * Notes:
 *   - Provides error-checked file output for critical data operations
 *   - Used extensively during data file writing
 */
void
fput_string PARM_2(char *, outstr, FILE *, filep)
{
  /* check the output of fputs */
  if (fputs(outstr, filep) == EOF) {
    syserr_msg("\nunable to write to the data file");
    abrt();
  }
}

/*
 * wr_header - Write data file header with version and type information
 *
 * Writes the data file header containing version information, patch level,
 * and size information for all data types and structures. This header is
 * used during file reading to ensure compatibility and enable data
 * conversion between different game versions.
 *
 * Parameters:
 *   filep - Output file stream (must be valid and writable)
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Writes VERSION string to file
 *   - Writes PATCHLEVEL number to file
 *   - Writes all type sizes from dio_types array
 *   - Writes all structure sizes from dio_structs array
 *
 * Testing Notes:
 *   Category: B (Integration) - Requires global arrays and string formatting
 *   Approach: Integration testing with controlled global data and mock I/O
 *   Key Tests: Version string output, patch level formatting, type/struct size arrays
 *   Dependencies: VERSION, PATCHLEVEL constants, dio_types[], dio_structs[] arrays
 *   Mock Requirements: Mock FILE*, sprintf() buffer, fput_string() integration
 *   Complexity: Moderate - Iterates through multiple global arrays with formatting
 *
 * Notes:
 *   - Header format is critical for data file compatibility
 *   - Type and structure sizes enable cross-platform compatibility
 *   - Uses fput_string() for error-checked output
 */
static void
wr_header PARM_1(FILE *, filep)
{
  int i;

  /* first send out the version and patchlevel */
  sprintf(string, "%s\n", VERSION);
  fput_string(string, filep);
  sprintf(string, "%d\n", PATCHLEVEL);
  fput_string(string, filep);

  /* send out all of the type sizes */
  for (i = 0; i < DIO_NUMBER; i++) {
    sprintf(string, "%d\n", dio_types[i].size);
    fput_string(string, filep);
  }

  /* now all of the structure sizes */
  for (i = 0; i < DIOS_NUMBER; i++) {
    sprintf(string, "%d\n", dio_structs[i].size);
    fput_string(string, filep);
  }
}

/*
 * wr_unumlist - Write unit numbering list to data file
 *
 * Serializes a linked list of unit numbering structures to the data file.
 * Each UNITNUM structure is written as a binary block, and the function
 * verifies that the actual count matches the expected count.
 *
 * Parameters:
 *   out_stream - Output file stream (must be valid and writable)
 *   unum_list - Linked list of UNITNUM structures to write
 *   num_unum - Expected number of unit numbering entries
 *
 * Returns:
 *   None (terminates program on error)
 *
 * Side Effects:
 *   - Writes binary UNITNUM data to file
 *   - Calls abrt() if write fails or count mismatch occurs
 *
 * Testing Notes:
 *   Category: B (Integration) - Requires linked list traversal and file I/O
 *   Approach: Integration testing with mock linked lists and controlled file operations
 *   Key Tests: Linked list iteration, count validation, fwrite() error handling
 *   Dependencies: UNUM_PTR linked list structure, fwrite(), fupdate global
 *   Mock Requirements: Mock FILE* operations, constructed UNITNUM linked lists
 *   Complexity: Moderate - Linked list processing with count verification and error handling
 *
 * Notes:
 *   - Validates data integrity by checking written count vs expected
 *   - Part of the larger data persistence system
 *   - Uses fwrite() for binary data output
 */
static void
wr_unumlist PARM_3(FILE *, out_stream, UNUM_PTR, unum_list, int, num_unum)
{
  int count, bytes;

  /* go through all of the data counting it */
  for (count = 0; unum_list != NULL; unum_list = unum_list->next, count++) {

    /* write it out */
    if ((bytes = fwrite(unum_list, sizeof(UNITNUM), 1, out_stream)) != 1) {
      fprintf(fupdate, "\nERROR: unum data not written\n");
      fprintf(fupdate, "Wrong data size (%d vs. 1)\n", bytes);
      abrt();
    }
  }

  /* check the data */
  if (count != num_unum) {
    fprintf(fupdate, "\nERROR: mismatch in unum count\n");
    fprintf(fupdate, "Result %d should have been %d.\n", count, num_unum);
    abrt();
  }
}

/*
 * wr_maplist - Write sector mapping list to data file
 *
 * Serializes a linked list of map structures to the data file. Each
 * MAP_STRUCT contains sector mapping information and is written as a
 * binary block. Validates the written count against expected count.
 *
 * Parameters:
 *   out_stream - Output file stream (must be valid and writable)
 *   map_list - Linked list of MAP_STRUCT structures to write
 *   num_maps - Expected number of map entries
 *
 * Returns:
 *   None (terminates program on error)
 *
 * Side Effects:
 *   - Writes binary MAP_STRUCT data to file
 *   - Calls abrt() if write fails or count mismatch occurs
 *
 * Testing Notes:
 *   Category: B (Integration) - Similar to wr_unumlist with MAP_STRUCT processing
 *   Approach: Integration testing with mock linked lists and file operations
 *   Key Tests: MAP_STRUCT iteration, count validation, binary write operations
 *   Dependencies: MAP_PTR linked list structure, fwrite(), fupdate global
 *   Mock Requirements: Mock FILE* operations, constructed MAP_STRUCT linked lists
 *   Complexity: Moderate - Identical pattern to wr_unumlist with different data types
 *
 * Notes:
 *   - Part of the sector mapping persistence system
 *   - Ensures data integrity through count validation
 *   - Used for storing player-specific map information
 */
static void
wr_maplist PARM_3(FILE *, out_stream, MAP_PTR, map_list, int, num_maps)
{
  int count, bytes;

  /* go through all of the data counting it */
  for (count = 0; map_list != NULL; map_list = map_list->next, count++) {

    /* write it out */
    if ((bytes = fwrite(map_list, sizeof(MAP_STRUCT), 1, out_stream)) != 1) {
      fprintf(fupdate, "\nERROR: map data not written\n");
      fprintf(fupdate, "Wrong data size (%d vs. 1)\n", bytes);
      abrt();
    }
  }

  /* check the data */
  if (count != num_maps) {
    fprintf(fupdate, "\nERROR: mismatch in map count\n");
    fprintf(fupdate, "Result %d should have been %d.\n", count, num_maps);
    abrt();
  }
}

/*
 * write_data - Save complete game state to data file
 *
 * Writes the entire game world state to a data file, including all nations,
 * armies, navies, cities, caravans, and items. The function handles data
 * conversion for older patch levels, compresses output if enabled, and
 * performs extensive error checking throughout the process.
 *
 * Parameters:
 *   None (uses global world data structures)
 *
 * Returns:
 *   None (terminates program on any error)
 *
 * Side Effects:
 *   - Creates temporary data file (.tmp extension)
 *   - Writes complete game state in binary format
 *   - Removes old data file and renames temporary file
 *   - May add spell caster units for data conversion
 *   - Uses compression if COMPRESS is defined
 *
 * Testing Notes:
 *   Category: C (System Level) - Requires complete game world state and file system
 *   Approach: System testing with full world initialization or extensive mocking
 *   Key Tests: File creation, compression handling, world serialization, atomic file operations
 *   Dependencies: Complete world state, nation lists, all game entities, file system
 *   Mock Requirements: Entire game world or extensive I/O mocking framework
 *   Complexity: Extremely Complex - 270+ lines, full world persistence, atomic operations
 *
 * Notes:
 *   - Critical function for game persistence
 *   - Handles backward compatibility through convert_level
 *   - Extensive DEBUG output shows data writing progress
 *   - File operations are atomic (temp file renamed at end)
 */
void
write_data PARM_0(void)
{
  FILE *fdata;
  int loop, ntn_count, checkval;
  long bytes;
#ifdef DEBUG
  int army_count=0, navy_count=0, city_count=0, cvn_count=0, item_count=0;
  long sum_bytes=0L;
#endif /*DEBUG*/
  char sys_str[BIGLTH];

  /* try to open the output file */
  sprintf(string, "%s.tmp", datafile);
#ifdef COMPRESS
  sprintf(sys_str, "%s > %s", COMP_NAME, string);
  if ((fdata = popen(sys_str, "w")) == NULL) {
    fprintf(fupdate, "\nCannot open pipe to compress for writing data.\n");
    abrt();
  }
#else
  if ((fdata = fopen(string, "w")) == NULL) {
    fprintf(fupdate, "\nCannot open file for writing data.  Check permissions\n");
    abrt();
  }
#endif /* COMPRESS */

  /* if necessary, add the new spell caster units to the data */
  if ((convert_level != 0) &&
      (convert_level < 26)) {
    errormsg("== Adding new spell casting units");
    for (country = 0; country < MAXNTN; country++) {
      if (((ntn_ptr = world.np[country]) != NULL) &&
	  n_isntn(ntn_ptr->active)) {
	give_spellcasters();
      }
    }
  }

  /* assure that the structures are properly configured */
  verify_data(__FILE__, __LINE__);
  align_data();
  if (!in_curses) {
    fprintf(fupdate, "\nWriting data");
  }

  /* write out the data header */
  wr_header(fdata);

  /* write the world structure */
  if ( (bytes = fwrite(&world, sizeof(struct s_world), 1, fdata)) != 1) {
    fprintf(fupdate, "\nERROR: world structure not written\n");
    fprintf(fupdate, "Wrong data size (%ld vs. 1)\n", bytes);
    abrt();
  }

  /* write the sector information */
  checkval = MAPX * MAPY;
  if ((bytes = fwrite(*sct, sizeof(SCT_STRUCT), checkval, fdata))
      != checkval) {
    fprintf(fupdate, "\nERROR: sector data not written\n");
    fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, checkval);
    abrt();
  }
#ifdef DEBUG
  if (!in_curses) {
    fprintf(fupdate, "\n%ld bytes of sector data written\n",
	    bytes  * (long) sizeof(SCT_STRUCT));
  }
#endif /*DEBUG*/

  /* write all of the nation information */
  ntn_count = 1;
  for (loop=0; loop < ABSMAXNTN; loop++) {
    if ((ntn_ptr = world.np[loop]) != NULL) {
      if ((bytes = fwrite(ntn_ptr, sizeof(NTN_STRUCT), 1, fdata)) != 1) {
	fprintf(fupdate, "\nERROR: nation data not written\n");
	fprintf(fupdate, "Wrong data size (%ld vs. 1)\n", bytes);
	abrt();
      }
      ntn_count++;
#ifdef DEBUG
      sum_bytes += sizeof(NTN_STRUCT);
#endif /*DEBUG*/

      /* store army units */
      checkval = 0;
      for (army_ptr = ntn_ptr->army_list; army_ptr != NULL;
	   army_ptr = army_ptr->next) {
	if ((bytes = fwrite(army_ptr, sizeof(ARMY_STRUCT), 1, fdata)) !=
	    1) {
	  fprintf(fupdate, "\nERROR: army data not written\n");
	  fprintf(fupdate, "Wrong data size (%ld vs. 1)\n", bytes);
	  abrt();
	}
	checkval++;
#ifdef DEBUG
	sum_bytes += sizeof(ARMY_STRUCT);
	army_count++;
#endif /*DEBUG*/
      }
      if (checkval != ntn_ptr->num_army) {
	fprintf(fupdate, "\nERROR: wrong number of armies stored\n");
	fprintf(fupdate, "Nation %s (%d): %d should be %d\n", ntn_ptr->name,
		ntn_count-1, checkval, ntn_ptr->num_army );
	abrt();
      }

      /* store naval units */
      checkval = 0;
      for (navy_ptr = ntn_ptr->navy_list; navy_ptr!=NULL;
	   navy_ptr = navy_ptr->next) {
	if ((bytes = fwrite(navy_ptr, sizeof(NAVY_STRUCT), 1, fdata)) != 1) {
	  fprintf(fupdate, "\nERROR: navy data not written\n");
	  fprintf(fupdate, "Wrong data size (%ld vs. 1)\n", bytes);
	  abrt();
	}
	checkval++;
#ifdef DEBUG
	sum_bytes += sizeof(NAVY_STRUCT);
	navy_count++;
#endif /*DEBUG*/
      }
      if (checkval != ntn_ptr->num_navy) {
	fprintf(fupdate, "\nERROR: wrong number of navies stored\n");
	fprintf(fupdate, "Nation %s (%d): %d should be %d\n", ntn_ptr->name,
		ntn_count-1, checkval, ntn_ptr->num_navy );
	abrt();
      }

      /* store city information */
      checkval = 0;
      for (city_ptr = ntn_ptr->city_list; city_ptr != NULL;
	   city_ptr = city_ptr->next) {
	if ((bytes = fwrite(city_ptr, sizeof(CITY_STRUCT), 1, fdata)) != 1) {
	  fprintf(fupdate, "\nERROR: city data not written\n");
	  fprintf(fupdate, "Wrong data size (%ld vs. 1)\n", bytes);
	  abrt();
	}
	checkval++;
#ifdef DEBUG
	sum_bytes += sizeof(CITY_STRUCT);
	city_count++;
#endif /*DEBUG*/
      }
      if (checkval != ntn_ptr->num_city) {
	fprintf(fupdate, "\nERROR: wrong number of cities stored\n");
	fprintf(fupdate, "Nation %s (%d): %d should be %d\n", ntn_ptr->name,
		ntn_count-1, checkval, ntn_ptr->num_city );
	abrt();
      }

      /* store caravan information */
      checkval = 0;
      for (cvn_ptr = ntn_ptr->cvn_list; cvn_ptr != NULL;
	   cvn_ptr = cvn_ptr->next) {
	if ((bytes = fwrite(cvn_ptr, sizeof(CVN_STRUCT), 1, fdata)) != 1) {
	  fprintf(fupdate, "\nERROR: caravan data not written\n");
	  fprintf(fupdate, "Wrong data size (%ld vs. 1)\n", bytes);
	  abrt();
	}
	checkval++;
#ifdef DEBUG
	sum_bytes += sizeof(CVN_STRUCT);
	cvn_count++;
#endif /*DEBUG*/
      }
      if (checkval != ntn_ptr->num_cvn) {
	fprintf(fupdate, "\nERROR: wrong number of caravans stored\n");
	fprintf(fupdate, "Nation %s (%d): %d should be %d\n", ntn_ptr->name,
		ntn_count-1, checkval, ntn_ptr->num_cvn );
	abrt();
      }

      /* store item information */
      checkval = 0;
      for (item_ptr = ntn_ptr->item_list; item_ptr != NULL;
	   item_ptr = item_ptr->next) {
	if ((bytes = fwrite(item_ptr, sizeof(ITEM_STRUCT), 1, fdata)) != 1) {
	  fprintf(fupdate, "\nERROR: commodities data not written\n");
	  fprintf(fupdate, "Wrong data size (%ld vs. 1)\n", bytes);
	  abrt();
	}
	checkval++;
#ifdef DEBUG
	sum_bytes += sizeof(ITEM_STRUCT);
	item_count++;
#endif /*DEBUG*/
      }
      if (checkval != ntn_ptr->num_item) {
	fprintf(fupdate, "\nERROR: wrong number of commodities stored\n");
	fprintf(fupdate, "Nation %s (%d): %d should be %d\n", ntn_ptr->name,
		ntn_count-1, checkval, ntn_ptr->num_item );
	abrt();
      }

      /* now store the unit numbering stuff */
      wr_unumlist(fdata, ntn_ptr->unum_list, ntn_ptr->num_unum);

      /* add the sector mapping stuff */
      wr_maplist(fdata, ntn_ptr->map_list, ntn_ptr->num_maps);

#ifdef DEBUG
      /* final checks */
      if (!in_curses) {
	fprintf(fupdate,
		"Stored nation %s: %d armies, %d navies, %d cities,\n",
		ntn_ptr->name, ntn_ptr->num_army, ntn_ptr->num_navy,
		ntn_ptr->num_city);
	fprintf(fupdate, "\t%d caravans, %d commodities.\n",
		ntn_ptr->num_cvn, ntn_ptr->num_item);
      }
#endif /*DEBUG*/
    }
  }
  if (ntn_count != world.nations) {
    fprintf(fupdate, "\nERROR: wrong number of nations written\n");
    fprintf(fupdate, "Stored %d instead of %d\n", ntn_count, world.nations);
    abrt();
  }
#ifdef DEBUG
  if (!in_curses) {
    fprintf(fupdate, "%ld bytes of nation data written\n", sum_bytes);
    fprintf(fupdate, "Stored: \t%d nations, %d armies, %d navies,\n",
	    ntn_count - 1, army_count, navy_count);
    fprintf(fupdate, "\t\t%d cities, %d caravans, %d commodities\n",
	    city_count, cvn_count, item_count);
  }
#endif /*DEBUG*/

  /* now store the unit numbering stuff */
  wr_unumlist(fdata, world.dflt_unum, world.num_unum);

  /* close the file descriptor */
#ifdef COMPRESS
  (void) pclose(fdata);
#else
  (void) fclose(fdata);
#endif /* COMPRESS */

  /* remove prior data file */
#ifdef COMPRESS
  sprintf(string, "%s%s", datafile, COMP_SFX);
#else
#ifndef VMS
  strcpy(string, datafile);
#else
  sprintf(string, "%s.", datafile);
#endif
#endif /* COMPRESS */
#ifdef DEBUG
  if (!in_curses) {
    fprintf(fupdate, "Removing %s\n", string);
  }
#endif /* DEBUG */
  unlink(string);

  /* rename the data file */
  (void) sprintf(sys_str, "%s.tmp", datafile);
#ifdef DEBUG
  if (!in_curses) {
    fprintf(fupdate, "rename %s => %s\n", sys_str, string);
  }
#endif /* DEBUG */
  if (move_file(sys_str, string) != 0) {
    fprintf(fupdate, "\nMAJOR ERROR: unable to rename %s to %s\n",
	    sys_str, string);
    abrt();
  }

#ifdef DEBUG
  if (!in_curses) {
    fprintf(fupdate, "Writing data");
  }
#endif /*DEBUG*/
  if (!in_curses) {
    fprintf(fupdate, "...done\n");
  }
}

/* storage for the actual sizes in the data file */
static int actual_types[DIO_NUMBER];
static int actual_structs[DIOS_NUMBER];
static long sum_bytes = 0L, bytes;

/*
 * set_convert - Configure data conversion settings for patch level compatibility
 *
 * Sets up conversion parameters when reading data files from older patch
 * levels. Determines whether the data file can be successfully converted
 * to the current format based on the patch level difference.
 *
 * Parameters:
 *   level - Patch level of the data file being read
 *
 * Returns:
 *   TRUE if conversion failed/not supported, FALSE if conversion possible
 *
 * Side Effects:
 *   - Sets global convert_level variable
 *   - Affects subsequent data reading operations
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple logic function with clear input/output contract
 *   Approach: Unit testing with various patch level inputs and validation of conversion logic
 *   Key Tests: Boundary conditions (level 23), current patch level, invalid levels
 *   Dependencies: Global convert_level variable, PATCHLEVEL constant
 *   Mock Requirements: Mock PATCHLEVEL constant for testing different scenarios
 *   Complexity: Simple - Clear conditional logic with minimal dependencies
 *
 * Notes:
 *   - Only supports conversion from patch levels 23 and higher
 *   - Older patch levels require special handling during data reads
 *   - Used to maintain backward compatibility with older save files
 */
static int
set_convert PARM_1(int, level)
{
  int hold = FALSE;

  /* convert based on the given level */
  if (level < PATCHLEVEL) {
    convert_level = level;
    if (level < 23) {
      hold = TRUE;
    }
  } else {
    hold = TRUE;
  }
  return(hold);
}

/*
 * rd_header - Read and validate data file header information
 *
 * Reads the data file header containing version, patch level, and type/structure
 * size information. Validates compatibility with the current game version and
 * sets up conversion parameters if needed for older data files.
 *
 * Parameters:
 *   filep - Input file stream (must be valid and readable)
 *
 * Returns:
 *   FALSE on successful header read, TRUE on error or incompatibility
 *
 * Side Effects:
 *   - Reads version string and validates against VERSION
 *   - Reads patch level and sets up conversion if needed
 *   - Reads and validates all type and structure sizes
 *   - May set convert_level for older data files
 *
 * Testing Notes:
 *   Category: B (Integration) - Requires file I/O, global arrays, and error reporting infrastructure
 *   Approach: Integration testing with mock files and controlled global state
 *   Key Tests: Version validation, patch level conversion, type size validation, error conditions
 *   Dependencies: Global fupdate, VERSION, PATCHLEVEL, dio_types, dio_structs arrays
 *   Mock Requirements: Mock FILE* operations, mock global arrays for validation
 *   Complexity: Moderate - File parsing with extensive validation and error handling
 *
 * Notes:
 *   - Critical for ensuring data file compatibility
 *   - Type size validation prevents platform-specific issues
 *   - Enables automatic conversion of older save files
 *   - Returns TRUE to indicate failure (unusual convention)
 */
static int
rd_header PARM_1(FILE *, filep)
{
  char line_data[BIGLTH];
  int i;

  /* read and compare the version information */
  line_data[BIGLTH - 1] = '\0';
  if (fgets(line_data, BIGLTH, filep) == NULL) {
    syserr_msg("\nunable to read from data file");
    return(TRUE);
  }
  line_data[strlen(line_data) - 1] = '\0';
  if (strcmp(line_data, VERSION) != 0) {
    fprintf(fupdate, "This is not a Conquer %s data file\n",
	    VERSION);
    return(TRUE);
  }

  /* read and compare with the patchlevel */
  if (fgets(line_data, BIGLTH, filep) == NULL) {
    syserr_msg("\nunexpected end of data file");
    return(TRUE);
  }
  if (sscanf(line_data, "%d", &i) != 1) {
    fprintf(fupdate, "\nunable to read the patchlevel\n");
    return(TRUE);
  }
  if (i != PATCHLEVEL) {
    if (i < PATCHLEVEL) {
      fprintf(fupdate, "\nWarning: this is a patchlevel %d data file\n",
	      i);
      fprintf(fupdate, "Setting up for automatic conversion\n");
      if (set_convert(i) == TRUE) {
	fprintf(fupdate, "...conversion failed.\n");
	return(TRUE);
      }
    } else {
      fprintf(fupdate, "\nThis is not a Conquer %s.%d data file\n",
	      VERSION, PATCHLEVEL);
      return(TRUE);
    }
  }

  /* read in all of the type sizes */
  for (i = 0; i < DIO_NUMBER; i++) {
    if (fgets(line_data, BIGLTH, filep) == NULL) {
      syserr_msg("\nunexpected end of data file");
      return(TRUE);
    }
    if (sscanf(line_data, "%d", &(actual_types[i])) != 1) {
      fprintf(fupdate, "\nunable to read the %s value\n",
	      dio_types[i].name);
      return(TRUE);
    }

    /* comparison check */
    if (actual_types[i] != dio_types[i].size) {
      fprintf(fupdate, "\nThe %s value was incompatible\n",
	      dio_types[i].name);
      return(TRUE);
    }
  }

  /* now all of the structure sizes */
  for (i = 0; i < DIOS_NUMBER; i++) {
    if (fgets(line_data, BIGLTH, filep) == NULL) {
      syserr_msg("\nunexpected end of data file");
      return(TRUE);
    }
    if (sscanf(line_data, "%d", &(actual_structs[i])) != 1) {
      fprintf(fupdate, "\nunable to read the %s value\n",
	      dio_structs[i].name);
      return(TRUE);
    }

    /* comparison check */
    if ((convert_level == 0) &&
	(actual_structs[i] != dio_structs[i].size)) {
      fprintf(fupdate, "\nThe %s value was incompatible\n",
	      dio_structs[i].name);
      return(TRUE);
    }
  }
  return(FALSE);
}

/*
 * rd_worlddata - Read world data structure with version conversion support
 *
 * Reads the main world data structure from the data file, handling conversion
 * from older patch level formats (24-27) to the current format. Each patch
 * level has specific conversion logic to preserve game state while updating
 * to newer data structures.
 *
 * Parameters:
 *   in_stream - Input file stream (must be valid and readable)
 *
 * Returns:
 *   FALSE on successful read, TRUE on error
 *
 * Side Effects:
 *   - Reads world structure based on convert_level
 *   - Initializes new fields for converted data
 *   - Sets global world structure with read data
 *
 * Testing Notes:
 *   Category: C (System Level) - Extremely complex function requiring complete world structure knowledge
 *   Approach: System testing with full game state or extensive mocking of world structures
 *   Key Tests: Each patch level conversion (24-27), normal read, field preservation, new field initialization
 *   Dependencies: Complete world structure, convert_level global, multiple versioned structures
 *   Mock Requirements: Extensive mocking of world structures and file I/O operations
 *   Complexity: Extremely Complex - Multiple conversion paths, large data structures, version-specific logic
 *
 * Notes:
 *   - Handles conversion from patch levels 24, 25, 26, 27 to current
 *   - Each conversion preserves existing data and initializes new fields
 *   - Critical for maintaining save game compatibility across versions
 *   - Returns TRUE to indicate failure (unusual convention)
 */
static int
rd_worlddata PARM_1(FILE *, in_stream)
{
  int i;
  struct s24_world world24_data;
  struct s25_world world25_data;
  struct s26_world world26_data;
  struct s27_world world27_data;

  /* to convert or not to convert */
  if ((convert_level > 27) || (convert_level == 0)) {

    /* normal read */
    if ((bytes = fread(&world, sizeof(struct s_world), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: world structure not read in\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      return(TRUE);
    }
#ifdef DEBUG
    if (!in_curses) {
      fprintf(fupdate, "\n%ld bytes of world data read\n", bytes *
	      (long) sizeof(struct s_world));
    }
#endif /*DEBUG*/

  } else if (convert_level == 27) {

    /* read in old data and then convert it */
    if ((bytes = fread(&(world27_data),
		       sizeof(struct s27_world), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: world data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      return(TRUE);
    }
#ifdef DEBUG
    if (!in_curses) {
      fprintf(fupdate, "\n%ld bytes of world data read\n", bytes *
	      (long) sizeof(struct s27_world));
    }
#endif /*DEBUG*/

    /* copy the information into a new data structure */
    world.mapx = world27_data.mapx;
    world.mapy = world27_data.mapy;
    world.nations = world27_data.nations;
    world.npcs = world27_data.npcs;
    world.active_ntns = world27_data.active_ntns;
    world.turn = world27_data.turn;
    world.start_turn = world27_data.start_turn;
    strcpy(world.demigod, world27_data.demigod);
    strcpy(world.passwd, world27_data.passwd);
    strcpy(world.addpwd, world27_data.addpwd);
    world.demibuild = world27_data.demibuild;
    world.demilimit = world27_data.demilimit;
    world.relative_map = world27_data.relative_map;
    world.hexmap = world27_data.hexmap;
    world.verify_login = world27_data.verify_login;
    world.hide_scores = world27_data.hide_scores;
    world.hide_login = world27_data.hide_login;
    world.builddist = world27_data.builddist;
    world.fleet_cbval = world27_data.fleet_cbval;
    world.wagon_cbval = world27_data.wagon_cbval;
    world.num_dice = world27_data.num_dice;
    world.avg_damage = world27_data.avg_damage;
    world.damage_limit = world27_data.damage_limit;
    world.overmatch_adj = world27_data.overmatch_adj;
    world.pmindamage = world27_data.pmindamage;
    world.smoothings = world27_data.smoothings;
    world.land_range = world27_data.land_range;
    world.preject_land = world27_data.preject_land;
    world.pwater = world27_data.pwater;
    world.pmount = world27_data.pmount;
    world.pmercval = world27_data.pmercval;
    world.prevolt = world27_data.prevolt;
    world.pscout = world27_data.pscout;
    world.pmonster = world27_data.pmonster;
    world.pmercmonst = world27_data.pmercmonst;
    world.ptrade = world27_data.ptrade;
    world.ptgmetal = world27_data.ptgmetal;
    world.ptgjewel = world27_data.ptgjewel;
    world.ptgspell = world27_data.ptgspell;
    world.supplylimit = world27_data.supplylimit;
    world.nvsplydist = world27_data.nvsplydist;
    world.cityxfdist = world27_data.cityxfdist;
    world.maxpts = world27_data.maxpts;
    world.lizards = world27_data.lizards;
    world.savages = world27_data.savages;
    world.nomads = world27_data.nomads;
    world.pirates = world27_data.pirates;
    world.numrandom = world27_data.numrandom;
    world.numweather = world27_data.numweather;
    world.m_mil = world27_data.m_mil;
    world.m_aplus = world27_data.m_aplus;
    world.m_dplus = world27_data.m_dplus;
    world.bribelevel = world27_data.bribelevel;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      world.w_mtrls[i] = world27_data.w_mtrls[i];
    }
    world.w_civ = world27_data.w_civ;
    world.w_mil = world27_data.w_mil;
    world.w_sctrs = world27_data.w_sctrs;
    world.w_score = world27_data.w_score;
    world.mbox_size = world27_data.mbox_size;
    world.news_size = world27_data.news_size;
    world.num_unum = world27_data.num_unum;
    world.exposelvl = world27_data.exposelvl;
    world.dflt_unum = NULL;
    world.maxdipadj = world27_data.maxdipadj;

    /* initialize the new elements */
    world.latestart = 5;
    world.growthrate = 10;

  } else if (convert_level == 26) {

    /* read in old data and then convert it */
    if ((bytes = fread(&(world26_data),
		       sizeof(struct s26_world), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: world data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      return(TRUE);
    }
#ifdef DEBUG
    if (!in_curses) {
      fprintf(fupdate, "\n%ld bytes of world data read\n", bytes *
	      (long) sizeof(struct s26_world));
    }
#endif /*DEBUG*/

    /* copy the information into a new data structure */
    world.mapx = world26_data.mapx;
    world.mapy = world26_data.mapy;
    world.nations = world26_data.nations;
    world.npcs = world26_data.npcs;
    world.active_ntns = world26_data.active_ntns;
    world.turn = world26_data.turn;
    world.start_turn = world26_data.start_turn;
    strcpy(world.demigod, world26_data.demigod);
    strcpy(world.passwd, world26_data.passwd);
    strcpy(world.addpwd, world26_data.addpwd);
    world.demibuild = world26_data.demibuild;
    world.demilimit = world26_data.demilimit;
    world.relative_map = world26_data.relative_map;
    world.hexmap = world26_data.hexmap;
    world.verify_login = world26_data.verify_login;
    world.hide_scores = world26_data.hide_scores;
    world.hide_login = world26_data.hide_login;
    world.builddist = world26_data.builddist;
    world.fleet_cbval = world26_data.fleet_cbval;
    world.wagon_cbval = world26_data.wagon_cbval;
    world.num_dice = world26_data.num_dice;
    world.avg_damage = world26_data.avg_damage;
    world.damage_limit = world26_data.damage_limit;
    world.overmatch_adj = world26_data.overmatch_adj;
    world.pmindamage = world26_data.pmindamage;
    world.smoothings = world26_data.smoothings;
    world.land_range = world26_data.land_range;
    world.preject_land = world26_data.preject_land;
    world.pwater = world26_data.pwater;
    world.pmount = world26_data.pmount;
    world.pmercval = world26_data.pmercval;
    world.prevolt = world26_data.prevolt;
    world.pscout = world26_data.pscout;
    world.pmonster = world26_data.pmonster;
    world.pmercmonst = world26_data.pmercmonst;
    world.ptrade = world26_data.ptrade;
    world.ptgmetal = world26_data.ptgmetal;
    world.ptgjewel = world26_data.ptgjewel;
    world.ptgspell = world26_data.ptgspell;
    world.supplylimit = world26_data.supplylimit;
    world.nvsplydist = world26_data.nvsplydist;
    world.cityxfdist = world26_data.cityxfdist;
    world.maxpts = world26_data.maxpts;
    world.lizards = world26_data.lizards;
    world.savages = world26_data.savages;
    world.nomads = world26_data.nomads;
    world.pirates = world26_data.pirates;
    world.numrandom = world26_data.numrandom;
    world.numweather = world26_data.numweather;
    world.m_mil = world26_data.m_mil;
    world.m_aplus = world26_data.m_aplus;
    world.m_dplus = world26_data.m_dplus;
    world.bribelevel = world26_data.bribelevel;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      world.w_mtrls[i] = world26_data.w_mtrls[i];
    }
    world.w_civ = world26_data.w_civ;
    world.w_mil = world26_data.w_mil;
    world.w_sctrs = world26_data.w_sctrs;
    world.w_score = world26_data.w_score;
    world.mbox_size = world26_data.mbox_size;
    world.news_size = world26_data.news_size;
    world.num_unum = world26_data.num_unum;
    world.exposelvl = world26_data.exposelvl;
    world.dflt_unum = NULL;

    /* initialize the new elements */
    world.maxdipadj = 2;
    world.latestart = 5;
    world.growthrate = 10;

  } else if (convert_level == 25) {

    /* read in old data and then convert it */
    if ((bytes = fread(&(world25_data),
		       sizeof(struct s25_world), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: world data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      return(TRUE);
    }
#ifdef DEBUG
    if (!in_curses) {
      fprintf(fupdate, "\n%ld bytes of world data read\n", bytes *
	      (long) sizeof(struct s25_world));
    }
#endif /*DEBUG*/

    /* copy the information into a new data structure */
    world.mapx = world25_data.mapx;
    world.mapy = world25_data.mapy;
    world.nations = world25_data.nations;
    world.npcs = world25_data.npcs;
    world.active_ntns = world25_data.active_ntns;
    world.turn = world25_data.turn;
    world.start_turn = world25_data.start_turn;
    strcpy(world.demigod, world25_data.demigod);
    strcpy(world.passwd, world25_data.passwd);
    strcpy(world.addpwd, world25_data.addpwd);
    world.demibuild = world25_data.demibuild;
    world.demilimit = world25_data.demilimit;
    world.relative_map = world25_data.relative_map;
    world.hexmap = world25_data.hexmap;
    world.verify_login = world25_data.verify_login;
    world.hide_scores = world25_data.hide_scores;
    world.hide_login = world25_data.hide_login;
    world.builddist = world25_data.builddist;
    world.fleet_cbval = world25_data.fleet_cbval;
    world.wagon_cbval = world25_data.wagon_cbval;
    world.num_dice = world25_data.num_dice;
    world.avg_damage = world25_data.avg_damage;
    world.damage_limit = world25_data.damage_limit;
    world.overmatch_adj = world25_data.overmatch_adj;
    world.pmindamage = world25_data.pmindamage;
    world.smoothings = world25_data.smoothings;
    world.land_range = world25_data.land_range;
    world.preject_land = world25_data.preject_land;
    world.pwater = world25_data.pwater;
    world.pmount = world25_data.pmount;
    world.pmercval = world25_data.pmercval;
    world.prevolt = world25_data.prevolt;
    world.pscout = world25_data.pscout;
    world.pmonster = world25_data.pmonster;
    world.pmercmonst = world25_data.pmercmonst;
    world.ptrade = world25_data.ptrade;
    world.ptgmetal = world25_data.ptgmetal;
    world.ptgjewel = world25_data.ptgjewel;
    world.ptgspell = world25_data.ptgspell;
    world.supplylimit = world25_data.supplylimit;
    world.nvsplydist = world25_data.nvsplydist;
    world.cityxfdist = world25_data.cityxfdist;
    world.maxpts = world25_data.maxpts;
    world.lizards = world25_data.lizards;
    world.savages = world25_data.savages;
    world.nomads = world25_data.nomads;
    world.pirates = world25_data.pirates;
    world.numrandom = world25_data.numrandom;
    world.numweather = world25_data.numweather;
    world.m_mil = world25_data.m_mil;
    world.m_aplus = world25_data.m_aplus;
    world.m_dplus = world25_data.m_dplus;
    world.bribelevel = world25_data.bribelevel;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      world.w_mtrls[i] = world25_data.w_mtrls[i];
    }
    world.w_civ = world25_data.w_civ;
    world.w_mil = world25_data.w_mil;
    world.w_sctrs = world25_data.w_sctrs;
    world.w_score = world25_data.w_score;
    world.mbox_size = world25_data.mbox_size;
    world.news_size = world25_data.news_size;
    world.num_unum = world25_data.num_unum;
    world.dflt_unum = NULL;

    /* initialize the new elements */
    world.exposelvl = 10;
    world.maxdipadj = 2;
    world.latestart = 5;
    world.growthrate = 10;

  } else {

    /* read in old data and then convert it */
    if ((bytes = fread(&(world24_data),
		       sizeof(struct s24_world), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: world data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      return(TRUE);
    }
#ifdef DEBUG
    if (!in_curses) {
      fprintf(fupdate, "\n%ld bytes of world data read\n", bytes *
	      (long) sizeof(struct s24_world));
    }
#endif /*DEBUG*/

    /* copy the information into a new data structure */
    world.mapx = world24_data.mapx;
    world.mapy = world24_data.mapy;
    world.nations = world24_data.nations;
    world.npcs = world24_data.npcs;
    world.active_ntns = world24_data.active_ntns;
    world.turn = world24_data.turn;
    world.start_turn = world24_data.start_turn;
    strcpy(world.demigod, world24_data.demigod);
    strcpy(world.passwd, world24_data.passwd);
    world.demibuild = world24_data.demibuild;
    world.demilimit = world24_data.demilimit;
    world.relative_map = world24_data.relative_map;
    world.hexmap = world24_data.hexmap;
    world.verify_login = world24_data.verify_login;
    world.hide_scores = world24_data.hide_scores;
    world.builddist = world24_data.builddist;
    world.fleet_cbval = world24_data.fleet_cbval;
    world.wagon_cbval = world24_data.wagon_cbval;
    world.num_dice = world24_data.num_dice;
    world.avg_damage = world24_data.avg_damage;
    world.damage_limit = world24_data.damage_limit;
    world.overmatch_adj = world24_data.overmatch_adj;
    world.pmindamage = world24_data.pmindamage;
    world.smoothings = world24_data.smoothings;
    world.land_range = world24_data.land_range;
    world.preject_land = world24_data.preject_land;
    world.pwater = world24_data.pwater;
    world.pmount = world24_data.pmount;
    world.pmercval = world24_data.pmercval;
    world.prevolt = world24_data.prevolt;
    world.pscout = world24_data.pscout;
    world.pmonster = world24_data.pmonster;
    world.pmercmonst = world24_data.pmercmonst;
    world.ptrade = world24_data.ptrade;
    world.ptgmetal = world24_data.ptgmetal;
    world.ptgjewel = world24_data.ptgjewel;
    world.ptgspell = world24_data.ptgspell;
    world.supplylimit = world24_data.supplylimit;
    world.nvsplydist = world24_data.nvsplydist;
    world.cityxfdist = world24_data.cityxfdist;
    world.maxpts = world24_data.maxpts;
    world.lizards = world24_data.lizards;
    world.savages = world24_data.savages;
    world.nomads = world24_data.nomads;
    world.pirates = world24_data.pirates;
    world.numrandom = world24_data.numrandom;
    world.numweather = world24_data.numweather;
    world.m_mil = world24_data.m_mil;
    world.m_aplus = world24_data.m_aplus;
    world.m_dplus = world24_data.m_dplus;
    world.bribelevel = world24_data.bribelevel;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      world.w_mtrls[i] = world24_data.w_mtrls[i];
    }
    world.w_civ = world24_data.w_civ;
    world.w_mil = world24_data.w_mil;
    world.w_sctrs = world24_data.w_sctrs;
    world.w_score = world24_data.w_score;
    world.mbox_size = world24_data.mbox_size;
    world.news_size = world24_data.news_size;
    strcpy(world.addpwd, world24_data.addpwd);
    world.hide_login = world24_data.hide_login;

    /* initialize the new elements */
    world.exposelvl = 10;
    world.maxdipadj = 2;
    world.latestart = 5;
    world.growthrate = 10;
    world.num_unum = 0;
    world.dflt_unum = NULL;
    ntn_ptr = NULL;
    unum_defaults();

  }

  /* all done */
  return(FALSE);
}

/*
 * rd_maplist - Read sector mapping list from data file
 *
 * Reads a specified number of MAP_STRUCT entries from the data file and
 * constructs a linked list. Each map structure contains sector mapping
 * information for a nation's explored areas.
 *
 * Parameters:
 *   in_stream - Input file stream (must be valid and readable)
 *   num_maps - Number of map structures to read
 *
 * Returns:
 *   Pointer to head of MAP_STRUCT linked list, NULL on error
 *
 * Side Effects:
 *   - Allocates memory for each map structure using new_map()
 *   - Constructs linked list of map structures
 *   - Frees allocated memory on read errors
 *
 * Testing Notes:
 *   Category: B (Integration) - Requires file I/O, memory allocation, and linked list construction
 *   Approach: Integration testing with mock file operations and controlled memory allocation
 *   Key Tests: Multiple map reading, linked list construction, error handling, memory cleanup
 *   Dependencies: FILE* operations, new_map() allocation, fupdate global, fread()
 *   Mock Requirements: Mock FILE* operations, mock new_map() with controllable failure scenarios
 *   Complexity: Moderate - File I/O with linked list construction and error handling
 *
 * Notes:
 *   - Part of the sector mapping persistence system
 *   - Creates proper linked list structure with next pointers
 *   - Returns NULL on any read error for error handling
 */
static MAP_PTR
rd_maplist PARM_2(FILE *, in_stream, int, num_maps)
{
  MAP_PTR mlist_ptr = NULL, temp_ptr, trace_ptr = NULL;
  int count;

  /* keep reading while not done */
  for (count = 0; count < num_maps; count++) {

    /* read it in */
    temp_ptr = new_map();
    if ((bytes = fread(temp_ptr, sizeof(MAP_STRUCT), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: map data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      free(temp_ptr);
      return((MAP_PTR) NULL);
    }

    /* now align things */
    temp_ptr->next = NULL;
    if (mlist_ptr == NULL) {
      mlist_ptr = temp_ptr;
    } else {
      trace_ptr->next = temp_ptr;
    }
    trace_ptr = temp_ptr;

  }
  return(mlist_ptr);
}

/*
 * rd_unumlist - Read unit numbering list from data file
 *
 * Reads a specified number of UNITNUM structures from the data file and
 * constructs a linked list. These structures track unit numbering for
 * armies, navies, and other game entities.
 *
 * Parameters:
 *   in_stream - Input file stream (must be valid and readable)
 *   num_unum - Number of unit numbering structures to read
 *
 * Returns:
 *   Pointer to head of UNITNUM linked list, NULL on error
 *
 * Side Effects:
 *   - Allocates memory for each UNITNUM structure using new_unum()
 *   - Constructs linked list of unit numbering structures
 *   - Frees allocated memory on read errors
 *
 * Testing Notes:
 *   Category: B (Integration) - Similar to rd_maplist with UNITNUM-specific processing
 *   Approach: Integration testing with mock file operations and UNITNUM allocation
 *   Key Tests: Multiple UNITNUM reading, linked list construction, error scenarios, memory management
 *   Dependencies: FILE* operations, new_unum() allocation, fupdate global, fread()
 *   Mock Requirements: Mock FILE* operations, mock new_unum() with failure testing capabilities
 *   Complexity: Moderate - Identical pattern to rd_maplist with different data types
 *
 * Notes:
 *   - Part of the unit identification and tracking system
 *   - Creates proper linked list structure with next pointers
 *   - Critical for maintaining unit numbering consistency
 */
static UNUM_PTR
rd_unumlist PARM_2(FILE *, in_stream, int, num_unum)
{
  UNUM_PTR ulist_ptr = NULL, temp_ptr, trace_ptr = NULL;
  int count;

  /* keep reading while not done */
  for (count = 0; count < num_unum; count++) {

    /* read it in */
    temp_ptr = new_unum();
    if ((bytes = fread(temp_ptr, sizeof(UNITNUM), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: unitnum data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      free(temp_ptr);
      return((UNUM_PTR) NULL);
    }

    /* now align things */
    temp_ptr->next = NULL;
    if (ulist_ptr == NULL) {
      ulist_ptr = temp_ptr;
    } else {
      trace_ptr->next = temp_ptr;
    }
    trace_ptr = temp_ptr;

  }
  return(ulist_ptr);
}

/*
 * switch_24attr - Convert patch 24 attribute list to current format
 *
 * Converts nation attributes from the old patch 24 format to the current
 * attribute system. Initializes new attributes with default values and
 * copies over existing attributes that are still valid.
 *
 * Parameters:
 *   attr_list - Target attribute array (current format)
 *   oattr_list - Source attribute array (patch 24 format)
 *
 * Returns:
 *   None
 *
 * Side Effects:
 *   - Initializes all new attributes with default starting values
 *   - Copies compatible attributes from old to new format
 *   - Modifies the target attribute array in place
 *
 * Testing Notes:
 *   Category: A (Unit) - Pure data transformation with clear input/output arrays
 *   Approach: Unit testing with controlled attribute arrays and validation of conversion mapping
 *   Key Tests: All attribute mappings, default value initialization, array boundary conditions
 *   Dependencies: bute_info global array, BUTE_NUMBER constant, attribute mapping constants
 *   Mock Requirements: Mock bute_info array with controlled default values
 *   Complexity: Simple - Array processing with clear mapping logic and minimal side effects
 *
 * Notes:
 *   - Part of patch 24 data conversion system
 *   - Maps old attribute indices to new attribute indices
 *   - Ensures backward compatibility with older save files
 *   - Uses bute_info array for default attribute values
 */
static void
switch_24attr PARM_2(short *, attr_list, short *, oattr_list)
{
  int i;

  /* first assign the "starting" values to fill any new items */
  for (i = 0; i < BUTE_NUMBER; i++) {
    attr_list[i] = bute_info[i].start_val;
  }

  /* now copy over the old values that are kept */
  attr_list[BUTE_CHARITY] = oattr_list[OBUTE_CHARITY];
  attr_list[BUTE_COMMRANGE] = oattr_list[OBUTE_COMMRANGE];
  attr_list[BUTE_EATRATE] = oattr_list[OBUTE_EATRATE];
  attr_list[BUTE_HEALTH] = oattr_list[OBUTE_HEALTH];
  attr_list[BUTE_INFLATION] = oattr_list[OBUTE_INFLATION];
  attr_list[BUTE_JEWELWORK] = oattr_list[OBUTE_JEWELWORK];
  attr_list[BUTE_KNOWLEDGE] = oattr_list[OBUTE_KNOWLEDGE];
  attr_list[BUTE_METALWORK] = oattr_list[OBUTE_METALWORK];
  attr_list[BUTE_MINING] = oattr_list[OBUTE_MINING];
  attr_list[BUTE_POPULARITY] = oattr_list[OBUTE_POPULARITY];
  attr_list[BUTE_SPELLPTS] = oattr_list[OBUTE_SPELLPTS];
  attr_list[BUTE_SPOILRATE] = oattr_list[OBUTE_SPOILRATE];
  attr_list[BUTE_TAXRATE] = oattr_list[OBUTE_TAXRATE];
  attr_list[BUTE_TERROR] = oattr_list[OBUTE_TERROR];
}

/*
 * rd_ntndata - Read nation data with version conversion support
 *
 * Reads a nation structure from the data file, handling conversion from
 * older patch levels (24, 25) to the current format. Allocates a new
 * nation structure and populates it with either direct read or converted
 * data depending on the file version.
 *
 * Parameters:
 *   in_stream - Input file stream (must be valid and readable)
 *
 * Returns:
 *   Pointer to newly allocated NTN_STRUCT, NULL on error
 *
 * Side Effects:
 *   - Allocates memory for nation structure using new_ntn()
 *   - Initializes linked list pointers to NULL
 *   - Converts attributes for patch 24 compatibility
 *   - Frees allocated memory on read errors
 *
 * Testing Notes:
 *   Category: B - Integration
 *   Approach: Integration testing with controlled file I/O and nation setup
 *   Key Tests: Version conversion paths, field mapping, error handling, memory allocation
 *   Dependencies: File I/O, global convert_level, nation structure definitions, new_ntn()
 *   Mock Requirements: File stream mocking, nation allocation mocking
 *   Complexity: Complex - 200+ lines with multiple version conversion paths
 *
 * Notes:
 *   - Handles conversion from patch levels 24 and 25
 *   - Initializes new fields added in later versions
 *   - Critical for nation persistence and compatibility
 *   - Returns NULL on any error for proper error handling
 */
static NTN_PTR
rd_ntndata PARM_1(FILE *, in_stream)
{
  NTN_PTR n1_ptr;
  struct s24_nation ntn24_data;
  struct s25_nation ntn25_data;
  int i;

  /* get the data for the structure */
  n1_ptr = new_ntn();

  /* to convert or not to convert */
  if ((convert_level == 0) ||
      (convert_level > 25)) {

    /* normal read */
    if ((bytes = fread(n1_ptr, sizeof(NTN_STRUCT), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: nation data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      free(n1_ptr);
      return((NTN_PTR) NULL);
    }

  } else if (convert_level == 25) {
    
    /* read in old data and then convert it */
    if ((bytes = fread(&(ntn25_data),
		       sizeof(N25_STRUCT), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: nation data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      free(n1_ptr);
      return((NTN_PTR) NULL);
    }

    /* copy the information into a new data structure */
    strcpy(n1_ptr->name, ntn25_data.name);
    strcpy(n1_ptr->login, ntn25_data.login);
    strcpy(n1_ptr->passwd, ntn25_data.passwd);
    strcpy(n1_ptr->leader, ntn25_data.leader);
    n1_ptr->repro = ntn25_data.repro;
    n1_ptr->race = ntn25_data.race;
    n1_ptr->mark = ntn25_data.mark;
    n1_ptr->location = ntn25_data.location;
    n1_ptr->capx = ntn25_data.capx;
    n1_ptr->capy = ntn25_data.capy;
    n1_ptr->centerx = ntn25_data.centerx;
    n1_ptr->centery = ntn25_data.centery;
    n1_ptr->leftedge = ntn25_data.leftedge;
    n1_ptr->rightedge = ntn25_data.rightedge;
    n1_ptr->topedge = ntn25_data.topedge;
    n1_ptr->bottomedge = ntn25_data.bottomedge;
    n1_ptr->class = ntn25_data.class;
    n1_ptr->aplus = ntn25_data.aplus;
    n1_ptr->dplus = ntn25_data.dplus;
    n1_ptr->score = ntn25_data.score;
    n1_ptr->active = ntn25_data.active;
    n1_ptr->maxmove = ntn25_data.maxmove;
    n1_ptr->num_army = ntn25_data.num_army;
    n1_ptr->num_navy = ntn25_data.num_navy;
    n1_ptr->num_city = ntn25_data.num_city;
    n1_ptr->num_item = ntn25_data.num_item;
    n1_ptr->num_cvn = ntn25_data.num_cvn;
    n1_ptr->num_unum = ntn25_data.num_unum;
    n1_ptr->army_list = NULL;
    n1_ptr->navy_list = NULL;
    n1_ptr->city_list = NULL;
    n1_ptr->item_list = NULL;
    n1_ptr->cvn_list = NULL;
    n1_ptr->unum_list = NULL;
    for (i = 0; i < ABSMAXNTN; i++) {
      n1_ptr->dstatus[i] = ntn25_data.dstatus[i];
    }
    for (i = 0; i < MTRLS_NUMBER; i++) {
      n1_ptr->mtrls[i] = ntn25_data.mtrls[i];
      n1_ptr->m_new[i] = (itemtype) 0;
    }
    n1_ptr->tsctrs = ntn25_data.tsctrs;
    n1_ptr->tunsctrs = ntn25_data.tunsctrs;
    n1_ptr->tships = ntn25_data.tships;
    n1_ptr->twagons = ntn25_data.twagons;
    n1_ptr->tmonst = ntn25_data.tmonst;
    n1_ptr->tleaders = ntn25_data.tleaders;
    n1_ptr->tmil = ntn25_data.tmil;
    n1_ptr->tciv = ntn25_data.tciv;
    for (i = 0; i < MAG_NUMBER; i++) {
      n1_ptr->powers[i] = ntn25_data.powers[i];
    }
    for (i = 0; i < BUTE_NUMBER; i++) {
      n1_ptr->attribute[i] = ntn25_data.attribute[i];
    }
    n1_ptr->mbox_size = ntn25_data.mbox_size;
    n1_ptr->news_size = ntn25_data.news_size;

    /* adjust new items */
    n1_ptr->map_list = NULL;
    n1_ptr->num_maps = 0;

  } else {

    /* read in old data and then convert it */
    if ((bytes = fread(&(ntn24_data),
		       sizeof(struct s24_nation), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: nation data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      free(n1_ptr);
      return((NTN_PTR) NULL);
    }

    /* copy the information into a new data structure */
    strcpy(n1_ptr->name, ntn24_data.name);
    strcpy(n1_ptr->login, ntn24_data.login);
    strcpy(n1_ptr->passwd, ntn24_data.passwd);
    strcpy(n1_ptr->leader, ntn24_data.leader);
    n1_ptr->repro = ntn24_data.repro;
    n1_ptr->race = ntn24_data.race;
    n1_ptr->mark = ntn24_data.mark;
    n1_ptr->location = ntn24_data.location;
    n1_ptr->capx = ntn24_data.capx;
    n1_ptr->capy = ntn24_data.capy;
    n1_ptr->centerx = ntn24_data.centerx;
    n1_ptr->centery = ntn24_data.centery;
    n1_ptr->leftedge = ntn24_data.leftedge;
    n1_ptr->rightedge = ntn24_data.rightedge;
    n1_ptr->topedge = ntn24_data.topedge;
    n1_ptr->bottomedge = ntn24_data.bottomedge;
    n1_ptr->class = ntn24_data.class;
    n1_ptr->aplus = ntn24_data.aplus;
    n1_ptr->dplus = ntn24_data.dplus;
    n1_ptr->score = ntn24_data.score;
    n1_ptr->active = ntn24_data.active;
    n1_ptr->maxmove = ntn24_data.maxmove;
    n1_ptr->num_army = ntn24_data.num_army;
    n1_ptr->num_navy = ntn24_data.num_navy;
    n1_ptr->num_city = ntn24_data.num_city;
    n1_ptr->num_item = ntn24_data.num_item;
    n1_ptr->num_cvn = ntn24_data.num_cvn;
    n1_ptr->army_list = NULL;
    n1_ptr->navy_list = NULL;
    n1_ptr->city_list = NULL;
    n1_ptr->item_list = NULL;
    n1_ptr->cvn_list = NULL;
    for (i = 0; i < ABSMAXNTN; i++) {
      n1_ptr->dstatus[i] = ntn24_data.dstatus[i];
    }
    for (i = 0; i < MTRLS_NUMBER; i++) {
      n1_ptr->mtrls[i] = ntn24_data.mtrls[i];
      n1_ptr->m_new[i] = (itemtype) 0;
    }
    n1_ptr->tsctrs = ntn24_data.tsctrs;
    n1_ptr->tunsctrs = ntn24_data.tunsctrs;
    n1_ptr->tships = ntn24_data.tships;
    n1_ptr->twagons = ntn24_data.twagons;
    n1_ptr->tmonst = ntn24_data.tmonst;
    n1_ptr->tleaders = ntn24_data.tleaders;
    n1_ptr->tmil = ntn24_data.tmil;
    n1_ptr->tciv = ntn24_data.tciv;
    for (i = 0; i < MAG_NUMBER; i++) {
      n1_ptr->powers[i] = ntn24_data.powers[i];
    }
    n1_ptr->mbox_size = ntn24_data.mbox_size;
    n1_ptr->news_size = ntn24_data.news_size;

    /* realligning all of the attributes */
    switch_24attr(n1_ptr->attribute, ntn24_data.attribute);

    /* adjust new items */
    n1_ptr->unum_list = NULL;
    n1_ptr->num_unum = 0;
    n1_ptr->map_list = NULL;
    n1_ptr->num_maps = 0;
  }

  /* all done */
  return(n1_ptr);
}

/*
 * p26_shift - Convert army unit types from patch 26 format
 *
 * Adjusts army unit types to account for new spell casting leaders and
 * engineers added in patch 26. Non-leader and magician units are shifted
 * by 2 positions, and units at or above Engineers are shifted by 1.
 *
 * Parameters:
 *   utype - Original unit type from patch 26 data
 *
 * Returns:
 *   Converted unit type for current patch level
 *
 * Side Effects:
 *   - None (pure conversion function)
 *
 * Testing Notes:
 *   Category: A - Unit
 *   Approach: Unit testing with mockable unit type checking functions
 *   Key Tests: Leader type handling, magician type handling, Engineer shift, boundary conditions
 *   Dependencies: unitbyname(), a_isleader(), a_ismagician() functions
 *   Mock Requirements: Unit type checking functions and unitbyname() lookup
 *   Complexity: Simple - 15 lines with clear conditional logic and unit type arithmetic
 *
 * Notes:
 *   - Part of patch 26 army data conversion system
 *   - Accounts for insertion of new spell casting leader types
 *   - Ensures army unit types remain valid after conversion
 *   - Uses unitbyname() and army type checking functions
 */
static int
p26_shift PARM_1(int, utype)
{
  int hold = utype;

  /* account for the new spell casting leaders */
  if (!a_isleader(utype) ||
      a_ismagician(utype)) {
    hold += 2;
  }
  if (hold >= unitbyname("Engineers")) {
    hold += 1;
  }

  /* done */
  return(hold);
}

/*
 * rd_armydata - Read army data with unit type conversion
 *
 * Reads an army structure from the data file and applies unit type
 * conversion if reading from older patch levels. Handles the unit type
 * shifts needed for compatibility with patch 26 changes.
 *
 * Parameters:
 *   in_stream - Input file stream (must be valid and readable)
 *
 * Returns:
 *   Pointer to newly allocated ARMY_STRUCT, NULL on error
 *
 * Side Effects:
 *   - Allocates memory for army structure using new_army()
 *   - Applies unit type conversion for older patch levels
 *   - Frees allocated memory on read errors
 *
 * Testing Notes:
 *   Category: B - Integration
 *   Approach: Integration testing with controlled file I/O and army setup
 *   Key Tests: Version conversion, unit type shifting, error handling, memory allocation
 *   Dependencies: File I/O, global convert_level, new_army(), p26_shift()
 *   Mock Requirements: File stream mocking, army allocation mocking
 *   Complexity: Moderate - 30 lines with version-specific logic and unit type conversion
 *
 * Notes:
 *   - Performs direct read for patch levels > 20
 *   - Applies p26_shift() conversion for older patch levels
 *   - Critical for army persistence and unit type compatibility
 *   - Returns NULL on any error for proper error handling
 */
static ARMY_PTR
rd_armydata PARM_1(FILE *, in_stream)
{
  ARMY_PTR a1_ptr;

  /* get the data for the structure */
  a1_ptr = new_army();

  /* to convert or not to convert */
  if ((convert_level == 0) ||
      (convert_level > 20)) {

    /* normal read */
    if ((bytes = fread(a1_ptr, sizeof(ARMY_STRUCT), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: army data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      free(a1_ptr);
      return((ARMY_PTR) NULL);
    }

  }

  /* perform any necessary additional conversions */
  if (convert_level != 0) {
    if (convert_level < 26) {
      a1_ptr->unittype = p26_shift(a1_ptr->unittype);
    }
  }

  /* done */
  return(a1_ptr);
}

/*
 * nv26_statconvert - Convert navy and caravan status values from patch 26
 *
 * Maps old status values from patch 26 to the current status enumeration.
 * Handles the status value changes that occurred when the status system
 * was reorganized in later patch levels.
 *
 * Parameters:
 *   value - Original status value from patch 26 data
 *
 * Returns:
 *   Converted status value for current patch level
 *
 * Side Effects:
 *   - None (pure conversion function)
 *
 * Testing Notes:
 *   Category: A - Unit
 *   Approach: Unit testing with comprehensive status value mapping validation
 *   Key Tests: All OST_* to ST_* mappings, default case handling, boundary conditions
 *   Dependencies: OST_* and ST_* constants only
 *   Mock Requirements: None - pure conversion function
 *   Complexity: Simple - 40 lines with straightforward switch statement logic
 *
 * Notes:
 *   - Maps OST_* constants to ST_* constants
 *   - Defaults to ST_CARRY for unknown status values
 *   - Part of patch 26 navy and caravan conversion system
 *   - Critical for maintaining unit status consistency
 */
static int
nv26_statconvert PARM_1(int, value)
{
  switch (value) {
  case OST_CARRY:
    value = ST_CARRY;
    break;
  case OST_DECOY:
    value = ST_DECOY;
    break;
  case OST_SUPPLY:
    value = ST_SUPPLY;
    break;
  case OST_ENGAGE:
    value = ST_ENGAGE;
    break;
  case OST_REPAIR:
    value = ST_REPAIR;
    break;
  case OST_SIEGED:
    value = ST_SIEGED;
    break;
  case OST_FORSALE:
    value = ST_TRADED;
    break;
  case OST_ONBOARD:
    value = ST_ONBOARD;
    break;
  case OST_ONBSPLY:
    value = ST_ONBSPLY;
    break;
  case OST_WORKCREW:
    value = ST_WORKCREW;
    break;
  default:
    /* hmmm... */
    value = ST_CARRY;
    break;
  }
  return(value);
}

/*
 * rd_navydata - Read navy data with status conversion
 *
 * Reads a navy structure from the data file and applies status value
 * conversion if reading from older patch levels. Handles the status
 * changes needed for compatibility with patch 26 modifications.
 *
 * Parameters:
 *   in_stream - Input file stream (must be valid and readable)
 *
 * Returns:
 *   Pointer to newly allocated NAVY_STRUCT, NULL on error
 *
 * Side Effects:
 *   - Allocates memory for navy structure using new_navy()
 *   - Applies status conversion for older patch levels
 *   - Frees allocated memory on read errors
 *
 * Testing Notes:
 *   Category: B - Integration
 *   Approach: Integration testing with controlled file I/O and navy setup
 *   Key Tests: Version conversion, status conversion, error handling, memory allocation
 *   Dependencies: File I/O, global convert_level, new_navy(), nv26_statconvert(), unit_status(), set_status()
 *   Mock Requirements: File stream mocking, navy allocation mocking, status manipulation mocking
 *   Complexity: Moderate - 35 lines with version-specific logic and status conversion
 *
 * Notes:
 *   - Performs direct read for patch levels > 20
 *   - Applies nv26_statconvert() for older patch levels
 *   - Critical for navy persistence and status compatibility
 *   - Returns NULL on any error for proper error handling
 */
static NAVY_PTR
rd_navydata PARM_1(FILE *, in_stream)
{
  NAVY_PTR n1_ptr;
  int oldstat;

  /* get the data for the structure */
  n1_ptr = new_navy();

  /* to convert or not to convert */
  if ((convert_level == 0) ||
      (convert_level > 20)) {

    /* normal read */
    if ((bytes = fread(n1_ptr, sizeof(NAVY_STRUCT), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: navy data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      free(n1_ptr);
      return((NAVY_PTR) NULL);
    }

  }

  /* handle the switch of unit statuses */
  if ((convert_level != 0) &&
      (convert_level < 26)) {
    oldstat = unit_status(n1_ptr->status);
    set_status(n1_ptr->status, nv26_statconvert(oldstat));
  }

  /* done */
  return(n1_ptr);
}

/*
 * rd_cvndata - Read caravan data with status conversion
 *
 * Reads a caravan structure from the data file and applies status value
 * conversion if reading from older patch levels. Uses the same status
 * conversion logic as navies for consistency.
 *
 * Parameters:
 *   in_stream - Input file stream (must be valid and readable)
 *
 * Returns:
 *   Pointer to newly allocated CVN_STRUCT, NULL on error
 *
 * Side Effects:
 *   - Allocates memory for caravan structure using new_cvn()
 *   - Applies status conversion for older patch levels
 *   - Frees allocated memory on read errors
 *
 * Testing Notes:
 *   Category: B - Integration
 *   Approach: Integration testing with controlled file I/O and caravan setup
 *   Key Tests: Version conversion, status conversion, error handling, memory allocation
 *   Dependencies: File I/O, global convert_level, new_cvn(), nv26_statconvert(), unit_status(), set_status()
 *   Mock Requirements: File stream mocking, caravan allocation mocking, status manipulation mocking
 *   Complexity: Moderate - 35 lines with version-specific logic and status conversion
 *
 * Notes:
 *   - Performs direct read for patch levels > 20
 *   - Applies nv26_statconvert() for older patch levels
 *   - Critical for caravan persistence and status compatibility
 *   - Returns NULL on any error for proper error handling
 */
static CVN_PTR
rd_cvndata PARM_1(FILE *, in_stream)
{
  CVN_PTR v1_ptr;
  int oldstat;

  /* get the data for the structure */
  v1_ptr = new_cvn();

  /* to convert or not to convert */
  if ((convert_level == 0) ||
      (convert_level > 20)) {

    /* normal read */
    if ((bytes = fread(v1_ptr, sizeof(CVN_STRUCT), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: caravan data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      free(v1_ptr);
      return((CVN_PTR) NULL);
    }

  }

  /* handle the switch of unit statuses */
  if ((convert_level != 0) &&
      (convert_level < 26)) {
    oldstat = unit_status(v1_ptr->status);
    set_status(v1_ptr->status, nv26_statconvert(oldstat));
  }

  /* done */
  return(v1_ptr);
}

/*
 * rd_citydata - Read city data with patch 27 conversion support
 *
 * Reads a city structure from the data file, handling conversion from
 * patch 27 format to the current format. Manages city ID assignment
 * and initializes new fields added in later versions.
 *
 * Parameters:
 *   in_stream - Input file stream (must be valid and readable)
 *
 * Returns:
 *   Pointer to newly allocated CITY_STRUCT, NULL on error
 *
 * Side Effects:
 *   - Allocates memory for city structure using new_city()
 *   - Assigns sequential city IDs during conversion
 *   - Initializes new fields for converted data
 *   - Frees allocated memory on read errors
 *
 * Testing Notes:
 *   Category: B - Integration
 *   Approach: Integration testing with controlled file I/O and city setup
 *   Key Tests: Version conversion, city ID assignment, static variable handling, error handling
 *   Dependencies: File I/O, global convert_level, new_city(), static state management
 *   Mock Requirements: File stream mocking, city allocation mocking, static variable control
 *   Complexity: Complex - 70+ lines with version conversion and static ID management
 *
 * Notes:
 *   - Handles conversion from patch 27 using C27_STRUCT
 *   - Maintains static city_id_num for ID assignment
 *   - Resets city ID counter for each new nation
 *   - Critical for city persistence and compatibility
 */
static CITY_PTR
rd_citydata PARM_1(FILE *, in_stream)
{
  static NTN_PTR nold_ptr = (NTN_PTR) NULL;
  static int city_id_num = 1;
  CITY_PTR c1_ptr;
  C27_STRUCT c27_data;
  int i;

  /* allocate the data */
  c1_ptr = new_city();

  /* read in and convert if needed */
  if ((convert_level == 0) ||
      (convert_level > 27)) {

    if ((bytes = fread(c1_ptr, sizeof(CITY_STRUCT), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: city data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      free(c1_ptr);
      return((CITY_PTR) NULL);
    }

  } else {

    /* use the old patch 27 city format and fill in new values */
    if ((bytes = fread(&c27_data, sizeof(C27_STRUCT), 1, in_stream)) != 1) {
      fprintf(fupdate, "ERROR: city data not read\n");
      fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, 1);
      free(c1_ptr);
      return((CITY_PTR) NULL);
    }

    /* now convert the data */
    strcpy(c1_ptr->name, c27_data.name);
    c1_ptr->xloc = c27_data.xloc;
    c1_ptr->yloc = c27_data.yloc;
    c1_ptr->i_people = c27_data.i_people;
    c1_ptr->efficiency = c27_data.efficiency;
    c1_ptr->weight = c27_data.weight;
    c1_ptr->s_talons = c27_data.s_talons;
    c1_ptr->fortress = c27_data.fortress;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      c1_ptr->c_mtrls[i] = c27_data.c_mtrls[i];
      c1_ptr->i_mtrls[i] = c27_data.i_mtrls[i];
    }

    /* check for reset of city identification number */
    if (ntn_ptr != nold_ptr) {
      nold_ptr = ntn_ptr;
      city_id_num = 1;
    }

    /* now initialize new elements */
    c1_ptr->cityid = city_id_num++;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      c1_ptr->m_mtrls[i] = (itemtype) 0;
      c1_ptr->auto_flags[i] = 0L;
    }
    c1_ptr->cmd_flag = 0;

  }
  return(c1_ptr);
}

/*
 * read_data - Load complete game state from data file
 *
 * Reads the entire game world state from a data file, including all nations,
 * armies, navies, cities, caravans, and items. Handles file compression,
 * version compatibility, and constructs all the linked list structures
 * needed for the game state.
 *
 * Parameters:
 *   None (uses global datafile path)
 *
 * Returns:
 *   TRUE on successful read, FALSE on error or file not found
 *
 * Side Effects:
 *   - Allocates memory for all game structures
 *   - Constructs linked lists for all nation-owned entities
 *   - Sets up global world structure and sector array
 *   - Handles decompression if COMPRESS is defined
 *
 * Testing Notes:
 *   Category: C - System Level
 *   Approach: System testing with complete file I/O and world state setup
 *   Key Tests: File operations, decompression, world initialization, complete data pipeline
 *   Dependencies: Complete game world, all data structures, file system, optional compression
 *   Mock Requirements: Entire I/O system or complete integration environment
 *   Complexity: Extremely Complex - 300+ lines, master data loading, multi-system coordination
 *
 * Notes:
 *   - Critical function for game state loading
 *   - Handles all patch level conversions automatically
 *   - Extensive DEBUG output shows data reading progress
 *   - Validates data integrity during the read process
 */
int
read_data PARM_0(void)
{
  int loop, loop2, checkval;
  char sys_str[BIGLTH], dfile_name[FILELTH];
  FILE *fdata;
#ifdef DEBUG
  int army_count = 0, navy_count = 0, city_count = 0;
  int cvn_count = 0, item_count = 0;
#endif /*DEBUG*/

  /* open the file */
  if (!in_curses) {
    fprintf(fupdate, "\nReading data");
  }
  strcpy(dfile_name, datafile);
#ifdef COMPRESS
  strcat(dfile_name, COMP_SFX);
#endif /* COMPRESS */

  /* check existance */
  if (exists(dfile_name) != 0) {
    fprintf(fupdate, "\nNo data file exists for that campaign\n");
    return(FALSE);
  }

#ifdef COMPRESS
  /* open a pipe to read the compressed file */
  sprintf(sys_str, "%s %s", COMP_READ, dfile_name);
  if (((fdata = popen(sys_str, "r")) == NULL)
     || feof(fdata)) {
    fprintf(fupdate, "Could not read compressed data.  Check permissions\n");
    fprintf(fupdate, "Perhaps that campaign does not exist?\n\n");
    return(FALSE);
  }
#else
  if (((fdata = fopen(dfile_name, "r")) == NULL) || (feof(fdata))) {
    fprintf(fupdate, "Could not read the data file.  Check permissions\n");
    fprintf(fupdate, "Perhaps that campaign does not exist?\n\n");
    return(FALSE);
  }
#endif

  /* try to read in the data header */
  if (rd_header(fdata) == TRUE) {
    return(FALSE);
  }

  /* read the world structure */
  if (rd_worlddata(fdata) == TRUE) {
    return(FALSE);
  }

  /* allocate sector space */
  if (sct != NULL) free(sct);
  sct = (SCT_PTR *) m2alloc(MAPX, MAPY, sizeof(SCT_STRUCT));

  /* read in the sector information */
  checkval = MAPX * MAPY;
  if ((bytes = fread(*sct, sizeof(struct s_sector), MAPX * MAPY, fdata))
     != checkval) {
    fprintf(fupdate, "ERROR: sector data not read\n");
    fprintf(fupdate, "Wrong data size (%ld vs. %d)\n", bytes, checkval);
    return(FALSE);
  }
#ifdef DEBUG
  if (!in_curses) {
    fprintf(fupdate, "%ld bytes of sector data read\n", bytes *
	    (long) sizeof(struct s_sector));
  }
#endif /*DEBUG*/

  /* read all of the nation information */
  sum_bytes = 0L;
  for (loop = 1; loop < world.nations; loop++) {

    /* read in the nation structure */
    if ((world.np[loop] = rd_ntndata(fdata)) == NULL) {
      return(FALSE);
    }
    ntn_ptr = world.np[loop];
#ifdef DEBUG
    sum_bytes += sizeof(NTN_STRUCT);
    army_count += ntn_ptr->num_army;
#endif /*DEBUG*/

    /* read in the armies */
    ntn_ptr->army_list = NULL;
    for (loop2 = 0; loop2 < ntn_ptr->num_army; loop2++) {

      /* do this */
      if ((army_tptr = rd_armydata(fdata)) == NULL) {
	return(FALSE);
      }
#ifdef DEBUG
      sum_bytes += sizeof(ARMY_STRUCT);
#endif /*DEBUG*/
      army_tptr->nrby = NULL;
      army_tptr->next = NULL;
      switch (loop2) {
      case 0:
	/* create parent army */
	ntn_ptr->army_list = army_tptr;
	break;
      case 1:
	/* create first child */
	ntn_ptr->army_list->next = army_tptr;
	army_ptr = army_tptr;
	break;
      default:
	/* append to the list */
	army_ptr->next = army_tptr;
	army_ptr = army_ptr->next;
	break;
      }
    }

    /* presorted; just align near pointers */
    align_armynear();

    /* read in the navies */
#ifdef DEBUG
    navy_count += ntn_ptr->num_navy;
#endif /*DEBUG*/
    ntn_ptr->navy_list=NULL;
    for (loop2 = 0; loop2 < ntn_ptr->num_navy; loop2++) {

      /* get the navy data */
      if ((navy_tptr = rd_navydata(fdata)) == NULL) {
	return(FALSE);
      }
#ifdef DEBUG
      sum_bytes += sizeof(NAVY_STRUCT);
#endif /*DEBUG*/
      navy_tptr->next = NULL;
      switch (loop2) {
      case 0:
	/* create parent navy */
	ntn_ptr->navy_list = navy_tptr;
	break;
      case 1:
	/* create first child */
	ntn_ptr->navy_list->next = navy_tptr;
	navy_ptr = navy_tptr;
	break;
      default:
	/* append to the list */
	navy_ptr->next = navy_tptr;
	navy_ptr = navy_ptr->next;
	break;
      }
    }

    /* read in the cities */
#ifdef DEBUG
    city_count += ntn_ptr->num_city;
#endif /*DEBUG*/
    ntn_ptr->city_list=NULL;
    for (loop2 = 0; loop2 < ntn_ptr->num_city; loop2++) {
      if ((city_tptr = rd_citydata(fdata)) == NULL) {
	return(FALSE);
      }
#ifdef DEBUG
      sum_bytes += sizeof(CITY_STRUCT);
#endif /*DEBUG*/
      city_tptr->next = NULL;
      switch (loop2) {
      case 0:
	/* create parent city */
	ntn_ptr->city_list = city_tptr;
	break;
      case 1:
	/* create first child */
	ntn_ptr->city_list->next = city_tptr;
	city_ptr = city_tptr;
	break;
      default:
	/* append to the list */
	city_ptr->next = city_tptr;
	city_ptr = city_ptr->next;
	break;
      }
    }

    /* read in the caravans */
#ifdef DEBUG
    cvn_count += ntn_ptr->num_cvn;
#endif /*DEBUG*/
    ntn_ptr->cvn_list = NULL;
    for (loop2 = 0; loop2 < ntn_ptr->num_cvn; loop2++) {

      /* get the caravan data */
      if ((cvn_tptr = rd_cvndata(fdata)) == NULL) {
	return(FALSE);
      }
#ifdef DEBUG
      sum_bytes += sizeof(CVN_STRUCT);
#endif /*DEBUG*/
      cvn_tptr->next = NULL;
      switch (loop2) {
      case 0:
	/* create parent caravan */
	ntn_ptr->cvn_list = cvn_tptr;
	break;
      case 1:
	/* create first child */
	ntn_ptr->cvn_list->next = cvn_tptr;
	cvn_ptr = cvn_tptr;
	break;
      default:
	/* append to the list */
	cvn_ptr->next = cvn_tptr;
	cvn_ptr = cvn_ptr->next;
	break;
      }
    }

    /* read in the commodities */
#ifdef DEBUG
    item_count += ntn_ptr->num_item;
#endif /*DEBUG*/
    ntn_ptr->item_list=NULL;
    for (loop2 = 0; loop2 < ntn_ptr->num_item; loop2++) {
      item_tptr = new_item();
      if ((bytes = fread(item_tptr, sizeof(ITEM_STRUCT), 1, fdata)) != 1) {
	fprintf(fupdate, "ERROR: commodity data not read\n");
	fprintf(fupdate, "Wrong data size (%zu vs. %d)\n", bytes, 1);
	return(FALSE);
      }
#ifdef DEBUG
      sum_bytes += sizeof(ITEM_STRUCT);
#endif /*DEBUG*/
      item_tptr->next = NULL;
      switch (loop2) {
      case 0:
	/* create parent caravan */
	ntn_ptr->item_list = item_tptr;
	break;
      case 1:
	/* create first child */
	ntn_ptr->item_list->next = item_tptr;
	item_ptr = item_tptr;
	break;
      default:
	/* append to the list */
	item_ptr->next = item_tptr;
	item_ptr = item_ptr->next;
	break;
      }
    }

    /* now, read in the unit numbering list */
    ntn_ptr->unum_list = rd_unumlist(fdata, ntn_ptr->num_unum);

    /* finally, read in the sector mapping list */
    ntn_ptr->map_list = rd_maplist(fdata, ntn_ptr->num_maps);

  }

  /* clear all remaining nation storage */
  for (; loop < ABSMAXNTN; loop++) {
    world.np[loop] = NULL;
  }

  /*  read in the unit numbering list */
  if ((convert_level == 0) ||
      (convert_level > 24)) {
    world.dflt_unum = rd_unumlist(fdata, world.num_unum);
  }

  /* close the file */
#ifdef COMPRESS
  (void) pclose(fdata);
#else
  (void) fclose(fdata);
#endif /* COMPRESS */
#ifdef DEBUG
  if (!in_curses) {
    fprintf(fupdate, "%ld bytes of nation data read\n", sum_bytes);
    fprintf(fupdate, "Read:\t\t%d nations, %d armies, %d navies\n",
	    world.nations - 1, army_count, navy_count);
    fprintf(fupdate, "\t\t%d cities, %d caravans, %d commodities\n",
	    city_count, cvn_count, item_count);
    fprintf(fupdate, "Reading data");
  }
#endif /*DEBUG*/
  if (!in_curses) {
    fprintf(fupdate, "...done\n\n");
  }
  return(TRUE);
}

/*
 * exists - Test for file existence using stat system call
 *
 * Checks whether a file exists and is accessible by attempting to get
 * file status information. Uses the standard stat() system call to
 * determine file existence.
 *
 * Parameters:
 *   file - Path to file to check (must not be NULL)
 *
 * Returns:
 *   0 if file exists and is accessible, non-zero if file doesn't exist or error
 *
 * Side Effects:
 *   - None (read-only operation)
 *
 * Testing Notes:
 *   Category: A - Unit
 *   Approach: Unit testing with mock file system or temporary files
 *   Key Tests: Existing files, non-existent files, permission issues, path validation
 *   Dependencies: stat() system call only
 *   Mock Requirements: File system mocking or temporary test files
 *   Complexity: Trivial - 5 lines, simple stat() wrapper with clear interface
 *
 * Notes:
 *   - Simple wrapper around stat() system call
 *   - Returns stat() return value directly
 *   - Used for checking data file existence before operations
 *   - POSIX-compliant file existence check
 */
int
exists PARM_1(char *, file)
{
  struct stat buf;

  return(stat(file, &buf));
}

/*
 * move_file - Rename a file with platform-specific implementation
 *
 * Renames a file from one path to another using either rename() (on VAXC)
 * or link()/unlink() combination (on other systems). The link/unlink
 * approach provides atomic file replacement on systems that support it.
 *
 * Parameters:
 *   from - Source file path (must exist)
 *   to - Destination file path
 *
 * Returns:
 *   0 on success, non-zero on failure
 *
 * Side Effects:
 *   - Creates hard link from source to destination
 *   - Removes source file after successful link
 *   - May leave partial state if link succeeds but unlink fails
 *
 * Testing Notes:
 *   Category: A - Unit
 *   Approach: Unit testing with temporary files and platform-specific behavior
 *   Key Tests: Successful rename, permission failures, cross-device moves, platform differences
 *   Dependencies: rename(), link(), unlink() system calls, platform macros
 *   Mock Requirements: File system operations or temporary test files
 *   Complexity: Simple - 15 lines with platform-specific conditional logic
 *
 * Notes:
 *   - Implementation suggested by Jerry Pierce
 *   - VAXC systems use simple rename() function
 *   - Other systems use link/unlink for atomic operations
 *   - Critical for safe data file replacement operations
 */
int
move_file PARM_2(char *, from, char *, to)
{
#ifdef VAXC
  /* just use the rename function */
  return( rename(from, to) );
#else
  /* make a hard link of the new file to the old */
  if (link(from, to) != 0)
    return(-1);

  /* now get rid of the older file */
  return(unlink(from));
#endif /* VAXC */
}
