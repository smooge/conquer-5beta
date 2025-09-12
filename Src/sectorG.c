/*
 * sectorG.c - Sector Manipulation and Information Interface
 *
 * This file implements comprehensive sector management functionality for the Conquer game.
 * It provides both user and god commands for manipulating sector properties, designations,
 * and construction options. The system handles major designations (cities, capitals, etc.),
 * minor designations (fortifications, constructions), and detailed sector information display.
 *
 * Key Features:
 * - Major Designation Management: City creation, capital designation, designation changes
 * - Minor Construction System: Fortifications, roads, and other improvements
 * - Sector Information Display: Detailed production, consumption, and status information
 * - God Mode Functions: Complete sector property manipulation (altitude, ownership, etc.)
 * - Resource Management: Cost calculation, resource validation, and consumption tracking
 * - City Integration: Automatic city creation/destruction during designation changes
 *
 * The system enforces game rules including:
 * - Ownership requirements for construction and modification
 * - Siege restrictions preventing construction in besieged sectors
 * - Resource availability validation for all construction projects
 * - Proper cost calculation and resource consumption
 * - Capital designation transfer mechanics
 *
 * God Mode Capabilities:
 * - Altitude adjustment with automatic terrain validation
 * - Ownership transfer with city relocation handling
 * - Population manipulation and construction timing
 * - Tradegood and mineral value assignment
 * - Vegetation control with terrain compatibility checking
 *
 * Functions:
 * - change_desg(): Interactive major designation modification system
 * - construct(): Minor designation construction interface
 * - sect_info(): Comprehensive sector information display
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
#define USE_CODES
#define PRINT_CODES
#include "cityX.h"
#include "worldX.h"
#undef USE_CODES
#undef PRINT_CODES
#include "magicX.h"
#include "mtrlsX.h"
#include "desigX.h"
#include "elevegX.h"
#include "hlightX.h"
#include "stringX.h"
#include "tgoodsX.h"
#include "displayG.h"

/*
 * change_desg - Interactive major designation modification system
 *
 * Provides comprehensive interface for changing sector major designations including
 * city creation, capital designation, and god mode sector property manipulation.
 * Handles all aspects of designation changes from resource validation to city
 * management and automatic updates of related game systems.
 *
 * The function operates in two distinct modes:
 * - God Mode: Complete sector property manipulation (altitude, ownership, population, etc.)
 * - Normal Mode: Major designation changes with proper resource costs and validation
 *
 * God Mode Functions:
 * - Altitude: Terrain modification with automatic property adjustment
 * - Designation: Direct designation changes bypassing normal restrictions
 * - Mineral Value: Resource value assignment for tradegood sectors
 * - Owner: Ownership transfer with automatic city relocation
 * - Population: Direct population manipulation for sectors
 * - Tradegood: Tradegood type assignment with mineral validation
 * - Vegetation: Terrain vegetation with altitude compatibility checking
 *
 * Normal Mode Functions:
 * - Major designation changes (None, City, Capital, etc.)
 * - Automatic city creation with naming interface
 * - Resource cost calculation and validation
 * - Capital designation transfer with old capital adjustment
 * - City destruction when changing from city designations
 * - Construction time delays for new city buildings
 *
 * Parameters:
 *   void - Uses global coordinates XREAL, YREAL for target sector
 *
 * Returns:
 *   int - MOVECOST on successful designation change, 0 on cancellation or error
 *
 * Side Effects:
 *   - Modifies sector designation and related properties
 *   - Creates or destroys cities as needed
 *   - Transfers capital designation and updates nation properties
 *   - Consumes resources for construction costs
 *   - Updates city weights and construction timing
 *   - Records all changes for game persistence
 *   - Temporarily switches god mode context for ownership changes
 *
 * Notes:
 *   - Validates map coordinates and ownership before operations
 *   - Prevents capital changes without designating new capital first
 *   - Enforces siege restrictions on construction
 *   - Handles city name conflicts with automatic resolution
 *   - God mode altitude changes automatically adjust other properties
 *   - Water sectors reset to default empty state
 *   - Mineral sectors require compatible tradegoods for non-zero values
 */
