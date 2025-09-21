/*
 * memoryX.c - Memory Management and Data Structure Operations
 *
 * This file provides comprehensive memory management, dynamic allocation,
 * and data structure manipulation for the Conquer game system. It handles
 * all aspects of creating, destroying, and organizing game entities.
 *
 * Key Functionality Areas:
 *   - Memory allocation for game structures (armies, navies, cities, etc.)
 *   - Linked list sorting algorithms for all entity types
 *   - Data structure creation and initialization
 *   - Memory deallocation and cleanup
 *   - Nation management and organization
 *   - 2D array allocation for map data
 *   - Display mode management
 *
 * This module is critical for game stability and performance, ensuring
 * proper memory usage and maintaining sorted lists for efficient access.
 * All dynamic memory allocation goes through these functions to provide
 * centralized error handling and consistent initialization.
 *
 * conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
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
#include "itemX.h"
#include "navyX.h"
#include "butesX.h"
#include "desigX.h"
#include "racesX.h"
#include "activeX.h"
#include "statusX.h"
#include "caravanX.h"
#include "displayX.h"
#include "dstatusX.h"

/*
 * clr_memory - Clear a block of memory to zero
 *
 * Provides a portable way to zero-initialize memory blocks across different
 * platforms. Uses either bzero() (BSD systems) or memset() (standard C)
 * depending on compile-time configuration.
 *
 * Parameters:
 *   mem_ptr - Pointer to memory block to clear (must not be NULL)
 *   len - Number of bytes to clear (must be positive)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Zeroes len bytes starting at mem_ptr
 *   - No bounds checking performed - caller must ensure valid memory range
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple memory clearing wrapper, pure function
 *   Approach: Unit testing with mock memory blocks and boundary testing
 *   Key Tests: Normal clearing, zero-length clearing, large block clearing
 *   Dependencies: memset() function (standard library)
 *   Mock Requirements: None - uses standard library functions
 *   Complexity: Simple - platform abstraction wrapper with clear interface
 *
 * Notes:
 *   - Platform abstraction layer for memory clearing
 *   - Used throughout the codebase for structure initialization
 */
void
clr_memory PARM_2(char *, mem_ptr, int, len)
{
  /* clean out len bytes of data */
  /* Use POSIX-compliant memset instead of BSD bzero */
  memset( mem_ptr, 0, len );
}

/*
 * align_data - Clean up nations and count all game elements
 *
 * Performs comprehensive data cleanup and counting during game updates.
 * Removes inactive nations, counts all entity types for each nation,
 * and recalculates nation territories. This is a critical maintenance
 * function called during update processing.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Removes nations marked as INACTIVE by calling dest_ntn()
 *   - Updates MAXNTN to current number of nations
 *   - Sets world.active_ntns counter
 *   - For each nation, counts and sets:
 *     * num_army (army units)
 *     * num_navy (naval units) 
 *     * num_cvn (caravan units)
 *     * num_city (cities)
 *     * num_item (items/commodities)
 *     * num_unum (unit numbering structures)
 *     * num_maps (map structures)
 *   - Recalculates territory boundaries via find_area()
 *   - Counts default world unit numbering structures
 *   - Calls ntn_sort() to reorganize nations if in update mode
 *   - Prints progress to update log file
 *
 * Testing Notes:
 *   Category: C (System Level) - World-wide data cleanup requiring complete game state
 *   Approach: System testing with complete game infrastructure and full world setup
 *   Key Tests: Nation counting, entity cleanup, territory calculation, update mode handling
 *   Dependencies: Complete game world (nations, armies, navies, cities, items), dest_ntn(), find_area(), ntn_sort()
 *   Mock Requirements: Extensive - full game state, file I/O, all entity types
 *   Complexity: Very Complex - touches all game data structures and performs major reorganization
 *
 * Notes:
 *   - Essential for maintaining data integrity between turns
 *   - Heavy operation that touches all game entities
 *   - Only sorts nations during actual updates (is_update == TRUE)
 */
void
align_data PARM_0(void)
{
  UNUM_PTR unum_ptr;
  MAP_PTR map_ptr;

  /* go through all nations keeping track of usages */
  MAXNTN = 1;
  world.active_ntns = 0;
  fprintf(fupdate, "aligning data...");
  for (country = 0; country < ABSMAXNTN; country++)
    if ((ntn_ptr = world.np[country]) != NULL) {

      /* check if the nation needs removal */
      if (ntn_ptr->active == INACTIVE) {
	/* nuke those useless nations */
	dest_ntn(ntn_ptr->name);
	continue;
      }

      /* count the nation */
      MAXNTN++;
      if (n_isactive(ntn_ptr->active)) {
	world.active_ntns++;
      }

      /* count army elements */
      ntn_ptr->num_army = 0;
      for (army_ptr = ntn_ptr->army_list;
	   army_ptr != NULL;
	   army_ptr = army_ptr->next) {
	ntn_ptr->num_army++;
      }

      /* count navy elements */
      ntn_ptr->num_navy = 0;
      for (navy_ptr = ntn_ptr->navy_list;
	   navy_ptr != NULL;
	   navy_ptr = navy_ptr->next) {
	ntn_ptr->num_navy++;
      }

      /* count caravan elements */
      ntn_ptr->num_cvn = 0;
      for (cvn_ptr = ntn_ptr->cvn_list;
	   cvn_ptr != NULL;
	   cvn_ptr = cvn_ptr->next) {
	ntn_ptr->num_cvn++;
      }

      /* count city elements */
      ntn_ptr->num_city = 0;
      for (city_ptr = ntn_ptr->city_list;
	   city_ptr != NULL;
	   city_ptr = city_ptr->next) {
	ntn_ptr->num_city++;
      }

      /* count item elements */
      ntn_ptr->num_item = 0;
      for (item_ptr = ntn_ptr->item_list;
	   item_ptr != NULL;
	   item_ptr = item_ptr->next) {
	ntn_ptr->num_item++;
      }

      /* count unum elements */
      ntn_ptr->num_unum = 0;
      for (unum_ptr = ntn_ptr->unum_list;
	   unum_ptr != NULL;
	   unum_ptr = unum_ptr->next) {
	ntn_ptr->num_unum++;
      }

      /* count map elements */
      ntn_ptr->num_maps = 0;
      for (map_ptr = ntn_ptr->map_list;
	   map_ptr != NULL;
	   map_ptr = map_ptr->next) {
	ntn_ptr->num_maps++;
      }

      /* now redefine the range for the nation */
      if (n_isntn(ntn_ptr->active)) {
	find_area(country);
      }

    }

  /* count the default unum elements */
  world.num_unum = 0;
  for (unum_ptr = world.dflt_unum;
       unum_ptr != NULL;
       unum_ptr = unum_ptr->next) {
    world.num_unum++;
  }

  /* finish the process; by resorting at end of update */
  fprintf(fupdate, "done\n");
  if (is_update == TRUE) ntn_sort();
}

/*
 * m2alloc - Allocate a two-dimensional array of memory
 *
 * Creates a dynamically allocated 2D array with contiguous memory layout.
 * The array is allocated as a single block with properly aligned row pointers,
 * allowing efficient access via array[row][col] syntax.
 *
 * Parameters:
 *   nrows - Number of rows to allocate (must be >= 0)
 *   ncols - Number of columns per row (must be > 0)
 *   entrysize - Size in bytes of each array element (must be > 0)
 *
 * Returns:
 *   Pointer to array of row pointers (char **) on success
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates memory for both row pointers and data
 *   - Sets up row pointers for direct array access
 *   - Calls errormsg() and abrt() on allocation failure
 *
 * Testing Notes:
 *   Category: A (Unit) - 2D array allocation with error handling, testable with mock malloc
 *   Approach: Unit testing with mock malloc to test allocation and failure scenarios
 *   Key Tests: Normal allocation, malloc failure, zero dimensions, large arrays
 *   Dependencies: malloc(), errormsg(), abrt()
 *   Mock Requirements: Mock malloc for failure testing, mock errormsg/abrt for error handling
 *   Complexity: Moderate - 2D array setup with pointer arithmetic and error handling
 *
 * Notes:
 *   - Memory layout: [row_pointers][data_block]
 *   - Enables efficient 2D array access patterns
 *   - Used primarily for map data structures
 *   - Caller must free returned pointer when done
 *   - Total allocation: nrows * (sizeof(char*) + ncols * entrysize)
 */
char **
m2alloc PARM_3 (int, nrows, int, ncols, int, entrysize)
{
  char **baseaddr;
  int j;

  entrysize *= ncols;
  baseaddr = (char **) malloc((unsigned)
			      (nrows * (sizeof(char *) + entrysize)));

  if (baseaddr == (char **) NULL) {
    sprintf(string, "ERROR -- can't allocate %d by %d blocks of %d bytes",
	    nrows, ncols, entrysize);
    errormsg(string);
    abrt();
  }
  if(nrows > 0){
    *baseaddr = (char *) (baseaddr + nrows);
    for(j = 1; j < nrows; j++)
      baseaddr[j] = baseaddr[j - 1] + entrysize;
  }
  return(baseaddr);
}

/*
 * army_sort - Sort army list by ID and establish proximity links
 *
 * Performs bubble sort on the nation's army list to maintain ascending
 * order by army ID. Optionally establishes proximity links between
 * armies in the same sector via the "nrby" (nearby) pointers.
 *
 * Parameters:
 *   skipnears - If TRUE, skip proximity linking; if FALSE, call align_armynear()
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Reorders ntn_ptr->army_list in ascending armyid order
 *   - Updates next pointers to maintain proper linked list structure
 *   - If skipnears is FALSE, calls align_armynear() to set proximity links
 *
 * Testing Notes:
 *   Category: B (Integration) - Bubble sort requiring army list setup and proximity functions
 *   Approach: Integration testing with controlled army list setup and mock proximity functions
 *   Key Tests: Empty list, single element, multiple armies, proximity linking, skipnears flag
 *   Dependencies: ntn_ptr->army_list, align_armynear()
 *   Mock Requirements: Mock army structures, mock align_armynear() function
 *   Complexity: Moderate - bubble sort with proximity linking coordination
 *
 * Notes:
 *   - Uses bubble sort algorithm (O(n²) complexity)
 *   - Handles empty lists and single-element lists efficiently
 *   - Essential for maintaining consistent army order in UI and commands
 *   - Proximity linking allows quick access to co-located units
 */
