/* routines designed for manipulation / verification of keybindings */
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
#include "keyvalsX.h"

/*
 * parse - Interpret pending or upcoming keyboard input
 *
 * Main entry point for processing keyboard input in the global interface.
 * Reads keyboard input, looks up the associated function in the key binding
 * system, and executes the bound function. If no valid key binding is found,
 * displays an error message and waits for user acknowledgment.
 *
 * This function serves as the primary keyboard event dispatcher for the
 * main game interface, routing all keyboard input through the customizable
 * key binding system.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Reads keyboard input through parse_keys()
 *   - Executes bound functions via function pointer calls
 *   - Displays error messages for unknown key bindings
 *   - May modify screen display (error messages, function execution effects)
 *
 * Notes:
 *   - Uses global cq_bindings for key binding lookups
 *   - Error messages displayed on bottom line of screen
 *   - Blocks until user acknowledges unknown key binding errors
 *   - Function pointer execution may have wide-ranging side effects
 */
void
parse PARM_0(void)
{
  FNCI fnc_ptr;

  /* simple wrapper */
  if ((fnc_ptr = parse_keys(cq_bindings, FALSE)) == NULL) {

    /* ain't what I wanted */
    clear_bottom(1);
    mvaddstr(LINES - 1, 0, "Unknown key binding: ");
    show_str(string, FALSE);
    presskey();

  } else {

    /* call the function */
    (*fnc_ptr)();

  }
}

