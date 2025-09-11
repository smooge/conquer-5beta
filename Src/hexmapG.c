/*
 * hexmapG.c - Hexagonal Map Display and Rendering System
 *
 * This file contains the complete map display and visualization system for Conquer,
 * supporting both hexagonal and rectangular map modes. It handles all aspects of
 * map rendering, cursor positioning, movement navigation, visibility calculation,
 * and interactive display features.
 *
 * CORE FUNCTIONALITY:
 * - Map character generation and sector rendering with multiple display modes
 * - 8-directional cursor movement and screen-based scrolling navigation
 * - Complex highlighting system for units, nations, resources, and strategic info
 * - Fog-of-war visibility calculation based on unit sight ranges and owned sectors
 * - Real-time map updates with efficient redraw strategies
 * - Coordinate system management with relative and absolute positioning
 *
 * DISPLAY MODES SUPPORTED:
 * - Terrain: elevation, vegetation, and geographic features
 * - Political: nation ownership, diplomatic relations, race information
 * - Military: unit locations, movement costs, defense values
 * - Economic: resources (food, metal, jewels, magic), trade goods, city weights
 * - Strategic: designations, supported sectors, region highlighting
 *
 * HIGHLIGHTING SYSTEM:
 * - Unit-based: moveable units, scouts, owned/all units
 * - Political: allied/enemy/neutral nations, ownership patterns
 * - Resource-based: trade goods, specific designations, unsupported sectors
 * - Strategic: movement range, region control, line-of-sight areas
 *
 * VISIBILITY MECHANICS:
 * - Owned sectors provide base visibility in surrounding area
 * - Army units extend sight range based on type and status
 * - Navy units provide coastal and sea visibility
 * - Caravan units offer limited local visibility
 * - Special units (agents, scouts) have enhanced sight capabilities
 * - Fog-of-war system tracks multiple visibility levels (none/partial/most/full/all)
 *
 * COORDINATE SYSTEMS:
 * - Absolute coordinates: fixed world position (XREAL, YREAL)
 * - Cursor coordinates: screen position (xcurs, ycurs) 
 * - Offset coordinates: screen viewport position (xoffset, yoffset)
 * - Relative coordinates: nation-centered coordinate system for players
 *
 * The system seamlessly switches between hexagonal and rectangular map modes,
 * automatically adjusting movement patterns, display algorithms, and coordinate
 * calculations to match the selected map topology.
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
#include "dataG.h"
#include "armyX.h"
#include "cityX.h"
#include "moveX.h"
#include "navyX.h"
#include "desigX.h"
#include "magicX.h"
#include "racesX.h"
#include "activeX.h"
#include "elevegX.h"
#include "hlightX.h"
#include "statusX.h"
#include "tgoodsX.h"
#include "dstatusX.h"
#include "caravanX.h"
#include "displayG.h"
#define USE_CODES
#define PRINT_CODES
#include "worldX.h"

/*
 * show_cursor - Position cursor at proper screen location for current sector
 *
 * Calculates and moves the display cursor to the correct screen position
 * based on the current sector coordinates (xcurs, ycurs), zoom level, and
 * display mode. Handles hexagonal and rectangular map positioning with
 * proper offset calculations for borders and focus positioning.
 *
 * The function performs complex coordinate transformations:
 * 1. Determines horizontal offset based on border presence and cursor position
 * 2. Calculates vertical offset with special handling for hexagonal odd-lift
 * 3. Applies zoom-level specific shift values for proper scaling
 * 4. Adjusts for focus position within detailed sector display
 * 5. Converts to final screen coordinates and positions cursor
 *
 * Parameters: None (uses global cursor and display state variables)
 *
 * Returns: void
 *
 * Side Effects:
 *   - Moves terminal cursor to calculated position using move() curses call
 *   - Position calculation depends on curmap_screen[zoom_level] settings
 *   - Uses display_mode.focus for fine positioning within sectors
 *
 * Notes:
 *   - Coordinate calculation differs between hex and rectangular modes
 *   - Odd-numbered x coordinates get special vertical lift in hex mode
 *   - Final y-coordinate is inverted for proper screen positioning
 */
void
show_cursor PARM_0(void)
{
  int xhold, yhold;

  /* determine original horizontal offset */
  if (curmap_screen[zoom_level].has_border) {
    xhold = 1;
  } else {
    xhold = 0;
  }

  /* determine horizontal position */
  xhold += xcurs * curmap_screen[zoom_level].xshift;

  /* determine original vertical offset */
  if ((curmap_screen[zoom_level].oddlift) &&
      (xcurs % 2 == 1)) {
    yhold = 1;
  } else {
    yhold = 0;
  }

  /* now find the vertical position */
  yhold += ycurs * curmap_screen[zoom_level].yshift;

  /* position the cursor in the focus position */
  if (curmap_screen[zoom_level].has_border) {
    xhold += (display_mode.focus % 2);
    yhold += (display_mode.focus / 2);
  }

  /* finally, position it */
  move(SCREEN_Y_SIZE - yhold - 1, xhold);
}

/*
 * max_xcurs - Calculate maximum horizontal cursor position for current display
 *
 * Determines the maximum valid xcurs value based on the current screen size
 * and display zoom level. This prevents the cursor from moving beyond the
 * visible screen area and ensures proper map boundaries.
 *
 * The calculation considers:
 * - Available horizontal screen space (SCREEN_X_SIZE)
 * - Current zoom level's display parameters (xsize, xshift)
 * - Map scaling and sector size requirements
 *
 * Formula: (available_space) / xshift
 * Where available_space = SCREEN_X_SIZE - (xsize - xshift)
 *
 * Parameters: None (uses global screen and zoom settings)
 *
 * Returns:
 *   Maximum valid xcurs value for current display configuration
 *
 * Notes:
 *   - Value changes when zoom level changes
 *   - Used by cursor movement and boundary checking functions
 *   - Critical for preventing off-screen cursor positioning
 */
int
max_xcurs PARM_0(void)
{
  int available = SCREEN_X_SIZE;

  /* this is determined as a function of the display zoom and mode */
  available -= (curmap_screen[zoom_level].xsize - 
		curmap_screen[zoom_level].xshift);

  /* now can calculate it easily */
  return(available / curmap_screen[zoom_level].xshift);
}

/*
 * max_ycurs - Calculate maximum vertical cursor position for given x-coordinate
 *
 * Determines the maximum valid ycurs value based on the current screen size,
 * display zoom level, and specific x-coordinate. In hexagonal mode, the
 * maximum y position can vary depending on the x-coordinate due to the
 * odd-lift positioning of alternating columns.
 *
 * The calculation considers:
 * - Available vertical screen space (SCREEN_Y_SIZE)
 * - Current zoom level's vertical shift parameters
 * - Hexagonal odd-lift adjustment for specific x coordinates
 * - Map offset positioning (xoffset)
 *
 * Parameters:
 *   xval - X coordinate to calculate maximum Y position for
 *
 * Returns:
 *   Maximum valid ycurs value for the given x-coordinate
 *
 * Notes:
 *   - In hexagonal mode, odd x-coordinates may have reduced max y due to lift
 *   - Result varies with zoom level and display mode
 *   - Critical for boundary checking in vertical cursor movement
 */
int
max_ycurs PARM_1(int, xval)
{
  int available = SCREEN_Y_SIZE;

  /* check for adjustments */
  if (curmap_screen[zoom_level].oddlift &&
      ((xval - xoffset) % 2 == 1)) {
    /* hopefully this will work in all cases */
    available--;
  }

  /* now can calculate it easily */
  return(available / curmap_screen[zoom_level].yshift);
}

/*
 * on_screen - Check if sector coordinates are visible on current screen
 *
 * Determines whether the specified world sector coordinates (x, y) are
 * currently visible within the display screen boundaries. Handles horizontal
 * wrap-around for the cylindrical world map and performs boundary checking
 * against current screen limits.
 *
 * The function:
 * 1. Adjusts for horizontal map wrap-around if x < xoffset
 * 2. Checks if adjusted x coordinate exceeds maximum horizontal cursor
 * 3. Verifies y coordinate is within vertical screen boundaries
 * 4. Uses max_ycurs(x) for x-dependent vertical boundary checking
 *
 * Parameters:
 *   x - World x-coordinate to check
 *   y - World y-coordinate to check
 *
 * Returns:
 *   TRUE if sector is visible on screen, FALSE otherwise
 *
 * Side Effects:
 *   - Modifies local x variable for wrap-around calculation (not parameter)
 *
 * Notes:
 *   - Handles cylindrical world wrap-around in x-direction only
 *   - Y boundaries are absolute (no wrap-around)
 *   - Critical for determining which sectors need rendering
 */
int
on_screen PARM_2(int, x, int, y)
{
  int hold = TRUE;

  /* realign horizontal wrap */
  if (x < xoffset) {
    x += MAPX;
  }

  /* check it */
  if ((x - xoffset > max_xcurs()) ||
      (y < yoffset) ||
      (y - yoffset > max_ycurs(x))) {
    hold = FALSE;
  }
  return(hold);
}

/*
 * centermap - Recenter map display with current sector at screen center
 *
 * Repositions the map display so that the current sector (XREAL, YREAL) appears
 * at the center of the screen. Performs complex coordinate calculations to handle
 * both hexagonal and rectangular map modes, with special consideration for map
 * boundaries, relative coordinate systems, and hexagonal odd-lift alignment.
 *
 * The centering process:
 * 1. Calculates optimal xcurs position (max_xcurs() / 2)
 * 2. Sets xoffset to position current sector at calculated xcurs
 * 3. Adjusts for hexagonal odd-lift alignment constraints
 * 4. Handles horizontal wrap-around at map edges
 * 5. Calculates optimal ycurs position with boundary checking
 * 6. Handles relative vs. absolute coordinate systems
 * 7. Prevents centering beyond map boundaries
 *
 * Parameters: None (uses global position variables XREAL, YREAL)
 *
 * Returns: void
 *
 * Side Effects:
 *   - Modifies global variables: xcurs, ycurs, xoffset, yoffset
 *   - Changes current screen viewport position
 *   - Affects all subsequent map display operations
 *
 * Notes:
 *   - Behavior differs for gods vs. players (relative_map setting)
 *   - Hexagonal mode requires special odd-lift edge alignment
 *   - Respects map boundaries and visibility restrictions
 *   - Essential for smooth map navigation and viewport management
 */
void
centermap PARM_0(void)
{
  int temp, maxy;

  /* first align the X location */
  temp = XREAL;
  xcurs = max_xcurs() / 2;
  xoffset = temp - xcurs;

  /* assure that edge is set properly */
  if ((curmap_screen[zoom_level].oddlift) &&
      (((xoffset + MAPX) % 2) == 1)) {
    xoffset++;
    xcurs--;
  }
  if (xoffset < 0) {
    xoffset += MAPX;
  }

  /* now align the Y axis; adjust for boundaries */
  maxy = max_ycurs(xcurs);
  if (!world.relative_map || (is_god == TRUE) ||
      (ntn_ptr == NULL) || WIZ_MAGIC(MW_KNOWALL)) {
    temp = YREAL;
    if (MAPY <= maxy) {
      /* make sure that the bottom of screen is always set */
      yoffset = 0;
      ycurs = temp;
    } else {
      /* recenter and adjust */
      ycurs = (maxy - 1) / 2;
      yoffset = temp - ycurs;
      if (yoffset < 0) {
	ycurs = YREAL;
	yoffset = 0;
      } else if (yoffset > MAPY - maxy - 1) {
	yoffset = MAPY - maxy;
	ycurs = temp - yoffset;
      }
    }
  } else {
    /* just recenter */
    temp = YREAL;
    ycurs = (maxy - 1) / 2;
    yoffset = temp - ycurs;
  }
}

