/* mechanism to transfer items between storage sites */
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
#include "xferG.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "statusX.h"
#include "keyvalsX.h"
#define USE_CODES
#define PRINT_CODES
#include "armyX.h"
#include "cityX.h"
#include "navyX.h"
#include "worldX.h"
#include "caravanX.h"
#include "dstatusX.h"

/* Declare the structures used for storage in these routines */
static XFER_STRUCT xfer_sites[3];

/*
 * sct2xfer - Copy sector information into transfer slot for exchange operations
 *
 * Initializes a transfer slot with sector data, setting up capacity limits
 * and available resources based on sector ownership and food production.
 * Only owned sectors with positive food production allow transfers.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1, represents left or right side)
 *   xloc - X coordinate of the sector on the world map
 *   yloc - Y coordinate of the sector on the world map
 *
 * Returns:
 *   void - No return value, modifies global xfer_sites array
 *
 * Side Effects:
 *   - Modifies xfer_sites[slot] structure with sector information
 *   - Sets transfer limits based on sector ownership and food production
 *   - Initializes material arrays to zero
 *   - Sets site classification to XFER_SECTOR
 *
 * Notes:
 *   - Performs bounds checking on slot index and map coordinates
 *   - Only sectors owned by current player with food production > 0 allow transfers
 *   - Unowned or foreign sectors have zero transfer capacity
 *   - Materials are initialized to zero as sectors don't store raw materials
 */
void
sct2xfer PARM_3(int, slot, int, xloc, int, yloc)
{
  XFER_PTR xfer_ptr;
  SCT_PTR s1_ptr;
  int i, dval;

  /* reality check */
  if ((slot < 0) || (slot > 1) ||
      !XY_ONMAP(xloc, yloc)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* set the description */
  xfer_ptr->site_class = XFER_SECTOR;
  s1_ptr = &(sct[xloc][yloc]);
  dval = major_desg(s1_ptr->designation);
  sprintf(xfer_ptr->description, "%s [%d,%d]",
	  (dval == MAJ_NONE) ? "Sector" : maj_dinfo[dval].name,
	  xloc_relative(xloc), yloc_relative(yloc));
  xfer_ptr->xloc = xloc;
  xfer_ptr->yloc = yloc;

  /* check the ownership */
  if ((s1_ptr->owner == country) &&
      (tofood(s1_ptr, country) > 0)) {
    xfer_ptr->civ_max = XF_NOLIMIT;
    xfer_ptr->crew_max = XF_NOLIMIT;
    xfer_ptr->onb_cmax = XF_NOLIMIT;
    xfer_ptr->civies = s1_ptr->people;
    xfer_ptr->crews = 0;
    xfer_ptr->civ_div = 1;
    xfer_ptr->crew_div = 1;
  } else {
    xfer_ptr->civ_max = 0;
    xfer_ptr->crew_max = 0;
    xfer_ptr->civ_div = 0;
    xfer_ptr->crew_div = 0;
    xfer_ptr->civies = 0;
    xfer_ptr->crews = 0;
    xfer_ptr->onb_cmax = 0;
  }
  xfer_ptr->onb_id = EMPTY_HOLD;
  xfer_ptr->onb_cid = EMPTY_HOLD;
  xfer_ptr->onb_max = XF_NOLIMIT;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    xfer_ptr->mtrls[i] = 0;
  }
  xfer_ptr->mtrl_max = 0;
}

/*
 * xfer2sct - Return transferred population to sector after transfer operations
 *
 * Transfers population changes back to the sector, updating the sector's
 * population count based on the difference between transfer slot and
 * original sector values. Only processes owned sectors.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) to read changes from
 *   xloc - X coordinate of the target sector
 *   yloc - Y coordinate of the target sector
 *
 * Returns:
 *   void - No return value, modifies sector data
 *
 * Side Effects:
 *   - Updates sector population if ownership allows
 *   - Temporarily changes cursor position for sector update macros
 *   - Calls SADJPEOP macro to notify of population changes
 *   - May display error message for invalid site class
 *
 * Notes:
 *   - Validates site class must be XFER_SECTOR before proceeding
 *   - Only processes sectors owned by current country
 *   - Uses temporary cursor position changes for proper macro operation
 *   - Population difference calculated as (crews + civies) - current people
 */
