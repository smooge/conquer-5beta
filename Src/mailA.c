/*
 * mailA.c - In-Game Messaging and News System
 *
 * This file implements the comprehensive messaging system for the Conquer
 * game, providing both mail delivery to individual nations and news
 * broadcasting functionality. The system supports multiple messaging
 * paradigms including grouped messages and complex multi-line messages
 * with advanced features like placeholders and deferred content.
 *
 * Key messaging subsystems:
 * - Grouped messaging for broadcasting to multiple recipients
 * - Multi-line message composition with placeholder support
 * - Sorted news system for game events and announcements
 * - File-based message delivery with nation-specific mail files
 * - Coordinate-based message formatting for location references
 *
 * The messaging system is essential for player communication, game
 * event notification, and maintaining awareness of world events
 * during game updates.
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
#include "dataA.h"
#include "activeX.h"

/*
 * Global mail file pointer for message output
 * Used by multiple functions to write to nation-specific mail files
 */
extern FILE *mailfp;

/*
 * Static variables for grouped mail messaging system
 * 
 * gmsg_during - Flag indicating if a grouped message session is active
 * gmsg_target - Array tracking which nations have received the current group message
 * gmsg_header - Header text for the current group message
 */
static int gmsg_during = FALSE;
static int gmsg_target[ABSMAXNTN];
static char gmsg_header[LINELTH];

/*
 * msg_ginit - Initialize grouped message session
 *
 * This function begins a new grouped messaging session, allowing
 * multiple nations to receive the same message with a common header.
 * It initializes the grouped message system state and prepares for
 * message delivery to multiple recipients.
 *
 * The grouped messaging system allows efficient broadcasting of
 * identical messages (like game events or announcements) to multiple
 * nations without duplicating the message content for each recipient.
 *
 * Parameters:
 *   str - Header text for the grouped message session
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Sets gmsg_during flag to TRUE to indicate active session
 *   - Copies header string to gmsg_header global variable
 *   - Resets gmsg_target array to FALSE for all nations
 *   - Writes error messages to fupdate if session already active
 *
 * Notes:
 *   - Only one grouped message session can be active at a time
 *   - Error is logged if attempting to start while session is active
 *   - Must be paired with msg_gfinish() to properly close session
 *   - Used in conjunction with msg_grouped() for message delivery
 */
void
msg_ginit PARM_1(char *, str)
{
  int i;

  /* give 'em hell */
  if (gmsg_during == TRUE) {
    fprintf(fupdate, "Mailing Error: attempting to start a new group\n");
    fprintf(fupdate, "old = %s\n", gmsg_header);
    fprintf(fupdate, "new = %s\n", str);
    return;
  }

  /* initial info */
  strcpy(gmsg_header, str);
  gmsg_during = TRUE;
  for (i = 0; i < ABSMAXNTN; i++) {
    gmsg_target[i] = FALSE;
  }
}

/*
 * msg_grouped - Send message to specific nation in grouped session
 *
 * This function delivers a message to a specific nation as part of an
 * active grouped messaging session. It handles file creation, header
 * insertion (for first message to each nation), coordinate formatting,
 * and message delivery to nation-specific mail files.
 *
 * The function manages both deity (god) messages and player nation
 * messages, formatting coordinates relative to the recipient's
 * perspective and ensuring each nation receives the group header
 * only once per session.
 *
 * Parameters:
 *   to     - Target nation ID (UNOWNED for deity messages)
 *   xloc   - X coordinate for message location (-1 if no location)
 *   yloc   - Y coordinate for message location (-1 if no location)
 *   msgstr - Message text to deliver
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Opens and closes nation-specific mail file for writing
 *   - Writes group header if this is first message to the nation
 *   - Formats and writes coordinate information and message text
 *   - Updates gmsg_target array to track nations that have received messages
 *   - Temporarily modifies global ntn_ptr for coordinate calculations
 *
 * Notes:
 *   - Requires active grouped message session (gmsg_during == TRUE)
 *   - Skips inactive or NULL nations automatically
 *   - Uses relative coordinate system for location display
 *   - Mail files are named "{nation_name}.{msgtag}"
 *   - Coordinates are displayed as [x,y] prefix when provided
 *   - Deity messages go to "god.{msgtag}" file
 */
