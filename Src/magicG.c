/* The display routines for magic powers and interface to spells */
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
#define PRINT_CODES
#include "dataG.h"
#include "armyX.h"
#include "cityX.h"
#include "moveX.h"
#include "navyX.h"
#include "butesX.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "magicX.h"
#include "racesX.h"
#include "worldX.h"
#include "activeX.h"
#include "spellsX.h"
#include "statusX.h"
#include "caravanX.h"
#include "dstatusX.h"
#include "keyvalsX.h"

/* indicator for whether or not the magic is casting from a shrine */
int shrine_helped = 0;

/*
 * spellok - Check if the given nation is able to cast a spell
 *
 * Validates whether a spell can be cast by checking multiple requirements:
 * prerequisites, magical knowledge, unit casting ability, spell points,
 * physical health, and locale restrictions. Handles special cases for
 * god mode and shrine assistance bonuses.
 *
 * Parameters:
 *   spl_num - The spell type/index to validate
 *   pts - Available spell points for casting
 *   hlth - Current physical health of the caster
 *   showwhy - Whether to display error messages if spell cannot be cast
 *
 * Returns:
 *   1 if spell can be cast successfully
 *   0 if spell cannot be cast (but no error)
 *   -1 if spell cannot be cast due to error condition
 *
 * Side Effects:
 *   - May display error messages if showwhy is TRUE
 *   - Accesses global variables: is_god, ntn_ptr, army_ptr, city_ptr
 *   - Uses shrine_helped for cost/health reduction calculations
 *
 * Notes:
 *   - God mode bypasses most restrictions except focus nation requirements
 *   - Shrine assistance reduces spell costs and health drain
 *   - Transmutation spells (class 'R') require supply center location
 *   - Monster units have restrictions on certain spell types
 */
static int
spellok PARM_4(Spelltype, spl_num, int, pts, int, hlth, int, showwhy)
{
  int costferspell;

  /* can he get away with everything? */
  if (is_god == TRUE) {
    if (spl_num == SPL_TRANSFER) {
      if (showwhy) {
	errormsg("God assigns spell points directly, no need to tranfer");
      }
      return(0);
    }
    if (spl_num == SPL_TRANSMUTE) {
      if (showwhy) {
	errormsg("God can adjust materials directly, no need to transmute");
      }
      return(0);
    }
    if (ntn_ptr == NULL) {
      if ((spell_list[spl_num].class != 'S') &&
	  (spell_list[spl_num].class != 'M')) {
	if (showwhy)
	  errormsg("That spell cannot be cast without a focus nation");
	return(-1);
      }
    }
    return(1);
  }

  /* check all of the national powers */
  if ( !(MIL_MAGIC(spell_list[spl_num].pow_need[MAG_MILITARY]) &&
	 CIV_MAGIC(spell_list[spl_num].pow_need[MAG_CIVILIAN]) &&
	 WIZ_MAGIC(spell_list[spl_num].pow_need[MAG_WIZARDRY])) ) {
    if (showwhy) errormsg("Your nation has no knowledge of that spell");
    return(0);
  }

  /* check the casting of leaders from statuses */
  if ((army_ptr != NULL) &&
      (is_god == FALSE)) {
    if (nocast_stat(real_stat(army_ptr))) {
      if (showwhy) {
	errormsg("Your unit cannot cast spells under that status");
      }
      return(-1);
    }
    if (!si_anycaster(spl_num)) {
      if (si_nonmonster(spl_num) &&
	  a_ismonster(ARMY_TYPE)) {
	if (showwhy) {
	  errormsg("Monsters are unable to cast that spell");
	}
	return(-1);
      }
      if (si_needcaster(spl_num) &&
	  !a_castspells(ARMY_TYPE)) {
	if (showwhy) {
	  errormsg("That unit is not able to cast that spell");
	}
	return(-1);
      }
      if (si_needfull(spl_num) &&
	  !a_fullcaster(ARMY_TYPE)) {
	if (showwhy) {
	  errormsg("That unit is not skilled enough to perform that spell");
	}
	return(-1);
      }
    }
  }

  /* check for decreased costs */
  costferspell = spell_list[spl_num].cost;
  if (shrine_helped) {
    costferspell *= (100 - shrine_helped * 2);
    costferspell /= 100;
  }

  /* now check the amount of casting possible */
  if ((spell_list[spl_num].class != 'U') &&
      (spell_list[spl_num].class != 'u') &&
      (costferspell > pts)) {
    if (showwhy)
      errormsg("Your leader lacks the magical strength to cast that spell");
    return(-1);
  }

  /* check the locale */
  if (spell_list[spl_num].class == 'R') {
    if ((city_ptr = citybyloc(ntn_ptr, XREAL, YREAL)) == NULL) {
      if (showwhy) {
	errormsg("Transmutation should be done in a supply center");
      }
      return(-1);
    }
  }

  /* now check the physical requirements */
  if (shrine_helped) {
    hlth *= 100;
    if (shrine_helped < 25) {
      hlth /= (100 - shrine_helped * 4);
    }
  }
  if (spell_list[spl_num].drain > hlth) {
    if (showwhy)
      errormsg("Your leader is too weak physically to cast that spell");
    return(-1);
  }

  /* So, what do ya know, it works! */
  return(1);
}

/*
 * work_spell - Execute the actual spell effects and mechanics
 *
 * Performs the core implementation of spell casting by handling each
 * spell type's specific effects. Manages spell point costs, target
 * validation, resource consumption, and unit modifications. Handles
 * complex spells like teleportation, summoning, and transmutation.
 *
 * Parameters:
 *   spellnum - The specific spell type to execute (SPL_COMBAT, SPL_HEAL, etc.)
 *   monst_val - Monster type for summoning/sending spells
 *   max_pts - Maximum spell points available to the caster
 *   pt_cost - Pointer to variable receiving actual spell point cost
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies target units (army_tptr, navy_tptr, cvn_tptr)
 *   - Consumes resources (jewels for summoning/sending)
 *   - Updates unit locations (teleportation)
 *   - Creates new units (summoning)
 *   - Modifies city materials (transmutation)
 *   - Updates display and cursor position
 *   - May reduce caster movement points
 *
 * Notes:
 *   - Uses extensive global state (army_ptr, city_ptr, world coordinates)
 *   - Teleportation validates distance limits and charges proportionally
 *   - Summoning creates monsters at caster location with proper initialization
 *   - God mode bypasses most resource costs but follows some restrictions
 *   - Shrine assistance affects costs and effectiveness
 */