void
xfer2sct PARM_3(int, slot, int, xloc, int, yloc)
{
  XFER_PTR xfer_ptr;
  SCT_PTR s1_ptr;
  long num_people;

  /* reality check */
  if ((slot < 0) || (slot > 1) ||
      !XY_ONMAP(xloc, yloc)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* check the ownership */
  if (xfer_ptr->site_class != XFER_SECTOR) {
    errormsg("ERROR:  This stuff doesn't belong in a sector");
    return;
  }
  s1_ptr = &(sct[xloc][yloc]);
  if (s1_ptr->owner == country) {
    num_people = (xfer_ptr->crews + xfer_ptr->civies) - s1_ptr->people;
    if (num_people != 0) {

      /* transfer the sector location */
      int oxloc = XREAL;
      int oyloc = YREAL;
      xcurs = xloc - xoffset;
      ycurs = yloc - yoffset;

      /* place the difference into the sector */
      sct[XREAL][YREAL].people += num_people;
      SADJPEOP;

      /* replace the sector location */
      xcurs = oxloc - xoffset;
      ycurs = oyloc - yoffset;

    }

  }
}

/*
 * city2xfer - Copy city information into transfer slot for resource exchange
 *
 * Initializes a transfer slot with city data including population,
 * materials, and capacity limits. Sets up transfer constraints based
 * on city attraction value and sector population.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) for the transfer operation
 *   c1_ptr - Pointer to city structure to copy data from (must not be NULL)
 *
 * Returns:
 *   void - No return value, modifies global xfer_sites array
 *
 * Side Effects:
 *   - Modifies xfer_sites[slot] structure with city information
 *   - Sets transfer limits based on city attraction value
 *   - Copies all city materials to transfer slot
 *   - Sets site classification to XFER_CITY
 *
 * Notes:
 *   - Performs bounds checking on slot index and NULL pointer validation
 *   - City description combines major designation name with city name
 *   - Transfer limits depend on attract_val() - positive allows unlimited transfers
 *   - Civilians come from sector population, crew from city i_people
 *   - Materials copied from city's i_mtrls array
 *   - Zero attraction cities have no transfer capacity
 */
void
city2xfer PARM_2(int, slot, CITY_PTR, c1_ptr)
{
  XFER_PTR xfer_ptr;
  SCT_PTR s1_ptr;
  int i, dval;

  /* reality check */
  if ((slot < 0) || (slot > 1) || (c1_ptr == NULL)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* set the description */
  s1_ptr = &(sct[c1_ptr->xloc][c1_ptr->yloc]);
  dval = major_desg(s1_ptr->designation);
  sprintf(xfer_ptr->description, "%s %s",
	  maj_dinfo[dval].name, c1_ptr->name);
  xfer_ptr->xloc = c1_ptr->xloc;
  xfer_ptr->yloc = c1_ptr->yloc;

  /* build it up */
  xfer_ptr->site_class = XFER_CITY;
  if (s1_ptr->people > 0) {
    xfer_ptr->civies = s1_ptr->people;
    if (c1_ptr->i_people > 0) {
      xfer_ptr->crews = c1_ptr->i_people;
    } else {
      xfer_ptr->crews = 0;
    }
  } else {
    xfer_ptr->civies = 0;
    xfer_ptr->crews = 0;
  }
  for (i = 0; i < MTRLS_NUMBER; i++) {
    xfer_ptr->mtrls[i] = c1_ptr->i_mtrls[i];
  }
  xfer_ptr->onb_id = EMPTY_HOLD;
  xfer_ptr->onb_cid = EMPTY_HOLD;
  xfer_ptr->mtrl_max = XF_NOLIMIT;
  xfer_ptr->onb_max = XF_NOLIMIT;
  xfer_ptr->onb_cmax = XF_NOLIMIT;

  /* check the limits for civilians and crew */
  if (attract_val(c1_ptr->xloc, c1_ptr->yloc) > 0) {
    xfer_ptr->civ_max = XF_NOLIMIT;
    xfer_ptr->civ_div = -1;
    xfer_ptr->crew_max = XF_JOINED;
    xfer_ptr->crew_div = -1;
  } else {
    xfer_ptr->civ_max = 0;
    xfer_ptr->civ_div = 0;
    xfer_ptr->crew_max = 0;
    xfer_ptr->crew_div = 0;
  }
}

/*
 * xfer2city - Apply transfer results back to city structure
 *
 * Updates city and sector data with transfer results, handling population
 * and material changes. Manages complex interactions between city crew,
 * sector civilians, and material storage.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) containing the changes
 *   c1_ptr - Pointer to city structure to update (must not be NULL)
 *
 * Returns:
 *   void - No return value, modifies city and sector data
 *
 * Side Effects:
 *   - Updates city population (i_people) and materials (i_mtrls)
 *   - Updates sector population through temporary cursor changes
 *   - Calls adjustment macros (CADJPEOP, CADJIMTRLS, CADJMTRLS, SADJPEOP)
 *   - Temporarily changes global city_ptr for macro operations
 *   - May display error message for invalid site class
 *
 * Notes:
 *   - Validates site class must be XFER_CITY before proceeding
 *   - Complex population management balances crew and civilian transfers
 *   - Material transfers handle both positive and negative differences
 *   - Negative material transfers affect city inventory first, then sector storage
 *   - Uses temporary cursor positioning for proper sector update operations
 *   - Restores original city_ptr before returning
 */
void
xfer2city PARM_2(int, slot, CITY_PTR, c1_ptr)
{
  XFER_PTR xfer_ptr;
  SCT_PTR s1_ptr;
  CITY_PTR chold_ptr = city_ptr;
  long num_people, crew_shift = 0;
  itemtype xch_mtrl;
  int i;

  /* reality check */
  if ((slot < 0) || (slot > 1) || (c1_ptr == NULL)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* configuration check */
  if (xfer_ptr->site_class != XFER_CITY) {
    errormsg("ERROR:  This stuff doesn't belong in a city");
    return;
  }
  city_ptr = c1_ptr;
  s1_ptr = &(sct[CITY_XLOC][CITY_YLOC]);

  /* transfer civilians */
  if (CITY_PEOPLE > 0) {
    if (CITY_PEOPLE != xfer_ptr->crews) {
      crew_shift = xfer_ptr->crews - CITY_PEOPLE;
      CITY_PEOPLE += crew_shift;
      CADJPEOP;
      if (CITY_PEOPLE < 0) {
	CITY_PEOPLE = 0;
      }
      CADJPEOP;
    } else {
      crew_shift = 0;
    }
  } else {
    crew_shift = 0;
  }
  num_people = xfer_ptr->civies - s1_ptr->people;
  if (num_people != 0) {

    /* transfer the sector location */
    int oxloc = XREAL;
    int oyloc = YREAL;
    xcurs = CITY_XLOC - xoffset;
    ycurs = CITY_YLOC - yoffset;

    /* place the difference into the sector */
    sct[XREAL][YREAL].people += num_people;
    SADJPEOP;

    /* replace the sector location */
    xcurs = oxloc - xoffset;
    ycurs = oyloc - yoffset;

    /* check the adjustment */
    if ((CITY_PEOPLE >= 0) &&
	(num_people - crew_shift < 0)) {
      CITY_PEOPLE += (num_people - crew_shift);
      if (CITY_PEOPLE < 0) {
	CITY_PEOPLE = 0;
      }
      CADJPEOP;
    }

  }

  /* transfer the materials */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    xch_mtrl = xfer_ptr->mtrls[i] - CITY_IMTRLS[i];
    if (xch_mtrl < 0) {
      CITY_IMTRLS[i] = xfer_ptr->mtrls[i];
      if (CITY_IMTRLS[i] < (itemtype) 0) {
	CITY_IMTRLS[i] = (itemtype) 0;
      }
      CADJIMTRLS(i);
      CITY_MTRLS[i] += xch_mtrl;
      CADJMTRLS(i);
    } else {
      CITY_MTRLS[i] += xch_mtrl;
      CADJMTRLS(i);
    }
  }

  /* fix before leaving */
  city_ptr = chold_ptr;
}

/*
 * navy2xfer - Copy naval fleet information into transfer slot
 *
 * Initializes a transfer slot with naval fleet data including ship capacities,
 * onboard units, materials, and population. Calculates capacity limits based
 * on different ship types (galleys, merchants, warships, barges).
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) for the transfer operation
 *   n1_ptr - Pointer to navy structure to copy data from (must not be NULL)
 *
 * Returns:
 *   void - No return value, modifies global xfer_sites array
 *
 * Side Effects:
 *   - Modifies xfer_sites[slot] structure with naval fleet information
 *   - Sets capacity limits based on ship types and counts
 *   - Copies fleet materials and onboard unit IDs
 *   - Sets site classification to XFER_NAVY
 *
 * Notes:
 *   - Performs bounds checking on slot index and NULL pointer validation
 *   - Civilian capacity based on galley ships and their people capacity
 *   - Crew capacity calculated from all ship types minus required crew
 *   - Material capacity from merchant ships only
 *   - Army capacity from warship cargo space
 *   - Caravan capacity from barge cargo space
 *   - Different ship types have different cargo specializations
 */
void
navy2xfer PARM_2(int, slot, NAVY_PTR, n1_ptr)
{
  XFER_PTR xfer_ptr;
  int i;

  /* reality check */
  if ((slot < 0) || (slot > 1) || (n1_ptr == NULL)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* set the description */
  sprintf(xfer_ptr->description, "Naval Fleet %d", n1_ptr->navyid);
  xfer_ptr->xloc = n1_ptr->xloc;
  xfer_ptr->yloc = n1_ptr->yloc;

  /* now file the information */
  xfer_ptr->site_class = XFER_NAVY;

  /* set the current storage */
  xfer_ptr->civies = (long) n1_ptr->people *
    navy_holds(n1_ptr, NSHP_GALLEYS);
  xfer_ptr->onb_id = n1_ptr->armynum;
  xfer_ptr->onb_cid = n1_ptr->cvnnum;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    xfer_ptr->mtrls[i] = n1_ptr->mtrls[i];
  }

  /* set it up */
  if (n1_ptr->crew > 0) {
    xfer_ptr->crews = (long) 0;
    for (i = 0; i < NSHP_NUMBER; i++) {
      xfer_ptr->crews += (long) (n1_ptr->crew - 1) *
	navy_holds(n1_ptr, i);
    }
  } else {
    xfer_ptr->crews = 0;
  }

  /* set the max values */
  xfer_ptr->mtrl_max = ((long) ninfo_list[NSHP_MERCHANTS].capacity *
			navy_holds(n1_ptr, NSHP_MERCHANTS));
  xfer_ptr->civ_div = (int) navy_holds(n1_ptr, NSHP_GALLEYS);
  xfer_ptr->civ_max = ((long) ninfo_list[NSHP_GALLEYS].capacity *
		       xfer_ptr->civ_div) / 1000;
  xfer_ptr->onb_max = ((long) ninfo_list[NSHP_WARSHIPS].capacity *
		       navy_holds(n1_ptr, NSHP_WARSHIPS)) / 1000;
  xfer_ptr->onb_cmax = ((long) ninfo_list[NSHP_BARGES].capacity *
			navy_holds(n1_ptr, NSHP_BARGES)) / 10000;
  xfer_ptr->crew_div = (int) 0;
  for (i = 0; i < NSHP_NUMBER; i++) {
    xfer_ptr->crew_div += (int) navy_holds(n1_ptr, i);
  }
  xfer_ptr->crew_max = ((long) (MAXNAVYCREW - (n1_ptr->crew > 0)) *
			xfer_ptr->crew_div);
}

/*
 * xfer2navy - Apply transfer results back to naval fleet structure
 *
 * Updates naval fleet data with transfer results including onboard armies,
 * caravans, materials, civilians, and crew. Handles capacity calculations
 * based on ship types and validates changes.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) containing the changes
 *   n1_ptr - Pointer to navy structure to update (must not be NULL)
 *
 * Returns:
 *   void - No return value, modifies navy data
 *
 * Side Effects:
 *   - Updates navy army and caravan assignments
 *   - Updates navy materials, population, and crew levels
 *   - Calls adjustment macros (NADJARMY, NADJCVN, NMTRLS, NADJPEOP, NADJCREW)
 *   - Temporarily changes global navy_ptr for macro operations
 *   - May display error message for invalid site class
 *
 * Notes:
 *   - Validates site class must be XFER_NAVY before proceeding
 *   - Army and caravan IDs updated if they differ from transfer slot
 *   - Material updates applied individually for each material type
 *   - Civilian count calculated based on galley capacity
 *   - Crew count calculated across all ship types with proper crew requirements
 *   - Restores original navy_ptr before returning
 */
void
xfer2navy PARM_2(int, slot, NAVY_PTR, n1_ptr)
{
  NAVY_PTR nhold_ptr = navy_ptr;
  XFER_PTR xfer_ptr;
  int i, j;

  /* reality check */
  if ((slot < 0) || (slot > 1) || (n1_ptr == NULL)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* configuration check */
  if (xfer_ptr->site_class != XFER_NAVY) {
    errormsg("ERROR:  This stuff doesn't belong on a fleet");
    return;
  }
  navy_ptr = n1_ptr;

  /* check the army storage */
  if (NAVY_ARMY != xfer_ptr->onb_id) {
    NAVY_ARMY = xfer_ptr->onb_id;
    NADJARMY;
  }
  if (NAVY_CVN != xfer_ptr->onb_cid) {
    NAVY_CVN = xfer_ptr->onb_cid;
    NADJCVN;
  }

  /* check the materials onboard */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (xfer_ptr->mtrls[i] != NAVY_MTRLS[i]) {
      NAVY_MTRLS[i] = xfer_ptr->mtrls[i];
      NMTRLS(i);
    }
  }

  /* check the civilians */
  if ((i = navy_holds(navy_ptr, NSHP_GALLEYS)) > 0) {
    i = (int) (xfer_ptr->civies / i);
    if (i != (int) NAVY_PEOP) {
      NAVY_PEOP = (uns_char) i;
      NADJPEOP;
    }
  }

  /* check the crew */
  i = 0;
  for (j = 0; j < NSHP_NUMBER; j++) {
    i += navy_holds(navy_ptr, j);
  }
  if (i > 0) {
    i = (int) (xfer_ptr->crews / i);
    if (i != ((int) NAVY_CREW + (NAVY_CREW > 0))) {
      NAVY_CREW = (uns_char) (i + (NAVY_CREW > 0));
      NADJCREW;
    }
  }

  /* reset before bugging out */
  navy_ptr = nhold_ptr;
}

/*
 * cvn2xfer - Copy caravan information into transfer slot
 *
 * Initializes a transfer slot with caravan data including size, population,
 * crew, and materials. Sets up capacity limits based on caravan size
 * and standard caravan carrying capacities.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) for the transfer operation
 *   v1_ptr - Pointer to caravan structure to copy data from (must not be NULL)
 *
 * Returns:
 *   void - No return value, modifies global xfer_sites array
 *
 * Side Effects:
 *   - Modifies xfer_sites[slot] structure with caravan information
 *   - Sets capacity limits based on caravan size
 *   - Copies caravan materials and population data
 *   - Sets site classification to XFER_CARAVAN
 *
 * Notes:
 *   - Performs bounds checking on slot index and NULL pointer validation
 *   - Capacity calculations multiply base values by caravan size
 *   - Civilian capacity set to XF_JOINED (special joined capacity mode)
 *   - Material capacity based on CVN_HOLD constant times caravan size
 *   - Crew capacity based on MAXCVNCREW minus current crew requirement
 *   - Caravans cannot carry armies or other caravans (onb_max = 0)
 */
void
cvn2xfer PARM_2(int, slot, CVN_PTR, v1_ptr)
{
  XFER_PTR xfer_ptr;
  int i;

  /* reality check */
  if ((slot < 0) || (slot > 1) || (v1_ptr == NULL)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* set the description */
  sprintf(xfer_ptr->description, "Caravan %d", v1_ptr->cvnid);
  xfer_ptr->xloc = v1_ptr->xloc;
  xfer_ptr->yloc = v1_ptr->yloc;

  /* now file the information */
  xfer_ptr->site_class = XFER_CARAVAN;

  /* set the current storage */
  xfer_ptr->civies = ((long) v1_ptr->people) * v1_ptr->size;
  if (v1_ptr->crew > 0) {
    xfer_ptr->crews = ((long) (v1_ptr->crew - 1)) * v1_ptr->size;
  } else {
    xfer_ptr->crews = 0;
  }
  xfer_ptr->onb_id = EMPTY_HOLD;
  xfer_ptr->onb_cid = EMPTY_HOLD;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    xfer_ptr->mtrls[i] = v1_ptr->mtrls[i];
  }

  /* set the max values */
  xfer_ptr->mtrl_max = ((long) CVN_HOLD) * v1_ptr->size;
  xfer_ptr->civ_max = XF_JOINED;
  xfer_ptr->civ_div = (int) v1_ptr->size;
  xfer_ptr->crew_max = ((long) (MAXCVNCREW - (v1_ptr->crew > 0)) *
			v1_ptr->size);
  xfer_ptr->crew_div = (int) v1_ptr->size;
  xfer_ptr->onb_max = 0;
  xfer_ptr->onb_cmax = 0;
}

/*
 * xfer2cvn - Apply transfer results back to caravan structure
 *
 * Updates caravan data with transfer results including materials,
 * civilians, and crew. Calculates per-unit values based on caravan
 * size and validates changes.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) containing the changes
 *   v1_ptr - Pointer to caravan structure to update (must not be NULL)
 *
 * Returns:
 *   void - No return value, modifies caravan data
 *
 * Side Effects:
 *   - Updates caravan materials, population, and crew levels
 *   - Calls adjustment macros (VMTRLS, VADJPEOP, VADJCREW)
 *   - Temporarily changes global cvn_ptr for macro operations
 *   - May display error message for invalid site class
 *
 * Notes:
 *   - Validates site class must be XFER_CARAVAN before proceeding
 *   - Material updates applied individually for each material type
 *   - Population and crew calculated as totals divided by caravan size
 *   - Crew calculation includes adjustment for existing crew requirements
 *   - All calculations account for caravan size scaling
 *   - Restores original cvn_ptr before returning
 */
void
xfer2cvn PARM_2(int, slot, CVN_PTR, v1_ptr)
{
  CVN_PTR vhold_ptr = cvn_ptr;
  XFER_PTR xfer_ptr;
  int i;

  /* reality check */
  if ((slot < 0) || (slot > 1) || (v1_ptr == NULL)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* configuration check */
  if (xfer_ptr->site_class != XFER_CARAVAN) {
    errormsg("ERROR:  This stuff doesn't belong on a caravan");
    return;
  }
  cvn_ptr = v1_ptr;

  /* check the materials onboard */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (xfer_ptr->mtrls[i] != CVN_MTRLS[i]) {
      CVN_MTRLS[i] = xfer_ptr->mtrls[i];
      VMTRLS(i);
    }
  }

  /* check the civilians */
  if (CVN_SIZE > 0) {
    i = (int) (xfer_ptr->civies / CVN_SIZE);
    if (i != (int) CVN_PEOP) {
      CVN_PEOP = (uns_char) i;
      VADJPEOP;
    }
  }

  /* check the crew */
  if (CVN_SIZE > 0) {
    i = (int) (xfer_ptr->crews / CVN_SIZE);
    if (i != ((int) CVN_CREW + (CVN_CREW > 0))) {
      CVN_CREW = (uns_char) (i + (CVN_CREW > 0));
      VADJCREW;
    }
  }

  /* reset before buzzing out */
  cvn_ptr = vhold_ptr;
}

/* === From here down, the code is for the interactive transfer mode === */

/* declaration of keybindings at bottom */
extern KBIND_STRUCT xfer_klist[];
extern PARSE_STRUCT xfer_funcs[];
extern KEYSYS_STRUCT xfer_keysys;
KLIST_PTR xfer_bindings = NULL;

/* various storage variables */
static char xfer_indicator[3] = "<>", xfer_error[LINELTH];
static long xfer_xchanges[MTRLS_NUMBER + 2];
static ARMY_PTR xfer_armylist;
static long xfer_civ_units, xfer_crew_units;
static long xfer_took_off;
static int xfer_do_army, xfer_do_mtrls, xfer_do_cvn;
static int xfer_selection, xfer_direction;
static int xf_max_items, xf_min_army;
static int xf_sel_crew, xf_sel_civ, xf_sel_cvn;
static int xfer_aslot[2], xfer_vslot[2];
static int xfer_done;

/*
 * xfer_init - Initialize transfer mode configuration and interface
 *
 * Sets up the transfer interface by configuring key bindings, calculating
 * item selection limits, and initializing transfer amounts. Determines
 * which transfer categories are available based on site capabilities.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0 (success)
 *
 * Side Effects:
 *   - Calls align_xfer_keys() to set up key bindings
 *   - Sets global variables for maximum items and selection indices
 *   - Initializes transfer amount arrays with default values
 *   - Locates army list for current transfer location
 *
 * Notes:
 *   - Calculates selection indices for civilians, crew, caravans, and armies
 *   - Default material transfer amounts set to 1000 units
 *   - Civilian and crew transfer amounts based on divisor units
 *   - Army list filtered to current location coordinates
 *   - Transfer amounts preserved between sessions unless incompatible
 *   - Maximum items limited by screen space (LINES - XF_USEDLINES)
 */
int
xfer_init PARM_0(void)
{
  static int hd_xchanges = FALSE;
  int i;

  /* check the bindings */
  align_xfer_keys();

  /* calculate the edges */
  if (xfer_do_mtrls == TRUE) {
    xf_max_items = MTRLS_NUMBER;
  } else {
    xf_max_items = 0;
  }
  if (xfer_civ_units > 0) {
    xf_sel_civ = xf_max_items;
    xf_max_items++;
  } else {
    xf_sel_civ = -1;
  }
  if (xfer_crew_units > 0) {
    xf_sel_crew = xf_max_items;
    xf_max_items++;
  } else {
    xf_sel_crew = -1;
  }
  if (xfer_do_cvn == TRUE) {
    xf_sel_cvn = xf_max_items;
    xf_max_items++;
  } else {
    xf_sel_cvn = -1;
  }
  xf_min_army = xf_max_items;
  if (xfer_do_army == TRUE) {
    for (xfer_armylist = ntn_ptr->army_list;
	 xfer_armylist != NULL;
	 xfer_armylist = xfer_armylist->next) {
      if ((xfer_armylist->xloc == xfer_sites[0].xloc) &&
	  (xfer_armylist->yloc == xfer_sites[0].yloc)) {
	break;
      }
    }
    xf_max_items = LINES - XF_USEDLINES;
  } else {
    xfer_armylist = NULL;
  }

  /* set the minimum transfer values */
  if (hd_xchanges == FALSE) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      xfer_xchanges[i] = 1000;
    }
    xfer_xchanges[i++] = xfer_civ_units;
    xfer_xchanges[i++] = xfer_crew_units;
    hd_xchanges = TRUE;
  } else {
    if ((xfer_civ_units > 0) &&
	((xfer_xchanges[MTRLS_NUMBER] <= 0) ||
	 (xfer_xchanges[MTRLS_NUMBER] % xfer_civ_units != 0))) {
      xfer_xchanges[MTRLS_NUMBER] = xfer_civ_units;
    }
    if ((xfer_crew_units > 0) &&
	((xfer_xchanges[MTRLS_NUMBER + 1] <= 0) ||
	 (xfer_xchanges[MTRLS_NUMBER + 1] % xfer_crew_units != 0))) {
      xfer_xchanges[MTRLS_NUMBER + 1] = xfer_crew_units;
    }
  }
  return(0);
}

