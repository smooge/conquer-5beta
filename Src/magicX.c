/* This file handles magic power enhancement and spell results */
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
#include "magicX.h"
#include "racesX.h"
#include "elevegX.h"

/*
 * mil_uppow - Apply military magic power enhancements to current nation
 *
 * Increases nation combat statistics based on the specific military power
 * being added. Each military power type provides different combat bonuses
 * including attack, defense, and movement enhancements.
 *
 * Parameters:
 *   powerval - Military magic power identifier (MM_* constants)
 *              Must be a valid military magic power type
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Modifies current nation's aplus (attack bonus percentage)
 *   - Modifies current nation's dplus (defense bonus percentage)
 *   - Modifies current nation's maxmove (maximum movement points)
 *   - Uses global ntn_ptr for the current nation
 *
 * Notes:
 *   - Static function, only called internally by add_powers()
 *   - Powers include warrior/captain/warlord (+10% combat), archery (+5% attack/+10% defense),
 *     equine/avian (+4 movement), sapper (+10% defense), armor (+10% defense/-2 movement),
 *     ninja (+5% attack)
 *   - Movement changes have minimum limits to prevent invalid values
 */
static void
mil_uppow PARM_1(long, powerval)
{
  /* increase statistics based on the power */
  switch (powerval) {
  case MM_WARRIOR:
  case MM_CAPTAIN:
  case MM_WARLORD:
    /* add 10% to combat ability */
    ntn_ptr->aplus += 10;
    ntn_ptr->dplus += 10;
    break;
  case MM_ARCHERY:
    /* add 5% to attack and 10% to defense */
    ntn_ptr->aplus += 5;
    ntn_ptr->dplus += 10;
    break;
  case MM_EQUINE:
    /* add 4 to movment */
    ntn_ptr->maxmove += 4;
    break;
  case MM_SAPPER:
    /* add +10% defense */
    ntn_ptr->dplus += 10;
    break;
  case MM_ARMOR:
    /* add +10% defense reduce movement by 2 */
    ntn_ptr->dplus += 10;
    if (ntn_ptr->maxmove > 6) {
      ntn_ptr->maxmove -= 2;
    } else {
      ntn_ptr->maxmove = 4;
    }
    break;
  case MM_AVIAN:
    /* add 4 to movment */
    ntn_ptr->maxmove += 4;
    break;
  case MM_NINJA:
    /* add 5% to attack */
    ntn_ptr->aplus += 5;
    break;
  default:
    /* doesn't enhance nation statistics */
    break;
  }
}

/*
 * civ_uppow - Apply civilian magic power enhancements to current nation
 *
 * Increases nation civilian statistics based on the specific civilian power
 * being added. Civilian powers primarily affect reproduction rates, with
 * overflow effects redirected to combat or movement bonuses when reproduction
 * limits are reached.
 *
 * Parameters:
 *   powerval - Civilian magic power identifier (MC_* constants)
 *              Must be a valid civilian magic power type
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Modifies current nation's repro (reproduction rate percentage)
 *   - Modifies current nation's aplus/dplus when reproduction overflows
 *   - Modifies current nation's maxmove for certain powers
 *   - Uses global ntn_ptr and race_info array
 *
 * Notes:
 *   - Static function, only called internally by add_powers()
 *   - Powers include religion/urban (+2% repro), breeder (+2% repro/-5% combat),
 *     democracy (+10% combat/+2 movement/+1% repro), socialism (+1% repro),
 *     roads (+4 movement)
 *   - Reproduction has race-specific limits; overflow converts to other bonuses
 *   - Complex overflow logic redirects excess reproduction benefits
 */
