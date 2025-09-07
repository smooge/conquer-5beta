/* This file provides quick reference to data structures */
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

/*
 * armybynum - Retrieve army pointer by unique identifier
 *
 * Searches the current nation's army list to find an army with the specified
 * ID number. The search is optimized by taking advantage of the sorted nature
 * of the army list - if a higher ID is encountered, the search terminates
 * early since the target army cannot exist later in the list.
 *
 * Parameters:
 *   idnum - Unique army identifier to search for
 *
 * Returns:
 *   ARMY_PTR - Pointer to the army structure if found
 *   NULL - If army with specified ID does not exist
 *
 * Side Effects:
 *   - Uses global ntn_ptr to access current nation's army list
 *   - Read-only operation, no modifications made
 *
 * Notes:
 *   - Assumes army list is sorted by armyid in ascending order
 *   - Early termination optimization when higher ID encountered
 *   - Thread-safe for read operations (no modifications)
 */
ARMY_PTR
armybynum PARM_1 (int, idnum)
{
  ARMY_PTR a1_ptr;

  for (a1_ptr = ntn_ptr->army_list; a1_ptr != NULL;
       a1_ptr = a1_ptr->next) {
    if (a1_ptr->armyid == idnum)
      break;
    if (a1_ptr->armyid > idnum)
      return ((ARMY_PTR) NULL);
  }
  return (a1_ptr);
}

/*
 * navybynum - Retrieve navy pointer by unique identifier
 *
 * Searches the current nation's navy list to find a naval unit with the
 * specified ID number. Uses the same optimization as armybynum, taking
 * advantage of the sorted navy list to terminate early when a higher ID
 * is encountered.
 *
 * Parameters:
 *   idnum - Unique navy identifier to search for
 *
 * Returns:
 *   NAVY_PTR - Pointer to the navy structure if found
 *   NULL - If navy with specified ID does not exist
 *
 * Side Effects:
 *   - Uses global ntn_ptr to access current nation's navy list
 *   - Read-only operation, no modifications made
 *
 * Notes:
 *   - Assumes navy list is sorted by navyid in ascending order
 *   - Early termination optimization when higher ID encountered
 *   - Parallel implementation to armybynum for naval units
 */
NAVY_PTR 
navybynum PARM_1 (int, idnum)
{
  NAVY_PTR n1_ptr;

  for (n1_ptr = ntn_ptr->navy_list;
       n1_ptr != NULL;
       n1_ptr = n1_ptr->next) {
    if (n1_ptr->navyid == idnum)
      break;
    if (n1_ptr->navyid > idnum)
      return((NAVY_PTR) NULL);
  }
  return (n1_ptr);
}

/*
 * cvnbynum - Retrieve caravan pointer by unique identifier
 *
 * Searches the current nation's caravan list to find a caravan with the
 * specified ID number. Caravans are trading units that move between cities
 * and sectors. Uses the same sorted list optimization as army and navy
 * lookup functions.
 *
 * Parameters:
 *   idnum - Unique caravan identifier to search for
 *
 * Returns:
 *   CVN_PTR - Pointer to the caravan structure if found
 *   NULL - If caravan with specified ID does not exist
 *
 * Side Effects:
 *   - Uses global ntn_ptr to access current nation's caravan list
 *   - Read-only operation, no modifications made
 *
 * Notes:
 *   - Assumes caravan list is sorted by cvnid in ascending order
 *   - Early termination optimization when higher ID encountered
 *   - Part of the trade and economic system
 */