/*
 * style_voidable - Check if display style is blocked by void magic
 *
 * Determines whether the specified display style can show information in
 * sectors that are protected by "The Void" magic spell. Certain display
 * modes (like defense, resources, people, food) are blocked by void magic,
 * while others (like terrain, elevation) can still be seen.
 *
 * Voidable display styles include:
 * - DI_DEFENSE: defensive strength information
 * - DI_JEWEL, DI_METAL, DI_MAGIC: resource value displays
 * - DI_PEOP: population information
 * - DI_FOOD: food production information
 * - DI_DESG, DI_TGDESGS: designation information
 *
 * Parameters:
 *   style - Display style constant to check (DI_* values)
 *
 * Returns:
 *   TRUE if style is blocked by void magic, FALSE if always visible
 *
 * Notes:
 *   - Used in conjunction with magic checking functions
 *   - Essential for implementing strategic void magic concealment
 *   - Terrain and basic elevation info cannot be voided
 *   - Part of the fog-of-war and strategic concealment system
 */
static int
style_voidable PARM_1(int, style)
{
  int outval = FALSE;

  /* simple switch */
  switch (style) {
  case DI_DEFENSE:
  case DI_JEWEL:
  case DI_METAL:
  case DI_PEOP:
  case DI_FOOD:
  case DI_MAGIC:
  case DI_DESG:
  case DI_TGDESGS:
    /* can't see it ... oh well */
    outval = TRUE;
    break;
  default:
    /* yep, you can see it */
    break;
  }

  /* ske'daddle */
  return(outval);
}

/*
 * need_wedge - Check if water sector needs bottom wedge display character
 *
 * Determines whether a water sector should display a wedge character ('_')
 * at its bottom edge to indicate land immediately below. This creates a
 * visual coastline effect where water meets land, improving map readability
 * by clearly showing the boundary between water and land sectors.
 *
 * The function checks:
 * 1. If sector below (x, y-1) is on the map
 * 2. If the sector below is visible on screen
 * 3. If the sector below is not water (altitude != ELE_WATER)
 *
 * Parameters:
 *   x - X coordinate of water sector to check
 *   y - Y coordinate of water sector to check
 *
 * Returns:
 *   TRUE if land exists below this water sector, FALSE otherwise
 *
 * Notes:
 *   - Only used for water sectors in detailed display modes
 *   - Creates attractive coastline visual effects
 *   - Works with conq_bottomlines and conq_waterbottoms display options
 *   - Part of the aesthetic map rendering system
 */
static int
need_wedge PARM_2(int, x, int, y)
{
  int hold = FALSE;

  /* check the sector below this one */
  if (XY_ONMAP(x, y - 1) &&
      on_screen(x, y - 1) &&
      (sct[x][y - 1].altitude != ELE_WATER)) {
    /* do it */
    hold = TRUE;
  }

  /* beat it */
  return(hold);
}

/*
 * map_char - Generate display character for sector based on style and position
 *
 * This is the core map rendering function that determines what character to
 * display for a specific sector position based on the display style, sector
 * data, visibility, and magical effects. Handles all display modes from basic
 * terrain to complex resource and strategic information displays.
 *
 * The function implements extensive display logic:
 * 1. Validates sector coordinates and handles out-of-bounds cases
 * 2. Checks for void magic concealment by enemy nations
 * 3. Processes display style to determine information type to show
 * 4. Handles special cases for water sectors and unit displays
 * 5. Applies distortion effects for partially visible sectors
 * 6. Returns appropriate character based on calculated values
 *
 * Supported display styles include:
 * - Movement costs (DI_AMOVE, DI_FMOVE, DI_NMOVE): army, flying, navy movement
 * - Resources (DI_FOOD, DI_JEWEL, DI_METAL, DI_MAGIC): production values
 * - Terrain (DI_VEGE, DI_CONT): vegetation and elevation information
 * - Political (DI_NATN, DI_RACE): ownership and racial information
 * - Strategic (DI_DESG, DI_YDESG, DI_DEFENSE): designations and defense
 * - Economic (DI_PEOP, DI_VALUES, DI_WOOD, DI_WEIGHTS): population and trade
 *
 * Position parameter affects detailed sector display:
 * - HXPOS_LOWLEFT, HXPOS_LOWRIGHT: bottom row of detailed display
 * - HXPOS_UPLEFT, HXPOS_UPRIGHT: top row of detailed display
 * - HXPOS_MINIMAL: single character display for zoomed-out view
 *
 * Parameters:
 *   x - World X coordinate of sector
 *   y - World Y coordinate of sector  
 *   position - Display position within sector (HXPOS_* constants)
 *   style - Display style mode (DI_* constants)
 *
 * Returns:
 *   Display character for the specified sector and style
 *
 * Side Effects:
 *   - Sets global sct_ptr to current sector for optimization
 *   - Sets global ntn_tptr to sector owner's nation data
 *
 * Notes:
 *   - Most complex function in map display system
 *   - Handles fog-of-war, magic concealment, and vision distortion
 *   - Critical for all map visualization modes
 *   - Water sectors get special fleet and coastline display logic
 */
static char
map_char PARM_4(int, x, int, y, int, position, int, style)
{
  int dflt_char = FALSE, ch = ' ';
  int value, sown;

  /* get the sector information */
  if (!XY_ONMAP(x, y)) {
    return '#';
  }

  /* allow quick referencing */
  sct_ptr = &(sct[x][y]);
  sown = sct_ptr->owner;
  ntn_tptr = world.np[sown];
  if (ntn_tptr == NULL)
    sown = UNOWNED;

  /* check it out */
  if ((country != UNOWNED) &&
      (country != sown) &&
      (sown != UNOWNED) &&
      (MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_THEVOID) &&
       !WIZ_MAGIC(MW_SEEALL)) &&
      style_voidable(style)) {
    /* can't see nuthin' */
    ch = '?';
  } else {

    /* varies with display style */
    switch (style) {
    case DI_BLANK:
    case DI_KEEP:
      /* just empty */
      dflt_char = TRUE;
      break;
    case DI_AMOVE:
    case DI_FMOVE:
    case DI_NMOVE:
      /* different movement display modes */
      if (style == DI_AMOVE) {
	value = move_cost(x, y, MOVE_ARMY);
      } else if (style == DI_FMOVE) {
	value = move_cost(x, y, MOVE_FLYARMY);
      } else {
	value = move_cost(x, y, MOVE_NAVY);
      }
      
      /* compute results */
      switch (value) {
      case MV_UNMET:
      case MV_HOSTILE:
	/* cannot enter the sector */
	ch = 'X';
	break;
      case MV_LANDING:
	/* costs all movement to enter */
	ch = '+';
	break;
      case MV_WATER:
      case MV_GROUND:
      case MV_IMPASSABLE:
	/* can't be entered */
	dflt_char = TRUE;
	break;
      default:
	/* number please */
	if (value < 0) {
	  dflt_char = TRUE;
	} else {
	  if (value > 9) {
	    ch = '+';
	  } else {
	    ch = value + '0';
	  }
	}
	break;
      }
      break;
    case DI_FOOD:
      /* display the food value of a sector */
      value = distort_vision(x, y, tofood(sct_ptr, country),
			     sct_ptr->people);

      /* display based on values */
      if (value == 0) {
	dflt_char = TRUE;
      } else if (value < 10) {
	ch = value + '0';
      } else {
	ch = '+';
      }
      break;
    case DI_VEGE:
      /* display the vegetation value of the sector */
      if (sct_ptr->altitude == ELE_WATER) {
	dflt_char = TRUE;
      } else {
	ch = veg_info[sct_ptr->vegetation].symbol;
      }
      break;
    case DI_DESG:
      /* display the designation of the sector */
      value = major_desg(sct_ptr->designation);
      if ((value == MAJ_NONE) ||
	  ((value == MAJ_CACHE) &&
	   (country != sct_ptr->owner) &&
	   (country != UNOWNED))) {
	dflt_char = TRUE;
      } else {
	ch = maj_dinfo[value].symbol;
      }
      break;
    case DI_YDESG:
      /* display the designation of the sector if you own it */
      value = major_desg(sct_ptr->designation);
      if (sown != UNOWNED && sown != country) {
	ch = ntn_tptr->mark;
      } else {
	if (value == MAJ_NONE) {
	  dflt_char = TRUE;
	} else {
	  ch = maj_dinfo[value].symbol;
	}
      }
      break;
    case DI_CONT:
      /* show the elevation of the sector */
      ch = ele_info[sct_ptr->altitude].symbol;
      break;
    case DI_NATN:
      /* display the owner of the sector */
      if (sown == UNOWNED) {
	dflt_char = TRUE;
      } else {
	ch = ntn_tptr->mark;
      }
      break;
    case DI_RACE:
      /* display the race of the sector owner */
      if (sown == UNOWNED) {
	dflt_char = TRUE;
      } else {
	ch = race_info[ntn_tptr->race].name[0];
      }
      break;
    case DI_DEFENSE:
      /* compute defensive power of a sector based on terrain */
      if (sct_ptr->altitude == ELE_WATER ||
	  sct_ptr->altitude == ELE_PEAK) {
	dflt_char = TRUE;
      } else {
	value = defense_val(x, y);
	if (value >= 95) {
	  value = (value - 50) / 100;
	  ch = rnumerals[roman_base(value)];
	} else {
	  ch = (value + 5) / 10 + '0';
	}
      }
      break;
    case DI_PEOP:
      /* display the number of people in the sector */
      if (sct_ptr->owner == UNOWNED) {
	dflt_char = TRUE;
      } else {

	/* determine visibility of sector */
	value = distort_vision(x, y, sct_ptr->people,
			       sct_ptr->people);
	
	if (value < 950) {
	  ch = (50 + value) / 100 + '0';
	} else {
	  value = (value - 500) / 1000;
	  ch = rnumerals[roman_base(value)];
	}
      }
      break;
    case DI_JEWEL:
    case DI_METAL:
    case DI_MAGIC:
    case DI_VALUES:
      /* display the magical, jewel or metal value of a sector */
      value = tofood( sct_ptr, country );
      if (value == 0) {
	dflt_char = TRUE;
      } else if ((country == UNOWNED) || (sown == UNOWNED)
		 || (country == sown) || WIZ_MAGIC(MW_SEEALL)) {

	/* can we dig it? */
	if (tg_ok(sct_ptr, country)) {

	  /* get resulting value */
	  switch (style) {
	  case DI_JEWEL:
	    value = jewel_value(sct_ptr);
	    break;
	  case DI_METAL:
	    value = metal_value(sct_ptr);
	    break;
	  case DI_MAGIC:
	    value = magic_value(sct_ptr);
	    break;
	  case DI_VALUES:
	    /* get 'em all */
	    if (((value = jewel_value(sct_ptr)) == 0) &&
		((value = metal_value(sct_ptr)) == 0) &&
		((value = magic_value(sct_ptr)) == 0)) {
	      value = tg_info[sct_ptr->tradegood].value;
	    }
	    break;
	  default:
	    value = 0;
	    break;
	  }

	  /* distort */
	  value = distort_vision(x, y, value, sct_ptr->people);

	  /* show resulting value */
	  if (value == 0) {
	    dflt_char = TRUE;
	  } else if (value > 9) {
	    ch = '+';
	  } else ch = value + '0';

	} else {
	  dflt_char = TRUE;
	}

      } else {
	ch = '?';
      }
      break;
    case DI_TGDESGS:
      /* display designations needed for tradegoods */
      value = tofood(sct_ptr, country);
      if ((value == 0) ||
	  ((tg_info[sct_ptr->tradegood].need_desg == MAJ_FARM) &&
	   (value < DESFOOD))) {
	dflt_char = TRUE;
      } else if ((country == UNOWNED) || (sown == UNOWNED)
		 || (country == sown) || WIZ_MAGIC(MW_SEEALL)) {
	if ((sct_ptr->tradegood != TG_NONE) &&
	    (tg_info[sct_ptr->tradegood].need_desg != MAJ_NONE) &&
	    tg_ok(sct_ptr, country) ) {
	  ch = maj_dinfo[tg_info[sct_ptr->tradegood].need_desg].symbol;
	} else {
	  dflt_char = TRUE;
	}
      } else {
	ch = '?';
      }
      break;
    case DI_WOOD:
      /* display the wood value for a sector */
      value = tofood(sct_ptr, country);
      if (value == 0) {
	dflt_char = TRUE;
      } else {

	/* determine the value that is thought to be there */
	value = distort_vision(x, y, towood(sct_ptr, country),
			       sct_ptr->people);
	if (value == 0) {
	  dflt_char = TRUE;
	} else if (value > 9) {
	  ch = '+';
	} else ch = value + '0';
      }
      break;
    case DI_WEIGHTS:
      /* show distribution weighting */
      if (((country == UNOWNED) && (sct_ptr->owner != UNOWNED)) ||
	  ((country != UNOWNED) && (sct_ptr->owner == country))) {

	/* find the value */
	if ((city_ptr =
	     citybyloc(world.np[sct_ptr->owner], x, y)) == NULL) {
	  value = 0;
	} else {
	  value = CITY_WEIGHT;
	}

	/* display it */
	if (value == 0) {
	  ch = '0';
	} else {
	  if (value >= 90) {
	    ch = '+';
	  } else {
	    ch = value / 10 + '1';
	  }
	}
      } else {
	dflt_char = TRUE;
      }
      break;
    default:
      /* hmmm... something funny here */
      break;
    }

  }

  /* check it out */
  if (dflt_char == TRUE) {

    /* water is just, oh so special */
    if (sct_ptr->altitude == ELE_WATER) {

      /* show either fleets in the water or the water itself */
      if (position == HXPOS_LOWLEFT) {
	/* nation mark if unique nation in place */
	if (TROOPLOCS(x, y) != 0) {
	  if (TROOPLOCS(x, y) == ABSMAXNTN) {
	    ch = '+';
	  } else {
	    ch = world.np[(int) TROOPLOCS(x, y)]->mark;
	  }
	} else if (!conq_allblanks &&
		   !conq_waterbottoms &&
		   (conq_bottomlines ||
		    need_wedge(x, y))) {
	  ch = '_';
	} else {
	  ch = ele_info[ELE_WATER].symbol;
	}
      } else {
	if ((position == HXPOS_LOWRIGHT) &&
	    !conq_allblanks &&
	    !conq_waterbottoms &&
	    (conq_bottomlines ||
	     need_wedge(x, y))) {
	  ch = '_';
	} else {
	  ch = ele_info[ELE_WATER].symbol;
	}
      }

    } else {

      /* depends on what position it is in */
      switch (position) {
      case HXPOS_LOWLEFT:
      case HXPOS_LOWRIGHT:
	/* underline sector, to look nice! */
	if (conq_allblanks) {
	  ch = ' ';
	} else {
	  ch = '_';
	}
	break;
      case HXPOS_UPLEFT:
      case HXPOS_UPRIGHT:
	/* just the blanks m'am */
	ch = ' ';
	break;
      default:
	/* show the elevation or designation of the sector */
	if (tofood( sct_ptr, country ) < DESFOOD) {
	  ch = veg_info[sct_ptr->vegetation].symbol;
	} else {
	  ch = ele_info[sct_ptr->altitude].symbol;
	}
	break;
      }

    }
  }

  /* all done... bye bye */
  return (ch);
}