/* the function list */
PARSE_STRUCT parse_list[] = {
  {adj_display, "adjust-display",
     "Select a different display mode for the main map display"},
  {change_view, "adjust-view",
     "Alter the character sets used for displaying sector information"},
  {do_sarmy, "army-choose",
     "Go to a specific army unit within your nation"},
  {do_narmy, "army-next",
     "Go to the next army (in numeric order) within your nation"},
  {army_report, "army-report",
     "Examine a report on all of the army units within your nation"},
  {aunit_numbering, "auto-army-numbering",
     "Enter a full screen interface to adjust how army units are numbered"},
  {camp_info, "campaign-info",
     "Display statistics about the campaign on this world"},
  {do_scvn, "caravan-choose",
     "Go to a specific caravan unit within your nation"},
  {do_ncvn, "caravan-next",
     "Go to the next caravan (in numeric order) within your nation"},
  {cvn_report, "caravan-report",
     "Examine a reprot on all of the wagon caravans within your nation"},
  {cast_spells, "cast-spells",
     "When a leader is selected, have that leader cast a spell"},
  {change_desg, "change-designation",
     "Change the current major designation (Farm, Town, etc.) of the sector"},
  {do_scity, "city-choose",
     "Go to a specified supply center within the nation"},
  {ext_cmd, "command-unit",
     "Send a command to the currently selected unit"},
  {construct, "construct",
     "Perform constructions in the current sector (Roads, Churchs, etc.)"},
  {do_options, "conquer-options",
     "Adjust the conquer environment by changing various options"},
  {custom_display, "customize-display",
     "Build new or adjust current conquer display modes"},
  {do_diplomacy, "diplomacy-report",
     "Examine or adjust your diplomatic status with other nations"},
  {show_city_info, "economy-report",
     "Examine the economic and fiscal status of the cities in the nation"},
  {do_motdedit, "edit-motd",
     "In god-mode, edit the startup message for the campaign"},
  {enlist, "enlist",
     "Enlist army, navy or caravan units from the current sector"},
  {go_east, "go-east", "Move to the sector one to the East"},
  {go_north, "go-north", "Move to the sector one to the North"},
  {go_ne, "go-northeast", "Move to the sector one to the NorthEast"},
  {go_nw, "go-northwest", "Move to the sector one to the NorthWest"},
  {go_south, "go-south", "Move to the sector one to the South"},
  {go_se, "go-southeast", "Move to the sector one to the SouthEast"},
  {go_sw, "go-southwest", "Move to the sector one to the SouthWest"},
  {go_west, "go-west", "Move to the sector one to the West"},
  {group_report, "group-report",
     "Read a report on army units within the current sector"},
  {do_help, "help",
     "Provide documentation on Conquer"},
  {hl_all, "highlight-all",
     "Adjust the highlight selection of all of the slots"},
  {hl_current, "highlight-current",
     "Adjust the highlight setting of the slot at the focus"},
  {hl_horz, "highlight-horizontal",
     "Adjust the slots on the same horizontal as the focus"},
  {hl_ullr, "highlight-upleft-lowright",
     "Adjust the highlight setting of upper left and lower right slots"},
  {hl_urll, "highlight-upright-lowleft",
     "Adjust the hightight setting of upper right and lower left slots"},
  {hl_vert, "highlight-vertical",
     "Adjust the slots on the same verticle as the focus"},
  {do_ignore, "ignore-key", "Do nothing when the key is pressed"},
  {do_cjump, "jump-to-capital",
     "Set the current sector to be your nation's capital"},
  {do_mjump, "jump-to-mark",
     "Set the current sector to be that marked using the mark-sector command"},
  {do_sjump, "jump-to-pick",
     "Set the current sector as specified"},
  {do_relogin, "login-nation",
     "Exit your current nation and asume the identity of another"},
  {show_magic, "magic-info",
     "Provide a summary of national magical powers"},
  {mark_sector, "mark-sector",
     "Remember the current sector for visiting with the jump-to-mark command"},
  {do_email, "mail-nation",
     "Send a mail message to another nation or nations"},
  {do_movepeop, "move-people",
     "Relocate civilians from the current sector to another sector"},
  {move_select, "move-unit",
     "Move the currently selected unit to another sector"},
  {ntn_info, "nation-info",
     "Examine the statistics of your nation"},
  {do_snavy, "navy-choose",
     "Go to a specific naval unit within your nation"},
  {do_nnavy, "navy-next",
     "Go to the next navy (in numeric order) within your nation"},
  {navy_report, "navy-report",
     "Examine a report on all of the fleets within your nation"},
  {pick_next, "pick-next",
     "Select the unit one below the currently selected unit"},
  {pick_previous, "pick-previous",
     "Select the unit one above the currently selected unit"},
  {do_quit, "quit", "Close up shop and leave conquer"},
  {rmail_parse, "read-mail",
     "Read through the mail messages sent to your nation"},
  {do_paper, "read-paper",
     "Examine world news and events in the tabloids"},
  {recenter_screen, "recenter-screen",
     "Move the current sector to the center of the screen"},
  {do_refresh, "redraw-screen",
     "Redraw the screen after clearing it first"},
  {region_cmd, "region-command",
     "Manipulate a supply center or the region around it"},
  {scr_east, "scroll-east", "Scroll the screen towards the East"},
  {scr_north, "scroll-north", "Scroll the screen towards the North"},
  {scr_ne, "scroll-northeast", "Scroll the screen towards the NorthEast"},
  {scr_nw, "scroll-northwest", "Scroll the screen towards the NorthWest"},
  {scr_south, "scroll-south", "Scroll the screen towards the South"},
  {scr_se, "scroll-southeast", "Scroll the screen towards the SouthEast"},
  {scr_sw, "scroll-southwest", "Scroll the screen towards the SouthWest"},
  {scr_west, "scroll-west", "Scroll the screen towards the West"},
  {shift_focus, "shift-focus", "Shift the focus slot to the next position"},
  {shift_rfocus, "shift-focus-back",
     "Shift the focus slot to the previous position"},
  {toggle_infomode, "toggle-infomode",
     "Switch between the normal and the information mode display"},
  {troop_listing, "troop-listing",
     "Provide a listing of any troops not belonging to the current nation"},
#ifdef SPAWN_OUT
  {spawn_out, "spawn_out",
     "Spawn a command shell and suspend conquer"},
#else
  {spawn_out, "spawn_out",
     "Spawn a command shell and suspend conquer (unavailable)"},
#endif /* SPAWN_OUT */
  {trans_cmd, "transfer",
     "Transfer commodities between navies, caravans, and cities"},
  {do_score, "world-scores",
     "List the scores of the various nations within the world"},
  {zoom_in, "zoom-in",
     "Display the world map with less sectors and more detail"},
  {zoom_out, "zoom-out",
     "Display the world map with more sectors and less detail"},
  {do_unimplemented, "", "This key binding is reserved for later use"}
};