CVN_PTR 
cvnbynum PARM_1 (int, idnum)
{
  CVN_PTR c1_ptr;

  for (c1_ptr = ntn_ptr->cvn_list; c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {
    if (c1_ptr->cvnid == idnum)
      break;
    if (c1_ptr->cvnid > idnum)
      return((CVN_PTR) NULL);
  }
  return (c1_ptr);
}

/*
 * itembynum - Retrieve item pointer by unique identifier
 *
 * Searches the current nation's item list to find an item (commodity or
 * equipment) with the specified ID number. Items represent tradeable goods,
 * weapons, armor, and other commodities that can be carried by units or
 * stored in cities.
 *
 * Parameters:
 *   idnum - Unique item identifier to search for
 *
 * Returns:
 *   ITEM_PTR - Pointer to the item structure if found
 *   NULL - If item with specified ID does not exist
 *
 * Side Effects:
 *   - Uses global ntn_ptr to access current nation's item list
 *   - Read-only operation, no modifications made
 *
 * Notes:
 *   - Assumes item list is sorted by itemid in ascending order
 *   - Early termination optimization when higher ID encountered
 *   - Items can be commodities, weapons, armor, or special equipment
 */
ITEM_PTR
itembynum PARM_1 (int, idnum)
{
  ITEM_PTR i1_ptr;

  for (i1_ptr = ntn_ptr->item_list; i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {

    if (i1_ptr->itemid == idnum)
      break;
    if (i1_ptr->itemid > idnum)
      return((ITEM_PTR) NULL);

  }
  return (i1_ptr);
}

/*
 * ntnbyname - Find nation by name with special case handling
 *
 * Searches for a nation by name string, with special handling for system
 * entities. Returns the nation pointer and sets global_int to the nation
 * index. Supports special keywords "god" (unowned sectors) and "news"
 * (newspaper system) that set global_int but return NULL.
 *
 * Parameters:
 *   str - Nation name string to search for (case-sensitive partial match)
 *
 * Returns:
 *   NTN_PTR - Pointer to matching nation structure
 *   NULL - If nation not found, or for special entities ("god", "news")
 *
 * Side Effects:
 *   - Sets global_int to nation index (0-MAXNTN-1)
 *   - Sets global_int to UNOWNED for "god"
 *   - Sets global_int to NEWSPAPER for "news"
 *   - Uses str_test() for partial string matching
 *
 * Notes:
 *   - Special entities are handled before nation search
 *   - Uses partial string matching (prefix match)
 *   - global_int provides the nation index for calling functions
 *   - Returns NULL with special global_int values for system entities
 */
NTN_PTR
ntnbyname PARM_1 (char *, str)
{
  NTN_PTR n1_ptr = NULL;

  /* is it god? */
  if (strcmp(str, "god") == 0) {
    global_int = UNOWNED;
    return(NULL);
  }

  /* is it the news */
  if (strcmp(str, "news") == 0) {
    global_int = NEWSPAPER;
    return(NULL);
  }

  /* compare with nation names */
  for (global_int = 0; global_int < MAXNTN; global_int++) {
    if ((n1_ptr = world.np[global_int]) != NULL)
      if (str_test(n1_ptr->name, str) == 0) break;
  }
  if (global_int == MAXNTN) n1_ptr = (NTN_PTR) NULL;

  return(n1_ptr);
}

/*
 * citybyname - Find city by name within current nation
 *
 * Searches the current nation's city list for a city with a name matching
 * the provided string. Uses partial string matching to allow abbreviated
 * city names for user convenience.
 *
 * Parameters:
 *   str - City name string to search for (case-sensitive partial match)
 *
 * Returns:
 *   CITY_PTR - Pointer to matching city structure
 *   NULL - If no city found with matching name
 *
 * Side Effects:
 *   - Uses global ntn_ptr to access current nation's city list
 *   - Read-only operation, no modifications made
 *
 * Notes:
 *   - Uses str_test() for partial string matching (prefix match)
 *   - Only searches within the current player's nation
 *   - Returns first matching city if multiple cities have similar names
 *   - Case-sensitive comparison
 */
CITY_PTR
citybyname PARM_1 (char *, str)
{
  CITY_PTR c1_ptr;

  /*search through list of cities*/
  for (c1_ptr = ntn_ptr->city_list; c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {
    if (str_test(c1_ptr->name, str) == 0) 
      break;
  }
  return (c1_ptr);
}

/*
 * citybyloc - Find city by map coordinates within specified nation
 *
 * Searches a specific nation's city list for a city located at the given
 * map coordinates. This is useful for determining if a city exists at a
 * particular location before performing actions like movement or siege.
 *
 * Parameters:
 *   n1_ptr - Pointer to nation to search (must not be NULL)
 *   x - X coordinate on the map
 *   y - Y coordinate on the map
 *
 * Returns:
 *   CITY_PTR - Pointer to city at specified location
 *   NULL - If no city exists at location or n1_ptr is NULL
 *
 * Side Effects:
 *   - Read-only operation, no modifications made
 *   - Validates input parameter before search
 *
 * Notes:
 *   - Searches specified nation, not current player's nation
 *   - Exact coordinate matching required (x,y must match exactly)
 *   - Used for collision detection and location-based queries
 *   - Safe handling of NULL nation pointer
 */
CITY_PTR
citybyloc PARM_3 (NTN_PTR, n1_ptr, int, x, int, y)
{
  CITY_PTR c1_ptr;

  /* check input */
  if (n1_ptr == NULL)
    return((CITY_PTR) NULL);

  /* search through list of cities in the given nation */
  for (c1_ptr = n1_ptr->city_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {
    if ((c1_ptr->xloc == x) && (c1_ptr->yloc == y)) break;
  }

  return (c1_ptr);
}

/*
 * uclassbyname - Find unit class index by name string
 *
 * Searches the unit class definitions to find a class matching the provided
 * name string. Unit classes categorize units into groups like "cavalry",
 * "infantry", "naval", etc. This function enables name-based unit class
 * lookup for command parsing and unit categorization.
 *
 * Parameters:
 *   match_str - Unit class name string to search for (partial match supported)
 *
 * Returns:
 *   int - Index of matching unit class (0 to num_aclasses-1)
 *   -1 - If no matching unit class found
 *
 * Side Effects:
 *   - Uses global ainfo_clist array for class names
 *   - Uses global num_aclasses for array bounds
 *   - Read-only operation, no modifications made
 *
 * Notes:
 *   - Uses str_test() for partial string matching
 *   - Returns first matching class if multiple partial matches exist
 *   - Used for command parsing and unit classification
 *   - Class index can be used with other unit system functions
 */
int
uclassbyname PARM_1(char *, match_str)
{
  int i;

  /* simple search routine */
  for (i = 0; i < num_aclasses; i++) {
    if (str_test(ainfo_clist[i], match_str) == 0) {
      return(i);
    }
  }
  return(-1);
}

/*
 * unitbyname - Find unit type index by name string
 *
 * Searches the unit type definitions to find a specific unit type matching
 * the provided name string. Unit types are specific military units like
 * "pikeman", "knight", "archer", etc. This enables name-based unit type
 * lookup for command parsing and unit creation.
 *
 * Parameters:
 *   match_str - Unit type name string to search for (partial match supported)
 *
 * Returns:
 *   int - Index of matching unit type (0 to num_armytypes-1)
 *   -1 - If no matching unit type found
 *
 * Side Effects:
 *   - Uses global ainfo_list array for unit type definitions
 *   - Uses global num_armytypes for array bounds
 *   - Read-only operation, no modifications made
 *
 * Notes:
 *   - Uses str_test() for partial string matching
 *   - Returns first matching type if multiple partial matches exist
 *   - Used for command parsing and unit creation/identification
 *   - Type index can be used with unit creation and stat lookup functions
 */
int
unitbyname PARM_1(char *, match_str)
{
  int i;

  /* simple search routine */
  for (i = 0; i < num_armytypes; i++) {
    if (str_test(ainfo_list[i].name, match_str) == 0) {
      return(i);
    }
  }
  return(-1);
}

/*
 * itembysct - Find items at specific sector coordinates
 *
 * Searches through a provided item list to find items located at the
 * specified map coordinates. Optionally filters by item type. This is
 * used to find items dropped on the ground, stored in sectors, or
 * available for pickup at specific locations.
 *
 * Parameters:
 *   i_list - Starting point in item list to search from
 *   x - X coordinate on the map
 *   y - Y coordinate on the map  
 *   type - Item type filter (-1 for any type, specific type for filtering)
 *
 * Returns:
 *   ITEM_PTR - Pointer to first matching item at location
 *   NULL - If no matching item found at specified coordinates
 *
 * Side Effects:
 *   - Read-only operation, no modifications made
 *   - Can be called iteratively with returned pointer to find multiple items
 *
 * Notes:
 *   - Exact coordinate matching required (x,y must match exactly)
 *   - Type filter allows searching for specific item categories
 *   - Use type = -1 to find any item type at the location
 *   - Can start search from middle of list for continuation searches
 */
ITEM_PTR
itembysct PARM_4(ITEM_PTR, i_list, int, x, int, y, int, type)
{
  ITEM_PTR i1_ptr;

  /* search */
  for (i1_ptr = i_list;
       i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {
    if ((i1_ptr->xloc == x) &&
	(i1_ptr->yloc == y)) {
      if ((type == -1) ||
	  (type == i1_ptr->type)) break;
    }
  }
  return(i1_ptr);
}

/*
 * itembyarmy - Find items associated with specific army
 *
 * Searches the current nation's item list for items that are carried by
 * or associated with a specific army. Armies can carry equipment, weapons,
 * armor, and other items that provide combat bonuses or special abilities.
 *
 * Parameters:
 *   idnum - Army identifier to search for in item associations
 *
 * Returns:
 *   ITEM_PTR - Pointer to first item associated with the army
 *   NULL - If no items found associated with specified army
 *
 * Side Effects:
 *   - Uses global ntn_ptr to access current nation's item list
 *   - Read-only operation, no modifications made
 *
 * Notes:
 *   - Items with matching armyid are carried by that specific army
 *   - Can be called iteratively to find multiple items for same army
 *   - Used for inventory management and equipment display
 *   - Items provide stat bonuses and special abilities to armies
 */
ITEM_PTR
itembyarmy PARM_1(int, idnum)
{
  ITEM_PTR i1_ptr;

  for (i1_ptr = ntn_ptr->item_list; i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {
    if (i1_ptr->armyid == idnum)
      break;
  }
  return (i1_ptr);
}

/*
 * itembynavy - Find items associated with specific navy
 *
 * Searches the current nation's item list for items that are carried by
 * or associated with a specific naval unit. Naval units can carry equipment
 * like weapons, armor, and special naval items that provide combat bonuses
 * or navigation advantages.
 *
 * Parameters:
 *   idnum - Navy identifier to search for in item associations
 *
 * Returns:
 *   ITEM_PTR - Pointer to first item associated with the navy
 *   NULL - If no items found associated with specified navy
 *
 * Side Effects:
 *   - Uses global ntn_ptr to access current nation's item list
 *   - Read-only operation, no modifications made
 *
 * Notes:
 *   - Items with matching navyid are carried by that specific naval unit
 *   - Can be called iteratively to find multiple items for same navy
 *   - Used for naval inventory management and equipment display
 *   - Naval items may include special equipment for sea warfare
 */
ITEM_PTR
itembynavy PARM_1(int, idnum)
{
  ITEM_PTR i1_ptr;

  for (i1_ptr = ntn_ptr->item_list; i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {
    if (i1_ptr->navyid == idnum)
      break;
  }
  return (i1_ptr);
}

/*
 * itembycvn - Find items associated with specific caravan
 *
 * Searches the current nation's item list for items that are carried by
 * or associated with a specific caravan. Caravans transport trade goods,
 * commodities, and equipment between cities and markets. This function
 * helps manage caravan cargo and trading inventories.
 *
 * Parameters:
 *   idnum - Caravan identifier to search for in item associations
 *
 * Returns:
 *   ITEM_PTR - Pointer to first item associated with the caravan
 *   NULL - If no items found associated with specified caravan
 *
 * Side Effects:
 *   - Uses global ntn_ptr to access current nation's item list
 *   - Read-only operation, no modifications made
 *
 * Notes:
 *   - Items with matching cvnid are transported by that specific caravan
 *   - Can be called iteratively to find multiple items for same caravan
 *   - Used for trade management and caravan inventory display
 *   - Essential for economic gameplay and resource distribution
 */
ITEM_PTR
itembycvn PARM_1(int, idnum)
{
  ITEM_PTR i1_ptr;

  for (i1_ptr = ntn_ptr->item_list; i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {
    if (i1_ptr->cvnid == idnum)
      break;
  }
  return (i1_ptr);
}