/*
 * xfer_unlimited - Check if transfer selection has unlimited storage capacity
 *
 * Determines whether a specific transfer selection has unlimited storage
 * capacity, which affects transfer limits and user interface display.
 * Different selection types have different unlimited conditions.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) to check
 *   select_num - Selection index within the transfer interface
 *
 * Returns:
 *   int - TRUE if selection has unlimited capacity, FALSE otherwise
 *
 * Side Effects:
 *   - None - read-only function
 *
 * Notes:
 *   - Army selections: unlimited if onb_max == XF_NOLIMIT
 *   - Caravan selections: unlimited if onb_cmax == XF_NOLIMIT
 *   - Civilian selections: unlimited if civ_max == XF_NOLIMIT or combined with unlimited materials
 *   - Crew selections: unlimited if crew_max == XF_NOLIMIT or XF_JOINED
 *   - Material selections: unlimited if mtrl_max == XF_NOLIMIT
 *   - Performs bounds checking on slot and selection parameters
 *   - XF_JOINED indicates crew integrated with civilian capacity
 */
static int
xfer_unlimited PARM_2(int, slot, int, select_num)
{
  if ((slot < 0) ||
      (slot > 1) ||
      (select_num < 0) ||
      (select_num >= xf_max_items)) return(FALSE);

  /* check the army selection */
  if (select_num >= xf_min_army) {
    if (xfer_sites[slot].onb_max == XF_NOLIMIT) {
      return(TRUE);
    } else {
      return(FALSE);
    }
  }

  /* check the selection for caravans */
  if (select_num == xf_sel_cvn) {
    if (xfer_sites[slot].onb_cmax == XF_NOLIMIT) {
      return(TRUE);
    } else {
      return(FALSE);
    }
  }

  /* check the materials selection */
  if (select_num == xf_sel_civ) {
    if ((xfer_sites[slot].civ_max == XF_NOLIMIT) ||
	((xfer_sites[slot].civ_max == XF_JOINED) &&
	 (xfer_sites[slot].mtrl_max == XF_NOLIMIT))) {
      return(TRUE);
    }
  } else if (select_num == xf_sel_crew) {
    if ((xfer_sites[slot].crew_max == XF_NOLIMIT) ||
	(xfer_sites[slot].crew_max == XF_JOINED)) {
      /* joined is always unlimited */
      return(TRUE);
    }
  } else {
    if (xfer_sites[slot].mtrl_max == XF_NOLIMIT) {
      return(TRUE);
    }
  }

  /* must be false */
  return(FALSE);
}

/*
 * xfer_space - Calculate available storage space for transfer selection
 *
 * Computes the remaining storage capacity for a specific transfer selection,
 * accounting for current loads and capacity limits. Handles different
 * calculation methods for various selection types.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) to check capacity
 *   select_num - Selection index within the transfer interface
 *
 * Returns:
 *   long - Available storage space in appropriate units, 0 if invalid
 *
 * Side Effects:
 *   - None - read-only function that performs calculations
 *
 * Notes:
 *   - Returns BIGINT for unlimited capacity selections
 *   - Army space: capacity minus current army load
 *   - Caravan space: capacity minus current caravan load
 *   - Crew space: maximum crew minus current crew
 *   - Combined capacity: accounts for civilians and materials together
 *   - Material space: capacity minus current material load, adjusted by weight
 *   - Performs bounds checking on slot and selection parameters
 *   - Space calculations use appropriate load calculation functions
 */
static long
xfer_space PARM_2(int, slot, int, select_num)
{
  long hold = 0;

  /* check the selection */
  if ((slot < 0) ||
      (slot > 1) ||
      (select_num < 0) ||
      (select_num >= xf_max_items)) return(0);

  /* check for a major return */
  if (xfer_unlimited(slot, select_num)) {
    return(BIGINT);
  }

  /* check for army space available */
  if (select_num >= xf_min_army) {
    /* check the army selection */
    hold = xfer_sites[slot].onb_max;
    if (xfer_sites[slot].onb_id != EMPTY_HOLD) {
      hold -= army_load(armybynum(xfer_sites[slot].onb_id));
    }
  } else if (select_num == xf_sel_cvn) {
    /* check the army selection */
    hold = xfer_sites[slot].onb_cmax;
    if (xfer_sites[slot].onb_cid != EMPTY_HOLD) {
      hold -= cvn_load(cvnbynum(xfer_sites[slot].onb_cid));
    }
  } else if (select_num == xf_sel_crew) {
    /* check the crew space */
    hold = xfer_sites[slot].crew_max;
    hold -= xfer_sites[slot].crews;
  } else if (xfer_sites[slot].civ_max == XF_JOINED) {
    /* combined civilians and raw materials */
    hold = xfer_sites[slot].mtrl_max;
    hold -= mtrls_load(&(xfer_sites[slot].mtrls[0]));
    hold -= (xfer_sites[slot].civies * 1000);
    if (select_num == xf_sel_civ) {
      hold /= 1000;
    } else {
      hold /= mtrls_info[select_num].weight;
    }
  } else if (select_num == xf_sel_civ) {
    /* only civilians */
    hold = xfer_sites[slot].civ_max;
    hold -= xfer_sites[slot].civies;
  } else {
    /* only raw materials */
    hold = xfer_sites[slot].mtrl_max;
    hold -= mtrls_load(&(xfer_sites[slot].mtrls[0]));
    hold /= mtrls_info[select_num].weight;
  }

  /* done */
  return(hold);
}

/*
 * xfer_armyptr - Get pointer to specific army unit in transfer context
 *
 * Locates and returns a pointer to a specific army unit based on the
 * slot and selection index. Handles both onboard armies and armies
 * in the same location that can be transferred.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) to search within
 *   which - Army selection index within the transfer interface
 *
 * Returns:
 *   ARMY_PTR - Pointer to the requested army, NULL if not found or invalid
 *
 * Side Effects:
 *   - None - read-only function that searches army lists
 *
 * Notes:
 *   - Adjusts selection index by subtracting xf_min_army offset
 *   - For onboard armies: returns leader army if which == 0
 *   - For location armies: searches through nearby army list
 *   - Skips armies already onboard the opposite transfer site
 *   - Uses army leader relationships to find grouped units
 *   - Accounts for army slot offset (xfer_aslot) for scrolling
 *   - Performs bounds checking on slot parameter
 */
static ARMY_PTR
xfer_armyptr PARM_2(int, slot, int, which)
{
  int cnt = 0, target;
  ARMY_PTR a1_ptr;

  /* check input */
  if ((slot < 0) ||
      (slot > 1)) {
    return((ARMY_PTR) NULL);
  }

  /* check the selection */
  which -= xf_min_army;

  /* determine the target */
  target = xfer_aslot[slot] + which;

  /* check the slot */
  if (xfer_sites[slot].onb_id == EMPTY_HOLD) {
    if (xfer_sites[slot].onb_max >= 0) {
      return((ARMY_PTR) NULL);
    }
  } else {
    if (which == 0) {
      /* return the shipboard leader */
      return(armybynum(xfer_sites[slot].onb_id));
    }
    target--;
  }

  /* try to locate the given army */
  for (a1_ptr = xfer_armylist;
       a1_ptr != NULL;
       a1_ptr = a1_ptr->nrby) {

    /* is this the leaderbum? */
    if (xfer_sites[slot].onb_id == a1_ptr->armyid) {
      continue;
    }

    /* check if the bloke is onboard this bugger */
    if (xfer_sites[slot].onb_id == army_shipleader(a1_ptr)) {
      if (++cnt > target) break;
    }

  }

  /* done */
  return(a1_ptr);
}

/*
 * xfer_cvnptr - Get pointer to caravan in transfer context
 *
 * Locates and returns a pointer to the caravan associated with the
 * specified transfer slot. Handles both onboard caravans and caravans
 * in the same location.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) to get caravan from
 *
 * Returns:
 *   CVN_PTR - Pointer to the caravan, NULL if not found or invalid
 *
 * Side Effects:
 *   - None - read-only function that searches caravan lists
 *
 * Notes:
 *   - First checks for caravan already onboard (onb_cid)
 *   - If no onboard caravan, searches location for available caravans
 *   - Skips caravans already assigned to the opposite transfer site
 *   - Uses caravan slot offset (xfer_vslot) for selection
 *   - Searches through nation's caravan list at matching coordinates
 *   - Performs bounds checking on slot parameter
 *   - Returns NULL if slot cannot carry caravans (onb_cmax >= 0)
 */