void
msg_grouped PARM_4(ntntype, to, int, xloc, int, yloc, char *, msgstr)
{
  NTN_PTR n1_ptr = NULL, ntemp_ptr = ntn_ptr;
  char tname[NAMELTH + 1];
  char fname[FILELTH + 1];

  /* check if a message is set */
  if (gmsg_during == FALSE) {
    errormsg("Warning: msg_grouped() ... no group message started");
    return;
  }

  /* check if it is to the deity */
  if (to == UNOWNED) {
    strcpy(tname, "god");
  } else {
    if (((n1_ptr = world.np[to]) == NULL) ||
	!n_ispc(n1_ptr->active)) {
      return;
    }
    strcpy(tname, n1_ptr->name);
  }

  /* open the output file */
  sprintf(fname, "%s.%s", tname, msgtag);
  if ((mailfp = fopen(fname, "a")) == NULL) {
    sprintf(string, "Could not open mail file %s\n", fname);
    errormsg(string);
    return;
  }

  /* check if this is the first message for this nation */
  if (gmsg_target[to] == FALSE) {
    gmsg_target[to] = TRUE;
    mail_header(NULL, NULL, gmsg_header, NULL, 0);
  }

  /* display the coordinates of the message */
  if ((xloc != -1) && (yloc != -1)) {
    ntn_ptr = n1_ptr;
    fprintf(mailfp, " [%d,%d]  \t", xloc_relative(xloc), yloc_relative(yloc));
  } else {
    fprintf(mailfp, "    ");
  }
  fprintf(mailfp, msgstr);
  putc('\n', mailfp);
  fclose(mailfp);
  mailfp = NULL;
  ntn_ptr = ntemp_ptr;
}

/*
 * msg_gfinish - End grouped message session
 *
 * This function terminates an active grouped messaging session by
 * clearing the session flag. It provides a clean way to close a
 * grouped message session after all messages have been delivered.
 *
 * The function is the counterpart to msg_ginit() and should be called
 * after all grouped messages have been sent using msg_grouped().
 * It ensures the messaging system is returned to a clean state.
 *
 * Parameters:
 *   void (no parameters)
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Sets gmsg_during flag to FALSE to indicate session end
 *   - Allows new grouped message sessions to be started
 *
 * Notes:
 *   - Safe to call even if no grouped session is active
 *   - Does not clear the target array or header (not necessary)
 *   - Must be called to properly terminate grouped message sessions
 *   - Enables detection of orphaned grouped message calls
 */
void
msg_gfinish PARM_0(void)
{
  /* check settings */
  if (gmsg_during == FALSE) return;

  /* done with messaging */
  gmsg_during = FALSE;
}

/*
 * Multi-line message system data structures and variables
 *
 * This system allows complex message composition with placeholder
 * support, enabling messages to be built incrementally with
 * deferred content insertion at marked positions.
 */

/* Storage structure for multi-line messages to the current country */
typedef struct s_cmsg {
  int line_info;	/* the status of this line (CMG_MARK or CMG_NORMAL) */
  char *data;		/* the text of the message (NULL for marked lines) */
  struct s_cmsg *next;	/* pointer to next line in message */
} CMSG_STRUCT, *CMSG_PTR;

/*
 * Static variables for multi-line message management
 * 
 * cmsg_mail   - Head pointer to linked list of message lines
 * last_cmsg   - Tail pointer for efficient list append operations
 * cmsg_header - Header text for the multi-line message
 * cmsg_target - Target nation ID for the message (-1 if unset)
 */
static CMSG_PTR cmsg_mail = NULL, last_cmsg = NULL;
static char cmsg_header[100];
static int cmsg_target = -1;

