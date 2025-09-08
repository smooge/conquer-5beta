
/*
 * get_cargo - Prompt user to select cargo type (army or people)
 *
 * Displays an interactive prompt asking the user to choose between loading/unloading
 * army units or civilian populations. Used in naval cargo management operations.
 *
 * Parameters:
 *   str - Context string ("Load" or "Unload") displayed in the prompt
 *
 * Returns:
 *   TRUE if user selects army (A/a key pressed)
 *   FALSE if user selects people (P/p key pressed)  
 *   -1 if user cancels or presses invalid key
 *
 * Side Effects:
 *   - Displays prompt on screen line LINES-3
 *   - Clears to end of line and refreshes display
 *   - Waits for single character input via getch()
 *
 * Notes:
 *   - Case insensitive input (A/a and P/p accepted)
 *   - Uses curses library for screen display and input
 *   - Invalid input returns -1 without error message
 */
static int
get_cargo(str)
	char *str;
{
	int choice=(-1);

	mvprintw(LINES-3,0,"%s (A)rmy or (P)eople?",str);
	clrtoeol();
	refresh();
	switch(getch()) {
	case 'a':
	case 'A':
	     choice=TRUE;
	     break;
	case 'p':
	case 'P':
	     choice=FALSE;
	     break;
	default:
	     break;
	}

	return(choice);
}

/*
 * loadstat - Check if army unit is eligible for naval loading
 *
 * Determines whether an army unit with the given status can be loaded onto
 * a naval fleet. Certain army statuses prevent loading due to operational
 * constraints or current commitments.
 *
 * Parameters:
 *   status - Current status/state of the army unit to check
 *
 * Returns:
 *   FALSE if loading is invalid (army cannot be loaded)
 *   TRUE if loading is valid (army can be loaded onto fleet)
 *
 * Side Effects:
 *   None - pure validation function
 *
 * Notes:
 *   Invalid statuses for loading:
 *   - TRADED: Unit is involved in trade operations
 *   - GENERAL: General units cannot board ships
 *   - MILITIA: Local militia tied to specific location
 *   - GARRISON: Garrison troops tied to defensive positions
 *   - ONBOARD: Already loaded on another vessel
 *   
 *   All other statuses (ATTACK, DEFEND, etc.) allow loading
 */
int
loadstat(status)
	int status;
{
	switch(status) {
	case TRADED:
	case GENERAL:
	case MILITIA:
	case GARRISON:
	case ONBOARD:
		return(FALSE);
		break;
	default:
		break;
	}
	return(TRUE);
}

/*
 * loadfleet - Interactive naval cargo management system
 *
 * Comprehensive function for loading and unloading cargo (armies and people) 
 * to/from naval fleets. Handles user interaction, validation, space calculations,
 * and game state updates for all naval transport operations.
 *
 * Parameters:
 *   None - operates on global game state and selected fleet
 *
 * Returns:
 *   void - all results communicated through game state changes and error messages
 *
 * Side Effects:
 *   - Modifies fleet cargo (P_NARMY, P_NPEOP)
 *   - Updates army status and location (P_ASTAT, P_AMOVE)
 *   - Changes sector population (sct[x][y].people)
 *   - Adjusts movement points (P_NMOVE, P_AMOVE)
 *   - Updates display through curses interface
 *   - May display error messages to user
 *
 * Operation Logic:
 *   1. Validates selected unit is a navy fleet
 *   2. Ensures fleet is landed (not at sea)
 *   3. Calculates available cargo space
 *   4. Prompts user for load/unload operation
 *   5. Handles army or people cargo as selected
 *   6. Validates ownership and diplomatic restrictions
 *   7. Updates all relevant game state
 *   8. Deducts movement costs
 *
 * Validation Rules:
 *   - Fleet must be landed on shore
 *   - Army units: Only marines can disembark in foreign territory
 *   - Army units: Must be valid status for loading (see loadstat())
 *   - People: Must own sector to load civilians
 *   - Capacity: Army size vs ship capacity, people vs available space
 *   - Location: Army must be in same sector as fleet
 *
 * Movement Costs:
 *   - N_CITYCOST movement points deducted if not in friendly city
 *   - Zero movement if operation occurs outside cities
 *
 * Notes:
 *   - Uses global variables for current sector (XREAL, YREAL)
 *   - Relies on game constants (MAXARM, MAXNAVY, SHIPHOLD)
 *   - Integrates with diplomatic system for territory restrictions
 *   - Complex state machine with multiple user interaction points
 */