static CVN_PTR
xfer_cvnptr PARM_1(int, slot)
{
  int cnt = 0, target, noton;
  CVN_PTR v1_ptr;

  /* check input */
  if ((slot < 0) ||
      (slot > 1)) {
    return((CVN_PTR) NULL);
  }

  /* check if the item carries caravans */
  if (xfer_sites[slot].onb_cid != EMPTY_HOLD) {
    /* got it */
    return(cvnbynum(xfer_sites[slot].onb_cid));
  } else if (xfer_sites[slot].onb_cmax >= 0) {
    return((CVN_PTR) NULL);
  }

  /* try to locate the given caravan */
  target = xfer_vslot[slot];
  noton = xfer_sites[(slot + 1) % 1].onb_cid;
  for (v1_ptr = ntn_ptr->cvn_list;
       v1_ptr != NULL;
       v1_ptr = v1_ptr->next) {

    /* check if the bloke is in this sector */
    if ((v1_ptr->xloc == xfer_sites[slot].xloc) &&
	(v1_ptr->yloc == xfer_sites[slot].yloc)) {

      /* skip over any already on the other side */
      if (noton == v1_ptr->cvnid) continue;
      if (++cnt > target) break;
    }

  }

  /* done */
  return(v1_ptr);
}

/*
 * xfer_maxarmy - Count army units available for transfer at site
 *
 * Counts the total number of army units that can be transferred from
 * the specified site, including both onboard and location-based armies.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) to count armies for
 *
 * Returns:
 *   int - Number of army units available for transfer, 0 if none
 *
 * Side Effects:
 *   - None - read-only function that counts armies
 *
 * Notes:
 *   - Returns 0 if site cannot carry armies or has no army leader
 *   - Counts armies that have the same ship leader as the site
 *   - Uses army_shipleader() to determine army grouping
 *   - Searches through the global army list (xfer_armylist)
 *   - Count used for interface scrolling and bounds checking
 *   - Performs bounds checking on slot parameter
 */
static int
xfer_maxarmy PARM_1(int, slot)
{
  ARMY_PTR a1_ptr;
  int cnt = 0;

  /* check input */
  if ((slot < 0) || (slot > 1) ||
      ((xfer_sites[slot].onb_max >= 0) &&
       (xfer_sites[slot].onb_id == EMPTY_HOLD))) {
    return(0);
  }

  /* count up the units in the given site */
  for (a1_ptr = xfer_armylist;
       a1_ptr != NULL;
       a1_ptr = a1_ptr->nrby) {
    /* at this site? */
    if (xfer_sites[slot].onb_id == army_shipleader(a1_ptr)) {
      cnt++;
    }
  }
  return(cnt);
}

/*
 * xfer_maxcvn - Count caravans available for transfer at site
 *
 * Counts the total number of caravans that can be transferred from
 * the specified site location, excluding caravans already assigned
 * to the opposite transfer site.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) to count caravans for
 *
 * Returns:
 *   int - Number of caravans available for transfer, 0 if none
 *
 * Side Effects:
 *   - None - read-only function that counts caravans
 *
 * Notes:
 *   - Returns 0 if site cannot carry caravans (onb_cmax >= 0)
 *   - Counts caravans at the same location coordinates as the site
 *   - Excludes caravans already assigned to opposite transfer site
 *   - Searches through nation's complete caravan list
 *   - Count used for interface scrolling and selection validation
 *   - Performs bounds checking on slot parameter
 */
static int
xfer_maxcvn PARM_1(int, slot)
{
  int cnt = 0, noton;
  CVN_PTR v1_ptr;

  /* check input */
  if ((slot < 0) ||
      (slot > 1)) {
    return(cnt);
  }

  /* check if the item carries caravans */
  if (xfer_sites[slot].onb_cmax >= 0) {
    return(cnt);
  }
  noton = xfer_sites[(slot + 1) % 1].onb_cid;
  for (v1_ptr = ntn_ptr->cvn_list;
       v1_ptr != NULL;
       v1_ptr = v1_ptr->next) {

    /* check if the bloke is in this sector */
    if ((v1_ptr->xloc == xfer_sites[slot].xloc) &&
	(v1_ptr->yloc == xfer_sites[slot].yloc)) {

      /* skip over any already on the other side */
      if (noton == v1_ptr->cvnid) continue;
      cnt++;
    }

  }
  return(cnt);
}

/*
 * xfer_quit - Exit transfer mode with user confirmation
 *
 * Prompts the user to confirm they want to exit transfer mode and
 * sets the completion flag if confirmed. Provides a safety check
 * to prevent accidental exit.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Displays confirmation prompt on bottom line
 *   - Sets xfer_done global flag to TRUE if user confirms
 *   - Clears end of line after prompt display
 *
 * Notes:
 *   - Uses y_or_n() function for user confirmation
 *   - Confirmation prevents accidental loss of transfer setup
 *   - Transfer changes are applied when mode exits regardless
 *   - Part of the transfer mode command interface
 */
static int
xfer_quit PARM_0(void)
{
  mvaddstr(LINES - 1, 0, "Are you done transfering items? ");
  clrtoeol();
  if (y_or_n()) {
    xfer_done = TRUE;
  }
  return(0);
}

/*
 * xfer_down - Move transfer selection cursor downward
 *
 * Moves the transfer interface selection cursor down one position
 * in the item list. Provides audio feedback if already at bottom.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Increments xfer_selection global variable
 *   - Calls beep() if already at maximum selection
 *
 * Notes:
 *   - Selection bounded by xf_max_items limit
 *   - Used for navigating through materials, civilians, crew, armies
 *   - Part of the transfer mode navigation interface
 *   - Audio feedback prevents confusion at interface boundaries
 */
static int
xfer_down PARM_0(void)
{
  /* move down if possible */
  if (xfer_selection < xf_max_items - 1) {
    xfer_selection++;
  } else {
    beep();
  }
  return(0);
}

/*
 * xfer_up - Move transfer selection cursor upward
 *
 * Moves the transfer interface selection cursor up one position
 * in the item list. Provides audio feedback if already at top.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Decrements xfer_selection global variable
 *   - Calls beep() if already at minimum selection (0)
 *
 * Notes:
 *   - Selection bounded by 0 minimum
 *   - Used for navigating through materials, civilians, crew, armies
 *   - Part of the transfer mode navigation interface
 *   - Audio feedback prevents confusion at interface boundaries
 */
static int
xfer_up PARM_0(void)
{
  /* move up if possible */
  if (xfer_selection > 0) {
    xfer_selection--;
  } else {
    beep();
  }
  return(0);
}

/*
 * xfer_xfer - Execute the actual transfer of items between sites
 *
 * Performs the core transfer operation moving armies, caravans, crew,
 * civilians, or materials between transfer sites. Handles complex
 * validation, capacity checking, and state management.
 *
 * Parameters:
 *   all_over - If TRUE, transfer all available items; if FALSE, transfer set amount
 *
 * Returns:
 *   void - No return value, modifies transfer site data
 *
 * Side Effects:
 *   - Modifies army status, leadership, and grouping
 *   - Updates navy and caravan assignments and status
 *   - Changes population and material quantities in transfer sites
 *   - May display error messages for invalid operations
 *   - Updates movement points and status for affected units
 *
 * Notes:
 *   - Handles army boarding/disembarking with diplomatic restrictions
 *   - Manages army leadership and grouping relationships
 *   - Processes caravan loading/unloading with status changes
 *   - Transfers civilians and crew with capacity validation
 *   - Moves materials with weight and space constraints
 *   - Complex logic for different transfer directions and unit types
 *   - Validates ownership and diplomatic status for movements
 */