static void
work_spell PARM_4(Spelltype, spellnum, int, monst_val,
		  int, max_pts, int *, pt_cost)
{
  int ntn_num, dist_lim, count, maxval;
  int oldx_spot, oldy_spot;
  ARMY_PTR targ_aptr = army_tptr;
  ARMY_PTR tmp_aptr = army_ptr;
  ITEM_PTR cost_ptr = NULL;

  /* perform each spell; if possible now */
  switch (spellnum) {
  case SPL_COMBAT:
    /* just make the unit magically enhanced */
    if (army_tptr != NULL) {
      army_ptr = army_tptr;
      set_spelled(ARMY_STAT);
      AADJSTAT;
    } else if (navy_tptr != NULL) {
      navy_ptr = navy_tptr;
      set_spelled(NAVY_STAT);
      NADJSTAT;
    } else if (cvn_tptr != NULL) {
      cvn_ptr = cvn_tptr;
      set_spelled(CVN_STAT);
      VADJSTAT;
    }
    break;
  case SPL_FLIGHT:
    /* give the unit wings! */
    if (army_tptr != NULL) {
      army_ptr = army_tptr;
      set_flight(ARMY_STAT);
      AADJSTAT;
    }
    break;
  case SPL_HEAL:
    /* make it feel better */
    if (army_tptr != NULL) {
      army_ptr = army_tptr;
      if (a_decay(ARMY_TYPE)) {
	ARMY_EFF += rand_val(6);
	if (ARMY_EFF > ARMY_MAXEFF) {
	  ARMY_EFF = ARMY_MAXEFF;
	}
      } else if (ARMY_EFF < (int) ARMY_MAXEFF - 25) {
	ARMY_EFF += 25;
      } else if (ARMY_EFF < ARMY_MAXEFF) {
	ARMY_EFF = ARMY_MAXEFF;
      } else {
	ARMY_EFF += 3;
	if (ARMY_EFF > ARMY_MAXEFF + 5) {
	  ARMY_EFF = ARMY_MAXEFF + 5;
	}
      }
      AADJEFF;
      set_healed(ARMY_STAT);
      AADJSTAT;
    }
    break;
  case SPL_TPORT:
    /* get the new location */
    if (targ_aptr == NULL) {
      errormsg("Hmm... I just can't find your army unit");
      break;
    }

    /* let them see the info */
    presskey();

    /* now relocate it */
    dist_lim = max_pts / (*pt_cost);
    if (shrine_helped) {
      dist_lim *= 100;
      if (shrine_helped < 50) {
	dist_lim /= (100 - shrine_helped * 2);
      }
    }
    global_int = dist_lim;
    oldx_spot = XREAL;
    oldy_spot = YREAL;
    move_parse(MOVE_TELEPORT);
    if ((XREAL != oldx_spot) || (YREAL != oldy_spot)) {

      /* find the displacement */
      if (is_god == FALSE) {

	/* check if it worked */
	if (!map_within(XREAL, YREAL, oldx_spot, oldy_spot, dist_lim)) {
	  errormsg("Woops, you have gone too far");
	  xcurs = oldx_spot - xoffset;
	  ycurs = oldy_spot - yoffset;
	  break;
	}

	/* find the distance */
	while (map_within(XREAL, YREAL, oldx_spot, oldy_spot, dist_lim)) {
	  dist_lim--;
	  if (dist_lim < 0) break;
	}
	dist_lim++;

	/* now charge for it */
	*pt_cost = dist_lim * (*pt_cost);
	if (shrine_helped) {
	  *pt_cost *= (100 - shrine_helped * 2);
	  *pt_cost /= 100;
	  if (*pt_cost <= 0) *pt_cost = 1;
	}

      }

      /* relocate the unit */
      army_ptr = targ_aptr;
      ARMY_XLOC = XREAL;
      ARMY_YLOC = YREAL;
      if (unit_leading(ARMY_STAT)) {
	set_grploc(ARMY_ID, XREAL, YREAL);
	GADJLOC;
      } else {
	AADJLOC;
      }
      army_sort(FALSE);
      hex_recalc();
      goto_army(ARMY_ID);

      /* give them a bottom message */
      if (unit_leading(ARMY_STAT)) {
	sprintf(string, "Group %d relocated to sector [%d,%d]",
		ARMY_ID, xloc_relative(XREAL), yloc_relative(YREAL));
      } else {
	sprintf(string, "%s %d relocated to sector [%d,%d]",
		ainfo_list[ARMY_TYPE].name, ARMY_ID,
		xloc_relative(XREAL), yloc_relative(YREAL));
      }
      bottommsg(string);

    } else {
      bottommsg("Teleportation aborted....");
      *pt_cost = 0;
    }
    break;
  case SPL_TRANSFER:
    /* relocate energy to the new being */
    if (army_tptr != NULL) {

      /* find the total for transfering */
      count = ARMY_SPTS;
      ARMY_SPTS = 0;
      AADJSPTS;

      /* just transfer it if god */
      if (is_god == TRUE) {
	ARMYT_SPTS += count;
	army_ptr = army_tptr;
	AADJSPTS;
      } else {

	/* check the maximum */
	if (a_isruler(ARMYT_TYPE)) {
	  maxval = ntn_ptr->attribute[BUTE_SPELLPTS] * 2;
	} else {
	  maxval = ntn_ptr->attribute[BUTE_SPELLPTS];
	}
	if (ARMYT_SPTS >= maxval) {
	  sprintf(string, "Your %s %d couldn't handle the extra energy!",
		  ainfo_list[ARMYT_TYPE].name, ARMYT_ID);
	  errormsg(string);
	  ARMYT_SPTS = maxval / 4;
	  army_ptr = army_tptr;
	  AADJSPTS;
	} else {
	  ARMYT_SPTS += count;
	  if (ARMYT_SPTS > maxval) {
	    ARMYT_SPTS = maxval;
	  }
	  army_ptr = army_tptr;
	  AADJSPTS;
	}

      }
    }
    break;
  case SPL_TRANSMUTE:
    /* increase the materials */
    CITY_MTRLS[monst_val] += global_long;
    CADJMTRLS(monst_val);
    break;
  case SPL_SENDING:
    /* initialize the sending information */
    ntn_num = -1;
    count = 0;

    /* check the costs */
    if (is_god == FALSE) {

      /* build the cost structure */
      cost_ptr = new_item();
      cost_ptr->xloc = XREAL;
      cost_ptr->yloc = YREAL;
      cost_ptr->itemid = 0;
      for (count = 0; count < MTRLS_NUMBER; count++) {
	cost_ptr->mtrls[count] = (itemtype) 0;
      }
      cost_ptr->mtrls[MTRLS_JEWELS] =
	(itemtype) ainfo_list[monst_val].ore_enlist * 2;

      /* quickie adjust */
      if (CIV_MAGIC(MC_JEWELER)) {
	cost_ptr->mtrls[MTRLS_JEWELS] *= 80;
	cost_ptr->mtrls[MTRLS_JEWELS] /= 100;
      }

      /* check if it is okay */
      if (take_resources(XREAL, YREAL, cost_ptr,
			 splyinsect_stat(ARMY_STAT))) {
	free(cost_ptr);
	cost_ptr = NULL;
	presskey();
	sprintf(string,
		"The %s, seeing a lack of jewels, vanishes amidst laughter",
		ainfo_list[monst_val].name);
	errormsg(string);
	ntn_num = country;
      }
    }

    /* determine a monster unit to get the culprit */
    while (ntn_num == -1) {
      /* get the nation name */
      mvprintw(LINES - 2, 0,
	       "Send the %s against what nation? ",
	       ainfo_list[monst_val].name);
      clrtoeol();
      ntn_num = get_country(FALSE);

      /* check it */
      if ((ntn_num == UNOWNED) ||
	  (ntn_num < 0) ||
	  (ntn_num >= MAXNTN) ||
	  (world.np[ntn_num] == NULL)) {
	if (no_input == TRUE) {
	  if (is_god == FALSE) {
	    errormsg("You must make a decision");
	    ntn_num = -1;
	    count++;
	  } else {
	    return;
	  }
	} else if ((is_god == FALSE) &&
		   (count >= 3)) {
	  errormsg("You cannot make up your mind? ... So be it ...");
	  ntn_num = country;
	} else {
	  errormsg("That is not among the nations I know... try again");
	  ntn_num = -1;
	  count++;
	}
      } else if (is_god == FALSE) {
	if (((world.np[ntn_num])->dstatus[country] == DIP_UNMET) ||
	    (ntn_ptr->dstatus[ntn_num] == DIP_UNMET)) {
	  errormsg("You have not yet met that nation... try again");
	  ntn_num = -1;
	  count++;
	} else if (ntn_ptr->dstatus[ntn_num] < DIP_WAR) {
	  errormsg("Sending monsters to attack a nation is an act of war");
	  ntn_num = -1;
	  count++;
	} else if (n_ismonster((world.np[ntn_num])->active)) {
	  errormsg("There is no focal point to attack in that nation");
	  ntn_num = -1;
	  count++;
	}
      }
    }

    /* record the costs for this operation */
    if ((is_god == FALSE) &&
	(cost_ptr != NULL)) {

      /* record it */
      TAKESTART;
      global_long = cost_ptr->mtrls[MTRLS_JEWELS];
      TAKEMTRLS(MTRLS_JEWELS);
      TAKEFINISH(splyinsect_stat(ARMY_STAT));
      free(cost_ptr);

    }

    /* okay, now do it */
    MGKSEND;
    break;
  case SPL_SUMMON:
    /* first check if the jewels are around */
    if (is_god == FALSE) {

      /* build the cost structure */
      cost_ptr = new_item();
      cost_ptr->xloc = XREAL;
      cost_ptr->yloc = YREAL;
      cost_ptr->itemid = 0;
      for (count = 0; count < MTRLS_NUMBER; count++) {
	cost_ptr->mtrls[count] = (itemtype) 0;
      }
      cost_ptr->mtrls[MTRLS_JEWELS] = utype_mayuse(ntn_ptr, monst_val) *
	(itemtype) ainfo_list[monst_val].ore_enlist;

      /* quickie adjust */
      if (CIV_MAGIC(MC_JEWELER)) {
	cost_ptr->mtrls[MTRLS_JEWELS] *= 80;
	cost_ptr->mtrls[MTRLS_JEWELS] /= 100;
      }

      /* check if it is okay */
      if (take_resources(XREAL, YREAL, cost_ptr,
			 splyinsect_stat(ARMY_STAT))) {
	free(cost_ptr);
	sprintf(string,
		"The %s, seeing the lack of jewels, vanishes amidst laughter",
		ainfo_list[monst_val].name);
	errormsg(string);
	ntn_num = country;
	MGKSEND;
	break;
      } else {
	/* record it */
	TAKESTART;
	global_long = cost_ptr->mtrls[MTRLS_JEWELS];
	TAKEMTRLS(MTRLS_JEWELS);
	TAKEFINISH(splyinsect_stat(ARMY_STAT));
	free(cost_ptr);
      }
      
    }

    /* reduce the leaders movement ability */
    if (is_god == FALSE) {

      /* check for decreasing movement */
      if (ARMY_MOVE > 0) {
	if (ARMY_MOVE > 25) {
	  ARMY_MOVE -= 25;
	} else {
	  ARMY_MOVE = 0;
	}
	if (unit_leading(ARMY_STAT)) {
	  set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
	  GADJMOVE;
	} else {
	  AADJMOVE;
	}
      }

    } else if (ntn_ptr == NULL) {
      for (ntn_num = 1;
	   ntn_num < MAXNTN;
	   ntn_num++) {
	if (((ntn_ptr = world.np[ntn_num]) != NULL) &&
	    (n_issavage(ntn_ptr->active))) break;
      }
      if (ntn_num == MAXNTN) return;
      if (get_god(ntn_num, TRUE)) return;
    }

    /* now build the monster unit */
    army_ptr = crt_army(monst_val);
    ARMY_XLOC = XREAL;
    ARMY_YLOC = YREAL;
    ARMY_SIZE = 1;
    ARMY_MOVE = 0;
    ARMY_SPLY = start_unitsply();	/* content to begin with */
    if (is_god == TRUE) {
      set_status(ARMY_STAT, ST_ATTACK);
      set_speed(ARMY_STAT, SPD_NORMAL);
    } else {
      /* assign the leader and status */
      ARMY_LEAD = tmp_aptr->armyid;
      ARMY_STAT = group_stat(tmp_aptr->armyid);
    }

    /* now record the creation */
    army_sort(FALSE);
    ACREATE;
    AADJSIZE;
    AADJSPLY;
    AADJSTAT;
    AADJLOC;
    break;
  default:
    /* just pass it on to the update routine */
    MGKSPELL;
    break;
  }

  /* restore the army pointer */
  army_ptr = tmp_aptr;
}