void
loadfleet()
{
	short nvynum;
	/* merchant holding unused */
	int ghold, mhold, unload, doarmy;
	int gcargo, mcargo, amount, armynum;

	clear_bottom(0);
	if((nvynum=getselunit()-MAXARM)>=0){
		if(nvynum>=MAXNAVY){
			errormsg("Invalid Navy");
			return;
		}
	} else {
		errormsg("Invalid Navy");
		return;
	}

	if(sct[XREAL][YREAL].altitude==WATER) {
		errormsg("Fleet must be landed");
		return;
	}

	/* process loading or unloading */
	ghold = fltghold(nvynum);
	mhold = fltmhold(nvynum);
	if((ghold==0)&&(mhold == 0)) {
		errormsg("No storage space available");
		return;
	} else if((P_NARMY==MAXARM)&&(P_NPEOP==0)) {
		unload=FALSE;
	} else {
		mvprintw(LINES-4,0,"Cargo:   People %d",P_NPEOP*mhold);
		if(P_NARMY==MAXARM) mvaddstr(LINES-4,25,"Army (none)");
		else mvprintw(LINES-4,25,"Army (%d)",P_NARMY);
		mvaddstr(LINES-3,0,"Do you wish to (L)oad or (U)nload?");
		refresh();
		switch(getch()) {
		case 'l':
		case 'L':
		     unload=FALSE;
		     break;
		case 'u':
		case 'U':
		     unload=TRUE;
		     break;
		default:
		     return;
		}
	}

	if(unload==TRUE) {
		if(P_NARMY==MAXARM) doarmy=FALSE;
		else if(P_NPEOP==0) doarmy=TRUE;
		else {
			doarmy=get_cargo("Unload");
		}
		if(doarmy==TRUE) {
			armynum=P_NARMY;
			if(sct[XREAL][YREAL].owner==0
			&& P_ATYPE!=A_MARINES
			&& P_ATYPE!=A_SAILOR) {
				errormsg("Only sailors or marines may disembark in unowned land");
				return;
			} else if (sct[XREAL][YREAL].owner!=country
			&& sct[XREAL][YREAL].owner!=0
			&& P_ATYPE!=A_MARINES) {
				errormsg("Only marines may disembark in someone else's land");
				return;
			}
			P_ASTAT=DEFEND;
			P_NARMY=MAXARM;
			if (!((sct[XREAL][YREAL].designation==DCITY
			|| sct[XREAL][YREAL].designation==DCAPITOL)
			&& (sct[XREAL][YREAL].owner==country
			|| (!ntn[sct[XREAL][YREAL].owner].dstatus[country]!=UNMET
			&& ntn[sct[XREAL][YREAL].owner].dstatus[country]<=NEUTRAL)))
			|| P_NMOVE < N_CITYCOST) {
				P_NMOVE=0;
			} else {
				P_NMOVE-= N_CITYCOST;
			}
			NADJMOV;
			NADJHLD;
			AADJSTAT;
		} else if(doarmy==FALSE){
			if (sct[XREAL][YREAL].owner!=country) {
				mvaddstr(LINES-3,0,"Unload in a sector you don't own? (y or n)");
				clrtoeol();
				refresh();
				if (getch()!='y') {
					return;
				}
			}
			mvaddstr(LINES-2,0,"Unload how many people?");
			refresh();
			amount=get_number();
			if(amount > mhold*P_NPEOP) {
				errormsg("There are not that many on board");
			} else if (amount > 0) {
				sct[XREAL][YREAL].people += amount;
				P_NPEOP=(unsigned char)((mhold*P_NPEOP-amount)/mhold);
				NADJHLD;
				if ((sct[XREAL][YREAL].designation!=DCITY
				&& sct[XREAL][YREAL].designation!=DCAPITOL)
				|| P_NMOVE < N_CITYCOST) {
					P_NMOVE=0;
				} else {
					P_NMOVE-= N_CITYCOST;
				}
				NADJMOV;
				SADJCIV;
			}
		}
	} else {
		clear_bottom(0);
		mcargo = mhold*(SHIPHOLD-P_NPEOP);
		if(P_NARMY==MAXARM) {
			gcargo = ghold*SHIPHOLD;
			mvprintw(LINES-4,0,"Available Space:  %d soldiers  %d people", gcargo, mcargo);
		} else {
			gcargo = 0;
			mvprintw(LINES-4,0,"Available Space:  0 soldiers  %d people", mcargo);
		}
		if(gcargo==0) doarmy=FALSE;
		else if(mcargo==0) doarmy=TRUE;
		else {
			doarmy=get_cargo("Load");
		}
		if(doarmy==TRUE) {
			mvaddstr(LINES-2,0,"Load what army?");
			refresh();
			armynum = get_number();
			if(armynum<0) {
				;
			} else if((armynum>=MAXARM)||(P_ASOLD<=0)
			||(loadstat(P_ASTAT)==FALSE)) {
				errormsg("Invalid Army");
			} else if((P_AXLOC!=XREAL)||(P_AYLOC!=YREAL)) {
				errormsg("Army not in sector");
			} else if(P_ASOLD > gcargo &&
			(P_ATYPE<MINLEADER || P_ATYPE>=MINMONSTER)) {
				errormsg("Army too large for fleet");
			} else {
				P_ASTAT=ONBOARD;
				P_AMOVE=0;
				P_NARMY=armynum;
				if (!((sct[XREAL][YREAL].designation==DCITY
				|| sct[XREAL][YREAL].designation==DCAPITOL)
				&& (sct[XREAL][YREAL].owner==country
				|| (!ntn[sct[XREAL][YREAL].owner].dstatus[country]!=UNMET
				&& ntn[sct[XREAL][YREAL].owner].dstatus[country]<=NEUTRAL)))
				|| P_NMOVE < N_CITYCOST) {
					P_NMOVE=0;
				} else {
					P_NMOVE-= N_CITYCOST;
				}
				NADJMOV;
				NADJHLD;
				AADJMOV;
				AADJSTAT;
			}
		} else if(doarmy==FALSE && mcargo!=0){
			mvaddstr(LINES-2,0,"Load how many people?");
			refresh();
			amount=get_number();
			if(sct[XREAL][YREAL].owner!=country) {
				errormsg("The people refuse to board");
			} else if(amount > mcargo) {
				errormsg("Not enough room on fleet");
			} else if(sct[XREAL][YREAL].people < amount) {
				errormsg("Not enough people in sector");
			} else if (amount > 0) {
				sct[XREAL][YREAL].people -= amount;
				P_NPEOP += (unsigned char)(amount / mhold);
				SADJCIV;
				if ((sct[XREAL][YREAL].designation!=DCITY
				&& sct[XREAL][YREAL].designation!=DCAPITOL)
				|| P_NMOVE < N_CITYCOST) {
					P_NMOVE=0;
				} else {
					P_NMOVE-= N_CITYCOST;
				}
				NADJMOV;
				NADJHLD;
			}
		} else if (mcargo==0) {
			errormsg("No more room onboard fleet");
		}
	}
}