/*
 * Constants for multi-line message line status
 * 
 * CMG_MARK   - Line is a placeholder marker for deferred content
 * CMG_NORMAL - Line contains actual message text
 */
#define CMG_MARK	-1
#define CMG_NORMAL	0

/*
 * msg_cinit - Initialize multi-line message to current nation
 *
 * This function begins a new multi-line message composition session
 * targeted at the current nation. It initializes the multi-line
 * message system and sets up the header for the message.
 *
 * The multi-line message system allows complex message composition
 * with features like placeholder markers and deferred content
 * insertion, enabling sophisticated message formatting and
 * conditional content based on game state.
 *
 * Parameters:
 *   headstr - Header text for the multi-line message
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Clears any existing undelivered message (with warning)
 *   - Sets cmsg_header to the provided header string
 *   - Sets cmsg_target to current nation (global 'country' variable)
 *   - Prepares system for message line addition via msg_conquer()
 *
 * Notes:
 *   - Automatically sends any pending undelivered message before initialization
 *   - Target is always set to the current nation (global 'country')
 *   - Must be followed by msg_conquer() calls to add message content
 *   - Should be terminated with msg_cfinish() to deliver or discard message
 *   - Part of the advanced multi-line messaging system
 */
void
msg_cinit PARM_1(char *, headstr)
{
  /* check for completion */
  if (cmsg_mail != NULL) {
    errormsg("Warning: msg_cinit()...  sending undelivered mail");
    msg_cfinish(TRUE);
  }

  /* otherwise, initialize settings properly */
  strcpy(cmsg_header, headstr);
  cmsg_target = country;
}

/*
 * msg_cadjust - Modify target and header of current multi-line message
 *
 * This function allows modification of the target nation and header
 * of an active multi-line message session. It provides flexibility
 * to redirect messages or change headers after message composition
 * has begun.
 *
 * Parameters:
 *   ntarget - New target nation ID for the message
 *   nheader - New header text for the message
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Updates cmsg_target to new target nation
 *   - Updates cmsg_header to new header text
 *   - Initializes new message session if none exists
 *
 * Notes:
 *   - Automatically starts new session if none is active
 *   - Can be used to redirect messages to different nations
 *   - Changes apply to the entire message when delivered
 *   - Warning is logged if called without active session
 */
void
msg_cadjust PARM_2(int, ntarget, char *, nheader)
{
  if (cmsg_target == -1) {
    errormsg("Warning: msg_cadjust() ... starting up new msg");
    msg_cinit("This and that...");
  }
  cmsg_target = ntarget;
  strcpy(cmsg_header, nheader);
}

/*
 * msg_conquer - Add text line to pending multi-line message
 *
 * This function adds a line of text to the current multi-line message
 * being composed. It allocates memory for the line, stores the text,
 * and appends it to the linked list of message lines.
 *
 * The function performs validation to ensure message consistency and
 * automatically initializes a new message session if none exists.
 * It maintains the linked list structure for efficient message
 * composition and delivery.
 *
 * Parameters:
 *   str - Text string to add as a line in the message
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Allocates memory for new CMSG_STRUCT and text string
 *   - Appends new line to cmsg_mail linked list
 *   - Updates last_cmsg pointer for efficient list management
 *   - May call abrt() on memory allocation failure
 *
 * Notes:
 *   - Automatically starts new session if none is active
 *   - Validates that current country matches target nation
 *   - Memory allocation failure is treated as fatal error
 *   - Lines are added in chronological order (append only)
 *   - Part of the multi-line message composition system
 */