/*
 * go_spell - Handle user interface and setup for spell casting
 *
 * Manages the spell casting interface by processing different spell classes,
 * gathering required parameters from the user, validating targets, and
 * calculating costs. Handles target selection for unit spells, monster
 * selection for summoning, and material selection for transmutation.
 *
 * Parameters:
 *   spl_number - The spell index to cast
 *   maxpts - Maximum spell points available for casting
 *
 * Returns:
 *   Actual spell point cost if successful
 *   -1 if spell casting was cancelled or failed
 *
 * Side Effects:
 *   - Prompts user for spell targets and parameters
 *   - Updates display with spell information
 *   - Sets global target variables (army_tptr, navy_tptr, cvn_tptr)
 *   - May modify city materials for transmutation preparation
 *   - Calls work_spell() to execute the actual spell effects
 *
 * Notes:
 *   - Spell classes: 'D'/'O'/'S' = direct cast, 'L'/'U'/'u' = unit target
 *   - 'M' = monster spells, 'R' = transmutation requiring material selection
 *   - Shrine assistance affects final spell point costs
 *   - Success rate check determines if spell actually takes effect
 *   - Different spell classes have different target validation requirements
 */
static int
go_spell PARM_2(Spelltype, spl_number, int, maxpts)
{
  int i, count, choice = -1, ptcost;
  long amount, maxamt;
  char bufstr[LINELTH];

  /* clear up the last few lines */
  clear_bottom(4);

  /* first check the spell */
  if ((spl_number < 0) ||
      (spl_number >= spell_number)) {
    errormsg("Major Internal Error: Unknown spell type!");
    return(-1);
  }

  /* begin based on the spell style */
  switch (spell_list[spl_number].class) {
  case 'D':
  case 'O':
  case 'S':
    /* these spells can just be cast without worry */
    strcpy(bufstr, spell_list[spl_number].good_str);
    ptcost = spell_list[spl_number].cost;
    break;
  case 'L':
  case 'U':
  case 'u':
    /* get a military unit */
    sprintf(bufstr, "Cast %s on what unit? ",
	    spell_list[spl_number].name);
    if (set_select(TRUE, bufstr) == FALSE) return(-1);

    /* check if only army units are allowed */
    if ((spell_list[spl_number].class == 'u') ||
	(spell_list[spl_number].class == 'L')) {
      /* not an army unit */
      if ((army_tptr = get_armyselect()) == NULL) {
	errormsg("That spell may only be cast upon army units");
	return(-1);
      } else if ((spell_list[spl_number].class == 'L') &&
		 !a_isleader(ARMYT_TYPE)) {
	errormsg("That spell may only be cast upon a leader unit");
	return(-1);
      }
      if ((spl_number == SPL_HEAL) &&
	  unit_healed(ARMYT_STAT)) {
	if (ARMYT_MOVE == 0) {
	  errormsg("Healing magic will not help that unit right now");
	} else {
	  errormsg("That unit is still recovering from its earlier healing");
	}
	return(-1);
      }
      if ((unit_leading(ARMYT_STAT)) &&
	  ((spl_number == SPL_FLIGHT) ||
	   (spl_number == SPL_TPORT))) {
	count = men_ingroup(ARMYT_ID, FALSE);
      } else {
	count = army_captsize(army_tptr, FALSE);
      }
      sprintf(string, "%s %d", ainfo_list[ARMYT_TYPE].name, ARMYT_ID);
    } else {
      /* now check which type is selected */
      army_tptr = NULL;
      navy_tptr = NULL;
      cvn_tptr = NULL;
      if ((army_tptr = get_armyselect()) != NULL) {
	/* army is selected */
	if ((unit_leading(ARMYT_STAT)) &&
	    ((spl_number == SPL_FLIGHT) ||
	     (spl_number == SPL_TPORT))) {
	  count = max(1, men_ingroup(ARMYT_ID, FALSE));
	} else {
	  count = max(1, army_captsize(army_tptr, FALSE));
	}
	sprintf(string, "%s %d", ainfo_list[ARMYT_TYPE].name, ARMYT_ID);
      } else if ((navy_tptr = get_navyselect()) != NULL) {
	/* navy is selected */
	count = 0;
	for (i = 0; i < NSHP_NUMBER; i++) {
	  count += navy_holds(navy_tptr, i);
	}
	count *= NAVYT_CREW;
	sprintf(string, "Navy Fleet %d", NAVYT_ID);
      } else if ((cvn_tptr = get_cvnselect()) != NULL) {
	/* caravan is selected */
	count = CVNT_CREW * CVNT_SIZE;
	sprintf(string, "Caravan %d", CVNT_ID);
      } else {
	/* not a one */
	errormsg("That spell must be cast upon a selected unit");
	return(-1);
      }
    }

    /* assign the information strings */
    sprintf(bufstr, spell_list[spl_number].good_str, string);
    ptcost = spell_list[spl_number].cost;
    if (ptcost > 0) {
      ptcost = (count - 1 + ptcost) / ptcost;
      if (ptcost < 1) ptcost = 1;
    }
    break;
  case 'M':
    /* get the desired monster */
    clear_bottom(0);
    choice = enter_unittype((spl_number == SPL_SUMMON) ? 4 : 5,
			    (spl_number == SPL_SUMMON) ? "Summon" : "Send",
			    "There are no summonable monsters available");
    if (choice >= num_armytypes) {
      return(-1);
    }

    /* now set the limits */
    sprintf(bufstr, spell_list[spl_number].good_str, ainfo_list[choice].name);
    ptcost = (int) ainfo_list[choice].tal_enlist;
    if (spl_number == SPL_SENDING) {
      ptcost *= 2;
    }
    ptcost *= (1 + utype_mayuse(ntn_ptr, choice));
    ptcost /= 2;
    break;
  case 'R':
    /* find materials to adjust and what to adjust them to */
    clear_bottom(0);
    mvaddstr(LINES - 4, 0, "Transmute which: ");
    i = get_mtrls(CITY_MTRLS);
    if (no_input == TRUE) {
      return(-1);
    }
    if (i == MTRLS_NUMBER) {
      errormsg("That was not a possible choice.");
      return(-1);
    }

    /* find out maximum amount */
    maxamt = 100000 / mtrls_info[i].weight;
    if (maxamt > CITY_MTRLS[i]) {
      maxamt = CITY_MTRLS[i];
    }
    if (maxamt <= 0) {
      errormsg("There are no such materials here to be transmuted");
      return(-1);
    }

    /* now find out desired amount */
    mvprintw(LINES - 3, 0, "Transmute how much %s? [%d] ",
	     mtrls_info[i].lname, maxamt);
    amount = get_number(FALSE);
    if (no_input == TRUE) {
      amount = maxamt;
    } else if (amount > maxamt) {
      errormsg("You may not transmute so much");
      return(-1);
    }

    /* adjust it to what? */
    mvaddstr(LINES - 2, 0, "To which: ");
    choice = get_mtrls(NULL);
    if (no_input == TRUE) {
      return(-1);
    }
    if (choice == MTRLS_NUMBER) {
      errormsg("I am afraid that isn't a possible choice");
      return(-1);
    }
    if (choice == i) {
      errormsg("You leave the materials alone");
      return(-1);
    }

    /* now compute the transmutation amount */
    global_long = (amount * mtrls_info[i].transval) /
      mtrls_info[choice].transval;
    CITY_MTRLS[i] -= amount;
    CADJMTRLS(i);
    sprintf(bufstr, spell_list[spl_number].good_str,
	    mtrls_info[i].lname, global_long, mtrls_info[choice].lname);
    ptcost = spell_list[spl_number].cost;
    break;
  default:
    /* unknown spell class */
    sprintf(string, "Unknown spell class %c!",
	    spell_list[spl_number].class);
    errormsg(string);
    return(-1);
  }

  /* first do a final check on the spell points */
  if ((shrine_helped) && (ptcost > 0)) {
    ptcost *= (100 - shrine_helped * 2);
    ptcost /= 100;
    if (ptcost <= 0) ptcost = 1;
  }
  if (ptcost > maxpts) {
    errormsg("You do not have enough magical energy to perform that spell");
    return(-1);
  }

  /* now check if the spell succeeded or not */
  if ((is_god == TRUE) ||
      (rand_val(100) < spell_list[spl_number].rate)) {

    /* it worked */
    clear_bottom(1);
    bottommsg(bufstr);
    work_spell(spl_number, choice, maxpts, &ptcost);
    
  } else {

    /* it sputtered */
    clear_bottom(1);
    mvaddstr(LINES - 2, 0, spell_list[spl_number].bad_str);
    clrtoeol();

  }
  return(ptcost);
}

/*
 * cast_spells - Main spell casting interface for player units
 *
 * Provides the primary spell casting interface allowing players to select
 * and cast spells from army units. Validates caster requirements, displays
 * available spells, handles user selection, and manages spell point and
 * health costs. Supports both normal play and god mode operation.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 if no spell was cast or operation cancelled
 *   MOVECOST if spell was successfully cast (for normal units)
 *
 * Side Effects:
 *   - Prompts user for spell caster selection
 *   - Displays available spells based on magical knowledge
 *   - Deducts spell points and health from caster
 *   - Updates army unit statistics after spell casting
 *   - Handles shrine location bonuses for magical assistance
 *   - May enter god mode for deity spell casting
 *
 * Notes:
 *   - Requires army unit with spell casting ability or leader status
 *   - Shrine locations provide magical assistance reducing costs
 *   - God mode allows unlimited spell casting with different restrictions
 *   - Health threshold (SPL_STR_LIM) required for spell casting
 *   - Spell availability depends on nation's magical knowledge
 *   - Interactive spell selection using key bindings
 */