static void
xfer_xfer PARM_1(int, all_over)
{
  ARMY_PTR a1_ptr, a2_ptr;
  CVN_PTR v1_ptr, v2_ptr;
  long value;
  int sown;

  /* switch based on the type of the material */
  if (xfer_selection >= xf_min_army) {

    /* is there an army here? */
    if ((a1_ptr = xfer_armyptr(xfer_direction ^ 1, xfer_selection))
	== NULL) {
      beep();
      return;
    }

    /* now find out if it fits */
    if (!xfer_unlimited(xfer_direction, xfer_selection) &&
	(army_load(a1_ptr) >
	 xfer_space(xfer_direction, xfer_selection))) {
      strcpy(xfer_error, "That army does not fit onboard");
      return;
    }

    /* does it need to be unloaded? */
    army_ptr = a1_ptr;
    if (xfer_unlimited(xfer_direction, xfer_selection)) {

      /* needs to be unloaded from the ship */
      if (real_stat(army_ptr) == ST_ONBOARD) {

	/* can it be unloaded? */
	sown = sct[ARMY_XLOC][ARMY_YLOC].owner;
	if ((sown != country) &&
	    (is_god == FALSE)) {
	  if (sown == UNOWNED) {
	    /* can it get there from here? */
	    if (!a_isleader(ARMY_TYPE) &&
		!a_mayunload(ARMY_TYPE)) {
	      errormsg("That army may not disembark in unowned land");
	      return;
	    }
	  } else if ((world.np[sown])->dstatus[country] %
		     dstatus_number != DIP_ALLIED) {
	    /* how about from here? */
	    if (!a_assault(ARMY_TYPE)) {
	      strcpy(xfer_error,
		     "That unit may not disembark in someone else's land");
	      return;
	    } else if (ntn_ptr->dstatus[sown] % dstatus_number < DIP_WAR) {
	      strcpy(xfer_error,
		     "You must declare war before invading another nation");
	      return;
	    }
	  }
	}
	
	/* unload it */
	if (!a_isscout(ARMY_TYPE) &&
	    minor_desg(sct[ARMY_XLOC][ARMY_YLOC].designation, MIN_SIEGED)) {
	  set_status(ARMY_STAT, ST_SIEGED);
	} else {
	  set_status(ARMY_STAT, ST_DEFEND);
	}
	if (unit_speed(ARMY_STAT) == SPD_STUCK) {
	  set_speed(ARMY_STAT, SPD_NORMAL);
	}
	AADJSTAT;

	/* align the speed properly */
	if (ARMY_MOVE > NAVY_MOVE) {
	  ARMY_MOVE = NAVY_MOVE;
	  if (unit_leading(ARMY_STAT)) {
	    set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
	    GADJMOVE;
	  } else {
	    AADJMOVE;
	  }
	} else if (NAVY_MOVE > ARMY_MOVE) {
	  NAVY_MOVE = ARMY_MOVE;
	  NADJMOVE;
	}

	/* check for any scouts on board */
	if (unit_leading(ARMY_STAT)) {
	  ungroup_scouts(ARMY_ID);
	}
      }

      /* now undo earlier ship */
      if (xfer_sites[xfer_direction ^ 1].onb_id == ARMY_ID) {
	xfer_sites[xfer_direction ^ 1].onb_id = EMPTY_HOLD;
      }

    } else {

      /* needs to be loaded onto the ship */
      if ((xfer_sites[xfer_direction].onb_id == EMPTY_HOLD) ||
	  ((a2_ptr = armybynum(xfer_sites[xfer_direction].onb_id))
	   == NULL)) {

	/* easy load up */
	xfer_sites[xfer_direction].onb_id = ARMY_ID;
	if (unit_status(ARMY_STAT) != ST_ONBOARD) {
	  set_status(ARMY_STAT, ST_ONBOARD);
	  AADJSTAT;
	  if (ARMY_MOVE > NAVY_MOVE) {
	    ARMY_MOVE = NAVY_MOVE;
	    if (unit_leading(ARMY_STAT)) {
	      set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
	      GADJMOVE;
	    } else {
	      AADJMOVE;
	    }
	  } else if (NAVY_MOVE > ARMY_MOVE) {
	    NAVY_MOVE = ARMY_MOVE;
	    NADJMOVE;
	  }
	}

      } else if (a_isleader(a2_ptr->unittype)) {

	/* group under the leader */
	if (unit_status(ARMY_STAT) != ST_GROUPED) {
	  set_status(ARMY_STAT, ST_GROUPED);
	  AADJSTAT;
	}
	if (ARMY_LEAD != a2_ptr->armyid) {
	  ARMY_LEAD = a2_ptr->armyid;
	  AADJLEAD;
	}
	if (ARMY_MOVE > NAVY_MOVE) {
	  ARMY_MOVE = NAVY_MOVE;
	  if (unit_leading(ARMY_STAT)) {
	    set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
	    GADJMOVE;
	  } else {
	    AADJMOVE;
	  }
	} else if (NAVY_MOVE > ARMY_MOVE) {
	  NAVY_MOVE = ARMY_MOVE;
	  NADJMOVE;
	}

	/* now check the leadership setting */
	if (!unit_leading(a2_ptr->status)) {
	  army_ptr = a2_ptr;
	  set_leading(ARMY_STAT);
	  AADJSTAT;
	}

      } else if (a_isleader(ARMY_TYPE)) {

	/* have the leader board the ship */
	xfer_sites[xfer_direction].onb_id = ARMY_ID;
	set_status(ARMY_STAT, ST_ONBOARD);
	if (!unit_leading(ARMY_STAT)) {
	  set_leading(ARMY_STAT);
	}
	AADJSTAT;
	if (ARMY_MOVE > NAVY_MOVE) {
	  ARMY_MOVE = NAVY_MOVE;
	  if (unit_leading(ARMY_STAT)) {
	    set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
	    GADJMOVE;
	  } else {
	    AADJMOVE;
	  }
	} else if (NAVY_MOVE > ARMY_MOVE) {
	  NAVY_MOVE = ARMY_MOVE;
	  NADJMOVE;
	}

	/* now group the first bugger under this guy */
	if (ARMY_ID != a2_ptr->leader) {
	  a2_ptr->leader = ARMY_ID;
	  army_ptr = a2_ptr;
	  AADJLEAD;
	}
	army_ptr = a2_ptr;
	set_status(ARMY_STAT, ST_GROUPED);
	AADJSTAT;

      } else {

	/* can't board this ship */
	strcpy(xfer_error, "This unit must be lead onto the ship");
	return;

      }

      /* now undo earlier ship */
      if (xfer_sites[xfer_direction ^ 1].onb_id == a1_ptr->armyid) {
	xfer_sites[xfer_direction ^ 1].onb_id = EMPTY_HOLD;
      }

    }

  } else if (xfer_selection == xf_sel_cvn) {

    /* is there a caravan here? */
    if ((v1_ptr = xfer_cvnptr(xfer_direction ^ 1)) == NULL) {
      beep();
      return;
    }

    /* now find out if it fits */
    if (!xfer_unlimited(xfer_direction, xfer_selection) &&
	(cvn_load(v1_ptr) >
	 xfer_space(xfer_direction, xfer_selection))) {
      strcpy(xfer_error, "That caravan does not fit onboard");
      return;
    }

    /* does it need to be unloaded? */
    cvn_ptr = v1_ptr;
    if (xfer_unlimited(xfer_direction, xfer_selection)) {

      /* needs to be unloaded from the ship */
      if (onship_stat(CVN_STAT)) {
	sown = sct[CVN_XLOC][CVN_YLOC].owner;
	if ((is_god == FALSE) &&
	    (sown != country) &&
	    ((world.np[sown])->dstatus[country] % dstatus_number >=
	     DIP_NEUTRAL)) {
	  errormsg("You cannot unload a caravan in this sector");
	  return;
	}

	/* now fix the status */
	if (unit_status(CVN_STAT) == ST_ONBSPLY) {
	  set_status(CVN_STAT, ST_SUPPLY);
	} else if (unit_status(CVN_STAT) == ST_ONBSSPLY) {
	  set_status(CVN_STAT, ST_SSUPPLY);
	} else {
	  set_status(CVN_STAT, ST_CARRY);
	}
	VADJSTAT;

	/* align the movement properly */
	if (CVN_MOVE > NAVY_MOVE) {
	  CVN_MOVE = NAVY_MOVE;
	  VADJMOVE;
	} else if (NAVY_MOVE > CVN_MOVE) {
	  NAVY_MOVE = CVN_MOVE;
	  NADJMOVE;
	}

      }

      /* now undo earlier ship */
      if (xfer_sites[xfer_direction ^ 1].onb_cid == CVN_ID) {
	xfer_sites[xfer_direction ^ 1].onb_cid = EMPTY_HOLD;
      }

    } else {

      /* align the movement properly */
      if (CVN_MOVE > NAVY_MOVE) {
	CVN_MOVE = NAVY_MOVE;
	VADJMOVE;
      } else if (NAVY_MOVE > CVN_MOVE) {
	NAVY_MOVE = CVN_MOVE;
	NADJMOVE;
      }

      /* place it on this ship */
      if ((xfer_sites[xfer_direction].onb_cid == EMPTY_HOLD) ||
	  ((v2_ptr = cvnbynum(xfer_sites[xfer_direction].onb_cid)) == NULL)) {
	xfer_sites[xfer_direction].onb_cid = CVN_ID;
	if (!onship_stat(CVN_STAT)) {
	  if (unit_status(CVN_STAT) == ST_SUPPLY) {
	    set_status(CVN_STAT, ST_ONBSPLY);
	  } else if (unit_status(CVN_STAT) == ST_SSUPPLY) {
	    set_status(CVN_STAT, ST_ONBSSPLY);
	  } else {
	    set_status(CVN_STAT, ST_ONBOARD);
	  }
	  VADJSTAT;
	}
      } else {
	/* combine the two caravan units into one */
	if (v2_ptr->size + (int) CVN_SIZE >= 256) {
	  errormsg("You cannot combine those two caravans");
	  return;
	} else {
	  if (v2_ptr->efficiency != CVN_EFF) {
	    value = (long) v2_ptr->efficiency * v2_ptr->size +
	      (long) CVN_EFF * CVN_SIZE;
	    v2_ptr->efficiency = value / (v2_ptr->size + CVN_SIZE);
	  }
	  if (v2_ptr->crew != CVN_CREW) {
	    value = (long) v2_ptr->crew * v2_ptr->size +
	      (long) CVN_CREW * CVN_SIZE;
	    v2_ptr->crew = value / (v2_ptr->size + CVN_SIZE);
	  }
	  if (v2_ptr->people != CVN_PEOP) {
	    value = (long) v2_ptr->people * v2_ptr->size +
	      (long) CVN_PEOP * CVN_SIZE;
	    v2_ptr->people = value / (v2_ptr->size + CVN_SIZE);
	  }
	  v2_ptr->size += CVN_SIZE;
	  cvn_ptr = v2_ptr;
	  for (global_int = 0; global_int < MTRLS_NUMBER; global_int++) {
	    if (v1_ptr->mtrls[global_int] != 0) {
	      CVN_MTRLS[global_int] += v1_ptr->mtrls[global_int];
	      VMTRLS(global_int);
	    }
	  }
	  VADJEFF;
	  VADJPEOP;
	  VADJCREW;
	  VADJSIZE;
	  cvn_ptr = v1_ptr;
	  VDESTROY;
	  dest_cvn(CVN_ID);
	}
      }

      /* now undo earlier ship */
      if (xfer_sites[xfer_direction ^ 1].onb_cid == CVN_ID) {
	xfer_sites[xfer_direction ^ 1].onb_cid = EMPTY_HOLD;
      }

    }

  } else if (xfer_selection == xf_sel_crew) {

    /* crew adjustment */
    if (all_over) {
      value = min(xfer_space(xfer_direction, xfer_selection),
		  xfer_sites[xfer_direction ^ 1].crews);
    } else {
      value = min(xfer_space(xfer_direction, xfer_selection),
		  xfer_xchanges[MTRLS_NUMBER + 1]);
      value = min(value, xfer_sites[xfer_direction ^ 1].crews);
    }

    /* check */
    if (value <= 0) {
      strcpy(xfer_error, "You may not transfer any more crew");
      return;
    }

    /* do the addition */
    if (xfer_sites[xfer_direction].crew_max == XF_JOINED) {
      xfer_sites[xfer_direction].crews += value;
      xfer_sites[xfer_direction].civies += value;
    } else {
      xfer_sites[xfer_direction].crews += value;
    }

    /* do the subtraction */
    if (xfer_sites[xfer_direction ^ 1].crew_max == XF_JOINED) {
      xfer_sites[xfer_direction ^ 1].civies -= value;
      xfer_sites[xfer_direction ^ 1].crews -= value;
    } else {
      xfer_sites[xfer_direction ^ 1].crews -= value;
    }

  } else if (xfer_selection == xf_sel_civ) {

    /* civilian transportation */
    if (all_over) {
      value = min(xfer_space(xfer_direction, xfer_selection),
		  xfer_sites[xfer_direction ^ 1].civies);
    } else {
      value = min(xfer_space(xfer_direction, xfer_selection),
		  xfer_xchanges[MTRLS_NUMBER]);
      value = min(value, xfer_sites[xfer_direction ^ 1].civies);
    }

    /* check */
    if (value <= 0) {
      strcpy(xfer_error, "You may not transfer any more civilians");
      return;
    }

    /* make adjustments */
    xfer_sites[xfer_direction].civies += value;
    xfer_sites[xfer_direction ^ 1].civies -= value;

    /* do the addition */
    if (xfer_sites[xfer_direction].crew_max == XF_JOINED) {
      if (xfer_took_off > 0) {
	if (value > xfer_took_off) {
	  value = xfer_took_off;
	}
	xfer_sites[xfer_direction].crews += value;
	xfer_took_off -= value;
      }
    }

    /* do the subtraction */
    if (xfer_sites[xfer_direction ^ 1].crew_max == XF_JOINED) {
      if (xfer_sites[xfer_direction ^ 1].civies <
	  xfer_sites[xfer_direction ^ 1].crews) {
	xfer_took_off += min(value, xfer_sites[xfer_direction ^ 1].crews);
	xfer_sites[xfer_direction ^ 1].crews -=
	  min(value, xfer_sites[xfer_direction ^ 1].crews);
      }
    }

  } else {

    /* materials transfer */
    if (all_over) {
      value = min(xfer_space(xfer_direction, xfer_selection),
		  xfer_sites[xfer_direction ^ 1].mtrls[xfer_selection]);
    } else {
      value = min(xfer_space(xfer_direction, xfer_selection),
		  xfer_xchanges[xfer_selection]);
      value = min(value,
		  xfer_sites[xfer_direction ^ 1].mtrls[xfer_selection]);
    }

    /* check */
    if (value <= 0) {
      strcpy(xfer_error, "You may not transfer any more of that raw material");
      return;
    }

    /* do the exchange */
    xfer_sites[xfer_direction].mtrls[xfer_selection] += value;
    xfer_sites[xfer_direction ^ 1].mtrls[xfer_selection] -= value;

  }
}

/*
 * xfer_left - Transfer items to the left site or set left direction
 *
 * Sets the transfer direction to leftward (slot 0) or performs a
 * complete transfer if already pointing left. Handles direction
 * setting and all-items transfer functionality.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Sets xfer_direction to 0 (leftward)
 *   - Calls xfer_xfer(TRUE) if already pointing left
 *
 * Notes:
 *   - Two-stage operation: first set direction, second execute transfer
 *   - Transfer direction affects which site receives items
 *   - TRUE parameter to xfer_xfer means transfer all available items
 *   - Part of the transfer mode directional interface
 */
static int
xfer_left PARM_0(void)
{
  /* are we going this way already? */
  if (xfer_direction == 0) {
    /* shift everything over */
    xfer_xfer(TRUE);
  } else {
    /* then go this way */
    xfer_direction = 0;
  }
  return(0);
}

/*
 * xfer_right - Transfer items to the right site or set right direction
 *
 * Sets the transfer direction to rightward (slot 1) or performs a
 * complete transfer if already pointing right. Handles direction
 * setting and all-items transfer functionality.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Sets xfer_direction to 1 (rightward)
 *   - Calls xfer_xfer(TRUE) if already pointing right
 *
 * Notes:
 *   - Two-stage operation: first set direction, second execute transfer
 *   - Transfer direction affects which site receives items
 *   - TRUE parameter to xfer_xfer means transfer all available items
 *   - Part of the transfer mode directional interface
 */