void
msg_conquer PARM_1(char *, str)
{
  CMSG_PTR tmp_cmsg;

  /* check the target */
  if (cmsg_target == -1) {
    errormsg("Warning: msg_conquer() ... starting up new msg");
    msg_cinit("This and that...");
  } else if (country != cmsg_target) {
    errormsg("Warning: msg_conquer() ... country not target");
  }

  /* position the new message in sequence */
  if ((tmp_cmsg = (CMSG_PTR) malloc(sizeof(CMSG_STRUCT))) == NULL) {
    errormsg("Memory allocation error");
    abrt();
  }
  tmp_cmsg->line_info = CMG_NORMAL;
  tmp_cmsg->next = NULL;

  /* store the new line of text */
  if ((tmp_cmsg->data = (char *) malloc(sizeof(char) * (strlen(str) + 1)))
      == NULL) {
    errormsg("Memory allocation error");
    abrt();
  }
  strcpy(tmp_cmsg->data, str);

  /* find the position of the new item */
  if (cmsg_mail == NULL) {
    cmsg_mail = tmp_cmsg;
  } else {
    last_cmsg->next = tmp_cmsg;
  }
  last_cmsg = tmp_cmsg;
}

/*
 * msg_cmark - Insert placeholder marker in multi-line message
 *
 * This function inserts a placeholder marker at the current position
 * in the multi-line message composition. The marker reserves a position
 * for content that will be added later using msg_creturn(), enabling
 * deferred content insertion and complex message formatting.
 *
 * This is particularly useful for messages where some content depends
 * on calculations or conditions that aren't known until later in the
 * message composition process.
 *
 * Parameters:
 *   void (no parameters)
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Allocates memory for new CMSG_STRUCT marker
 *   - Inserts marker with CMG_MARK status at current position
 *   - Updates last_cmsg pointer for list management
 *   - May call abrt() on memory allocation failure
 *
 * Notes:
 *   - Marker has NULL data pointer initially
 *   - Must be filled later with msg_creturn() or removed with msg_cunmark()
 *   - Automatically starts new session if none is active
 *   - Validates that current country matches target nation
 *   - Part of the advanced placeholder system for complex messages
 */
void
msg_cmark PARM_0(void)
{
  CMSG_PTR tmp_cmsg;

  /* check the target */
  if (cmsg_target == -1) {
    errormsg("Warning: msg_cmark() ... starting up new msg");
    msg_cinit("This and that...");
  } else if (country != cmsg_target) {
    errormsg("Warning: msg_cmark() ... country not target");
  }

  /* position the new message in sequence */
  if ((tmp_cmsg = (CMSG_PTR) malloc(sizeof(CMSG_STRUCT))) == NULL) {
    errormsg("Memory allocation error");
    abrt();
  }
  tmp_cmsg->line_info = CMG_MARK;
  tmp_cmsg->next = NULL;
  tmp_cmsg->data = NULL;

  /* find the position of the new item */
  if (cmsg_mail == NULL) {
    cmsg_mail = tmp_cmsg;
  } else {
    last_cmsg->next = tmp_cmsg;
  }
  last_cmsg = tmp_cmsg;
}

/*
 * msg_creturn - Fill first placeholder marker with text content
 *
 * This function finds the first placeholder marker (CMG_MARK) in the
 * multi-line message and fills it with the provided text content.
 * It converts the marker to a normal message line with actual content,
 * enabling deferred content insertion at previously marked positions.
 *
 * This is the counterpart to msg_cmark() and allows sophisticated
 * message composition where content order doesn't match the order
 * of content availability during composition.
 *
 * Parameters:
 *   str - Text content to insert at the first marked position
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Allocates memory for text string in marked position
 *   - Changes first marked line from CMG_MARK to CMG_NORMAL status
 *   - Fills data pointer with provided text content
 *   - May call abrt() on memory allocation failure
 *
 * Notes:
 *   - Searches from head of message list to find first marker
 *   - Warning is logged if no marked line is found
 *   - Memory allocation failure is treated as fatal error
 *   - Only fills the first marker found (FIFO order)
 *   - Part of the deferred content insertion system
 */