/*
 * map_highlight - Determine if sector should be highlighted based on criteria
 *
 * This function implements the complex highlighting system that allows players
 * to visually emphasize specific types of sectors on the map. It evaluates
 * various highlighting criteria including unit status, diplomatic relations,
 * resource availability, and strategic importance.
 *
 * The function uses static caching to optimize performance by avoiding
 * redundant calculations for the same sector and style combination. It
 * performs comprehensive checking of sector ownership, magical concealment,
 * and visibility before applying highlighting logic.
 *
 * Supported highlighting modes:
 * - HI_MOVEABLE: units with full movement points remaining
 * - HI_SCOUT: sectors containing scouting units
 * - HI_YUNITS: player-owned units (armies, navies, caravans)
 * - HI_UNITS: all units visible on the map
 * - HI_TGOODS: sectors with trade goods (with class filtering)
 * - HI_OWN: sectors owned by specific nation or any nation
 * - HI_ALLIED: sectors owned by diplomatically allied nations
 * - HI_ENEMY: sectors owned by hostile or evil nations
 * - HI_NEUTRAL: sectors owned by neutral/unmet nations
 * - HI_MINDESG: sectors with specific minor designations
 * - HI_MAJDESG: sectors with specific major designations
 * - HI_RANGE: sectors within city influence range
 * - HI_REGION: sectors in same region as cursor
 * - HI_SUPPORTED: sectors with supply line support
 * - HI_UNSUPPORTED: sectors lacking supply line support
 *
 * Parameters:
 *   x - World X coordinate to check
 *   y - World Y coordinate to check
 *   style - Highlighting style (HI_* constants)
 *   opval - Optional parameter for style-specific filtering
 *
 * Returns:
 *   TRUE if sector should be highlighted, FALSE otherwise
 *
 * Side Effects:
 *   - Uses static variables for caching previous results
 *   - Sets global sct_ptr and ntn_tptr for sector access
 *   - May call complex diplomatic and magical checking functions
 *
 * Notes:
 *   - Caches results using static variables for performance optimization
 *   - Handles magical concealment (illusion, void magic)
 *   - Critical for strategic map analysis and unit management
 *   - opval meaning varies by highlighting style (trade good class, nation ID, etc.)
 */
static int
map_highlight PARM_4(int, x, int, y, int, style, int, opval)
{
  CITY_PTR c1_ptr = NULL;
  SCT_PTR s1_ptr = NULL;
  static int hold = FALSE, ox = -1, oy = -1, ostyle = -1;
  int range, value, sown;

  /* quick return if out of bounds */
  if (x < 0 || y < 0 || x >= MAPX || y >= MAPY) {
    return (hold = FALSE);
  }
  if ((style == HI_KEEP) ||
      (style == HI_NONE)) {
    return (hold = FALSE);
  }

  /* now check for repetition */
  if ((x == ox) &&
      (y == oy) &&
      (style == ostyle)) {
    /* same as the last request */
    return(hold);
  }
  hold = FALSE;
  ox = x;
  oy = y;
  ostyle = style;

  /* allow quick referencing */
  sct_ptr = &(sct[x][y]);
  sown = sct_ptr->owner;
  ntn_tptr = world.np[sown];
  if (ntn_tptr == NULL)
    sown = UNOWNED;

  /* return result based on the desired mode */
  switch (style) {
  case HI_MOVEABLE:
    /* highlight units with original movement */
    if (has_unmoved(x, y)) {
      hold = TRUE;
    }
    break;
#ifdef UNIMP
  case HI_STARTMOVE:
    /* highlight units with movement remaining */
    if (has_movable(x, y)) {
      hold = TRUE;
    }
    break;
#endif /* UNIMP */
  case HI_SCOUT:
    /* highlight any scouting units */
    if (has_scouts(x, y)) {
      hold = TRUE;
    }
    break;
  case HI_YUNITS:
    /* highlight any armies, etc. owned by the current nation */
    if (has_troops(x, y)) {
      hold = TRUE;
    }
    break;
  case HI_UNITS:
    /* highlight any armies, etc. on the map */
    if (TROOPLOCS(x, y)) {
      hold = TRUE;
    }
    break;
  case HI_TGOODS:
    /* highlight sectors with trade goods */
    if ((sown != UNOWNED) &&
	(country != UNOWNED) &&
	(sown != country) &&
	MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_ILLUSION) &&
	!WIZ_MAGIC(MW_VISION)) {
      /* give no report for illusionary nations */
      break;
    }
    if ( (tg_ok( sct_ptr, country )
	  && sct_ptr->tradegood != TG_NONE)
	&& (((sown == UNOWNED) ||
	     !MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_THEVOID)) ||
	    (country == UNOWNED) ||
	    (country == sown) ||
	    WIZ_MAGIC(MW_SEEALL)) ) {
      if ((opval == TG_NONE) ||
	  (opval == (int)tg_info[sct_ptr->tradegood].class)) {
	hold = TRUE;
      }
    }
    break;
  case HI_OWN:
    /* highlight owned sectors */
    if (opval == UNOWNED) {
      if (sown != UNOWNED) hold = TRUE;
    } else {
      if (sown == opval) hold = TRUE;
    }
    break;
  case HI_ALLIED:
    if (country == UNOWNED) {

      /* highlight good nations */
      if (sown == UNOWNED) break;
      if (n_isgood(ntn_tptr->active)) {
	hold = TRUE;
      }

    } else {

      /* highlight sectors owned by friendly nations */
      if ((sown == UNOWNED) ||
	  (sown == country)) break;
      value = ntn_tptr->dstatus[country] % dstatus_number;
      if (value != DIP_UNMET && value < DIP_NEUTRAL) {
	hold = TRUE;
      }

    }
    break;
  case HI_ENEMY:
    if (country == UNOWNED) {

      /* highlight evil nations */
      if (sown == UNOWNED) break;
      if (n_isevil(ntn_tptr->active) || n_ismonster(ntn_tptr->active)) {
	hold = TRUE;
      }

    } else {

      /* highlight sectors owned by unfriendly nations */
      if ((sown == UNOWNED) ||
	  (sown == country)) break;
      value = ntn_tptr->dstatus[country] % dstatus_number;
      if (value > DIP_NEUTRAL) {
	hold = TRUE;
      }

    }
    break;
  case HI_NEUTRAL:
    if (country == UNOWNED) {

      /* highlight neutral nations */
      if (sown == UNOWNED) break;
      if (n_isneutral(ntn_tptr->active)) {
	hold = TRUE;
      }

    } else {

      /* highlight sectors owned by neutral nations */
      if ((sown == UNOWNED) ||
	  (sown == country)) break;
      value = ntn_tptr->dstatus[country];
      if (value == DIP_UNMET || value == DIP_NEUTRAL) {
	hold = TRUE;
      }

    }
    break;
  case HI_MINDESG:
    /* check input */
    if ((opval < 0) ||
	(opval >= MIN_NUMBER)) break;

    /* highlight a sector containing a given minor designation */
    if (((MIN_START << opval) != MIN_DEVASTATED) &&
	((MIN_START << opval) != MIN_TRADINGPOST)) {
      if ((sown != UNOWNED) &&
	  (country != UNOWNED) &&
	  (sown != country) &&
	  (ntn_tptr->dstatus[country] % dstatus_number > DIP_FRIENDLY) &&
	  MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_ILLUSION) &&
	  !WIZ_MAGIC(MW_VISION)) {
	/* give no report for illusionary nations */
	break;
      }
    }

    /* check the value */
    if (minor_desg(sct_ptr->designation, MIN_START << opval)) {
      hold = TRUE;
    }
    break;
  case HI_MAJDESG:
    /* check input */
    if ((opval < 0) ||
	(opval >= MAJ_NUMBER)) break;

    /* highlight a sector containing a given minor designation */
    if (opval == MAJ_CACHE) {
      if ((sown != UNOWNED) &&
	  (country != UNOWNED) &&
	  (sown != country)) {
	break;
      }
    }

    /* check the value */
    if ((opval == MAJ_FARM) &&
	IS_FARM(sct_ptr->designation)) {
      hold = TRUE;
    } else if (major_desg(sct_ptr->designation) == opval) {
      hold = TRUE;
    }
    break;
  case HI_REGION:
    /* check if they do not belong */
    if (sown != sct[XREAL][YREAL].owner) break;
    /* fall through - continue to range check */
  case HI_RANGE:
    /* more checking */
    if (country == UNOWNED) {
      /* first check if the cursor is on a city */
      s1_ptr = &(sct[XREAL][YREAL]);
      ntn_tptr = world.np[s1_ptr->owner];
      if ((c1_ptr = citybyloc(ntn_tptr, XREAL, YREAL)) == NULL) {
	/* then check if the current sector is within range of any city */
	s1_ptr = &(sct[x][y]);
	ntn_tptr = world.np[s1_ptr->owner];
	c1_ptr = citybyloc(ntn_tptr, x, y);
      }
    } else {
      /* first check if the cursor is on a city */
      s1_ptr = &(sct[XREAL][YREAL]);
      ntn_tptr = ntn_ptr;
      if ((c1_ptr = citybyloc(ntn_tptr, XREAL, YREAL)) == NULL) {
	/* then check if the cursor is within range of any city */
	s1_ptr = &(sct[x][y]);
	c1_ptr = citybyloc(ntn_tptr, x, y);
      }
    }
    if (c1_ptr != NULL) {
      range = r10_region(ntn_tptr, c1_ptr, s1_ptr) / 10;
      hold = map_within(XREAL, YREAL, x, y, range);
    }
    break;
  case HI_UNSUPPORTED:
    if ((((country == UNOWNED) && (sct_ptr->owner != UNOWNED)) ||
	 ((country != UNOWNED) && (sct_ptr->owner == country))) &&
	(SUM_WEIGHTS(x, y) == 0)) hold = TRUE;
    break;
  case HI_SUPPORTED:
    if ((((country == UNOWNED) && (sct_ptr->owner != UNOWNED)) ||
	 ((country != UNOWNED) && (sct_ptr->owner == country))) &&
	(SUM_WEIGHTS(x, y) != 0)) hold = TRUE;
    break;
  default:
    bottommsg("Unknown highlighting selection!!!!!");
    break;
  }

  /* give back results */
  return(hold);
}

