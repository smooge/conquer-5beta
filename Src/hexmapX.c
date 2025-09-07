/*
 * hexmapX.c - Hexagonal and rectangular map coordinate system utilities
 *
 * This file implements the core map coordinate system and range calculation
 * utilities that support both hexagonal and rectangular map layouts in the
 * Conquer game system. These functions provide essential geometric operations
 * for determining sector relationships, proximity calculations, and area-of-effect
 * algorithms.
 *
 * Key Function Categories:
 * - Map Loop Operations: Iterate over sectors within specified range
 * - Distance Calculations: Determine if coordinates are within range
 * - Coordinate System Support: Handle both hex and rectangular map types
 *
 * Map Types Supported:
 * - Hexagonal Maps: Uses complex geometric calculations for 6-sided adjacency
 * - Rectangular Maps: Uses standard Cartesian distance calculations
 *
 * The hexagonal map system implements a sophisticated coordinate transformation
 * that accounts for the offset pattern of hexagonal grids. This enables realistic
 * movement patterns and area effects that follow natural hex geometry rather
 * than forcing square grid limitations.
 *
 * These utilities are used extensively by movement systems, combat calculations,
 * spell effects, influence ranges, and any operation requiring spatial relationships
 * between map sectors.
 */
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

/*
 * map_loop - Execute function on all sectors within specified range of center
 *
 * Iterates through all map sectors within a given range of a center coordinate,
 * calling the provided function for each valid sector found. Handles both
 * hexagonal and rectangular map geometries with appropriate distance calculations.
 * The iteration pattern ensures proper coverage of the target area according
 * to the selected map type.
 *
 * Parameters:
 *   xcent - X coordinate of center sector for iteration
 *   ycent - Y coordinate of center sector for iteration  
 *   range - Maximum distance from center to include sectors
 *   perform_func - Function pointer to call for each sector (void func(int x, int y))
 *
 * Returns:
 *   Nothing (calls function for side effects)
 *
 * Side Effects:
 *   - Calls perform_func for each valid sector within range
 *   - Skips sectors that are off the map (invalid Y coordinates)
 *   - Uses global world.hexmap to determine iteration geometry
 *
 * Notes:
 *   - Hexagonal maps: Uses complex geometric calculations for natural hex patterns
 *   - Rectangular maps: Uses simple square range iteration
 *   - X coordinates wrap around map edges using modulo arithmetic
 *   - Y coordinates are validated with Y_ONMAP() before function calls
 *   - Essential for area-of-effect calculations, influence ranges, movement options
 *   - Function pointer allows flexible operations without code duplication
 */
void
map_loop PARM_4(int, xcent, int, ycent, int, range, FNCV2, perform_func)
{
  register int xloc, yloc;
  int xend, xreal, ystart, yend, ycount;

  /* calculate the horizontal range */
  xend = xcent + range;

  /* now cruise the horizontal */
  for (xloc = xcent - range; xloc <= xend; xloc++) {

    /* find the range for the vertical */
    if (world.hexmap) {
      ycount = 2 * range - abs(xcent - xloc);
      ystart = ycent - (ycount + (xcent + 1) % 2) / 2;
      yend = ystart + ycount;
    } else {
      ystart = ycent - range;
      yend = ycent + range;
    }

    /* find the proper x location */
    xreal = (xloc + MAPX) % MAPX;

    /* cruise the vertical */
    for (yloc = ystart; yloc <= yend; yloc++) {

      /* if it is on the map... do the work */
      if (Y_ONMAP(yloc)) {

	/* call it */
	(*perform_func)(xreal, yloc);

      }

    }

  }
}

/*
 * map_within - Determine if two coordinates are within specified range
 *
 * Calculates whether two map coordinates are within a given range of each other,
 * accounting for both hexagonal and rectangular map geometries as well as
 * map wrapping in the X direction. The function implements sophisticated
 * distance calculations that respect the geometric properties of each map type.
 *
 * Parameters:
 *   xa - X coordinate of first location
 *   ya - Y coordinate of first location
 *   xb - X coordinate of second location
 *   yb - Y coordinate of second location
 *   range - Maximum allowable distance between locations
 *
 * Returns:
 *   TRUE if locations are within the specified range
 *   FALSE if locations are beyond the range limit
 *
 * Notes:
 *   - Handles map wrapping: Calculates shortest X distance across map edges
 *   - Hexagonal maps: Uses complex geometric calculations for 6-sided adjacency
 *     * Accounts for hex offset patterns using coordinate transformation
 *     * Implements cone-based range checking for natural hex geometry
 *     * Different calculation for odd vs even X coordinates
 *   - Rectangular maps: Uses standard Cartesian distance (max of X,Y differences)
 *   - Essential for determining spell ranges, movement limits, area effects
 *   - Used by combat systems, magic effects, and proximity-based game mechanics
 *   - Optimized for frequent calls in game loops and calculations
 */
int
map_within PARM_5(int, xa, int, ya, int, xb, int, yb, int, range)
{
  int hold = FALSE, xdiff, ydiff, aydiff;

  /* find some info */
  xdiff = xa - xb;
  xdiff = abs(xdiff);
  ydiff = ya - yb;
  aydiff = abs(ydiff);

  /* find the horizontal difference */
  xdiff = xdiff % MAPX;
  if (xdiff > MAPX - xdiff) {
    xdiff = MAPX - xdiff;
  }

  /* depends on map type */
  if (world.hexmap) {

    /* translate */
    if ((xa % 2) == 1) {
      ydiff = -ydiff;
    }

    /* is it within this number of moves? */
    if ((aydiff <= range) &&
	(xdiff <= range)) {

      /* find central cone */
      if (aydiff <= range - 3) {
	/* always fine in here */
	hold = TRUE;
      } else if (xdiff <= (range - aydiff) * 2 + (ydiff >= 0)) {
	/* a bit tricky, but okay here now as well */
	hold = TRUE;
      }
    }

  } else {

    /* is it within range? */
    if ((aydiff <= range) &&
	(xdiff <= range)) {
      hold = TRUE;
    }

  }

  /* all done */
  return(hold);
}
