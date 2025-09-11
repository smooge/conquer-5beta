/*
 * checkX.c - Data Validation, Integrity Checking, and File Locking
 *
 * This file provides comprehensive data validation and integrity checking
 * for the Conquer game system. It ensures all game entities maintain valid
 * states and enforces constraints to prevent data corruption. Also handles
 * file locking for multi-user coordination.
 *
 * Key Functionality Areas:
 *   - Nation data validation and constraint enforcement
 *   - Army/Navy/Caravan/City/Item validation and cleanup
 *   - Sector data integrity verification
 *   - Cross-reference validation (diplomacy, ownership, etc.)
 *   - File locking for concurrent access control
 *   - User management and login validation
 *   - Multi-user session tracking and display
 *
 * This module is critical for game stability and data integrity, preventing
 * corruption that could crash the game or create unfair advantages. It performs
 * extensive validation during updates and can automatically fix many issues.
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
#include "calenX.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "racesX.h"
#include "activeX.h"
#include "elevegX.h"
#include "nclassX.h"
#include "statusX.h"
#include "tgoodsX.h"
#include "caravanX.h"
#include "dstatusX.h"
#define F_TLOCK 2
#include <unistd.h>
#include <stdio.h>
#include <sys/file.h>
#ifndef VAXC
#include <pwd.h>
#ifdef cyber
extern struct passwd *getpwuid();
#endif /* cyber */
#endif /* VAXC */
#ifndef VAXC
#include <fcntl.h>
#include <sys/file.h>
#else
#include <file.h>
#endif
#ifdef FILELOCK
#ifdef LOCKF
#  define do_lock(fd) lockf(fd,F_TLOCK,0)
#else
#  define do_lock(fd) flock(fd,LOCK_EX|LOCK_NB)
#endif
#else /* FILELOCK */
#endif /* FILELOCK */
#ifdef UNAME
#include <sys/utsname.h>
#endif

/*
 * verify_ntn - Comprehensive nation data validation and integrity checking
 *
 * Performs extensive validation of all nations and their entities, checking
 * constraints and automatically fixing violations. This is one of the most
 * critical functions for maintaining game data integrity.
 *
 * Parameters:
 *   __file__ - Source file name for error reporting (usually __FILE__)
 *   __line__ - Source line number for error reporting (usually __LINE__)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Validates and fixes nation properties (name, login, race, capital, etc.)
 *   - Checks and corrects army units (size, location, leadership chains)
 *   - Validates navy units (ships, crew, cargo, location)
 *   - Verifies caravan units (size, crew, cargo, location)
 *   - Checks city data (location, ownership, designation, resources)
 *   - Validates item/commodity data (location, quantities)
 *   - Enforces diplomacy constraints (monster nations always at war)
 *   - Removes invalid entities via dest_* functions
 *   - Logs all violations and fixes to update file
 *
 * Notes:
 *   - Automatically fixes many data integrity issues
 *   - Can remove units that violate fundamental constraints
 *   - Essential for preventing game crashes and exploits
 *   - Called during update processing and debug verification
 *   - Uses goto statements for efficient entity removal loops
 */