int
change_desg PARM_0(void)
{
  ITEM_PTR cost_ptr = NULL;
  SCT_PTR shold_ptr;
  int under_siege = FALSE;
  int mustset = FALSE, high_item;
  char outbuf[LINELTH];
  int xloc, yloc, count;
  long value;

  /* first check if it is possible to adjust the sector */
  if (!XY_ONMAP(XREAL, YREAL) ||
      ((is_god == FALSE) && (sct[XREAL][YREAL].owner != country))) {

    if (is_god == TRUE)
      errormsg("That sector is off the map");
    else errormsg("That sector is not owned by you");
    return(0);

  }

  /* quick reference */
  sct_ptr = &(sct[XREAL][YREAL]);
  shold_ptr = sct_ptr;
  if (minor_desg(sct_ptr->designation, MIN_SIEGED)) {
    under_siege = TRUE;
  }

  /* check for special god functions */
  if (is_god == TRUE) {

    /* let god know the adjustment options */
    clear_bottom(0);
    mvaddstr(LINES - 5, 0, "Super User Sector Change Options:");
    move(LINES - 4, 3);
    hip_string("Altitude, ", 1, TRUE);
    hip_string("Designation, ", 1, TRUE);
    hip_string("Mineral Value, ", 1, TRUE);
    hip_string("Owner, ", 1, TRUE);
    move(LINES - 3, 3);
    hip_string("Population, ", 1, TRUE);
    hip_string("Tradegood, ", 1, TRUE);
    hip_string("Vegetation, ", 1, TRUE);
    mvaddstr(LINES - 2, 0, "Enter your selection: ");

    /* get the results */
    refresh();
    switch (next_char()) {
    case '\n':
    case '\r':
    case ' ':
      /* quick out */
      return(0);
    case 'A':
    case 'a':
      /* adjust the altitude */
      mvaddstr(LINES - 2, 0, "Set sector altitude to what? ");
      value = get_altitude();
      if ((no_input == TRUE) || (value == sct_ptr->altitude)) {
	return(0);
      }
      if ((value > ELE_PEAK) || (value < 0)) {
	errormsg("I did not know that such an altitude existed");
	return(0);
      }

      /* set the altitude */
      sct_ptr->altitude = (uns_char) value;
      SADJALT;

      /* check water sectors */
      if (value == ELE_WATER) {
	sct_ptr->vegetation = VEG_NONE;
	SADJVEG;
	sct_ptr->people = 0L;
	SADJPEOP;
	sct_ptr->designation = MAJ_NONE;
	SADJDESG;
	sct_ptr->tradegood = TG_NONE;
	SADJTGOOD;
	sct_ptr->minerals = 0;
	SADJMNRLS;
	return(0);
      }

      /* a vegetation value must be entered */
      if (sct_ptr->vegetation == VEG_NONE) mustset = TRUE;
      /* FALLTHROUGH */
    case 'V':
    case 'v':
      /* check for water */
      if (sct_ptr->altitude == ELE_WATER) {
	if (sct_ptr->vegetation != VEG_NONE) {
	  sct_ptr->vegetation = VEG_NONE;
	  SADJVEG;
	}
	errormsg("This sector must have a vegetation of \"None\"");
	return(0);
      }

      /* check for the vegetation */
    set_veg:
      mvaddstr(LINES - 2, 0, "Set sector vegetation to what? ");
      clrtoeol();
      value = get_vegetation();

      /* check for proper input */
      if ((mustset == TRUE) &&
	  ((no_input == TRUE) || (value == VEG_NONE))) {
	errormsg("You must set a vegetation for this sector!");
	goto set_veg;
      }
      if ((value > VEG_NONE) || (value < 0)) {
	errormsg("I do not know of any such vegetation");
	if (mustset == TRUE) goto set_veg;
	return(0);
      }

      /* adjust the vegetation */
      if ((no_input == TRUE) || (value == sct_ptr->vegetation)) {
	return(0);
      }
      if ((value == VEG_VOLCANO) && (sct_ptr->altitude < ELE_HILL)) {
	errormsg("A volcano must at least be on a hill");
	if (mustset == TRUE) goto set_veg;
      } else if ((sct_ptr->altitude > ELE_HILL) &&
		 ((value == VEG_DESERT) || (value == VEG_SWAMP) ||
		  (value == VEG_JUNGLE))) {
	errormsg("You may not have that vegetation for that altitude");
	if (mustset == TRUE) goto set_veg;
      } else {
	sct_ptr->vegetation = (uns_char) value;
	SADJVEG;
      }
      return(0);
    case 'D':
    case 'd':
      /* check sector owner */
      if (sct_ptr->owner == UNOWNED) {
	errormsg("You may not place a designation on an unowned sector");
	return(0);
      }
      /* just go down to the designation mode */
      if (get_god(sct_ptr->owner, TRUE)) {
	return(0);
      }
      break;
    case 'O':
    case 'o':
      mvaddstr(LINES - 2, 0,
	       "Who should own this sector? ['god' for unowned] ");
      value = get_country(FALSE);
      if ((no_input == TRUE) || (value == sct_ptr->owner)) {
	return(0);
      }

      /* check the value */
      if ((value >= ABSMAXNTN) ||
	  (value < 0) ||
	  ((value != 0) && (world.np[value] == NULL))) {
	errormsg("I do not know of any such country");
	return(0);
      }

      /* check the current owner */
      city_ptr = NULL;
      if (IS_CITY(sct_ptr->designation)) {
	if (!get_god(sct_ptr->owner, TRUE)) {
	  if ((city_ptr = citybyloc(ntn_ptr, XREAL, YREAL)) != NULL) {
	    /* copy the city */
	    city_tptr = new_city();
	    strcpy(CITYT_NAME, CITY_NAME);
	    CITYT_XLOC = CITY_XLOC;
	    CITYT_YLOC = CITY_YLOC;
	    CITYT_PEOPLE = CITY_PEOPLE;
	    CITYT_FORT = CITY_FORT;
	    CITYT_WEIGHT = CITY_WEIGHT;
	    CITYT_STALONS = CITY_STALONS;
	    for (count = 0; count < MTRLS_NUMBER; count++) {
	      CITYT_MTRLS[count] = CITY_MTRLS[count];
	      CITYT_IMTRLS[count] = CITY_IMTRLS[count];
	    }

	    /* now get rid of the original */
	    CDESTROY;
	    dest_city(CITY_NAME);
	    city_ptr = city_tptr;
	  }
	  reset_god();
	}
      }

      /* assign the owner */
      sct_ptr->owner = (ntntype) value;
      SADJOWN;

      /* check if it should all be reset */
      if (value == UNOWNED) {
	/* clear out the sector */
	sct_ptr->designation = MAJ_NONE;
	SADJDESG;
	if (city_ptr == NULL) free(city_ptr);
      } else if (IS_CITY(sct_ptr->designation)) {
	if ((city_ptr != NULL) &&
	    !get_god(value, TRUE)) {
	  /* transfer any cities */
	  while (citybyname(CITY_NAME) != NULL) {
	    /* adjust names */
	    do {
	      city_ptr->name[0] = 'A' + rand_val('Z' - 'A');
	    } while (!isprint(city_ptr->name[0]));
	  }

	  /* now perform the transfer internally */
	  city_ptr->next = ntn_ptr->city_list;
	  ntn_ptr->city_list = city_ptr;
	  city_sort();

	  /* now make sure to create the city on the records */
	  CCREATE;
	  CADJLOC;
	  CADJPEOP;
	  CADJWEIGHT;
	  CADJFORT;
	  CADJSTALONS;
	  for (count = 0; count < MTRLS_NUMBER; count++) {
	    if (CITY_MTRLS[count] > 0) {
	      CADJMTRLS(count);
	    }
	    if (CITY_IMTRLS[count] > 0) {
	      CADJIMTRLS(count);
	    }
	  }

	  /* fix god */
	  reset_god();
	} else {
	  /* clear out the sector */
	  sct_ptr->designation = MAJ_NONE;
	  SADJDESG;
	}
      }
      return(0);
    case 'P':
    case 'p':
      /* check the ownership */
      if (sct_ptr->altitude == ELE_WATER) {
	errormsg("You may not place people underwater");
	return(0);
      }
      if (get_god(sct_ptr->owner, TRUE)) {
	return(0);
      }
      
      /* set the value of the population */
      mvaddstr(LINES - 2, 0, "Set the sector population to how much? ");
      value = get_number(FALSE);
      if ((no_input == TRUE) || (sct_ptr->people == value)) {
	return(0);
      }

      /* now change it */
      sct_ptr->people = value;
      SADJPEOP;
      reset_god();
      return(0);
    case 'T':
    case 't':
      /* adjust the tradegood */
      if (sct_ptr->altitude == ELE_WATER) {
	errormsg("You may not place a tradegood underwater");
	return(0);
      }
      mvaddstr(LINES - 2, 0, "Set the sector tradegood to what? ");
      value = get_tradegood();
      if ((no_input == TRUE) || (value == sct_ptr->tradegood)) {
	return(0);
      }

      /* check the value */
      if ((value >= tgoods_number) || (value < 0)) {
	errormsg("I do not know of any such tradegood");
	return(0);
      }

      /* assign the tradegood */
      sct_ptr->tradegood = (uns_char) value;
      SADJTGOOD;

      if (tg_ismetal(value) ||
	  tg_isspell(value) ||
	  tg_isjewel(value)) mustset = TRUE;
      /* FALLTHROUGH */
    case 'M':
    case 'm':
      /* check for emptry tradegoods */
      if (sct_ptr->altitude == ELE_WATER) {
	errormsg("You may not place minerals underwater");
	return(0);
      }
      if (!tg_ismetal(sct_ptr->tradegood) &&
	  !tg_isspell(sct_ptr->tradegood) &&
	  !tg_isjewel(sct_ptr->tradegood)) {
	if (sct_ptr->minerals != 0) {
	  sct_ptr->minerals = 0;
	  SADJMNRLS;
	}
	errormsg("This sector must have a mineral value of 0");
	return(0);
      }

      /* check for the minerals */
    set_mnrls:
      mvaddstr(LINES - 2, 0, "Set sector mineral value to what? ");
      clrtoeol();
      value = get_number(FALSE);
      if (value > 100) {
	errormsg("You may not set the value so high!");
	if (mustset == TRUE) goto set_mnrls;
	return(0);
      }

      /* check for proper input */
      if ((mustset == TRUE) &&
	  ((no_input == TRUE) || (value <= 0))) {
	errormsg("You must set a mineral value for this sector!");
	goto set_mnrls;
      }

      /* adjust the vegetation */
      if ((no_input == TRUE) || (value == sct_ptr->minerals)) {
	return(0);
      }
      sct_ptr->minerals = (uns_char) value;
      SADJMNRLS;
      return(0);
    default:
      errormsg("That is not among the options I gave you!");
      return(0);
    }

  } else if (under_siege == TRUE) {

    /* can't adjust it */
    errormsg("It is not possible to build in a sector under siege");
    return(0);

  } else {

    /* now check if any resources are nearby */
    if ((cost_ptr = find_resources(XREAL, YREAL, under_siege)) == NULL) {
      errormsg("There are no resources available to this sector");
      return(0);
    }
    free(cost_ptr);
    cost_ptr = NULL;

  }

  /* if it is a capital.. forget it */
  if ((is_god == FALSE) &&
      (major_desg(sct_ptr->designation) == MAJ_CAPITAL)) {
    errormsg("You must first designate a new Capital");
    return(0);
  }

  /* list possible designations for the sector */
  clear_bottom(0);
  mvaddstr(LINES - 4, 0, "Possible Designations: ");
  xloc = 23;
  yloc = LINES - 4;
  mustset = FALSE;
  for (count = 0; count < MAJ_NUMBER; count++) {

    /* to comma or not to comma? */
    if (count > 0) {
      addstr(", ");
      xloc += 2;
    }

    /* is it possible? */
    if (desg_ok(XREAL, YREAL, count, FALSE) == TRUE) {

      /* can be done */
      high_item = TRUE;
      mustset = TRUE;

    } else {

      /* can't be done */
      high_item = FALSE;

    }

    /* check spacing */
    if (xloc + (int)strlen(desg_selects[count]) > COLS - 8) {
      yloc++;
      xloc = 2;
      move(yloc, xloc);
    }

    /* show show it */
    hip_string(desg_selects[count], 1, high_item);

    /* now place it */
    xloc += strlen(desg_selects[count]) + 2;

  }

  /* was there an available designation */
  if (mustset == FALSE) {
    if ((is_god != TRUE) &&
	(find_resources(XREAL, YREAL, under_siege) == NULL)) {
      errormsg("There are not enough available resources");
    } else {
      errormsg("There are no possible designations for this sector");
      if (is_god == TRUE) reset_god();
    }
    return(0);
  }

  /* ask for selection */
  mvaddstr(++yloc, 0, "Enter your selection: ");
  refresh();
  value = next_char();
  if ((value == ' ') || (value == '\n') || (value == '\r')) {
    if (is_god == TRUE) reset_god();
    return(0);
  }

  /* check for the selection */
  for (count = 0; count < MAJ_NUMBER; count++) {
    if (value == desg_selects[count][0]) break;
  }

  /* check input */
  if (count == MAJ_NUMBER) {
    errormsg("That was not among your choices");
    if (is_god == TRUE) reset_god();
    return(0);
  }

  /* check it */
  value = count;
  if (value == major_desg(sct_ptr->designation)) {
    errormsg("Hey, wake up!  That is the current designation");
  } else if (desg_ok(XREAL, YREAL, value, TRUE) == TRUE) {

    /* check costs */
    if (is_god == FALSE) {

      /* get the costs */
      if ((cost_ptr = majdesg_costs(value, XREAL, YREAL, TRUE)) ==
	  NULL) {
	errormsg("Help me, I am so confused");
	return(0);
      }

      /* query the building */
      if (conq_expert == FALSE) {
	strcpy(outbuf, "Cost:");
	for (count = 0; count < MTRLS_NUMBER; count++) {
	  if (cost_ptr->mtrls[count] > 0) {
	    sprintf(string, " %.0f %s",
		    (double) cost_ptr->mtrls[count],
		    mtrls_info[count].name);
	    strcat(outbuf, string);
	  }
	}
	strcat(outbuf, "; Continue? ");
	bottommsg(outbuf);
	if (y_or_n() == FALSE) {
	  free(cost_ptr);
	  return(0);
	}
      }

    }

    /* check if a city is being created */
    if (!IS_CITY(sct_ptr->designation) &&
	IS_CITY(value)) {
    buildnewcity:
      mvprintw(LINES - 2, 0,
	       "What name do you wish to give to the new %s? ",
	       maj_dinfo[value].name);
      clrtoeol();
      get_string(string, STR_NORM, NAMELTH);
      if (no_input == TRUE) {
	if (is_god == TRUE) reset_god();
	else free(cost_ptr);
	return(0);
      }
      if (citybyname(string) != NULL) {
	errormsg("That name is already in use in your nation");
	goto buildnewcity;
      }

      /* create it */
      city_ptr = crt_city(string);
      CITY_XLOC = XREAL;
      CITY_YLOC = YREAL;

      /* record the creation */
      set_majordesg(sct_ptr->designation, (uns_short) value);
      CITY_WEIGHT = dflt_cityweight(country, city_ptr);
      CCREATE;
      CADJWEIGHT;
      CADJLOC;
      set_weights(FALSE);

    } else if (IS_CITY(sct_ptr->designation)) {

      /* handle destruction */
      if (!IS_CITY(value)) {

	/* check inventory */
	if ((city_ptr = citybyloc(ntn_ptr, XREAL, YREAL)) != NULL) {
	  /* get rid of it */
	  CDESTROY;
	  dest_city(CITY_NAME);
	}

      } else {

	if ((city_ptr = citybyloc(ntn_ptr, XREAL, YREAL)) != NULL) {

	  /* set the time needed to build the item */
	  int olddesg = sct_ptr->designation;

	  /* determine the new weighting */
	  set_majordesg(sct_ptr->designation, (uns_short) value);
	  if (CITY_WEIGHT < dflt_cityweight(country, city_ptr)) {
	    CITY_WEIGHT = dflt_cityweight(country, city_ptr);
	    CADJWEIGHT;
	    set_weights(FALSE);
	  }
	  sct_ptr = shold_ptr;
	  sct_ptr->designation = olddesg;

	} else {
	  /* have to build a new city... what happened? */
	  goto buildnewcity;
	}

      }
    }

    /* record everything */
    sct_ptr = shold_ptr;
    if (is_god == FALSE) {

      /* spend the money */
      if (take_resources(XREAL, YREAL, cost_ptr, under_siege)) {
	free(cost_ptr);
	errormsg("You lack the resources to build that here");
	return(0);
      }

      TAKESTART;
      for (count = 0; count < MTRLS_NUMBER; count++) {
	if (cost_ptr->mtrls[count] > 0) {
	  global_long = cost_ptr->mtrls[count];
	  TAKEMTRLS(count);
	}
      }
      TAKEFINISH(under_siege);
      free(cost_ptr);
    }
      
    /* remove invalid minor designations */
    if (minor_desg(sct_ptr->designation,
		   MIN_DEVASTATED | maj_dinfo[value].info_val)) {
      /* maybe I should ask? Nyah! */
      sct_ptr->designation ^=
	minor_desg(sct_ptr->designation,
		   MIN_DEVASTATED | maj_dinfo[value].info_val);
    }
    set_majordesg(sct_ptr->designation, (uns_short) value);
    SADJDESG;

    /* assure the construction time delay */
    if ((is_god != TRUE) &&
	IS_CITY(value) &&
	((city_ptr = citybyloc(ntn_ptr, XREAL, YREAL)) != NULL)) {
      CITY_PEOPLE = -maj_dinfo[value].build_time;
      CADJPEOP;
      if (value == MAJ_CACHE) {
	CITY_FORT = 0;
	CADJFORT;
      }
    }

    /* new capital? */
    if (value == MAJ_CAPITAL) {

      /* adjust the old capital designation */
      if (citybyloc(ntn_ptr, ntn_ptr->capx, ntn_ptr->capy) != NULL) {

	/* quick shift to the old location */
	int tempx = XREAL, tempy = YREAL;
	xcurs = ((int) ntn_ptr->capx) - xoffset;
	ycurs = ((int) ntn_ptr->capy) - yoffset;
	set_majordesg(sct[XREAL][YREAL].designation, MAJ_CITY);
	SADJDESG;
	xcurs = tempx - xoffset;
	ycurs = tempy - yoffset;

      }

      /* now adjust things to the new location */
      if ((ntn_ptr->centerx == ntn_ptr->capx) &&
	  (ntn_ptr->centery == ntn_ptr->capy)) {
	ntn_ptr->centerx = XREAL;
	ntn_ptr->centery = YREAL;
	XADJRLOC;
      }
      ntn_ptr->capx = XREAL;
      ntn_ptr->capy = YREAL;
      XADJLOC;
    }
  }

  /* fix god settings */
  if (is_god == TRUE) {
    reset_god();
  }
  return(MOVECOST);
}