int
cast_spells PARM_0(void)
{
  Spelltype spell_num;
  int limit, strength;
  int xspot, yspot, len, okval;

  /* show the information */
  clear_bottom(0);
  mvaddstr(LINES - 4, 0, "Spell Casting Mode:");

  /* deity may always cast spells */
  if (country != UNOWNED) {

    /* now find the spell caster */
    if ((army_ptr = get_armyselect()) == NULL) {
      errormsg("You must select an army unit who can cast spells");
      return(0);
    }

    /* is it a leader */
    if (!a_isleader(ARMY_TYPE) &&
	!a_castspells(ARMY_TYPE)) {
      errormsg("That unit is not capable of casting spells");
      return(0);
    }

    /* check the limit of spell ability */
    if ((limit = (int) ARMY_SPTS) <= 0) {
      errormsg("That unit has no magic strength available to cast spells");
      return(0);
    }

    /* now check the physical strength */
    strength = ARMY_EFF;
    if (strength < SPL_STR_LIM) {
      errormsg("That unit is not healthy enough to cast any spells now");
      return(0);
    }

  } else {

    /* find out for what country */
    if (get_god(-1, TRUE)) {
      /* check for the need to exert some influence */
      mvaddstr(LINES - 3, 0, "Do you wish to cast a spell as God? ");
      clrtoeol();
      if (y_or_n() == FALSE) {
	return(0);
      }
    }

    /* pretty much unlimited */
    limit = 1000;
    strength = 1000;

  }

  /* what are the capabilities of this leader? */
  sct_ptr = &(sct[XREAL][YREAL]);
  if ((major_desg(sct_ptr->designation) == MAJ_SHRINE) &&
      (sct_ptr->owner == country)) {
    /* spiritual support in a shrine */
    shrine_helped = magic_value(sct_ptr) + 1;
  } else {
    /* only normal physical strength is used */
    shrine_helped = 0;
  }

  /* clean things up */
  clear_bottom(0);
  xspot = 2;
  yspot = LINES - 4;

  /* now go through the list of possible spells */
  for (spell_num = 0;
       spell_num < spell_number;
       spell_num++) {

    /* is the spell possible? */
    if ((okval = spellok(spell_num, limit, strength - SPL_STR_MIN, FALSE))
	!= 0) {

      /* show the spell name */
      if ((xspot != 2) ||
	  (yspot != LINES - 4)) {
	addstr(", ");
	xspot += 2;
      }
      len = strlen(spell_list[spell_num].prompt) + 2;
      if (xspot + len > COLS - 5) {
	xspot = 2;
	yspot++;
      }

      /* now show it */
      move(yspot, xspot);
      hip_string(spell_list[spell_num].prompt,
		 spell_list[spell_num].keypos,
		 okval == 1);
      xspot += len;

    }

  }

  /* now get the desired spell name */
  mvaddstr(LINES - 5, 0, "Spell Casting Mode;  Cast which spell? ");
  refresh();
  xspot = next_char();
  if ((xspot == ' ') || (xspot == '\n') || (xspot == '\r')) {
    if ((is_god == TRUE) &&
	(ntn_ptr != NULL)) {
      reset_god();
    }
    return(0);
  }
  if (islower(xspot)) {
    xspot = toupper(xspot);
  }
  for (spell_num = 0;
       spell_num < spell_number;
       spell_num++) {
    if (xspot ==
	spell_list[spell_num].prompt[spell_list[spell_num].keypos - 1]) break;
  }
  if (spell_num == spell_number) {
    /* nope, not right on that one */
    errormsg("That is not any spell that I have heard of");
    if ((is_god == TRUE) &&
	(ntn_ptr != NULL)) {
      reset_god();
    }
    return(0);
  }

  /* is the selected spell possible? */
  if (spellok(spell_num, limit, strength - SPL_STR_MIN, TRUE) == 1) {

    /* did we cast it? */
    if (((limit = go_spell(spell_num, limit)) != -1) &&
	(is_god == FALSE)) {

      /* then charge for it */
      if (limit > 0) {
	ARMY_SPTS -= limit;
	AADJSPTS;
      }

      /* find the health cost */
      strength = spell_list[spell_num].drain;
      strength *= (100 - shrine_helped * 4);
      strength /= 100;
      if (strength <= 0) {
	strength = 1;
      }
      if ((limit == 0) &&
	  (spell_num != SPL_TRANSFER)) strength /= 4;
      if (strength > 0) {
	ARMY_EFF -= strength;
	AADJEFF;
      }

      /* now show the cost */
      if ((is_god == FALSE) &&
	  (limit > 0)) {
	sprintf(string, "Pt Cost: %d", limit);
	mvaddstr(LINES - 1, COLS - strlen(string) - 2, string);
	refresh();
	next_char();
      } else {
	presskey();
      }

    }

  }

  /* out'a here */
  if ((is_god == TRUE) &&
      (ntn_ptr != NULL)) {
    reset_god();
  }
  return(0);
}

/* Number of powers in a column */
#define MG_COLNUM	16	/* was (LINES - 8); like 16 better */

/* Variables for the Magic Screen */
int mg_done, mg_select, mg_type, mg_fulldraw, mg_mode;
KLIST_PTR mg_bindings = NULL;
extern PARSE_STRUCT mg_funcs[];
extern KBIND_STRUCT mg_klist[];

/* information structure concerning movement keybindings */
KEYSYS_STRUCT magic_keysys = {
  "magic", mg_funcs, mg_klist, 0, 0
};

/*
 * mg_help - Display help information for magic power screen commands
 *
 * Shows comprehensive help screen listing all available commands and
 * key bindings for the magic power management interface. Uses the
 * global help system to display command descriptions and usage.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Displays help screen with command list
 *   - Sets mg_fulldraw flag to trigger complete screen redraw
 *   - Temporarily replaces current display content
 *
 * Notes:
 *   - Help content generated from mg_funcs and mg_bindings arrays
 *   - Returns to magic screen after help is dismissed
 *   - Part of the magic screen key binding system
 */
static int
mg_help PARM_0(void)
{
  /* use the global help command */
  create_help("Magic Power Summary Command List",
	      mg_bindings, mg_funcs, magic_keysys.num_parse);
  
  /* clear out */
  mg_fulldraw = TRUE;
  return(0);
}

/*
 * mg_powcnt - Count magical powers in specified class and ownership mode
 *
 * Counts the number of magical powers available for display based on
 * the magic class (Military/Civilian/Wizardry) and whether showing
 * owned or unowned powers. Used for navigation bounds checking and
 * display organization in the magic power interface.
 *
 * Parameters:
 *   magic_type - Magic class index (MAG_MILITARY, MAG_CIVILIAN, MAG_WIZARDRY)
 *   normal - Mode flag: TRUE for owned powers, FALSE for unowned/available
 *
 * Returns:
 *   Number of powers in the specified class and mode
 *   0 if no powers available or nation has no powers in normal mode
 *
 * Side Effects:
 *   - Accesses nation power data (ntn_ptr->powers[])
 *   - May call magic_ok() for prerequisite validation
 *
 * Notes:
 *   - Uses bitwise operations to check power ownership
 *   - Filters unowned powers through magic_ok() prerequisite checking
 *   - Essential for magic screen navigation and bounds checking
 *   - Returns 0 immediately if normal mode and no powers owned
 */
static int
mg_powcnt PARM_2(int, magic_type, int, normal)
{
  int i, count = 0, max_val = 0;
  long pow_tmp = 0L;

  /* init vars */
  max_val = mclass_list[magic_type].maxval;
  pow_tmp = ntn_ptr->powers[magic_type];

  /* check this */
  if ((pow_tmp == 0) &&
      (normal)) {
    return(0);
  }

  /* scan the power list */
  for (i = 0; i < max_val; i++) {
    if (((1L << i) & pow_tmp) == (normal << i)) {
      if (normal ||
	  magic_ok(magic_type, i)) {
	count++;
      }
    }
  }
  return(count);
}

/*
 * mg_powint - Get bit position of currently selected magical power
 *
 * Converts the current selection index (mg_select) into the actual
 * bit position within the nation's power bitmask for the current
 * magic type. Handles both owned and unowned power modes by counting
 * through the appropriate power set.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Bit position (0-31) of the currently selected power
 *   32 if no powers available in the current mode
 *
 * Side Effects:
 *   - Accesses global magic screen state (mg_select, mg_type, mg_mode)
 *   - May call magic_ok() for prerequisite validation
 *
 * Notes:
 *   - Essential for converting display selection to power bitmask position
 *   - Used by other functions to identify specific powers for operations
 *   - Handles edge case of empty power lists gracefully
 *   - Works with both owned powers display and available powers display
 */
static int
mg_powint PARM_0(void)
{
  int count = 0, cnt_pos;
  long pow_tmp = 0L;

  /* is this anything? */
  pow_tmp = ntn_ptr->powers[mg_type];
  if ((pow_tmp == 0L) &&
      (mg_mode == TRUE)) {
    return(32);
  }

  /* now count and return the value */
  for (cnt_pos = 0; cnt_pos < 32; cnt_pos++) {
    /* stop when the selected power is reached */
    if (((1L << cnt_pos) & pow_tmp) == (mg_mode << cnt_pos)) {
      if (mg_mode ||
	  magic_ok(mg_type, cnt_pos)) {
	if (++count > mg_select) break;
      }
    }
  }
  return(cnt_pos);
}