void
verify_ntn PARM_2( char *, __file__, int, __line__ )
{
  register int i;
  int j, statval, sleadval;

  /* go through all of the world */
  for (country = 1; country < MAXNTN; country++ )
    if ((ntn_ptr = world.np[country]) != NULL) {

      /* if it is inactive just go on */
      if (n_notactive(ntn_ptr->active)) continue;

      /* check the nation statistics */
      if (strlen(ntn_ptr->name) < 2) {
	fprintf(fupdate,
		"%s[%d] Warning: Nation #%d has short name of \"%s\"\n",
		__file__, __line__, country, ntn_ptr->name);
      }

      /* verify login setting */
      if (user_exists(ntn_ptr->login) == FALSE) {
	fprintf(fupdate,
		"%s[%d] Warning: Nation %s login name of %s not a user\n",
		__file__, __line__, ntn_ptr->name, ntn_ptr->login);
	strcpy(ntn_ptr->login, LOGIN);
      }

      /* check the repro rate */
      if (!n_ismonster(ntn_ptr->active)) {
	if (ntn_ptr->repro < 4) {
	  fprintf(fupdate,
		  "%s[%d]: %s has repro < 4 of %d\n",
		  __file__, __line__, ntn_ptr->name, ntn_ptr->repro);
	  ntn_ptr->repro = 4;
	}
	if (ntn_ptr->repro > 15) {
	  fprintf(fupdate, "%s[%d]: %s has repro > 15 of %d\n",
		  __file__, __line__, ntn_ptr->name, ntn_ptr->repro);
	  ntn_ptr->repro = 15;
	}
      }

      /* check the race */
      if (ntn_ptr->race < 0 || ntn_ptr->race > TUNKNOWN) {
	fprintf(fupdate, "%s[%d]: %s has unknown race of %d\n",
		__file__, __line__, ntn_ptr->name, ntn_ptr->race);
	ntn_ptr->race = TUNKNOWN;
      }

      /* check the capital location */
      if (ntn_ptr->capx >= MAPX) {
	fprintf(fupdate, "%s[%d]: %s capital X location off map (%d)\n",
		__file__, __line__, ntn_ptr->name, ntn_ptr->capx);
	ntn_ptr->capx = 0;
      }
      if (ntn_ptr->capy >= MAPY) {
	fprintf(fupdate, "%s[%d]: %s capital Y location off map (%d)\n",
		__file__, __line__, ntn_ptr->name, ntn_ptr->capy);
	ntn_ptr->capy = 0;
      }

      /* check the nation class */
      if (ntn_ptr->class < 0 || ntn_ptr->class >= nclass_number) {
	fprintf(fupdate, "%s[%d]: %s nation class of %d invalid\n",
		__file__, __line__, ntn_ptr->name, ntn_ptr->class);
	ntn_ptr->class = NC_DEFAULT;
      }

      /* check the nation activity level */
      if (ntn_ptr->active > ACT_MAXIMUM) {
	fprintf(fupdate, "%s[%d]: %s activity value of %d invalid\n",
		__file__, __line__, ntn_ptr->name, ntn_ptr->active);
	ntn_ptr->active = NPC_INACTIVE;
      }

      /* check the movement rate */
      if (ntn_ptr->maxmove < 4) {
	fprintf(fupdate, "%s[%d]: %s movement rate of %d < 4\n",
		__file__, __line__, ntn_ptr->name, ntn_ptr->maxmove);
	ntn_ptr->maxmove = 4;
      }

      /* go through the entire list of armies */
      for (army_ptr = ntn_ptr->army_list;
	   army_ptr != NULL;
	   army_ptr = army_ptr->next) {

      army_recheck:
	/* check the size */
	if (ARMY_SIZE <= 0) {
	  fprintf(fupdate, "%s[%d]: %s army[%d] removing %ld size unit\n",
		  __file__, __line__, ntn_ptr->name, ARMY_ID, ARMY_SIZE );
	  army_tptr = army_ptr->next;
	  dest_army(ARMY_ID);
	  army_ptr = army_tptr;
	  if (army_ptr != NULL) goto army_recheck;
	  else break;
	}

	/* check the efficiency */
	if (a_isleader(ARMY_TYPE)) {
	  if (ARMY_MAXEFF != 100) {
	    ARMY_MAXEFF = 100;
	  }
	}

	/* check the leader */
	if (ARMY_LEAD == ARMY_ID) {
	  fprintf( fupdate, "%s[%d]: %s army[%d] being lead by himself\n",
		  __file__, __line__, ntn_ptr->name, ARMY_ID );
	  ARMY_LEAD = EMPTY_HOLD;
	}

	/* check the grouping */
	statval = unit_status(ARMY_STAT);
	if (statval == ST_GROUPED) {
	  if (ARMY_LEAD == EMPTY_HOLD) {
	    fprintf( fupdate, "%s[%d]: %s army[%d] grouped without leader\n",
		    __file__, __line__, ntn_ptr->name, ARMY_ID );
	    set_status(ARMY_STAT, ST_DEFEND);
	  } else {
	    statval = group_stat(ARMY_LEAD);
	  }
	}

	/* check the leader information */
	if ((ARMY_LEAD != EMPTY_HOLD) &&
	    (armybynum(ARMY_LEAD) == NULL)) {
	  fprintf( fupdate,
		  "%s[%d]: %s army[%d] leader %d non-existent [fixed]\n",
		  __file__, __line__, ntn_ptr->name, ARMY_ID, ARMY_LEAD );
	  ARMY_LEAD = EMPTY_HOLD;
	}

	/* check the location */
	if (ARMY_XLOC >= MAPX) {
	  fprintf( fupdate, "%s[%d]: %s army[%d] xlocation = %d\n",
		  __file__, __line__, ntn_ptr->name, ARMY_ID, ARMY_XLOC );
	  ARMY_XLOC = 0;
	}
	if (ARMY_YLOC >= MAPY) {
	  fprintf( fupdate, "%s[%d]: %s army[%d] ylocation = %d\n",
		  __file__, __line__, ntn_ptr->name, ARMY_ID, ARMY_YLOC );
	  ARMY_YLOC = 0;
	}

	/* check the spell point totals */
	if (ARMY_SPTS > 200) {
	  fprintf( fupdate, "%s[%d]: %s army[%d] spell points = %d\n",
		  __file__, __line__, ntn_ptr->name, ARMY_ID, ARMY_SPTS );
	  ARMY_SPTS = 0;
	}

	/* check for bad stats */
	if (!aunit_stat(ARMY_STAT)) {
	  fprintf(fupdate, "%s[%d]: %s army[%d] has bad status[%s]\n",
		  __file__, __line__, ntn_ptr->name, ARMY_ID,
		  stat_info[unit_status(ARMY_STAT)].name);
	  set_status(ARMY_STAT, ST_DEFEND);
	}

	/* check info on stored fleets */
	if (onship_stat(statval)) {

	  /* check if there is a navy that is carrying it */
	  i = FALSE;
	  sleadval = army_shipleader(army_ptr);
	  for (navy_ptr = ntn_ptr->navy_list;
	       navy_ptr != NULL;
	       navy_ptr = navy_ptr->next) {

	    /* verify storage */
	    if (NAVY_ARMY == sleadval) {
	      i = TRUE;
	      break;
	    }
	  }
	  if (i == FALSE) {
	    fprintf(fupdate, "%s[%d]: %s army[%d] onboard nothing\n",
		    __file__, __line__, ntn_ptr->name, ARMY_ID);
	    set_status(ARMY_STAT, ST_DEFEND);
	  }

	} else {

	  if ((ARMY_MOVE > 75) &&
	      nomove_stat(ARMY_STAT)) {

	    /* invalid movement */
	    fprintf(fupdate, "%s[%d]: %s army[%d] status[%s] move > 75\n",
		    __file__, __line__, ntn_ptr->name, ARMY_ID,
		    stat_info[unit_status(ARMY_STAT)].name);
	    ARMY_MOVE = 75;

	  }

	  /* check for units in the water */
	  if ((sct[ARMY_XLOC][ARMY_YLOC].altitude == ELE_WATER) &&
	      (major_desg(sct[ARMY_XLOC][ARMY_YLOC].designation)
	       != MAJ_BRIDGE)) {

	    /* should be drowned */
	    fprintf(fupdate, "%s[%d]: %s army[%d] loc[%d,%d](water) men=%ld\n",
		    __file__, __line__, ntn_ptr->name, ARMY_ID,
		    ARMY_XLOC, ARMY_YLOC,
		    ARMY_SIZE);
	    army_tptr = army_ptr->next;
	    dest_army(ARMY_ID);
	    army_ptr = army_tptr;
	    if (army_ptr != NULL) goto army_recheck;
	    else break;

	  }

	}

      } /* for (army_ptr = ...) */

      /* check the information about the navies */
      for (navy_ptr = ntn_ptr->navy_list;
	   navy_ptr != NULL;
	   navy_ptr = navy_ptr->next) {

      navy_recheck:
	/* check the navy size */
	j = 0;
	for (i = 0; i < NSHP_NUMBER; i++) {
	  j |= NAVY_SHIPS[i];
	}
	if (j == 0) {
	  fprintf(fupdate, "%s[%d]: %s navy[%d] crew=%d empty\n",
		  __file__, __line__, ntn_ptr->name, NAVY_ID, NAVY_CREW);
	  navy_tptr = navy_ptr->next;
	  dest_navy(NAVY_ID);
	  navy_ptr = navy_tptr;
	  if (navy_ptr != NULL) goto navy_recheck;
	  else break;

	}
	if (navy_ptr->crew == 0) {
	  fprintf(fupdate, "%s[%d]: %s navy[%d] crew=0\n",
		  __file__, __line__, ntn_ptr->name, NAVY_ID);
	  navy_tptr = navy_ptr->next;
	  dest_navy(NAVY_ID);
	  navy_ptr = navy_tptr;
	  if (navy_ptr != NULL) goto navy_recheck;
	  else break;

	}

	/* check the location */
	if (NAVY_XLOC >= MAPX) {
	  fprintf( fupdate, "%s[%d]: %s navy[%d] xlocation = %d\n",
		  __file__, __line__, ntn_ptr->name, NAVY_ID, NAVY_XLOC );
	  NAVY_XLOC = 0;
	}
	if (NAVY_YLOC >= MAPY) {
	  fprintf( fupdate, "%s[%d]: %s navy[%d] ylocation = %d\n",
		  __file__, __line__, ntn_ptr->name, NAVY_ID, NAVY_YLOC );
	  NAVY_YLOC = 0;
	}

	/* verify contents */
	if (NAVY_ARMY != EMPTY_HOLD) {
	  army_ptr = armybynum(NAVY_ARMY);
	  if (army_ptr == NULL || !onship_stat(ARMY_STAT)) {
	    fprintf(fupdate, "%s[%d]: %s navy[%d] carrying invalid troop\n",
		    __file__, __line__, ntn_ptr->name, NAVY_ID);
	    NAVY_ARMY = EMPTY_HOLD;
	  }
	}
	if (NAVY_CVN != EMPTY_HOLD) {
	  cvn_ptr = cvnbynum(NAVY_CVN);
	  if (cvn_ptr == NULL || !onship_stat(CVN_STAT)) {
	    fprintf(fupdate, "%s[%d]: %s navy[%d] carrying invalid wagon\n",
		    __file__, __line__, ntn_ptr->name, NAVY_ID);
	    NAVY_CVN = EMPTY_HOLD;
	  }
	}

	/* verify level of commodities */
	for (i = 0; i < MTRLS_NUMBER; i++) {
	  if ((i != MTRLS_TALONS) &&
	      (NAVY_MTRLS[i] < 0)) {
	    fprintf(fupdate, "%s[%d]: %s navy[%d] carrying negative %s\n",
		    __file__, __line__, ntn_ptr->name, NAVY_ID,
		    mtrls_info[i].lname);
	    NAVY_MTRLS[i] = 0;
	  }
	}


	/* check for bad stats */
	if (!nunit_stat(NAVY_STAT)) {
	  fprintf(fupdate, "%s[%d]: %s navy[%d] has bad status[%s]\n",
		  __file__, __line__, ntn_ptr->name, NAVY_ID,
		  stat_info[unit_status(NAVY_STAT)].name);
	  set_status(NAVY_STAT, ST_CARRY);
	}

      } /* for (navy_ptr = ... ) */

      /* go through all of the caravans */
      for (cvn_ptr = ntn_ptr->cvn_list;
	   cvn_ptr != NULL;
	   cvn_ptr = cvn_ptr->next) {

      cvn_recheck:
	/* check the carvan size */
	if (cvn_ptr->size == 0) {
	  fprintf(fupdate, "%s[%d]: %s caravan[%d] crew=%d no wagons\n",
		  __file__, __line__, ntn_ptr->name, CVN_ID, CVN_CREW);
	  cvn_tptr = cvn_ptr->next;
	  dest_cvn(CVN_ID);
	  cvn_ptr = cvn_tptr;
	  if (cvn_ptr != NULL) goto cvn_recheck;
	  else break;
	}
	if (cvn_ptr->crew == 0) {
	  fprintf(fupdate, "%s[%d]: %s cvn[%d] crew=0\n",
		  __file__, __line__, ntn_ptr->name, CVN_ID);
	  cvn_tptr = cvn_ptr->next;
	  dest_cvn(CVN_ID);
	  cvn_ptr = cvn_tptr;
	  if (cvn_ptr != NULL) goto cvn_recheck;
	  else break;
	}

	/* check the location */
	if (CVN_XLOC >= MAPX) {
	  fprintf( fupdate, "%s[%d]: %s cvn[%d] xlocation = %d\n",
		  __file__, __line__, ntn_ptr->name, CVN_ID, CVN_XLOC );
	  CVN_XLOC = 0;
	}
	if (CVN_YLOC >= MAPY) {
	  fprintf( fupdate, "%s[%d]: %s cvn[%d] ylocation = %d\n",
		  __file__, __line__, ntn_ptr->name, CVN_ID, CVN_YLOC );
	  CVN_YLOC = 0;
	}

	/* verify level of commodities */
	for (i = 0; i < MTRLS_NUMBER; i++) {
	  if ((i != MTRLS_TALONS) &&
	      (CVN_MTRLS[i] < 0)) {
	    fprintf(fupdate, "%s[%d]: %s caravan[%d] carrying negative %s\n",
		    __file__, __line__, ntn_ptr->name, CVN_ID,
		    mtrls_info[i].lname);
	    CVN_MTRLS[i] = 0;
	  }
	}

	if (!vunit_stat(CVN_STAT)) {
	  fprintf(fupdate, "%s[%d]: %s caravan[%d] has bad status[%s]\n",
		  __file__, __line__, ntn_ptr->name, CVN_ID,
		  stat_info[unit_status(CVN_STAT)].name);
	  set_status(CVN_STAT, ST_CARRY);
	}

      } /* for (cvn_ptr = ... ) */

      /* go through all of the cities */
      for (city_ptr = ntn_ptr->city_list;
	   city_ptr != NULL;
	   city_ptr = city_ptr->next) {

      city_recheck:
	/* check the location */
	if (CITY_XLOC >= MAPX) {
	  fprintf( fupdate, "%s[%d]: %s city[%s] xlocation = %d\n",
		  __file__, __line__, ntn_ptr->name, CITY_NAME, CITY_XLOC );
	  CITY_XLOC = 0;
	}
	if (CITY_YLOC >= MAPY) {
	  fprintf( fupdate, "%s[%d]: %s city[%s] ylocation = %d\n",
		  __file__, __line__, ntn_ptr->name, CITY_NAME, CITY_YLOC );
	  CITY_YLOC = 0;
	}

	/* check the sector information */
	sct_ptr = &(sct[CITY_XLOC][CITY_YLOC]);
	if (sct_ptr->owner != country) {
	  fprintf(fupdate, "%s[%d]: %s city[%s] sector owned by %d\n",
		  __file__, __line__, ntn_ptr->name, CITY_NAME,
		  sct_ptr->owner);
	  city_tptr = city_ptr->next;
	  dest_city(CITY_NAME);
	  city_ptr = city_tptr;
	  if (city_ptr != NULL) goto city_recheck;
	  else break;
	}
	if (!IS_CITY(sct_ptr->designation)) {
	  fprintf(fupdate, "%s[%d]: %s city[%s] sector not a city (%s)\n",
		  __file__, __line__, ntn_ptr->name, CITY_NAME,
		  maj_dinfo[major_desg(sct_ptr->designation)].name);
	  city_tptr = city_ptr->next;
	  dest_city(CITY_NAME);
	  city_ptr = city_tptr;
	  if (city_ptr != NULL) goto city_recheck;
	  else break;
 	}

	/* check values of items */
	for (i = 0; i < MTRLS_NUMBER; i++) {
	  if (i == MTRLS_TALONS) continue;
	  if (city_ptr->c_mtrls[i] < (itemtype) 0) {
	    fprintf(fupdate, "%s[%d]: %s city[%s] negative %s (%.0f)\n",
		    __file__, __line__, ntn_ptr->name, CITY_NAME,
		    mtrls_info[i].lname,
		    (double) city_ptr->c_mtrls[i]);
	    city_ptr->c_mtrls[i] = (itemtype) 0;
	  }
	}

      }

      /* go through all of the commodities */
      for (item_ptr = ntn_ptr->item_list;
	   item_ptr != NULL;
	   item_ptr = item_ptr->next) {

	/* check the location */
	if (ITEM_XLOC >= MAPX) {
	  fprintf( fupdate, "%s[%d]: %s item[%d] xlocation = %d\n",
		  __file__, __line__, ntn_ptr->name, ITEM_ID, ITEM_XLOC );
	  ITEM_XLOC = 0;
	}
	if (ITEM_YLOC >= MAPY) {
	  fprintf( fupdate, "%s[%d]: %s item[%d] ylocation = %d\n",
		  __file__, __line__, ntn_ptr->name, ITEM_ID, ITEM_YLOC );
	  ITEM_YLOC = 0;
	}

	/* check values of items */
	for (i = 0; i < MTRLS_NUMBER; i++) {
	  if (i == MTRLS_TALONS) continue;
	  if (item_ptr->mtrls[i] < (itemtype) 0) {
	    fprintf(fupdate, "%s[%d]: %s item[%d] negative %s (%.0f)\n",
		    __file__, __line__, ntn_ptr->name, ITEM_ID,
		    mtrls_info[i].lname,
		    (double) item_ptr->mtrls[i]);
	    item_ptr->mtrls[i] = (itemtype) 0;
	  }
	}

      }

      /* assure proper diplomacy statuses */
      if (ntn_ptr->dstatus[UNOWNED] != DIP_NEUTRAL) {
	fprintf( fupdate, "%s[%d]: %s diplomacy with god = %d not NEUTRAL\n",
		__file__, __line__, ntn_ptr->name,
		ntn_ptr->dstatus[UNOWNED] );
	ntn_ptr->dstatus[UNOWNED] = DIP_NEUTRAL;
      }
      for (i = 1; i < MAXNTN; i++) {

	/* get the value */
	if ((i == country) || ((ntn_tptr = world.np[i]) == NULL)) continue;

	/* check the statuses */
	if ((ntn_tptr->active != INACTIVE) &&
	    n_ismonster(ntn_tptr->active)) {

	  /* monster nations should always be at war */
	  if (ntn_ptr->dstatus[i] != DIP_WAR) {
	    fprintf( fupdate, "%s[%d]: %s diplomatic status with %s = %d\n",
		    __file__, __line__, ntn_ptr->name,
		    ntn_tptr->name, ntn_ptr->dstatus[i] );
	    ntn_ptr->dstatus[i] = DIP_WAR;
	  }
	  if (ntn_tptr->dstatus[country] != DIP_WAR ) {
	    fprintf( fupdate, "%s[%d]: %s diplomatic status with %s = %d\n",
		    __file__, __line__, ntn_tptr->name,
		    ntn_ptr->name, ntn_tptr->dstatus[country]);
	    ntn_tptr->dstatus[country] = DIP_WAR;
	  }

	} else if ((is_update == TRUE) &&
		   (ntn_ptr->dstatus[i] >= dstatus_number)) {

	  /* check limit */
	  fprintf( fupdate, "%s[%d]: %s diplomatic status with %d = %d\n",
		  __file__, __line__, ntn_ptr->name, i, ntn_ptr->dstatus[i] );
	  ntn_ptr->dstatus[i] = DIP_WAR;

	}

      } /* for (i = 1; ... ) */

    } /* for (country = 1; ... ) */

} /* verify_ntn() */