/*
 * construct - Minor designation construction interface
 *
 * Provides interactive interface for constructing minor designations (improvements)
 * on sectors including fortifications, roads, and other sector enhancements.
 * Handles cost calculation, resource validation, and construction execution
 * with proper game rule enforcement.
 *
 * The function manages all minor construction types:
 * - Fortifications: Defensive improvements that increase sector defense
 * - Roads: Transportation improvements that facilitate movement
 * - Other Improvements: Various minor enhancements available in the game
 * - Devastation: Special construction that clears all other improvements
 * - Undevastation: Removal of devastation status from sectors
 *
 * Construction Process:
 * - Validates sector ownership and accessibility
 * - Checks resource availability from nearby supply centers
 * - Displays available construction options with possibility indicators
 * - Calculates and displays construction costs (non-expert mode)
 * - Processes resource consumption and records construction
 * - Updates sector designation and related game systems
 *
 * Parameters:
 *   void - Uses global coordinates XREAL, YREAL for target sector
 *
 * Returns:
 *   int - MOVECOST on successful construction, 0 on cancellation or error
 *
 * Side Effects:
 *   - Modifies sector minor designations
 *   - Consumes resources from nearby supply centers
 *   - Updates fortification levels for cities
 *   - Records construction activities for game persistence
 *   - May clear all existing improvements (devastation)
 *   - Temporarily switches god mode context if operating as administrator
 *
 * Notes:
 *   - Enforces siege restrictions preventing construction in besieged sectors
 *   - Validates resource availability before allowing construction
 *   - God mode bypasses resource requirements and ownership restrictions
 *   - Devastation requires confirmation as it eliminates other improvements
 *   - Fortification construction increases city fort values up to maximum
 *   - Special handling for "Undevastate" option when sector is devastated
 *   - Cost display and confirmation can be bypassed in expert mode
 */