/*
 * mg_powcost - Calculate jewel cost for acquiring a magical power
 *
 * Computes the jewel cost for purchasing a magical power based on
 * the nation's current power holdings, racial cost modifiers, and
 * magical bonuses. Cost increases with the number of powers already
 * owned and varies by magic class.
 *
 * Parameters:
 *   magic_type - Magic class for cost calculation
 *
 * Returns:
 *   Jewel cost for acquiring a power in the specified class
 *
 * Side Effects:
 *   - Accesses nation race and power data
 *   - Checks for Jeweler magical power bonus
 *
 * Notes:
 *   - Base cost from race_info[].cost_base[] for each magic class
 *   - Cost scales with total powers: target class * (count+1), others * count/2
 *   - Learning mode (mg_mode == FALSE) triples the cost
 *   - Jeweler power provides 5% discount (95% of normal cost)
 *   - Used for both display and actual purchase validation
 */
static long
mg_powcost PARM_1(int, magic_type)
{
  long cost;
  int i, num_have[MAG_NUMBER];

  /* find out how many powers in the various classes */
  for (i = 0; i < MAG_NUMBER; i++) {
    num_have[i] = mg_powcnt(i, TRUE);
  }

  /* figure it based on the costs */
  cost = 0;
  for (i = 0; i < MAG_NUMBER; i++) {
    if (i == magic_type) {
      cost += race_info[ntn_ptr->race].cost_base[i] * (num_have[i] + 1);
    } else {
      cost += (race_info[ntn_ptr->race].cost_base[i] * num_have[i]) / 2L;
    }
  }

  /* if mode is on */
  if (mg_mode == FALSE) {
    cost *= 3;
  }

  /* check for jeweler power */
  if (CIV_MAGIC(MC_JEWELER)) {
    cost *= 95;
    cost /= 100;
  }

  /* now give back what will be taken */
  return(cost);
}

/*
 * mg_display - Render the complete magical powers interface screen
 *
 * Draws the comprehensive magic power management interface including
 * three-column layout for magic classes, power lists, costs, and
 * navigation aids. Handles both owned and unowned power display modes
 * with proper highlighting and formatting.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Clears and redraws entire screen or sections as needed
 *   - Updates display based on mg_fulldraw flag
 *   - Shows current selection with standout highlighting
 *   - Displays power costs and jewel funding information
 *   - Renders help prompt at bottom
 *
 * Notes:
 *   - Three-column layout: Military, Civilian, Wizardry powers
 *   - MG_COLNUM (16) rows per column with automatic wrapping
 *   - Current magic type highlighted in title and cost sections
 *   - Selected power highlighted within appropriate column
 *   - Capital jewel supply shown for funding reference
 *   - Screen layout adapts to terminal width (COLS)
 */
static void
mg_display PARM_0(void)
{
  int xloc, yloc, col_wid, sect_wid, edge;
  int section, i, max_val = 0;
  long tmp_long;
  MAGIC_PTR tmp_mgray = NULL;

  /* initialize the display configuration */
  sect_wid = (COLS - 4) / 3;
  col_wid = (sect_wid - 1) / 2;
  if (COLS % 3 == 0) {
    edge = 1;
  } else {
    edge = 0;
  }

  /* clear up shop */
  if (mg_fulldraw == TRUE) {
    clear();
    mg_fulldraw = FALSE;
  } else {
    move(0, 0);
    clrtobot();
  }

  /* build the framework */
  for (yloc = 1; yloc <= 4 + MG_COLNUM; yloc++) {

    /* build the line type based on the line number */
    xloc = edge;
    switch (yloc) {
    case 2:
      /* provide the titles */
      mvaddch(yloc, xloc, '|');
      for (section = MAG_MILITARY; section <= MAG_WIZARDRY; section++) {

	/* build the string */
	i = strlen(mclass_list[section].name);
	sprintf(string, "%*s%s%*s", (sect_wid - i + 1) / 2, "",
		mclass_list[section].name, (sect_wid - i) / 2, "");

	/* display it */
	if (section == mg_type) standout();
	addstr(string);
	if (section == mg_type) standend();

	/* append the end */
	addch('|');
      }
      break;
    case 3:
      /* build a line across the board -- with joints*/
      mvaddch(yloc, xloc, '|');
      for (xloc = 0; xloc < sect_wid; xloc++) {
	addch('-');
      }
      addch('+');
      for (xloc = 0; xloc < sect_wid; xloc++) {
	addch('-');
      }
      addch('+');
      for (xloc = 0; xloc < sect_wid; xloc++) {
	addch('-');
      }
      addch('|');
      break;
    default:
      /* is it either the beginning or ending line? */
      if ((yloc != 1) && (yloc != 4 + MG_COLNUM)) {

	/* quick build of the normal lines */
	mvaddch(yloc, xloc++, '|');
	xloc += sect_wid;
	mvaddch(yloc, xloc++, '|');
	xloc += sect_wid;
	mvaddch(yloc, xloc++, '|');
	xloc += sect_wid;
	mvaddch(yloc, xloc++, '|');

      } else {

	/* build the special lines */
	mvaddch(yloc, xloc, '-');
	for (xloc = 0; xloc < sect_wid; xloc++) {
	  addch('-');
	}
	addch('+');
	for (xloc = 0; xloc < sect_wid; xloc++) {
	  addch('-');
	}
	addch('+');
	for (xloc = 0; xloc < sect_wid; xloc++) {
	  addch('-');
	}
	addch('-');

      }
    }
  }

  /* now calculate and show the various different cost levels */
  xloc = edge + 1;
  yloc = 4 + MG_COLNUM;
  for (section = MAG_MILITARY; section <= MAG_WIZARDRY; section++) {
    /* calc and show */
    tmp_long = mg_powcost(section);
    sprintf(string, "| %ld |", tmp_long);
    i = (sect_wid - strlen(string) + 1) / 2;
    mvaddstr(yloc, xloc + i, string);

    /* now check if emphasis needs to be given */
    if (mg_type == section) {
      standout();
      mvprintw(yloc, xloc + i + 2, "%ld", tmp_long);
      standend();
    }

    /* move on */
    xloc += sect_wid + 1;
  }

  /* display all of the powers */
  for (section = MAG_MILITARY; section <= MAG_WIZARDRY; section++) {

    /* prep the location */
    xloc = edge + (1 + sect_wid) * section + 1;
    yloc = 4;
    tmp_mgray = mclass_list[section].pow_list;
    max_val = mclass_list[section].maxval;
    tmp_long = ntn_ptr->powers[section];

    /* now go ahead with the list in this column */
    for (i = 0; i < max_val; i++) {
      if (((1L << i) & tmp_long) == (mg_mode << i)) {

	/* check limits */
	if (!mg_mode &&
	    !magic_ok(section, i)) {
	  continue;
	}

	/* build the power and show it */
	sprintf(string, "%-*s", col_wid, tmp_mgray[i].name);
	if ((mg_type == section) &&
	    (i == mg_powint())) {
	  standout();
	}
	mvaddstr(yloc, xloc, string);
	if ((mg_type == section) &&
	    (i == mg_powint())) {
	  standend();
	}

	/* increment counters */
	yloc++;
	if (yloc > 3 + MG_COLNUM) {
	  yloc = 4;
	  xloc += col_wid + 1;
	}

      }
    }
  }

  /* show funding */
  if ((city_ptr = citybyloc(ntn_ptr,
			    ntn_ptr->capx, ntn_ptr->capy)) == NULL) {
    sprintf(string, "Jewel Funding: [none]");
  } else {
    sprintf(string, "Capital Jewel Supply: %ld",
	    (long) CITY_MTRLS[MTRLS_JEWELS]);
  }
  yloc = MG_COLNUM + 6;
  xloc = (COLS - strlen(string) - 1) / 2;
  mvaddstr(yloc, xloc, string);

  /* give a bottom prompt */
  sprintf(string, "Hit '?' for help; or 'q' to exit");
  yloc++;
  xloc = (COLS - strlen(string) - 1) / 2;
  mvaddstr(yloc, xloc, string);

  /* now show the title */
  if (mg_mode == TRUE) {
    sprintf(string, " Nation %s Magical Power Summary", ntn_ptr->name);
  } else {
    sprintf(string, " Powers Needed by Nation %s", ntn_ptr->name);
  }
  yloc = 0;
  xloc = (COLS - strlen(string) - 1) / 2;
  standout();
  mvaddstr(yloc, xloc, string);
  standend();

  /* now show it */
  refresh();
}

/*
 * mg_limit - Ensure selection index stays within valid bounds
 *
 * Validates and corrects the current power selection index to prevent
 * navigation beyond available powers in the current magic type and mode.
 * Called after navigation operations to maintain interface consistency.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Modifies mg_select global variable to stay within bounds
 *   - Sets mg_select to 0 if no powers available
 *   - Reduces mg_select to last valid index if beyond range
 *
 * Notes:
 *   - Essential for preventing array bounds violations
 *   - Called after column switches and mode toggles
 *   - Works with current mg_type and mg_mode settings
 *   - Handles empty power lists gracefully
 */
static void
mg_limit PARM_0(void)
{
  int lim = mg_powcnt(mg_type, mg_mode);

  /* check the limit */
  if (lim == 0) {
    mg_select = 0;
  } else if (mg_select >= lim) {
    mg_select = lim - 1;
  }
}

/*
 * mg_left - Navigate left in the magic power display interface
 *
 * Implements leftward navigation either within the current column
 * (moving up by MG_COLNUM positions) or switching to the previous
 * magic class column. Handles wraparound and bounds checking.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Modifies mg_select and potentially mg_type global variables
 *   - Calls mg_limit() to ensure valid selection bounds
 *
 * Notes:
 *   - If selection >= MG_COLNUM, moves up one column within class
 *   - Otherwise switches to previous magic class (with wraparound)
 *   - Attempts to maintain similar vertical position in new column
 *   - Part of the magic screen navigation key binding system
 */