void
msg_creturn PARM_1(char *, str)
{
  CMSG_PTR tmp_cmsg;

  /* find the first marked line */
  for (tmp_cmsg = cmsg_mail;
       tmp_cmsg != NULL;
       tmp_cmsg = tmp_cmsg->next) {
    if (tmp_cmsg->line_info == CMG_MARK) break;
  }

  /* was there a success? */
  if (tmp_cmsg == NULL) {
    errormsg("Warning: msg_creturn() ... could not find marked line");
  } else {

    /* store the new line of text */
    if ((tmp_cmsg->data = (char *) malloc(sizeof(char) * (strlen(str) + 1)))
	== NULL) {
      errormsg("Memory allocation error");
      abrt();
    }
    strcpy(tmp_cmsg->data, str);

    /* clear out the mark */
    tmp_cmsg->line_info = CMG_NORMAL;

  }
}

/*
 * msg_cunmark - Remove first placeholder marker from message
 *
 * This function finds and removes the first placeholder marker
 * (CMG_MARK) from the multi-line message list. It provides a way
 * to cancel deferred content insertion by removing unused markers,
 * keeping the message structure clean.
 *
 * The function handles both head-of-list removal and mid-list
 * removal, maintaining proper linked list integrity and memory
 * management.
 *
 * Parameters:
 *   void (no parameters)
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Removes first marked line from cmsg_mail linked list
 *   - Frees memory allocated for the marked line structure
 *   - Updates list pointers to maintain list integrity
 *   - May update cmsg_mail head pointer if first line is removed
 *
 * Notes:
 *   - Searches from head of message list to find first marker
 *   - Handles special case when marked line is at head of list
 *   - Warning is logged if no marked line is found
 *   - Does not free data pointer (should be NULL for markers)
 *   - Part of the placeholder management system
 *   - Used to clean up unused markers before message delivery
 */
void
msg_cunmark PARM_0(void)
{
  CMSG_PTR tmp_cmsg, tmp2_cmsg;

  /* find the first marked line */
  tmp_cmsg = cmsg_mail;
  if ((cmsg_mail != NULL) &&
      (cmsg_mail->line_info == CMG_MARK)) {

    /* eliminate the first line of the mail message */
    cmsg_mail = cmsg_mail->next;
    free(tmp_cmsg);

  } else {

    /* find the mark */
    for (; tmp_cmsg->next != NULL;
	 tmp_cmsg = tmp_cmsg->next) {
      if ((tmp_cmsg->next)->line_info == CMG_MARK) break;
    }

    /* was there a success? */
    if ((tmp_cmsg == NULL) ||
	(tmp_cmsg->next == NULL)) {
      /* no go */
      errormsg("Warning: msg_cunmark() ... could not find marked line");
    } else {
      /* elminate the line from the message */
      tmp2_cmsg = tmp_cmsg->next;
      tmp_cmsg->next = tmp2_cmsg->next;
      tmp2_cmsg->next = NULL;
      free(tmp2_cmsg);
    }

  }
}

/*
 * msg_csend - Deliver pending multi-line message to target nation
 *
 * This function delivers the composed multi-line message to the
 * target nation's mail file. It handles file creation, header
 * insertion, and content output while performing validation to
 * ensure message integrity and proper delivery.
 *
 * The function processes the entire linked list of message lines,
 * writing each line to the target nation's mail file while checking
 * for and warning about any remaining placeholder markers.
 *
 * Parameters:
 *   void (no parameters)
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Creates or appends to target nation's mail file
 *   - Writes message header and all content lines to file
 *   - Closes mail file and resets mailfp to NULL
 *   - Validates message state and logs warnings for problems
 *
 * Notes:
 *   - Returns silently if no message content exists
 *   - Validates that target matches current country
 *   - Warns if grouped message session is still active
 *   - Skips inactive or NULL nations automatically
 *   - Warns about any remaining unmarked placeholder lines
 *   - Mail files are named "{nation_name}.{msgtag}"
 *   - Deity messages go to "god.{msgtag}" file
 *   - Does not free message memory (use msg_cfinish for cleanup)
 */