/*
 * hex_init - Initialize hexagonal map display system and base display modes
 *
 * Performs complete initialization of the map display system, setting up
 * display modes, screen parameters, and map positioning. This function is
 * called once during game startup to prepare the map visualization system
 * for operation.
 *
 * Initialization process:
 * 1. Selects appropriate screen parameters (hexmap_screen vs rectmap_screen)
 * 2. Centers the map at the current position using centermap()
 * 3. Creates and configures all base display modes (DMODE_NUMBER modes)
 * 4. Sets up focus, style, highlighting, and target parameters for each mode
 * 5. Performs memory-dependent calculations via hex_recalc()
 * 6. Initializes supply line weight calculations
 *
 * The function configures display modes from the base_modes array, setting:
 * - Display focus position for detailed sector view
 * - Style arrays for different hex positions (HXPOS_NUMBER positions)
 * - Highlighting arrays for visual emphasis
 * - Target arrays for highlighting parameter values
 *
 * Parameters: None (uses global world and display settings)
 *
 * Returns: void
 *
 * Side Effects:
 *   - Sets global curmap_screen to appropriate screen parameter array
 *   - Creates all display modes in the global display mode list
 *   - Initializes map position via centermap()
 *   - Allocates and calculates map data structures
 *   - Modifies global display and positioning variables
 *
 * Notes:
 *   - Must be called before any map display operations
 *   - Switches between hex and rectangular modes based on world.hexmap
 *   - Essential for proper map visualization system initialization
 */
void
hex_init PARM_0(void)
{
  int count, count2;

  /* prep the map mode */
  if (world.hexmap) {
    curmap_screen = hexmap_screen;
  } else {
    curmap_screen = rectmap_screen;
  }
  centermap();

  /* now build up the list of displays */
  for (count = 0; count < DMODE_NUMBER; count++) {
    /* add in each one */
    if ((dmode_tptr = crt_dmode(base_modes[count].name)) != NULL) {
      dmode_tptr->d.focus = base_modes[count].focus;
      for (count2 = 0; count2 < HXPOS_NUMBER; count2++) {
	dmode_tptr->d.style[count2] = base_modes[count].style[count2];
	dmode_tptr->d.highlight[count2] = base_modes[count].highlight[count2];
	dmode_tptr->d.target[count2] = base_modes[count].target[count2];
      }
    }
  }

  /* do memory dependent calculations */
  hex_recalc();
  set_weights(FALSE);
}

/*
 * hex_recalc - Recalculate map data structures and unit visibility arrays
 *
 * Performs comprehensive recalculation of map data structures, rebuilding
 * the unit location tracking arrays and movement status indicators. This
 * function is called whenever the map needs to be updated due to unit
 * movement, turn progression, or visibility changes.
 *
 * Recalculation process:
 * 1. Resets and reallocates sectstat and trooplocs arrays
 * 2. Iterates through all active nations and their units
 * 3. Updates unit visibility based on magical concealment
 * 4. Records unit positions in trooplocs array for display
 * 5. Sets movement status flags (movable, unmoved, scouts)
 * 6. Handles troop ownership and monster unit special cases
 * 7. Recalculates visibility maps via whatcansee()
 *
 * The function processes three unit types:
 * - Armies: land-based military units with scout detection
 * - Navies: water-based units with movement tracking
 * - Caravans: supply and trade units with basic movement
 *
 * Magical concealment handling:
 * - Void magic can hide entire nations from enemy view
 * - Illusion magic creates uncertainty in unit positions
 * - Vision magic can penetrate illusion concealment
 * - Random factors affect illusion penetration
 *
 * Parameters: None (operates on global game state)
 *
 * Returns: void
 *
 * Side Effects:
 *   - Reallocates sectstat and trooplocs memory arrays
 *   - Updates global unit position and status tracking
 *   - Modifies visibility calculations via whatcansee()
 *   - Preserves army_ptr, navy_ptr, cvn_ptr values
 *
 * Notes:
 *   - Essential for accurate map display after any game state change
 *   - Performance-critical function called frequently during gameplay
 *   - Handles complex magical concealment and visibility rules
 *   - Must maintain consistency between unit lists and display arrays
 */
void
hex_recalc PARM_0(void)
{
  ARMY_PTR ahold_ptr = army_ptr;
  NAVY_PTR nhold_ptr = navy_ptr;
  CVN_PTR chold_ptr = cvn_ptr;
  int count;

  /* clean up the memory */
  sectstat = new_mapchar(sectstat);
  trooplocs = new_mapchar(trooplocs);

  /* go through appropriate nations */
  for (count = 1; count < MAXNTN; count++) {

    /* check if country should be skipped */
    if ((ntn_tptr = world.np[count]) == NULL) continue;
    if ((country != UNOWNED) &&
	(country != count) &&
	MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_THEVOID) &&
	!WIZ_MAGIC(MW_SEEALL)) {
      continue;
    }

    /* check armies in sector */
    for (army_tptr = ntn_tptr->army_list;
	 army_tptr != NULL;
	 army_tptr = army_tptr->next) {

      /* check all armies */
      if (ARMYT_SIZE > 0) {
	if ((country == UNOWNED) ||
	    (country == count) ||
	    !MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_ILLUSION) ||
	    WIZ_MAGIC(MW_VISION) ||
	    rand_val(2)) {

	  /* is owned by this nation? or is god watching? */
	  if ((country == count) ||
	      (country == UNOWNED)) {

	    /* possessive of troops? */
	    if ((count == country) ||
		n_ismonster(ntn_tptr->active)) {
	      set_has_troops(ARMYT_XLOC, ARMYT_YLOC);
	    }

	    /* scout? */
	    if (a_isscout(ARMYT_TYPE)) {
	      set_has_scouts(ARMYT_XLOC, ARMYT_YLOC);
	    }

	    /* check for movement */
	    if (!nomove_stat(ARMYT_MOVE) &&
		(ARMYT_MOVE != 0)) {
	      set_has_movable(ARMYT_XLOC, ARMYT_YLOC);
	      if (ARMYT_MOVE == 100) {
		set_has_unmoved(ARMYT_XLOC, ARMYT_YLOC);
	      }
	    } else {
	      /* wall patrol? */
	      if (wall_patrol(count, army_tptr) == FALSE) {
		set_has_movable(ARMYT_XLOC, ARMYT_YLOC);
		set_has_unmoved(ARMYT_XLOC, ARMYT_YLOC);
	      }
	    }

	  }

	  /* now add it to the list of troops in the sector */
	  if ((TROOPLOCS(ARMYT_XLOC, ARMYT_YLOC) != count) &&
	      (TROOPLOCS(ARMYT_XLOC, ARMYT_YLOC) != UNOWNED)) {
	    TROOPLOCS(ARMYT_XLOC, ARMYT_YLOC) = ABSMAXNTN;
	  } else {
	    TROOPLOCS(ARMYT_XLOC, ARMYT_YLOC) = count;
	  }

	}
      }

    }

    /* go through all navies in the nation */
    for (navy_tptr = ntn_tptr->navy_list;
	 navy_tptr != NULL;
	 navy_tptr = navy_tptr->next) {

      /* check if a navy is in the given sector */
      if ((country == UNOWNED) ||
	  (country == count) ||
	  rand_val(2) ||
	  !MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_ILLUSION) ||
	    WIZ_MAGIC(MW_VISION)) {

	if ((country == count) ||
	    (country == UNOWNED)) {

	  /* these be owned by them? */
	  if ((count == country) ||
	      n_ismonster(ntn_tptr->active)) {
	    set_has_troops(NAVYT_XLOC, NAVYT_YLOC);
	  }

	  /* check movement */
	  if (NAVYT_MOVE != 0) {
	    set_has_movable(NAVYT_XLOC, NAVYT_YLOC);
	    if (NAVYT_MOVE == navy_mvpts(ntn_tptr, navy_tptr)) {
	      set_has_unmoved(NAVYT_XLOC, NAVYT_YLOC);
	    }
	  }
	}

	/* now add it to the list of troops in the sector */
	if ((TROOPLOCS(NAVYT_XLOC, NAVYT_YLOC) != count) &&
	    (TROOPLOCS(NAVYT_XLOC, NAVYT_YLOC) != UNOWNED)) {
	  TROOPLOCS(NAVYT_XLOC, NAVYT_YLOC) = ABSMAXNTN;
	} else {
	  TROOPLOCS(NAVYT_XLOC, NAVYT_YLOC) = count;
	}
      }
    }

    /* go through all caravans in the nation */
    for (cvn_tptr = ntn_tptr->cvn_list;
	 cvn_tptr != NULL;
	 cvn_tptr = cvn_tptr->next) {

      /* check if a caravan is in the given sector */
      if (CVNT_SIZE != 0) {
	if ((country == UNOWNED) ||
	    (country == count) ||
	    rand_val(2) ||
	    !MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_ILLUSION) ||
	    WIZ_MAGIC(MW_VISION)) {

	  if ((country == count) ||
	      (country == UNOWNED)) {

	    /* these be owned by them? */
	    if ((count == country) ||
		n_ismonster(ntn_tptr->active)) {
	      set_has_troops(CVNT_XLOC, CVNT_YLOC);
	    }

	    /* check movement */
	    if (CVNT_MOVE != 0) {
	      set_has_movable(CVNT_XLOC, CVNT_YLOC);
	      if (CVNT_MOVE == cvn_mvpts(ntn_tptr, cvn_tptr)) {
		set_has_unmoved(CVNT_XLOC, CVNT_YLOC);
	      }
	    }
	  }

	  /* now add it to the list of troops in the sector */
	  if ((TROOPLOCS(CVNT_XLOC, CVNT_YLOC) != count) &&
	      (TROOPLOCS(CVNT_XLOC, CVNT_YLOC) != UNOWNED)) {
	    TROOPLOCS(CVNT_XLOC, CVNT_YLOC) = ABSMAXNTN;
	  } else {
	    TROOPLOCS(CVNT_XLOC, CVNT_YLOC) = count;
	  }

	}
      }

    }
  }

  /* now take care of the other things */
  whatcansee();
  army_ptr = ahold_ptr;
  navy_ptr = nhold_ptr;
  cvn_ptr = chold_ptr;
}