static int
mg_left PARM_0(void)
{
  /* move to the left */
  if (mg_select >= MG_COLNUM) {
    mg_select -= MG_COLNUM;
  } else {
    mg_type = (mg_type + 2) % 3;
    if (mg_select + MG_COLNUM < mg_powcnt(mg_type, mg_mode)) {
      mg_select += MG_COLNUM;
    }
    mg_limit();
  }
  return(0);
}

/*
 * mg_right - Navigate right in the magic power display interface
 *
 * Implements rightward navigation either within the current column
 * (moving down by MG_COLNUM positions) or switching to the next
 * magic class column. Handles wraparound and bounds checking.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Modifies mg_select and potentially mg_type global variables
 *   - Calls mg_limit() to ensure valid selection bounds
 *
 * Notes:
 *   - If room exists, moves down one column within current class
 *   - Otherwise switches to next magic class (with wraparound)
 *   - Attempts to maintain similar vertical position in new column
 *   - Part of the magic screen navigation key binding system
 */
static int
mg_right PARM_0(void)
{
  /* move to the right */
  if (mg_select + MG_COLNUM < mg_powcnt(mg_type, mg_mode)) {
    mg_select += MG_COLNUM;
  } else {
    mg_type = (mg_type + 1) % 3;
    if (mg_select >= MG_COLNUM) {
      mg_select -= MG_COLNUM;
    }
    mg_limit();
  }
  return(0);
}

/*
 * mg_up - Move selection up one position in current magic class
 *
 * Implements upward navigation within the current magic class column.
 * Decrements the selection index if possible, otherwise provides
 * audio feedback that the top has been reached.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Decrements mg_select if not at top
 *   - Calls beep() if already at top position
 *
 * Notes:
 *   - Simple single-step upward movement
 *   - Prevents navigation above index 0
 *   - Provides user feedback when movement not possible
 *   - Part of the magic screen navigation key binding system
 */
static int
mg_up PARM_0(void)
{
  /* go up if possible */
  if (mg_select > 0) {
    mg_select--;
  } else {
    beep();
  }
  return(0);
}

/*
 * mg_down - Move selection down one position in current magic class
 *
 * Implements downward navigation within the current magic class column.
 * Increments the selection index if possible, otherwise provides
 * audio feedback that the bottom has been reached.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Increments mg_select if not at bottom
 *   - Calls beep() if already at bottom position
 *
 * Notes:
 *   - Simple single-step downward movement
 *   - Prevents navigation beyond available powers
 *   - Provides user feedback when movement not possible
 *   - Part of the magic screen navigation key binding system
 */
static int
mg_down PARM_0(void)
{
  /* go down if possible */
  if (mg_select < mg_powcnt(mg_type, mg_mode) - 1) {
    mg_select++;
  } else {
    beep();
  }
  return(0);
}

/*
 * mg_buy - Purchase a magical power for the nation
 *
 * Handles the complete purchase process for acquiring magical powers,
 * including cost validation, jewel deduction, random power selection
 * (if in random mode), and updating nation power data. Supports both
 * specific power purchase and random power acquisition.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (purchase completed or cancelled)
 *
 * Side Effects:
 *   - Deducts jewels from capital city materials
 *   - Adds new magical power to nation's power set
 *   - Updates both current and initial city material stores
 *   - Calls mg_limit() to update selection bounds
 *   - May prompt user for purchase confirmation
 *
 * Notes:
 *   - Prevents purchases during god browsing mode
 *   - God mode bypasses jewel costs but still requires capital
 *   - Random mode (mg_mode == TRUE) selects available power randomly
 *   - Specific mode purchases the currently selected power
 *   - Validates sufficient jewel funding before purchase
 *   - Expert mode skips confirmation prompts
 */
static int
mg_buy PARM_0(void)
{
  long cost_amt = 0, new_magic = 0L, old_magic = 0L;
  int pow_num = -1;
  MAGIC_PTR tmp_mgray = NULL;

  /* out if not able to buy */
  if (god_browsing == TRUE) {
    errormsg("You are here browsing, you aren't allowed to buy anything.");
    return(0);
  }

  /* first store the older powers and do some initing */
  pow_num = mg_powint();
  new_magic = 1L << pow_num;
  tmp_mgray = mclass_list[mg_type].pow_list;
  old_magic = ntn_ptr->powers[mg_type];

  /* check for invalid powers */
  if (mg_powcnt(mg_type, FALSE) == 0) {
    errormsg("There are no more powers available within that class");
    return(0);
  }

  /* God has good credit */
  if (is_god == FALSE) {

    /* first check if there is enough to make the purchase */
    if ((city_ptr =
	 citybyloc(ntn_ptr, ntn_ptr->capx, ntn_ptr->capy)) == NULL) {
      errormsg("You lack the national focus to gain any magic powers");
      return(0);
    }

    /* now check the cost of the powers */
    cost_amt = mg_powcost(mg_type);
    if (CITY_MTRLS[MTRLS_JEWELS] < (itemtype) cost_amt) {
      if (mg_mode == TRUE) {
	sprintf(string, "You need %ld more jewels to gain a %s Power",
		(long) ((itemtype)cost_amt - CITY_MTRLS[MTRLS_JEWELS]),
		mclass_list[mg_type].name);
      } else {
	sprintf(string, "You need %ld more jewels to gain that %s Power",
		(long) ((itemtype)cost_amt - CITY_MTRLS[MTRLS_JEWELS]),
		mclass_list[mg_type].name);
      }
      errormsg(string);
      return(0);
    }
  }

  /* now get the query as to whether it should be done */
  move(MG_COLNUM + 6, 0);
  clrtoeol();
  if (conq_expert == FALSE) {
    if (mg_mode == TRUE) {
      /* query what is being done */
      if (is_god == TRUE) {
	mvprintw(MG_COLNUM + 7, 0,
		 "Give this nation a Random %s Power? ",
		 mclass_list[mg_type].name);
      } else {
	mvprintw(MG_COLNUM + 7, 0,
		 "Do you wish to puchase a Random %s Power for %ld jewels? ",
		 mclass_list[mg_type].name, cost_amt);
      }
    } else {
      /* display it */
      if (is_god == TRUE) {
	mvprintw(MG_COLNUM + 7, 0,
		 "Give this nation the %s %s Power? ",
		 tmp_mgray[pow_num].name, mclass_list[mg_type].name);
      } else {
	mvprintw(MG_COLNUM + 7, 0,
		 "Buy the %s %s Power for %ld jewels? ",
		 tmp_mgray[pow_num].name, mclass_list[mg_type].name,
		 cost_amt);
      }
    }

    /* get the answer */
    clrtoeol();
    if (y_or_n() == FALSE) {
      return(0);
    }
  }

  /* now get the magical power */
  if (mg_mode == TRUE) {
    /* get the random power */
    if ((new_magic = rand_magic(mg_type)) == 0L) {
      errormsg("Hmmm... I just couldn't find a power to give");
      return(0);
    }
  }

  /* now add in the magic */
  global_long = old_magic;
  global_int = mg_type;
  add_powers(mg_type, new_magic);
  MGKADJ;

  /* now pay the price */
  if (is_god == FALSE) {

    /* first take care of the actual jewels */
    CITY_MTRLS[MTRLS_JEWELS] -= (itemtype) cost_amt;
    CADJMTRLS(MTRLS_JEWELS);

    /* should handle the initial jewels as well */
    if (CITY_IMTRLS[MTRLS_JEWELS] < cost_amt) {
      CITY_IMTRLS[MTRLS_JEWELS] = (itemtype) 0;
    } else {
      CITY_IMTRLS[MTRLS_JEWELS] -= (itemtype) cost_amt;
    }
    CADJIMTRLS(MTRLS_JEWELS);

  }

  /* all done */
  mg_limit();
  return(0);
}

/*
 * mg_info - Display detailed information about selected magical power
 *
 * Shows comprehensive information screen for the currently selected
 * magical power including description, prerequisites, dependent army
 * units, dependent spells, and attribute effects. Provides full
 * context for understanding power utility and relationships.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (information displayed)
 *
 * Side Effects:
 *   - Clears screen and displays detailed power information
 *   - Sets mg_fulldraw flag for complete screen redraw when returning
 *   - Waits for user input before returning to magic screen
 *
 * Notes:
 *   - Returns immediately if no valid power selected
 *   - Shows power description from magic power database
 *   - Lists prerequisite powers from all magic classes
 *   - Identifies army units that require this power
 *   - Lists spells that depend on this power
 *   - Shows attribute bonuses/penalties if applicable
 *   - Uses '*' marker to indicate additional prerequisites needed
 */