void
msg_csend PARM_0(void)
{
  NTN_PTR n1_ptr;
  CMSG_PTR travel_cmsg;
  char tname[NAMELTH + 1];
  char fname[FILELTH + 1];

  /* check the length */
  if (cmsg_mail == NULL) {
    /* no message... quiet return */
    return;
  }

  /* check the recipient */
  if (cmsg_target != country) {
    errormsg("Warning: msg_csend() ... target ntn not current");
  }

  /* check if a grouped message is being sent */
  if (gmsg_during == TRUE) {
    errormsg("Warning: msg_csend() ... grouped msg still active");
  }

  /* check if it is an npc */
  if (cmsg_target == UNOWNED) {
    strcpy(tname, "god");
  } else {
    if (((n1_ptr = world.np[cmsg_target]) == NULL) ||
	!n_ispc(ntn_ptr->active)) {
      return;
    } else {
      strcpy(tname, n1_ptr->name);
    }
  }

  /* open the output file */
  sprintf(fname, "%s.%s", tname, msgtag);
  if ((mailfp = fopen(fname, "a")) == NULL) {
    sprintf(string, "Could not open mail file %s\n", fname);
    errormsg(string);
    return;
  }

  /* send the header */
  mail_header(NULL, NULL, cmsg_header, NULL, 0);

  /* now send all of the mail */
  for (travel_cmsg = cmsg_mail;
       travel_cmsg != NULL;
       travel_cmsg = travel_cmsg->next) {
    if (travel_cmsg->line_info == CMG_MARK) {
      errormsg("Warning: msg_csend() ... marked line encountered");
    } else if (travel_cmsg->data != NULL) {
      fprintf(mailfp, "%s", travel_cmsg->data);
    }
  }
  putc('\n', mailfp);

  /* all done */
  fclose(mailfp);
  mailfp = NULL;
}

/*
 * msg_cfinish - Complete multi-line message session with optional delivery
 *
 * This function finalizes a multi-line message session by optionally
 * delivering the message and then cleaning up all allocated memory.
 * It provides a clean way to end message composition with full
 * memory management and session reset.
 *
 * The function handles both delivery and discard scenarios, allowing
 * messages to be composed and then cancelled if needed while ensuring
 * proper memory cleanup in all cases.
 *
 * Parameters:
 *   sendit - Boolean flag: TRUE to deliver message, FALSE to discard
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Optionally delivers message via msg_csend() if sendit is TRUE
 *   - Frees all allocated memory for message lines and text data
 *   - Resets cmsg_mail to NULL and cmsg_target to -1
 *   - Clears all message composition state
 *
 * Notes:
 *   - Must be called to properly clean up multi-line message sessions
 *   - Safe to call even with empty or partially composed messages
 *   - Frees both structure memory and text data memory
 *   - Resets session to allow new message composition
 *   - Essential for preventing memory leaks in message system
 *   - Can be used to discard messages by passing FALSE for sendit
 */
void
msg_cfinish PARM_1(int, sendit)
{
  CMSG_PTR cmsg_next;

  /* first deliver it */
  if (sendit) msg_csend();

  /* now free everything up */
  for ( ; cmsg_mail != NULL; cmsg_mail = cmsg_next) {
    cmsg_next = cmsg_mail->next;
    free(cmsg_mail->data);
    free(cmsg_mail);
  }
  cmsg_target = -1;
}

/*
 * News system data structures and variables
 *
 * This system manages sorted news items for game events, organizing
 * them by country for coherent news delivery and reporting.
 */

/* Structure for holding sorted news items */
typedef struct s_snews {
  int country;			/* the country who triggered the news event */
  char newsitem[LINELTH+1];	/* the actual news text about the event */
  struct s_snews *next;		/* pointer to next news item in sorted list */
} SNEWS_STRUCT, *SNEWS_PTR;