static void
civ_uppow PARM_1(long, powerval)
{
  /* now increase statistics based on the power */
  switch (powerval) {
  case MC_RELIGION:
    /* add 2% to reproduction; overflow goes to attack */
    if (ntn_ptr->repro < race_info[ntn_ptr->race].repro_limit - 1) {
      ntn_ptr->repro += 2;
    } else {
      ntn_ptr->aplus += (2 - (race_info[ntn_ptr->race].repro_limit -
			      ntn_ptr->repro)) * 5;
      ntn_ptr->repro = race_info[ntn_ptr->race].repro_limit;
    }
    break;
  case MC_URBAN:
    /* add 2% to repro; overflow goes to movment */
    if (ntn_ptr->repro < race_info[ntn_ptr->race].repro_limit - 1) {
      ntn_ptr->repro += 2;
    } else {
      ntn_ptr->maxmove += (2 - (race_info[ntn_ptr->race].repro_limit -
				ntn_ptr->repro)) * 2;
      ntn_ptr->repro = race_info[ntn_ptr->race].repro_limit;
    }
    break;
  case MC_BREEDER:
    /* add 2% to repro, subtract 5% from combat; overflow to combat */
    if (ntn_ptr->repro < race_info[ntn_ptr->race].repro_limit - 1) {
      ntn_ptr->repro += 2;
      ntn_ptr->aplus -= 5;
      ntn_ptr->dplus -= 5;
    } else {
      ntn_ptr->aplus -= (2 - (ntn_ptr->repro -
			      race_info[ntn_ptr->race].repro_limit)) * 5;
      ntn_ptr->dplus -= (2 - (ntn_ptr->repro - 
			      race_info[ntn_ptr->race].repro_limit)) * 5;
      ntn_ptr->repro = race_info[ntn_ptr->race].repro_limit;
    }
    break;
  case MC_DEMOCRACY:
    /* improve combat, movement, and repro */
    ntn_ptr->aplus += 10;
    ntn_ptr->dplus += 10;
    ntn_ptr->maxmove += 2;
    if (ntn_ptr->repro < race_info[ntn_ptr->race].repro_limit)
      ntn_ptr->repro++;
    else {
      ntn_ptr->aplus += 2;
      ntn_ptr->dplus += 2;
    }
    break;
  case MC_SOCIALISM:
    /* add 1% to reproduction */
    if (ntn_ptr->repro < race_info[ntn_ptr->race].repro_limit)
      ntn_ptr->repro++;
    else {
      ntn_ptr->aplus += 5;
      ntn_ptr->dplus += 5;
    }
    break;
  case MC_ROADS:
    /* increase movement by 4 */
    ntn_ptr->maxmove += 4;
    break;
  default:
    /* doesn't enhance nation statistics */
    break;
  }
}

/*
 * wiz_uppow - Apply wizardry magic power enhancements to current nation
 *
 * Increases nation combat statistics based on the specific wizardry power
 * being added. Wizardry powers provide focused combat bonuses, either
 * enhancing attack or defense capabilities.
 *
 * Parameters:
 *   powerval - Wizardry magic power identifier (MW_* constants)
 *              Must be a valid wizardry magic power type
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Modifies current nation's aplus (attack bonus) for vision-based powers
 *   - Modifies current nation's dplus (defense bonus) for illusion-based powers
 *   - Uses global ntn_ptr for the current nation
 *
 * Notes:
 *   - Static function, only called internally by add_powers()
 *   - Illusion/hidden/void powers provide +5% defense bonus
 *   - Vision/seeall powers provide +5% attack bonus
 *   - Most wizardry powers don't affect base nation statistics
 */
static void
wiz_uppow PARM_1(long, powerval)
{
  /* now increase statistics based on the power */
  switch (powerval) {
  case MW_ILLUSION:
  case MW_HIDDEN:
  case MW_THEVOID:
    /* increase defense by 5% */
    ntn_ptr->dplus += 5;
    break;
  case MW_SEEALL:
  case MW_VISION:
    /* increase attack by 5% */
    ntn_ptr->aplus += 5;
    break;
  default:
    /* doesn't enhance nation statistics */
    break;
  }
}

/*
 * mil_downpow - Remove military magic power enhancements from current nation
 *
 * Decreases nation combat statistics by reversing the effects of military
 * powers being removed. This function undoes the exact bonuses that were
 * applied by mil_uppow() for the corresponding power.
 *
 * Parameters:
 *   powerval - Military magic power identifier (MM_* constants)
 *              Must be a valid military magic power type
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Reduces current nation's aplus (attack bonus percentage)
 *   - Reduces current nation's dplus (defense bonus percentage)
 *   - Modifies current nation's maxmove (reverses movement changes)
 *   - Uses global ntn_ptr for the current nation
 *
 * Notes:
 *   - Static function, only called internally by kill_powers()
 *   - Exactly reverses the effects of mil_uppow() for each power type
 *   - Movement changes have minimum limits to prevent invalid values
 *   - Armor power reversal increases movement (+2) to offset original penalty
 */