int
construct PARM_0(void)
{
  ITEM_PTR tp_ptr;
  char outbuf[LINELTH];
  int xloc, yloc, can_do = FALSE;
  int value, count, high_item, is_sieged = FALSE;

  /* first check if it is possible to adjust the sector */
  if (!XY_ONMAP(XREAL, YREAL) ||
      ((is_god == FALSE) && (sct[XREAL][YREAL].owner != country))) {
	
    if (is_god == TRUE)
      errormsg("That sector is off the map");
    else errormsg("That sector is not owned by you");
    return(0);

  }

  /* check current sector information */
  sct_ptr = &(sct[XREAL][YREAL]);
  if (minor_desg(sct_ptr->designation, MIN_SIEGED)) {
    is_sieged = TRUE;
  }

  /* check the owner setting */
  if (is_god == TRUE) {

    /* does he want to play */
    if (get_god(sct_ptr->owner, TRUE)) {
      return(0);
    }

  } else if (is_sieged == TRUE) {

    /* no constructions possible */
    errormsg("It is not possible to construct in a sector under siege");
    return(0);

  } else {

    /* now check if any resources are nearby */
    if ((tp_ptr = find_resources(XREAL, YREAL, is_sieged)) == NULL) {
      errormsg("There are no resources available to this sector");
      return(0);
    }
    free(tp_ptr);

  }

  /* list possible designations for the sector */
  xloc = 23;
  yloc = LINES - 4;
  clear_bottom(0);
  mvaddstr(yloc, 0, "Possible Construction: ");
  for (count = 0; count < MIN_NUMBER; count++) {

    /* don't bother showing these for normal people */
    if ((is_god == FALSE) &&
	(((MIN_START << count) == MIN_SIEGED) ||
	 ((MIN_START << count) == MIN_FORSALE))) {
      continue;
    }

    /* to comma or not to comma */
    if (count > 0) {
      addstr(", ");
      xloc += 2;
    }

    /* check if it is okay */
    if (mindesg_ok(XREAL, YREAL, count, FALSE)) {

      /* can do it! */
      high_item = TRUE;
      can_do = TRUE;

    } else {

      /* can't do it :-( */
      high_item = FALSE;

    }

    /* show show it */
    if (((MIN_START << count) == MIN_DEVASTATED) &&
	(minor_desg(sct_ptr->designation, MIN_DEVASTATED))) {
      strcpy(string, "Undevastate");
    } else {
      strcpy(string, min_desg_selects[count]);
    }

    /* check positioning */
    if (xloc + (int)strlen(string) > COLS - 8) {
      yloc++;
      xloc = 5;
      move(yloc, xloc);
    }

    /* now show it */
    hip_string(string, 1, high_item);
    xloc += strlen(string) + 2;

  }

  /* nothing set */
  if (can_do == FALSE) {
    errormsg("You are not able to construct anything here right now");
    return(0);
  }

  /* ask for selection */
  mvaddstr(++yloc, 0, "Enter your selection: ");
  refresh();
  value = next_char();
  if ((value == ' ') || (value == '\n') || (value == '\r')) {
    if (is_god == TRUE) reset_god();
    return(0);
  }
  if (islower(value)) {
    value = toupper(value);
  }

  /* check for the selection */
  for (count = 0; count < MIN_NUMBER; count++) {
    if (value == min_desg_selects[count][0]) break;
  }
  if (value == 'U') {
    if (!minor_desg(sct_ptr->designation, MIN_DEVASTATED)) {
      errormsg("That was not a possible selection");
      if (is_god == TRUE) reset_god();
      return(0);
    } else {
      count = 0;
    }
  }

  /* check input */
  if (count == MIN_NUMBER) {
    errormsg("That was not among your choices");
    if (is_god == TRUE) reset_god();
    return(0);
  }

  /* check if it is already there */
  if (!mindesg_ok(XREAL, YREAL, count, TRUE)) {
    if (is_god == TRUE) reset_god();
    return(0);
  }
  value = MIN_START << count;

  /* let them know what devestated will do */
  if ((value == MIN_DEVASTATED) &&
      !minor_desg(sct_ptr->designation, MIN_DEVASTATED)) {
    bottommsg("This will eliminate all prior constructions. Continue? ");
    if (y_or_n() == FALSE) {
      if (is_god == TRUE) reset_god();
      return(0);
    }
  }

  /* subtract costs */
  if (is_god == FALSE) {
    ITEM_PTR cost_ptr;

    /* figure out the costs */
    if ((cost_ptr = mindesg_costs(count, XREAL, YREAL, TRUE)) == NULL) {
      return(0);
    }

    /* query them */
    if (conq_expert == FALSE) {
      strcpy(outbuf, "Cost:");
      for (count = 0; count < MTRLS_NUMBER; count++) {
	if (cost_ptr->mtrls[count] > 0) {
	  sprintf(string, " %.0f %s",
		  (double) cost_ptr->mtrls[count],
		  mtrls_info[count].name);
	  strcat(outbuf, string);
	}
      }
      strcat(outbuf, "; Continue? ");
      bottommsg(outbuf);
      if (y_or_n() == FALSE) {
	free(cost_ptr);
	return(0);
      }
    }

    /* check it */
    if (take_resources(XREAL, YREAL, cost_ptr, is_sieged)) {
      free(cost_ptr);
      errormsg("You do not have the resources to construct that here");
      return(0);
    }

    /* record everything */
    TAKESTART;
    for (count = 0; count < MTRLS_NUMBER; count++) {
      if (cost_ptr->mtrls[count] > 0) {
	global_long = cost_ptr->mtrls[count];
	TAKEMTRLS(count);
      }
    }
    TAKEFINISH(is_sieged);
    free(cost_ptr);
  }

  /* now provide the designation */
  if (((value != MIN_FORTIFIED) &&
       (value != MIN_DEVASTATED)) ||
      ((value != MIN_DEVASTATED) &&
       !minor_desg(sct_ptr->designation, value))) {
    set_minordesg(sct_ptr->designation, value);
    SADJDESG;
  } else if (!minor_desg(sct_ptr->designation, value)) {
    /* devastate it */
    clr_minordesg(sct_ptr->designation, 0xFFF0);
    set_minordesg(sct_ptr->designation, value);
    SADJDESG;
  } else if (value == MIN_DEVASTATED) {
    clr_minordesg(sct_ptr->designation, value);
    SADJDESG;
  } else if ((city_ptr = citybyloc(ntn_ptr, XREAL, YREAL)) != NULL) {
    if (CITY_FORT < MAXFORTVAL) {
      CITY_FORT++;
      CADJFORT;
    }
  } else {
    errormsg("Okay, now what did you do with that town?");
  }

  /* close up shop */
  if (is_god == TRUE) {
    reset_god();
  }
  return(MOVECOST);
}