/* Default keybindings */
KBIND_STRUCT dflt_bindings[] = {
  { ESTR_IGN, do_ignore },
  { ESTR_ESC, ext_cmd },
  { ESTR_NUL, mark_sector },
  { "\t", troop_listing },
  { "!", spawn_out },
  { "@", toggle_infomode },
  { "#", aunit_numbering },
  { "$", do_motdedit },
  { "?", do_help },
  { ">", zoom_in },
  { "<", zoom_out },
  { "=", hl_all },
  { "/", hl_urll },
  { "\\", hl_ullr },
  { "|", hl_vert },
  { "-", hl_horz },
  { ",", hl_current },
  { "(", shift_rfocus },
  { ")", shift_focus },
  { "+", shift_focus },
  { " ", do_ignore },
  { "\n", do_ignore },
  { "\r", do_ignore },
  { CSTR_A, do_sarmy },
  { CSTR_D, change_view },
  { CSTR_E, do_scity },
  { CSTR_F, do_snavy },
  { CSTR_G, do_sarmy },
  { CSTR_L, do_refresh },
  { CSTR_R, recenter_screen },
  { CSTR_V, do_scvn },
  { CSTR_X, do_mjump },
  { "a", army_report },
  { "A", do_narmy },
  { "b", go_sw },
  { "B", scr_sw },
  { "c", change_desg },
  { "C", construct },
  { "d", adj_display },
  { "D", custom_display },
  { "e", show_city_info },
  { "E", enlist },
  { "f", navy_report },
  { "F", do_nnavy },
  { "g", group_report },
  { "G", do_narmy },
  { "h", go_west },
  { "H", scr_west },
  { "i", ntn_info },
  { "I", camp_info },
  { "j", go_south },
  { "J", scr_south },
  { "k", go_north },
  { "K", scr_north },
  { "l", go_east },
  { "L", scr_east },
  { "m", move_select },
  { "M", do_email },
  { "n", go_se },
  { "N", scr_se },
  { "o", pick_previous },
  { "O", do_options },
  { "p", pick_next },
  { "P", do_paper },
  { "q", do_quit },
  { "Q", do_quit },
  { "r", region_cmd },
  { "R", rmail_parse },
  { "s", do_score },
  { "S", do_diplomacy },
  { "t", trans_cmd },
  { "T", do_unimplemented },
  { "u", go_ne },
  { "U", scr_ne },
  { "v", cvn_report },
  { "V", do_ncvn },
  { "w", cast_spells },
  { "W", show_magic },
  { "x", do_sjump },
  { "X", do_cjump },
  { "y", go_nw },
  { "Y", scr_nw },
  { "z", do_relogin },
  { "Z", do_movepeop },
  { ".1", scr_sw },
  { ".2", scr_south },
  { ".3", scr_se },
  { ".4", scr_west },
  { ".5", do_ignore },
  { ".6", scr_east },
  { ".7", scr_nw },
  { ".8", scr_north },
  { ".9", scr_ne },
  { "1", go_sw },
  { "2", go_south },
  { "3", go_se },
  { "4", go_west },
  { "5", do_ignore },
  { "6", go_east },
  { "7", go_nw },
  { "8", go_north },
  { "9", go_ne }
};

/* information structure concerning global keybindings */
KEYSYS_STRUCT global_keysys = {
  "global", parse_list, dflt_bindings, 0, 0
};

/* pointer to the list of key bindings */
KLIST_PTR cq_bindings = NULL;

/*
 * num_bindings - Report the number of keys bound to a function
 *
 * Counts how many different key combinations are bound to the specified
 * function pointer. This is useful for determining if a function has
 * multiple key bindings or if it's unbound entirely.
 *
 * Iterates through the entire key binding list and counts all entries
 * that point to the specified function. This provides a measure of
 * redundancy in the key binding system.
 *
 * Parameters:
 *   klist_ptr - Pointer to the head of the key binding list to search
 *   f_ptr - Function pointer to count bindings for
 *
 * Returns:
 *   Integer count of key bindings for the specified function (0 if none)
 *
 * Side Effects:
 *   None - read-only operation
 *
 * Notes:
 *   - Linear search through entire key binding list
 *   - Returns 0 if function has no key bindings
 *   - Used for validation and key binding management interfaces
 */
int
num_bindings PARM_2(KLIST_PTR, klist_ptr, FNCI, f_ptr)
{
  int count = 0;

  /* count through all of the list */
  while (klist_ptr != NULL) {
    if (klist_ptr->key_data.func == f_ptr) {
      count++;
    }
    klist_ptr = klist_ptr->next;
  }
  return(count);
}

/*
 * sort_keys - Re-sort the entire key binding list
 *
 * Sorts the key binding linked list in lexicographic order by key sequence
 * strings. Uses a bubble sort algorithm to repeatedly scan through the list
 * and swap adjacent elements that are out of order.
 *
 * The sorting ensures that key bindings are stored in a predictable order,
 * which is essential for proper key clash detection and consistent key
 * binding validation. Sorted order also improves lookup performance and
 * provides consistent display ordering in configuration interfaces.
 *
 * Parameters:
 *   klist_ptr - Pointer to pointer to the head of the key binding list
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Modifies the order of elements in the key binding linked list
 *   - May update the head pointer if the first element changes position
 *
 * Notes:
 *   - Uses bubble sort algorithm (O(n²) complexity)
 *   - Handles empty lists and single-element lists efficiently
 *   - Preserves all key binding data, only reorders nodes
 *   - Essential for key clash detection algorithm to work correctly
 *   - Head pointer may be modified if first element moves
 */