static void
mil_downpow PARM_1(long, powerval)
{
  switch (powerval) {
  case MM_WARRIOR:
  case MM_CAPTAIN:
  case MM_WARLORD:
    /* take away 10% from combat ability */
    ntn_ptr->aplus -= 10;
    ntn_ptr->dplus -= 10;
    break;
  case MM_ARCHERY:
    /* subtract 5% from attack and 10% from defense */
    ntn_ptr->aplus -= 5;
    ntn_ptr->dplus -= 10;
    break;
  case MM_EQUINE:
    /* take away 4 from movment */
    if (ntn_ptr->maxmove > 8) {
      ntn_ptr->maxmove -= 4;
    } else {
      ntn_ptr->maxmove = 4;
    }
    break;
  case MM_SAPPER:
    /* remove 10% from defense */
    ntn_ptr->dplus -= 10;
    break;
  case MM_ARMOR:
    /* remove 10% from defense and increase movement by 2 */
    ntn_ptr->dplus -= 10;
    ntn_ptr->maxmove += 2;
    break;
  case MM_AVIAN:
    /* remove 4 from movment */
    if (ntn_ptr->maxmove > 8) {
      ntn_ptr->maxmove -= 4;
    } else {
      ntn_ptr->maxmove = 4;
    }
    break;
  case MM_NINJA:
    /* remove 5% from attack */
    ntn_ptr->aplus -= 5;
    break;
  default:
    /* doesn't enhance nation statistics */
    break;
  }
}

/*
 * civ_downpow - Remove civilian magic power enhancements from current nation
 *
 * Decreases nation civilian statistics by reversing the effects of civilian
 * powers being removed. Handles complex underflow logic when reproduction
 * rates fall below minimum thresholds.
 *
 * Parameters:
 *   powerval - Civilian magic power identifier (MC_* constants)
 *              Must be a valid civilian magic power type
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Reduces current nation's repro (reproduction rate percentage)
 *   - Modifies current nation's aplus/dplus when reproduction underflows
 *   - Modifies current nation's maxmove for certain powers
 *   - Uses global ntn_ptr for the current nation
 *
 * Notes:
 *   - Static function, only called internally by kill_powers()
 *   - Reverses the effects of civ_uppow() for each power type
 *   - Complex underflow logic when reproduction goes below minimum (5%)
 *   - Underflow effects are taken from combat or movement stats
 *   - Breeder power removal adds back combat bonuses before reducing reproduction
 */
static void
civ_downpow PARM_1(long, powerval)
{
  /* now decrease statistics based on the power */
  switch (powerval) {
  case MC_RELIGION:
    /* remove 2% from reproduction; underflow comes from attack */
    if (ntn_ptr->repro > 6) {
      ntn_ptr->repro -= 2;
    } else {
      ntn_ptr->aplus -= (7 - ntn_ptr->repro) * 5;
      ntn_ptr->repro = 5;
    }
    break;
  case MC_URBAN:
    /* remove 2% from repro; underflow comes from movment */
    if (ntn_ptr->repro > 6) {
      ntn_ptr->repro -= 2;
    } else {
      ntn_ptr->maxmove -= (7 - ntn_ptr->repro) * 2;
      ntn_ptr->repro = 5;
    }
    break;
  case MC_BREEDER:
    /* remove 2% from repro, add 5% to combat; underflow from combat */
    ntn_ptr->aplus += 5;
    ntn_ptr->dplus += 5;
    if (ntn_ptr->repro > 6) {
      ntn_ptr->repro -= 2;
    } else {
      ntn_ptr->aplus -= (7 - ntn_ptr->repro) * 5;
      ntn_ptr->dplus -= (7 - ntn_ptr->repro) * 5;
      ntn_ptr->repro = 5;
    }
    break;
  case MC_DEMOCRACY:
    /* decrease combat, movement, and repro */
    ntn_ptr->aplus -= 10;
    ntn_ptr->dplus -= 10;
    ntn_ptr->maxmove -= 2;
    if (ntn_ptr->repro > 5) ntn_ptr->repro--;
    else {
      ntn_ptr->aplus -= 2;
      ntn_ptr->dplus -= 2;
    }
    break;
  case MC_SOCIALISM:
    /* subtract 1% from reproduction */
    if (ntn_ptr->repro > 5) ntn_ptr->repro--;
    else {
      ntn_ptr->aplus -= 5;
      ntn_ptr->dplus -= 5;
    }
    break;
  case MC_ROADS:
    /* decrease movement by 4 */
    ntn_ptr->maxmove -= 4;
    break;
  default:
    /* doesn't enhance nation statistics */
    break;
  }
}