/*
 * go_north - Move cursor one sector north
 *
 * Moves the map cursor one sector northward (increasing Y coordinate).
 * This function implements basic single-step cursor movement for navigation.
 * Automatically resets selection state (pager and selector) when moving.
 *
 * Parameters: None (modifies global cursor position)
 *
 * Returns: 0 (success)
 *
 * Side Effects:
 *   - Increments ycurs by 1
 *   - Resets pager and selector to 0 (clears unit selection)
 *
 * Notes:
 *   - Movement is identical for both hex and rectangular modes
 *   - Part of 8-directional movement system
 *   - Boundary checking handled by calling code
 */
int
go_north PARM_0(void)
{
  pager = 0;
  selector = 0;
  ycurs++;
  return(0);
}

/*
 * go_ne - Move cursor one sector northeast
 *
 * Moves the map cursor one sector in the northeast direction. The movement
 * algorithm differs significantly between hexagonal and rectangular map modes
 * due to the different coordinate systems and adjacency patterns.
 *
 * Movement patterns:
 * - Hexagonal mode: xcurs++, ycurs += (XREAL % 2)
 *   Northeast movement depends on current column parity (even/odd)
 *   Even columns: move right only, odd columns: move right and up
 * - Rectangular mode: xcurs++, ycurs++ (simple diagonal movement)
 *
 * Parameters: None (modifies global cursor position)
 *
 * Returns: 0 (success)
 *
 * Side Effects:
 *   - Modifies xcurs and ycurs based on map mode
 *   - Resets pager and selector to 0 (clears unit selection)
 *
 * Notes:
 *   - Demonstrates hexagonal coordinate system complexity
 *   - XREAL % 2 determines column parity for hex movement
 *   - Essential for proper 6-directional hex navigation
 *   - Part of 8-directional movement system (8 dirs for rect, 6 for hex)
 */
int
go_ne PARM_0(void)
{
  pager = 0;
  selector = 0;
  if (world.hexmap) {
    ycurs += (XREAL % 2);
    xcurs++;
  } else {
    xcurs++;
    ycurs++;
  }
  return(0);
}

/*
 * go_nw - Move cursor one sector northwest
 *
 * Advances the cursor position one sector in the northwest direction.
 * In hexagonal mode, northwest movement depends on column parity due to
 * the offset layout of hex grids. Movement involves adjusting both xcurs
 * and ycurs coordinates according to hex geometry rules.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Updates xcurs and ycurs cursor position
 *   - Movement pattern differs between hex and rectangular modes
 *
 * Notes:
 *   - Hex mode: ycurs += (XREAL % 2), xcurs-- (column parity adjustment)
 *   - Rectangular mode: xcurs--, ycurs++ (simple diagonal)
 *   - Uses XREAL for world coordinate to determine hex column parity
 */
int
go_nw PARM_0(void)
{
  pager = 0;
  selector = 0;
  if (world.hexmap) {
    ycurs += (XREAL % 2);
    xcurs--;
  } else {
    xcurs--;
    ycurs++;
  }
  return(0);
}

/*
 * go_west - Move cursor one sector west
 *
 * Advances the cursor position one sector directly west. In hexagonal
 * mode, west movement requires moving 2 cursor positions due to the
 * compressed hex display where each hex sector spans 2 character columns.
 * This maintains proper hex geometry alignment.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Updates xcurs cursor position (west)
 *   - No ycurs change (pure horizontal movement)
 *
 * Notes:
 *   - Hex mode: xcurs -= 2 (hex sectors span 2 char columns)
 *   - Rectangular mode: xcurs-- (single character movement)
 *   - Simplest movement function with no vertical component
 */
int
go_west PARM_0(void)
{
  pager = 0;
  selector = 0;
  if (world.hexmap) {
    xcurs -= 2;
  } else {
    xcurs--;
  }
  return(0);
}

/*
 * go_east - Move cursor one sector east
 *
 * Advances the cursor position one sector directly east. Mirror function
 * of go_west(), following the same hexagonal geometry rules. In hex mode,
 * east movement requires advancing 2 cursor positions to maintain proper
 * alignment with the compressed hex display format.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Updates xcurs cursor position (east)
 *   - No ycurs change (pure horizontal movement)
 *
 * Notes:
 *   - Hex mode: xcurs += 2 (hex sectors span 2 char columns)
 *   - Rectangular mode: xcurs++ (single character movement)
 *   - Complement to go_west() with identical logic but opposite direction
 */
int
go_east PARM_0(void)
{
  pager = 0;
  selector = 0;
  if (world.hexmap) {
    xcurs += 2;
  } else {
    xcurs++;
  }
  return(0);
}

/*
 * go_south - Move cursor one sector south
 *
 * Advances the cursor position one sector directly south. This is the
 * simplest movement function as south movement works identically in both
 * hexagonal and rectangular display modes. Only affects the ycurs
 * coordinate with no horizontal component.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Decrements ycurs cursor position (screen coordinates)
 *   - No xcurs change (pure vertical movement)
 *
 * Notes:
 *   - Identical behavior in both hex and rectangular modes
 *   - ycurs-- (screen y coordinates increase upward)
 *   - Complement to go_north() which increments ycurs
 */
int
go_south PARM_0(void)
{
  pager = 0;
  selector = 0;
  ycurs--;
  return(0);
}

/*
 * go_se - Move cursor one sector southeast
 *
 * Advances the cursor position one sector in the southeast direction.
 * In hexagonal mode, uses the opposite parity calculation from go_ne()
 * by using (XREAL + 1) % 2 instead of XREAL % 2. This ensures proper
 * hex geometry when moving from northeast to southeast directions.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Updates both xcurs and ycurs cursor positions
 *   - Movement pattern differs between hex and rectangular modes
 *
 * Notes:
 *   - Hex mode: ycurs -= (XREAL + 1) % 2, xcurs++ (inverted parity)
 *   - Rectangular mode: ycurs--, xcurs++ (simple diagonal)
 *   - Uses inverted column parity compared to go_ne()
 */
int
go_se PARM_0(void)
{
  pager = 0;
  selector = 0;
  if (world.hexmap) {
    ycurs -= (XREAL + 1) % 2;
    xcurs++;
  } else {
    ycurs--;
    xcurs++;
  }
  return(0);
}

/*
 * go_sw - Move cursor one sector southwest
 *
 * Advances the cursor position one sector in the southwest direction.
 * Uses the same parity calculation as go_se() with (XREAL + 1) % 2
 * but moves west instead of east. This maintains proper hex geometry
 * for the southern diagonal directions.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Updates both xcurs and ycurs cursor positions
 *   - Movement pattern differs between hex and rectangular modes
 *
 * Notes:
 *   - Hex mode: ycurs -= (XREAL + 1) % 2, xcurs-- (inverted parity)
 *   - Rectangular mode: ycurs--, xcurs-- (simple diagonal)
 *   - Complements go_nw() for southwest movement pattern
 */
int
go_sw PARM_0(void)
{
  pager = 0;
  selector = 0;
  if (world.hexmap) {
    ycurs -= (XREAL + 1) % 2;
    xcurs--;
  } else {
    ycurs--;
    xcurs--;
  }
  return(0);
}

/*
 * scr_north - Move cursor north by half-screen distance
 *
 * Advances the cursor position northward by approximately half the visible
 * screen height. This provides efficient navigation for large maps by
 * allowing rapid movement across significant distances. Uses max_ycurs()
 * to calculate the appropriate screen-relative movement distance.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Advances ycurs by half the maximum screen height
 *   - Movement distance adapts to current screen dimensions
 *
 * Notes:
 *   - Uses max_ycurs(xcurs) / 2 for adaptive screen-relative movement
 *   - Movement distance depends on current cursor x position
 *   - Useful for rapid map navigation and exploration
 */
int
scr_north PARM_0(void)
{
  pager = 0;
  selector = 0;
  ycurs += max_ycurs(xcurs) / 2;
  return(0);
}

/*
 * scr_ne - Move cursor northeast by half-screen distance
 *
 * Advances the cursor position diagonally northeast by approximately
 * half the visible screen dimensions in both x and y directions. This
 * combines half-screen movement in both axes for efficient diagonal
 * navigation across large map areas.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Advances xcurs by half the maximum screen width
 *   - Advances ycurs by half the maximum screen height
 *   - Movement distance adapts to current screen dimensions
 *
 * Notes:
 *   - xcurs += max_xcurs() / 2, ycurs += max_ycurs(xcurs) / 2
 *   - Combines horizontal and vertical half-screen movements
 *   - Efficient for diagonal map exploration and navigation
 */
int
scr_ne PARM_0(void)
{
  pager = 0;
  selector = 0;
  xcurs += max_xcurs() / 2;
  ycurs += max_ycurs(xcurs) / 2;
  return(0);
}

/*
 * scr_nw - Move cursor northwest by half-screen distance
 *
 * Advances the cursor position diagonally northwest by approximately
 * half the visible screen dimensions. Moves west by half-screen width
 * while moving north by half-screen height, enabling efficient diagonal
 * navigation in the northwest direction.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Retreats xcurs by half the maximum screen width
 *   - Advances ycurs by half the maximum screen height
 *   - Movement distance adapts to current screen dimensions
 *
 * Notes:
 *   - xcurs -= max_xcurs() / 2, ycurs += max_ycurs(xcurs) / 2
 *   - Mirror of scr_ne() but moves west instead of east
 *   - Useful for rapid northwest map exploration
 */
int
scr_nw PARM_0(void)
{
  pager = 0;
  selector = 0;
  xcurs -= max_xcurs() / 2;
  ycurs += max_ycurs(xcurs) / 2;
  return(0);
}

/*
 * scr_west - Move cursor west by half-screen distance
 *
 * Advances the cursor position westward by approximately half the visible
 * screen width. This provides efficient horizontal navigation for large
 * maps by allowing rapid east-west movement across significant distances.
 * Pure horizontal movement with no vertical component.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Retreats xcurs by half the maximum screen width
 *   - No ycurs change (pure horizontal movement)
 *
 * Notes:
 *   - xcurs -= max_xcurs() / 2 (half-screen westward)
 *   - Complement to scr_east() with identical logic but opposite direction
 *   - Efficient for horizontal map scanning and navigation
 */
int
scr_west PARM_0(void)
{
  pager = 0;
  selector = 0;
  xcurs -= max_xcurs() / 2;
  return(0);
}

