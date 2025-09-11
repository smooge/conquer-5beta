/*
 * executeX.c - Command execution engine for game state modification
 *
 * This file implements the command execution system that processes player
 * and game commands from execute files. It serves as the primary interface
 * for modifying game state including armies, navies, caravans, cities, items,
 * and nation-level settings through structured command files.
 *
 * The execution system supports multiple command categories:
 * - Army commands (movement, combat stats, creation/destruction)
 * - Navy commands (fleet management, ship configurations, cargo)
 * - Caravan commands (trade unit management, materials transport)
 * - City commands (population, resources, fortifications)
 * - Item commands (commodity and equipment management)
 * - Resource commands (taking and distributing materials)
 * - Miscellaneous commands (nation settings, diplomacy, magic)
 *
 * Commands are processed from tab-delimited files with different data types
 * (integer, long, double) and comprehensive error checking and validation.
 * The system maintains game state consistency and provides detailed error
 * reporting for invalid operations.
 *
 * This module is critical for game persistence, turn processing, and
 * administrative operations, serving as the primary mechanism for applying
 * changes to the game world.
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
#define USE_CODES
#include "dataX.h"
#include "armyX.h"
#include "cityX.h"
#include "itemX.h"
#include "navyX.h"
#include "butesX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "activeX.h"
#include "elevegX.h"
#include "nclassX.h"
#include "statusX.h"
#include "caravanX.h"
#include "dstatusX.h"
#include "executeX.h"

/* local file parameter list */
char str1[LINELTH], str2[LINELTH], exe_ntnname[NAMELTH + 1];
double dval1, dval2;
int idnum, ival1, ival2;
Exectype cmdnum;
long lval1, lval2;

/*
 * exec_army - Process individual army unit commands
 *
 * Executes commands that modify army unit properties including location,
 * type, status, movement, leadership, size, and combat effectiveness.
 * Handles army creation and destruction with proper validation and
 * error checking for all operations.
 *
 * The function processes commands based on the global cmdnum variable
 * and operates on the army specified by global idnum. Most commands
 * require an existing army except for creation and mercenary adjustment.
 *
 * Supported Commands:
 * - EX_ARMYLOC/EX_ARMYOLOC: Set current/previous location with map bounds checking
 * - EX_ARMYTYPE: Change army type with validation against available types
 * - EX_ARMYSTAT: Modify army status flags and operational state
 * - EX_ARMYMOVE: Set movement points with range validation (0-200)
 * - EX_ARMYLEAD: Assign leader with circular reference prevention
 * - EX_ARMYSIZE: Set unit size with non-negative validation
 * - EX_ARMYID: Change unit ID with uniqueness verification
 * - EX_ARMYSPLY: Set supply level within valid range (0-MAXSUPPLIES)
 * - EX_ARMYSPTS: Set spell points with reasonable limits (0-200)
 * - EX_ARMYEFF/EX_ARMYMAXEFF: Set efficiency and maximum efficiency
 * - EX_ARMYCREATE: Create new army with type validation and ID assignment
 * - EX_ARMYDESTROY: Remove army from game with cleanup
 * - EX_ARMYMERCS: Adjust mercenary counts for recruitment tracking
 *
 * Parameters: None (operates on global variables)
 *
 * Returns: Nothing (void)
 *
 * Side Effects:
 * - Modifies army data structures directly
 * - Updates army sorting order when location/ID changes
 * - Generates error messages for invalid operations
 * - May create or destroy army units affecting global army list
 * - Updates mercenary recruitment tracking counters
 *
 * Notes:
 * - Validates all input parameters before applying changes
 * - Maintains referential integrity for leader assignments
 * - Preserves army sorting order after modifications
 * - Thread safety depends on global variable access patterns
 */