/*
 * wiz_downpow - Remove wizardry magic power enhancements from current nation
 *
 * Decreases nation combat statistics by reversing the effects of wizardry
 * powers being removed. This function undoes the exact bonuses that were
 * applied by wiz_uppow() for the corresponding power.
 *
 * Parameters:
 *   powerval - Wizardry magic power identifier (MW_* constants)
 *              Must be a valid wizardry magic power type
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Reduces current nation's aplus (attack bonus) for vision-based powers
 *   - Reduces current nation's dplus (defense bonus) for illusion-based powers
 *   - Uses global ntn_ptr for the current nation
 *
 * Notes:
 *   - Static function, only called internally by kill_powers()
 *   - Exactly reverses the effects of wiz_uppow() for each power type
 *   - Comment says "Increase" but function actually decreases (removes bonuses)
 */
static void
wiz_downpow PARM_1(long, powerval)
{
  /* now decrease statistics based on the power */
  switch (powerval) {
  case MW_ILLUSION:
  case MW_HIDDEN:
  case MW_THEVOID:
    /* decrease defense by 5% */
    ntn_ptr->dplus -= 5;
    break;
  case MW_SEEALL:
  case MW_VISION:
    /* decrease attack by 5% */
    ntn_ptr->aplus -= 5;
    break;
  default:
    /* doesn't enhance nation statistics */
    break;
  }
}

/*
 * add_powers - Add magic powers to current nation with stat enhancements
 *
 * Adds specified magic powers to the current nation and applies their
 * corresponding statistical enhancements. Processes a bitmask of powers,
 * checking each bit position for new powers that the nation doesn't already
 * possess, then applies the appropriate bonuses.
 *
 * Parameters:
 *   powtype - Magic power category (MAG_MILITARY, MAG_CIVILIAN, MAG_WIZARDRY)
 *             Must be valid index (0 <= powtype < MAG_NUMBER)
 *   powlist - Bitmask of powers to add (each bit represents one power)
 *             Can be 0 (no-op) or combination of power flags
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Adds new powers to ntn_ptr->powers[powtype] using ADDMAGIC macro
 *   - Applies stat bonuses by calling appropriate *_uppow() functions
 *   - Skips powers the nation already possesses
 *   - Uses global ntn_ptr for the current nation
 *
 * Notes:
 *   - Public interface for adding magic powers to nations
 *   - Validates input parameters before processing
 *   - Uses bit shifting to iterate through power positions
 *   - Only applies bonuses for newly acquired powers, not existing ones
 *   - Thread safety depends on ntn_ptr global state management
 */
void
add_powers PARM_2(int, powtype, long, powlist)
{
  int count, maxval;
  long curpow = 1L;

  /* don't bother with invalid input */
  if (ntn_ptr == NULL || powlist == 0L) return;
  if ((powtype < 0) || (powtype >= MAG_NUMBER)) return;

  /* match any powers in the list */
  maxval = mclass_list[powtype].maxval;
  for (count = 0; count <= maxval; count++, curpow = curpow << 1) {

    /* check if the power is already there */
    if (curpow & ntn_ptr->powers[powtype]) continue;

    /* check for the power */
    if (curpow & powlist) {

      /* upgrade things */
      switch (powtype) {
      case MAG_MILITARY:
	mil_uppow(curpow);
	break;
      case MAG_CIVILIAN:
	civ_uppow(curpow);
	break;
      default:
	wiz_uppow(curpow);
	break;
      }

    }
  }

  /* now add in the powers */
  ADDMAGIC(ntn_ptr->powers[powtype], powlist);
}

/*
 * kill_powers - Remove magic powers from current nation with stat reductions
 *
 * Removes specified magic powers from the current nation and reverses their
 * corresponding statistical enhancements. Processes a bitmask of powers,
 * checking each bit position for existing powers that the nation possesses,
 * then removes the appropriate bonuses.
 *
 * Parameters:
 *   powtype - Magic power category (MAG_MILITARY, MAG_CIVILIAN, MAG_WIZARDRY)
 *             Must be valid index (0 <= powtype < MAG_NUMBER)
 *   powlist - Bitmask of powers to remove (each bit represents one power)
 *             Can be 0 (no-op) or combination of power flags
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Removes powers from ntn_ptr->powers[powtype] using KILLMAGIC macro
 *   - Removes stat bonuses by calling appropriate *_downpow() functions
 *   - Only affects powers the nation currently possesses
 *   - Uses global ntn_ptr for the current nation
 *
 * Notes:
 *   - Public interface for removing magic powers from nations
 *   - Validates input parameters before processing
 *   - Uses bit shifting to iterate through power positions
 *   - Only removes bonuses for powers the nation actually has
 *   - Companion function to add_powers() with reverse effects
 */