/*
 * sect_info - Comprehensive sector information display
 *
 * Provides detailed information display for the current sector including
 * ownership, designations, production/consumption data, and various sector
 * statistics. Handles visibility restrictions and displays appropriate
 * information based on player access level and magical detection abilities.
 *
 * Information Categories Displayed:
 * - Ownership: Nation name and sector control information
 * - Designations: Major designation (city name, type) and minor improvements
 * - Statistics: Attraction, weights, defense values, and supply range
 * - City Details: Recruits/construction status, fortification, materials
 * - Production: Resource production rates based on sector configuration
 * - Consumption: Resource consumption requirements for the sector
 * - Workers: Population-based worker estimates for non-city sectors
 *
 * Visibility System:
 * - Full visibility for owned sectors and god mode
 * - Partial visibility based on reconnaissance level
 * - Magic detection affects city visibility (wizardry powers)
 * - Hidden cache cities and void-protected sectors
 * - Production/consumption visible only for owned or fully visible sectors
 *
 * Display Layout:
 * - Right side of screen (SCREEN_MIDDLE to COLS)
 * - Structured information with highlighting for important elements
 * - Production and consumption in columnar format
 * - Construction status and recruitment information
 * - Material stores for supply centers
 *
 * Parameters:
 *   void - Uses global coordinates XREAL, YREAL for target sector
 *
 * Returns:
 *   void - Function purely displays information, no return value
 *
 * Side Effects:
 *   - Displays information on the right side of the game screen
 *   - Temporarily switches nation context for production calculations
 *   - Uses standout mode for highlighting important information
 *   - May reveal or hide information based on magical detection
 *
 * Notes:
 *   - Returns early if sector not visible or off-map
 *   - Handles unowned sectors and nation lookup safely
 *   - Production/consumption calculated using sector owner's context
 *   - City information requires proper city lookup for accuracy
 *   - Magic detection (MW_SEEALL) affects information availability
 *   - Cache cities hidden unless proper detection available
 *   - Support level affects construction/devastation status display
 */