void
army_sort PARM_1(int, skipnears)
{
  ARMY_PTR a1_ptr, a2_ptr;
  int done = FALSE;

  /* check for no need of ordering */
  if ((ntn_ptr->army_list == NULL) ||
      ((ntn_ptr->army_list)->next == NULL)) {
    done = TRUE;
  }

  /* perform sort */
  while (!done) {

    /* test for initial element swap */
    a1_ptr = ntn_ptr->army_list;
    if (a1_ptr->armyid > (a1_ptr->next)->armyid) {
      ntn_ptr->army_list = a1_ptr->next;
      a1_ptr->next = (ntn_ptr->army_list)->next;
      (ntn_ptr->army_list)->next = a1_ptr;
      continue;
    }

    /* initialization */
    done = TRUE;

    /* sort remaining linked list */
    for (; (a1_ptr->next)->next != NULL; a1_ptr = a1_ptr->next) {
      a2_ptr = (a1_ptr->next)->next;
      if ((a1_ptr->next)->armyid > a2_ptr->armyid) {
	/* swap locations in list */
	(a1_ptr->next)->next = a2_ptr->next;
	a2_ptr->next = a1_ptr->next;
	a1_ptr->next = a2_ptr;
	/* continue sort */
	done = FALSE;
      }
    }
  }

  /* now connect all near pointers properly */
  if (!skipnears) {
    align_armynear();
  }
}

/*
 * navy_sort - Sort navy list by ID in ascending order
 *
 * Maintains the nation's navy list in ascending order by navy ID using
 * a bubble sort algorithm. Similar to army_sort but simpler since navies
 * don't require proximity linking.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Reorders ntn_ptr->navy_list in ascending navyid order
 *   - Updates next pointers to maintain linked list integrity
 *
 * Testing Notes:
 *   Category: B (Integration) - Bubble sort requiring navy list setup
 *   Approach: Integration testing with controlled navy list setup and mock nation structure
 *   Key Tests: Empty list, single element, multiple navies, ID ordering validation
 *   Dependencies: ntn_ptr->navy_list, navy structures with navyid field
 *   Mock Requirements: Mock navy structures, mock nation pointer (ntn_ptr)
 *   Complexity: Moderate - bubble sort algorithm with linked list manipulation
 *
 * Notes:
 *   - Uses bubble sort algorithm (O(n²) complexity)
 *   - Handles empty and single-element lists efficiently
 *   - Required for consistent navy numbering and UI display
 */
void
navy_sort PARM_0(void)
{
  NAVY_PTR n1_ptr, n2_ptr;
  int done = FALSE;

  /* check for no need of ordering */
  if (ntn_ptr->navy_list == NULL || (ntn_ptr->navy_list)->next == NULL) {
    done = TRUE;
  }

  /* perform sort */
  while (!done) {
    /* initialization */
    n1_ptr = ntn_ptr->navy_list;
    done = TRUE;

    /* test for initial element swap */
    if (n1_ptr->navyid > (n1_ptr->next)->navyid) {
      ntn_ptr->navy_list = n1_ptr->next;
      n1_ptr->next = (ntn_ptr->navy_list)->next;
      (ntn_ptr->navy_list)->next = n1_ptr;
      n1_ptr = ntn_ptr->navy_list;
    }

    /* sort remaining linked list */
    for (; (n1_ptr->next)->next != NULL; n1_ptr = n1_ptr->next) {
      n2_ptr = (n1_ptr->next)->next;
      if ((n1_ptr->next)->navyid > n2_ptr->navyid) {
	/* swap locations in list */
	(n1_ptr->next)->next = n2_ptr->next;
	n2_ptr->next = n1_ptr->next;
	n1_ptr->next = n2_ptr;
	/* continue sort */
	done = FALSE;
      }
    }
  }
}

/*
 * city_sort - Sort city list alphabetically by name
 *
 * Maintains the nation's city list in alphabetical order by city name
 * using bubble sort. Unlike army/navy sorts, this sorts by string
 * comparison rather than numeric ID.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Reorders ntn_ptr->city_list in alphabetical order by name
 *   - Updates next pointers to maintain linked list integrity
 *   - Uses str_test() for string comparison
 *
 * Testing Notes:
 *   Category: B (Integration) - Bubble sort requiring city list setup and string comparison
 *   Approach: Integration testing with controlled city list setup and mock string functions
 *   Key Tests: Empty list, single city, multiple cities, alphabetical ordering, string comparison edge cases
 *   Dependencies: ntn_ptr->city_list, city structures with name field, str_test()
 *   Mock Requirements: Mock city structures, mock nation pointer (ntn_ptr), mock str_test()
 *   Complexity: Moderate - bubble sort with string comparison logic
 *
 * Notes:
 *   - Uses bubble sort algorithm with string comparison
 *   - Alphabetical ordering makes city lists user-friendly
 *   - Essential for consistent city display in interfaces
 */
void
city_sort PARM_0(void)
{
  CITY_PTR c1_ptr, c2_ptr;
  int done = FALSE;

  /* check for no need of ordering */
  if (ntn_ptr->city_list == NULL || (ntn_ptr->city_list)->next == NULL) {
    done = TRUE;
  }

  /* perform sort */
  while (!done) {
    /* initialization */
    c1_ptr = ntn_ptr->city_list;
    done = TRUE;

    /* test for initial element swap */
    if ( str_test( c1_ptr->name, (c1_ptr->next)->name ) > 0) {
      ntn_ptr->city_list = c1_ptr->next;
      c1_ptr->next = (ntn_ptr->city_list)->next;
      (ntn_ptr->city_list)->next = c1_ptr;
      c1_ptr = ntn_ptr->city_list;
    }

    /* sort remaining linked list */
    for (; (c1_ptr->next)->next != NULL; c1_ptr = c1_ptr->next) {
      c2_ptr = (c1_ptr->next)->next;
      if ( str_test( (c1_ptr->next)->name, c2_ptr->name ) > 0) {
	/* swap locations in list */
	(c1_ptr->next)->next = c2_ptr->next;
	c2_ptr->next = c1_ptr->next;
	c1_ptr->next = c2_ptr;
	/* continue sort */
	done = FALSE;
      }
    }
  }
}

/*
 * cvn_sort - Sort caravan list by ID in ascending order
 *
 * Maintains the nation's caravan list in ascending order by caravan ID
 * using bubble sort algorithm. Similar to navy_sort for caravan entities.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Reorders ntn_ptr->cvn_list in ascending cvnid order
 *   - Updates next pointers to maintain linked list integrity
 *
 * Testing Notes:
 *   Category: B (Integration) - Bubble sort requiring caravan list setup
 *   Approach: Integration testing with controlled caravan list setup and mock nation structure
 *   Key Tests: Empty list, single element, multiple caravans, ID ordering validation
 *   Dependencies: ntn_ptr->cvn_list, caravan structures with cvnid field
 *   Mock Requirements: Mock caravan structures, mock nation pointer (ntn_ptr)
 *   Complexity: Moderate - bubble sort algorithm with linked list manipulation
 *
 * Notes:
 *   - Uses bubble sort algorithm (O(n²) complexity)
 *   - Essential for consistent caravan numbering and display
 *   - Handles empty and single-element lists efficiently
 */
void
cvn_sort PARM_0(void)
{
  CVN_PTR c1_ptr, c2_ptr;
  int done = FALSE;

  /* check if ordering is not needed */
  if (ntn_ptr->cvn_list == NULL || (ntn_ptr->cvn_list)->next == NULL) {
    done = TRUE;
  }

  /* perform sort */
  while (!done) {
    /* initialization */
    c1_ptr = ntn_ptr->cvn_list;
    done = TRUE;

    /* test for initial element swap */
    if (c1_ptr->cvnid > (c1_ptr->next)->cvnid) {
      ntn_ptr->cvn_list = c1_ptr->next;
      c1_ptr->next = (ntn_ptr->cvn_list)->next;
      (ntn_ptr->cvn_list)->next = c1_ptr;
      c1_ptr = ntn_ptr->cvn_list;
    }

    /* sort remaining linked list */
    for (; (c1_ptr->next)->next != NULL; c1_ptr = c1_ptr->next) {
      c2_ptr = (c1_ptr->next)->next;
      if ((c1_ptr->next)->cvnid > c2_ptr->cvnid) {
	/* swap locations in list */
	(c1_ptr->next)->next = c2_ptr->next;
	c2_ptr->next = c1_ptr->next;
	c1_ptr->next = c2_ptr;
	/* continue sort */
	done = FALSE;
      }
    }
  }
}

/*
 * item_sort - Sort item list by ID in ascending order
 *
 * Maintains the nation's item/commodity list in ascending order by item ID
 * using bubble sort algorithm. Similar to other entity sorting functions.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Reorders ntn_ptr->item_list in ascending itemid order
 *   - Updates next pointers to maintain linked list integrity
 *
 * Testing Notes:
 *   Category: B (Integration) - Bubble sort requiring item list setup
 *   Approach: Integration testing with controlled item list setup and mock nation structure
 *   Key Tests: Empty list, single item, multiple items, ID ordering validation
 *   Dependencies: ntn_ptr->item_list, item structures with itemid field
 *   Mock Requirements: Mock item structures, mock nation pointer (ntn_ptr)
 *   Complexity: Moderate - bubble sort algorithm with linked list manipulation
 *
 * Notes:
 *   - Uses bubble sort algorithm (O(n²) complexity)
 *   - Essential for consistent item numbering and inventory display
 *   - Handles empty and single-element lists efficiently
 */
void
item_sort PARM_0(void)
{
  ITEM_PTR i1_ptr, i2_ptr;
  int done = FALSE;

  /* check for no need of ordering */
  if (ntn_ptr->item_list == NULL || (ntn_ptr->item_list)->next == NULL) {
    done=TRUE;
  }

  /* perform sort */
  while (!done) {
    /* initialization */
    i1_ptr = ntn_ptr->item_list;
    done = TRUE;

    /* test for initial element swap */
    if (i1_ptr->itemid > (i1_ptr->next)->itemid) {
      ntn_ptr->item_list = i1_ptr->next;
      i1_ptr->next = (ntn_ptr->item_list)->next;
      (ntn_ptr->item_list)->next = i1_ptr;
      i1_ptr = ntn_ptr->item_list;
    }

    /* sort remaining linked list */
    for (; (i1_ptr->next)->next != NULL; i1_ptr = i1_ptr->next) {
      i2_ptr = (i1_ptr->next)->next;
      if ((i1_ptr->next)->itemid > i2_ptr->itemid) {
	/* swap locations in list */
	(i1_ptr->next)->next = i2_ptr->next;
	i2_ptr->next = i1_ptr->next;
	i1_ptr->next = i2_ptr;
	/* continue sort */
	done = FALSE;
      }
    }
  }
}

/*
 * align_armynear - Establish proximity links between co-located armies
 *
 * Scans through the nation's army list and sets up "nrby" (nearby) pointers
 * to link armies that are located in the same sector. This creates a
 * secondary linked list structure for quick access to units at the same
 * coordinates.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Sets nrby pointer for each army to point to next army at same location
 *   - Sets nrby to NULL if no other army is at the same coordinates
 *   - Must be called after army_sort() to work on sorted list
 *
 * Testing Notes:
 *   Category: B (Integration) - Proximity linking requiring army list setup and coordinate management
 *   Approach: Integration testing with controlled army list setup and coordinate positioning
 *   Key Tests: Empty list, single army, multiple armies same location, multiple armies different locations, mixed scenarios
 *   Dependencies: ntn_ptr->army_list, army structures with xloc/yloc fields
 *   Mock Requirements: Mock army structures with coordinates, mock nation pointer (ntn_ptr)
 *   Complexity: Moderate - coordinate comparison with pointer manipulation
 *
 * Notes:
 *   - Assumes army list is already sorted by army_sort()
 *   - Creates linked chains of armies at each coordinate
 *   - Essential for multi-unit movement and combat calculations
 *   - Only links to the immediately next co-located unit, not all units
 */