static int
xfer_right PARM_0(void)
{
  /* are we going this way already? */
  if (xfer_direction == 1) {
    /* shift everything over */
    xfer_xfer(TRUE);
  } else {
    /* then go this way */
    xfer_direction = 1;
  }
  return(0);
}

/*
 * xfer_move - Transfer specified amount of items in current direction
 *
 * Executes a transfer of the currently configured amount of items
 * in the currently set direction. Uses the amount specified in
 * the transfer configuration rather than all available items.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Calls xfer_xfer(FALSE) to execute limited transfer
 *
 * Notes:
 *   - FALSE parameter means transfer only the configured amount
 *   - Transfer amount comes from xfer_xchanges array
 *   - Direction determined by current xfer_direction setting
 *   - Part of the transfer mode execution interface
 *   - Primary transfer command for precise quantity control
 */
static int
xfer_move PARM_0(void)
{
  /* just do it */
  xfer_xfer(FALSE);
  return(0);
}

/*
 * xfer_add - Increase transfer amount for current selection
 *
 * Increases the transfer amount for the currently selected item type.
 * Amount increment depends on item type (materials, civilians, crew).
 * Provides audio feedback if maximum limit reached.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Modifies xfer_xchanges array for current selection
 *   - Calls beep() if at maximum limit or invalid selection
 *
 * Notes:
 *   - Cannot modify army or caravan selections (single unit transfers)
 *   - Civilian increments by xfer_civ_units (divisor-based)
 *   - Crew increments by xfer_crew_units (divisor-based)
 *   - Material increments by 1 unit
 *   - Maximum limit is 1,000,000 units
 *   - Part of the transfer amount adjustment interface
 */
static int
xfer_add PARM_0(void)
{
  /* check the selection */
  if ((xfer_selection >= xf_min_army) ||
      (xfer_selection == xf_sel_cvn)) {
    beep();
  } else if (xfer_selection == xf_sel_civ) {
    /* simple addition */
    if (xfer_xchanges[MTRLS_NUMBER] >= 1000000) {
      beep();
    } else {
      xfer_xchanges[MTRLS_NUMBER] += xfer_civ_units;
    }
  } else if (xfer_selection == xf_sel_crew) {
    /* simple addition */
    if (xfer_xchanges[MTRLS_NUMBER + 1] >= 1000000) {
      beep();
    } else {
      xfer_xchanges[MTRLS_NUMBER + 1] += xfer_crew_units;
    }
  } else {
    /* simple addition */
    if (xfer_xchanges[xfer_selection] >= 1000000) {
      beep();
    } else {
      xfer_xchanges[xfer_selection]++;
    }
  }
  return(0);
}

/*
 * xfer_subtract - Decrease transfer amount for current selection
 *
 * Decreases the transfer amount for the currently selected item type.
 * Amount decrement depends on item type and minimum limits.
 * Provides audio feedback if minimum limit reached.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Modifies xfer_xchanges array for current selection
 *   - Calls beep() if at minimum limit or invalid selection
 *
 * Notes:
 *   - Cannot modify army or caravan selections
 *   - Civilian decrements by xfer_civ_units with minimum check
 *   - Crew decrements by xfer_crew_units with minimum check
 *   - Material decrements by 1 with minimum of 1
 *   - Prevents reduction below minimum viable transfer amounts
 *   - Part of the transfer amount adjustment interface
 */
static int
xfer_subtract PARM_0(void)
{
  /* check the selection */
  if ((xfer_selection >= xf_min_army) ||
      (xfer_selection == xf_sel_cvn)) {
    beep();
  } else if (xfer_selection == xf_sel_civ) {
    /* just subtract */
    if (xfer_xchanges[MTRLS_NUMBER] > xfer_civ_units) {
      xfer_xchanges[MTRLS_NUMBER] -= xfer_civ_units;
    } else {
      beep();
    }
  } else if (xfer_selection == xf_sel_crew) {
    /* just subtract */
    if (xfer_xchanges[MTRLS_NUMBER + 1] > xfer_crew_units) {
      xfer_xchanges[MTRLS_NUMBER + 1] -= xfer_crew_units;
    } else {
      beep();
    }
  } else {
    /* simple subtraction */
    if (xfer_xchanges[xfer_selection] > 1) {
      xfer_xchanges[xfer_selection]--;
    } else {
      beep();
    }
  }
  return(0);
}

/*
 * xfer_mult - Multiply transfer amount by 10 for current selection
 *
 * Multiplies the current transfer amount by 10 for quick adjustment
 * to larger quantities. Provides audio feedback if maximum limit
 * would be exceeded.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Modifies xfer_xchanges array for current selection
 *   - Calls beep() if operation would exceed maximum or invalid selection
 *
 * Notes:
 *   - Cannot modify army or caravan selections
 *   - Applies to civilians, crew, and material amounts
 *   - Maximum limit check prevents overflow (1,000,000)
 *   - Quick way to adjust to larger transfer quantities
 *   - Part of the transfer amount adjustment interface
 */
static int
xfer_mult PARM_0(void)
{
  /* check the selection */
  if ((xfer_selection >= xf_min_army) &&
      (xfer_selection == xf_sel_cvn)) {
    beep();
  } else if (xfer_selection == xf_sel_civ) {
    /* simple multiplication by 10 */
    if (xfer_xchanges[MTRLS_NUMBER] >= 1000000) {
      beep();
    } else {
      xfer_xchanges[MTRLS_NUMBER] *= 10;
    }
  } else if (xfer_selection == xf_sel_crew) {
    /* simple multiplication by 10 */
    if (xfer_xchanges[MTRLS_NUMBER + 1] >= 1000000) {
      beep();
    } else {
      xfer_xchanges[MTRLS_NUMBER + 1] *= 10;
    }
  } else {
    /* simple multiplication by 10 */
    if (xfer_xchanges[xfer_selection] >= 1000000) {
      beep();
    } else {
      xfer_xchanges[xfer_selection] *= 10;
    }
  }
  return(0);
}

/*
 * xfer_div - Divide transfer amount by 10 for current selection
 *
 * Divides the current transfer amount by 10 for quick adjustment
 * to smaller quantities. Maintains minimum limits and divisor
 * alignment for civilian and crew amounts.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Modifies xfer_xchanges array for current selection
 *   - Calls beep() if invalid selection
 *
 * Notes:
 *   - Cannot modify army or caravan selections
 *   - Civilian and crew amounts aligned to their divisor units
 *   - Material amounts maintain minimum of 1
 *   - Ensures divided amounts still meet minimum transfer requirements
 *   - Quick way to adjust to smaller transfer quantities
 *   - Part of the transfer amount adjustment interface
 */
static int
xfer_div PARM_0(void)
{
  int hold;

  /* check the selection */
  if ((xfer_selection >= xf_min_army) &&
      (xfer_selection == xf_sel_cvn)) {
    beep();
  } else if (xfer_selection == xf_sel_civ) {

    /* division by 10 and check for proper settings */
    hold = xfer_xchanges[MTRLS_NUMBER] / 10;
    if (hold < xfer_civ_units) {
      hold = xfer_civ_units;
    } else if ((hold % xfer_civ_units) != 0) {
      hold = (hold / xfer_civ_units) * xfer_civ_units;
    }
    xfer_xchanges[MTRLS_NUMBER] = hold;

  } else if (xfer_selection == xf_sel_crew) {

    /* division by 10 and check for proper settings */
    hold = xfer_xchanges[MTRLS_NUMBER + 1] / 10;
    if (hold < xfer_crew_units) {
      hold = xfer_crew_units;
    } else if ((hold % xfer_crew_units) != 0) {
      hold = (hold / xfer_crew_units) * xfer_crew_units;
    }
    xfer_xchanges[MTRLS_NUMBER + 1] = hold;

  } else {

    /* division by 10 and check for proper settings */
    hold = xfer_xchanges[xfer_selection] / 10;
    if (hold < 1) {
      hold = 1;
    }
    xfer_xchanges[xfer_selection] = hold;

  }
  return(0);
}

/*
 * xfer_assign - Set specific transfer amount for current selection
 *
 * Prompts the user to enter a specific transfer amount for the
 * currently selected item type. Validates input against minimum
 * limits and divisor requirements.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Displays input prompt on bottom line
 *   - Modifies xfer_xchanges array if valid input provided
 *   - May display error messages for invalid input
 *   - Calls clrtoeol() and get_number() for user input
 *
 * Notes:
 *   - Cannot modify army or caravan selections
 *   - Civilian amounts must be multiples of xfer_civ_units
 *   - Crew amounts must be multiples of xfer_crew_units
 *   - Material amounts must be greater than zero
 *   - Validates divisibility requirements for population types
 *   - Part of the transfer amount configuration interface
 */
static int
xfer_assign PARM_0(void)
{
  long value = 0;

  /* check selection */
  if ((xfer_selection >= xf_min_army) ||
      (xfer_selection == xf_sel_cvn)) {
    beep();
  } else if (xfer_selection == xf_sel_civ) {
    /* civilians setting */
    mvprintw(LINES - 1, 0,
	     "Set transfer amount to how many civilians [Mult of %ld] ? ",
	     xfer_civ_units);
    clrtoeol();
    value = get_number(FALSE);
    if (no_input == TRUE) return(0);
    if (value == 0) {
      errormsg("You may only set some amount greater than zero");
    } else if ((value % xfer_civ_units) != 0) {
      sprintf(string, "That was not divisible by %ld", xfer_civ_units);
      errormsg(string);
    } else {
      xfer_xchanges[MTRLS_NUMBER] = value;
    }
  } else if (xfer_selection == xf_sel_crew) {
    /* civilians setting */
    mvprintw(LINES - 1, 0,
	     "Set transfer amount to how much crew [Mult of %ld] ? ",
	     xfer_crew_units);
    clrtoeol();
    value = get_number(FALSE);
    if (no_input == TRUE) return(0);
    if (value == 0) {
      errormsg("You may only set some amount greater than zero");
    } else if ((value % xfer_crew_units) != 0) {
      sprintf(string, "That was not divisible by %ld", xfer_crew_units);
      errormsg(string);
    } else {
      xfer_xchanges[MTRLS_NUMBER + 1] = value;
    }
  } else {
    /* some other materials */
    mvprintw(LINES - 1, 0,
	     "Set transfer amount to how many %s? ",
	     mtrls_info[xfer_selection].lname);
    clrtoeol();
    value = get_number(FALSE);
    if (no_input == TRUE) return(0);
    if (value == 0) {
      errormsg("You may only set some amount greater than zero");
    } else {
      xfer_xchanges[xfer_selection] = value;
    }
  }
  return(0);
}

/*
 * xfer_shiftup - Scroll army or caravan list upward in interface
 *
 * Scrolls the display of army or caravan units upward to show
 * different units when more are available than can fit on screen.
 * Only works for army and caravan selections.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Modifies xfer_aslot or xfer_vslot arrays for scrolling
 *   - Calls beep() if at maximum scroll position or invalid selection
 *
 * Notes:
 *   - Only applies to army selections (>= xf_min_army) or caravan selection
 *   - Scroll position limited by maximum available units
 *   - Uses opposite slot direction for scrolling source list
 *   - Part of the unit list navigation interface
 *   - Provides access to units beyond initial display limit
 */
static int
xfer_shiftup PARM_0(void)
{
  int which_slot = xfer_direction ^ 1;

  /* simple check */
  if (xfer_selection >= xf_min_army) {
    if (xfer_aslot[which_slot] >= xfer_maxarmy(which_slot) - 1) {
      beep();
    } else {
      xfer_aslot[which_slot]++;
    }
  } else if (xfer_selection == xf_sel_cvn) {
    if (xfer_vslot[which_slot] >= xfer_maxcvn(which_slot) - 1) {
      beep();
    } else {
      xfer_vslot[which_slot]++;
    }
  } else {
    beep();
  }
  return(0);
}