void
sect_info PARM_0(void)
{
  SHEET_STRUCT sval_info;
  SCT_PTR s1_ptr;
  NTN_PTR n1_ptr, nptr_hold = ntn_ptr;
  CITY_PTR c1_ptr;
  int ctry_hold = country, dval, width, seeit;
  int sown, line, col_val, count, h_on;

  /* first check if it anything is visable */
  if (!XY_ONMAP(XREAL, YREAL) ||
      ((sct[XREAL][YREAL].owner != country) &&
       (VIS_CHECK(XREAL, YREAL) <= HS_SEEPART))) {
    return;
  }

  /* initialize sector information */
  s1_ptr = &(sct[XREAL][YREAL]);
  sown = s1_ptr->owner;
  n1_ptr = world.np[sown];
  col_val = COLS - SCREEN_MIDDLE;
  h_on = FALSE;
  line = 0;

  /* determine the designation, if any */
  dval = major_desg(s1_ptr->designation);
  c1_ptr = NULL;
  if (IS_CITY(dval)) {
    country = sown;
    ntn_ptr = world.np[sown];
    c1_ptr = citybyloc(ntn_ptr, XREAL, YREAL);
    country = ctry_hold;
    ntn_ptr = nptr_hold;
  }

  /* now determine the visibility */
  if ((dval == MAJ_NONE) ||
      ((sown != country) &&
       (is_god == FALSE) &&
       !WIZ_MAGIC(MW_SEEALL) &&
       (MAGIC((world.np[sown])->powers[MAG_WIZARDRY], MW_THEVOID) ||
	(dval == MAJ_CACHE)))) {
    seeit = FALSE;
  } else {
    seeit = TRUE;
  }

  /* show the name of the owner, if any */
  move(line++, col_val);
  if (n1_ptr == NULL) {
    addstr("Unowned ");
  } else if ((c1_ptr == NULL) ||
	     (seeit == FALSE)) {
    h_on = TRUE;
    standout();
    printw("%s", n1_ptr->name);
    if (seeit == TRUE) {
      addch(' ');
    }
  } else {
    h_on = TRUE;
  }

  /* now show the sector title */
  if ((c1_ptr != NULL) &&
      (seeit == TRUE)) {
    standout();
    printw("%s of %s", maj_dinfo[dval].name, c1_ptr->name);
    if ((sown == country) ||
	(is_god == TRUE)) {
      printw(" [%d]", c1_ptr->weight);
    }
  } else if (seeit == TRUE) {
    addstr(maj_dinfo[dval].name);
    if (((is_god == TRUE) || (sown == country)) &&
	(count = s_uselevel(s1_ptr)) != 0) {
      printw(" [%d]", count);
    }
  }
  if (h_on) standend();
  line++;

  /* show the minor designations */
  if ((sown != country) &&
      (VIS_CHECK(XREAL, YREAL) < HS_SEEFULL)) {
    width = 0;
  } else {
    width = (SCREEN_MIDDLE - SCREEN_RIGHT) / 2 + 1;
  }
  h_on = FALSE;
  standout();
  if ((sown != UNOWNED) &&
      ((sown == country) ||
       (country == UNOWNED)) &&
      (SUM_WEIGHTS(XREAL, YREAL) == 0)) {
    if (minor_desg(s1_ptr->designation, MIN_DEVASTATED)) {
      strcpy(string, "=NoSpt/Dvst=");
    } else {
      strcpy(string, "=No=Support=");
    }
  } else if (minor_desg(s1_ptr->designation, MIN_DEVASTATED)) {
    strcpy(string, "*Devastated*");
  } else {
    strcpy(string, "Construction");
  }
  mvaddstr(line++, col_val + width, string);
  standend();
  if ((seeit == TRUE) ||
      (sown == country) ||
      (is_god == TRUE)) {
    for (count = 0; count < MIN_NUMBER; count++) {
      if ((MIN_START << count) == MIN_DEVASTATED) continue;
      if (minor_desg(s1_ptr->designation, MIN_START << count)) {
	mvaddstr(line++, col_val + width, min_dinfo[count].name);
	h_on = TRUE;
      }
    }
  }
  if (h_on == FALSE) {
    mvaddstr(line++, col_val + width, "none");
  }
  
  /* provide detailed sector information; if visible */
  if (((seeit == FALSE) &&
       (sown != country) &&
       (is_god == FALSE)) ||
      ((sown == UNOWNED) ||
       ((sown != country) &&
	(VIS_CHECK(XREAL, YREAL) < HS_SEEFULL)))) return;

  /* show the attraction, fortification, etc. */
  line = 2;
  mvprintw(line++, col_val, "Attract: %d",
	   attract_val(XREAL, YREAL));
  if ((count = SUM_WEIGHTS(XREAL, YREAL)) > 0) {
    mvprintw(line++, col_val, "Weights: %d", count);
  }
  if ((count = fort_val(XREAL, YREAL)) > 0) {
    mvprintw(line++, col_val, "Defense: %d%%", count);
  }
  if (c1_ptr != NULL) {

    /* show supply center specifics */
    if (c1_ptr->i_people >= 0) {
      mvprintw(line++, col_val, "Recruits: %d",
	       c1_ptr->i_people);
    } else {
      mvprintw(line++, col_val, "Build: %d mths", - c1_ptr->i_people);
    }
    count = r10_region(n1_ptr, c1_ptr, s1_ptr);
    mvprintw(line++, col_val, "Range: %d.%d",
	     count / 10, count % 10);

    /* now show the materials */
    for (count = 0; count < MTRLS_NUMBER; count++) {
      mvprintw(line++, col_val, "%s: %.0f",
	       mtrls_info[count].name,
	       (double) c1_ptr->c_mtrls[count]);
    }

  } else {
    /* display worker estimate */
    mvprintw(line++, col_val, "Workers: %ld",
	     poptoworkers(s1_ptr->people));
  }

  /* now generate production and consumption info */
  line = 13;
  for (count = 0; count < MTRLS_NUMBER; count++) {
    mvaddstr(line + count + 1, col_val, mtrls_info[count].name);
  }
  width = (SCREEN_MIDDLE - SCREEN_RIGHT) / 3 - 2;

  /* production */
  country = sown;
  ntn_ptr = world.np[sown];
  sector_produce(XREAL, YREAL, &sval_info);
  adjust_production(&sval_info, FALSE);
  country = ctry_hold;
  ntn_ptr = nptr_hold;
  standout();
  mvaddstr(line, col_val + width, "Produce");
  standend();
  for (count = 0; count < MTRLS_NUMBER; count++) {
    mvprintw(line + count + 1, col_val + width, "%.0f",
	     (double) sval_info.mtrls[count]);
  }

  /* consumption */
  width = (2 * (SCREEN_MIDDLE - SCREEN_RIGHT)) / 3 - 2;
  country = sown;
  ntn_ptr = world.np[sown];
  sector_consume(XREAL, YREAL, &sval_info);
  country = ctry_hold;
  ntn_ptr = nptr_hold;
  standout();
  mvaddstr(line, col_val + width, "Consume");
  standend();
  for (count = 0; count < MTRLS_NUMBER; count++) {
    mvprintw(line + count + 1, col_val + width, "%.0f",
	     (double) sval_info.mtrls[count]);
  }
}