void
sort_keys PARM_1(KLIST_PTR *, klist_ptr)
{
  KLIST_PTR k1_ptr, k2_ptr;
  int done = FALSE;

  /* check for no need of ordering */
  if (((*klist_ptr) == NULL) || ((*klist_ptr)->next == NULL)) {
    return;
  }

  /* perform sort */
  while (!done) {
    /* initialization */
    k1_ptr = (*klist_ptr);
    done = TRUE;

    /* test for initial element swap */
    if ( strcmp( k1_ptr->key_data.chlist,
		(k1_ptr->next)->key_data.chlist ) > 0) {
      (*klist_ptr) = k1_ptr->next;
      k1_ptr->next = (*klist_ptr)->next;
      (*klist_ptr)->next = k1_ptr;
      k1_ptr = (*klist_ptr);
    }

    /* sort remaining linked list */
    for (; (k1_ptr->next)->next != NULL; k1_ptr = k1_ptr->next) {
      k2_ptr = (k1_ptr->next)->next;
      if ( strcmp( (k1_ptr->next)->key_data.chlist,
		  k2_ptr->key_data.chlist ) > 0) {
	/* swap locations in list */
	(k1_ptr->next)->next = k2_ptr->next;
	k2_ptr->next = k1_ptr->next;
	k1_ptr->next = k2_ptr;
	/* continue sort */
	done = FALSE;
      }
    }
  }
}

/*
 * keys_clash - Does a key binding clash? (STATIC FUNCTION)
 *
 * Determines if two key binding strings would conflict with each other.
 * A clash occurs when one key sequence is a prefix of another, which would
 * create ambiguity in the key parsing system.
 *
 * For example, "a" and "ab" would clash because when the user types "a",
 * the system wouldn't know whether to execute the "a" command immediately
 * or wait for more characters to see if it's "ab".
 *
 * The function compares two key strings character by character and detects
 * prefix relationships in either direction.
 *
 * Parameters:
 *   s1 - First key binding string to compare
 *   s2 - Second key binding string to compare
 *
 * Returns:
 *   TRUE if the key bindings clash (one is prefix of other)
 *   FALSE if the key bindings are distinct and non-conflicting
 *
 * Side Effects:
 *   None - read-only comparison operation
 *
 * Notes:
 *   - Static function, internal to this module
 *   - Essential for key binding validation system
 *   - Prevents ambiguous key binding configurations
 *   - Used by check_keys() for comprehensive validation
 */
static int
keys_clash PARM_2(char *, s1, char *, s2)
{
  int count = 0;

  /* check the keys */
  while (1) {
    /* done yet? */
    if ((*s1 == '\0') ||
	(*s1 != *s2)) {
      break;
    }

    /* travel on */
    s1++;
    s2++;
    count++;
  }

  /* check ending characters */
  if (count > 0) {
    if ((*s1 == '\0') ||
	(*s2 == '\0')) {
      return(TRUE);
    }
  } else if (*s1 == *s2) {
    return(TRUE);
  }
  return(FALSE);
}

/*
 * check_keys - Verify that the key bindings are set properly
 *
 * Comprehensive validation function for key binding configurations.
 * Performs multiple checks to ensure the key binding system is consistent,
 * complete, and free of conflicts. This is essential for maintaining a
 * stable and predictable user interface.
 *
 * The function performs three main validation checks:
 * 1. Key clash detection - ensures no key sequences conflict with each other
 * 2. Function validation - verifies all bound functions exist in the parse table
 * 3. Completeness check - ensures all required functions have key bindings
 *
 * Parameters:
 *   l_of_keys - Pointer to pointer to the key binding list to validate
 *   key_info - Key system information structure containing parse table and counts
 *
 * Returns:
 *   Number of errors found (0 indicates valid configuration)
 *
 * Side Effects:
 *   - Sorts the key binding list for consistent validation order
 *   - Displays error messages for any validation failures found
 *   - May modify key binding list order due to sorting
 *
 * Notes:
 *   - Critical validation function for key binding system integrity
 *   - Must be called after any key binding modifications
 *   - Error messages provide specific details for troubleshooting
 *   - Allows functions to have multiple key bindings (not an error)
 *   - Used during configuration loading and interactive key binding
 */