/*
 * scr_east - Move cursor east by half-screen distance
 *
 * Advances the cursor position eastward by approximately half the visible
 * screen width. Mirror function of scr_west(), providing efficient
 * horizontal navigation for rapid east-west movement across large map
 * areas. Pure horizontal movement with no vertical component.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Advances xcurs by half the maximum screen width
 *   - No ycurs change (pure horizontal movement)
 *
 * Notes:
 *   - xcurs += max_xcurs() / 2 (half-screen eastward)
 *   - Complement to scr_west() with identical logic but opposite direction
 *   - Efficient for horizontal map scanning and navigation
 */
int
scr_east PARM_0(void)
{
  pager = 0;
  selector = 0;
  xcurs += max_xcurs() / 2;
  return(0);
}

/*
 * scr_south - Move cursor south by half-screen distance
 *
 * Advances the cursor position southward by approximately half the visible
 * screen height. Mirror function of scr_north(), providing efficient
 * vertical navigation for rapid north-south movement across large map
 * areas. Pure vertical movement with no horizontal component.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Retreats ycurs by half the maximum screen height
 *   - No xcurs change (pure vertical movement)
 *
 * Notes:
 *   - ycurs -= max_ycurs(xcurs) / 2 (half-screen southward)
 *   - Complement to scr_north() with identical logic but opposite direction
 *   - Movement distance depends on current cursor x position
 */
int
scr_south PARM_0(void)
{
  pager = 0;
  selector = 0;
  ycurs -= max_ycurs(xcurs) / 2;
  return(0);
}

/*
 * scr_se - Move cursor southeast by half-screen distance
 *
 * Advances the cursor position diagonally southeast by approximately
 * half the visible screen dimensions. Moves south by half-screen height
 * while moving east by half-screen width, enabling efficient diagonal
 * navigation in the southeast direction.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Retreats ycurs by half the maximum screen height
 *   - Advances xcurs by half the maximum screen width
 *   - Movement distance adapts to current screen dimensions
 *
 * Notes:
 *   - ycurs -= max_ycurs(xcurs) / 2, xcurs += max_xcurs() / 2
 *   - Mirror of scr_nw() but moves south and east instead
 *   - Useful for rapid southeast map exploration
 */
int
scr_se PARM_0(void)
{
  pager = 0;
  selector = 0;
  ycurs -= max_ycurs(xcurs) / 2;
  xcurs += max_xcurs() / 2;
  return(0);
}

/*
 * scr_sw - Move cursor southwest by half-screen distance
 *
 * Advances the cursor position diagonally southwest by approximately
 * half the visible screen dimensions. Moves south by half-screen height
 * while moving west by half-screen width, enabling efficient diagonal
 * navigation in the southwest direction.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Resets pager and selector to 0 (clears paging state)
 *   - Retreats ycurs by half the maximum screen height
 *   - Retreats xcurs by half the maximum screen width
 *   - Movement distance adapts to current screen dimensions
 *
 * Notes:
 *   - ycurs -= max_ycurs(xcurs) / 2, xcurs -= max_xcurs() / 2
 *   - Mirror of scr_ne() but moves south and west instead
 *   - Completes the 8-direction screen movement function set
 */
int
scr_sw PARM_0(void)
{
  pager = 0;
  selector = 0;
  ycurs -= max_ycurs(xcurs) / 2;
  xcurs -= max_xcurs() / 2;
  return(0);
}

/*
 * show_sect - Render individual sector at specified screen position
 *
 * This function handles the actual drawing of a single map sector to the
 * screen, supporting both detailed (4-character) and simple (1-character)
 * display modes. It performs coordinate transformation, visibility checking,
 * highlighting calculation, and character rendering for map visualization.
 *
 * The function supports two positioning modes:
 * 1. Automatic positioning: x_loc/y_loc = -1, calculates screen position
 * 2. Manual positioning: specific x_loc/y_loc screen coordinates provided
 *
 * Rendering process:
 * 1. Validates sector visibility and screen boundaries
 * 2. Performs coordinate transformation from world to screen space
 * 3. Handles hexagonal vs rectangular coordinate differences
 * 4. Determines appropriate display characters via map_char()
 * 5. Applies highlighting effects via map_highlight()
 * 6. Renders characters using curses drawing functions
 *
 * Display modes supported:
 * - ZOOM_DETAIL: 4-character sector display with borders and multiple positions
 * - Other zoom levels: single character display at focus position
 *
 * Method parameter controls special rendering options:
 * - Bit 0: bypass visibility checking if set
 * - Bit 1: add top border decoration if set
 *
 * Parameters:
 *   x - World X coordinate of sector to display
 *   y - World Y coordinate of sector to display
 *   x_loc - Screen X position (-1 for automatic calculation)
 *   y_loc - Screen Y position (-1 for automatic calculation)
 *   method - Rendering flags controlling special display options
 *
 * Returns: void
 *
 * Side Effects:
 *   - Draws characters to screen using curses mvaddch/mvprintw functions
 *   - May enable/disable standout mode for highlighting
 *   - Updates screen cursor position during rendering
 *
 * Notes:
 *   - Core rendering function for all map display operations
 *   - Handles complex coordinate transformations for hex/rect modes
 *   - Integrates with highlighting system for visual emphasis
 *   - Essential for both full map rendering and incremental updates
 */
void
show_sect PARM_5(int, x, int, y, int, x_loc, int, y_loc, int, method)
{
  int high_on = FALSE, focus, ch1, ch2, tmp;
  int i, j, k;

  /* check the positioning */
  if ((x_loc == -1) ||
      (y_loc == -1)) {

    /* check positioning */
    if (!on_screen(x, y) ||
	!Y_ONMAP(y)) return;

    /* check if the sector is visible; if necessary */
    if (((method & 1) == 0) &&
	(VIS_CHECK((x + MAPX) % MAPX, y) == HS_NOSEE)) return;

    /* now find the real location */
    if (x < xoffset) {
      tmp = x + MAPX;
    } else {
      tmp = x;
    }
    x_loc = (tmp - xoffset) * curmap_screen[zoom_level].xshift;
    y_loc = (y - yoffset) * curmap_screen[zoom_level].yshift;
    if (curmap_screen[zoom_level].oddlift &&
	((x % 2) == 1)) {
      y_loc++;
    }
    y_loc = SCREEN_Y_SIZE - y_loc - 1;
    
  }

  /* check highlighting of the sector */
  focus = display_mode.focus;

  /* have the position, now place the data */
  if (zoom_level == ZOOM_DETAIL) {

    /* set the borders */
    if (world.hexmap) {
      ch1 = '/';
      ch2 = '\\';
    } else {
      ch1 = '|';
      ch2 = '|';
    }

    /* provide the topping, if it fits */
    if (conq_allblanks == FALSE) {
      if ((method & 2) && (y_loc > 1)) {
	mvaddstr(y_loc - 2, x_loc + 1, "__");
      }
    }

    /* first build the sector brace */
    mvprintw(y_loc, x_loc, "%c  %c", ch2, ch1);
    mvprintw(y_loc - 1, x_loc, "%c  %c", ch1, ch2);

    /* now show the top row */
    for (i = 0; i < 2; i++) {
      for (j = 0; j < 2; j++) {

	/* find the positioning */
	k = i + 2 * j;
	move(y_loc - j, x_loc + 1 + i);

	/* check the highlighting */
	if (map_highlight((x + MAPX) % MAPX, y,
			  display_mode.highlight[k],
			  display_mode.target[k])) {
	  high_on = TRUE;
	  standout();
	}

	/* show it and stow it */
	addch(map_char((x + MAPX) % MAPX, y, k,
		       display_mode.style[k]));
	if (high_on == TRUE) {
	  high_on = FALSE;
	  standend();
	}

      }
    }

  } else {

    /* just show the focus sector */
    if (map_highlight((x + MAPX) % MAPX, y,
		      display_mode.highlight[focus],
		      display_mode.target[focus])) {
      high_on = TRUE;
      standout();
    }
    ch1 = map_char((x + MAPX) % MAPX, y, HXPOS_MINIMAL,
		  display_mode.style[focus]);
    mvaddch(y_loc, x_loc, ch1);

    /* turn off highlighting if on */
    if (high_on) standend();

  }
}

/*
 * print_map - Send formatted map output to stdout (UNIMPLEMENTED)
 *
 * This function was designed to generate complete map output to stdout
 * for capture and printing purposes. Currently disabled via #ifdef UNIMPLEMENTED,
 * it would provide an interactive interface for selecting highlighting modes
 * and display options before generating formatted map output.
 *
 * Intended functionality:
 * - Interactive highlighting mode selection from HI_MAXIMUM options
 * - Display style selection for terrain, political, or strategic views
 * - Coordinate system display options
 * - Map boundary and centering controls
 * - Full map rendering to stdout with selected formatting
 *
 * Parameters:
 *   widemap - Flag for wide format output (enables additional options)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Would output interactive prompts to stderr
 *   - Would generate formatted map to stdout
 *   - Currently does nothing due to UNIMPLEMENTED ifdef
 *
 * Notes:
 *   - Implementation is complete but disabled for production
 *   - Requires user interaction via scanf for option selection
 *   - Designed for external map capture and printing workflows
 */