/*
 * verify_sct - Validate sector data integrity across the world map
 *
 * Scans every sector on the map and validates sector properties,
 * automatically correcting violations and inconsistencies. Ensures
 * map data remains consistent and valid.
 *
 * Parameters:
 *   __file__ - Source file name for error reporting (usually __FILE__)
 *   __line__ - Source line number for error reporting (usually __LINE__)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Validates tradegood types and resets invalid ones to TG_NONE
 *   - Checks mineral deposits match appropriate tradegood types
 *   - Enforces population limits (0 to ABSMAXPEOPLE)
 *   - Prevents ownership of water sectors (except by unowned)
 *   - Logs all violations and corrections to update file
 *
 * Notes:
 *   - Processes entire world map (MAPX * MAPY sectors)
 *   - Critical for preventing invalid map states
 *   - Called as part of comprehensive data verification
 *   - Automatically fixes many common map data corruption issues
 */
void
verify_sct PARM_2(char *, __file__, int, __line__ )
{
  register int x, y;

  /* go through the entire map */
  for( x = 0; x < MAPX; x++ ) {
    for( y = 0; y < MAPY; y++ ) {

      /* get sector reference */
      sct_ptr = &(sct[x][y]);

      /* check the trade good */
      if (sct_ptr->tradegood >= tgoods_number) {
	fprintf( fupdate, "%s[%d]: sct[%d][%d].tradegood = %d (invalid)\n",
		__file__, __line__, x, y, sct_ptr->tradegood );
	sct_ptr->tradegood = TG_NONE;
      }
      if ((sct_ptr->minerals != 0) &&
	  (!tg_ismetal(sct_ptr->tradegood) &&
	   !tg_isjewel(sct_ptr->tradegood) &&
	   !tg_isspell(sct_ptr->tradegood))) {
	fprintf( fupdate,
		"%s[%d]: sct[%d][%d].minerals = %d with improper tradegood\n",
		__file__, __line__, x, y, sct_ptr->minerals );
	sct_ptr->minerals = 0;
      }
      
      /* check limit of people */
      if (sct_ptr->people > ABSMAXPEOPLE)
	sct_ptr->people = ABSMAXPEOPLE;

      if (sct_ptr->people < 0) {
	fprintf( fupdate, "%s[%d]: sct[%d][%d].people = %ld\n",
		__file__, __line__, x, y, sct_ptr->people );
	if ( sct_ptr->people < -1 * ABSMAXPEOPLE )
	  sct_ptr->people = ABSMAXPEOPLE;
	else sct_ptr->people = 0;
      }

      /* check ownerships */
      if (sct_ptr->owner != UNOWNED && sct_ptr->altitude == ELE_WATER) {
	fprintf(fupdate, "%s[%d]: sct[%d][%d].owner = %d (a water sector)\n",
		__file__,__line__, x, y, sct_ptr->owner);
	sct_ptr->owner = 0;
      }

    } /* for */
  } /* for */

} /* verify_sct() */