int
check_keys PARM_2(KLIST_PTR *, l_of_keys, KEYSYS_STRUCT, key_info)
{
  KLIST_PTR kl_tmp, kl_last = NULL;
  register int i;
  int num_bad = 0;
  char is_bound[BIGLTH], buf[LINELTH], obuf[LINELTH];

  /* initialize settings */
  for (i = 0; i < key_info.num_parse; i++) {
    is_bound[i] = FALSE;
  }

  /* first sort the key list */
  sort_keys(l_of_keys);

  /* now go through the list, checking off items */
  for (kl_tmp = *l_of_keys;
       kl_tmp != NULL;
       kl_tmp = kl_tmp->next) {

    /* check for key clash */
    form_str(buf, kl_tmp->key_data.chlist, FALSE);
    if (kl_last != NULL) {
      if (keys_clash(kl_tmp->key_data.chlist,
		     kl_last->key_data.chlist)) {

	/* naughty naughty */
	num_bad++;
	sprintf(string, "%s key binding %s conflicts with %s.",
		key_info.name, buf, obuf);
	errormsg(string);

      }
    }

    /* match binding */
    for (i = 0; i < key_info.num_parse; i++) {
      if (kl_tmp->key_data.func == key_info.parse_p[i].func) break;
    }

    /* check search */ 
    if (i == key_info.num_parse) {
      sprintf(string, "Unknown function bound to %s key %s",
	      key_info.name, buf);
      errormsg(string);
      num_bad++;
    } else {
      is_bound[i] = TRUE;
    }

    /* store what the last key was */
    kl_last = kl_tmp;
    strcpy(obuf, buf);
  }

  /* now anything left unbound */
  for (i = 0; i < key_info.num_parse; i++) if (is_bound[i] == FALSE) {

    /* quick check through the bindings to allow double naming */
    for (kl_tmp = *l_of_keys;
	 kl_tmp != NULL;
	 kl_tmp = kl_tmp->next) {
      if (kl_tmp->key_data.func == key_info.parse_p[i].func) {
	is_bound[i] = TRUE;
	break;
      }
    }
    if (is_bound[i] == TRUE) continue;

    /* guess it wasn't bound to anything */
    num_bad++;
    sprintf(string, "Function %s not bound to any %s key",
	    key_info.parse_p[i].realname,
	    key_info.name);
    errormsg(string);
  }
  return(num_bad);
}

/*
 * find_func - Return the pointer to the function information
 *
 * Searches through a parse table to find the function information structure
 * that corresponds to a given function pointer. This is used to look up
 * function names, descriptions, and other metadata associated with a
 * function pointer.
 *
 * The function performs a linear search through the parse table array,
 * comparing function pointers until a match is found or the entire table
 * has been searched.
 *
 * Parameters:
 *   f_ptr - Function pointer to search for in the parse table
 *   p_list - Array of parse structures to search through
 *   maxnum - Maximum number of entries in the parse table array
 *
 * Returns:
 *   Pointer to matching PARSE_STRUCT if found, NULL if not found
 *
 * Side Effects:
 *   None - read-only search operation
 *
 * Notes:
 *   - Linear search algorithm (O(n) complexity)
 *   - Used for reverse lookup of function metadata
 *   - Essential for key binding configuration interfaces
 *   - Returns pointer to actual parse table entry, not a copy
 */
PARSE_PTR
find_func PARM_3(FNCI, f_ptr, PARSE_PTR, p_list, int, maxnum)
{
  int i;

  /* just find it */
  for (i = 0; i < maxnum; i++) {
    if (p_list[i].func == f_ptr) {
      return(&(p_list[i]));
    }
  }
  return((PARSE_PTR) NULL);
}

/*
 * find_keys - Locate a key binding from within the list
 *
 * Searches through the key binding linked list to find a specific key
 * binding by its key sequence string. This is used to check if a particular
 * key sequence is already bound to a function or to retrieve an existing
 * binding for modification.
 *
 * The function performs a linear search through the linked list, comparing
 * the key sequence strings until an exact match is found or the end of
 * the list is reached.
 *
 * Parameters:
 *   klist_ptr - Pointer to the head of the key binding list to search
 *   kstr - Key sequence string to search for (null-terminated)
 *
 * Returns:
 *   Pointer to matching key binding node if found, NULL if not found
 *
 * Side Effects:
 *   None - read-only search operation
 *
 * Notes:
 *   - Linear search through linked list (O(n) complexity)
 *   - Uses exact string matching (case-sensitive)
 *   - Returns pointer to actual list node, not a copy
 *   - Used for key binding lookup and modification operations
 */
KLIST_PTR
find_keys PARM_2(KLIST_PTR, klist_ptr, char *, kstr)
{
  KLIST_PTR tmp_ptr;

  /* look for it */
  for (tmp_ptr = klist_ptr;
       tmp_ptr != NULL;
       tmp_ptr = tmp_ptr->next) {
    if (strcmp(tmp_ptr->key_data.chlist, kstr) == 0) break;
  }
  return(tmp_ptr);
}