void
print_map PARM_1(int, widemap)
{
  /*ARGSUSED*/
  (void)widemap; /* suppress unused parameter warning */
#ifdef UNIMPLEMENTED
  char tempstr[20];
  register int x, y;
  char *x_seen;
  int ch, choice, hichoice = 0, coord_on, last_on;
  int map_top, map_bottom, map_left, map_right;
  int center_left, center_right;

  fprintf(stderr, "\n=== Conquer Map Display ===\n\n");
  if (widemap) {
    fprintf(stderr, "To capture the map use: conquer -P > foo\n\n");
  } else {
    fprintf(stderr, "To capture the map use: conquer -p > foo\n\n");
  }
  if (widemap) {

    /* show the available highlighting methods */
    fprintf(stderr, "Map Highlighting Options:\n");
    fprintf(stderr, "   ");
    x = 3;
    for (hichoice = 1; hichoice <= HI_MAXIMUM; hichoice++) {

      /* keep track of screen wrapping */
      x += strlen(highl_list[hichoice]) + 4;
      if (x > 70) {
	fprintf(stderr, ",\n   ");
	x = strlen(highl_list[hichoice]) + 3;
      } else if (hichoice != 1) {
	fprintf(stderr, ", ");
      }
      fprintf(stderr, "(%c)%s", highl_list[hichoice][0],
	      &(highl_list[hichoice][1]));
    }
    fprintf(stderr, "\nUse which type of highlighting? ");

    /* now find the choice */
    scanf("%s", string);
    if (strlen(string) == 0) return;
    for (hichoice = 1; hichoice <= HI_MAXIMUM; hichoice++) {
      if (completion(string, highl_list[hichoice])) break;
    }
    if (hichoice > HI_MAXIMUM) {
      fprintf(stderr, "Unknown highlight type; no map generated\n");
      return;
    }
    fprintf(stderr, "\n");
  }

  /* now provide display selection */
  fprintf(stderr, "Map Display Options:\n");
  fprintf(stderr, "   ");
  x = 3;
  for (choice = 1; choice <= DI_MAXIMUM; choice++) {

    /* keep track of screen wrapping */
    x += strlen(display_list[choice]) + 4;
    if (x > 70) {
      fprintf(stderr, ",\n   ");
      x = strlen(display_list[choice]) + 3;
    } else if (choice != 1) {
      fprintf(stderr, ", ");
    }
    fprintf(stderr, "(%c)%s", display_list[choice][0],
	    &(display_list[choice][1]));
  }
  fprintf(stderr, "\nDisplay which type of map? ");

  /* now find the choice */
  scanf("%s", string);
  if (strlen(string) == 0) return;
  for (choice = 1; choice <= DI_MAXIMUM; choice++) {
    if (completion(string, display_list[choice])) break;
  }
  if (choice > DI_MAXIMUM) {
    fprintf(stderr, "Unknown map display type; no map generated\n");
    return;
  }

  /* show the title */
  ch = display_list[choice][0];
  if (islower(ch)) ch = toupper(ch);
  fprintf(stdout, "Conquer %s.%d:  %c%s ",
	  VERSION, PATCHLEVEL, ch, &(display_list[choice][1]));
  if (widemap && hichoice != HI_NONE) {
    ch = highl_list[hichoice][0];
    if (islower(ch)) ch = toupper(ch);
    fprintf(stdout, "/ %c%s ", ch, &(highl_list[hichoice][1]));
  }
  fprintf(stdout, "Map for ");
  if ((country == UNOWNED) || (ntn_ptr == NULL)) {
    fprintf(stdout, "the World");
  } else {
    fprintf(stdout, "Nation %s", ntn_ptr->name);
  }
  roman_number(string, TURN - START_TURN + 1);
  fprintf(stdout, " on Turn %s\n", string);

  /* calculate the range of the map */
  if ((country == UNOWNED) ||
      ( (ntn_ptr != NULL) && WIZ_MAGIC(MW_KNOWALL) )) {

    /* provide the whole map */
    map_top = MAPY - 1;
    map_bottom = 0;
    map_right = MAPX;
    map_left = 0;

  } else {

    /* provide space for horizontal edge detection */
    if ((x_seen = (char *) malloc(sizeof(char) * MAPX)) == NULL) {
      fprintf(stderr, "Memory allocation error in print_map()\n");
      abrt();
    }

    /* clear the array */
    for (x = 0; x < MAPX; x++) x_seen[x] = FALSE;

    /* find the rectangle of what they can see; start with top */
    map_bottom = (-1);
    for (y = 0; (map_bottom == (-1)) && (y < MAPY); y++) {
      for (x = 0; (map_bottom == (-1)) && (x < MAPX); x++) {
	if (VIS_CHECK(x, y)) {
	  map_bottom = y;
	  break;
	}
      }
    }
    map_top = (-1);
    for (y = MAPY - 1; (map_top == (-1)) && (y >= 0); y--) {
      for (x = 0; (map_top == (-1)) && (x < MAPX); x++) {
	if (VIS_CHECK(x, y)) {
	  map_top = y;
	  break;
	}
      }
    }

    /* now keep track of the pattern of visible land */
    map_left = MAPX;
    map_right = (-1);
    for (y = map_bottom; y <= map_top; y++) {
      for (x = 0; x < MAPX; x++) {
	if (VIS_CHECK(x, y)) {
	  x_seen[x] = TRUE;
	  if (x > map_right) map_right = x;
	  if (x < map_left) map_left = x;
	}
      }
    }

    /* now analyze the pattern */
    last_on = map_left;
    center_left = center_right = (-1);

    /* first set the left edge */
    for (x = map_left; x <= map_right; x++) {

      /* check off and on status */
      if (last_on != x - 1) {
	if (x_seen[x] == TRUE) {
	  if (center_right - center_left < x - last_on) {
	    center_right = x;
	    center_left = last_on;
	  }
	}
      }

      /* mark that it is now within owned region */
      if (x_seen[x] == TRUE) {
	last_on = x;
      }

    }

    /* now compare the two large gaps */
    if (center_right - center_left > MAPX + map_left - map_right) {
      map_left = center_right - MAPX;
      map_right = center_left;
    }

    free(x_seen);
  }

  /* find out about coordinates */
  fprintf(stderr, "\n");
  fprintf(stderr, "Do you wish to see the coordinates? ");
  scanf("%s", string);
  putc('\n', stderr);
  if ((string[0] == '\0') ||
      (strcmp(string, "yes") == 0) ||
      (strcmp(string, "y") == 0)) {
    coord_on = 1;
  } else {
    coord_on = FALSE;
  }

  /* display horizontal coordinates */
  if (coord_on == 1) {

    /* determine range of placement */
    if (map_right - map_left >= 100) {
      coord_on = 10;
    } else {
      coord_on = 5;
    }

    /* calculate the coordinate axis */
    for (x = 0; x <= map_right - map_left + 6; x++) {
      string[x] = ' ';
    }
    string[x] = '\0';
    for (x = 0; x <= map_right - map_left; x++) {
      if (((global_int = xloc_relative(x + map_left)) % coord_on) == 0) {
	sprintf(tempstr, "%d", global_int);
	strncpy(string + x + 7 - strlen(tempstr), tempstr, strlen(tempstr));
      }
    }
    fprintf(stdout, "\n%s\n\n", string);
  } else {
    fprintf(stdout, "\n\n\n");
  }

  /* now display the map */
  for (y = map_top; y >= map_bottom; y--) {
    if ((coord_on != 0) && ((global_int = yloc_relative(y)) % 2 == 0)) {
      fprintf(stdout, "%5d ", global_int);
    } else {
      fprintf(stdout, "%5s ", " ");
    }
    for (x = map_left; x <= map_right; x++) {
      if (VIS_CHECK((x + MAPX) % MAPX, y)) {
	ch = get_display_for((x + MAPX) % MAPX, y, choice);
	if (widemap && highl_on((x + MAPX) % MAPX, y, hichoice)) {
	  putc(ch, stdout);
	  putc('\b', stdout);
	}
	putc(ch, stdout);
      } else {
	putc(' ', stdout);
      }
    }
    fprintf(stdout, "\n");
  }
#endif /* UNIMPLEMENTED */
}

/*
 * see_one - Display individual sector with forced visibility
 *
 * Helper function that renders a single sector to the screen with
 * forced visibility parameters. Used by see_around() to display
 * sectors in the immediate vicinity without regard to normal
 * visibility restrictions. Provides tactical view of specific areas.
 *
 * Parameters:
 *   x - Absolute world x coordinate of sector to display
 *   y - Absolute world y coordinate of sector to display
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Calls show_sect() with forced display parameters
 *   - May override normal fog-of-war visibility
 *   - Updates screen display for specified sector
 *
 * Notes:
 *   - Static function used internally by visibility system
 *   - Screen position parameters (-1, -1) let show_sect() calculate position
 *   - Force parameter (1) overrides visibility restrictions
 */
static void
see_one PARM_2(int, x, int, y)
{
  show_sect(x, y, -1, -1, 1);
}

/*
 * see_around - Display all sectors within 1-hex radius of position
 *
 * Renders all sectors immediately surrounding the specified coordinates,
 * providing a tactical overview of the local area. Uses map_loop() to
 * visit each sector in a 1-hex radius and calls see_one() to force
 * display regardless of normal visibility restrictions.
 *
 * This function is used for tactical displays where the player needs
 * to see immediate surroundings, such as during unit movement or when
 * examining specific areas of strategic importance.
 *
 * Parameters:
 *   x - Central absolute world x coordinate
 *   y - Central absolute world y coordinate
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Updates screen display for 7 sectors (center + 6 adjacent)
 *   - May override normal fog-of-war visibility restrictions
 *   - Calls see_one() for each sector via map_loop()
 *
 * Notes:
 *   - Uses map_loop(x, y, 1, see_one) for efficient sector iteration
 *   - Radius of 1 includes center hex plus 6 adjacent hexes
 *   - Forced visibility overrides concealment and fog-of-war
 */
void
see_around PARM_2 ( int, x, int, y )
{
  /* visit each sector */
  map_loop(x, y, 1, see_one);
}

/*
 * coffmap - Check cursor bounds and trigger screen redraw if needed
 *
 * This critical function monitors cursor position to detect when it has
 * moved outside the visible screen boundaries and triggers appropriate
 * screen recentering and redrawing. It handles both relative and absolute
 * coordinate systems, implementing different boundary checking logic for
 * gods versus normal players.
 *
 * Boundary checking logic:
 * - Gods/absolute mode: Uses world boundaries (MAPY) with special edge cases
 * - Relative mode: Uses screen boundaries with automatic recentering
 * - Handles screen wrapping and edge cases for both hex and rectangular modes
 *
 * Screen redraw management:
 * - DRAW_FULL: Complete screen clear and redraw (including VAX compatibility)
 * - Partial redraw: Clear from cursor to bottom, preserves top content
 * - Calls makemap() to rebuild the map display
 * - Updates bottom status and side information panels
 * - Restores cursor position and refreshes display
 *
 * Parameters:
 *   None (operates on global cursor state)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - May call centermap() to recenter view when cursor goes out of bounds
 *   - Clears and redraws screen based on redraw mode
 *   - Updates makebottom() and makeside() displays
 *   - Calls show_cursor() and refresh() to finalize display
 *   - Resets redraw flag to DRAW_DONE when complete
 *
 * Notes:
 *   - Name suggests "Cursor Off Map" - boundary detection function
 *   - Handles special VAX curses compatibility with extra refresh()
 *   - Critical for maintaining proper display during navigation
 *   - Different logic for relative vs absolute coordinate systems
 */
void
coffmap PARM_0(void)
{
  int x, maxx, maxy;

  /* set boundaries */
  if (!world.relative_map ||
      ((is_god == TRUE) || (ntn_ptr == NULL) || WIZ_MAGIC(MW_KNOWALL))) {
    if (YREAL > MAPY - 1) {
      ycurs = MAPY - yoffset - 1;
    }
    if (YREAL < 0) {
      ycurs = - yoffset;
    }
  }

  /* find the boundaries */
  maxx = max_xcurs();
  maxy = max_ycurs(xcurs + xoffset);

  /* now check them */
  if (!world.relative_map || (is_god == TRUE) ||
      (ntn_ptr == NULL) || WIZ_MAGIC(MW_KNOWALL)) {

    /* check the wrapping */
    if ((xcurs <= 0) ||
	((ycurs == 0) && (YREAL != 0)) ||
	(ycurs < 0) ||
	(xcurs >= maxx - 1) ||
	((ycurs == maxy - 1) && (YREAL < MAPY - 1)) ||
	(ycurs > maxy - 1)) {
      centermap();
    }

  } else {

    /* check wrapping at all times */
    if ((xcurs <= 0) ||
	(ycurs <= 0) ||
	(xcurs >= maxx - 1) ||
	(ycurs >= maxy - 1)) {
      centermap();
    }

  }

  /* clear screen appropriately */
  if (redraw == DRAW_FULL) {

    /* redraw all of the screen */
    clear();

#ifdef VAXC
    /* extra refreshment for VAXC curses */
    refresh();
#endif /* VAXC */

  } else {

    /* just draw what has been changed */
    move(0,0);
    clrtobot();

  }

  /* build the map */
  makemap();

  /* handle bottom and side of screen */
  if (redraw != DRAW_NOFRILLS) {

    /* do the bottom */
    makebottom();

    /* redraw the side of the screen */
    if (display_mode.style[display_mode.focus] == DI_FMOVE) {
      x = MOVEIND_FLY;
    } else if (display_mode.style[display_mode.focus] == DI_NMOVE) {
      x = MOVEIND_WATER;
    } else {
      x = MOVEIND_NORM;
    }
    makeside(FALSE, x);

    /* show the cursor */
    show_cursor();
    refresh();
    redraw = DRAW_DONE;

  }
}

/*
 * hs_seepart - Set sector visibility to partial sight level
 *
 * Helper function that upgrades sector visibility to HS_SEEPART level
 * if current visibility is lower. This provides basic terrain visibility
 * without revealing detailed information about units, ownership, or
 * strategic resources. Part of the fog-of-war visibility system.
 *
 * Parameters:
 *   x - Absolute world x coordinate of sector
 *   y - Absolute world y coordinate of sector
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Updates VIS_STORE(x, y) to HS_SEEPART if current visibility is lower
 *   - Does not downgrade visibility if already higher than HS_SEEPART
 *
 * Notes:
 *   - Static helper function used by visibility calculation systems
 *   - HS_SEEPART allows basic terrain recognition without strategic detail
 *   - Part of graduated visibility system: NOSEE < SEEPART < SEEMOST < SEEFULL < SEEALL
 */