/*
 * verify_data - Master data integrity verification function
 *
 * Top-level function that performs complete game data validation by
 * calling all specialized verification routines. This is the main
 * entry point for comprehensive data integrity checking.
 *
 * Parameters:
 *   __file__ - Source file name for error reporting (usually __FILE__)
 *   __line__ - Source line number for error reporting (usually __LINE__)
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Calls verify_ntn() to check all nation data
 *   - Calls verify_sct() to check all sector data
 *   - Indirectly logs all violations and fixes via sub-functions
 *
 * Notes:
 *   - Central coordination point for all data validation
 *   - Used during updates and debug verification
 *   - Can be called from any location for data integrity checks
 */
void
verify_data PARM_2( char *, __file__, int, __line__ )
{
  /* check for invalid values */
  verify_ntn( __file__, __line__ );
  verify_sct( __file__, __line__ );
}

#ifdef DEBUG
/*
 * checkout - Debug trace function with data verification
 *
 * Debugging utility that prints current execution location and
 * performs complete data verification. Used for tracing execution
 * and catching data corruption at specific code points.
 *
 * Parameters:
 *   file - Source file name for trace output
 *   line - Source line number for trace output
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Prints location information to update log
 *   - Calls verify_data() for complete integrity check
 *
 * Notes:
 *   - Only compiled in DEBUG builds
 *   - Useful for tracking down data corruption sources
 *   - Can be placed at any code location for monitoring
 */