void
align_armynear PARM_0(void)
{
  ARMY_PTR a1_ptr, a2_ptr;

  /* go through the list one by one */
  for (a1_ptr = ntn_ptr->army_list;
       a1_ptr != NULL; a1_ptr = a1_ptr->next) {

    /* find nearest unit */
    for (a2_ptr = a1_ptr->next;
	 a2_ptr != NULL;
	 a2_ptr = a2_ptr->next) {
      if ((a2_ptr->xloc == a1_ptr->xloc) &&
	  (a2_ptr->yloc == a1_ptr->yloc)) break;
    }
    a1_ptr->nrby = a2_ptr;

  }

}

/*
 * new_mapchar - Allocate world-sized character array
 *
 * Allocates or reuses a character array large enough to hold one byte
 * per world sector (MAPX * MAPY). Initializes the memory to zero.
 * Used for various map overlays and temporary data storage.
 *
 * Parameters:
 *   mem_ptr - Existing memory pointer to reuse, or NULL to allocate new
 *
 * Returns:
 *   Pointer to initialized character array of size MAPX * MAPY
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates MAPX * MAPY bytes if mem_ptr is NULL
 *   - Zeroes all bytes in the array via clr_memory()
 *   - Calls errormsg() and abrt() on allocation failure
 *
 * Testing Notes:
 *   Category: A (Unit) - Map memory allocation with clear interface, testable with mock malloc
 *   Approach: Unit testing with mock malloc and memory validation
 *   Key Tests: New allocation (NULL input), memory reuse (existing pointer), malloc failure, memory initialization
 *   Dependencies: malloc(), clr_memory(), errormsg(), abrt(), MAPX/MAPY constants
 *   Mock Requirements: Mock malloc for failure testing, mock clr_memory, mock errormsg/abrt
 *   Complexity: Simple - straightforward memory allocation with error handling
 *
 * Notes:
 *   - Optimized for map-sized data structures
 *   - Allows memory reuse to avoid repeated allocations
 *   - Common pattern for temporary map calculations
 */
char *
new_mapchar PARM_1(char *, mem_ptr)
{
  /* check it */
  if (mem_ptr == NULL) {
    if ((mem_ptr = (char *) malloc(sizeof(char) * (MAPX * MAPY))) == NULL) {
      errormsg("MAJOR ERROR: Memory allocation error");
      abrt();
    }
  }

  /* zeroify it */
  clr_memory( mem_ptr, MAPX * MAPY * sizeof(char));
  return(mem_ptr);
}

/*
 * new_mapshort - Allocate world-sized short integer array
 *
 * Allocates or reuses a short integer array large enough to hold one
 * short per world sector (MAPX * MAPY). Initializes the memory to zero.
 * Used for map data requiring larger value ranges than char arrays.
 *
 * Parameters:
 *   mem_ptr - Existing memory pointer to reuse, or NULL to allocate new
 *
 * Returns:
 *   Pointer to initialized short array of size MAPX * MAPY
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates MAPX * MAPY * sizeof(short) bytes if mem_ptr is NULL
 *   - Zeroes all bytes in the array via clr_memory()
 *   - Calls errormsg() and abrt() on allocation failure
 *
 * Testing Notes:
 *   Category: A (Unit) - Map memory allocation with clear interface, testable with mock malloc
 *   Approach: Unit testing with mock malloc and memory validation
 *   Key Tests: New allocation (NULL input), memory reuse (existing pointer), malloc failure, memory initialization
 *   Dependencies: malloc(), clr_memory(), errormsg(), abrt(), MAPX/MAPY constants
 *   Mock Requirements: Mock malloc for failure testing, mock clr_memory, mock errormsg/abrt
 *   Complexity: Simple - straightforward memory allocation with error handling
 *
 * Notes:
 *   - For map data requiring values > 255 (char range)
 *   - Memory reuse pattern same as new_mapchar()
 *   - Commonly used for distance maps, weights, counters
 */
short *
new_mapshort PARM_1(short *, mem_ptr)
{
  /* check it */
  if (mem_ptr == NULL) {
    if ((mem_ptr = (short *) malloc(sizeof(short) *
				    (MAPX * MAPY))) == NULL) {
      errormsg("MAJOR ERROR: Memory allocation error");
      abrt();
    }
  }

  /* zeroify it */
  clr_memory( (char *) mem_ptr, MAPX * MAPY * sizeof(short) );
  return(mem_ptr);
}

/*
 * new_maplong - Allocate world-sized long integer array
 *
 * Allocates or reuses a long integer array large enough to hold one
 * long per world sector (MAPX * MAPY). Initializes the memory to zero.
 * Used for map data requiring full long integer range.
 *
 * Parameters:
 *   mem_ptr - Existing memory pointer to reuse, or NULL to allocate new
 *
 * Returns:
 *   Pointer to initialized long array of size MAPX * MAPY
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates MAPX * MAPY * sizeof(long) bytes if mem_ptr is NULL
 *   - Zeroes all bytes in the array via clr_memory()
 *   - Calls errormsg() and abrt() on allocation failure
 *
 * Testing Notes:
 *   Category: A (Unit) - Map memory allocation with clear interface, testable with mock malloc
 *   Approach: Unit testing with mock malloc and memory validation
 *   Key Tests: New allocation (NULL input), memory reuse (existing pointer), malloc failure, memory initialization
 *   Dependencies: malloc(), clr_memory(), errormsg(), abrt(), MAPX/MAPY constants
 *   Mock Requirements: Mock malloc for failure testing, mock clr_memory, mock errormsg/abrt
 *   Complexity: Simple - straightforward memory allocation with error handling
 *
 * Notes:
 *   - For map data requiring large numeric values
 *   - Memory reuse pattern consistent with other new_map*() functions
 *   - Used for calculations involving large sums, distances, or complex data
 */
long *
new_maplong PARM_1(long *, mem_ptr)
{
  /* check it */
  if (mem_ptr == NULL) {
    if ((mem_ptr = (long *) malloc(sizeof(long) * (MAPX * MAPY))) == NULL) {
      errormsg("MAJOR ERROR: Memory allocation error");
      abrt();
    }
  }

  /* zeroify it */
  clr_memory( (char *) mem_ptr, MAPX * MAPY * sizeof(long) );
  return(mem_ptr);
}

/*
 * new_dmode - Allocate memory for a new display mode structure
 *
 * Allocates memory for a DMODE_STRUCT to store display mode configuration.
 * Display modes control how different elements are highlighted and displayed
 * in the game interface.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly allocated DMODE_STRUCT
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates sizeof(DMODE_STRUCT) bytes
 *   - Calls errormsg() and abrt() on allocation failure
 *   - Memory is NOT initialized - caller must set values
 *
 * Notes:
 *   - Part of display system memory management
 *   - Caller responsible for linking into display mode list
 *   - Must be freed by caller when no longer needed
 */
/*
 * new_dmode - Allocate memory for a display mode structure
 *
 * Allocates memory for a DMODE structure used to manage display settings
 * and graphics mode configurations for the user interface.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly allocated DMODE structure
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates sizeof(DMODE_STRUCT) bytes
 *   - Calls errormsg() and abrt() on allocation failure
 *   - Memory is NOT initialized - caller must set values
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple allocation function with clear dependencies
 *   Approach: Unit tests with mock malloc for allocation failure testing
 *   Key Tests: Successful allocation, malloc failure handling, return pointer validation
 *   Dependencies: malloc(), errormsg(), abrt()
 *   Mock Requirements: Mock malloc to simulate failure conditions
 *   Complexity: Simple - basic allocation pattern with error handling
 *
 * Notes:
 *   - Used for graphics and display mode management
 *   - Caller responsible for initialization and cleanup
 *   - Consistent with other new_* allocation functions
 */
DMODE_PTR
new_dmode PARM_0(void)
{
  DMODE_PTR d1_ptr;

  /* create the space */
  d1_ptr = (DMODE_PTR) malloc(sizeof(DMODE_STRUCT));

  /* verify memory */
  if (d1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for display");
    abrt();
  }
  return(d1_ptr);
}

/*
 * new_unum - Allocate memory for a unit numbering structure
 *
 * Allocates memory for a UNITNUM structure used to manage unit numbering
 * schemes and ID assignment ranges for different unit types.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly allocated UNITNUM structure
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates sizeof(UNITNUM) bytes
 *   - Calls errormsg() and abrt() on allocation failure
 *   - Memory is NOT initialized - caller must set values
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple allocation function with clear dependencies
 *   Approach: Unit tests with mock malloc for allocation failure testing
 *   Key Tests: Successful allocation, malloc failure handling, return pointer validation
 *   Dependencies: malloc(), errormsg(), abrt()
 *   Mock Requirements: Mock malloc to simulate failure conditions
 *   Complexity: Simple - basic allocation pattern with error handling
 *
 * Notes:
 *   - Used for managing unit ID assignment ranges
 *   - Caller responsible for initialization and linking
 *   - Consistent with other new_* allocation functions
 */
UNUM_PTR
new_unum PARM_0(void)
{
  UNUM_PTR u1_ptr;

  /* create the space */
  u1_ptr = (UNUM_PTR) malloc(sizeof(UNITNUM));

  /* verify memory */
  if (u1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for a unum");
    abrt();
  }
  return(u1_ptr);
}

/*
 * new_map - Allocate memory for a map structure
 *
 * Allocates memory for a MAP structure used to manage unit mapping
 * and coordinate systems for the game world.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly allocated MAP structure
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates sizeof(MAP_STRUCT) bytes
 *   - Calls errormsg() and abrt() on allocation failure
 *   - Memory is NOT initialized - caller must set values
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple allocation function with clear dependencies
 *   Approach: Unit tests with mock malloc for allocation failure testing
 *   Key Tests: Successful allocation, malloc failure handling, return pointer validation
 *   Dependencies: malloc(), errormsg(), abrt()
 *   Mock Requirements: Mock malloc to simulate failure conditions
 *   Complexity: Simple - basic allocation pattern with error handling
 *
 * Notes:
 *   - Used for unit mapping and coordinate management
 *   - Caller responsible for initialization and cleanup
 *   - Consistent with other new_* allocation functions
 */

/* NEW_MAP -- Allocate space for a new unit mapping structure */
MAP_PTR
new_map PARM_0(void)
{
  MAP_PTR m1_ptr;

  /* create the space */
  m1_ptr = (MAP_PTR) malloc(sizeof(MAP_STRUCT));

  /* verify memory */
  if (m1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for a map struct");
    abrt();
  }
  return(m1_ptr);
}