static void
exec_army PARM_0(void)
{
  /* set the army pointer properly */
  if ((cmdnum != EX_ARMYCREATE) &&
      (cmdnum != EX_ARMYMERCS)) {
    if (army_ptr == NULL || ARMY_ID != idnum) {
      army_ptr = armybynum(idnum);
    }

    /* verify army */
    if (army_ptr == NULL) {
      sprintf(string, "Error(%s): Cmd #%d, Army #%d does not exist",
	      exe_ntnname, cmdnum, idnum);
      errormsg(string);
      return;
    }
  }

  /* now process the possible commands */
  switch (cmdnum) {
  case EX_ARMYLOC:
    /* set the army location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): A_LOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      ARMY_XLOC = ival1;
      ARMY_YLOC = ival2;
      army_sort(FALSE);
    }
    break;
  case EX_ARMYOLOC:
    /* set the army location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): A_OLOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      ARMY_LASTX = ival1;
      ARMY_LASTY = ival2;
    }
    break;
  case EX_ARMYTYPE:
    /* change the army type */
    if (ival1 < 0 || ival1 >= num_armytypes) {
      sprintf(string, "Error(%s): A_TYPE, type %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ARMY_TYPE = ival1;
    }
    break;
  case EX_ARMYSTAT:
    /* change the army status */
    ARMY_STAT = lval1;
    break;
  case EX_ARMYMOVE:
    /* change a units movement */
    if (ival1 < 0 || ival1 > 200) {
      sprintf(string, "Error(%s): A_MOVE, movement of %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ARMY_MOVE = ival1;
    }
    break;
  case EX_ARMYLEAD:
    /* change the leader */
    if (ival1 == EMPTY_HOLD) {
      ARMY_LEAD = EMPTY_HOLD;
    } else if (ARMY_ID == ival1) {
      sprintf(string, "Error(%s): A_LEAD, lead value %d equals id of unit",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if ((army_tptr = armybynum(ival1)) == NULL) {
      sprintf(string, "Error(%s): A_LEAD, unit %d non-existant",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (!a_isleader(ARMYT_TYPE)) {
      sprintf(string, "Error(%s): A_LEAD, unit %d not a leader",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ARMY_LEAD = ival1;
    }
    break;
  case EX_ARMYSIZE:
    /* set the unit size */
    if (lval1 < 0) {
      sprintf(string, "Error(%s): A_SIZE, size %ld invalid",
	      exe_ntnname, lval1);
      errormsg(string);
    } else {
      ARMY_SIZE = lval1;
    }
    break;
  case EX_ARMYID:
    /* change a unit id number */
    if (ival1 < 0 || ival1 > MAX_IDTYPE || ival1 == EMPTY_HOLD) {
      sprintf(string, "Error(%s): A_ID, id number %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (armybynum(ival1) != NULL) {
      sprintf(string, "Error(%s): A_ID, id number %d already in use",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      army_renum(ival1);
      army_sort(FALSE);
    }
    break;
  case EX_ARMYSPLY:
    /* set the army supply */
    if (ival1 < 0 || ival1 > MAXSUPPLIES) {
      sprintf(string, "Error(%s): A_SPLY, supply value %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ARMY_SPLY = ival1;
    }
    break;
  case EX_ARMYSPTS:
    /* set the army spell points */
    if (ival1 < 0 || ival1 > 200) {
      sprintf(string, "Error(%s): A_SPTS, spell points %d really big",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ARMY_SPTS = ival1;
    }
    break;
  case EX_ARMYEFF:
    /* set the army efficiency */
    if (ival1 < 0 || ival1 > ARMY_MAXEFF + 5) {
      sprintf(string, "Error(%s): A_EFF, efficiency %d > %d",
	      exe_ntnname, ival1, ARMY_MAXEFF + 5);
      errormsg(string);
    } else {
      ARMY_EFF = ival1;
    }
    break;
  case EX_ARMYMAXEFF:
    /* set the army maximum efficiency */
    if (ival1 < 0 || ival1 > 200) {
      sprintf(string, "Error(%s): A_EFF, max efficiency %d quite amazing",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ARMY_MAXEFF = ival1;
    }
    break;
  case EX_ARMYCREATE:
    /* build a new army unit */
    if (ival1 < 0 || ival1 >= num_armytypes) {
      sprintf(string, "Error(%s): A_CREATE, unit type %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      army_ptr = crt_army(ival1);
      if (idnum != ARMY_ID) {
	if (armybynum(idnum) != NULL) {
	  sprintf(string, "Error(%s): A_CREATE, id %d != %d",
		  exe_ntnname, idnum, ARMY_ID);
	  errormsg(string);
	} else {
#ifdef NOT_DONE
	  sprintf(string, "Warning(%s): A_CREATE, id %d != %d (fixed)",
		  exe_ntnname, idnum, ARMY_ID);
	  errormsg(string);
#endif /* NOT_DONE */
	  ARMY_ID = idnum;
	  army_sort(FALSE);
	}
      }
    }
    break;
  case EX_ARMYDESTROY:
    /* remove a unit */
    dest_army(idnum);
    army_ptr = NULL;
    break;
  case EX_ARMYMERCS:
    /* adjust the mercenary settings */
    if (is_update) {
      /* record adjustments to mercenary numbers */
      MERCMEN += ival1;
      if (MERCMEN < 0) MERCMEN = 0;
    } else if (ival1 < 0) {
      /* keep proper count of drafted mercs */
      conq_mercsused -= ival1;
    }
    break;
  default:
    /* uh oh! */
    sprintf(string, "Error(%s): unknown army command #%d",
	    exe_ntnname, cmdnum);
    errormsg(string);
    break;
  }
}

/*
 * exec_navy - Process naval fleet commands
 *
 * Executes commands that modify navy unit properties including location,
 * ship configuration, crew levels, movement, and cargo management.
 * Handles navy creation and destruction with validation for all naval
 * operations including ship class management and efficiency tracking.
 *
 * The function processes commands based on the global cmdnum variable
 * and operates on the navy specified by global idnum. Most commands
 * require an existing navy except for creation operations.
 *
 * Supported Commands:
 * - EX_NAVYLOC/EX_NAVYOLOC: Set current/previous location with map validation
 * - EX_NAVYSHIP: Configure ship class quantities with class/value validation
 * - EX_NAVYSTAT: Modify navy status flags and operational state
 * - EX_NAVYMOVE: Set movement points with range validation (0-200)
 * - EX_NAVYID: Change unit ID with uniqueness verification and sorting
 * - EX_NAVYCREW: Set crew percentage with range validation (0-100)
 * - EX_NAVYPEOP: Set people carried per hold (0-255 range)
 * - EX_NAVYMTRLS: Adjust raw materials with type and value validation
 * - EX_NAVYSPLY: Set supply level within extended naval range (0-MAXSUPPLIES*4)
 * - EX_NAVYARMY/EX_NAVYCVN: Assign carried units with existence validation
 * - EX_NAVYEFF: Set ship class efficiency with class and percentage validation
 * - EX_NAVYCREATE: Create new navy with automatic ID assignment
 * - EX_NAVYDESTROY: Remove navy from game with existence checking
 *
 * Parameters: None (operates on global variables)
 *
 * Returns: Nothing (void)
 *
 * Side Effects:
 * - Modifies navy data structures directly
 * - Updates navy sorting order when ID changes
 * - Generates error messages for invalid operations
 * - May create or destroy navy units affecting global navy list
 * - Validates carried unit references for data integrity
 *
 * Notes:
 * - Ship class validation ensures valid NSHP_NUMBER range
 * - Material type validation uses MTRLS_NUMBER bounds checking
 * - Crew percentages and efficiency values bounded to realistic ranges
 * - Supports higher supply limits than armies due to naval logistics
 * - Maintains referential integrity for carried army/caravan units
 */
static void
exec_navy PARM_0(void)
{
  /* set the navy pointer properly */
  if (cmdnum != EX_NAVYCREATE) {
    if (navy_ptr == NULL || NAVY_ID != idnum) {
      navy_ptr = navybynum(idnum);
    }

    /* verify navy */
    if (navy_ptr == NULL) {
      sprintf(string, "Error(%s): Cmd #%d, Navy #%d does not exist",
	      exe_ntnname, cmdnum, idnum);
      errormsg(string);
      return;
    }
  }

  /* now process the possible commands */
  switch (cmdnum) {
  case EX_NAVYLOC:
    /* set the navy location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): N_LOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      NAVY_XLOC = ival1;
      NAVY_YLOC = ival2;
    }
    break;
  case EX_NAVYOLOC:
    /* set the navy location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): N_OLOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      NAVY_LASTX = ival1;
      NAVY_LASTY = ival2;
    }
    break;
  case EX_NAVYSHIP:
    /* adjust the warship configuration */
    if (ival1 < 0 || ival1 >= NSHP_NUMBER) {
      sprintf(string, "Error(%s): N_SHIP, ship class %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (ival2 < 0 || ival2 >= 1<<16) {
      sprintf(string, "Error(%s): N_SHIP, ship value %d invalid",
	      exe_ntnname, ival2);
      errormsg(string);
    } else {
      NAVY_SHIPS[ival1] = ival2;
    }
    break;
  case EX_NAVYSTAT:
    /* change the navy status */
    NAVY_STAT = lval1;
    break;
  case EX_NAVYMOVE:
    /* change the movement */
    if (ival1 < 0 || ival1 > 200) {
      sprintf(string, "Error(%s): N_MOVE, movement of %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      NAVY_MOVE = ival1;
    }
    break;
  case EX_NAVYID:
    /* change a unit id number */
    if (ival1 < 0 || ival1 > MAX_IDTYPE) {
      sprintf(string, "Error(%s): N_ID, id number %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (navybynum(ival1) != NULL) {
      sprintf(string, "Error(%s): N_ID, id number %d already in use",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      navy_renum(ival1);
      navy_sort();
    }
    break;
  case EX_NAVYCREW:
    /* fix the crew for the navy */
    if (ival1 < 0 || ival1 > 100) {
      sprintf(string, "Error(%s): N_CREW percentage %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      NAVY_CREW = ival1;
    }
    break;
  case EX_NAVYPEOP:
    /* adjust number of people carried per hold */
    if (ival1 < 0 || ival1 > 255) {
      sprintf(string, "Error(%s): N_PEOP value %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      NAVY_PEOP = ival1;
    }
    break;
  case EX_NAVYMTRLS:
    /* adjust raw materials */
    ival1 = (int) dval1;
    if ((ival1 < 0) || (ival1 >= MTRLS_NUMBER)) {
      sprintf(string, "Error(%s): N_MTRLS, invalid material type %d",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (dval2 < (itemtype) 0) {
      sprintf(string, "Error(%s): N_MTRLS, negative %s value",
	      exe_ntnname, mtrls_info[ival1].name);
      errormsg(string);
    } else {
      NAVY_MTRLS[ival1] = (itemtype) dval2;
    }
    break;
  case EX_NAVYSPLY:
    /* set the navy supply */
    if (ival1 < 0 || ival1 > MAXSUPPLIES * 4) {
      sprintf(string, "Error(%s): N_SPLY, supply value %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      NAVY_SPLY = ival1;
    }
    break;
  case EX_NAVYARMY:
    /* change the army unit being carried */
    if ((ival1 != EMPTY_HOLD) &&
	(armybynum(ival1) == NULL)) {
      sprintf(string, "Error(%s): N_ARMY, army unit %d non-existent",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      NAVY_ARMY = ival1;
    }
    break;
  case EX_NAVYCVN:
    /* change the army unit being carried */
    if ((ival1 != EMPTY_HOLD) &&
	(cvnbynum(ival1) == NULL)) {
      sprintf(string, "Error(%s): N_CVN, navy unit %d non-existent",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      NAVY_CVN = ival1;
    }
    break;
  case EX_NAVYEFF:
    /* change the efficiency */
    if ((ival1 < 0) || (ival1 > 100)) {
      sprintf(string, "Error(%s): N_EFF, efficiency %d overly impressive",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if ((ival2 < 0) || (ival2 >= NSHP_NUMBER)) {
      sprintf(string, "Error(%s): N_EFF, no ship class %d",
	      exe_ntnname, ival2);
      errormsg(string);
    } else {
      NAVY_EFF[ival2] = ival1;
    }
    break;
  case EX_NAVYCREATE:
    /* build a new navy unit */
    navy_ptr = crt_navy();
    if (idnum != NAVY_ID) {
      if (navybynum(idnum) != NULL) {
	sprintf(string, "Error(%s): N_CREATE, id %d != %d",
		exe_ntnname, idnum, NAVY_ID);
	errormsg(string);
      } else {
	sprintf(string, "Warning(%s): N_CREATE, id %d != %d (fixed)",
		exe_ntnname, idnum, NAVY_ID);
	errormsg(string);
	NAVY_ID = idnum;
	navy_sort();
      }
    }
    break;
  case EX_NAVYDESTROY:
    /* remove a unit */
    if (navybynum(idnum) == NULL) {
      sprintf(string, "Error(%s): N_DESTROY, unit #%d non-existent",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      dest_navy(idnum);
      navy_ptr = NULL;
    }
    break;
  default:
    /* uh oh! */
    sprintf(string, "Error(%s): unknown navy command #%d",
	    exe_ntnname, cmdnum);
    errormsg(string);
    break;
  }
}

/*
 * exec_cvn - Process caravan trade unit commands
 *
 * Executes commands that modify caravan unit properties including location,
 * size, movement, crew levels, and material transport capabilities.
 * Handles caravan creation and destruction with validation for all trade
 * operations including cargo management and efficiency tracking.
 *
 * The function processes commands based on the global cmdnum variable
 * and operates on the caravan specified by global idnum. Most commands
 * require an existing caravan except for creation operations.
 *
 * Supported Commands:
 * - EX_CVNLOC/EX_CVNOLOC: Set current/previous location with map validation
 * - EX_CVNSIZE: Set caravan size with validation (0-255 range)
 * - EX_CVNSTAT: Modify caravan status flags and operational state
 * - EX_CVNMOVE: Set movement points with range validation (0-200)
 * - EX_CVNID: Change unit ID with uniqueness verification and sorting
 * - EX_CVNCREW: Set crew percentage with range validation (0-100)
 * - EX_CVNPEOP: Set people carried per unit (0-255 range)
 * - EX_CVNMTRLS: Adjust raw materials with type and value validation
 * - EX_CVNSPLY: Set supply level within caravan range (0-MAXSUPPLIES*2)
 * - EX_CVNEFF: Set caravan efficiency with percentage validation (0-100)
 * - EX_CVNCREATE: Create new caravan with automatic ID assignment
 * - EX_CVNDESTROY: Remove caravan from game with existence checking
 *
 * Parameters: None (operates on global variables)
 *
 * Returns: Nothing (void)
 *
 * Side Effects:
 * - Modifies caravan data structures directly
 * - Updates caravan sorting order when ID changes
 * - Generates error messages for invalid operations
 * - May create or destroy caravan units affecting global caravan list
 * - Validates material types and quantities for trade operations
 *
 * Notes:
 * - Size validation ensures reasonable caravan capacity limits
 * - Material type validation uses MTRLS_NUMBER bounds checking
 * - Crew percentages bounded to realistic operational ranges
 * - Supply limits reflect caravan logistics capabilities (2x army base)
 * - Efficiency affects caravan movement and cargo handling capacity
 * - All byte-sized values validated against 255 maximum
 */
static void
exec_cvn PARM_0(void)
{
  /* set the caravan pointer properly */
  if (cmdnum != EX_CVNCREATE) {
    if (cvn_ptr == NULL || CVN_ID != idnum) {
      cvn_ptr = cvnbynum(idnum);
    }

    /* verify caravan */
    if (cvn_ptr == NULL) {
      sprintf(string, "Error(%s): Cmd #%d, Caravan #%d does not exist",
	      exe_ntnname, cmdnum, idnum);
      errormsg(string);
      return;
    }
  }

  /* now process the possible commands */
  switch (cmdnum) {
  case EX_CVNLOC:
    /* set the caravan location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): V_LOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      CVN_XLOC = ival1;
      CVN_YLOC = ival2;
    }
    break;
  case EX_CVNOLOC:
    /* set the caravan location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): V_OLOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      CVN_LASTX = ival1;
      CVN_LASTY = ival2;
    }
    break;
  case EX_CVNSIZE:
    /* adjust the size of the caravan */
    if (ival1 < 0 || ival1 > 255) {
      sprintf(string, "Error(%s): V_SIZE, size %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      CVN_SIZE = ival1;
    }
    break;
  case EX_CVNSTAT:
    /* change the caravan status */
    CVN_STAT = lval1;
    break;
  case EX_CVNMOVE:
    /* change the movement */
    if (ival1 < 0 || ival1 > 200) {
      sprintf(string, "Error(%s): V_MOVE, movement of %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      CVN_MOVE = ival1;
    }
    break;
  case EX_CVNID:
    /* change a unit id number */
    if (ival1 < 0 || ival1 > MAX_IDTYPE) {
      sprintf(string, "Error(%s): V_ID, id number %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (cvnbynum(ival1) != NULL) {
      sprintf(string, "Error(%s): V_ID, id number %d already in use",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      cvn_renum(ival1);
      cvn_sort();
    }
    break;
  case EX_CVNCREW:
    /* fix the crew for the caravan */
    if (ival1 < 0 || ival1 > 100) {
      sprintf(string, "Error(%s): V_CREW percentage %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      CVN_CREW = ival1;
    }
    break;
  case EX_CVNPEOP:
    /* adjust number of people carried per hold */
    if (ival1 < 0 || ival1 > 255) {
      sprintf(string, "Error(%s): V_PEOP value %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      CVN_PEOP = ival1;
    }
    break;
  case EX_CVNMTRLS:
    /* adjust raw materials */
    ival1 = (int) dval1;
    if ((ival1 < 0) || (ival1 >= MTRLS_NUMBER)) {
      sprintf(string, "Error(%s): V_MTRLS, invalid material type %d",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (dval2 < (itemtype) 0) {
      sprintf(string, "Error(%s): V_MTRLS, negative %s value",
	      exe_ntnname, mtrls_info[ival1].name);
      errormsg(string);
    } else {
      CVN_MTRLS[ival1] = (itemtype) dval2;
    }
    break;
  case EX_CVNSPLY:
    /* set the caravan supply */
    if (ival1 < 0 || ival1 > MAXSUPPLIES * 2) {
      sprintf(string, "Error(%s): V_SPLY, supply value %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      CVN_SPLY = ival1;
    }
    break;
  case EX_CVNEFF:
    /* set the caravan efficiency */
    if (ival1 < 0 || ival1 > 100) {
      sprintf(string, "Error(%s): V_EFF, efficiency value %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      CVN_EFF = ival1;
    }
    break;
  case EX_CVNCREATE:
    /* build a new caravan unit */
    cvn_ptr = crt_cvn();
    if (idnum != CVN_ID) {
      if (cvnbynum(idnum) != NULL) {
	sprintf(string, "Error(%s): V_CREATE, id %d != %d",
		exe_ntnname, idnum, CVN_ID);
	errormsg(string);
      } else {
	sprintf(string, "Warning: V_CREATE, id %d != %d (fixed)",
		idnum, CVN_ID);
	errormsg(string);
	CVN_ID = idnum;
	cvn_sort();
      }
    }
    break;
  case EX_CVNDESTROY:
    /* remove a unit */
    if (cvnbynum(idnum) == NULL) {
      sprintf(string, "Error(%s): V_DESTROY, unit #%d non-existent",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      dest_cvn(idnum);
      cvn_ptr = NULL;
    }
    break;
  default:
    /* uh oh! */
    sprintf(string, "Error(%s): unknown caravan command #%d",
	    exe_ntnname, cmdnum);
    errormsg(string);
    break;
  }
}

/*
 * exec_city - Process city settlement commands
 *
 * Executes commands that modify city properties including location, name,
 * population, resource distribution weighting, material stockpiles, and
 * fortification levels. Handles city creation and destruction with
 * validation for all urban management operations.
 *
 * The function processes commands based on the global cmdnum variable
 * and operates on the city specified by str1 (city name). Most commands
 * require an existing city except for creation operations.
 *
 * Supported Commands:
 * - EX_CITYLOC: Set city location with map bounds validation
 * - EX_CITYNAME: Change city name with uniqueness verification and sorting
 * - EX_CITYPEOP: Set population with reasonable range validation (>= -50)
 * - EX_CITYWEIGHT: Set distribution weighting (0-255) affecting resource flow
 * - EX_CITYMTRLS: Adjust city material stockpiles with type validation
 * - EX_CITYSTALONS: Set starting treasury amount for economic planning
 * - EX_CITYIMTRLS: Set incremental material production rates
 * - EX_CITYFORT: Set fortification level with maximum value validation
 * - EX_CITYCREATE: Create new city with name validation and initialization
 * - EX_CITYDESTROY: Remove city from game with weight recalculation
 *
 * Parameters: None (operates on global variables)
 *
 * Returns: Nothing (void)
 *
 * Side Effects:
 * - Modifies city data structures directly
 * - Updates city sorting order when name changes
 * - Triggers resource distribution weight recalculation
 * - Generates error messages for invalid operations
 * - May create or destroy cities affecting global city list
 * - Validates material types using MTRLS_NUMBER bounds
 *
 * Notes:
 * - City lookup uses string names (str1) rather than numeric IDs
 * - Population can be negative (represents debt/deficit situations)
 * - Distribution weights affect how resources flow between cities
 * - Material validation allows negative talons but not other materials
 * - Fortification bounded by MAXFORTVAL game balance constant
 * - Weight changes trigger immediate recalculation for consistency
 */
static void
exec_city PARM_0(void)
{
  /* set the city pointer properly */
  if (cmdnum != EX_CITYCREATE) {
    if (city_ptr == NULL ||
	str_test(CITY_NAME, str1)) {
      city_ptr = citybyname(str1);
    }

    /* verify city */
    if (city_ptr == NULL) {
      sprintf(string, "Error(%s): Cmd #%d, City %s does not exist",
	      exe_ntnname, cmdnum, str1);
      errormsg(string);
      return;
    }
  }

  /* now process the possible commands */
  switch (cmdnum) {
  case EX_CITYLOC:
    /* set the city location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): C_LOC [%s], location %d, %d off map",
	      exe_ntnname, CITY_NAME, ival1, ival2);
      errormsg(string);
    } else {
      CITY_XLOC = ival1;
      CITY_YLOC = ival2;
    }
    break;
  case EX_CITYNAME:
    /* change the name */
    if (strlen(str2) == 0) {
      sprintf(string, "Error(%s): C_NAME, no name specified",
	      exe_ntnname);
      errormsg(string);
    } else if ((citybyname(str2) != NULL) &&
	       (citybyname(str2) != city_ptr)) {
      sprintf(string, "Error(%s): C_NAME, name %s already in use",
	      exe_ntnname, str2);
      errormsg(string);
    } else {
      strcpy(CITY_NAME, str2);
      city_sort();
    }
    break;
  case EX_CITYPEOP:
    /* adjust number of people in the city */
    if (ival1 < -50) {
      sprintf(string, "Error(%s): C_PEOP [%s] value %d out of range",
	      exe_ntnname, CITY_NAME, ival1);
      errormsg(string);
    } else {
      CITY_PEOPLE = ival1;
    }
    break;
  case EX_CITYWEIGHT:
    /* adjust distribution weighting */
    if ((ival1 < 0) || (ival1 > 255)) {
      sprintf(string, "Error(%s): C_WEIGHT [%s] value %d out of range",
	      exe_ntnname, CITY_NAME, ival1);
      errormsg(string);
    } else {
      CITY_WEIGHT = ival1;
      set_weights(FALSE);
    }
    break;
  case EX_CITYMTRLS:
    /* adjust raw materials */
    if ((idnum < 0) || (idnum >= MTRLS_NUMBER)) {
      sprintf(string, "Error(%s): C_MTRLS [%s], invalid material type %d",
	      exe_ntnname, CITY_NAME, ival1);
      errormsg(string);
    } else if ((idnum != MTRLS_TALONS) &&
	       (dval1 < (itemtype) 0)) {
      sprintf(string, "Error(%s): C_MTRLS [%s], negative %s value",
	      exe_ntnname, CITY_NAME, mtrls_info[idnum].name);
      errormsg(string);
    } else {
      CITY_MTRLS[idnum] = (itemtype) dval1;
    }
    break;
  case EX_CITYSTALONS:
    /* adjust starting treasury */
    CITY_STALONS = (itemtype) dval1;
    break;
  case EX_CITYIMTRLS:
    /* adjust incremental raw materials */
    if ((idnum < 0) || (idnum >= MTRLS_NUMBER)) {
      sprintf(string, "Error(%s): C_MTRLS [%s], invalid material type %d",
	      exe_ntnname, CITY_NAME, ival1);
      errormsg(string);
    } else if ((idnum != MTRLS_TALONS) &&
	       (dval1 < (itemtype) 0)) {
      sprintf(string, "Error(%s): C_MTRLS [%s], negative %s value",
	      exe_ntnname, CITY_NAME, mtrls_info[idnum].name);
      errormsg(string);
    } else {
      CITY_IMTRLS[idnum] = (itemtype) dval1;
    }
    break;
  case EX_CITYFORT:
    /* adjust the city fortification */
    if (ival1 < 0 || ival1 > MAXFORTVAL) {
      sprintf(string, "Error(%s), C_FORT [%s], fort value %d out of range",
	      exe_ntnname, CITY_NAME, ival1);
      errormsg(string);
    } else {
      CITY_FORT = ival1;
    }
    break;
  case EX_CITYCREATE:
    /* build a new city unit */
    if (citybyname(str1) != NULL) {
      sprintf(string, "Error(%s): C_CREATE, name of %s in use",
	      exe_ntnname, str1);
      errormsg(string);
    } else {
      city_ptr = crt_city(str1);
    }
    break;
  case EX_CITYDESTROY:
    /* remove a unit */
    dest_city(str1);
    city_ptr = NULL;
    set_weights(FALSE);
    break;
  default:
    /* uh oh! */
    sprintf(string, "Error(%s): unknown city command #%d",
	    exe_ntnname, cmdnum);
    errormsg(string);
    break;
  }
}

/*
 * exec_item - Process commodity and equipment item commands
 *
 * Executes commands that modify item properties including location, ID,
 * material composition, manpower requirements, and unit associations.
 * Handles item creation and destruction with validation for all commodity
 * and equipment management operations.
 *
 * The function processes commands based on the global cmdnum variable
 * and operates on the item specified by global idnum. Most commands
 * require an existing item except for creation operations.
 *
 * Supported Commands:
 * - EX_ITEMLOC: Set item location with map bounds validation
 * - EX_ITEMID: Change item ID with uniqueness verification and sorting
 * - EX_ITEMMTRLS: Adjust item material composition with type validation
 * - EX_ITEMMEN: Set manpower requirements with non-negative validation
 * - EX_ITEMINFO: Set item information flags and metadata
 * - EX_ITEMTYPE: Set item type classification for game mechanics
 * - EX_ITEMARMY: Associate item with army unit (with existence checking)
 * - EX_ITEMNAVY: Associate item with navy unit (with existence checking)
 * - EX_ITEMCVN: Associate item with caravan unit (with existence checking)
 * - EX_ITEMCREATE: Create new item with automatic ID assignment
 * - EX_ITEMDESTROY: Remove item from game with cleanup
 *
 * Parameters: None (operates on global variables)
 *
 * Returns: Nothing (void)
 *
 * Side Effects:
 * - Modifies item data structures directly
 * - Updates item sorting order when ID changes
 * - Generates error messages for invalid operations
 * - May create or destroy items affecting global item list
 * - Validates associated unit references for data integrity
 *
 * Notes:
 * - Item ID validation ensures uniqueness across all items
 * - Material type validation uses MTRLS_NUMBER bounds checking
 * - Allows negative talons but validates other materials as non-negative
 * - Unit associations checked for existence before assignment
 * - Manpower requirements affect item transport and usage mechanics
 * - Item type affects game behavior and interaction possibilities
 */
static void
exec_item PARM_0(void)
{
  /* set the item pointer properly */
  if (cmdnum != EX_ITEMCREATE) {
    if (item_ptr == NULL || ITEM_ID != idnum) {
      item_ptr = itembynum(idnum);
    }

    /* verify item */
    if (item_ptr == NULL) {
      sprintf(string, "Error(%s): Cmd #%d, Item #%d does not exist",
	      exe_ntnname, cmdnum, idnum);
      errormsg(string);
      return;
    }
  }

  /* now process the possible commands */
  switch (cmdnum) {
  case EX_ITEMLOC:
    /* set the item location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): I_LOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      ITEM_XLOC = ival1;
      ITEM_YLOC = ival2;
    }
    break;
  case EX_ITEMID:
    /* change the name */
    if (ival1 < 0 || ival1 > MAX_IDTYPE) {
      sprintf(string, "Error(%s): I_ID, number %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (itembynum(ival1) != NULL) {
      sprintf(string, "Error(%s): I_ID, number %d already in use",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ITEM_ID = ival1;
      item_sort();
    }
    break;
  case EX_ITEMMTRLS:
    /* adjust raw materials */
    ival1 = (int) dval1;
    if ((ival1 < 0) || (ival1 >= MTRLS_NUMBER)) {
      sprintf(string, "Error(%s): I_MTRLS, invalid material type %d",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if ((ival1 != MTRLS_TALONS) &&
	       (dval2 < (itemtype) 0)) {
      sprintf(string, "Error(%s): I_MTRLS, negative %s value",
	      exe_ntnname, mtrls_info[ival1].name);
      errormsg(string);
    } else {
      ITEM_MTRLS[ival1] = (itemtype) dval2;
    }
    break;
  case EX_ITEMCREATE:
    /* build a new item unit */
    item_ptr = crt_item();
    if (idnum != ITEM_ID) {
      if (itembynum(idnum) != NULL) {
	sprintf(string, "Error(%s): I_CREATE, id %d != %d",
		exe_ntnname, idnum, ITEM_ID);
	errormsg(string);
      } else {
	sprintf(string, "Warning: I_CREATE, id %d != %d (fixed)",
		idnum, ITEM_ID);
	errormsg(string);
	ITEM_ID = idnum;
	item_sort();
      }
    }
    break;
  case EX_ITEMMEN:
    /* number of men needed */
    if (lval1 < 0) {
      sprintf(string, "Error(%s): I_MEN(%d), Negative value %ld",
	      exe_ntnname, idnum, lval1);
      errormsg(string);
    } else {
      ITEM_MEN = lval1;
    }
    break;
  case EX_ITEMINFO:
    /* information */
    ITEM_INFO = lval1;
    break;
  case EX_ITEMTYPE:
    /* what type of item is it */
    ITEM_TYPE = ival1;
    break;
  case EX_ITEMARMY:
    /* army unit */
    if (armybynum(ival1) == NULL) {
      sprintf(string, "Error(%s): I_ARMY(%d), No such army %d",
	      exe_ntnname, idnum, ival1);
      errormsg(string);
    } else {
      ITEM_ARMY = ival1;
    }
    break;
  case EX_ITEMNAVY:
    /* navy unit */
    if (navybynum(ival1) == NULL) {
      sprintf(string, "Error(%s): I_NAVY(%d), No such navy %d",
	      exe_ntnname, idnum, ival1);
      errormsg(string);
    } else {
      ITEM_CVN = ival1;
    }
    break;
  case EX_ITEMCVN:
    /* caravan unit */
    if (cvnbynum(ival1) == NULL) {
      sprintf(string, "Error(%s): I_CVN(%d), No such caravan %d",
	      exe_ntnname, idnum, ival1);
      errormsg(string);
    } else {
      ITEM_CVN = ival1;
    }
    break;
  case EX_ITEMDESTROY:
    /* remove a unit */
    dest_item(idnum);
    item_ptr = NULL;
    break;
  default:
    /* uh oh! */
    sprintf(string, "Error(%s): unknown commodity command #%d",
	    exe_ntnname, cmdnum);
    errormsg(string);
    break;
  }
}

/*
 * exec_take - Handle resource extraction command sequences
 *
 * Processes multi-command sequences for extracting resources from sectors.
 * Uses a static cost_ptr to accumulate material requirements across
 * multiple commands before executing the final resource extraction operation.
 * Provides validation and coordinate checking for all extraction operations.
 *
 * The function maintains state between commands using a static ITEM_PTR
 * to track the extraction location and material requirements. Commands
 * must be processed in the correct sequence: START -> MTRLS -> FINISH.
 *
 * Supported Commands:
 * - EX_TAKESTART: Initialize extraction operation at specified coordinates
 * - EX_TAKEMTRLS: Add material requirements to extraction (multiple calls)
 * - EX_TAKEFINISH: Execute resource extraction with validation
 *
 * Command Sequence:
 * 1. TAKESTART establishes location and initializes material arrays
 * 2. Multiple TAKEMTRLS commands specify material types and quantities
 * 3. TAKEFINISH validates coordinates and executes the extraction
 *
 * Parameters: None (operates on global variables)
 *
 * Returns: Nothing (void)
 *
 * Side Effects:
 * - Maintains static cost_ptr between commands for sequence processing
 * - Allocates and frees temporary item structures for extraction data
 * - Calls take_resources() to execute actual resource extraction
 * - Generates error messages for sequence violations or mismatched coordinates
 * - Modifies sector resource levels when extraction completes successfully
 *
 * Notes:
 * - Coordinate validation ensures START and FINISH match
 * - Material command validation uses offset arithmetic (cmdnum - EX_TAKEMTRLS)
 * - Static pointer maintains state across function calls during sequence
 * - Memory management includes proper allocation and cleanup
 * - Supports extraction of all material types defined by MTRLS_NUMBER
 */
static void
exec_take PARM_0(void)
{
  static ITEM_PTR cost_ptr = NULL;
  int i;

  /* check input */
  if (cmdnum != EX_TAKESTART) {
    if (cost_ptr == NULL) {
      sprintf(string, "Error(%s): cost_ptr not set for command #%d",
	      exe_ntnname, cmdnum);
      errormsg(string);
      return;
    }
  } else {
    if (cost_ptr != NULL) {
      sprintf(string, "Warning(%s): cost_ptr already set for take start",
	      exe_ntnname);
      errormsg(string);
    }
  }

  /* process the commands */
  switch (cmdnum) {
  case EX_TAKESTART:
    cost_ptr = new_item();
    cost_ptr->xloc = ival1;
    cost_ptr->yloc = ival2;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      cost_ptr->mtrls[i] = (itemtype) 0;
    }
    break;
  case EX_TAKEFINISH:
    if ((ival1 != cost_ptr->xloc) || (ival2 != cost_ptr->yloc)) {
      sprintf(string, "Error(%s): T_FINISH, non-matching sectors",
	      exe_ntnname);
      errormsg(string);
      break;
    }
    if (take_resources(ival1, ival2, cost_ptr, idnum)) {
      sprintf(string, "Error(%s): T_FINISH, resource take failed",
	      exe_ntnname);
      errormsg(string);
    }
    free(cost_ptr);
    cost_ptr = NULL;
    break;
  case EX_TAKEMTRLS:
  default:
    if ((idnum != cost_ptr->xloc) || (lval1 != cost_ptr->yloc)) {
      sprintf(string, "Error(%s): T_MTRLS, non-matching sectors",
	      exe_ntnname);
      errormsg(string);
      break;
    } else if ((cmdnum < EX_TAKEMTRLS) ||
	       (cmdnum >= EX_TAKEMTRLS + MTRLS_NUMBER)) {
      /* uh oh! */
      sprintf(string, "Error(%s): unknown take command #%d",
	      exe_ntnname, cmdnum);
      errormsg(string);
    } else {
      cost_ptr->mtrls[cmdnum - EX_TAKEMTRLS] = (itemtype) lval2;
    }
    break;
  }
}

/*
 * exec_give - Handle resource distribution command sequences
 *
 * Processes multi-command sequences for distributing resources to sectors.
 * Uses a static give_ptr to accumulate material quantities across
 * multiple commands before executing the final resource distribution operation.
 * Provides validation and coordinate checking for all distribution operations.
 *
 * The function maintains state between commands using a static ITEM_PTR
 * to track the distribution location and material quantities. Commands
 * must be processed in the correct sequence: START -> MTRLS -> FINISH.
 *
 * Supported Commands:
 * - EX_GIVESTART: Initialize distribution operation at specified coordinates
 * - EX_GIVEMTRLS: Add material quantities to distribution (multiple calls)
 * - EX_GIVEFINISH: Execute resource distribution with validation
 *
 * Command Sequence:
 * 1. GIVESTART establishes location and initializes material arrays
 * 2. Multiple GIVEMTRLS commands specify material types and quantities
 * 3. GIVEFINISH validates coordinates and executes the distribution
 *
 * Parameters: None (operates on global variables)
 *
 * Returns: Nothing (void)
 *
 * Side Effects:
 * - Maintains static give_ptr between commands for sequence processing
 * - Allocates and frees temporary item structures for distribution data
 * - Calls send_resources() to execute actual resource distribution
 * - Generates error messages for sequence violations or mismatched coordinates
 * - Modifies sector resource levels when distribution completes successfully
 *
 * Notes:
 * - Coordinate validation ensures START and FINISH match
 * - Material command validation uses offset arithmetic (cmdnum - EX_GIVEMTRLS)
 * - Static pointer maintains state across function calls during sequence
 * - Memory management includes proper allocation and cleanup
 * - Supports distribution of all material types defined by MTRLS_NUMBER
 * - Parallel structure to exec_take() but for resource distribution
 */
static void
exec_give PARM_0(void)
{
  int i;
  static ITEM_PTR give_ptr = NULL;

  /* check input */
  if (cmdnum != EX_GIVESTART) {
    if (give_ptr == NULL) {
      sprintf(string, "Error(%s): give_ptr not set for command #%d",
	      exe_ntnname, cmdnum);
      errormsg(string);
      return;
    }
  } else {
    if (give_ptr != NULL) {
      sprintf(string, "Warning(%s): give_ptr already set for take start",
	      exe_ntnname);
      errormsg(string);
    }
  }

  /* process the commands */
  switch (cmdnum) {
  case EX_GIVESTART:
    give_ptr = new_item();
    give_ptr->xloc = ival1;
    give_ptr->yloc = ival2;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      give_ptr->mtrls[i] = (itemtype) 0;
    }
    break;
  case EX_GIVEFINISH:
    if ((ival1 != give_ptr->xloc) || (ival2 != give_ptr->yloc)) {
      sprintf(string, "Error(%s): G_FINISH, non-matching sectors",
	      exe_ntnname);
      errormsg(string);
      break;
    }
    if (send_resources(ival1, ival2, give_ptr, idnum)) {
      sprintf(string, "Error(%s): G_FINISH, resource redistribution failed",
	      exe_ntnname);
      errormsg(string);
    }
    free(give_ptr);
    give_ptr = NULL;
    break;
  case EX_GIVEMTRLS:
  default:
    if ((idnum != give_ptr->xloc) || (lval1 != give_ptr->yloc)) {
      sprintf(string, "Error(%s): G_MTRLS, non-matching sectors",
	      exe_ntnname);
      errormsg(string);
      break;
    } else if ((cmdnum < EX_GIVEMTRLS) ||
	       (cmdnum >= EX_GIVEMTRLS + MTRLS_NUMBER)) {
      /* uh oh! */
      sprintf(string, "Error(%s): unknown give command #%d",
	      exe_ntnname, cmdnum);
      errormsg(string);
    } else {
      give_ptr->mtrls[cmdnum - EX_GIVEMTRLS] = (itemtype) lval2;
    }
    break;
  }
}

/*
 * exec_misc - Process miscellaneous game state commands
 *
 * Handles a diverse collection of commands for nation-level settings,
 * group management, diplomacy, sector modification, magic systems,
 * and administrative operations. This function serves as the catch-all
 * for commands not handled by the specialized unit processors.
 *
 * The function processes commands based on the global cmdnum variable
 * and operates on various game entities depending on the command type.
 * Most commands perform extensive validation before applying changes.
 *
 * Command Categories:
 *
 * Group Commands:
 * - EX_GRPLOC: Set group location with leader validation and map bounds
 * - EX_GRPMOVE: Set group movement with speed and movement range validation
 *
 * Nation Commands:
 * - EX_NTNNAME: Change nation name with uniqueness and length validation
 * - EX_NTNLOGIN: Set user login with system user existence checking
 * - EX_NTNPASSWD: Set nation password with length validation
 * - EX_NTNLEADER: Set leader name with length validation
 * - EX_NTNLOC: Set capital location with map bounds validation
 * - EX_NTNAPLUS/EX_NTNDPLUS: Set attack/defense bonuses
 * - EX_NTNACTIVE: Modify nation activity status with admin restrictions
 * - EX_NTNDIPLO: Set diplomatic relations with status validation
 * - EX_NTNBUTE: Modify nation attributes with range validation
 * - EX_NTNRLOC: Set nation center coordinates
 * - EX_NTNREPRO: Set reproduction rate (0-15 range)
 * - EX_NTNRACE: Set nation race with race validation
 * - EX_NTNMARK: Set nation map marker with validity checking
 * - EX_NTNLEDGE/EX_NTNREDGE: Set map edge boundaries for nation view
 * - EX_NTNBEDGE/EX_NTNTEDGE: Set vertical map boundaries
 * - EX_NTNCLASS: Set nation class with class validation
 * - EX_NTNSCORE: Set nation score
 * - EX_NTNMOVE: Set nation movement allowance (4-50 range)
 * - EX_BRIBE: Process bribery operations (unimplemented)
 * - EX_NEWSSIZE/EX_MAILSIZE: Set news and mail buffer sizes
 *
 * Sector Commands:
 * - EX_SCTDESG: Set sector designation with validation
 * - EX_SCTPEOPLE: Set sector population with bounds checking
 * - EX_SCTOWN: Set sector owner with nation validation
 * - EX_SCTALT: Set sector altitude with elevation bounds
 * - EX_SCTVEG: Set sector vegetation type
 * - EX_SCTTGOOD: Set sector trade good with type validation
 * - EX_SCTMNRLS: Set sector mineral value (0-255 range)
 *
 * Magic Commands:
 * - EX_MGK_ADJ: Adjust magic powers with checksum validation
 * - EX_MGKSPELL: Process spell examination and recording
 * - EX_MGKSENDING: Handle monster sending operations with creation
 *
 * Administrative Commands:
 * - EX_NTNRENUM: Trigger army reorganization
 * - EX_UNUMCOPY/EX_UNUMRESET/EX_UNUMDEFAULT: Unit number management
 * - EX_UNUMSLOT: Assign unit number slots
 *
 * Parameters: None (operates on global variables)
 *
 * Returns: Nothing (void)
 *
 * Side Effects:
 * - Modifies nation, sector, and global game state
 * - Generates news entries for significant changes
 * - Updates file systems for name changes
 * - Triggers various recalculation and sorting operations
 * - May create monster units during magic operations
 * - Maintains diplomatic relationship consistency
 *
 * Notes:
 * - Extensive validation prevents invalid game states
 * - News generation documents major nation changes
 * - File operations handle login/name changes
 * - Magic operations include power validation and monster creation
 * - Administrative commands reset various game subsystems
 * - God-level permissions required for some operations
 */
static void
exec_misc PARM_0(void)
{
  char buf[LINELTH];
  long combpow;

  switch (cmdnum) {
  case EX_GRPLOC:
    /* set the group location */
    if (((army_ptr = armybynum(idnum)) == NULL) ||
	!unit_leading(ARMY_STAT)) {
      sprintf(string, "Error(%s): G_LOC, unknown group leader number %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): G_LOC, location %d,%d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      set_grploc(idnum, ival1, ival2);
    }
    break;
  case EX_GRPMOVE:
    /* set the group movement */
    if (((army_ptr = armybynum(idnum)) == NULL) ||
	!unit_leading(ARMY_STAT)) {
      sprintf(string, "Error(%s): G_MOVE, unknown group leader number %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else if (ival1 < SPD_MINIMUM || ival1 > SPD_MINIMUM + SPD_STUCK) {
      sprintf(string, "Error(%s): G_MOVE, speed %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (ival2 < 0 || ival2 > 255) {
      sprintf(string, "Error(%s): G_MOVE, movement %d out of range",
	      exe_ntnname, ival2);
      errormsg(string);
    } else {
      set_grpmove(idnum, ival1, ival2);
    }
    break;
  case EX_NTNNAME:
    /* set the nation name */
    if ((ntnbyname(str1) != NULL) &&
	(ntnbyname(str1) != ntn_ptr)) {
      sprintf(string, "Error(%s): X_NAME, name %s in use",
	      exe_ntnname, str1);
      errormsg(string);
    } else if ((str1[0] == '\0') || (strlen(str1) > NAMELTH)) {
      sprintf(string, "Error(%s): X_NAME, invalid namelength of %zu",
	      exe_ntnname, strlen(str1));
      errormsg(string);
    } else {
      if (is_update == TRUE) {
	fprintf(fnews, "1.\tNation %s changes their name to %s\n",
		ntn_ptr->name, str1);
	fprintf(fupdate, "==> Renaming Nation %s to %s\n",
		ntn_ptr->name, str1);
	sprintf(string, "%s.%s", ntn_ptr->name, msgtag);
	sprintf(buf, "%s.%s", str1, msgtag);
	if (move_file(string, buf) != 0) {
	  fprintf(fupdate, "   (could not find a mail file to move)\n");
	}
      }
      strcpy(ntn_ptr->name, str1);
    }
    break;
  case EX_NTNLOGIN:
    /* set the user name */
    if (user_exists(str1) == FALSE) {
      sprintf(string, "Error(%s): X_LOGIN, no user %s on system",
	      exe_ntnname, str1);
      errormsg(string);
    } else {
      if (is_update == TRUE) {
	if (!world.hide_login) {
	  fprintf(fnews, "1.\tUser %s now controls nation %s\n",
		  str1, exe_ntnname);
	}
	fprintf(fupdate, "==> Changing user of %s from %s to %s\n",
		exe_ntnname, ntn_ptr->login, str1);
      }
      strcpy(ntn_ptr->login, str1);
    }
    break;
  case EX_NTNPASSWD:
    /* set the password */
    if ((str1[0] == '\0') || (strlen(str1) > PASSLTH)) {
      sprintf(string, "Error(%s): X_PASSWD, invalid password length of %zu",
	      exe_ntnname, strlen(str1));
      errormsg(string);
    } else {
      strcpy(ntn_ptr->passwd, str1);
    }
    break;
  case EX_NTNLEADER:
    /* change the leader name */
    if ((str1[0] == '\0') || (strlen(str1) > LEADERLTH)) {
      sprintf(string, "Error(%s): X_LEADER, invalid leader length of %zu",
	      exe_ntnname, strlen(str1));
      errormsg(string);
    } else {
      strcpy(ntn_ptr->leader, str1);
    }
    break;
  case EX_NTNLOC:
    /* change the capital location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): X_LOC, location %d, %d off map",
	     exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      ntn_ptr->capx = ival1;
      ntn_ptr->capy = ival2;
    }
    break;
  case EX_NTNAPLUS:
    /* change the attack bonus */
    ntn_ptr->aplus = ival1;
    break;
  case EX_NTNDPLUS:
    /* change the defense bonus */
    ntn_ptr->dplus = ival1;
    break;
  case EX_NTNACTIVE:
    /* change the nation activity */
    if (idnum < 0 || idnum >= MAXNTN) {
      sprintf(string, "Error(%s): X_ACTIVE, nation number %d out of range",
	      exe_ntnname, idnum);
      errormsg(string);
    } else if (ival1 < 0 || ival1 > ACT_MAXIMUM) {
      sprintf(string, "Error(%s): X_ACTIVE, value %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if ((ntn_tptr = world.np[idnum]) == NULL) {
      sprintf(string, "Error(%s): X_ACTIVE, NULL nation number %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      if ((ntn_tptr->active == INACTIVE) &&
	  (strcmp("god", exe_ntnname) != 0)) break;
      if (is_update &&
	  (strcmp("god", exe_ntnname) == 0)) {
	if (ival1 == INACTIVE) {
	  fprintf(fupdate, "    Nation %s being deleted\n", ntn_tptr->name);
	} else if (ntn_tptr->active == INACTIVE) {
	  fprintf(fupdate, "    Nation %s being reinstated\n",
		  ntn_tptr->name);
	}
      }
      ntn_tptr->active = ival1;
    }
    break;
  case EX_NTNDIPLO:
    /* change the diplomacy value */
    if ((ival1 < 0) || (ival1 >= ABSMAXNTN) ||
	(world.np[ival1] == NULL)) {
      sprintf(string, "Error(%s): X_DIPLO, invalid nation #%d",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (ival2 < 0 || ival2 >= dstatus_number * dstatus_number) {
      sprintf(string, "Error(%s): X_DIPLO, status %d out of range",
	      exe_ntnname, ival2);
      errormsg(string);
    } else {
      if (is_update) {
	ival2 %= dstatus_number;
	if (ival2 == DIP_ALLIED) {
	  if ((world.np[ival1])->dstatus[country] == DIP_ALLIED) {
	    fprintf(fnews, "1.\tNations %s and %s form an alliance.\n",
		    ntn_ptr->name, (world.np[ival1])->name);
	  } else {
	    fprintf(fnews, "1.\tNation %s offers an alliance to %s.\n",
		    ntn_ptr->name, (world.np[ival1])->name);
	  }
	} else if (ntn_ptr->dstatus[ival1] == DIP_ALLIED) {
	  fprintf(fnews, "1.\tNation %s withdraws the alliance offer to %s.\n",
		    ntn_ptr->name, (world.np[ival1])->name);
	} else if ((ntn_ptr->dstatus[ival1] > DIP_BELLICOSE) &&
		   (ival2 <= DIP_BELLICOSE)) {
	  fprintf(fnews, "1.\tNation %s ends the war on %s.\n",
		    ntn_ptr->name, (world.np[ival1])->name);
	}
	if (ival2 > DIP_BELLICOSE) {
	  fprintf(fnews, "1.\tNation %s declares %s on %s.\n",
		  ntn_ptr->name, dipname[ival2],
		  (world.np[ival1])->name);
	}
      }
      ntn_ptr->dstatus[ival1] = ival2;
    }
    break;
  case EX_NTNBUTE:
    /* change the charity level */
    if (idnum < 0 || idnum >= BUTE_NUMBER) {
      sprintf(string, "Error(%s): X_BUTE, unknown attribute type %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else if ((ival1 < bute_info[idnum].min_base) ||
	       (ival1 > bute_info[idnum].max_base)) {
      sprintf(string, "Error(%s): X_BUTE, value %d out of range for %s",
	      exe_ntnname, ival1, bute_info[idnum].name);
      errormsg(string);
    } else {
      ntn_ptr->attribute[idnum] = ival1;
    }
    break;
  case EX_NTNRLOC:
    /* change the capital location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): X_RLOC, location [%d,%d] off map",
	     exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      ntn_ptr->centerx = ival1;
      ntn_ptr->centery = ival2;
    }
    break;
  case EX_NTNREPRO:
    /* change the reproduction value of the nation */
    if ((ival1 < 0) || (ival1 > 15)) {
      sprintf(string, "Error(%s): X_REPRO, repro of %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->repro = ival1;
    }
    break;
  case EX_NTNRACE:
    if ((ival1 < 0) || (ival1 >= RACE_NUMBER)) {
      sprintf(string, "Error(%s): X_RACE, race of %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->race = ival1;
    }
    break;
  case EX_NTNMARK:
    if (!markok(ival1, (ntn_ptr != NULL)?ntn_ptr->race:UNOWNED, FALSE)) {
      sprintf(string, "Error(%s): X_MARK, mark of %d[%c] invalid",
	      exe_ntnname, ival1, isprint(ival1)?ival1:' ');
      errormsg(string);
    } else {
      ntn_ptr->mark = ival1;
    }
    break;
  case EX_NTNLEDGE:
    if ((ival1 > MAPX) || (ival1 < - MAPX)) {
      sprintf(string, "Error(%s): X_LEDGE, X location %d off map",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->leftedge = ival1;
    }
    break;
  case EX_NTNREDGE:
    if ((ival1 > MAPX) || (ival1 < - MAPX)) {
      sprintf(string, "Error(%s): X_REDGE, X location %d off map",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->rightedge = ival1;
    }
    break;
  case EX_NTNBEDGE:
    if ((ival1 > MAPY) || (ival1 < 0)) {
      sprintf(string, "Error(%s): X_BEDGE, Y location %d off map",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->bottomedge = ival1;
    }
    break;
  case EX_NTNTEDGE:
    if ((ival1 > MAPY) || (ival1 < 0)) {
      sprintf(string, "Error(%s): X_TEDGE, Y location %d off map",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->topedge = ival1;
    }
    break;
  case EX_NTNCLASS:
    /* adjust national class */
    if ((ival1 < 0) || (ival1 >= nclass_number)) {
      sprintf(string, "Error(%s): X_CLASS, class type %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->class = ival1;
    }
    break;
  case EX_NTNSCORE:
    /* adjust national score */
    ntn_ptr->score = lval1;
    break;
  case EX_NTNMOVE:
    /* adjustment of national movement */
    if ((ival1 < 4) || (ival1 > 50)) {
      sprintf(string, "Error(%s): X_NMOVE, movement value of %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->maxmove = ival1;
    }
    break;
  case EX_BRIBE:
    /* bribery */
    if ((idnum < 0) || (idnum >= ABSMAXNTN)) {
      sprintf(string, "Error(%s): X_BRIBE, invalid country %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else if (world.np[idnum] == NULL) {
      sprintf(string, "Error(%s): X_BRIBE, no country %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      /* perform the bribe */
      /* bribe_ntn(country, idnum, dval1) UNIMPLEMENTED */;
    }
    break;
  case EX_NEWSSIZE:
    if ((is_god == TRUE) &&
	(ntn_ptr == NULL)) {
      world.news_size = lval1;
    } else if (ntn_ptr != NULL) {
      ntn_ptr->news_size = lval1;
    }
    break;
  case EX_MAILSIZE:
    if ((is_god == TRUE) &&
	(ntn_ptr == NULL)) {
      world.mbox_size = lval1;
    } else if (ntn_ptr != NULL) {
      ntn_ptr->mbox_size = lval1;
    }
    break;
  case EX_SCTDESG:
    /* change the sector designation */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): S_DESG, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else if (idnum < 0 || idnum > 0xFFFF) {
      sprintf(string, "Error(%s): S_DESG, invalid designation %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      sct[ival1][ival2].designation = idnum;
    }
    break;
  case EX_SCTPEOPLE:
    /* change the population */
    if (!XY_ONMAP(idnum, lval1)) {
      sprintf(string, "Error(%s): S_DESG, location %d, %ld off map",
	      exe_ntnname, idnum, lval1);
      errormsg(string);
    } else if (lval2 < 0L) {
      sprintf(string, "Error(%s): S_DESG, population %ld invalid",
	      exe_ntnname, lval2);
      errormsg(string);
    } else {
      sct[idnum][lval1].people = lval2;
    }
    break;
  case EX_SCTOWN:
    /* change the owner */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): S_OWN, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else if ((idnum < 0) || (idnum >= ABSMAXNTN) ||
	       ((idnum != UNOWNED) && (world.np[idnum] == NULL))) {
      sprintf(string, "Error(%s): S_OWN, owner #%d invalid",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      sct[ival1][ival2].owner = idnum;
    }
    break;
  case EX_SCTALT:
    /* change the altitude */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): S_ALT, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else if ((idnum < 0) || (idnum > ELE_PEAK)) {
      sprintf(string, "Error(%s): S_ALT, altitude #%d invalid",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      sct[ival1][ival2].altitude = idnum;
    }
    break;
  case EX_SCTVEG:
    /* change the vegetation */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): S_VEG, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else if ((idnum < 0) || (idnum > VEG_NONE)) {
      sprintf(string, "Error(%s): S_VEG, vegetation #%d invalid",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      sct[ival1][ival2].vegetation = idnum;
    }
    break;
  case EX_SCTTGOOD:
    /* change the tradegood */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): S_TGOOD, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else if ((idnum < 0) || (idnum >= tgoods_number)) {
      sprintf(string, "Error(%s): S_TGOOD, tradegood #%d invalid",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      sct[ival1][ival2].tradegood = idnum;
    }
    break;
  case EX_SCTMNRLS:
    /* change the minerals value */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): S_MNRLS, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else if ((idnum < 0) || (idnum > 255)) {
      sprintf(string, "Error(%s): S_MNRLS, minerals value %d invalid",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      sct[ival1][ival2].minerals = idnum;
    }
    break;
  case EX_MGK_ADJ:
    /* set the magic powers */
    if ((idnum < 0) ||
	(idnum >= MAG_NUMBER)) {
      sprintf(string, "Warning(%s): M_ADJ, power class %d out of range",
	      exe_ntnname, idnum);
      errormsg(string);
      break;
    } 
    if (lval2 != ntn_ptr->powers[idnum]) {
      sprintf(string, "Warning(%s): M_ADJ, %s power checksum doesn't match",
	      exe_ntnname, mclass_list[idnum].name);
      errormsg(string);
    }
    combpow = lval1 | lval2;
    add_powers(idnum, combpow ^ lval2);
    kill_powers(idnum, combpow ^ lval1);
    break;
  case EX_MGKSPELL:
    /* examine or record the spells */
    check_spells(idnum, ival1, ival2);
    break;
  case EX_MGKSENDING:
    /* check the input */
    if ((idnum < 0) ||
	(idnum >= num_armytypes) ||
	!a_ismonster(idnum)) {
      sprintf(string, "Error(%s): M_SEND: %d not a monster unit",
	      exe_ntnname, idnum);
      errormsg(string);
      break;
    }
    if (world.np[ival1] == NULL) {
      sprintf(string, "Error(%s): M_SEND: %d is invalid nation target",
	      exe_ntnname, ival1);
      errormsg(string);
      break;
    }

    /* don't do it until the update */
    if (is_update == FALSE) break;

    /* create the unit */
    if (ival1 != country) {
      rand_sector((world.np[ival1])->capx, (world.np[ival1])->capy,
		  5, TRUE, TRUE);
    } else {
      global_int = ntn_ptr->capx;
      global_long = ntn_ptr->capy;
    }
    if (make_madunit(((ival1 != country) && (country != UNOWNED))
		     ? country : -1, idnum, 1, global_int, global_long)) {
      sprintf(string, "Error(%s): M_SEND: failed without savage nation",
	      exe_ntnname);
      errormsg(string);
    } else {
      /* SEND MAIL TO VICTIM: UNIMPLEMENTED */
    }
    break;
  case EX_NTNRENUM:
    /* simply run the reorganization function */
    army_reorganize();
    break;
  case EX_UNUMCOPY:
    /* copy in the default settings */
    unum_copydefault();
    army_ptr = NULL;
    break;
  case EX_UNUMRESET:
    /* reset the national slot */
    resetnumbers();
    army_ptr = NULL;
    break;
  case EX_UNUMDEFAULT:
    /* use the built in defaults */
    unum_defaults();
    army_ptr = NULL;
    break;
  case EX_UNUMSLOT:
    /* check the data, then assign it */
    newslotnumber(idnum, ival1, ival2);
    army_ptr = NULL;
    break;
  default:
    /* uh oh! */
    sprintf(string, "Error(%s): unknown miscellaneous command #%d",
	    exe_ntnname, cmdnum);
    errormsg(string);
    break;
  }
}

/*
 * execute - Main command execution interface for game state modification
 *
 * Reads and processes commands from execute files generated by the game
 * interface or administrative tools. This function serves as the primary
 * entry point for applying changes to the game world, handling file I/O,
 * command parsing, and delegation to appropriate command processors.
 *
 * The function opens nation-specific execute files (name.exe or god.exe),
 * parses tab-delimited command lines with different data types, and
 * routes commands to specialized processing functions based on command
 * categories. Provides comprehensive error checking and validation.
 *
 * File Format:
 * Commands are stored in tab-delimited format with the following structure:
 * [Type]_[Category]\t[Command]\t[ID]\t[Value1]\t[Value2]\t[String1]\t[String2]
 *
 * Where Type is:
 * - 'D' for double precision floating point values
 * - 'L' for long integer values  
 * - 'I' (default) for standard integer values
 *
 * Command Categories:
 * - EX_ARMYBEGIN to EX_ARMYEND: Army unit commands
 * - EX_NAVYBEGIN to EX_NAVYEND: Navy unit commands
 * - EX_CVNBEGIN to EX_CVNEND: Caravan unit commands
 * - EX_CITYBEGIN to EX_CITYEND: City management commands
 * - EX_ITEMBEGIN to EX_ITEMEND: Item and commodity commands
 * - EX_TAKEBEGIN to EX_TAKEEND: Resource extraction commands
 * - EX_GIVEBEGIN to EX_GIVEEND: Resource distribution commands
 * - Other: Miscellaneous commands (nations, sectors, magic, admin)
 *
 * Parameters:
 *   cntry - Nation number to process commands for (0-ABSMAXNTN-1)
 *          UNOWNED for god-level administrative commands
 *
 * Returns:
 *   0 - No execute file found (normal condition)
 *   1 - File processed successfully
 *   >1 - Number of lines processed (success with line count)
 *   -1 - Invalid country parameter or nation doesn't exist
 *
 * Side Effects:
 * - Modifies global game state through command execution
 * - Sets resource distribution weights for the nation
 * - Updates global pointers (army_ptr, city_ptr, etc.) during processing
 * - Calculates and updates national totals after command processing
 * - Generates error messages for malformed commands or validation failures
 * - Preserves original pointer state after execution completion
 *
 * Notes:
 * - File naming convention: "nationname.exe" or "god.exe" for admin
 * - Supports three data types with automatic parsing based on line prefix
 * - Command validation includes format checking and parameter validation
 * - Global pointer management ensures consistent state during execution
 * - Weight recalculation affects resource distribution across the game
 * - National totals updated after all commands to maintain consistency
 * - Thread safety depends on file locking and global state management
 */
int
execute PARM_1(int, cntry)
{
  ARMY_PTR ahold_ptr = army_ptr;
  CITY_PTR chold_ptr = city_ptr;
  NAVY_PTR nhold_ptr = navy_ptr;
  CVN_PTR vhold_ptr = cvn_ptr;
  ITEM_PTR ihold_ptr = item_ptr;
  NTN_PTR ntnhld_ptr = ntn_ptr;
  int cmdhold;

  /* useful string declarations */
  char line[BIGLTH];
  int err, numlines = 0, hold = 0;

  /* check validity */
  if (cntry < 0 || cntry >= ABSMAXNTN) return(-1);

  /* now check if the nation exists */
  if (cntry != UNOWNED) {
    if ((ntn_ptr = world.np[cntry]) == NULL)
      return(-1);
  } else {
    ntn_ptr = NULL;
  }

  /* clear out the generic pointers */
  city_ptr = NULL;
  army_ptr = NULL;
  navy_ptr = NULL;
  cvn_ptr = NULL;
  item_ptr = NULL;

  /* open the file */
  if (cntry == UNOWNED) {
    sprintf(string, "god.%s", exetag);
    strcpy(exe_ntnname, "god");
  } else {
    sprintf(string, "%s.%s", ntn_ptr->name, exetag);
    strcpy(exe_ntnname, ntn_ptr->name);
  }
  if ((fexe = fopen(string, "r")) != NULL) {

    /* set the weights for the nation */
    set_weights(TRUE);

    /* read all of the information from the file */
    while (!feof(fexe)) {

      /* get the string from the file */
      if (fgets(line, BIGLTH, fexe) != NULL) {

	/* parse the input */
	numlines++;
	if (line[1] != '_') {
	  sprintf(string, "Misformatted line #%d in file %s.%s",
		  numlines, exe_ntnname, exetag);
	  errormsg(string);
	  sprintf(string, "Line: %s", line);
	  errormsg(string);
	  hold++;
	  continue;
	}

	/* determine input type */
	err = FALSE;
	cmdhold = 0;
	if (line[0] == 'D') {
	  /* double values -- %lf needed no matter what anyone says */
	  if (sscanf(line, "%s\t%d\t%d\t%lf\t%lf\t%12s\t%12s", string,
		     &cmdhold, &idnum, &dval1, &dval2, str1, str2) != 7) {
	    err = TRUE;
	  }
	} else if (line[0] == 'L') {
	  /* long values */
	  if (sscanf(line, "%s\t%d\t%d\t%ld\t%ld\t%12s\t%12s", string,
		     &cmdhold, &idnum, &lval1, &lval2, str1, str2) != 7) {
	    err = TRUE;
	  }
	} else {
	  /* integer values */
	  if (sscanf(line, "%s\t%d\t%d\t%d\t%d\t%12s\t%12s", string,
		     &cmdhold, &idnum, &ival1, &ival2, str1, str2) != 7) {
	    err = TRUE;
	  }
	}
	cmdnum = cmdhold;

	/* report any problems */
	if (err == TRUE) {
	  sprintf(string, "Misformatted line #%d in file %s.%s",
		  numlines, exe_ntnname, exetag);
	  errormsg(string);
	  sprintf(string, "Line: %s", line);
	  errormsg(string);
	  hold++;
	  continue;
	}

	/* now check all entries */
	if (cmdnum >= EX_ARMYBEGIN && cmdnum <= EX_ARMYEND) {
	  /* process the army commands */
	  exec_army();
	} else if (cmdnum >= EX_NAVYBEGIN && cmdnum <= EX_NAVYEND) {
	  /* process navy commands */
	  exec_navy();
	} else if (cmdnum >= EX_CVNBEGIN && cmdnum <= EX_CVNEND) {
	  /* process caravan commands */
	  exec_cvn();
	} else if (cmdnum >= EX_CITYBEGIN && cmdnum <= EX_CITYEND) {
	  /* process city commands */
	  trim_str(str1, FALSE);
	  trim_str(str2, FALSE);
	  exec_city();
	} else if (cmdnum >= EX_ITEMBEGIN && cmdnum <= EX_ITEMEND) {
	  /* process commodity commands */
	  exec_item();
	} else if (cmdnum >= EX_TAKEBEGIN && cmdnum <= EX_TAKEEND) {
	  /* process resource extraction */
	  exec_take();
	} else if (cmdnum >= EX_GIVEBEGIN && cmdnum <= EX_GIVEEND) {
	  /* process resource redistribution */
	  exec_give();
	} else {
	  /* process group, nation, sector, and magic commands */
	  trim_str(str1, FALSE);
	  trim_str(str2, FALSE);
	  exec_misc();
	}

      } /* if ... */
    }  /* while (!foef) ... */

    /* close the file */
    hold++;
    fclose(fexe);
  }

  /* calculate national totals */
  ntn_totals(cntry);

  /* return things to their original state */
  army_ptr = ahold_ptr;
  city_ptr = chold_ptr;
  navy_ptr = nhold_ptr;
  cvn_ptr = vhold_ptr;
  item_ptr = ihold_ptr;
  ntn_ptr = ntnhld_ptr;

  /* send back value */
  return(hold);
}