/*
 * xfer_shiftdown - Scroll army or caravan list downward in interface
 *
 * Scrolls the display of army or caravan units downward to show
 * different units when more are available than can fit on screen.
 * Only works for army and caravan selections.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Modifies xfer_aslot or xfer_vslot arrays for scrolling
 *   - Calls beep() if at minimum scroll position or invalid selection
 *
 * Notes:
 *   - Only applies to army selections (>= xf_min_army) or caravan selection
 *   - Scroll position limited by 0 minimum
 *   - Uses opposite slot direction for scrolling source list
 *   - Part of the unit list navigation interface
 *   - Returns to beginning of available unit list
 */
static int
xfer_shiftdown PARM_0(void)
{
  int which_slot = xfer_direction ^ 1;

  /* army shift */
  if (xfer_selection >= xf_min_army) {
    /* simple check */
    if (xfer_aslot[which_slot] <= 0) {
      beep();
    } else {
      xfer_aslot[which_slot]--;
    }
  } else if (xfer_selection == xf_sel_cvn) {
    /* simple check */
    if (xfer_vslot[which_slot] <= 0) {
      beep();
    } else {
      xfer_vslot[which_slot]--;
    }
  } else {
    beep();
  }
  return(0);
}

/*
 * xfer_options - Access game options configuration from transfer mode
 *
 * Provides access to the game's options configuration system while
 * in transfer mode. Allows players to adjust game settings without
 * exiting the transfer interface.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Calls option_cmd() with transfer mode key system and bindings
 *   - May modify game configuration options
 *   - Temporarily exits transfer display for options interface
 *
 * Notes:
 *   - Uses transfer mode's key system for consistency
 *   - Options changes apply immediately to the game
 *   - Returns to transfer mode after options configuration
 *   - Part of the transfer mode utility interface
 */
static int
xfer_options PARM_0(void)
{
  option_cmd(xfer_keysys, &xfer_bindings);
  return(0);
}

/*
 * xfer_help - Display transfer mode command help
 *
 * Shows comprehensive help documentation for all transfer mode
 * commands and key bindings. Provides interactive help system
 * for learning the transfer interface.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   int - Always returns 0
 *
 * Side Effects:
 *   - Calls create_help() to display help system
 *   - Temporarily exits transfer display for help interface
 *
 * Notes:
 *   - Help includes all transfer mode commands and descriptions
 *   - Uses transfer mode key bindings and function definitions
 *   - Interactive help system with navigation
 *   - Returns to transfer mode after help consultation
 *   - Part of the transfer mode user assistance interface
 */
static int
xfer_help PARM_0(void)
{
  /* create the help system */
  create_help("Conquer Transfer Mode Command List",
	      xfer_bindings, xfer_funcs, xfer_keysys.num_parse);
  return(0);
}

/*
 * xfer_capstr - Build capacity information string for display
 *
 * Constructs a formatted string showing the storage capacity for
 * a specific transfer selection, handling different unit types
 * and unlimited capacity cases.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) to get capacity for
 *   select_num - Selection index within the transfer interface
 *   out_str - Output buffer to write capacity string (must not be NULL)
 *
 * Returns:
 *   void - No return value, writes to out_str buffer
 *
 * Side Effects:
 *   - Writes formatted capacity string to out_str buffer
 *   - Sets out_str[0] = '\0' for invalid parameters
 *
 * Notes:
 *   - Shows "unlimited" for unlimited capacity selections
 *   - Army capacity shown as "X men" or "X man"
 *   - Caravan capacity shown as "X wagons" or "X wagon"
 *   - Other selections show numeric capacity
 *   - Performs bounds checking on all parameters
 *   - Used by transfer interface display system
 */
static void
xfer_capstr PARM_3(int, slot, int, select_num, char *, out_str)
{
  long lval;

  /* check the input */
  if (out_str == NULL) return;
  out_str[0] = '\0';
  if ((slot < 0) || (slot > 1)) return;
  if ((select_num < 0) ||
      (select_num >= xf_max_items) ||
      (select_num > xf_min_army)) return;

  /* check for unlimited entries */
  if (xfer_unlimited(slot, select_num)) {
    strcpy(out_str, "unlimited");
    return;
  }

  /* now figure out what item is in use */
  if (select_num == xf_min_army) {
    lval = xfer_space(slot, select_num);
    sprintf(out_str, "%ld %s", lval, (lval != 1) ? "men" : "man");
  } else if (select_num == xf_sel_cvn) {
    lval = xfer_space(slot, select_num);
    sprintf(out_str, "%ld %s", lval, (lval != 1) ? "wagons" : "wagon");
  } else {
    sprintf(out_str, "%ld", xfer_space(slot, select_num));
  }
}

/*
 * xfer_stostr - Build current storage information string for display
 *
 * Constructs a formatted string showing the current storage contents
 * for a specific transfer selection, handling different unit types
 * and displaying appropriate information.
 *
 * Parameters:
 *   slot - Transfer slot index (0 or 1) to get storage for
 *   select_num - Selection index within the transfer interface
 *   out_str - Output buffer to write storage string (must not be NULL)
 *
 * Returns:
 *   void - No return value, writes to out_str buffer
 *
 * Side Effects:
 *   - Writes formatted storage string to out_str buffer
 *   - Sets out_str[0] = '\0' for invalid parameters
 *
 * Notes:
 *   - Army storage shows unit type and ID number
 *   - Caravan storage shows "caravan X" with ID
 *   - Civilian storage shows population count
 *   - Crew storage shows crew count
 *   - Material storage shows quantity as floating point
 *   - Performs bounds checking on all parameters
 *   - Used by transfer interface display system
 */
static void
xfer_stostr PARM_3(int, slot, int, select_num, char *, out_str)
{
  ARMY_PTR a1_ptr;
  CVN_PTR v1_ptr;

  /* check the input */
  if (out_str == NULL) return;
  out_str[0] = '\0';
  if ((slot < 0) || (slot > 1)) return;
  if ((select_num < 0) ||
      (select_num >= xf_max_items)) return;

  /* determine the item type */
  if (select_num >= xf_min_army) {
    if ((a1_ptr = xfer_armyptr(slot, select_num)) != NULL) {
      if (a_isnormal(a1_ptr->unittype)) {
	sprintf(out_str, "%s %d",
		ainfo_list[a1_ptr->unittype].sname,
		a1_ptr->armyid);
      } else {
	sprintf(out_str, "%s %d", ainfo_list[a1_ptr->unittype].name,
		a1_ptr->armyid);
      }
    }
  } else if (select_num == xf_sel_cvn) {
    if ((v1_ptr = xfer_cvnptr(slot)) != NULL) {
      sprintf(out_str, "caravan %d", v1_ptr->cvnid);
    }
  } else if (select_num == xf_sel_civ) {
    sprintf(out_str, "%ld", xfer_sites[slot].civies);
  } else if (select_num == xf_sel_crew) {
    sprintf(out_str, "%ld", xfer_sites[slot].crews);
  } else {
    sprintf(out_str, "%.0f",
	    (double) xfer_sites[slot].mtrls[select_num]);
  }
}

/*
 * xfer_midstr - Build middle column information string for display
 *
 * Constructs a formatted string for the middle column of the transfer
 * interface, showing transfer amounts or unit information depending
 * on selection type.
 *
 * Parameters:
 *   select_num - Selection index within the transfer interface
 *   out_str - Output buffer to write middle string (must not be NULL)
 *
 * Returns:
 *   void - No return value, writes to out_str buffer
 *
 * Side Effects:
 *   - Writes formatted middle column string to out_str buffer
 *   - Sets out_str[0] = '\0' for invalid parameters
 *
 * Notes:
 *   - Army selections show load capacity as "X men" or "X man"
 *   - Caravan selections show load capacity as "X wagons" or "X wagon"
 *   - Civilian selections show configured transfer amount
 *   - Crew selections show configured transfer amount
 *   - Material selections show configured transfer amount
 *   - Uses xfer_direction to determine source slot
 *   - Used by transfer interface display system for center column
 */
static void
xfer_midstr PARM_2(int, select_num, char *, out_str)
{
  ARMY_PTR a1_ptr;
  CVN_PTR v1_ptr;
  long lval;

  /* check the input */
  if (out_str == NULL) return;
  out_str[0] = '\0';
  if ((select_num < 0) ||
      (select_num >= xf_max_items)) return;

  /* determine the type */
  if (select_num >= xf_min_army) {
    if ((a1_ptr = xfer_armyptr(xfer_direction ^ 1,
			       select_num)) != NULL) {
      lval = army_load(a1_ptr);
      sprintf(out_str, "%ld %s", lval, (lval != 1) ? "men" : "man");
    }
  } else if (select_num == xf_sel_cvn) {
    if ((v1_ptr = xfer_cvnptr(xfer_direction ^ 1)) != NULL) {
      lval = cvn_load(v1_ptr);
      sprintf(out_str, "%ld %s", lval, (lval != 1) ? "wagons" : "wagon");
    }
  } else if (select_num == xf_sel_civ) {
    /* just show the transfer amount */
    sprintf(out_str, "%ld", xfer_xchanges[MTRLS_NUMBER]);
  } else if (select_num == xf_sel_crew) {
    /* just show the transfer amount */
    sprintf(out_str, "%ld", xfer_xchanges[MTRLS_NUMBER + 1]);
  } else {
    /* just show the transfer amounts */
    sprintf(out_str, "%ld", xfer_xchanges[select_num]);
  }
}

/*
 * xfer_show - Display the complete transfer mode interface
 *
 * Renders the full transfer mode interface including site descriptions,
 * capacity information, current storage, transfer amounts, and navigation
 * indicators. Creates a comprehensive visual transfer management system.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   void - No return value, updates screen display
 *
 * Side Effects:
 *   - Clears and redraws the entire screen
 *   - Displays error messages if present
 *   - Shows site titles, column headers, and data rows
 *   - Highlights current selection with directional indicators
 *   - Updates bottom instruction line
 *
 * Notes:
 *   - Calculates column widths based on screen size
 *   - Shows two transfer sites side by side with center amounts column
 *   - Current selection highlighted with direction arrows
 *   - Displays capacity, storage, and transfer amounts for each item
 *   - Error messages shown on bottom line when present
 *   - Complete interface refresh for transfer mode
 */