/*
 * new_army - Allocate memory for an army unit
 *
 * Allocates memory for an ARMY structure used to represent military
 * units with combat capabilities, movement, and tactical attributes.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly allocated ARMY structure
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates sizeof(ARMY_STRUCT) bytes
 *   - Calls errormsg() and abrt() on allocation failure
 *   - Memory is NOT initialized - caller must set values
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple allocation function with clear dependencies
 *   Approach: Unit tests with mock malloc for allocation failure testing
 *   Key Tests: Successful allocation, malloc failure handling, return pointer validation
 *   Dependencies: malloc(), errormsg(), abrt()
 *   Mock Requirements: Mock malloc to simulate failure conditions
 *   Complexity: Simple - basic allocation pattern with error handling
 *
 * Notes:
 *   - Core military unit allocation for game combat system
 *   - Caller responsible for initialization and cleanup
 *   - Consistent with other new_* allocation functions
 */
ARMY_PTR
new_army PARM_0(void)
{
  ARMY_PTR a1_ptr;

  /* create the space */
  a1_ptr = (ARMY_PTR) malloc(sizeof(ARMY_STRUCT));

  /* verify memory */
  if (a1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for army");
    abrt();
  }
  return(a1_ptr);
}

/*
 * new_navy - Allocate memory for a naval unit
 *
 * Allocates memory for a NAVY structure used to represent naval
 * units with sea combat capabilities, movement, and maritime attributes.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly allocated NAVY structure
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates sizeof(NAVY_STRUCT) bytes
 *   - Calls errormsg() and abrt() on allocation failure
 *   - Memory is NOT initialized - caller must set values
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple allocation function with clear dependencies
 *   Approach: Unit tests with mock malloc for allocation failure testing
 *   Key Tests: Successful allocation, malloc failure handling, return pointer validation
 *   Dependencies: malloc(), errormsg(), abrt()
 *   Mock Requirements: Mock malloc to simulate failure conditions
 *   Complexity: Simple - basic allocation pattern with error handling
 *
 * Notes:
 *   - Core naval unit allocation for game maritime system
 *   - Caller responsible for initialization and cleanup
 *   - Consistent with other new_* allocation functions
 */
NAVY_PTR
new_navy PARM_0(void)
{
  NAVY_PTR n1_ptr;

  /* create the space */
  n1_ptr = (NAVY_PTR) malloc(sizeof(NAVY_STRUCT));

  /* verify memory */
  if (n1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for navy");
    abrt();
  }
  return(n1_ptr);
}

/*
 * new_cvn - Allocate memory for a caravan unit
 *
 * Allocates memory for a CVN structure used to represent caravan
 * units with trade capabilities, movement, and economic attributes.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly allocated CVN structure
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates sizeof(CVN_STRUCT) bytes
 *   - Calls errormsg() and abrt() on allocation failure
 *   - Memory is NOT initialized - caller must set values
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple allocation function with clear dependencies
 *   Approach: Unit tests with mock malloc for allocation failure testing
 *   Key Tests: Successful allocation, malloc failure handling, return pointer validation
 *   Dependencies: malloc(), errormsg(), abrt()
 *   Mock Requirements: Mock malloc to simulate failure conditions
 *   Complexity: Simple - basic allocation pattern with error handling
 *
 * Notes:
 *   - Core caravan unit allocation for game trade system
 *   - Caller responsible for initialization and cleanup
 *   - Consistent with other new_* allocation functions
 */
CVN_PTR
new_cvn PARM_0(void)
{
  CVN_PTR c1_ptr;

  /* create the space */
  c1_ptr = (CVN_PTR) malloc(sizeof(CVN_STRUCT));

  /* verify memory */
  if (c1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for cvn");
    abrt();
  }
  return(c1_ptr);
}

/*
 * new_city - Allocate memory for a city structure
 *
 * Allocates memory for a CITY structure used to represent cities
 * with population, production, and infrastructure attributes.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly allocated CITY structure
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates sizeof(CITY_STRUCT) bytes
 *   - Calls errormsg() and abrt() on allocation failure
 *   - Memory is NOT initialized - caller must set values
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple allocation function with clear dependencies
 *   Approach: Unit tests with mock malloc for allocation failure testing
 *   Key Tests: Successful allocation, malloc failure handling, return pointer validation
 *   Dependencies: malloc(), errormsg(), abrt()
 *   Mock Requirements: Mock malloc to simulate failure conditions
 *   Complexity: Simple - basic allocation pattern with error handling
 *
 * Notes:
 *   - Core city allocation for game civilization system
 *   - Caller responsible for initialization and cleanup
 *   - Consistent with other new_* allocation functions
 */
CITY_PTR
new_city PARM_0(void)
{
  CITY_PTR c1_ptr;

  /* create the space */
  c1_ptr = (CITY_PTR) malloc(sizeof(CITY_STRUCT));

  /* verify memory */
  if (c1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for city");
    abrt();
  }
  return(c1_ptr);
}

/*
 * new_item - Allocate memory for an item/commodity structure
 *
 * Allocates memory for an ITEM structure used to represent commodities,
 * resources, and trade goods with economic and inventory attributes.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly allocated ITEM structure
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates sizeof(ITEM_STRUCT) bytes
 *   - Calls errormsg() and abrt() on allocation failure
 *   - Memory is NOT initialized - caller must set values
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple allocation function with clear dependencies
 *   Approach: Unit tests with mock malloc for allocation failure testing
 *   Key Tests: Successful allocation, malloc failure handling, return pointer validation
 *   Dependencies: malloc(), errormsg(), abrt()
 *   Mock Requirements: Mock malloc to simulate failure conditions
 *   Complexity: Simple - basic allocation pattern with error handling
 *
 * Notes:
 *   - Core item allocation for game economic system
 *   - Caller responsible for initialization and cleanup
 *   - Consistent with other new_* allocation functions
 */
ITEM_PTR
new_item PARM_0(void)
{
  ITEM_PTR i1_ptr;

  /* create the space */
  i1_ptr = (ITEM_PTR) malloc(sizeof(ITEM_STRUCT));

  /* verify memory */
  if (i1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for item");
    abrt();
  }
  return(i1_ptr);
}

/*
 * new_ntn - Allocate memory for a nation structure
 *
 * Allocates memory for a NTN (nation) structure used to represent
 * player nations with complete game state, unit lists, and attributes.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly allocated NTN structure
 *   Function terminates program via abrt() if allocation fails
 *
 * Side Effects:
 *   - Allocates sizeof(NTN_STRUCT) bytes
 *   - Calls errormsg() and abrt() on allocation failure
 *   - Memory is NOT initialized - caller must set values
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple allocation function with clear dependencies
 *   Approach: Unit tests with mock malloc for allocation failure testing
 *   Key Tests: Successful allocation, malloc failure handling, return pointer validation
 *   Dependencies: malloc(), errormsg(), abrt()
 *   Mock Requirements: Mock malloc to simulate failure conditions
 *   Complexity: Simple - basic allocation pattern with error handling
 *
 * Notes:
 *   - Core nation allocation for game player management
 *   - Caller responsible for initialization and cleanup
 *   - Consistent with other new_* allocation functions
 */
NTN_PTR
new_ntn PARM_0(void)
{
  NTN_PTR n1_ptr;

  /* create the space */
  n1_ptr = (NTN_PTR) malloc(sizeof(NTN_STRUCT));

  /* verify memory */
  if (n1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for nation");
    abrt();
  }
  return(n1_ptr);
}

/*
 * dest_army - Remove and deallocate an army unit from the nation
 *
 * Removes an army unit from the nation's army list, handles leader/follower
 * relationships, deallocates memory, and maintains list integrity. This is
 * a complex operation that must handle unit leadership chains.
 *
 * Parameters:
 *   idnum - ID number of the army unit to remove
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Removes unit from ntn_ptr->army_list linked list
 *   - Transfers leadership to former leader if this unit was a leader
 *   - Updates follower units to point to new leader
 *   - Deallocates army structure memory via free()
 *   - Calls army_sort(FALSE) to maintain list order and proximity links
 *   - Returns silently if unit ID not found
 *
 * Testing Notes:
 *   Category: B (Integration) - Complex leader/follower relationships requiring army list setup
 *   Approach: Integration testing with controlled army list setup and relationship management
 *   Key Tests: Simple removal, leader removal with followers, follower removal, head/middle/tail removal, invalid ID
 *   Dependencies: ntn_ptr->army_list, army structures with leader/follower relationships, army_sort()
 *   Mock Requirements: Mock army structures with leadership chains, mock army_sort(), mock nation pointer
 *   Complexity: Complex - leader/follower relationship management with list manipulation
 *
 * Notes:
 *   - Handles complex leader/follower relationships automatically
 *   - Maintains army list integrity after removal
 *   - Essential for unit destruction during combat or disbanding
 *   - Automatically resorts army list after removal
 */
void
dest_army PARM_1(int, idnum)
{
  ARMY_PTR a1_ptr, a2_ptr;

  /* stupidity check */
  if (ntn_ptr == NULL) return;

  /* start the predecessor pointer */
  a2_ptr = (ARMY_PTR) NULL;

  /* go through and find the unit */
  for (a1_ptr = ntn_ptr->army_list;
       a1_ptr != NULL;
       a1_ptr = a1_ptr->next) {
    /* check for it */
    if (a1_ptr->armyid == idnum) break;
    a2_ptr = a1_ptr;
  }

  /* get rid of it */
  if (a1_ptr != NULL) {

    /* check for the head of the list */
    if (a2_ptr == NULL) {
      ntn_ptr->army_list = a1_ptr->next;
    } else {
      a2_ptr->next = a1_ptr->next;
    }

    /* check if the unit was a leader and fix any followers */
    if (unit_leading(a1_ptr->status) ||
	a_isleader(a1_ptr->unittype)) {
      for (a2_ptr = ntn_ptr->army_list;
	   a2_ptr != NULL;
	   a2_ptr = a2_ptr->next) {
	/* clean up them doggies */
	if (a2_ptr->leader == a1_ptr->armyid) {
	  a2_ptr->leader = a1_ptr->leader;
	  if (unit_status(a2_ptr->status) == ST_GROUPED) {
	    set_status(a2_ptr->status, unit_status(a1_ptr->status));
	  }
	}
      }
    }

    /* now make the space available */
    a1_ptr->next = (ARMY_PTR) NULL;
    a1_ptr->nrby = (ARMY_PTR) NULL;
    free(a1_ptr);

    /* fix the list */
    army_sort(FALSE);
  }
}