void
checkout PARM_2 ( char *, file, int, line )
{
  fprintf(fupdate,"file %s line %d\n", file, line);
  verify_data(file, line);
}
#endif /* DEBUG */

#ifdef LISTUSERS
/* code to store and list users who are logged in */
#ifndef UNAME
extern int gethostname(), ttyslot();
#endif /* UNAME */

/* STORE_USER -- Store information about the current user */
static void
store_user PARM_1(int, fdval)
{
  int numchars;
  char infostr[BIGLTH], hname[BIGLTH], termname[BIGLTH], dstr[BIGLTH];
#ifdef UNAME
  struct utsname name;
#endif /* UNAME */

  /* first check the file descriptor */
  if (fdval > 0) {

    /* build up the user and host information */
#ifdef UNAME
    if (uname(&name))
      strcpy(hname, "[unknown]");
    strcpy(hname, name.sysname);
#else
    if (gethostname(hname, BIGLTH - 1) != 0) {
      strcpy(hname, "[unknown]");
    }
#endif /* UNAME */
    snprintf(infostr, sizeof(infostr), "%.10s@%.488s", loginname, hname);
    strcpy(hname, infostr);

    /* build up the terminal information */
    if (ttyname(0) != NULL) {
      strcpy(termname, ttyname(0));
    } else {
      strcpy(termname, "[none]");
    }

    /* now find the date */
    strcpy(dstr, mach_time());

    /* finally, compse the whole thing */
    snprintf(infostr, sizeof(infostr), "%-18.18s %-12.12s %.467s", hname, termname, dstr);

    /* now output it */
    numchars = (strlen(infostr) + 1) * sizeof(char);
    if (write(fdval, &numchars, sizeof(int)) != sizeof(int)) {
      fprintf(fupdate, "Warning: could not store user information into file\n");
      return;
    }
    if (write(fdval, &(infostr[0]),  numchars) != numchars) {
      fprintf(fupdate, "Warning: could not store user information into file\n");
      return;
    }
  }
}