static int
mg_info PARM_0(void)
{
  MAGIC_PTR tmp_mgptr, tmp_mg2ptr;
  int i, j, xloc, yloc, yep, uhhuh, pow_num = mg_powint();
  long lng_tmp, pow_lng;

  /* check if there is the information */
  if (mg_select >= mg_powcnt(mg_type, mg_mode)) return(0);

  /* provide a screen full of power information */
  clear();
  mg_fulldraw = TRUE;
  pow_lng = 1L << pow_num;
  tmp_mgptr = mclass_list[mg_type].pow_list;

  /* now show the power name */
  sprintf(string, " %s %s Power Description ",
	  tmp_mgptr[pow_num].name, mclass_list[mg_type].name);
  yloc = 1;
  xloc = (COLS - strlen(string) - 1) / 2;
  standout();
  mvaddstr(yloc, xloc, string);
  yloc += 2;
  standend();

  /* give the power description */
  mvaddstr(yloc++, 2, "Description:");
  mvaddstr(yloc, 5, tmp_mgptr[pow_num].descript);
  yloc += 2;

  /* now list the prerequisite powers */
  mvaddstr(yloc++, 2, "Prerequisites:");
  yep = TRUE;

  /* go through the military powers */
  xloc = 5;
  for (j = 0; j < MAG_NUMBER; j++) {

    /* quick init */
    lng_tmp = tmp_mgptr[pow_num].pow_need[j];

    /* now search the list */
    tmp_mg2ptr = mclass_list[j].pow_list;
    for (i = 0; i < mclass_list[j].maxval; i++) {
      if (lng_tmp & (1L << i)) {
	if (yep == FALSE) {
	  addstr(", ");
	  xloc += 2;
	} else {
	  yep = FALSE;
	}

	/* check the limit */
	strcpy(string, tmp_mg2ptr[i].name);
	if (strlen(string) + xloc > COLS - 20) {
	  xloc = 5;
	  yloc++;
	}
	mvaddstr(yloc, xloc, string);
	xloc += strlen(string);
      }
    }
  }

  /* finish up the magical summary */
  if (yep) {
    mvaddstr(yloc, xloc, "[none]");
  }
  yloc += 2;

  /* now compute the troop types supported */
  mvaddstr(yloc++, 2,
	   "Army Unit Dependencies:");
  xloc = 5;
  yep = TRUE;
  for (i = 0; i < num_armytypes; i++) {

    /* Does this unit type need the current power? */
    uhhuh = FALSE;
    if (pow_lng & ainfo_list[i].pow_need[mg_type]) {
      uhhuh = TRUE;
      lng_tmp = pow_lng ^ ainfo_list[i].pow_need[mg_type];
      lng_tmp |= ainfo_list[i].pow_need[(mg_type + 1) % MAG_NUMBER];
      lng_tmp |= ainfo_list[i].pow_need[(mg_type + 2) % MAG_NUMBER];
    }

    /* it is indeed */
    if (uhhuh) {
      /* build the string of the army type and check location */
      if (yep == FALSE) {
	addstr(", ");
	xloc += 2;
      } else {
	yep = FALSE;
      }
      if (strlen(ainfo_list[i].name) + xloc > COLS - 15) {
	xloc = 5;
	yloc++;
      }
      move(yloc, xloc);

      /* now place it */
      if (lng_tmp) {
	addch('*');
	xloc++;
      }
      addstr(ainfo_list[i].name);

      /* adjust position */
      xloc += strlen(ainfo_list[i].name);
    }

  }

  /* finish up the summary */
  if (yep) {
    mvaddstr(yloc, xloc, "[none]");
  }
  yloc += 2;

  /* now compute the spells supported */
  mvaddstr(yloc++, 2,
	   "Spell Dependencies:");
  xloc = 5;
  yep = TRUE;
  for (i = 0; i < spell_number; i++) {

    /* Does this spell need the current power? */
    uhhuh = FALSE;
    if (pow_lng & spell_list[i].pow_need[mg_type]) {
      uhhuh = TRUE;
      lng_tmp = pow_lng ^ spell_list[i].pow_need[mg_type];
      lng_tmp |= spell_list[i].pow_need[(mg_type + 1) % MAG_NUMBER];
      lng_tmp |= spell_list[i].pow_need[(mg_type + 2) % MAG_NUMBER];
    }

    /* it is indeed */
    if (uhhuh) {
      /* build the string of the spell names and check location */
      if (yep == FALSE) {
	addstr(", ");
	xloc += 2;
      } else {
	yep = FALSE;
      }
      if (strlen(spell_list[i].name) + xloc > COLS - 15) {
	xloc = 5;
	yloc++;
      }
      move(yloc, xloc);

      /* now place it */
      if (lng_tmp) {
	addch('*');
	xloc++;
      }
      addstr(spell_list[i].name);

      /* adjust position */
      xloc += strlen(spell_list[i].name);
    }

  }

  /* finish up the summary */
  if (yep) {
    mvaddstr(yloc, xloc, "[none]");
  }
  yloc += 2;

  /* now, the last little bit */
  mvaddstr(yloc++, 2, "Additional Notes:");
  mvaddstr(yloc++, 5, tmp_mgptr[pow_num].info);
  if (tmp_mgptr[pow_num].bute_num != BUTE_NUMBER) {
    move(yloc++, 5);
    xloc = tmp_mgptr[pow_num].bute_val;
    if (abs(xloc) < 5) {
      addstr("Small");
    } else if (abs(xloc) < 10) {
      addstr("Medium");
    } else {
      addstr("Large");
    }
    if (xloc < 0) {
      addstr(" decrease of ");
    } else {
      addstr(" increase of ");
    }
    addstr(bute_info[tmp_mgptr[pow_num].bute_num].name);
  }
  strcpy(string, "* = Other magical powers are also needed");
  xloc = (COLS - strlen(string) - 1) / 2;
  mvaddstr(++yloc, xloc, string);

  /* now get the input */
  presskey();
  return(0);
}

/*
 * mg_killpow - Remove a magical power from the nation
 *
 * Handles the complete process of selling back a magical power,
 * including validation that the power can be safely removed,
 * calculating refund value, and updating nation power data.
 * Prevents removal of powers required by other owned powers.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (removal completed or cancelled)
 *
 * Side Effects:
 *   - Removes magical power from nation's power set
 *   - Adds refund jewels to capital city materials
 *   - Calls mg_limit() to update selection bounds
 *   - May prompt user for removal confirmation
 *
 * Notes:
 *   - Only works in owned powers mode (mg_mode == TRUE)
 *   - Prevents removal during god browsing mode
 *   - Validates that power is not required by other owned powers
 *   - Refund is 1/3 of current purchase cost
 *   - God mode removes powers without jewel refund
 *   - Expert mode skips confirmation prompts
 *   - Requires capital city for jewel storage
 */
static int
mg_killpow PARM_0(void)
{
  long cost_amt = 0L, old_magic = 0L, lng_tmp = 0L;
  int i, maxval, j, pow_num = -1;
  MAGIC_PTR tmp_mgray = NULL;
  
  /* is there an individual power selected? */
  if ((mg_mode == FALSE) ||
      (mg_select >= mg_powcnt(mg_type, mg_mode))) {
    errormsg("Select a power currently in possession to delete it");
    return(0);
  }

  /* is god really wanting to do it */
  if (god_browsing == TRUE) {
    errormsg("You are just browsing, you can do trade-ins.");
    return(0);
  }

  /* check fiscal issues */
  if (is_god == FALSE) {
    /* check for the capital */
    if ((city_ptr =
	 citybyloc(ntn_ptr, ntn_ptr->capx, ntn_ptr->capy)) == NULL) {
      errormsg("You have not national treasury to store the jewels to");
      return(0);
    }

    /* check for the return value */
    cost_amt = mg_powcost(mg_type);
    cost_amt /= 3;
  }

  /* now check for currently conflicting powers */
  pow_num = mg_powint();
  old_magic = 1L << pow_num;
  for (j = 0; j < MAG_NUMBER; j++) {

    /* init it */
    tmp_mgray = mclass_list[j].pow_list;
    lng_tmp = ntn_ptr->powers[j];
    maxval = mclass_list[j].maxval;

    /* now check the full list of each class of powers */
    for (i = 0; i < maxval; i++) {
      /* check if the power is owned by the nation */
      if ((1L << i) & lng_tmp) {
	if (MAGIC(tmp_mgray[i].pow_need[mg_type], old_magic)) {
	  errormsg("That power is required by other powers you possess");
	  return(0);
	}
      }
    }
  }

  /* now to query the user */
  move(MG_COLNUM + 6, 0);
  clrtoeol();
  tmp_mgray = mclass_list[mg_type].pow_list;
  if (conq_expert == FALSE) {
    if (is_god == TRUE) {
      mvprintw(MG_COLNUM + 7, 0, "Remove the %s Power of %s? ", 
	       mclass_list[mg_type].name,
	       tmp_mgray[pow_num].name);
    } else {
      mvprintw(MG_COLNUM + 7, 0,
	       "Give up the %s Power of %s to gain %ld Jewels? ", 
	       mclass_list[mg_type].name,
	       tmp_mgray[pow_num].name,
	       cost_amt);
    }
    clrtoeol();
    if (y_or_n() == FALSE) {
      return(0);
    }
  }

  /* okay, they asked for it */
  global_long = ntn_ptr->powers[mg_type];
  global_int = mg_type;
  kill_powers(mg_type, old_magic);
  MGKADJ;

  /* now get the price */
  if (is_god == FALSE) {

    /* first take care of the actual jewels */
    CITY_MTRLS[MTRLS_JEWELS] += (itemtype) cost_amt;
    CADJMTRLS(MTRLS_JEWELS);

  }
  mg_limit();
  return(0);
}

/*
 * mg_switch - Cycle to the next magic class column
 *
 * Switches the current magic type to the next class in sequence
 * (Military -> Civilian -> Wizardry -> Military). Updates selection
 * bounds to ensure valid positioning in the new magic class.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Increments mg_type with wraparound (modulo 3)
 *   - Calls mg_limit() to ensure valid selection in new class
 *
 * Notes:
 *   - Provides quick way to cycle through magic classes
 *   - Maintains current selection index when possible
 *   - Part of the magic screen navigation key binding system
 *   - Bound to tab key for easy access
 */