/*
 * dest_navy - Remove and deallocate a naval unit from the nation
 *
 * Removes a naval unit from the nation's navy list, deallocates memory,
 * and maintains list integrity. Simpler than dest_army as naval units
 * do not have complex leader/follower relationships.
 *
 * Parameters:
 *   idnum - ID number of the naval unit to remove
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Removes unit from ntn_ptr->navy_list linked list
 *   - Deallocates navy structure memory via free()
 *   - Returns silently if unit ID not found
 *   - Sets next pointer to NULL before freeing
 *
 * Testing Notes:
 *   Category: B (Integration) - Navy list management requiring list setup
 *   Approach: Integration testing with controlled navy list setup
 *   Key Tests: Head removal, middle removal, tail removal, invalid ID, empty list
 *   Dependencies: ntn_ptr->navy_list, navy structures with navyid field
 *   Mock Requirements: Mock navy structures with IDs, mock nation pointer (ntn_ptr)
 *   Complexity: Moderate - standard linked list removal with memory management
 *
 * Notes:
 *   - Simpler than dest_army (no leader/follower relationships)
 *   - Essential for naval unit destruction during combat or disbanding
 *   - Does not require list resorting after removal
 *   - Maintains list integrity with proper pointer management
 */
void
dest_navy PARM_1(int, idnum)
{
  NAVY_PTR n1_ptr, n2_ptr;

  /* stupidity check */
  if (ntn_ptr == NULL) return;

  /* start the predecessor pointer */
  n2_ptr = (NAVY_PTR) NULL;

  /* go through and find the unit */
  for (n1_ptr = ntn_ptr->navy_list;
       n1_ptr != NULL;
       n1_ptr = n1_ptr->next) {
    /* check for it */
    if (n1_ptr->navyid == idnum) break;
    n2_ptr = n1_ptr;
  }

  /* get rid of it */
  if (n1_ptr != NULL) {

    /* check for the head of the list */
    if (n2_ptr == NULL) {
      ntn_ptr->navy_list = n1_ptr->next;
    } else {
      n2_ptr->next = n1_ptr->next;
    }

    /* now make the space available */
    n1_ptr->next = (NAVY_PTR) NULL;
    free(n1_ptr);

    /* the list should be in order */
  }
}

/*
 * dest_cvn - Remove and deallocate a caravan unit from the nation
 *
 * Removes a caravan unit from the nation's caravan list, deallocates memory,
 * and maintains list integrity. Handles trade unit destruction with proper
 * memory management and list maintenance.
 *
 * Parameters:
 *   idnum - ID number of the caravan unit to remove
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Removes unit from ntn_ptr->cvn_list linked list
 *   - Deallocates caravan structure memory via free()
 *   - Returns silently if unit ID not found
 *   - Sets next pointer to NULL before freeing
 *
 * Testing Notes:
 *   Category: B (Integration) - Caravan list management requiring list setup
 *   Approach: Integration testing with controlled caravan list setup
 *   Key Tests: Head removal, middle removal, tail removal, invalid ID, empty list
 *   Dependencies: ntn_ptr->cvn_list, caravan structures with cvnid field
 *   Mock Requirements: Mock caravan structures with IDs, mock nation pointer (ntn_ptr)
 *   Complexity: Moderate - standard linked list removal with memory management
 *
 * Notes:
 *   - Similar to dest_navy (no complex relationships)
 *   - Essential for caravan destruction during combat or economic changes
 *   - Does not require list resorting after removal
 *   - Maintains list integrity with proper pointer management
 */