/* SHOW_USER -- Display the user information stored within the file */
static void
show_user PARM_1(int, fdval)
{
  int numchars;
  char infostr[BIGLTH];

  /* check file descriptor */
  if (fdval > 0) {

    /* now how much information needs to be read in? */
    if (read(fdval, &numchars, sizeof(int)) != sizeof(int)) {
      return;
    }

    /* now read in the proper amount of data */
    if (read(fdval, &(infostr[0]), numchars) != numchars) {
      return;
    }

    /* now send out the retrieved data */
    printf("%s", infostr);
  }
}

/* CANSEELOGIN -- If the current user is able to view the info */
static int
canseelogin PARM_0(void)
{
  return (!world.hide_login ||
	  (strcmp(loginname, LOGIN) == 0) ||
	  (strcmp(loginname, world.demigod) == 0));
}
#endif /* LISTUSERS */

/*
 * who_is_on - Display all users currently logged into the game
 *
 * Shows comprehensive information about current game activity including
 * active players, ongoing updates, and system status. Provides different
 * levels of detail based on user permissions and compilation options.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Prints campaign information and current turn details
 *   - Shows last update time if available
 *   - Checks for ongoing update or nation addition processes
 *   - Scans all nations for active login sessions
 *   - Displays user details if LISTUSERS is enabled and permissions allow
 *   - Counts and reports total active users
 *
 * Notes:
 *   - Uses file locking to detect active sessions
 *   - Different output based on LISTUSERS compilation flag
 *   - Respects privacy settings (hide_login option)
 *   - Essential tool for game administration and player coordination
 *   - Checks special lock files for system operations
 */