/*
 * Static variable for sorted news list management
 * 
 * snews_list - Head pointer to sorted linked list of news items
 *              organized by country ID for coherent news delivery
 */
static SNEWS_PTR snews_list = NULL;

/*
 * sorted_news - Add news item to sorted news list
 *
 * This function adds a news item to the sorted news list, maintaining
 * the list in ascending order by country ID. This organization ensures
 * that news items are grouped by country when delivered, providing
 * coherent news reports for game events.
 *
 * The function performs insertion sort to maintain list order,
 * handling head insertion, tail insertion, and mid-list insertion
 * as needed to preserve the sorted sequence.
 *
 * Parameters:
 *   cntry - Country ID that triggered the news event
 *   line  - News text describing the event
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Allocates memory for new SNEWS_STRUCT
 *   - Inserts new item in sorted position in snews_list
 *   - May call abrt() on memory allocation failure
 *
 * Notes:
 *   - Returns silently if line parameter is NULL
 *   - Memory allocation failure is treated as fatal error
 *   - Maintains ascending sort order by country ID
 *   - Uses insertion sort algorithm for list management
 *   - Part of the game news and event reporting system
 *   - News items are delivered later via send_sortednews()
 */
void
sorted_news PARM_2(int, cntry, char *, line)
{
  SNEWS_PTR snews_tmp, snews_travel;

  /* check the input */
  if (line == NULL) return;

  /* allocate the storage and copy in the data */
  if ((snews_tmp = (SNEWS_PTR) malloc(sizeof(SNEWS_STRUCT))) == NULL) {
    errormsg("SERIOUS ERROR: memory allocation failure in sorted_news()");
    abrt();
  }
  snews_tmp->country = cntry;
  strcpy(snews_tmp->newsitem, line);

  /* find the position */
  if (snews_list == NULL) {

    /* begin the list */
    snews_list = snews_tmp;
    snews_tmp->next = NULL;

  } else if (snews_tmp->country < snews_list->country) {

    /* position it at the beginning of the list */
    snews_tmp->next = snews_list;
    snews_list = snews_tmp;

  } else {

    /* insert it in the middle of the list */
    for (snews_travel = snews_list;
	 snews_travel->next != NULL;
	 snews_travel = snews_travel->next) {
      if (snews_tmp->country < (snews_travel->next)->country) {
	break;
      }
    }
    snews_tmp->next = snews_travel->next;
    snews_travel->next = snews_tmp;

  }
}

/*
 * send_sortednews - Deliver all accumulated sorted news items
 *
 * This function outputs all accumulated news items from the sorted
 * news list to the news file, delivering them in country-sorted
 * order for coherent news reporting. It processes the entire list
 * and cleans up all allocated memory.
 *
 * The function is typically called at the end of game updates to
 * deliver all news events that occurred during the update cycle,
 * ensuring that news items are presented in an organized fashion
 * grouped by the countries that triggered the events.
 *
 * Parameters:
 *   void (no parameters)
 *
 * Returns:
 *   void (no return value)
 *
 * Side Effects:
 *   - Writes all news items to global fnews file pointer
 *   - Frees all memory allocated for news items and structures
 *   - Resets snews_list to NULL (empty list)
 *   - Processes items in country-sorted order
 *
 * Notes:
 *   - Safe to call even with empty news list
 *   - News items are written directly to fnews without formatting
 *   - Memory cleanup is performed for each item after output
 *   - List is completely cleared after processing
 *   - Part of the game news and event reporting system
 *   - Called during update finalization to deliver accumulated news
 */
void
send_sortednews PARM_0(void)
{
  SNEWS_PTR snews_next;
  
  /* just keep going until done */
  for (; snews_list != NULL; snews_list = snews_next) {

    /* keep on track */
    snews_next = snews_list->next;

    /* send out the item */
    fprintf(fnews, snews_list->newsitem);

    /* clean up */
    snews_list->next = NULL;
    free(snews_list);

  }
}