/*
 * bind_keys - Attach a key binding to the list
 *
 * Creates a new key binding node and adds it to the front of the key binding
 * linked list. Allocates memory for the new binding, copies the key sequence
 * string, stores the function pointer, and links it into the list.
 *
 * The new binding is added at the head of the list for efficiency. The list
 * can be sorted later if needed for validation or display purposes.
 *
 * Parameters:
 *   klist_ptr - Pointer to pointer to the head of the key binding list
 *   kstr - Key sequence string to bind (copied into new node)
 *   f_ptr - Function pointer to associate with the key sequence
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Allocates memory for new key binding node
 *   - Modifies the key binding list by adding new node at head
 *   - Updates head pointer to point to new node
 *   - Program terminates on memory allocation failure
 *
 * Notes:
 *   - Memory allocation failure causes program termination
 *   - New binding is added at list head (O(1) insertion)
 *   - Key string is copied, not referenced
 *   - No validation of duplicate bindings performed here
 *   - Head pointer is modified to point to new node
 */
void
bind_keys PARM_3(KLIST_PTR *, klist_ptr, char *, kstr, FNCI, f_ptr)
{
  KLIST_PTR tmp_kptr;

  /* obtain the memory */
  if ((tmp_kptr = (KLIST_PTR) malloc(sizeof(KLIST_STRUCT))) == NULL) {
    fprintf(stderr, "Unable to allocate memory for key bindings\n");
    abrt();
  }

  /* copy it */
  strcpy(tmp_kptr->key_data.chlist, kstr);
  tmp_kptr->key_data.func = f_ptr;
  tmp_kptr->next = *klist_ptr;

  /* now attach it */
  (*klist_ptr) = tmp_kptr;
}

/*
 * get_keys - Read in a list of keys to be bound
 *
 * Interactive function that prompts the user to enter a key sequence for
 * binding purposes. Provides a full-featured key entry interface with
 * editing capabilities including backspace, restart, and escape quoting.
 *
 * The function displays help text and prompts, then reads keys one by one
 * until the user presses Enter to finish. Special editing keys are handled
 * for user convenience, and a backslash can be used to quote special
 * characters.
 *
 * Parameters:
 *   pr_str - Prompt string to display to user (e.g., "Bind", "Unbind")
 *   out_str - Output buffer to store the entered key sequence
 *
 * Returns:
 *   Number of characters in the entered key sequence (0 if empty)
 *
 * Side Effects:
 *   - Displays prompt and help text on screen
 *   - Reads from keyboard and displays entered characters
 *   - Modifies screen display during key entry process
 *   - Stores result in provided output buffer
 *
 * Notes:
 *   - Interactive function requiring user input
 *   - Supports editing operations (backspace, restart, quote)
 *   - Maximum key sequence length limited by MAXKEYS
 *   - Backslash (\) can quote any character including special keys
 *   - Enter or Return terminates key sequence entry
 */
int
get_keys PARM_2(char *, pr_str, char *, out_str)
{
  int ch, done = FALSE, count = 0, xloc, yloc;

  /* ask for the keystrokes */
  clear_bottom(0);
  mvaddstr(LINES - 3, 0,
	   "Enter Keystrokes: ['\\' - Quote  DEL - Delete  ^U - Restart  RET - Finish]");
  mvprintw(LINES - 2, 0, "%s what key binding? ", pr_str);

  /* go until finished */
  while (done == FALSE) {

    /* access input */
    refresh();
    switch (ch = next_char()) {
    case '\n':
    case '\r':
      /* end it */
      done = TRUE;
      break;
    case CNTRL_U:
      /* remove all keys */
      while (count > 0) {
	unshow_char(out_str[--count], TRUE);
      }
      break;
    case '\b':
    case EXT_DEL:
      /* remove a character */
      if (count > 0) {
	unshow_char(out_str[--count], TRUE);
      }
      break;
    case '\\':
      /* just get the next character */
      addch('\\');
      refresh();
      ch = next_char();
      getyx(curscr, yloc, xloc);
      move(yloc, --xloc);
      addch(' ');
      move(yloc, xloc);
      /* falls through */
    default:
      /* add it in */
      if (count < MAXKEYS) {
	out_str[count++] = ch;
	show_char(ch, TRUE);
      }
      break;
    }
  }

  /* close up shoppe */
  out_str[count] = '\0';
  return(count);
}

/*
 * rm_keys - Remove a key from the list
 *
 * Searches for a specific key binding in the linked list and removes it
 * if found. Handles proper linked list node deletion including updating
 * pointers and freeing memory. The function safely handles removal from
 * any position in the list.
 *
 * The function performs a linear search to find the matching key binding,
 * then unlinks the node from the list and frees its memory. Special care
 * is taken to handle removal of the head node.
 *
 * Parameters:
 *   klist_ptr - Pointer to pointer to the head of the key binding list
 *   kstr - Key sequence string to remove from the list
 *
 * Returns:
 *   1 if key binding was found and removed, 0 if not found
 *
 * Side Effects:
 *   - Removes matching node from the linked list
 *   - Frees memory allocated to the removed node
 *   - May update head pointer if first node is removed
 *   - Updates link pointers to maintain list integrity
 *
 * Notes:
 *   - Safe removal from any position in linked list
 *   - Properly handles head node removal
 *   - Memory is freed to prevent leaks
 *   - Returns success/failure indicator
 *   - Used for unbinding key sequences
 */