void
who_is_on PARM_0(void)
{
  FILE *timefp;
#ifdef LISTUSERS
  int fileid;
#endif /* LISTUSERS */
  int num_on = 0, ntnnum, found_some = FALSE;
  char filename[FILELTH], ntnstr[NAMELTH + 1];

  /* inform what is being checked */
  printf("\nCampaign: %s\n", datadirname);
  roman_number( string, YEAR(TURN) );
  printf("Turn %d: %s of Year %s\n",
	 TURN - START_TURN + 1, PMONTH(TURN),
	 string);

  /* show the time if it is there */
  if ((timefp = fopen(timefile, "r")) != NULL) {
    fgets(string, 50, timefp);
    string[strlen(string) - 1] = '\0';
    printf("Last Update: %s\n", string);
    fclose(timefp);
  }
  printf("\n");

  /* check for update in progress */
  sprintf(filename, "godup.%s", isontag);
  if (check_lock(filename, FALSE) == -1) {

#ifdef LISTUSERS
    /* open it for reading */
    if (canseelogin()) {
      if ((fileid = open(filename, O_RDONLY, 0444)) >= 0) {
	printf("%-12s=> ", "Update");
	show_user(fileid);
	close(fileid);
      }
    } else {
#endif /* LISTUSERS */
      printf("Conquer is currently updating.");
#ifdef LISTUSERS
    }
#endif /* LISTUSERS */
    found_some = TRUE;

  }

  /* check for player addition in progress */
  sprintf(filename, "godadd.%s", isontag);
  if (check_lock(filename, FALSE) == -1) {

#ifdef LISTUSERS
    if (canseelogin()) {
      /* open it for reading */
      if ((fileid = open(filename, O_RDONLY, 0444)) >= 0) {
	printf("%-12s=> ", "Adding");
	show_user(fileid);
	close(fileid);
      }
    } else {
#endif /* LISTUSERS */
      printf("Someone is currently adding a nation.\n");
#ifdef LISTUSERS
    }
#endif /* LISTUSERS */
    found_some = TRUE;

  }

  /* go through the list of all nations and detect who is on */
  country = -1;
  for (ntnnum = 0; ntnnum < MAXNTN; ntnnum++) {
    if ((ntnnum == UNOWNED) ||
	(world.np[ntnnum] != NULL)) {

      /* now check who is logged on */
      ntn_realname(ntnstr, ntnnum);
      sprintf(filename, "%s.%s", ntnstr, isontag);
      if (check_lock(filename, FALSE) == -1) {

#ifdef LISTUSERS
	if (canseelogin()) {
	  /* open it for reading */
	  if ((fileid = open(filename, O_RDONLY, 0444)) >= 0) {
	    printf("%-12s=> ", ntnstr);
	    show_user(fileid);
	    close(fileid);
	  }
	} else {
	  num_on++;
	}
#else /* LISTUSERS */
	printf("Nation %s is logged in.\n", ntnstr);
	num_on++;
#endif /* LISTUSERS */
	found_some = TRUE;

      }
    }
  }

  /* somebody found? */
  if (found_some == FALSE) {
    printf("Nobody is currently logged in.\n");
  } else if (num_on) {
    printf("There are %d nations currently logged in.\n", num_on);
  }
  printf("\n");
}

/*
 * check_lock - File locking mechanism for multi-user coordination
 *
 * Implements file-based locking to coordinate access between multiple
 * game processes and users. Prevents conflicts during updates, logins,
 * and other critical operations. Supports both real file locking and
 * timestamp-based locking depending on system capabilities.
 *
 * Parameters:
 *   filename - Name of lock file to check/create
 *   keeplock - If TRUE, maintain lock after checking; if FALSE, release immediately
 *
 * Returns:
 *   File descriptor (> 0) if lock acquired successfully
 *   0 if no lock needed or released
 *   -1 if file is already locked by another process
 *
 * Side Effects:
 *   - Creates lock file if keeplock is TRUE
 *   - Uses flock()/lockf() or timestamp checking based on FILELOCK setting
 *   - Stores user information in lock file if LISTUSERS is enabled
 *   - Removes stale lock files automatically (timestamp method)
 *   - May terminate program on compiler optimization detection
 *
 * Notes:
 *   - Critical for preventing data corruption in multi-user environment
 *   - Two locking strategies: FILELOCK (real locks) vs timestamps
 *   - Includes compiler optimization detection for Sun/gcc compatibility
 *   - Used throughout system for session management and update coordination
 */