static int
mg_switch PARM_0(void)
{
  /* shift columns to the right */
  mg_type = (mg_type + 1) % 3;
  mg_limit();
  return(0);
}

/*
 * mg_toggle - Toggle between owned and unowned power display modes
 *
 * Switches the magic screen between showing owned powers and showing
 * available (unowned) powers that can be purchased. Updates selection
 * bounds to accommodate the different power sets in each mode.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Toggles mg_mode flag (TRUE = owned, FALSE = unowned)
 *   - Calls mg_limit() to ensure valid selection in new mode
 *
 * Notes:
 *   - Owned mode shows powers currently possessed by nation
 *   - Unowned mode shows powers that can be purchased
 *   - Different power counts may require selection adjustment
 *   - Essential for both viewing current powers and shopping for new ones
 */
static int
mg_toggle PARM_0(void)
{
  /* switch and limit */
  mg_mode ^= TRUE;
  mg_limit();
  return(0);
}

/*
 * mg_init - Initialize magic screen state and key bindings
 *
 * Sets up the initial state for the magic power management interface
 * including selection position, magic type, and key binding system.
 * Ensures the magic screen starts in a consistent, known state.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Initializes mg_done, mg_select, mg_type global variables
 *   - Calls align_magic_keys() to set up key binding system
 *
 * Notes:
 *   - Sets mg_done = FALSE to enable main loop
 *   - Starts with mg_select = 0 (first power)
 *   - Defaults to MAG_MILITARY magic type
 *   - Key bindings initialized only once via align_magic_keys()
 */
static void
mg_init PARM_0(void)
{
  /* assign things properly */
  mg_done = FALSE;
  mg_select = 0;
  mg_type = MAG_MILITARY;

  /* bind the keys if not already done */
  align_magic_keys();
}

/*
 * mg_refresh - Force complete redraw of magic screen display
 *
 * Triggers a full screen redraw to clear any display corruption
 * or artifacts. Handles platform-specific refresh requirements
 * for proper display restoration.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Sets mg_fulldraw flag to trigger complete redraw
 *   - On VAXC platform, immediately clears and refreshes screen
 *
 * Notes:
 *   - Used when screen becomes corrupted or cluttered
 *   - Bound to Ctrl-L and Ctrl-R for standard refresh behavior
 *   - Platform-specific code for VMS/VAXC compatibility
 *   - Part of the magic screen key binding system
 */
static int
mg_refresh PARM_0(void)
{
  mg_fulldraw = TRUE;
#ifdef VAXC
  clear();
  refresh();
#endif /* VAXC */
  return(0);
}

/*
 * mg_exit - Exit the magic power management screen
 *
 * Signals termination of the magic screen interface by setting
 * the done flag. Causes the main magic screen loop to exit and
 * return control to the calling function.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Sets mg_done flag to TRUE
 *   - Causes magic screen main loop to terminate
 *
 * Notes:
 *   - Clean exit mechanism for magic screen interface
 *   - Bound to 'q' and 'Q' keys for intuitive quitting
 *   - Part of the magic screen key binding system
 */
static int
mg_exit PARM_0(void)
{
  mg_done = TRUE;
  return(0);
}

/*
 * show_magic - Main entry point for magical power management interface
 *
 * Provides the complete magical power management system allowing players
 * to view, purchase, and sell magical powers. Handles both normal play
 * and god mode operation with appropriate permission checking and state
 * management.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 if god mode or operation cancelled
 *   MOVECOST if normal play mode (consumes movement points)
 *
 * Side Effects:
 *   - Initializes magic screen state and display
 *   - Processes user input for power management
 *   - May enter/exit god mode as appropriate
 *   - Updates display based on user actions
 *
 * Notes:
 *   - God mode requires nation selection before proceeding
 *   - Main loop continues until mg_exit() sets mg_done flag
 *   - Uses key binding system for command interpretation
 *   - Unknown keys display error message with key identification
 *   - Properly handles god mode state restoration on exit
 *   - Costs movement points in normal play to prevent abuse
 */
int
show_magic PARM_0(void)
{
  FNCI fnc_ptr;

  /* quick initializations */
  if (is_god == TRUE) {
    if (get_god(-1, FALSE)) return(0);
  }

  /* go through and process all input */
  mg_init();
  redraw = DRAW_FULL;
  mg_fulldraw = TRUE;
  mg_mode = TRUE;
  while (mg_done == FALSE) {

    /* show it */
    mg_display();

    /* get the input */
    if ((fnc_ptr = parse_keys(mg_bindings, FALSE)) == NULL) {

      /* why that weren't right */
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Unknown key binding: ");
      show_str(string, FALSE);
      presskey();
      clear_bottom(1);

    } else {

      /* now that is better */
      (*fnc_ptr)();

    }

  }

  /* finish up */
  if (is_god == TRUE) {
    reset_god();
    return(0);
  }
  return(MOVECOST);
}

/*
 * mg_options - Access configuration options for magic screen
 *
 * Provides access to the global options system specifically for
 * the magic power management interface. Allows customization of
 * key bindings and other magic screen behaviors.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Opens options configuration interface
 *   - May modify key bindings for magic screen
 *   - Updates mg_bindings based on user changes
 *
 * Notes:
 *   - Uses global option_cmd() function with magic-specific context
 *   - Passes magic_keysys and mg_bindings for configuration
 *   - Part of the magic screen key binding system
 *   - Allows runtime customization of magic screen behavior
 */
static int
mg_options PARM_0(void)
{
  option_cmd(magic_keysys, &mg_bindings);
  return(0);
}

/* Give the functional information */
PARSE_STRUCT mg_funcs[] = {
  {mg_options, "conquer-options",
     "Adjust the conquer environment by changing various options"},
  {do_ignore, "ignore-key",
     "Do nothing when the key is pressed"},
  {mg_left, "magic-left",
     "Magic Screen, go left along the list of powers within the magic class"},
  {mg_buy, "magic-buy",
     "Magic Screen, purchase a power from within the current magic class"},
  {mg_killpow, "magic-delete",
     "Magic Screen, give back to the gods the currently selected power"},
  {mg_down, "magic-down",
     "Magic Screen, move downward among the list of magic powers"},
  {mg_exit, "magic-exit",
     "Magic Screen, leave the magic screen"},
  {mg_help, "magic-help",
     "Magic Screen, display help for the magic screen"},
  {mg_info, "magic-info",
     "Magic Screen, provide information about the current magic power"},
  {mg_right, "magic-right",
     "Magic Screen, go to the right along the list of powers"},
  {mg_switch, "magic-switch",
     "Magic Screen, switch to the next magic type"},
  {mg_toggle, "magic-toggle",
     "Magic Screen, toggle mode between owned and unowned powers"},
  {mg_up, "magic-up",
     "Magic Screen, move upward among the list of magic powers"},
  {mg_refresh, "redraw-screen",
     "Clear and redraw the screen so that any clutter is removed"}
};

/* Now actually define everything */
KBIND_STRUCT mg_klist[] = {
  { ESTR_IGN, do_ignore },
  { CSTR_B, mg_left },
  { CSTR_F, mg_right },
  { CSTR_L, mg_refresh },
  { CSTR_N, mg_down },
  { CSTR_P, mg_up },
  { CSTR_R, mg_refresh },
  { AKEY_UP1, mg_up },
  { AKEY_UP2, mg_up },
  { AKEY_DOWN1, mg_down },
  { AKEY_DOWN2, mg_down },
  { AKEY_LEFT1, mg_left },
  { AKEY_LEFT2, mg_left },
  { AKEY_RIGHT1, mg_right },
  { AKEY_RIGHT2, mg_right },
  { "\t", mg_switch },
  { "?", mg_help },
  { " ", do_ignore },
  { "B", mg_buy },
  { "b", mg_buy },
  { "D", mg_killpow },
  { "d", mg_killpow },
  { "H", mg_left },
  { "h", mg_left },
  { "I", mg_info },
  { "i", mg_info },
  { "L", mg_right },
  { "l", mg_right },
  { "N", mg_down },
  { "n", mg_down },
  { "O", mg_options },
  { "J", mg_down },
  { "j", mg_down },
  { "\n", mg_down },
  { "\r", mg_down },
  { "P", mg_up },
  { "p", mg_up },
  { "K", mg_up },
  { "k", mg_up },
  { ESTR_DEL, mg_up },
  { "\b", mg_up },
  { "T", mg_toggle },
  { "t", mg_toggle },
  { "Q", mg_exit },
  { "q", mg_exit }
};

/*
 * align_magic_keys - Initialize the magic screen key binding system
 *
 * Sets up the complete key binding system for the magical power
 * management interface. Initializes key mappings from the static
 * binding arrays and calculates array sizes for the binding system.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Initializes mg_bindings key binding structure (if not already done)
 *   - Sets magic_keysys array size values
 *   - Calls init_keys() to establish key binding mappings
 *
 * Notes:
 *   - Only initializes bindings once (checks mg_bindings == NULL)
 *   - Uses mg_klist[] array for key-to-function mappings
 *   - Uses mg_funcs[] array for function descriptions
 *   - Calculates array sizes using sizeof operations
 *   - Essential for magic screen functionality
 *   - Called from mg_init() during magic screen startup
 */
void
align_magic_keys PARM_0(void)
{
  /* initialize the system */
  if (mg_bindings == NULL) {
    magic_keysys.num_binds = (sizeof(mg_klist)/sizeof(KBIND_STRUCT));
    magic_keysys.num_parse = (sizeof(mg_funcs)/sizeof(PARSE_STRUCT));
    init_keys(&mg_bindings, mg_klist, magic_keysys.num_binds);
  }
}