int
rm_keys PARM_2(KLIST_PTR *, klist_ptr, char *, kstr)
{
  KLIST_PTR n_ptr = NULL, tmp_ptr;

  /* look for it */
  for (tmp_ptr = (*klist_ptr);
       tmp_ptr != NULL;
       tmp_ptr = tmp_ptr->next) {
    if (strcmp(tmp_ptr->key_data.chlist, kstr) == 0) break;
    n_ptr = tmp_ptr;
  }

  /* check for exit */
  if (tmp_ptr == NULL) return(0);

  /* nuke it */
  if (n_ptr == NULL) {
    (*klist_ptr) = tmp_ptr->next;
  } else {
    n_ptr->next = tmp_ptr->next;
  }
  tmp_ptr->next = NULL;
  free(tmp_ptr);
  return(1);
}

/*
 * init_keys - Initialize all of the key bindings
 *
 * Initializes a key binding list from a static array of default bindings.
 * Clears any existing bindings first, then creates new bindings from the
 * provided array. The resulting list is sorted for proper validation and
 * lookup operations.
 *
 * This function is typically used during program startup to establish
 * default key bindings or when resetting key bindings to defaults.
 * It ensures a clean slate by freeing existing bindings before creating
 * new ones.
 *
 * Parameters:
 *   klist_ptr - Pointer to pointer to the key binding list head
 *   kbind_list - Array of default key binding structures to copy
 *   numbind - Number of bindings in the default array
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Frees existing key binding list if present
 *   - Allocates memory for new key binding nodes
 *   - Creates new linked list from array data
 *   - Sorts the resulting key binding list
 *
 * Notes:
 *   - Always clears existing bindings first
 *   - Creates fresh list from static array data
 *   - Automatically sorts list after creation
 *   - Used for establishing default key bindings
 *   - Memory allocation failures cause program termination
 */
void
init_keys PARM_3(KLIST_PTR *, klist_ptr, KBIND_PTR, kbind_list, int, numbind)
{
  int i;

  /* check the keys */
  if ((*klist_ptr) != NULL) {
    free_keys(klist_ptr);
  }

  /* now copy everything */
  for (i = 0; i < numbind; i++) {

    /* bind it */
    bind_keys(klist_ptr, kbind_list[i].chlist, kbind_list[i].func);

  }

  /* sort it */
  sort_keys(klist_ptr);
}

/*
 * free_keys - Free up the memory of the key bindings
 *
 * Deallocates all memory used by a key binding linked list. Traverses the
 * entire list and frees each node, setting the head pointer to NULL when
 * complete. This prevents memory leaks when key binding lists are no
 * longer needed.
 *
 * The function safely handles empty lists and ensures all nodes are
 * properly freed. After completion, the list head pointer is NULL and
 * the list is ready for reuse or program termination.
 *
 * Parameters:
 *   klist_ptr - Pointer to pointer to the head of the key binding list
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Frees all memory allocated to key binding nodes
 *   - Sets head pointer to NULL
 *   - Destroys the entire linked list structure
 *
 * Notes:
 *   - Safe to call on empty lists (NULL head pointer)
 *   - Sets head pointer to NULL when complete
 *   - Used during cleanup and reinitialization
 *   - Essential for preventing memory leaks
 *   - List is unusable after this function completes
 */
void
free_keys PARM_1(KLIST_PTR *, klist_ptr)
{
  KLIST_PTR tmp_ptr;

  /* go through the entire key binding list */
  for (; (*klist_ptr) != NULL; (*klist_ptr) = tmp_ptr) {

    /* free up the memory */
    tmp_ptr = (*klist_ptr)->next;
    (*klist_ptr)->next = NULL;
    free(*klist_ptr);

  }
}

/*
 * parse_keys - Find a function from the given key list (COMPLEX FUNCTION)
 *
 * Core key parsing engine that reads keyboard input and matches it against
 * the key binding list to find the associated function. Handles multi-key
 * sequences by continuing to read input until a complete match is found
 * or no possible matches remain.
 *
 * The algorithm uses a stateful approach to handle prefix matching. If the
 * current input is a prefix of one or more key bindings, it continues
 * reading more keys. If an exact match is found, it returns the associated
 * function pointer. If no matches are possible, it returns NULL.
 *
 * Parameters:
 *   klist_ptr - Head of the key binding list to search
 *   echo_key - Whether to echo typed keys to the screen (TRUE/FALSE)
 *
 * Returns:
 *   Function pointer if matching key binding found, NULL if no match
 *
 * Side Effects:
 *   - Reads keyboard input character by character
 *   - May echo keystrokes to screen if echo_key is TRUE
 *   - Updates global 'string' variable with the key sequence
 *   - Blocks until complete key sequence is determined
 *
 * Notes:
 *   - Core parsing engine for the entire key binding system
 *   - Handles multi-character key sequences intelligently
 *   - Uses goto for state management (prefix detection)
 *   - Critical for main input processing loop
 *   - Global 'string' contains the final key sequence
 */