int
check_lock PARM_2 (char *, filename, int, keeplock)
{
  int filed_id;
#ifdef FILELOCK
  int filed_id2;

  /* check if the file exists */
  filed_id = open(filename, O_WRONLY | O_CREAT, 0600);
  if (filed_id != -1) {

    /* is there a lock on the file? */
    filed_id2 = filed_id;
    if (do_lock(filed_id) == -1) {
      close(filed_id);
      filed_id = (-1);
    }

    /* check the value of the descriptor */
    if ((filed_id != -1) &&
	(filed_id != filed_id2)) {
      fprintf(fupdate, "possible error in compiler optimization: %d != %d\n",
	      filed_id, filed_id2);
      fprintf(fupdate, "If you are NOT compiling with gcc on a Sun let me know.\nBut, for now...\n");
      fprintf(fupdate,
	      "type \"cd Src; rm checkX.o; make CFLAGS='-g' checkX.o; make build\"\n");
      exit(FAIL);
    }

    /* remove lock after checking */
    if ((keeplock == FALSE) && (filed_id != -1)) {
      close(filed_id);
      filed_id = 0;
      unlink(filename);
    }

  } else {
    fprintf(fupdate, "Error opening lock file <%s>\n", filename);
  }
#else
  struct stat fst;

  /* check if the file exists and how old it is */
  filed_id = 0;
  if (stat( filename, &fst ) == 0 ) {
    long now;
    now = time(0);
    if (now - fst.st_mtime < TIME_DEAD * 3) {
      filed_id = (-1);
    } else {
      /* remove useless file */
      unlink(filename);
    }
  }

  /* does a lock need to be made? */
  if ((filed_id != -1) && (keeplock == TRUE)) {
    /* create lock file */
    if ((filed_id = open(filename, O_WRONLY | O_CREAT, 0600)) == (-1)) {
      fprintf(fupdate, "Error opening lock file <%s>\n", filename);
    }
  }
#endif /* FILELOCK */
#ifdef LISTUSERS
  if ((keeplock == TRUE) && (filed_id > 0)) {
    store_user(filed_id);
  }
#endif /* LISTUSERS */
  return(filed_id);
}

/*
 * kill_lock - Remove a previously acquired file lock
 *
 * Safely removes a file lock by closing the file descriptor and
 * unlinking the lock file. Provides error reporting if removal fails.
 *
 * Parameters:
 *   fid - File descriptor of the lock file to remove
 *   fname - Name of the lock file to unlink
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Closes the file descriptor
 *   - Removes the lock file from filesystem
 *   - Logs warning if removal fails
 *
 * Notes:
 *   - Always call this to properly release locks
 *   - Essential for preventing lock file accumulation
 *   - Used in conjunction with check_lock()
 */
void
kill_lock PARM_2(int, fid, char *, fname)
{
  /* only remove proper locks */
  if ((close(fid) == -1) || (unlink(fname) == -1)) {
    fprintf(fupdate, "Warning: lock file <%s> fd = %d not removed\n",
	    fname, fid);
  }
}

/*
 * user_exists - Validate if a username exists on the system
 *
 * Checks if the specified username corresponds to a valid system user
 * account. Used for validating nation login names and ensuring only
 * legitimate users can control nations.
 *
 * Parameters:
 *   who - Username string to validate
 *
 * Returns:
 *   TRUE if user exists on system, FALSE otherwise
 *
 * Side Effects:
 *   None
 *
 * Notes:
 *   - Uses getpwnam() on Unix systems for validation
 *   - Returns TRUE on VMS systems (simplified validation)
 *   - Critical for preventing unauthorized nation access
 *   - Called during nation login validation
 */
int
user_exists PARM_1 (char *, who)
{
#ifndef VAXC
  /* under Unix this will work */
  if (getpwnam(who) == NULL)
    return(FALSE);
  else return(TRUE);
#else
  /* just return TRUE for now */
  return(TRUE);
#endif /* VAXC */
}

/*
 * get_userid - Retrieve current user's login name
 *
 * Determines the login name of the currently executing user using
 * system calls. Provides fallback mechanisms for different Unix
 * variants and handles memory allocation for the result.
 *
 * Parameters:
 *   outname - Buffer to store username, or NULL to allocate new buffer
 *
 * Returns:
 *   Pointer to username string on success
 *   NULL on failure or if user cannot be determined
 *
 * Side Effects:
 *   - May allocate memory if outname is NULL
 *   - Calls abrt() on malloc failure
 *
 * Notes:
 *   - Uses getpwuid(getuid()) or cuserid() depending on availability
 *   - Essential for identifying users in multi-user environment
 *   - Handles both provided buffers and dynamic allocation
 *   - Platform-specific implementation variations
 */
char *
get_userid PARM_1(char *, outname)
{
#ifndef CUSERID
  struct passwd *pwtemp = NULL;

  /* retrieve from the user id under Unix */
  if ((pwtemp = getpwuid(getuid())) == NULL) {
    strcpy(outname, "");
    return ((char *) NULL);
  } else {
    /* store the loginname */
    if (outname == NULL) {
      if ((outname = (char *) malloc(sizeof(char) * 12)) == NULL) {
	fprintf(fupdate, "Malloc Failure");
	abrt();
      }
    }
    strcpy(outname, pwtemp->pw_name);
  }
  return(outname);
#else
  /* use portable getpwuid implementation instead of deprecated cuserid */
  struct passwd *pwtemp;
  
  if ((pwtemp = getpwuid(getuid())) == NULL) {
    return(NULL);
  } else {
    if (outname == NULL) {
      if ((outname = (char *) malloc(sizeof(char) * 12)) == NULL) {
        return(NULL);
      }
    }
    strcpy(outname, pwtemp->pw_name);
  }
  return(outname);
#endif /* CUSERID */
}