static void
xfer_show PARM_0(void)
{
  int i, xloc, yloc, width, apt, bpt;
  char astr[LINELTH], bstr[LINELTH];

  /* calculate the width of the information squares */
  width = (COLS - (XF_INDENT + XF_SPACING)) / 5;

  /* start with the bottom title */
  errorbar("Transfer Mode", "Hit '?' for Bindings");

  /* show the site titles */
  xloc = XF_INDENT + width + 2;
  for (i = 0; i < 2; i++) {
    /* center the item in the appropriate position */
    xloc -= (strlen(xfer_sites[i].description) / 2);
    standout();
    mvaddstr(0, xloc, xfer_sites[i].description);
    standend();
    xloc = XF_INDENT + 4 * (width + 1) + 3;
  }

  /* now display the item titles */
  yloc = 4;
  if (xfer_do_mtrls == TRUE) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      mvaddstr(yloc++, 1, mtrls_info[i].name);
    }
  }
  if (xfer_civ_units > 0) {
    mvaddstr(yloc++, 1, "Civilians");
  }
  if (xfer_crew_units > 0) {
    mvaddstr(yloc++, 1, "Crew");
  }
  if (xfer_do_cvn == TRUE) {
    mvaddstr(yloc++, 1, "Caravans");
  }
  if (xfer_do_army == TRUE) {
    mvaddstr(yloc++, 1, "Armies");
  }

  /* now display the column headings */
  yloc = 2;
  xloc = XF_INDENT;
  for (i = 0; i < 2; i++) {

    /* find the central points */
    apt = xloc + (width + 1) / 2 + 1;
    bpt = xloc + (3 * width + 1) / 2 + 2;

    /* build the items */
    strcpy((i == 0) ? astr : bstr, "Capacity");
    apt -= strlen(astr) / 2;
    strcpy((i == 0) ? bstr : astr, "Storage");
    bpt -= strlen(bstr) / 2;

    /* now position */
    mvaddstr(yloc, apt, astr);
    mvaddstr(yloc, bpt, bstr);

    /* build the separator line */
    mvaddch(yloc + 1, xloc, '|');
    for (apt = 0; apt < 2; apt++) {
      for (bpt = 0; bpt < width; bpt++) {
	addch('-');
      }
      addch('|');
    }

    /* next positioning */
    xloc += (5 + 3 * width);
  }

  /* provide the title of the amount column */
  xloc = XF_INDENT + (5 * width + 1) / 2 + 4;
  strcpy(astr, "Amounts");
  xloc -= strlen(astr) / 2;
  mvaddstr(yloc + 1, xloc, astr);

  /* now display all of the possible selections */
  yloc = 4;
  for (i = 0; i < xf_max_items; i++) {

    /* Build up the strings for the first site */
    xloc = XF_INDENT;
    xfer_capstr(0, i, astr);
    xfer_stostr(0, i, bstr);
    mvprintw(yloc, xloc, "|%*s|%*s|",
	     width, astr, width, bstr);

    /* now build up the middle string */
    xloc += (2 * width + 3);
    xfer_midstr(i, astr);
    apt = (width - strlen(astr) + 1) / 2;
    if (apt < 0) apt = 0;
    if (i == xfer_selection) {
      /* Ensure safe formatting with comprehensive bounds checking */
      int max_width = 40; /* Conservative limit for display width */
      int safe_apt = (apt > max_width) ? max_width : apt;
      int safe_width_minus_apt = (width - apt > max_width) ? max_width : (width - apt);
      /* Truncate astr if too long */
      char safe_astr[50];
      strncpy(safe_astr, astr, sizeof(safe_astr) - 1);
      safe_astr[sizeof(safe_astr) - 1] = '\0';
      
      snprintf(bstr, sizeof(bstr), "%c%*s%-*s%c",
	       xfer_indicator[xfer_direction],
	       safe_apt, "",
	       safe_width_minus_apt, safe_astr,
	       xfer_indicator[xfer_direction]);
      standout();
      mvaddstr(yloc, xloc, bstr);
      standend();
    } else {
      mvaddstr(yloc, xloc + apt + 1, astr);
    }

    /* now conclude with the second site */
    xloc += (width + 2);
    xfer_capstr(1, i, bstr);
    xfer_stostr(1, i, astr);
    mvprintw(yloc++, xloc, "|%*s|%*s|",
	     width, astr, width, bstr);

  }

  /* now complete the box */
  xloc = XF_INDENT;
  for (i = 0; i < 2; i++) {

    /* draw the bottom line */
    mvaddch(yloc, xloc, '+');
    for (apt = 0; apt < 2; apt++) {
      for (bpt = 0; bpt < width; bpt++) {
	addch('-');
      }
      addch('+');
    }

    /* next positioning */
    xloc += (5 + 3 * width);
  }

  /* finish with the quickie instructions */
  if (xfer_error[0] != '\0') {
    bottommsg(xfer_error);
    xfer_error[0] = '\0';
  } else {
    mvaddstr(LINES - 1, 3,
	     "' ' - xfer  '<'- xfer left '>' - xfer right  '=' - set amount  'q' - quit");
  }
  refresh();
}

/*
 * get_divunits - Calculate lowest common multiple for transfer units
 *
 * Computes the lowest common multiple of two values to determine
 * appropriate transfer unit sizes for population transfers between
 * sites with different capacity divisors.
 *
 * Parameters:
 *   a - First divisor value
 *   b - Second divisor value
 *
 * Returns:
 *   long - Lowest common multiple of a and b, or 0/single value for edge cases
 *
 * Side Effects:
 *   - None - pure mathematical calculation function
 *
 * Notes:
 *   - Returns 0 if either value is 0
 *   - Returns the positive value if one is negative
 *   - Returns 0 if both values are negative
 *   - Uses Euclidean algorithm to find greatest common divisor
 *   - LCM calculated as (a * b) / GCD(a, b)
 *   - Used for determining civilian and crew transfer unit sizes
 *   - Ensures transfers align with both sites' capacity divisors
 */
static long
get_divunits PARM_2(long, a, long, b)
{
  long tmp, hold = a * b;

  /* check for negatives */
  if ((a == 0) ||
      (b == 0)) {
    return(0);
  }
  if (a < 0) {
    if (b < 0) {
      return(0);
    }
    return(b);
  }
  if (b < 0) {
    return(a);
  }

  /* compute the greatest common divisor */
  while ((b > 1) && (a % b != 0)) {
    tmp = b;
    b = a % b;
    a = tmp;
  }

  /* should be it now */
  return(hold / b);
}

/*
 * xfer_mode - Main interactive transfer mode interface
 *
 * Provides the main interactive interface for transferring items between
 * two storage sites. Handles initialization, user input processing,
 * and maintains the transfer loop until completion.
 *
 * Parameters:
 *   void - No parameters required, uses global xfer_sites array
 *
 * Returns:
 *   void - No return value, modifies transfer site data
 *
 * Side Effects:
 *   - Modifies global transfer configuration variables
 *   - Processes user keyboard input through key binding system
 *   - Updates transfer site data based on user operations
 *   - Displays error messages and interface updates
 *   - May modify army, navy, caravan, and sector data
 *
 * Notes:
 *   - Initializes transfer configuration based on site capabilities
 *   - Determines available transfer types (materials, civilians, crew, armies, caravans)
 *   - Main event loop processes user input until xfer_done flag set
 *   - Validates transfer compatibility between sites
 *   - Exits with error if no transferable items found
 *   - Uses key binding system for command processing
 *   - Complete transfer mode implementation for item management
 */
void
xfer_mode PARM_0(void)
{
  FNCI fnc_ptr;
  int i;

  /* check mtrls input */
  if ((xfer_sites[0].mtrl_max == 0) ||
      (xfer_sites[1].mtrl_max == 0)) {
    xfer_do_mtrls = FALSE;
  } else {
    xfer_do_mtrls = TRUE;
  }

  /* check the input */
  xfer_civ_units = get_divunits(xfer_sites[0].civ_div,
				xfer_sites[1].civ_div);
  xfer_crew_units = get_divunits(xfer_sites[0].crew_div,
				 xfer_sites[1].crew_div);

  /* now check the army and caravan transference */
  if ((xfer_sites[0].site_class != XFER_NAVY) &&
      (xfer_sites[1].site_class != XFER_NAVY)) {
    xfer_do_army = FALSE;
    xfer_do_cvn = FALSE;
  } else {
    xfer_do_cvn = TRUE;
    xfer_do_army = TRUE;
    for (i = 0; i < 2; i++) {
      if (xfer_sites[i].onb_max == 0) {
	xfer_do_army = FALSE;
      }
      if (xfer_sites[i].onb_cmax == 0) {
	xfer_do_cvn = FALSE;
      }
    }
  }

  /* initialize some stuff */
  xfer_init();
  xfer_aslot[0] = xfer_aslot[1] = 0;
  xfer_vslot[0] = xfer_vslot[1] = 0;
  xfer_selection = 0;
  xfer_direction = 0;
  xfer_took_off = 0;
  xfer_done = FALSE;
  xfer_error[0] = '\0';

  /* check if there is anything to transfer */
  if (xf_max_items == 0) {
    errormsg("There is nothing which can be transfered between these sites");
    return;
  }

  /* go into the xfer loop */
  while (xfer_done == FALSE) {

    /* display the transfer screen */
    move(0, 0);
    clrtobot();
    xfer_show();

    /* do the keyboard thing */
    if ((fnc_ptr = parse_keys(xfer_bindings, FALSE)) == NULL) {

      /* bad input */
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Unknown key binding: ");
      show_str(string, FALSE);
      presskey();

    } else {

      /* perform the function */
      (*fnc_ptr)();

    }

  }
}

/* -=- do the functions and default keybindings -=- */

PARSE_STRUCT xfer_funcs[] = {
  { xfer_options, "conquer-options",
      "Adjust the conquer environment by changing various options" },
  { do_ignore, "ignore-key",
      "I'll just pretend as if I never saw that key stroke" },
  { do_redraw, "redraw-screen",
      "redraw the display of the screen" },
  { xfer_add, "xfer-add",
      "Xfer-mode, add one unit to the transfer amount" },
  { xfer_assign, "xfer-assign",
      "Xfer-mode, assign a value to the transfer amount" },
  { xfer_div, "xfer-divide",
      "Xfer-mode, divide the transfer amount by ten" },
  { xfer_down, "xfer-down",
      "Xfer-mode, move the selection pointer downward" },
  { xfer_help, "xfer-help",
      "Xfer-mode, provide a listing of the commands in transfer mode" },
  { xfer_left, "xfer-left",
      "Xfer-mode, move goods from the right to the left" },
  { xfer_move, "xfer-move",
      "Xfer-mode, exchange goods between the two sites" },
  { xfer_mult, "xfer-multiply",
      "Xfer-mode, mulitply the transfer amount by ten" },
  { xfer_quit, "xfer-quit",
      "Xfer-mode, exit the transfer mode, finalizing the transfers" },
  { xfer_right, "xfer-right",
      "Xfer-mode, move goods from the left to the right" },
  { xfer_shiftdown, "xfer-shiftdown",
      "Xfer-mode, move the list of army or caravan units downward" },
  { xfer_shiftup, "xfer-shiftup",
      "Xfer-mode, move the list of army or caravan units upward" },
  { xfer_subtract, "xfer-subtract",
      "Xfer-mode, subtract one unit from the transfer amount" },
  { xfer_up, "xfer-up",
      "Xfer-mode, move the selection pointer upward" }
};

KBIND_STRUCT xfer_klist[] = {
  { ESTR_IGN, do_ignore },
  { CSTR_B, xfer_left },
  { CSTR_F, xfer_right },
  { CSTR_L, do_redraw },
  { CSTR_N, xfer_down },
  { CSTR_P, xfer_up },
  { CSTR_R, do_redraw },
  { AKEY_UP1, xfer_up },
  { AKEY_UP2, xfer_up },
  { AKEY_DOWN1, xfer_down },
  { AKEY_DOWN2, xfer_down },
  { AKEY_LEFT1, xfer_left },
  { AKEY_LEFT2, xfer_left },
  { AKEY_RIGHT1, xfer_right },
  { AKEY_RIGHT2, xfer_right },
  { "\t", do_ignore },
  { "?", xfer_help },
  { "<", xfer_left },
  { ">", xfer_right },
  { "=", xfer_assign },
  { "*", xfer_mult },
  { "/", xfer_div },
  { "+", xfer_add },
  { "-", xfer_subtract },
  { " ", xfer_move },
  { "H", xfer_left },
  { "h", xfer_left },
  { "J", xfer_down },
  { "j", xfer_down },
  { "K", xfer_up },
  { "k", xfer_up },
  { "L", xfer_right },
  { "l", xfer_right },
  { "O", xfer_options },
  { "o", xfer_shiftup },
  { "p", xfer_shiftdown },
  { "Q", xfer_quit },
  { "q", xfer_quit }
};

/* information structure concerning movement keybindings */
KEYSYS_STRUCT xfer_keysys = {
  "xfer", xfer_funcs, xfer_klist, 0, 0
};

/*
 * align_xfer_keys - Initialize transfer mode key binding system
 *
 * Sets up the key binding system for transfer mode by calculating
 * array sizes and initializing the key binding structure. Called
 * during transfer mode initialization.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   void - No return value, initializes global key binding data
 *
 * Side Effects:
 *   - Calculates and sets xfer_keysys.num_binds and num_parse
 *   - Calls init_keys() to set up xfer_bindings structure
 *   - Modifies global xfer_bindings pointer
 *
 * Notes:
 *   - Only initializes if xfer_bindings is NULL (first call)
 *   - Array sizes calculated using sizeof operations
 *   - Links function definitions with key bindings
 *   - Part of the transfer mode initialization sequence
 *   - Ensures proper key binding system setup for user interaction
 */
void
align_xfer_keys PARM_0(void)
{
  /* initialize the system */
  if (xfer_bindings == NULL) {
    xfer_keysys.num_binds = (sizeof(xfer_klist)/sizeof(KBIND_STRUCT));
    xfer_keysys.num_parse = (sizeof(xfer_funcs)/sizeof(PARSE_STRUCT));
    init_keys(&xfer_bindings, xfer_klist, xfer_keysys.num_binds);
  }
}