static void
hs_seepart PARM_2(int, x, int, y)
{
  /* check and assign */
  if (VIS_CHECK(x, y) < HS_SEEPART)
    VIS_STORE(x, y, HS_SEEPART);
}

/*
 * hs_armysee - Set sector visibility based on army sight capabilities
 *
 * Helper function that determines appropriate visibility level for sectors
 * containing armies, based on the army's sight capabilities and status.
 * Armies that can see (a_cansee) and are not sieged provide full visibility,
 * while limited armies provide partial visibility.
 *
 * Visibility logic:
 * - Armies with sight capability + not sieged: HS_SEEFULL (complete detail)
 * - Limited or sieged armies: HS_SEEMOST (most information visible)
 * - Never downgrades existing higher visibility levels
 *
 * Parameters:
 *   x - Absolute world x coordinate of sector with army
 *   y - Absolute world y coordinate of sector with army
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Updates VIS_STORE(x, y) based on army sight capabilities
 *   - Checks ARMY_TYPE macro and unit_status() for army properties
 *   - May upgrade visibility to HS_SEEFULL or HS_SEEMOST
 *
 * Notes:
 *   - Static helper function used by whatcansee() visibility calculation
 *   - Sieged armies have reduced sight capabilities
 *   - a_cansee(ARMY_TYPE) determines if army has sight capability
 *   - Part of tactical visibility system for unit-based reconnaissance
 */
static void
hs_armysee PARM_2(int, x, int, y)
{
  /* set the spot as visible */
  if (a_cansee(ARMY_TYPE) &&
      (unit_status(ARMY_STAT) != ST_SIEGED)) {
    if (VIS_CHECK(x, y) < HS_SEEFULL) {
      VIS_STORE(x, y, HS_SEEFULL);
    }
  } else if (VIS_CHECK(x, y) < HS_SEEMOST) {
    VIS_STORE(x, y, HS_SEEMOST);
  }
}

/*
 * whatcansee - Calculate complete map visibility based on nation's assets
 *
 * This is the core fog-of-war calculation function that determines what sectors
 * the current nation can see based on owned territory, unit positions, and
 * special abilities. It implements a sophisticated visibility system with
 * multiple levels of sight and strategic concealment.
 *
 * Visibility calculation process:
 * 1. Gods and deity mode: grant complete map visibility (HS_SEEALL)
 * 2. Clear entire visibility map to HS_NOSEE (no visibility)
 * 3. Owned sectors: provide base visibility around territory (LANDSEE range)
 * 4. Navy units: grant sea and coastal visibility (NAVYSEE range)
 * 5. Army units: provide tactical visibility with enhanced sight for scouts
 * 6. Caravan units: offer limited local visibility (CVNSEE range)
 * 7. Special units: agents and scouts get enhanced visibility levels
 *
 * Visibility levels implemented:
 * - HS_NOSEE: complete darkness, no information available
 * - HS_SEEPART: basic terrain visible, limited detail
 * - HS_SEEMOST: most sector information visible
 * - HS_SEEFULL: complete sector detail visible
 * - HS_SEEALL: perfect visibility including hidden information
 *
 * Unit sight capabilities:
 * - Standard armies: ARMYSEE range base visibility, 1-hex detailed view
 * - Scout armies: enhanced sight range and can penetrate concealment
 * - Agent armies: perfect local visibility if capable of seeing
 * - All navies: NAVYSEE range for water and coastal reconnaissance
 * - Caravans: CVNSEE range limited local visibility for trade routes
 *
 * Parameters: None (operates on current nation's global game state)
 *
 * Returns: void
 *
 * Side Effects:
 *   - Completely recalculates and updates global visibility array
 *   - Sets VIS_STORE values for all map sectors
 *   - Affects all subsequent map display and strategic calculations
 *
 * Notes:
 *   - Performance-critical function affecting entire game experience
 *   - Implements complex fog-of-war strategy game mechanics
 *   - Balances strategic information vs. realistic limited visibility
 *   - Essential for multiplayer competitive gameplay
 *   - Supports both exploration and reconnaissance gameplay elements
 */
void
whatcansee PARM_0(void)
{
  register int x = 0, y = 0;
  int i, j;

  /* the shadow knows... */
  if ((is_god == TRUE) ||
      (ntn_ptr == NULL) ||
      WIZ_MAGIC(MW_KNOWALL)) {
    for (x = 0; x < MAPX; x++)
    for (y = 0; y < MAPY; y++) {
      VIS_STORE(x ,y, HS_SEEALL);
    }
    return;
  }

  /* blank out the what is known */
  for (x = 0; x < MAPX; x++)
  for (y = 0; y < MAPY; y++) {
    VIS_STORE(x, y, HS_NOSEE);
  }

  /* search for all visible sectors */
  for (x = ntn_ptr->leftedge; x <= ntn_ptr->rightedge; x++)
  for (y = ntn_ptr->bottomedge; y <= ntn_ptr->topedge; y++) {

    /* if a sector is owned by the country */
    if (Y_ONMAP(y)
	&& (sct[(x + MAPX) % MAPX][y].owner == country)) {

      /* loop around the sector */
      map_loop((x + MAPX) % MAPX, y, LANDSEE, hs_seepart);

    }

  }

  /* go through all of the navies and mark visibility */
  for (navy_ptr = ntn_ptr->navy_list;
       navy_ptr != NULL;
       navy_ptr = navy_ptr->next) {

    /* if the navy is active it can see */
    j = 0;
    for (i = 0; i < NSHP_NUMBER; i++) {
      j |= NAVY_SHIPS[i];
    }
    if (j == 0) continue;
    x = NAVY_XLOC;
    y = NAVY_YLOC;
    if (XY_ONMAP(x, y)) {
      /* loop around location of the navy */
      map_loop(x, y, NAVYSEE, hs_seepart);
      if (VIS_CHECK(x, y) < HS_SEEMOST) {
	VIS_STORE(x, y, HS_SEEMOST);
      }
    }
  }

  /* go though all of the soldiers */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {

    /* check if the army is active */
    if (ARMY_SIZE > 0) {

      /* See around it */
      map_loop(ARMY_XLOC, ARMY_YLOC, ARMYSEE, hs_seepart);

      /* See near it */
      map_loop(ARMY_XLOC, ARMY_YLOC, 1, hs_armysee);

      /* See on it */
      if (a_isagent(ARMY_TYPE) &&
	  a_cansee(ARMY_TYPE)) {
	VIS_STORE(ARMY_XLOC, ARMY_YLOC, HS_SEEALL);
      } else if ((a_isagent(ARMY_TYPE) || a_cansee(ARMY_TYPE)) &&
		 VIS_CHECK(ARMY_XLOC, ARMY_YLOC) < HS_SEEFULL) {
	VIS_STORE(ARMY_XLOC, ARMY_YLOC, HS_SEEFULL);
      }

    }
  }

  /* go though all of the supply lines */
  for (cvn_ptr = ntn_ptr->cvn_list;
       cvn_ptr != NULL;
       cvn_ptr = cvn_ptr->next) {

    /* check if the caravan is active */
    if (CVN_SIZE == 0) continue;

    /* set visibility */
    x = CVN_XLOC;
    y = CVN_YLOC;
    if (XY_ONMAP(x, y)) {
      map_loop(x, y, CVNSEE, hs_seepart);
      if (VIS_CHECK(x, y) < HS_SEEMOST) {
	VIS_STORE(x, y, HS_SEEMOST);
      }
    }

  }

}

/*
 * mark_sector - Store current cursor position for jump-to-mark feature
 *
 * Records the current cursor position in global mark variables (xloc_mark,
 * yloc_mark) for later retrieval via the "jump-to-mark" command. This
 * provides a bookmark system allowing players to mark strategic locations
 * and quickly return to them during gameplay.
 *
 * User feedback:
 * - Clears bottom screen area for message display
 * - Shows confirmation message about jump-to-mark availability
 * - Pauses for 2 seconds to ensure message visibility
 * - Stores current XREAL, YREAL coordinates as mark position
 *
 * Parameters:
 *   None (operates on current cursor position)
 *
 * Returns:
 *   0 always (success)
 *
 * Side Effects:
 *   - Clears bottom screen area with clear_bottom(0)
 *   - Displays user message via bottommsg()
 *   - Updates global xloc_mark and yloc_mark variables
 *   - Pauses execution for 2 seconds for user feedback
 *
 * Notes:
 *   - Stores absolute world coordinates (XREAL, YREAL)
 *   - Mark persists until overwritten by another mark_sector() call
 *   - Essential for strategic gameplay and map navigation efficiency
 *   - Complemented by jump-to-mark command for retrieval
 */
int
mark_sector PARM_0(void)
{
  clear_bottom(0);
  bottommsg("That sector may be revisited using the \"jump-to-mark\" command");
  move(LINES - 1, 0);
  sleep(2);
  xloc_mark = XREAL;
  yloc_mark = YREAL;
  return(0);
}

/*
 * recenter_ntn - Realign nation's relative coordinate system to current position
 *
 * Adjusts the nation's relative coordinate system center to the current
 * cursor position, allowing players to redefine their coordinate reference
 * point. This is essential for nations using relative mapping mode where
 * coordinates are displayed relative to a central reference point.
 *
 * Validation checks:
 * - Only functions in relative map mode (world.relative_map)
 * - Gods and null nations don't need coordinate adjustment
 * - Target sector must be on-map and visible (charted territory)
 * - Provides appropriate error messages for invalid operations
 *
 * Recentering process:
 * - Updates nation's centerx and centery to current XREAL, YREAL
 * - Displays confirmation message about coordinate alignment
 * - Prompts user for permanent adjustment (via y_or_n())
 * - If confirmed, calls XADJRLOC to make adjustment permanent
 *
 * Parameters:
 *   None (operates on current nation and cursor position)
 *
 * Returns:
 *   0 always (success or graceful failure)
 *
 * Side Effects:
 *   - Clears bottom screen area for user interaction
 *   - Updates ntn_ptr->centerx and ntn_ptr->centery coordinates
 *   - May call XADJRLOC macro to save permanent coordinate adjustment
 *   - Displays status messages and prompts for user confirmation
 *
 * Notes:
 *   - Only valid for non-god players in relative map mode
 *   - Cannot center on uncharted or invisible sectors
 *   - Permanent adjustment affects future game sessions
 *   - Critical for relative coordinate system usability
 */
int
recenter_ntn PARM_0(void)
{
  /* clean bottom */
  clear_bottom(0);

  /* do we need to realign system? */
  if (!world.relative_map ||
      (is_god == TRUE) ||
      (ntn_ptr == NULL)) {
    errormsg("There is no need to adjust the coordinate system");
    return(0);
  }

  /* is this a proper sector? */
  if (!Y_ONMAP(YREAL) ||
      (VIS_CHECK(XREAL, YREAL) == FALSE)) {
    /* can't realign it */
    errormsg("Your coordinate system cannot be centered on uncharted lands");
    return(0);
  }

  /* align it */
  ntn_ptr->centerx = XREAL;
  ntn_ptr->centery = YREAL;
  mvaddstr(LINES - 3, 0, "Aligning coordinates to new central position");

  /* query permanence */
  mvaddstr(LINES - 2, 0, "Make the adjustment permanent? ");
  if (y_or_n() == TRUE) {
    XADJRLOC;
  }
  return(0);
}