void
kill_powers PARM_2(int, powtype, long, powlist)
{
  int count, maxval;
  long curpow = 1L;

  /* don't bother if there is no current nation or no powers */
  if (ntn_ptr == NULL || powlist == 0L) return;
  if ((powtype < 0) || (powtype >= MAG_NUMBER)) return;

  /* match any powers in the list */
  maxval = mclass_list[powtype].maxval;
  for (count = 0; count <= maxval; count++, curpow = curpow << 1) {

    /* check if they have it */
    if (!(ntn_ptr->powers[powtype] & curpow)) continue;

    /* check for the power */
    if (curpow & powlist) {

      /* now decrease statistics based on the power */
      switch (powtype) {
      case MAG_MILITARY:
	mil_downpow(curpow);
	break;
      case MAG_CIVILIAN:
	civ_downpow(curpow);
	break;
      default:
	wiz_downpow(curpow);
	break;
      }

    }
  }

  /* now add in the powers */
  KILLMAGIC(ntn_ptr->powers[powtype], powlist);
}

/*
 * magic_ok - Check if a specific magic power can be acquired by current nation
 *
 * Validates whether the current nation can acquire a specific magic power
 * by checking prerequisites, race restrictions, and existing powers. Used
 * to enforce magic system rules and prevent invalid power acquisitions.
 *
 * Parameters:
 *   magic_type - Magic power category (MAG_MILITARY, MAG_CIVILIAN, MAG_WIZARDRY)
 *                Must be valid magic type index
 *   new_mint   - Power index within the category (0-based bit position)
 *                Converted to power flag via (1L << new_mint)
 *
 * Returns:
 *   TRUE (non-zero) if the power can be acquired
 *   FALSE (0) if the power cannot be acquired
 *
 * Side Effects:
 *   - None (read-only validation function)
 *   - Uses global ntn_ptr and race_info array
 *
 * Notes:
 *   - Checks if power is already possessed (returns FALSE)
 *   - Checks race-specific power limitations using race_info
 *   - Verifies all prerequisite powers are met across all magic categories
 *   - Uses MAGIC macro for bitmask testing
 *   - Critical for maintaining magic system balance and progression
 */
int
magic_ok PARM_2(int, magic_type, int, new_mint)
{
  long new_magic = (1L << new_mint);
  MAGIC_PTR tmp_mgptr = NULL;

  /* check if it is already in place */
  if (MAGIC(ntn_ptr->powers[magic_type], new_magic)) {
    return(FALSE);
  }

  /* check if it is okay by race */
  if (MAGIC(race_info[ntn_ptr->race].pow_limit[magic_type],
	    new_magic)) {
    return(FALSE);
  }

  /* now check all of the necessary prereqs */
  tmp_mgptr = mclass_list[magic_type].pow_list;
  if (MIL_MAGIC(tmp_mgptr[new_mint].pow_need[MAG_MILITARY]) &&
      CIV_MAGIC(tmp_mgptr[new_mint].pow_need[MAG_CIVILIAN]) &&
      WIZ_MAGIC(tmp_mgptr[new_mint].pow_need[MAG_WIZARDRY])) {
    return(TRUE);
  }

  /* oh well, guess not */
  return(FALSE);
}

/*
 * rand_magic - Generate and validate a random magic power for current nation
 *
 * Attempts to generate a random valid magic power that the current nation
 * can acquire. Uses repeated random selection with validation until a valid
 * power is found or maximum attempts are reached. Useful for random magic
 * rewards and power generation.
 *
 * Parameters:
 *   magic_type - Magic power category (MAG_MILITARY, MAG_CIVILIAN, MAG_WIZARDRY)
 *                Must be valid magic type index
 *
 * Returns:
 *   Power bitmask (1L << power_index) if successful power was found
 *   0L if no valid power could be generated within attempt limit
 *
 * Side Effects:
 *   - None (does not modify nation state, only generates candidates)
 *   - Uses rand_val() for random number generation
 *
 * Notes:
 *   - Maximum 500 attempts to find valid power (prevents infinite loops)
 *   - Uses magic_ok() to validate each candidate power
 *   - Returns actual power bitmask, not the index
 *   - May return 0L if nation has acquired all available powers for the type
 *   - Depends on proper random number generator initialization
 */