FNCI
parse_keys PARM_2(KLIST_PTR, klist_ptr, int, echo_key)
{
  char inp_str[MAXKEYS + 1];
  KLIST_PTR kl_ptr;
  int i, ch = -1, hold = 0, count = 0;

 get_some_more:
  /* enter more keys */
  inp_str[count++] = next_char();
  if (echo_key) {
    show_char(inp_str[count - 1], TRUE);
  }
  inp_str[count] = '\0';

  /* now check for key bindings */
  for (kl_ptr = klist_ptr;
       kl_ptr != NULL;
       kl_ptr = kl_ptr->next) {

    /* examine the string */
    for (i = 0; i <= count; i++) {
      ch = kl_ptr->key_data.chlist[i];
      if ((ch == '\0') ||
	  (ch != inp_str[i])) break;
    }

    /* now check the differing key */
    if ((ch == '\0') &&
	(inp_str[i] == '\0')) {
      hold = 2;
      break;
    } else if ((i > 0) &&
	       (inp_str[i] == '\0')) {
      hold = 1;
      break;
    }

  }

  /* was a command found? */
  if (hold == 1) {

    /* go back and get more keys */
    hold = 0;
    goto get_some_more;

  } else if (hold == 2) {

    /* found the command... give it back */
    strcpy(string, inp_str);
    return(kl_ptr->key_data.func);

  }

  /* record the mistake */
  strcpy(string, inp_str);
  return((FNCI) NULL);
}

/*
 * func_match - Return the matching function given the string name
 *
 * Searches through a parse table to find a function by its string name.
 * This is used to convert function names (as they appear in configuration
 * files or user input) back to function pointers for key binding operations.
 *
 * The function performs a linear search through the parse table array,
 * comparing the realname field of each entry until an exact match is found
 * or the entire table has been searched.
 *
 * Parameters:
 *   namestr - Function name string to search for (null-terminated)
 *   p_list - Array of parse structures to search through
 *   maxnum - Maximum number of entries in the parse table array
 *
 * Returns:
 *   Function pointer if matching function name found, NULL if not found
 *
 * Side Effects:
 *   None - read-only search operation
 *
 * Notes:
 *   - Linear search algorithm (O(n) complexity)
 *   - Used for configuration file parsing and interactive key binding
 *   - Enables function name to function pointer conversion
 *   - Case-sensitive string matching
 *   - Essential for text-based configuration systems
 */
FNCI
func_match PARM_3(char *, namestr, PARSE_PTR, p_list, int, maxnum)
{
  int i;

  /* simple search pattern */
  for (i = 0; i < maxnum; i++) {
    if (strcmp(p_list[i].realname, namestr) == 0) {
      return(p_list[i].func);
    }
  }
  return((FNCI) NULL);
}

/*
 * align_global_keys - Align all of the global keys
 *
 * Initializes the global key binding system if it hasn't been set up yet.
 * This function sets up the global key binding structures, calculates array
 * sizes, and creates the initial key binding list from the default bindings.
 *
 * The function is idempotent - it only initializes the system if the global
 * key binding list is NULL. This ensures the key binding system is ready
 * for use but doesn't reinitialize if already set up.
 *
 * This is typically called during program startup to ensure the global
 * key binding system is properly initialized before any key parsing begins.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   void
 *
 * Side Effects:
 *   - Initializes global_keysys structure with array sizes
 *   - Creates global cq_bindings list from default bindings
 *   - Sets up the entire global key binding system
 *
 * Notes:
 *   - Idempotent function - safe to call multiple times
 *   - Only initializes if cq_bindings is NULL
 *   - Calculates array sizes using sizeof operations
 *   - Essential setup function for the key binding system
 *   - Must be called before any key parsing operations
 */
void
align_global_keys PARM_0(void)
{
  /* initialize the system */
  if (cq_bindings == NULL) {
    global_keysys.num_binds = (sizeof(dflt_bindings)/sizeof(KBIND_STRUCT));
    global_keysys.num_parse = (sizeof(parse_list)/sizeof(PARSE_STRUCT));
    init_keys(&cq_bindings, dflt_bindings, global_keysys.num_binds);
  }
}