void
dest_cvn PARM_1(int, idnum)
{
  CVN_PTR c1_ptr, c2_ptr;

  /* stupidity check */
  if (ntn_ptr == NULL) return;

  /* start the predecessor pointer */
  c2_ptr = (CVN_PTR) NULL;

  /* go through and find the unit */
  for (c1_ptr = ntn_ptr->cvn_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {
    /* check for it */
    if (c1_ptr->cvnid == idnum) break;
    c2_ptr = c1_ptr;
  }

  /* get rid of it */
  if (c1_ptr != NULL) {

    /* check for the head of the list */
    if (c2_ptr == NULL) {
      ntn_ptr->cvn_list = c1_ptr->next;
    } else {
      c2_ptr->next = c1_ptr->next;
    }

    /* now make the space available */
    c1_ptr->next = (CVN_PTR) NULL;
    free(c1_ptr);

    /* the list should be in order */
  }
}

/*
 * dest_city - Remove a city from the national city list
 *
 * Searches through the nation's city list to find a city with the given
 * name and removes it from the linked list. Performs string-based lookup
 * using str_test() for case-insensitive comparison.
 *
 * Parameters:
 *   cname - Name of the city to remove (null-terminated string)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Removes city from ntn_ptr->city_list if found
 *   - Frees memory allocated to the city structure
 *   - Updates linked list pointers to maintain integrity
 *   - No effect if city name not found or ntn_ptr is NULL
 *
 * Testing Notes:
 *   Category: B (Integration) - City list removal requiring nation state setup
 *   Approach: Integration testing with controlled city list setup and name-based lookup
 *   Key Tests: City found (head/middle/tail removal), city not found, NULL nation, empty list, NULL name
 *   Dependencies: ntn_ptr->city_list, city structures with name field, str_test() function
 *   Mock Requirements: Mock nation pointer with city list, mock str_test comparison
 *   Complexity: Moderate - string comparison with linked list manipulation
 *
 * Notes:
 *   - Uses str_test() for case-insensitive name comparison
 *   - Handles removal from any position in the linked list
 *   - Memory management: frees city structure after removal
 */
void
dest_city PARM_1(char *, cname)
{
  CITY_PTR c1_ptr, c2_ptr;

  /* stupidity check */
  if (ntn_ptr == NULL) return;

  /* start the predecessor pointer */
  c2_ptr = (CITY_PTR) NULL;

  /* go through and find the unit */
  for (c1_ptr = ntn_ptr->city_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {
    /* check for it */
    if (str_test(c1_ptr->name, cname) == 0) break;
    c2_ptr = c1_ptr;
  }

  /* get rid of it */
  if (c1_ptr != NULL) {

    /* check for the head of the list */
    if (c2_ptr == NULL) {
      ntn_ptr->city_list = c1_ptr->next;
    } else {
      c2_ptr->next = c1_ptr->next;
    }

    /* now make the space available */
    c1_ptr->next = (CITY_PTR) NULL;
    free(c1_ptr);

    /* the list should be in order */
  }
}

/*
 * dest_item - Remove an item from the national commodity list
 *
 * Searches through the nation's item list to find an item with the given
 * ID number and removes it from the linked list. Performs integer-based
 * lookup for precise item identification.
 *
 * Parameters:
 *   idnum - Unique ID number of the item to remove
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Removes item from ntn_ptr->item_list if found
 *   - Frees memory allocated to the item structure
 *   - Updates linked list pointers to maintain integrity
 *   - No effect if item ID not found or ntn_ptr is NULL
 *
 * Testing Notes:
 *   Category: B (Integration) - Item list removal requiring nation state setup
 *   Approach: Integration testing with controlled item list setup and ID-based lookup
 *   Key Tests: Item found (head/middle/tail removal), item not found, NULL nation, empty list, invalid ID
 *   Dependencies: ntn_ptr->item_list, item structures with itemid field
 *   Mock Requirements: Mock nation pointer with item list, mock item structures
 *   Complexity: Moderate - integer comparison with linked list manipulation
 *
 * Notes:
 *   - Uses itemid field for exact numeric matching
 *   - Handles removal from any position in the linked list
 *   - Memory management: frees item structure after removal
 */
void
dest_item PARM_1(int, idnum)
{
  ITEM_PTR i1_ptr, i2_ptr;

  /* stupidity check */
  if (ntn_ptr == NULL) return;

  /* start the predecessor pointer */
  i2_ptr = (ITEM_PTR) NULL;

  /* go through and find the unit */
  for (i1_ptr = ntn_ptr->item_list;
       i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {
    /* check for it */
    if (i1_ptr->itemid == idnum) break;
    i2_ptr = i1_ptr;
  }

  /* get rid of it */
  if (i1_ptr != NULL) {

    /* check for the head of the list */
    if (i2_ptr == NULL) {
      ntn_ptr->item_list = i1_ptr->next;
    } else {
      i2_ptr->next = i1_ptr->next;
    }

    /* now make the space available */
    i1_ptr->next = (ITEM_PTR) NULL;
    free(i1_ptr);

    /* the list should be in order */
  }
}

/*
 * crt_dmode - Initialize a new display mode into display mode list
 *
 * Creates or retrieves a display mode structure with the given name. If a
 * display mode with the same name already exists, returns the existing one.
 * Otherwise creates a new mode with default settings and adds it to the
 * global display mode list.
 *
 * Parameters:
 *   dmodename - Name of the display mode to create or retrieve
 *
 * Returns:
 *   Pointer to DMODE structure (existing or newly created)
 *   NULL if allocation fails (handled by new_dmode() abort)
 *
 * Side Effects:
 *   - May allocate new DMODE structure via new_dmode()
 *   - Adds new mode to global dmode_list if not found
 *   - Initializes all display settings to default values
 *   - Updates dmode_tptr global pointer during traversal
 *
 * Testing Notes:
 *   Category: C (System) - Display mode management requiring global state initialization
 *   Approach: System testing with full display mode system initialization
 *   Key Tests: New mode creation, existing mode retrieval, empty list, list management
 *   Dependencies: dmode_list global, dmode_tptr global, new_dmode(), strcmp()
 *   Mock Requirements: Extensive mocking of display system globals and structures
 *   Complexity: Complex - global state management with display mode configuration
 *
 * Notes:
 *   - Manages global display mode linked list
 *   - Initializes comprehensive display settings (focus, highlight, style, target)
 *   - Uses string comparison for mode name matching
 */
DMODE_PTR
crt_dmode PARM_1(char *, dmodename)
{
  DMODE_PTR d1_ptr;
  int count;

  /* go through the list */
  for (dmode_tptr = dmode_list;
       dmode_tptr != NULL;
       dmode_tptr = dmode_tptr->next) {

    /* if there is a match, just return it */
    if (strcmp(dmode_tptr->d.name, dmodename) == 0) {
      return(dmode_tptr);
    }

    /* exit when pointing to the last item */
    if (dmode_tptr->next == NULL) break;
  }

  /* now create the new display mode */
  d1_ptr = new_dmode();
  if (dmode_list == NULL) {
    dmode_list = d1_ptr;
  } else {
    dmode_tptr->next = d1_ptr;
  }

  /* now initialize things */
  strcpy(d1_ptr->d.name, dmodename);
  d1_ptr->next = NULL;
  d1_ptr->d.focus = HXPOS_LOWLEFT;
  for (count = 0; count < HXPOS_NUMBER; count++) {
    d1_ptr->d.highlight[count] = HI_KEEP;
    d1_ptr->d.style[count] = DI_KEEP;
    d1_ptr->d.target[count] = 0;
  }
  return(d1_ptr);
}

/*
 * crt_ntn - Initialize a new nation into the world
 *
 * Creates a new nation structure and integrates it into the global world
 * state. Assigns a unique nation slot, initializes all nation attributes
 * to default values, and establishes diplomatic relationships with existing
 * nations based on monster/player status.
 *
 * Parameters:
 *   ntnname - Name of the new nation (copied to nation structure)
 *   actval - Activity value indicating nation type (player/monster/NPC)
 *
 * Returns:
 *   Pointer to newly created NTN structure
 *   NULL if maximum nations reached (ABSMAXNTN exceeded)
 *
 * Side Effects:
 *   - Allocates new NTN structure via new_ntn()
 *   - Assigns nation to first available slot in world.np[] array
 *   - Sets global_int to the assigned nation number
 *   - Initializes all nation lists (army, navy, city, item, caravan) to NULL
 *   - Sets up diplomatic status with all existing nations
 *   - Copies global login name and password to nation
 *
 * Testing Notes:
 *   Category: C (System) - Nation creation requiring full world state initialization
 *   Approach: System testing with complete world state setup and nation management
 *   Key Tests: First nation creation, maximum nations, diplomatic setup, attribute initialization
 *   Dependencies: world.np[] array, global_int, loginname, world.passwd, bute_info[], various globals
 *   Mock Requirements: Complete world state mocking, all global variables and structures
 *   Complexity: Complex - comprehensive world state management with diplomatic system
 *
 * Notes:
 *   - Manages global nation allocation in world.np[] array
 *   - Establishes automatic war status between monsters and other nations
 *   - Initializes all nation attributes from bute_info[] configuration
 *   - Sets default capital position to world center
 */
NTN_PTR
crt_ntn PARM_2(char *, ntnname, int, actval)
{
  int count;
  NTN_PTR n1_ptr, n2_ptr;

  /* make sure it is okay to build a new nation */
  for (count = 1; count < ABSMAXNTN; count++) {
    if ((n1_ptr = world.np[count]) == NULL) break;
  }
  if (count == ABSMAXNTN) return((NTN_PTR) NULL);
  global_int = count;

  /* now create the new nation */
  n1_ptr = new_ntn();
  world.np[global_int] = n1_ptr;

  /* assign some default values */
  clr_memory((char *) n1_ptr, sizeof(NTN_STRUCT));
  strcpy(n1_ptr->name, ntnname);
  strcpy(n1_ptr->login, loginname);
  strcpy(n1_ptr->passwd, world.passwd);
  strcpy(n1_ptr->leader, "");
  n1_ptr->race = TUNKNOWN;
  n1_ptr->mark = '-';
  n1_ptr->capx = MAPX / 2;
  n1_ptr->capy = MAPY / 2;
  n1_ptr->rightedge = MAPX - 1;
  n1_ptr->bottomedge = MAPY - 1;
  n1_ptr->active = actval;
  n1_ptr->army_list = (ARMY_PTR) NULL;
  n1_ptr->navy_list = (NAVY_PTR) NULL;
  n1_ptr->city_list = (CITY_PTR) NULL;
  n1_ptr->item_list = (ITEM_PTR) NULL;
  n1_ptr->cvn_list = (CVN_PTR) NULL;
  for (count = 0; count < BUTE_NUMBER; count++) {
    n1_ptr->attribute[count] = bute_info[count].start_val;
  }

  /* now reset the diplomacy vectors */
  n2_ptr = NULL;
  n1_ptr->dstatus[UNOWNED] = DIP_NEUTRAL;
  for (count = 1; count < ABSMAXNTN; count++) {
    if (((n2_ptr = world.np[count]) != NULL) &&
	(count != global_int) &&
	(n_ismonster(n1_ptr->active) ||
	 n_ismonster(n2_ptr->active))) {
      n1_ptr->dstatus[count] = DIP_WAR;
      n2_ptr->dstatus[global_int] = DIP_WAR;
    } else {
      n1_ptr->dstatus[count] = DIP_UNMET;
      if (n2_ptr != NULL) 
	n2_ptr->dstatus[global_int] = DIP_UNMET;
    }
  }

  /* done... nation will be resorted during the writing */
  return(n1_ptr);
}

/*
 * crt_army - Build an army unit assigning an ID number based on unit type
 *
 * Creates a new army unit of the specified type and assigns it a unique
 * ID number within the appropriate range for that unit type. Initializes
 * the unit with default values and adds it to the nation's army list in
 * sorted order.
 *
 * Parameters:
 *   utype - Unit type code determining ID range and unit characteristics
 *
 * Returns:
 *   Pointer to newly created ARMY structure
 *   NULL if nation is NULL, ID range exhausted, or allocation fails
 *
 * Side Effects:
 *   - Allocates new ARMY structure via new_army()
 *   - Assigns unique armyid via find_newarmynum()
 *   - Adds unit to ntn_ptr->army_list at head position
 *   - Calls army_sort(FALSE) to maintain list order
 *   - Initializes all army attributes to default values
 *
 * Testing Notes:
 *   Category: B (Integration) - Army creation requiring nation state and ID management
 *   Approach: Integration testing with controlled nation setup and unit ID tracking
 *   Key Tests: Various unit types, ID assignment, list management, maximum ID limits
 *   Dependencies: ntn_ptr, find_newarmynum(), new_army(), army_sort(), set_speed()
 *   Mock Requirements: Mock nation pointer, mock ID generation, mock army list management
 *   Complexity: Moderate - unit creation with ID management and list integration
 *
 * Notes:
 *   - Uses find_newarmynum() for type-specific ID assignment
 *   - Enforces MAX_IDTYPE limit to prevent ID overflow
 *   - Maintains sorted army list via army_sort()
 *   - Initializes unit status to ST_DEFEND with normal speed
 */
ARMY_PTR
crt_army PARM_1(int, utype)
{
  ARMY_PTR a1_ptr;
  int newunum;

  /* make sure it is okay to find a new unit */
  if (ntn_ptr == NULL) return((ARMY_PTR) NULL);

  /* find location to begin count from */
  newunum = find_newarmynum(utype);

  /* verify that a new unit is possible */
  if (newunum >= MAX_IDTYPE) {
#ifdef DEBUG
    sprintf(string, "Ran out of army numbers %d >= %ld",
	    newunum, (long) MAX_IDTYPE);
    bottommsg(string);
    sleep(1);
#endif /*DEBUG*/
    return( (ARMY_PTR) NULL );
  }

  /* now create the new unit */
  a1_ptr = new_army();

  /* assign some default values */
  clr_memory((char *) a1_ptr, sizeof(ARMY_STRUCT));
  a1_ptr->armyid = newunum;
  a1_ptr->unittype = utype;
  a1_ptr->status = ST_DEFEND;
  set_speed(a1_ptr->status, SPD_NORMAL);
  a1_ptr->efficiency = 100;
  a1_ptr->max_eff = 100;
  a1_ptr->leader = EMPTY_HOLD;
  a1_ptr->nrby = NULL;

  /* now reorganize the army list */
  a1_ptr->next = ntn_ptr->army_list;
  ntn_ptr->army_list = a1_ptr;
  army_sort(FALSE);

  /* return the location of the new unit */
  return(a1_ptr);
}

/*
 * crt_navy - Add a new navy to the nation list
 *
 * Creates a new naval unit and assigns it the lowest available ID number
 * starting from EMPTY_HOLD + 1. Initializes the navy with default values
 * and adds it to the nation's navy list in sorted order.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly created NAVY structure
 *   NULL if nation is NULL or ID range exhausted (>= MAX_IDTYPE)
 *
 * Side Effects:
 *   - Allocates new NAVY structure via new_navy()
 *   - Assigns lowest available navyid starting from EMPTY_HOLD + 1
 *   - Adds unit to ntn_ptr->navy_list at head position
 *   - Calls navy_sort() to maintain list order
 *   - Initializes all navy attributes and ship arrays to default values
 *
 * Testing Notes:
 *   Category: B (Integration) - Navy creation requiring nation state and ID management
 *   Approach: Integration testing with controlled nation setup and naval ID tracking
 *   Key Tests: First navy creation, ID collision avoidance, list management, maximum ID limits
 *   Dependencies: ntn_ptr, new_navy(), navy_sort(), NSHP_NUMBER constant
 *   Mock Requirements: Mock nation pointer, mock navy list management, mock ship arrays
 *   Complexity: Moderate - naval unit creation with ID collision detection and list integration
 *
 * Notes:
 *   - Scans existing navy list to find lowest available ID
 *   - Initializes all ship types and efficiency arrays
 *   - Sets default status to ST_CARRY for transport operations
 *   - Maintains sorted navy list via navy_sort()
 */
NAVY_PTR
crt_navy PARM_0(void)
{
  NAVY_PTR n1_ptr;
  int i, startnum = EMPTY_HOLD + 1;

  /* check nation */
  if (ntn_ptr == NULL) return( (NAVY_PTR) NULL );

  /* find the lowest available id */
  for (n1_ptr = ntn_ptr->navy_list;
       n1_ptr != NULL;
       n1_ptr = n1_ptr->next) {
    /* increment if a unit already has that number */
    if (startnum == n1_ptr->navyid) startnum++;
    else if (startnum < n1_ptr->navyid) break;
  }

  /* check limit */
  if (startnum >= MAX_IDTYPE) return( (NAVY_PTR) NULL);

  /* create the unit */
  n1_ptr = new_navy();
  clr_memory((char *) n1_ptr, sizeof(NAVY_STRUCT));
  n1_ptr->navyid = startnum;
  n1_ptr->status = ST_CARRY;
  n1_ptr->armynum = EMPTY_HOLD;
  n1_ptr->cvnnum = EMPTY_HOLD;
  for (i = 0; i < NSHP_NUMBER; i++) {
    n1_ptr->ships[i] = 0;
    n1_ptr->efficiency[i] = 100;
  }

  /* reorganize nation fleet */
  n1_ptr->next = ntn_ptr->navy_list;
  ntn_ptr->navy_list = n1_ptr;
  navy_sort();

  /* return the value */
  return (n1_ptr);
}

/*
 * crt_cvn - Create and add a new caravan to the nation list
 *
 * Creates a new caravan unit with automatically assigned ID and adds it
 * to the current nation's caravan list. Assigns the lowest available ID
 * number starting from 1, checking for existing ID conflicts.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly created caravan on success
 *   NULL if nation is NULL or maximum ID limit reached
 *
 * Side Effects:
 *   - Allocates memory for new caravan via new_cvn()
 *   - Clears allocated memory to zero
 *   - Sets default values: ID (auto-assigned), STAT=ST_CARRY, EFF=100
 *   - Adds caravan to nation's cvn_list as head node
 *   - Calls cvn_sort() to maintain sorted order
 *
 * Testing Notes:
 *   Category: B (Integration) - Caravan creation requiring nation state and list management
 *   Approach: Integration testing with controlled nation setup and caravan list validation
 *   Key Tests: NULL nation check, ID assignment sequence, list insertion, sorting verification, limit testing
 *   Dependencies: ntn_ptr global, new_cvn(), clr_memory(), cvn_sort(), MAX_IDTYPE constant
 *   Mock Requirements: Mock nation pointer, mock new_cvn allocation, mock cvn_sort
 *   Complexity: Moderate - ID collision detection with list management and sorting
 *
 * Notes:
 *   - Requires valid ntn_ptr global to be set
 *   - ID assignment starts at 1 and increments to avoid conflicts
 *   - Returns NULL on ID exhaustion (>= MAX_IDTYPE)
 *   - New caravan inserted at list head for efficiency
 */
CVN_PTR
crt_cvn PARM_0(void)
{
  int startnum = 1;

  /* check nation */
  if (ntn_ptr == NULL) return( (CVN_PTR) NULL );

  /* find the lowest available id */
  for (cvn_tptr = ntn_ptr->cvn_list;
       cvn_tptr != NULL;
       cvn_tptr = cvn_tptr->next) {
    /* increment if a unit already has that number */
    if (startnum == CVNT_ID) startnum++;
    else if (startnum < CVNT_ID) break;
  }

  /* check limit */
  if (startnum >= MAX_IDTYPE) return( (CVN_PTR) NULL);

  /* create the unit */
  cvn_tptr = new_cvn();
  clr_memory((char *) cvn_tptr, sizeof(CVN_STRUCT));
  CVNT_ID = startnum;
  CVNT_STAT = ST_CARRY;
  CVNT_EFF = 100;

  /* reorganize nation fleet */
  cvn_tptr->next = ntn_ptr->cvn_list;
  ntn_ptr->cvn_list = cvn_tptr;
  cvn_sort();

  /* return the value */
  return (cvn_tptr);
}

/*
 * crt_item - Create and add a new item to the nation list
 *
 * Creates a new item with automatically assigned ID and adds it to the
 * current nation's item list. Assigns the lowest available ID number
 * starting from 1, checking for existing ID conflicts.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Pointer to newly created item on success
 *   NULL if nation is NULL or maximum ID limit reached
 *
 * Side Effects:
 *   - Allocates memory for new item via new_item()
 *   - Clears allocated memory to zero
 *   - Sets default values: ID (auto-assigned), EFF=100
 *   - Adds item to nation's item_list as head node
 *   - Calls item_sort() to maintain sorted order
 *
 * Testing Notes:
 *   Category: B (Integration) - Item creation requiring nation state and list management
 *   Approach: Integration testing with controlled nation setup and item list validation
 *   Key Tests: NULL nation check, ID assignment sequence, list insertion, sorting verification, limit testing
 *   Dependencies: ntn_ptr global, new_item(), clr_memory(), item_sort(), MAX_IDTYPE constant
 *   Mock Requirements: Mock nation pointer, mock new_item allocation, mock item_sort
 *   Complexity: Moderate - ID collision detection with list management and sorting
 *
 * Notes:
 *   - Requires valid ntn_ptr global to be set
 *   - ID assignment starts at 1 and increments to avoid conflicts
 *   - Returns NULL on ID exhaustion (>= MAX_IDTYPE)
 *   - New item inserted at list head for efficiency
 */
ITEM_PTR
crt_item PARM_0(void)
{
  ITEM_PTR i1_ptr;
  int startnum = 1;

  /* check nation */
  if (ntn_ptr == NULL) return( (ITEM_PTR) NULL );

  /* find the lowest available id */
  for (i1_ptr = ntn_ptr->item_list;
       i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {
    /* increment if a unit already has that number */
    if (startnum == i1_ptr->itemid) startnum++;
    else if (startnum < i1_ptr->itemid) break;
  }

  /* check limit */
  if (startnum >= MAX_IDTYPE) return( (ITEM_PTR) NULL);

  /* create the unit */
  i1_ptr = new_item();
  clr_memory((char *) i1_ptr, sizeof(ITEM_STRUCT));
  i1_ptr->itemid = startnum;

  /* reorganize nation fleet */
  i1_ptr->next = ntn_ptr->item_list;
  ntn_ptr->item_list = i1_ptr;
  item_sort();

  /* return the value */
  return (i1_ptr);
}

/*
 * crt_city - Create and add a new city to the nation city list
 *
 * Creates a new city with the specified name and automatically assigned ID,
 * adding it to the current nation's city list. Ensures city name uniqueness
 * and assigns the lowest available city ID number.
 *
 * Parameters:
 *   cname - Name for the new city (must be unique across all nations)
 *
 * Returns:
 *   Pointer to newly created city on success
 *   NULL if nation is NULL or city name already exists
 *
 * Side Effects:
 *   - Checks city name uniqueness via citybyname()
 *   - Allocates memory for new city via new_city()
 *   - Copies cname to city->name field
 *   - Sets default values: cityid (auto-assigned), pop=0
 *   - Adds city to nation's city_list as head node
 *   - Calls city_sort() to maintain sorted order
 *
 * Testing Notes:
 *   Category: B (Integration) - City creation requiring nation state, name validation, and list management
 *   Approach: Integration testing with controlled nation setup and city list validation
 *   Key Tests: NULL nation check, duplicate name rejection, ID assignment sequence, list insertion, sorting verification
 *   Dependencies: ntn_ptr global, citybyname(), new_city(), strcpy(), city_sort()
 *   Mock Requirements: Mock nation pointer, mock citybyname lookup, mock new_city allocation, mock city_sort
 *   Complexity: Moderate - Name uniqueness checking with ID collision detection and list management
 *
 * Notes:
 *   - Requires valid ntn_ptr global to be set
 *   - City names must be globally unique across all nations
 *   - ID assignment starts at 1 and increments to avoid conflicts
 *   - New city inserted at list head for efficiency
 */
CITY_PTR
crt_city PARM_1(char *, cname)
{
  CITY_PTR c1_ptr;
  int count, num_val = 0, new_num = FALSE;

  /* check nation */
  if (ntn_ptr == NULL) return( (CITY_PTR) NULL );

  /* check for uniqueness */
  if (citybyname(cname) != NULL) {
    return( (CITY_PTR) NULL);
  }

  /* find a unique number for starting */
  do {
    CITY_PTR ctmp_ptr;
    new_num = TRUE;
    num_val++;
    for (ctmp_ptr = ntn_ptr->city_list;
	 ctmp_ptr != NULL;
	 ctmp_ptr = ctmp_ptr->next) {
      if (num_val == ctmp_ptr->cityid) {
	new_num = FALSE;
	break;
      }
    }
  } while (new_num == FALSE);

  /* assign default values */
  c1_ptr = new_city();
  strcpy(c1_ptr->name, cname);
  c1_ptr->xloc = 0;
  c1_ptr->yloc = 0;
  c1_ptr->cityid = num_val;
  c1_ptr->i_people = 0;
  c1_ptr->weight = 0;
  c1_ptr->fortress = 0;
  c1_ptr->s_talons = (itemtype) 0;
  c1_ptr->cmd_flag = 0L;
  for (count = 0; count < MTRLS_NUMBER; count++) {
    c1_ptr->c_mtrls[count] = (itemtype) 0;
    c1_ptr->i_mtrls[count] = (itemtype) 0;
    c1_ptr->m_mtrls[count] = (itemtype) 0;
    c1_ptr->auto_flags[count] = 0L;
  }

  /* reorganize national townships */
  c1_ptr->next = ntn_ptr->city_list;
  ntn_ptr->city_list = c1_ptr;
  city_sort();

  /* return the value */
  return (c1_ptr);
}

/*
 * dest_ntn - Destroy a nation and free all associated memory
 *
 * Completely destroys a nation specified by name, freeing all memory
 * associated with its armies, navies, cities, caravans, and items.
 * Removes the nation from the world array without realigning diplomacy.
 *
 * Parameters:
 *   nname - Name of the nation to destroy
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Searches world.np[] array to find nation by name
 *   - Sets world.np[index] to NULL to unlink nation
 *   - Frees all armies in nation's army_list
 *   - Frees all navies in nation's navy_list
 *   - Frees all cities in nation's city_list
 *   - Frees all caravans in nation's cvn_list
 *   - Frees all items in nation's item_list
 *   - Frees the nation structure itself
 *   - Does NOT realign diplomacy vectors (caller's responsibility)
 *
 * Testing Notes:
 *   Category: C (System) - Nation destruction requiring full world state and complex cleanup
 *   Approach: System testing with complete world initialization and memory validation
 *   Key Tests: Valid nation destruction, non-existent nation handling, empty lists, full cleanup verification
 *   Dependencies: world.np[] array, all entity lists (army, navy, city, cvn, item), strcmp(), free()
 *   Mock Requirements: Mock world structure, mock entity lists with memory tracking
 *   Complexity: Complex - Multi-entity cleanup with world state modification and memory management
 *
 * Notes:
 *   - Performs linear search through world.np[] array
 *   - Returns silently if nation not found
 *   - Memory cleanup is thorough but doesn't handle diplomacy realignment
 *   - Critical for game state management and memory leak prevention
 *   - Order of cleanup: unlink nation, then free all entities
 */
void
dest_ntn PARM_1 (char *, nname)
{
  ARMY_PTR a1_ptr, a2_ptr;
  NAVY_PTR n1_ptr, n2_ptr;
  CITY_PTR c1_ptr, c2_ptr;
  CVN_PTR v1_ptr, v2_ptr;
  ITEM_PTR i1_ptr, i2_ptr;
  int cntry;

  /* this routine need only free the memory, */
  /* not reallign diplomacy vectors.         */
  for (cntry = 1; cntry < ABSMAXNTN; cntry++) {
    if ((ntn_tptr = world.np[cntry]) != NULL) {
      if (strcmp(nname, ntn_tptr->name) == 0) break;
    }
  }

  /* return if nation is not found */
  if (cntry == ABSMAXNTN) return;

  /* first... unlink the nation */
  world.np[cntry] = (NTN_PTR) NULL;

  /* now go and remove all of the armies */
  a2_ptr = (ARMY_PTR) NULL;
  for (a1_ptr = ntn_tptr->army_list;
       a1_ptr != NULL; ) {

    /* go the the next element */
    a2_ptr = a1_ptr;
    a1_ptr = a1_ptr->next;

    /* destroy the old element */
    if (a2_ptr != NULL) {
      a2_ptr->next = (ARMY_PTR) NULL;
      a2_ptr->nrby = (ARMY_PTR) NULL;
      free(a2_ptr);
    }

  }

  /* >poof< goes all of the navies */
  n2_ptr = (NAVY_PTR) NULL;
  for (n1_ptr = ntn_tptr->navy_list;
       n1_ptr != NULL; ) {

    /* go the the next element */
    n2_ptr = n1_ptr;
    n1_ptr = n1_ptr->next;

    /* destroy the old element */
    if (n2_ptr != NULL) {
      n2_ptr->next = (NAVY_PTR) NULL;
      free(n2_ptr);
    }

  }

  /* convoy munching */
  v2_ptr = (CVN_PTR) NULL;
  for (v1_ptr = ntn_tptr->cvn_list;
       v1_ptr != NULL; ) {

    /* go the the next element */
    v2_ptr = v1_ptr;
    v1_ptr = v1_ptr->next;

    /* destroy the old element */
    if (v2_ptr != NULL) {
      v2_ptr->next = (CVN_PTR) NULL;
      free(v2_ptr);
    }

  }

  /* play godzilla and destroy cities! */
  c2_ptr = (CITY_PTR) NULL;
  for (c1_ptr = ntn_tptr->city_list;
       c1_ptr != NULL; ) {

    /* go the the next element */
    c2_ptr = c1_ptr;
    c1_ptr = c1_ptr->next;

    /* destroy the old element */
    if (c2_ptr != NULL) {
      c2_ptr->next = (CITY_PTR) NULL;
      free(c2_ptr);
    }

  }

  /* now gobble up the commodities */
  i2_ptr = (ITEM_PTR) NULL;
  for (i1_ptr = ntn_tptr->item_list;
       i1_ptr != NULL; ) {

    /* go the the next element */
    i2_ptr = i1_ptr;
    i1_ptr = i1_ptr->next;

    /* destroy the old element */
    if (i2_ptr != NULL) {
      i2_ptr->next = (ITEM_PTR) NULL;
      free(i2_ptr);
    }

  }
  
  /* now take care of the nation itself */
#ifndef VMS
  sprintf (string, "%s %s* 2> /dev/null", REMOVE_NAME, ntn_tptr->name);
#else
  sprintf (string, "%s %s.*;* 2> /dev/null", REMOVE_NAME, ntn_tptr->name);
#endif
  system(string);
  free(ntn_tptr);
}

/* element to keep track of switched nations */
static ntntype slot_val[ABSMAXNTN];

/*
 * ntn_swap - Swap two nations in world array and track the swap
 *
 * Swaps two nation pointers in the world.np[] array and maintains
 * corresponding swap tracking in the slot_val[] array. Used as a
 * helper function for nation sorting operations.
 *
 * Parameters:
 *   left - Index of first nation to swap
 *   right - Index of second nation to swap
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Swaps world.np[left] and world.np[right] pointers
 *   - Swaps slot_val[left] and slot_val[right] tracking values
 *
 * Testing Notes:
 *   Category: A (Unit) - Simple array swapping with clear dependencies
 *   Approach: Unit tests with mock world array and slot tracking validation
 *   Key Tests: Basic swap operation, boundary indices, swap tracking verification
 *   Dependencies: world.np[] array, slot_val[] static array
 *   Mock Requirements: Mock world structure, mock slot_val array
 *   Complexity: Simple - straightforward array element swapping
 *
 * Notes:
 *   - Static function used internally by ntn_qsort()
 *   - Essential for maintaining nation order tracking during sorts
 *   - Both nation pointers and tracking values must be swapped together
 */
static void
ntn_swap PARM_2(int, left, int, right)
{
  NTN_PTR n_ptr;
  int value;

  /* swap the nation */
  n_ptr = world.np[left];
  world.np[left] = world.np[right];
  world.np[right] = n_ptr;

  /* keep track of the swap */
  value = slot_val[left];
  slot_val[left] = slot_val[right];
  slot_val[right] = value;
}

/*
 * ntn_qsort - Recursive quicksort for nations by name
 *
 * Implements the quicksort algorithm from K&R Second Edition (Page 110)
 * to sort nations alphabetically by name. Uses ntn_swap() to maintain
 * both nation pointer order and tracking information.
 *
 * Parameters:
 *   left - Left boundary index for sorting range
 *   right - Right boundary index for sorting range
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Recursively sorts world.np[] array in specified range
 *   - Uses str_test() for string comparison
 *   - Calls ntn_swap() to maintain tracking during swaps
 *
 * Testing Notes:
 *   Category: A (Unit) - Recursive sorting algorithm with clear dependencies
 *   Approach: Unit tests with mock nation arrays and string comparison validation
 *   Key Tests: Empty range, single element, sorted array, reverse sorted, random order, boundary conditions
 *   Dependencies: world.np[] array, str_test(), ntn_swap()
 *   Mock Requirements: Mock world structure with nation names, mock str_test comparison function
 *   Complexity: Simple - standard quicksort implementation with clear recursive structure
 *
 * Notes:
 *   - Static function used internally by ntn_sort()
 *   - Implements classic K&R quicksort algorithm
 *   - Partition element selection uses midpoint strategy
 *   - Recursive termination when left >= right
 */
static void
ntn_qsort PARM_2(int, left, int, right)
{
  int i, last;

  /* check for single elements */
  if (left >= right) return;

  ntn_swap(left, (left + right) / 2);
  last = left;
  for (i = left + 1; i <= right; i++) {
    if (str_test((world.np[i])->name, (world.np[left])->name) < 0) {
      ntn_swap(++last, i);
    }
  }
  ntn_swap(left, last);
  ntn_qsort(left, last - 1);
  ntn_qsort(last + 1, right);
}

/*
 * ntn_sort - Sort nations alphabetically and compact nation array
 *
 * Major reorganization function that sorts all nations alphabetically,
 * removes empty slots, and updates all cross-references throughout the
 * game world. This is one of the most complex functions in the system.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Sorts world.np[] array alphabetically by nation name
 *   - Removes empty nation slots and compacts array
 *   - Updates MAXNTN to new nation count
 *   - Recalculates all diplomacy status vectors between nations
 *   - Updates sector ownership throughout the world map
 *   - Resets unowned sectors to MAJ_NONE designation
 *   - Prints progress messages to update log
 *   - Calls verify_data() in debug mode
 *
 * Notes:
 *   - Uses quicksort algorithm via ntn_qsort() helper function
 *   - Critical for maintaining game data integrity
 *   - Updates ALL references to nation IDs throughout the world
 *   - Must be called after any nation creation/destruction
 *   - Heavy operation that touches most game data structures
 */
void
ntn_sort PARM_0(void)
{
  register int i, j;
  int count;
  ntntype newloc[ABSMAXNTN];
  uns_char stats[ABSMAXNTN];

  /* let them know what is up */
  if (!in_curses) {
    fprintf(fupdate, "sorting nations...");
  }

  /* assign default locations */
  for (i = 0; i < ABSMAXNTN; i++) {
    slot_val[i] = UNOWNED;
    newloc[i] = UNOWNED;
  }

  /* go through and shift out any empty slots */
  count = 1;
  for (i = 1; i < ABSMAXNTN; i++) {
    if (world.np[i] != NULL) {
      slot_val[count] = i;
      world.np[count++] = world.np[i];
    }
  }

  /* there should NEVER be a nation zero */
  if (world.np[0] != NULL) {
    slot_val[0] = count;
    world.np[count++] = world.np[0];
  }

  /* now make sure all remaining slots are nulled out */
  for (i = count; i < ABSMAXNTN; i++) {
    world.np[i] = (NTN_PTR) NULL;
  }

  /* the value of MAXNTN may now be set */
  MAXNTN = count;

  /* now perform a sort on the remaining nations */
  ntn_qsort(1, MAXNTN - 1);

  /* figure out where things came from */
  for (i = 0; i < ABSMAXNTN; i++) {
    if (slot_val[i] != UNOWNED) {
      newloc[slot_val[i]] = i;
    }
  }

  /* go through all nations; re-assigning storage */
  for (i = 0; i < MAXNTN; i++) {
    
    /* locate nation informatin */
    if ((ntn_tptr = world.np[i]) == NULL) continue;

    /* store diplomacy vectors */
    for (j = 0; j < ABSMAXNTN; j++) {
      stats[j] = ntn_tptr->dstatus[j];
    }

    /* go through and switch those vectors which need it */
    for (j = 0; j < ABSMAXNTN; j++) {
      if (newloc[j] == UNOWNED) {
	ntn_tptr->dstatus[j] = DIP_UNMET;
      } else {
	ntn_tptr->dstatus[newloc[j]] = stats[j];
      }
    }
    ntn_tptr->dstatus[UNOWNED] = DIP_NEUTRAL;
  }

  /* now assign sector ownership properly */
  for (i = 0; i < MAPX; i++)
    for (j = 0; j < MAPY; j++) {
      if ((newloc[sct[i][j].owner] == UNOWNED) &&
	  (sct[i][j].owner != UNOWNED)) {
	sct[i][j].designation = MAJ_NONE;
	sct[i][j].people = 0;
      }
      sct[i][j].owner = newloc[sct[i][j].owner];
    }
  if (!in_curses) {
    fprintf(fupdate, "done\n");
  }
#ifdef DEBUG
  verify_data(__FILE__, __LINE__);
#endif /*DEBUG*/
}

/*
 * unum_defaults - Initialize default unit numbering scheme
 *
 * Sets up the default unit numbering ranges for different unit types.
 * This establishes the standard ID assignment patterns used throughout
 * the game for consistent unit numbering.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Calls resetnumbers() to clear existing numbering
 *   - Sets up default numbering slots via newslotnumber()
 *   - Assigns standard ranges for different unit classes:
 *     * Leaders: start at 1
 *     * Spellcasters: start at 25/50 (depending on SAVE_SPACE)
 *     * Monsters: start at 50/100
 *     * Scouts/Agents: start at 200/1000
 *   - Calls startnumber() for each unit type class
 *
 * Testing Notes:
 *   Category: B (Integration) - Unit numbering system requiring game state and configuration setup
 *   Approach: Integration testing with controlled initialization state and numbering verification
 *   Key Tests: Default numbering ranges, SAVE_SPACE flag variations, slot allocation verification, startnumber calls
 *   Dependencies: resetnumbers(), newslotnumber(), startnumber(), SAVE_SPACE compilation flag
 *   Mock Requirements: Mock numbering system functions, mock compilation flag testing
 *   Complexity: Moderate - Configuration-dependent initialization with multiple subsystem calls
 *
 * Notes:
 *   - Compilation flag SAVE_SPACE affects starting numbers
 *   - Essential for consistent unit ID assignment
 *   - Called during game initialization
 *   - Establishes numbering conventions used by find_newarmynum()
 */
void
unum_defaults PARM_0(void)
{
  /* now initialize the default unit numbering */
  resetnumbers();
#ifdef SAVE_SPACE
  newslotnumber(100, 0, 0);
  startnumber("all_leader 1", " in main_init");
  startnumber("all_spellcaster 25", " in main_init");
  startnumber("all_monster 50", " in main_init");
  startnumber("all_scout 200", " in main_init");
  startnumber("all_agent 200", " in main_init");
#else
  newslotnumber(200, 0, 0);
  startnumber("all_leader 1", " in main_init");
  startnumber("all_spellcaster 50", " in main_init");
  startnumber("all_monster 100", " in main_init");
  startnumber("all_scout 1000", " in main_init");
  startnumber("all_agent 1000", " in main_init");
#endif /* SAVE_SPACE */
}