long
rand_magic PARM_1( int, magic_type )
{
  long new_magic = 0L;
  int new_int, number, count = 0, was_set = FALSE;

  /* make note of the maximum value available */
  number = mclass_list[magic_type].maxval;

  /* randomly obtain a new magic power */
  while (was_set == FALSE && count++ < 500) {

    /* generate the number */
    new_int = rand_val(number);
    new_magic = 1L << new_int;

    /* check if it is okay */
    if (magic_ok(magic_type, new_int) == FALSE) {
      continue;
    }

    /* it was added */
    was_set = TRUE;
  }

  /* give back the power */
  if (was_set) return(new_magic);

  /* otherwise, nothing was set */
  return(0L);
}
  
/*
 * mgk_sctval - Calculate magical combat bonus for nation in specific sector
 *
 * Computes the combat bonus percentage that a nation receives from their
 * magic powers when fighting in a specific map sector. Different terrain
 * types and elevations provide advantages to nations with appropriate magic
 * powers, creating strategic terrain considerations.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation structure (can be different from global ntn_ptr)
 *            Must not be NULL for meaningful results
 *   x      - Map x-coordinate of the sector
 *   y      - Map y-coordinate of the sector
 *            Coordinates must be valid map positions
 *
 * Returns:
 *   Integer combat bonus percentage (can be positive, negative, or zero)
 *   0 if nation pointer is NULL or coordinates are off-map
 *
 * Side Effects:
 *   - None (read-only calculation function)
 *   - Uses global sct[][] map array for terrain data
 *
 * Notes:
 *   - Vegetation bonuses: desert/ice (+20 dervish/destroyer), forest (+20 druid/+5 botanist),
 *     swamp (+5 water), jungle (+30 amphibian)
 *   - Altitude bonuses: water (+30 water/+20 marine/+10 sailor/-10 earth),
 *     mountain (+20 earth)
 *   - Bonuses stack if nation has multiple relevant powers
 *   - Critical for tactical combat calculations in terrain-dependent battles
 *   - Uses MAGIC macro to test for specific power possession
 */
int
mgk_sctval PARM_3(NTN_PTR, n1_ptr, int, x, int, y)
{
  int hold = 0;

  /* check the sector for magical bonuses */
  if ((n1_ptr == NULL) ||
      !XY_ONMAP(x, y)) return(hold);

  /* check the magic */
  switch (sct[x][y].vegetation) {
  case VEG_DESERT:
  case VEG_ICE:
    /* dervish have advantage on such land */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_DERVISH)) {
      hold += 20;
    }

    /* so do destroyers */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DESTROYER)) {
      hold += 20;
    }
    break;
  case VEG_FOREST:
    /* forests provide much help for druids */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DRUIDISM)) {
      hold += 20;
    }

    /* slight bonus to botanists */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_BOTANY)) {
      hold += 5;
    }
    break;
  case VEG_WOOD:
    /* the trees help the druids */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DRUIDISM)) {
      hold += 10;
    }

    /* slight bonus to botanists */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_BOTANY)) {
      hold += 2;
    }
    break;
  case VEG_SWAMP:
    /* slight advantage to water power */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_WATER)) {
      hold += 5;
    }
    /* fallthrough */
  case VEG_JUNGLE:
    /* advantage in such sectors */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_AMPHIBIAN)) {
      hold += 30;
    }
    break;
  default:
    /* not much worth doing here */
    break;
  }

  /* check the altitude now */
  switch (sct[x][y].altitude) {
  case ELE_WATER:
    /* major bonus for water magicians */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_WATER)) {
      hold += 30;
    }
    /* earth creatures not as good in water */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_EARTH)) {
      hold -= 10;
    }
    /* sailors get a bit and marines get more */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_SAILOR)) {
      hold += 10;
    }
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_MARINE)) {
      hold += 20;
    }
    break;
  case ELE_MOUNTAIN:
    /* good bonus for earth people */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_EARTH)) {
      hold += 20;
    }
    break;
  }
  return(hold);
}
