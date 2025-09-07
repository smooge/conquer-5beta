/* routines to read and manipulate conquer mail messages */
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
#include "rmailX.h"
#include "keyvalsX.h"

/* declaration of keybindings at bottom */
extern KBIND_STRUCT reader_klist[];
extern PARSE_STRUCT reader_funcs[];
KLIST_PTR reader_bindings = NULL;

/* information structure concerning movement keybindings */
KEYSYS_STRUCT reader_keysys = {
  "reader", reader_funcs, reader_klist, 0, 0
};

/* pointer to the list of all of the mail */
static RMAIL_PTR mbox_list = NULL;

/* list of other useful data settings */
static int ml_done, msg_count = 0, hdr_start = 1, cur_count;
static int line_pos, header_mode = FALSE;
static RMAIL_PTR read_msg = NULL, read_hold = NULL, last_msg = NULL;
static MAILD_PTR read_line = NULL, travel_line;
static char ml_error[LINELTH];
extern int email_subjlim;
extern int email_collim;

/*
 * ml_first - Find the first unread mail message in mailbox
 *
 * Searches through the mail message list to locate the first unread message.
 * If no unread messages are found, defaults to the first message in the list.
 * Sets global pointers to position the mail reader at the appropriate message.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Sets read_msg to first unread message (or first message if none unread)
 *   - Sets cur_count to position number of selected message
 *   - Sets read_line to beginning of selected message text
 *   - Modifies global mail reader state
 *
 * Notes:
 *   - Used during mail reader initialization to position at first unread message
 *   - Provides user-friendly behavior by showing unread content first
 *   - Falls back gracefully to first message when all messages are read
 */
/* ML_FIRST -- Find the first unread mail message */
static int
ml_first PARM_0(void)
{
  /* search through the list of mail messages */
  cur_count = 1;
  for (read_msg = mbox_list;
       read_msg != NULL;
       read_msg = read_msg->next) {
    /* stop on any unread mail message */
    if (!(read_msg->status & MSTAT_READ)) break;
    cur_count++;
  }

  /* check if there were none */
  if (read_msg == NULL) {
    read_msg = mbox_list;
    cur_count = 1;
  }

  /* set the start of the message */
  if (read_msg != NULL) {
    read_line = read_msg->text;
  } else {
    read_line = NULL;
  }
  return(0);
}

/*
 * ml_countem - Find current mail message position and count totals
 *
 * Traverses the mail message list to determine the total number of messages
 * and locate the position of the currently selected message. Updates global
 * counters used for display and navigation throughout the mail reader interface.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Sets cur_count to position number of current message (read_msg)
 *   - Sets msg_count to total number of messages in mailbox
 *   - Modifies global mail reader navigation state
 *
 * Notes:
 *   - Essential for maintaining accurate position information in mail reader
 *   - Used after message deletions or list modifications to update counters
 *   - Provides data for "Message X of Y" display information
 *   - Linear search algorithm with O(n) complexity for message counting
 */
/* ML_COUNTEM -- Find the current pointer position and totals */
static int
ml_countem PARM_0(void)
{
  RMAIL_PTR rmail_tmp;

  /* loop and count */
  cur_count = 1;
  msg_count = 0;
  for (rmail_tmp = mbox_list;
       rmail_tmp != NULL;
       rmail_tmp = rmail_tmp->next) {
    /* locate the current message */
    msg_count++;
    if (rmail_tmp == read_msg) {
      cur_count = msg_count;
    }
  }
  return(0);
}

/*
 * ml_help - Display mail reader command help documentation
 *
 * Creates and displays a comprehensive help screen showing all available
 * mail reader commands and their key bindings. Uses the standard help
 * system to present organized command reference information to the user.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Displays help screen overlay over current mail reader interface
 *   - Temporarily suspends mail reader operation until help dismissed
 *   - Uses global reader_bindings and reader_funcs for help content
 *
 * Notes:
 *   - Accessible via '?' key binding in mail reader interface
 *   - Help content dynamically generated from current key binding configuration
 *   - Provides essential user assistance for mail reader navigation
 *   - Returns control to mail reader after help screen dismissed
 */
/* ML_HELP -- Show some documentation */
static int
ml_help PARM_0(void)
{
  /* create the help system */
  create_help("Conquer Mail Reader Command List",
	      reader_bindings, reader_funcs, reader_keysys.num_parse);
  return(0);
}

/*
 * rmail_header - Parse and load mail message header from file
 *
 * Reads and parses a mail message header from the input file stream, extracting
 * sender information, nickname, status, date, and subject fields. Creates and
 * populates a new mail message structure with the parsed header data. Uses
 * structured format validation to ensure proper header parsing.
 *
 * Parameters:
 *   infp - File pointer to read header data from (must not be NULL)
 *
 * Returns:
 *   TRUE - Header successfully parsed and loaded
 *   FALSE - Header parsing failed due to format error or memory allocation failure
 *
 * Side Effects:
 *   - Allocates memory for new mail message structure (cur_message)
 *   - Allocates and copies sender, nickname, date, and subject strings
 *   - Calls init_mail() to initialize mail message structure
 *   - Sets cur_message->to_whom[0] to current country on success
 *   - Calls free_mail() and aborts on memory allocation failure
 *
 * Notes:
 *   - Expected header format: sender:, nickname:, status:, date:, subject:
 *   - Uses goto for centralized error handling on malformed headers
 *   - Critical function for mail file parsing during mailbox loading
 *   - Memory allocated here must be freed by caller using kill_mail()
 */
/* RMAIL_HEADER -- Retrieve the header from the file */
static int
rmail_header PARM_1(FILE *, infp)
{
  char buf[BIGLTH];
  int lstr = 0;

  /* check the file pointer */
  if (infp == NULL) return(FALSE);

  /* obtain a new current message */
  if ((cur_message = (RMAIL_PTR) malloc(sizeof(RMAIL_STRUCT))) == NULL) {
    errormsg("Memory Allocation Failure");
    abrt();
  }

  /* clean up the storage and "send" it to us */
  init_mail();
  cur_message->to_whom[0] = country;

  /* obtain who it is from */
  if ((fgets(buf, BIGLTH - 1, infp) == NULL) ||
      ((lstr = strlen(buf)) < 2) ||
      (buf[lstr - 2] != ':')) {
  exit_on_misformat:
    free_mail();
    return(FALSE);
  }
  buf[lstr - 2] = '\0';
  if ((cur_message->sender = (char *) malloc(sizeof(char) * lstr)) == NULL) {
    errormsg("Memory allocation failure");
    abrt();
  }
  strcpy(cur_message->sender, buf);

  /* find the nickname */
  if ((fgets(buf, BIGLTH - 1, infp) == NULL) ||
      ((lstr = strlen(buf)) < 2) ||
      (buf[lstr - 2] != ':')) {
    goto exit_on_misformat;
  }
  buf[lstr - 2] = '\0';
  if ((cur_message->nickname =
       (char *) malloc(sizeof(char) * lstr)) == NULL) {
    errormsg("Memory allocation failure");
    abrt();
  }
  strcpy(cur_message->nickname, buf);

  /* get the message status */
  if ((fgets(buf, BIGLTH - 1, infp) == NULL) ||
      ((lstr = strlen(buf)) < 2) ||
      (buf[lstr - 2] != ':')) {
    goto exit_on_misformat;
  }
  buf[lstr - 2] = '\0';
  if (sscanf(buf, "%d", &cur_message->status) != 1) {
    goto exit_on_misformat;
  }

  /* when? */
  if ((fgets(buf, BIGLTH - 1, infp) == NULL) ||
      ((lstr = strlen(buf)) < 2) ||
      (buf[lstr - 2] != ':')) {
    goto exit_on_misformat;
  }
  buf[lstr - 2] = '\0';
  if ((cur_message->date = (char *) malloc(sizeof(char) * lstr)) == NULL) {
    errormsg("Memory allocation failure");
    abrt();
  }
  strcpy(cur_message->date, buf);

  /* what is it about? */
  if ((fgets(buf, BIGLTH - 1, infp) == NULL) ||
      ((lstr = strlen(buf)) < 2) ||
      (buf[lstr - 2] != ':')) {
    goto exit_on_misformat;
  }
  buf[lstr - 2] = '\0';
  if ((cur_message->subj = (char *) malloc(sizeof(char) * lstr)) == NULL) {
    errormsg("Memory allocation failure");
    abrt();
  }
  strcpy(cur_message->subj, buf);

  return(TRUE);
}

/*
 * open_mailbox - Load all mail messages from mailbox file into memory
 *
 * Opens and reads the entire contents of the user's mailbox file, parsing
 * individual mail messages and loading them into a linked list structure.
 * Handles both message headers and message body text, building complete
 * mail message structures for the mail reader interface.
 *
 * Parameters:
 *   void - No parameters required (uses global nationname and msgtag)
 *
 * Returns:
 *   mcount - Number of mail messages successfully loaded from file
 *   0 - No messages loaded (file doesn't exist or empty)
 *
 * Side Effects:
 *   - Creates linked list of mail messages (mbox_list)
 *   - Allocates memory for all mail message structures and text lines
 *   - Calls rmail_header() to parse message headers
 *   - Opens and closes mailbox file
 *   - Terminates program on memory allocation failure
 *
 * Notes:
 *   - Mailbox filename format: "nationname.msgtag"
 *   - Messages delimited by ":BEGIN:" markers in file
 *   - Each message contains header followed by body text lines
 *   - Memory allocated here freed by rmail_close() after mail reader exits
 *   - Critical function for mail reader initialization
 */
/* OPEN_MAILBOX -- Read in all of the messages inside the mail box */
static int
open_mailbox PARM_0(void)
{
  RMAIL_PTR rmail_ptr = NULL;
  MAILD_PTR maild_ptr = NULL, nextmail_ptr = NULL;
  char line[BIGLTH];
  int mcount = 0, mlen = 0;
  FILE *fp;

  /* open and read in all of the mail */
  sprintf(line, "%s.%s", nationname, msgtag);
  if ((fp = fopen(line, "r")) == NULL) {
    return(mcount);
  }

  /* now read them all in */
  while (!feof(fp)) {

    /* get the input */
    if (fgets(line, BIGLTH - 1, fp) == NULL) break;

    /* remove the nl and find length */
    mlen = strlen(line);
    line[mlen - 1] = '\0';

    /* check for a new header */
    if (strcmp(line, ":BEGIN:") == 0) {

      /* retreive the valid header */
      if (rmail_header(fp) != FALSE) {
	if (mbox_list == NULL) {
	  mbox_list = cur_message;
	  rmail_ptr = mbox_list;
	} else {
	  rmail_ptr->next = cur_message;
	  cur_message->prev = rmail_ptr;
	  rmail_ptr = cur_message;
	}
	maild_ptr = NULL;
	mcount++;
      }

    } else if (rmail_ptr != NULL) {

      /* find the space for it */
      if ((maild_ptr = (MAILD_PTR) malloc(sizeof(MAILD_STRUCT))) == NULL) {
	errormsg("SERIOUS ERROR: Memory allocation failure");
	abrt();
      }
      if ((maild_ptr->line_data =
	   (char *) malloc(sizeof(char) * mlen)) == NULL) {
	errormsg("SERIOUS ERROR: Memory allocation failure");
	abrt();
      }
      strcpy(maild_ptr->line_data, line);
      maild_ptr->next = NULL;
      maild_ptr->prev = NULL;

      /* add it to the message */
      if (rmail_ptr->text == NULL) {
	nextmail_ptr = rmail_ptr->text = maild_ptr;
      } else {
	nextmail_ptr->next = maild_ptr;
	maild_ptr->prev = nextmail_ptr;
	nextmail_ptr = maild_ptr;
      }

    }

  }

  /* finito */
  fclose(fp);
  return(mcount);
}

/* lock file storage */
static char maillock[FILELTH];
static int lockid;

/*
 * rmail_close - Save mail messages back to file and release lock
 *
 * Finalizes the mail reading session by storing all mail messages back to
 * the mailbox file and releasing the file lock. Processes all messages in
 * the mail list, delivering them via the mail system and freeing associated
 * memory structures.
 *
 * Parameters:
 *   void - No parameters required (uses global mbox_list and lock variables)
 *
 * Returns:
 *   void - No return value
 *
 * Side Effects:
 *   - Removes original mailbox file (unlink)
 *   - Calls deliver_mail() for each message to save back to file
 *   - Calls kill_mail() to free memory for each message
 *   - Calls kill_lock() to release file lock (lockid, maillock)
 *   - Completely deallocates mbox_list and associated structures
 *
 * Notes:
 *   - Critical cleanup function called when exiting mail reader
 *   - Ensures all changes (deletions, status updates) are persisted
 *   - Removes file lock to allow other processes to access mailbox
 *   - Memory cleanup prevents leaks after mail reader session
 *   - Must be called to properly close mail reading session
 */
/* RMAIL_CLOSE -- Store the mail messages and remove the lock */
void
rmail_close PARM_0(void)
{
  /* remove the original file */
  sprintf(string, "%s.%s", nationname, msgtag);
  unlink(string);

  /* now store all of the mail messages and free storage */
  for (cur_message = mbox_list;
       cur_message != NULL;
       cur_message = mbox_list) {

    /* keep it */
    mbox_list = cur_message->next;
    deliver_mail();
    kill_mail();

  }

  /* take care of the lock file */
  kill_lock(lockid, maillock);
}

/*
 * format_str - Generate formatted string from mail message data
 *
 * Processes a format string containing placeholder tokens and substitutes
 * them with actual data from the specified mail message. Supports multiple
 * format tokens for date, sender name, nickname, and other message fields.
 * Used for creating customizable mail message headers and quotes.
 *
 * Parameters:
 *   rmail_ptr - Pointer to mail message structure containing data (can be NULL)
 *   format - Format string containing $-prefixed tokens for substitution
 *   outstr - Output buffer to store the formatted result string
 *
 * Returns:
 *   void - No return value (result stored in outstr)
 *
 * Side Effects:
 *   - Modifies outstr buffer with formatted output
 *   - Null-terminates the output string
 *
 * Notes:
 *   - Format tokens: $d (date), $S (nickname), $s (sender), others copied literally
 *   - If rmail_ptr is NULL, outputs empty string
 *   - Used for reply headers, forward headers, and quote prefixes
 *   - Format string parsing handles escape sequences and unknown tokens
 *   - Output buffer must be large enough to contain formatted result
 */
/* FORMAT_STR -- Generate a string from the supplied format */
static void
format_str PARM_3 (RMAIL_PTR, rmail_ptr, char *, format, char *, outstr)
{
  int i, j = 0;
  char *ch_ptr;

  if (rmail_ptr == NULL) {
    outstr[j] = '\0';
  } else {
    for (i = 0; format[i] != '\0'; i++) {

      /* copy it */
      if (format[i] == '$') {
	switch (format[++i]) {
	case '\0':
	  /* must end here */
	  i--;
	  break;
	case 'd':
	  /* the date of the message */
	  for (ch_ptr = rmail_ptr->date;
	       *ch_ptr != '\0';
	       ch_ptr++) {
	    outstr[j++] = *ch_ptr;
	  }
	  break;
	case 'S':
	  /* copy in the nickname of the original sender */
	  for (ch_ptr = rmail_ptr->nickname;
	       *ch_ptr != '\0';
	       ch_ptr++) {
	    outstr[j++] = *ch_ptr;
	  }
	  break;
	case 's':
	  /* copy in the name of the original sender */
	  for (ch_ptr = rmail_ptr->sender;
	       *ch_ptr != '\0';
	       ch_ptr++) {
	    outstr[j++] = *ch_ptr;
	  }
	  break;
	default:
	  /* just copy it; no error */
	  outstr[j++] = format[i];
	  break;
	}
      } else {
	outstr[j++] = format[i];
      }

    }
    outstr[j] = '\0';
  }
}

/*
 * rmail_mstart - Prepare new mail message for composition or reply
 *
 * Creates and initializes a new mail message structure for composition,
 * handling reply setup, message forwarding, and quote generation. Configures
 * recipient, subject line, and quoted text based on the operation type and
 * source message. Calls the mail editor to allow user to compose the message.
 *
 * Parameters:
 *   isreply - TRUE if replying to message, FALSE if forwarding or new message
 *   quotemsg - TRUE to include quoted text, FALSE for no quotes, -1 for forward
 *   msg_ptr - Source message for reply/forward operations (NULL for new message)
 *
 * Returns:
 *   void - No return value
 *
 * Side Effects:
 *   - Allocates memory for new mail message structure (cur_message)
 *   - Calls init_mail() to initialize message structure
 *   - Sets email_collim based on nation name length
 *   - Creates subject line with "Re:" or "FWD:" prefix as appropriate
 *   - Generates quoted text with formatting when requested
 *   - Calls email_parse() to launch mail composition interface
 *   - May call free_mail() and return early on validation failures
 *
 * Notes:
 *   - Complex function handling multiple mail composition scenarios
 *   - Quote format controlled by rmail_from_format and rmail_quote_prefix
 *   - Validates sender exists for reply operations
 *   - Memory allocation failures terminate program
 *   - Quote prefix applied to each line of original message text
 */
/* RMAIL_MSTART -- Prepare a mail message for editing */
static void
rmail_mstart PARM_3(int, isreply, int, quotemsg, RMAIL_PTR, msg_ptr )
{
  MAILD_PTR in_line, out_line = NULL;
  char buf[LINELTH];

  /* now build the new mail element */
  if ((cur_message = (RMAIL_PTR) malloc(sizeof(RMAIL_STRUCT))) == NULL) {
    /* uh oh */
    errormsg("Major Error:  Memory allocation error");
    abrt();
  }

  /* now begin it */
  init_mail();
  email_collim = 80 - (strlen(nationname) + 4);

  /* how much must be done? */
  if (msg_ptr != NULL) {

    /* setup the subject line */
    if (msg_ptr->subj != NULL) {
      if (isreply == TRUE) {

	/* replying to the sender */
	if (ntnbyname(msg_ptr->sender) == NULL) {
	  if (global_int != UNOWNED) {
	    errormsg("There was no return address in that message.");
	    free_mail();
	    return;
	  }
	}
	cur_message->to_whom[0] = global_int;
	if (strncmp(msg_ptr->subj, "Re:", 3) == 0) {
	  strcpy(string, msg_ptr->subj);
	} else {
	  sprintf(string, "Re: %s", msg_ptr->subj);
	}

      } else {
	sprintf(string, "FWD: %s", msg_ptr->subj);
      }

      /* now finish it */
      if ((cur_message->subj =
	   (char *) malloc((email_subjlim + 1) * sizeof(char))) == NULL) {
	/* uh oh */
	errormsg("Major Error:  Memory allocation error");
	abrt();
      }
      strncpy(cur_message->subj, string, email_subjlim);
      cur_message->subj[email_subjlim] = '\0';
    }

    /* find the quoting prefix */
    if ((quotemsg != FALSE) &&
	(msg_ptr->text != NULL)) {

      /* build the starting line */
      if (rmail_from_format[0] != '\0') {

	/* construct it from the format */
	format_str(msg_ptr, &(rmail_from_format[0]), buf);

	/* now allocate the line space */
	if ((cur_message->text =
	     (MAILD_PTR) malloc(sizeof(MAILD_STRUCT))) == NULL) {
	  /* uh oh */
	  errormsg("Major Error:  Memory allocation error");
	  abrt();
	}
	out_line = cur_message->text;
	out_line->next = NULL;
	out_line->prev = NULL;

	/* add in the string */
	if ((out_line->line_data =
	     (char *) malloc((email_collim + 1) * sizeof(char))) == NULL) {
	  /* uh oh */
	  errormsg("Major Error:  Memory allocation error");
	  abrt();
	}
	strncpy(out_line->line_data, buf, email_collim);
	out_line->line_data[email_collim] = '\0';

	/* add in "blank line" */
	if (quotemsg == TRUE) {
	  format_str(msg_ptr, &(rmail_quote_prefix[0]), buf);
	} else {
	  /* no prefix */
	  buf[0] = '\0';
	}
	if ((out_line->next =
	     (MAILD_PTR) malloc(sizeof(MAILD_STRUCT))) == NULL) {
	  /* uh oh */
	  errormsg("Major Error:  Memory allocation error");
	  abrt();
	}
	out_line->next->prev = out_line;
	out_line = out_line->next;
	out_line->next = NULL;
	if ((out_line->line_data =
	     (char *) malloc((email_collim + 1) * sizeof(char))) == NULL) {
	  /* uh oh */
	  errormsg("Major Error:  Memory allocation error");
	  abrt();
	}
	strncpy(out_line->line_data, buf, email_collim);
	out_line->line_data[email_collim] = '\0';
      }
      

      /* parse the quote prefix format */
      if (quotemsg == TRUE) {
	format_str(msg_ptr, &(rmail_quote_prefix[0]), buf);
      } else {
	/* no prefix */
	buf[0] = '\0';
      }

      /* now add in the entire text of the message */
      for (in_line = msg_ptr->text;
	   in_line != NULL;
	   in_line = in_line->next) {

	/* is it the beginning of the message? */
	if (out_line == NULL) {

	  /* allocate the initial line space */
	  if ((cur_message->text =
	       (MAILD_PTR) malloc(sizeof(MAILD_STRUCT))) == NULL) {
	    /* uh oh */
	    errormsg("Major Error:  Memory allocation error");
	    abrt();
	  }
	  out_line = cur_message->text;
	  out_line->next = NULL;
	  out_line->prev = NULL;

	} else {

	  /* allocate the next line space */
	  if ((out_line->next =
	       (MAILD_PTR) malloc(sizeof(MAILD_STRUCT))) == NULL) {
	    /* uh oh */
	    errormsg("Major Error:  Memory allocation error");
	    abrt();
	  }
	  (out_line->next)->prev = out_line;
	  out_line = out_line->next;
	  out_line->next = NULL;

	}

	/* now add in the line */
	strcpy(string, buf);
	strcat(string, in_line->line_data);

	/* tag it in */
	if ((out_line->line_data =
	     (char *) malloc((email_collim + 1) * sizeof(char))) == NULL) {
	  /* uh oh */
	  errormsg("Major Error:  Memory allocation error");
	  abrt();
	}
	strncpy(out_line->line_data, string, email_collim);
	out_line->line_data[email_collim] = '\0';
      }

    }
  }

  /* now call the mail editor and let it handle everything else */
  email_parse();
}

/*
 * do_email - Launch mail composition interface for new message
 *
 * Entry point function for composing and sending a new mail message.
 * Calls the mail preparation function with parameters set for creating
 * a fresh message without reply or quote functionality. Provides simple
 * interface for normal mail composition from game interface.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls rmail_mstart() to initialize mail composition
 *   - Launches mail editor interface for user input
 *   - May allocate memory for mail message structures
 *
 * Notes:
 *   - Public interface function for new mail composition
 *   - Used by game interface to allow players to send mail
 *   - Wrapper function around rmail_mstart() with new message parameters
 *   - Returns to caller after mail composition completed or cancelled
 */
/* DO_EMAIL -- Send a normal mail message */
int
do_email PARM_0(void)
{
  /* call the prefix routine */
  rmail_mstart(FALSE, FALSE, NULL);
  return(0);
}

/*
 * ml_center_hstart - Center header display around current message
 *
 * Calculates the starting position for header display to center the current
 * message within the visible header list. Adjusts the header starting position
 * to provide optimal viewing of the current message context within the
 * available screen space.
 *
 * Parameters:
 *   void - No parameters required (uses global cur_count and LINES)
 *
 * Returns:
 *   void - No return value
 *
 * Side Effects:
 *   - Modifies hdr_start global variable to set header display starting position
 *   - Ensures hdr_start is never less than 1
 *
 * Notes:
 *   - Used in header display mode to position current message in center
 *   - Calculation based on available screen lines (LINES - 8) / 2
 *   - Provides better user experience by showing context around current message
 *   - Called when jumping to specific messages via ml_goto()
 */
/* ML_CENTER_HSTART -- Align the header starting position to center current */
static void
ml_center_hstart PARM_0(void)
{
  hdr_start = cur_count - (LINES - 8) / 2;
  if (hdr_start < 1) hdr_start = 1;
}

/*
 * ml_realign - Realign message display to keep content within screen bounds
 *
 * Adjusts the current message line position to ensure the displayed content
 * fits within the available screen space. Moves the reading position backward
 * if necessary to maintain proper display alignment within the message viewer.
 *
 * Parameters:
 *   void - No parameters required (uses global read_line and LINES)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - May modify read_line position to maintain display alignment
 *   - Ensures message text fits within available screen lines
 *
 * Notes:
 *   - Called after movement operations to maintain proper display
 *   - Prevents display overflow by adjusting reading position
 *   - Uses LINES - 11 to calculate available display space
 *   - Essential for maintaining consistent message viewing experience
 */
/* ML_REALIGN -- Realign the bottom portion of the mail message */
static int
ml_realign PARM_0(void)
{
  if (read_line != NULL) {
    travel_line = read_line->next;
    for (line_pos = LINES - 11;
	 line_pos > 0;
	 line_pos--) {
      if (travel_line == NULL) break;
      travel_line = travel_line->next;
    }

    /* now traverse backward the proper amount */
    for (; line_pos > 0; line_pos--) {
      if (read_line->prev == NULL) break;
      read_line = read_line->prev;
    }
  }
  return(0);
}

/*
 * ml_downone - Move message display down one line
 *
 * Advances the message reading position down by one line, allowing the user
 * to scroll through message content line by line. Automatically realigns
 * the display to maintain proper screen positioning after the movement.
 *
 * Parameters:
 *   void - No parameters required (uses global read_line)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Advances read_line to next line if available
 *   - Calls ml_realign() to maintain display alignment
 *
 * Notes:
 *   - Provides fine-grained message navigation control
 *   - Does not move if already at last line of message
 *   - Bound to Enter/Return keys in default key bindings
 *   - Part of core message navigation interface
 */
/* ML_DOWNONE -- Move down one line */
static int
ml_downone PARM_0(void)
{
  /* go down a single line */
  if ((read_line != NULL) &&
      (read_line->next != NULL)) {
    read_line = read_line->next;
  }

  /* now realign */
  ml_realign();
  return(0);
}

/*
 * ml_bottom - Jump to bottom of current message
 *
 * Moves the message reading position to the last line of the current message,
 * allowing users to quickly navigate to the end of long messages. Automatically
 * realigns the display to show the bottom portion of the message content.
 *
 * Parameters:
 *   void - No parameters required (uses global read_line)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Sets read_line to last line of current message
 *   - Calls ml_realign() to position display appropriately
 *
 * Notes:
 *   - Provides quick navigation to message end
 *   - Useful for long messages where scrolling would be tedious
 *   - Bound to '>' key in default key bindings
 *   - Counterpart to ml_top() function
 */
/* ML_BOTTOM -- Move down to the bottom of the message */
static int
ml_bottom PARM_0(void)
{
  /* go down all the way */
  while ((read_line != NULL) &&
	 (read_line->next != NULL)) {
    read_line = read_line->next;
  }

  /* now realign */
  ml_realign();
  return(0);
}

/*
 * ml_top - Jump to top of current message
 *
 * Moves the message reading position to the first line of the current message,
 * allowing users to quickly return to the beginning of the message content.
 * Provides immediate navigation to message start without scrolling.
 *
 * Parameters:
 *   void - No parameters required (uses global read_msg)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Sets read_line to first line of current message text
 *
 * Notes:
 *   - Provides quick navigation to message beginning
 *   - Essential for reviewing message content from start
 *   - Bound to '<' key in default key bindings
 *   - Counterpart to ml_bottom() function
 *   - No realignment needed since positioned at top
 */
/* ML_TOP -- Move to the very top of the current mail message */
static int
ml_top PARM_0(void)
{
  /* set back to the beginning */
  if (read_msg != NULL) {
    read_line = read_msg->text;
  }
  return(0);
}

/*
 * ml_pageup - Scroll message display up one page
 *
 * Moves the message reading position backward by one screen page, allowing
 * users to scroll through message content in page-sized increments. Provides
 * efficient navigation for long messages by moving multiple lines at once.
 *
 * Parameters:
 *   void - No parameters required (uses global read_line and LINES)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Moves read_line backward by LINES - 11 positions
 *   - Stops at beginning of message if fewer lines available
 *
 * Notes:
 *   - Page size calculated as LINES - 11 to account for interface elements
 *   - Provides efficient navigation for long message content
 *   - Bound to 'b' key in default key bindings
 *   - Counterpart to ml_pagedown() function
 *   - Does not require realignment since moving backward
 */
/* ML_PAGEUP -- Move up a page of text */
static int
ml_pageup PARM_0(void)
{
  /* page mail up */
  if (read_line != NULL) {

    /* go LINES - 11 lines back */
    for (line_pos = 0;
	 line_pos < LINES - 11;
	 line_pos++) {
      if (read_line->prev == NULL) break;
      read_line = read_line->prev;
    }

  }
  return(0);
}

/*
 * ml_upone - Move message display up one line
 *
 * Moves the message reading position backward by one line, allowing users
 * to scroll through message content with fine-grained control. Provides
 * precise navigation for reviewing message text line by line.
 *
 * Parameters:
 *   void - No parameters required (uses global read_line)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Moves read_line to previous line if available
 *   - Does not move if already at first line of message
 *
 * Notes:
 *   - Provides precise backward navigation control
 *   - Bound to backspace and delete keys in default bindings
 *   - Part of core message navigation interface
 *   - Counterpart to ml_downone() function
 *   - No realignment needed when moving backward
 */
/* ML_UPONE -- Move upward one line */
static int
ml_upone PARM_0(void)
{
  if ((read_line != NULL) &&
      (read_line->prev != NULL)) {
    read_line = read_line->prev;
  }
  return(0);
}

/*
 * ml_to_next - Navigate to next mail message with optional filtering
 *
 * Moves to the next mail message in the mailbox, with optional filtering
 * to skip deleted messages. Provides core navigation functionality for
 * advancing through the mail list with user-configurable behavior.
 *
 * Parameters:
 *   non_deleted - If TRUE, skip deleted messages; if FALSE, include all messages
 *
 * Returns:
 *   void - No return value
 *
 * Side Effects:
 *   - Updates read_msg to point to next appropriate message
 *   - Updates cur_count to reflect new message position
 *   - Sets read_line to beginning of new message text
 *   - Sets ml_error message if no appropriate next message found
 *   - Maintains previous position if navigation fails
 *
 * Notes:
 *   - Core navigation function used by ml_next() and ml_nextund()
 *   - Preserves current position if no valid next message exists
 *   - Error messages provide user feedback for navigation failures
 *   - Essential for mail reader navigation workflow
 */
/* ML_TO_NEXT -- Move to the appropriate next mail message */
static
void ml_to_next PARM_1(int, non_deleted)
{
  /* is there a current and a next? */
  if ((read_msg != NULL) &&
      (read_msg->next != NULL)) {

    /* get the next none-deleted mail message */
    read_hold = read_msg;
    line_pos = cur_count;
    do {
      read_msg = read_msg->next;
      cur_count++;
    } while ((non_deleted) &&
	     (read_msg != NULL) &&
	     (read_msg->status & MSTAT_DELETED));

    /* reset if necessary */
    if (read_msg == NULL) {
      strcpy(ml_error, "There is no following non-deleted mail message");
      read_msg = read_hold;
      cur_count = line_pos;
    } else {
      read_line = read_msg->text;
    }

  } else {
    strcpy(ml_error, "There is no following mail message");
  }
}

/*
 * ml_next - Move to next mail message (including deleted)
 *
 * Advances to the next mail message in the mailbox, including deleted
 * messages. Provides basic forward navigation through the entire mail
 * list without filtering by message status.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls ml_to_next() with non_deleted=FALSE
 *   - Updates current message position and reading line
 *
 * Notes:
 *   - Bound to 'N' and Ctrl-N keys in default bindings
 *   - Shows all messages regardless of deletion status
 *   - Useful for reviewing all mail content including deleted items
 *   - Counterpart to ml_prev() function
 */
/* ML_NEXT -- Move to the next mail message */
static int
ml_next PARM_0(void)
{
  ml_to_next(FALSE);
  return(0);
}

/*
 * ml_nextund - Move to next undeleted mail message
 *
 * Advances to the next mail message that has not been marked for deletion.
 * Provides filtered navigation that skips deleted messages, allowing users
 * to focus on active mail content during normal mail reading workflow.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls ml_to_next() with non_deleted=TRUE
 *   - Updates current message position to next undeleted message
 *
 * Notes:
 *   - Bound to 'j' and 'n' keys in default bindings
 *   - Standard navigation function for normal mail reading
 *   - Skips over messages marked for deletion
 *   - Provides error feedback if no undeleted messages remain
 */
/* ML_NEXTUND -- Move to the next undeleted mail message */
static int
ml_nextund PARM_0(void)
{
  ml_to_next(TRUE);
  return(0);
}

/*
 * ml_pagedown - Scroll message display down one page or switch to message mode
 *
 * Advances the message display by one page if in message reading mode, or
 * switches from header mode to message mode. Automatically advances to next
 * undeleted message if already at bottom of current message. Provides intelligent
 * navigation that adapts to current display mode and content position.
 *
 * Parameters:
 *   void - No parameters required (uses global display state variables)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Switches header_mode to FALSE if currently in header mode
 *   - Advances read_line by LINES - 11 positions when in message mode
 *   - Calls ml_realign() to maintain display positioning
 *   - May call ml_nextund() if at bottom of message
 *
 * Notes:
 *   - Bound to space key in default bindings for intuitive paging
 *   - Intelligent behavior adapts to current mode and position
 *   - Provides seamless transition between header and message modes
 *   - Core function for efficient message reading workflow
 */
/* ML_PAGEDOWN -- Move the currently viewed message down one screen */
static int
ml_pagedown PARM_0(void)
{
  MAILD_PTR readl_hold = read_line;

  /* switch out of header mode */
  if (header_mode == TRUE) {
    header_mode = FALSE;
    if (read_msg != NULL) {
      read_line = read_msg->text;
    } else {
      read_line = NULL;
    }
    return(0);
  }

  /* now do the paging */
  if (read_line != NULL) {

    /* go LINES - 11 lines forward */
    for (line_pos = 0;
	 line_pos < LINES - 11;
	 line_pos++) {
      if (read_line->next == NULL) break;
      read_line = read_line->next;
    }

    /* now check the alignment */
    ml_realign();
    if (read_line == readl_hold) {
      ml_nextund();
    }

  }
  return(0);
}

/*
 * ml_to_prev - Navigate to previous mail message with optional filtering
 *
 * Moves to the previous mail message in the mailbox, with optional filtering
 * to skip deleted messages. Provides core backward navigation functionality
 * through the mail list with user-configurable filtering behavior.
 *
 * Parameters:
 *   non_deleted - If TRUE, skip deleted messages; if FALSE, include all messages
 *
 * Returns:
 *   void - No return value
 *
 * Side Effects:
 *   - Updates read_msg to point to previous appropriate message
 *   - Updates cur_count to reflect new message position
 *   - Sets read_line to beginning of new message text
 *   - Sets ml_error message if no appropriate previous message found
 *   - Maintains previous position if navigation fails
 *
 * Notes:
 *   - Core navigation function used by ml_prev() and ml_prevund()
 *   - Preserves current position if no valid previous message exists
 *   - Error messages provide user feedback for navigation failures
 *   - Mirror functionality to ml_to_next() for backward navigation
 */
/* ML_TO_PREV -- Move to the appropriate previous mail message */
static
void ml_to_prev PARM_1(int, non_deleted)
{
  /* go to the previous non-deleted message */
  if ((read_msg != NULL) &&
      (read_msg->prev != NULL)) {

    /* get the next none-deleted mail message */
    read_hold = read_msg;
    line_pos = cur_count;
    do {
      read_msg = read_msg->prev;
      cur_count--;
    } while ((non_deleted) &&
	     (read_msg != NULL) &&
	     (read_msg->status & MSTAT_DELETED));

    /* reset if necessary */
    if (read_msg == NULL) {
      strcpy(ml_error, "There is no preceding non-deleted mail message");
      read_msg = read_hold;
      cur_count = line_pos;
    } else {
      read_line = read_msg->text;
    }

  } else {
    strcpy(ml_error, "There is no preceding mail message");
  }
}

/*
 * ml_prev - Move to previous mail message (including deleted)
 *
 * Moves backward to the previous mail message in the mailbox, including
 * deleted messages. Provides basic backward navigation through the entire
 * mail list without filtering by message status.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls ml_to_prev() with non_deleted=FALSE
 *   - Updates current message position and reading line
 *
 * Notes:
 *   - Bound to 'P', 'K', and Ctrl-P keys in default bindings
 *   - Shows all messages regardless of deletion status
 *   - Useful for reviewing all mail content including deleted items
 *   - Counterpart to ml_next() function
 */
/* ML_PREV -- Move to the previous mail message */
static int
ml_prev PARM_0(void)
{
  ml_to_prev(FALSE);
  return(0);
}

/*
 * ml_prevund - Move to previous undeleted mail message
 *
 * Moves backward to the previous mail message that has not been marked for
 * deletion. Provides filtered backward navigation that skips deleted messages,
 * allowing users to focus on active mail content during mail reading workflow.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls ml_to_prev() with non_deleted=TRUE
 *   - Updates current message position to previous undeleted message
 *
 * Notes:
 *   - Bound to 'k' and 'p' keys in default bindings
 *   - Standard backward navigation for normal mail reading
 *   - Skips over messages marked for deletion
 *   - Provides error feedback if no undeleted messages remain
 */
/* ML_PREVUND -- Move to the previous undeleted mail message */
static int
ml_prevund PARM_0(void)
{
  ml_to_prev(TRUE);
  return(0);
}

/*
 * ml_purge - Remove all deleted mail messages from mailbox
 *
 * Permanently removes all messages marked for deletion from the mailbox.
 * Repositions to an undeleted message if the current message is deleted,
 * and updates message counts. Provides cleanup functionality to finalize
 * deletion operations and free memory from deleted messages.
 *
 * Parameters:
 *   void - No parameters required (operates on global mbox_list)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Removes all messages with MSTAT_DELETED status from mbox_list
 *   - Repositions current message to undeleted message if necessary
 *   - Calls kill_mail() to free memory for each deleted message
 *   - Updates mbox_list linkage to remove deleted nodes
 *   - Calls ml_countem() to update message counts
 *
 * Notes:
 *   - Bound to 'x' key for explicit purge operation
 *   - Also called automatically by ml_pandq() before exit
 *   - Permanent operation - deleted messages cannot be recovered
 *   - Essential for freeing memory and finalizing deletions
 */
/* ML_PURGE -- Purge all of the deleted mail messages */
static int
ml_purge PARM_0(void)
{
  RMAIL_PTR rmail_tmp;

  /* first move to any previously undeleted mail message */
  if ((read_msg != NULL) &&
      (read_msg->prev != NULL)) {

    /* get the next none-deleted mail message */
    read_hold = read_msg;
    line_pos = cur_count;
    while ((read_msg != NULL) &&
	   (read_msg->status & MSTAT_DELETED)) {
      read_msg = read_msg->prev;
      cur_count--;
    }

    /* try going forward? */
    if (read_msg == NULL) {

      /* now go forward */
      read_msg = read_hold;
      cur_count = line_pos;
      while ((read_msg != NULL) &&
	     (read_msg->status & MSTAT_DELETED)) {
	read_msg = read_msg->next;
	cur_count--;
      }

    }

    /* position the text */
    if (read_msg != NULL) {
      read_line = read_msg->text;
    } else {
      read_line = NULL;
    }
  }

  /* Remove all deleted mail */
  for (cur_message = mbox_list;
       cur_message != NULL;
       cur_message = rmail_tmp) {

    /* check for deletions */
    rmail_tmp = cur_message->next;
    if (cur_message->status & MSTAT_DELETED) {

      /* now get rid of it */
      if (cur_message == mbox_list) {
	mbox_list = rmail_tmp;
      }
      kill_mail();

    }
  }
  if (ml_done != TRUE) {
    ml_countem();
  }
  return(0);
}

/*
 * ml_quit - Exit mail reader without purging deleted messages
 *
 * Sets the mail reader exit flag to terminate the mail reading session.
 * Deleted messages remain in the mailbox and will be visible in future
 * sessions until explicitly purged. Provides clean exit without data loss.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Sets ml_done to TRUE to terminate mail reader main loop
 *
 * Notes:
 *   - Bound to 'Q' key for quit without purge
 *   - Preserves deleted messages for potential recovery
 *   - Contrasts with ml_pandq() which purges before exit
 *   - Allows user to exit without committing deletions
 */
/* ML_QUIT -- Exit the mail reader */
static int
ml_quit PARM_0(void)
{
  ml_done = TRUE;
  return(0);
}

/*
 * ml_pandq - Purge deleted messages and exit mail reader
 *
 * Combines message purging with mail reader exit in a single operation.
 * Permanently removes all deleted messages from the mailbox and then
 * terminates the mail reading session. Provides convenient cleanup and exit.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls ml_quit() to set exit flag
 *   - Calls ml_purge() to remove deleted messages
 *   - Terminates mail reader session with cleanup
 *
 * Notes:
 *   - Bound to 'q' key for standard exit with cleanup
 *   - Most common exit method for normal mail reading workflow
 *   - Permanent deletion operation - deleted messages cannot be recovered
 *   - Combines two operations for user convenience
 */
/* ML_PANDQ -- Purge the mail and then exit */
static int
ml_pandq PARM_0(void)
{
  ml_quit();
  ml_purge();
  return(0);
}

/*
 * ml_goto - Jump to specified mail message by number
 *
 * Prompts user for a message number and navigates directly to that message
 * in the mailbox. Provides random access navigation to any message by its
 * position number, with input validation and error handling for invalid numbers.
 *
 * Parameters:
 *   void - No parameters required (prompts user for message number)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Prompts user for input on bottom line of screen
 *   - Updates read_msg to point to specified message
 *   - Updates cur_count to specified message number
 *   - Sets read_line to beginning of target message text
 *   - Calls ml_center_hstart() to center header display
 *   - Displays error message for invalid message numbers
 *
 * Notes:
 *   - Bound to 'g' and 'G' keys in default bindings
 *   - Validates input range against total message count
 *   - Provides immediate navigation to any message in mailbox
 *   - Essential for navigating large mailboxes efficiently
 */
/* ML_GOTO -- Jump to a specified mail message */
static int
ml_goto PARM_0(void)
{
  /* query them */
  mvaddstr(LINES - 1, 0, "Move to what mail message? ");
  clrtoeol();
  line_pos = get_number(FALSE);
  if (no_input == TRUE) return(0);

  /* test the input */
  if ((line_pos < 1) || (line_pos > msg_count)) {
    errormsg("There is no such mail message");
    return(0);
  }

  /* now go there */
  if (cur_count < line_pos) {
    while (cur_count < line_pos) {
      read_msg = read_msg->next;
      cur_count++;
    }
  } else if (cur_count > line_pos) {
    while (cur_count > line_pos) {
      read_msg = read_msg->prev;
      cur_count--;
    }
  }
  read_line = read_msg->text;
  ml_center_hstart();
  return(0);
}

/*
 * ml_delete - Mark current message for deletion
 *
 * Marks the currently selected mail message for deletion by setting the
 * MSTAT_DELETED status flag. The message remains visible until purged but
 * is marked for removal. Provides non-destructive deletion marking that
 * can be reversed with ml_undelete().
 *
 * Parameters:
 *   void - No parameters required (operates on current message)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Sets MSTAT_DELETED flag in current message status
 *   - Message remains in mailbox until purged
 *
 * Notes:
 *   - Bound to 'D' key in default bindings
 *   - Non-destructive operation - message can be undeleted
 *   - Deleted messages skipped by ml_nextund() and ml_prevund()
 *   - Actual removal occurs during ml_purge() or ml_pandq()
 */
/* ML_DELETE -- Mark the current message for deletion */
static int
ml_delete PARM_0(void)
{
  /* do it */
  if (read_msg != NULL) {
    read_msg->status |= MSTAT_DELETED;
  }
  return(0);
}

/*
 * ml_dmarkall - Mark multiple messages for deletion based on criteria
 *
 * Marks messages for deletion based on specified criteria. Can mark either
 * all messages or only messages that have been read, depending on the
 * readonly parameter. Provides bulk deletion marking for mailbox cleanup.
 *
 * Parameters:
 *   readonly - If TRUE, mark only read messages; if FALSE, mark all messages
 *
 * Returns:
 *   void - No return value
 *
 * Side Effects:
 *   - Sets MSTAT_DELETED flag on selected messages
 *   - Traverses entire mailbox to evaluate each message
 *
 * Notes:
 *   - Used by ml_delread() and ml_delall() functions
 *   - Provides bulk operation capability for mailbox management
 *   - Messages remain in mailbox until purged
 *   - Deletion marking can be reversed with ml_undelete()
 */
/* ML_DMARKALL -- Mark all (or all read) messages for deletion */
static void
ml_dmarkall PARM_1(int, readonly)
{
  RMAIL_PTR rmail_tmp;

  /* loop and mark for deletion */
  for (rmail_tmp = mbox_list;
       rmail_tmp != NULL;
       rmail_tmp = rmail_tmp->next) {
    /* has this message already been read? */
    if ((readonly == FALSE) ||
	(rmail_tmp->status & MSTAT_READ)) {
      rmail_tmp->status |= MSTAT_DELETED;
    }
  }
}

/*
 * ml_delread - Prompt user to mark all read messages for deletion
 *
 * Prompts the user with a confirmation dialog to mark all previously read
 * messages for deletion. Provides bulk deletion capability for mailbox
 * cleanup while preserving unread messages. Requires explicit user confirmation.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Displays confirmation prompt to user
 *   - Calls ml_dmarkall(TRUE) if user confirms
 *   - Marks all read messages with MSTAT_DELETED flag
 *
 * Notes:
 *   - Bound to 'c' key in default bindings
 *   - Requires user confirmation via y_or_n() function
 *   - Preserves unread messages for user attention
 *   - Useful for regular mailbox maintenance
 */
/* ML_DELREAD -- Query user to Mark ALL read messages for deletion */
static int
ml_delread PARM_0(void)
{
  bottommsg("Mark all mail that has been read for deletion? ");
  if (y_or_n()) {
    ml_dmarkall(TRUE);
  }
  return(0);
}

/*
 * ml_delall - Prompt user to mark all messages for deletion
 *
 * Prompts the user with a confirmation dialog to mark ALL messages in the
 * mailbox for deletion. Provides complete mailbox clearing capability with
 * safety confirmation. Requires explicit user confirmation for this
 * destructive operation.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Displays confirmation prompt to user
 *   - Calls ml_dmarkall(FALSE) if user confirms
 *   - Marks all messages with MSTAT_DELETED flag
 *
 * Notes:
 *   - Bound to 'C' key in default bindings
 *   - Requires user confirmation via y_or_n() function
 *   - Destructive operation affecting all messages
 *   - Used for complete mailbox clearing
 */
/* ML_DELALL -- Query user to Mark ALL messages for deletion */
static int
ml_delall PARM_0(void)
{
  bottommsg("Mark ALL mail messages for deletion? ");
  if (y_or_n()) {
    ml_dmarkall(FALSE);
  }
  return(0);
}

/*
 * ml_delnext - Delete current message and advance to next
 *
 * Marks the current message for deletion and immediately advances to the
 * next undeleted message. Provides efficient workflow for quickly processing
 * and removing unwanted messages during mail reading sessions.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls ml_delete() to mark current message for deletion
 *   - Calls ml_nextund() to advance to next undeleted message
 *
 * Notes:
 *   - Bound to 'd' key in default bindings
 *   - Combines deletion with navigation for efficient workflow
 *   - Advances to next undeleted message automatically
 *   - Common operation for quickly cleaning mailbox
 */
/* ML_DELNEXT -- Delete the current message and go to the next */
static int
ml_delnext PARM_0(void)
{
  ml_delete();
  ml_nextund();
  return(0);
}

/*
 * ml_delprev - Delete current message and move to previous
 *
 * Marks the current message for deletion and immediately moves to the
 * previous undeleted message. Provides efficient backward workflow for
 * processing and removing messages while reviewing mailbox content.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls ml_delete() to mark current message for deletion
 *   - Calls ml_prevund() to move to previous undeleted message
 *
 * Notes:
 *   - Bound to Ctrl-D key in default bindings
 *   - Combines deletion with backward navigation
 *   - Moves to previous undeleted message automatically
 *   - Useful for reviewing and cleaning messages in reverse order
 */
/* ML_DELPREV -- Delete the current message and go to the previous */
static int
ml_delprev PARM_0(void)
{
  ml_delete();
  ml_prevund();
  return(0);
}

/*
 * ml_undelete - Restore previously deleted message
 *
 * Searches backward from the current position to find the most recently
 * deleted message and removes its deletion marker. Provides recovery
 * mechanism for accidentally deleted messages. Moves to the undeleted
 * message upon successful restoration.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Searches backward through message list for deleted message
 *   - Removes MSTAT_DELETED flag from found message
 *   - Updates read_msg and read_line to point to restored message
 *   - Updates cur_count to position of restored message
 *   - Sets ml_error if no deleted message found
 *
 * Notes:
 *   - Bound to 'u' key in default bindings
 *   - Searches backward from current position only
 *   - Restores most recently encountered deleted message
 *   - Provides safety mechanism for accidental deletions
 */
/* ML_UNDELETE -- Undelete the current or previously deleted message */
static int
ml_undelete PARM_0(void)
{
  RMAIL_PTR rmail_tmp;

  if (read_msg != NULL) {

    /* find the previously undeleted message */
    line_pos = cur_count;
    for (rmail_tmp = read_msg;
	 rmail_tmp != NULL;
	 rmail_tmp = rmail_tmp->prev) {
      if (rmail_tmp->status & MSTAT_DELETED) break;
      cur_count--;
    }

    /* do the undeleting */
    if (rmail_tmp != NULL) {
      read_msg = rmail_tmp;
      read_line = read_msg->text;
      read_msg->status &= ~MSTAT_DELETED;
    } else {
      cur_count = line_pos;
      strcpy(ml_error, "There is no previously deleted message");
    }
  }
  return(0);
}

/*
 * ml_unread - Mark previously read message as unread
 *
 * Searches backward from the current position to find the most recently
 * read message and removes its read status marker. Allows users to mark
 * messages as unread for later attention or review. Moves to the unmarked
 * message upon successful status change.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Searches backward through message list for read message
 *   - Removes MSTAT_READ flag from found message
 *   - Updates read_msg and read_line to point to unmarked message
 *   - Updates cur_count to position of unmarked message
 *   - Sets ml_error if no read message found
 *
 * Notes:
 *   - Bound to 'U' key in default bindings
 *   - Searches backward from current position only
 *   - Useful for marking messages for later review
 *   - Helps manage message status for workflow organization
 */
/* ML_UNREAD -- Unread the current or previously read message */
static int
ml_unread PARM_0(void)
{
  RMAIL_PTR rmail_tmp;

  if (read_msg != NULL) {

    /* find the previously unread message */
    line_pos = cur_count;
    for (rmail_tmp = read_msg;
	 rmail_tmp != NULL;
	 rmail_tmp = rmail_tmp->prev) {
      if (rmail_tmp->status & MSTAT_READ) break;
      cur_count--;
    }

    /* do the unreading */
    if (rmail_tmp != NULL) {
      read_msg = rmail_tmp;
      read_line = read_msg->text;
      read_msg->status &= ~MSTAT_READ;
    } else {
      cur_count = line_pos;
      strcpy(ml_error, "There is no previously read message");
    }
  }
  return(0);
}

/*
 * ml_toggle - Toggle between header list and message display modes
 *
 * Switches the mail reader display between header list mode (showing
 * multiple message summaries) and full message display mode (showing
 * complete message content). Provides flexible viewing options for
 * different mail reading workflows.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Toggles header_mode flag between TRUE and FALSE
 *   - Changes display format in next screen refresh
 *
 * Notes:
 *   - Bound to 'h' and 'H' keys in default bindings
 *   - Header mode shows message list with status indicators
 *   - Message mode shows full content of current message
 *   - Allows users to choose preferred viewing style
 */
/* ML_TOGGLE -- Toggle the header mode */
static int
ml_toggle PARM_0(void)
{
  header_mode ^= TRUE;
  return(0);
}

/*
 * ml_mail - Compose new mail message from reader
 *
 * Launches the mail composition interface from within the mail reader,
 * allowing users to compose and send new messages without exiting the
 * reader. Provides convenient access to mail composition during mail
 * reading sessions.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls rmail_mstart() to initialize new message composition
 *   - Launches mail editor interface
 *   - May allocate memory for new message structures
 *
 * Notes:
 *   - Bound to 'm' and 'M' keys in default bindings
 *   - Creates new message without reply or forward context
 *   - Returns to mail reader after composition completed
 *   - Convenient for sending messages while reading mail
 */
/* ML_MAIL -- Send some mail */
static int
ml_mail PARM_0(void)
{
  rmail_mstart(FALSE, FALSE, NULL);
  return(0);
}

/*
 * ml_qreply - Send quoted reply to current message
 *
 * Composes a reply to the current message with the original message text
 * included as quoted content. Sets up recipient, subject line with "Re:"
 * prefix, and includes original message with quote formatting. Provides
 * standard email reply functionality with context preservation.
 *
 * Parameters:
 *   void - No parameters required (uses current message as source)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls rmail_mstart() with reply and quote parameters
 *   - Sets recipient to sender of current message
 *   - Creates subject line with "Re:" prefix
 *   - Includes quoted text from original message
 *   - Launches mail editor for composition
 *
 * Notes:
 *   - Bound to 'R' key in default bindings
 *   - Includes original message text with quote prefix
 *   - Standard reply function for maintaining conversation context
 *   - Quote format controlled by global quote prefix settings
 */
/* ML_QREPLY -- Send a quoted reply */
static int
ml_qreply PARM_0(void)
{
  rmail_mstart(TRUE, TRUE, read_msg);
  return(0);
}

/*
 * ml_reply - Send reply to current message without quoting
 *
 * Composes a reply to the current message without including the original
 * message text. Sets up recipient and subject line with "Re:" prefix but
 * starts with empty message body. Provides clean reply functionality for
 * brief responses that don't require original context.
 *
 * Parameters:
 *   void - No parameters required (uses current message as source)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls rmail_mstart() with reply but no quote parameters
 *   - Sets recipient to sender of current message
 *   - Creates subject line with "Re:" prefix
 *   - Starts with empty message body
 *   - Launches mail editor for composition
 *
 * Notes:
 *   - Bound to 'r' key in default bindings
 *   - Clean reply without original message text
 *   - Useful for brief responses that don't need context
 *   - Faster composition for simple acknowledgments
 */
/* ML_REPLY -- Send a quoted reply */
static int
ml_reply PARM_0(void)
{
  rmail_mstart(TRUE, FALSE, read_msg);
  return(0);
}

/*
 * ml_qforward - Forward current message with quoted text
 *
 * Forwards the current message to another recipient with the original
 * message text included as quoted content. Sets up subject line with
 * "FWD:" prefix and includes complete original message with formatting.
 * Provides standard email forwarding with full context preservation.
 *
 * Parameters:
 *   void - No parameters required (uses current message as source)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls rmail_mstart() with forward and quote parameters
 *   - Creates subject line with "FWD:" prefix
 *   - Includes quoted text from original message
 *   - Launches mail editor for recipient selection and composition
 *
 * Notes:
 *   - Bound to 'F' key in default bindings
 *   - Includes original message text with quote prefix
 *   - Standard forwarding function for sharing message content
 *   - Allows adding comments before forwarded content
 */
/* ML_QFORWARD -- Send a quoted reply */
static int
ml_qforward PARM_0(void)
{
  rmail_mstart(FALSE, TRUE, read_msg);
  return(0);
}

/*
 * ml_forward - Forward current message without quoting
 *
 * Forwards the current message to another recipient without including
 * the original message text as quotes. Sets up subject line with "FWD:"
 * prefix but starts with empty message body for custom forwarding content.
 *
 * Parameters:
 *   void - No parameters required (uses current message as source)
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls rmail_mstart() with forward but special quote parameter (-1)
 *   - Creates subject line with "FWD:" prefix
 *   - Starts with empty message body for custom content
 *   - Launches mail editor for recipient selection and composition
 *
 * Notes:
 *   - Bound to 'f' key in default bindings
 *   - Clean forward without original message text
 *   - Allows custom forwarding message composition
 *   - Useful for forwarding with personal commentary only
 */
/* ML_FORWARD -- Send a quoted reply */
static int
ml_forward PARM_0(void)
{
  rmail_mstart(FALSE, -1, read_msg);
  return(0);
}

/*
 * ml_show - Display current mail message or header list
 *
 * Renders the mail reader interface, displaying either a list of message
 * headers or the full content of the current message based on the current
 * display mode. Handles complex display logic including status indicators,
 * message formatting, header alignment, and screen positioning.
 *
 * Parameters:
 *   void - No parameters required (uses global display state variables)
 *
 * Returns:
 *   void - No return value
 *
 * Side Effects:
 *   - Clears screen and redraws entire mail reader interface
 *   - Marks current message as read when displayed in message mode
 *   - Updates last_msg tracking for read status management
 *   - Displays message headers with status indicators in header mode
 *   - Shows formatted message content with sender/subject/date in message mode
 *   - Updates screen with current position information and status
 *   - Displays error messages when available
 *
 * Notes:
 *   - Complex function handling dual display modes (header/message)
 *   - Header mode shows multiple messages with status flags (N/D/R/F)
 *   - Message mode shows full content with formatting and navigation context
 *   - Automatically marks messages as read when viewed
 *   - Uses gaudy_lineout() or normal_lineout() based on display preferences
 *   - Central display function called in main mail reader loop
 */
/* ML_SHOW -- Show the mail message */
static void
ml_show PARM_0(void)
{
  /* clean up shop */
  line_pos = 0;
  move(0, 0);
  clrtobot();

  /* show the current message and information */
  if (read_msg != NULL) {

    /* now choose the mode of display */
    if (header_mode == TRUE) {

      /* variable to keep track of message number */
      int header_count = cur_count;

      /* keep header within bounds */
      if (hdr_start < 1) {
	hdr_start = 1;
      } else if (hdr_start > cur_count) {
	hdr_start = cur_count;
      } else if (cur_count - hdr_start >= LINES - 7) {
	hdr_start = cur_count - (LINES - 8);
      }

      /* found out number to preceed with */
      line_pos = cur_count - hdr_start;
      for (read_hold = read_msg;
	   line_pos > 0;
	   line_pos--) {
	if (read_hold->prev == NULL) break;
	read_hold = read_hold->prev;
	header_count--;
      }

      /* now travel forward */
      for (line_pos = 2; line_pos < LINES - 5; line_pos++) {

	/* are we highlighting? */
	if (read_hold == read_msg) standout();

	/* keep track of message number */
	move(line_pos, 0);

	/* now show the information */
	printw("%3d ", header_count++);

	/* display the status indicators */
	if (read_hold->status & MSTAT_READ) {
	  addch('-');
	} else {
	  addch('N');
	}
	if (read_hold->status & MSTAT_DELETED) {
	  addch('D');
	} else {
	  addch('-');
	}
	if (read_hold->status & MSTAT_REPLIED) {
	  addch('R');
	} else {
	  addch('-');
	}
	if (read_hold->status & MSTAT_FORWARDED) {
	  addch('F');
	} else {
	  addch('-');
	}

	/* show who sent it */
	printw(" %-*.*s", NAMELTH, NAMELTH, read_hold->sender);

	/* when was it sent? */
	printw(" %-24.24s", read_hold->date);

	/* now send the subject */
	if (strcmp(read_hold->subj, "[none]") != 0) {
	  printw(" \"%-.*s\"", COLS - NAMELTH - 38, read_hold->subj);
	} else {
	  addstr(" [none]");
	}

	/* were we highlighting? */
	if (read_hold == read_msg) standend();

	/* go to the next line */
	if ((read_hold = read_hold->next) == NULL) break;
      }

    } else {

      /* indicate the the message has been read */
      if (last_msg != read_msg) {
	read_msg->status |= MSTAT_READ;
	last_msg = read_msg;
      }

      /* show who it is from */
      line_pos++;
      mvaddstr(line_pos++, 0, "From: ");
      if (strlen(read_msg->nickname) > 0) {
	printw("%s (%s)", read_msg->sender, read_msg->nickname);
      } else {
	printw("%s", read_msg->sender);
      }

      /* show what it is about */
      if (strcmp(read_msg->subj, "[none]") != 0) {
	mvaddstr(line_pos++, 0, "Subject: ");
	standout();
	printw("%.*s", COLS - 9, read_msg->subj);
	standend();
      } else {
	mvaddstr(line_pos++, 0, "Subject: [none]");
      }

      /* show the creation time */
      mvprintw(line_pos++, 0, "Date: %s", read_msg->date);

      /* display the message */
      if ((read_line != NULL) &&
	  (read_line->prev != NULL)) {
	mvprintw(line_pos, 0, "...cont...");
      }
      line_pos++;

      /* show what is visible */
      for (travel_line = read_line;
	   (line_pos < LINES - 5) &&
	   (travel_line != NULL);
	   travel_line = travel_line->next) {

	/* show it */
	if (conq_gaudy == TRUE) {
	  gaudy_lineout(line_pos++, travel_line->line_data);
	} else {
	  normal_lineout(line_pos++, travel_line->line_data);
	}

      }

      /* check for more */
      if (travel_line != NULL) {
	mvaddstr(line_pos, 0, "...more...");
      }

    }
  }

  /* where are we in the message list? */
  mvprintw(0, 0, "Message Number %d of %d",
	   cur_count, msg_count);

  /* show any headers */
  if (read_msg != NULL) {

    /* display the status indicators */
    if (!(read_msg->status & MSTAT_READ)) {
      addstr("; unread");
    }
    if (read_msg->status & MSTAT_DELETED) {
      addstr("; deleted");
    }
    if (read_msg->status & MSTAT_REPLIED) {
      addstr("; replied");
    }
    if (read_msg->status & MSTAT_FORWARDED) {
      addstr("; forwarded");
    }
  }

  /* show the command summary */
  errorbar("Conquer Mail Reader", "Hit '?' for help");
  move(LINES - 1, 0);
  if (ml_error[0] != '\0') {
    bottommsg(ml_error);
    ml_error[0] = '\0';
  } else {
    clrtoeol();
  }
  refresh();
}

/*
 * rmail_parse - Main mail reader interface and message processing loop
 *
 * Primary entry point for the mail reading interface. Initializes the mail
 * reading environment, loads all messages from the mailbox file, and runs
 * the main input processing loop for mail reader commands. Handles file
 * locking, error conditions, and cleanup operations.
 *
 * Parameters:
 *   void - No parameters required (uses global mail settings)
 *
 * Returns:
 *   0 - Always returns success after mail reading session completion
 *
 * Side Effects:
 *   - Creates mail file lock to prevent concurrent access
 *   - Calls open_mailbox() to load all messages into memory
 *   - Initializes mail reader display and navigation state
 *   - Runs main command processing loop with parse_keys()
 *   - Calls rmail_close() to save changes and release lock
 *   - Sets global mail indicators (mail_ind, conq_mail_status)
 *   - May mark old messages for deletion if mailbox is very large
 *
 * Notes:
 *   - Public interface function called from main game to read mail
 *   - Handles empty mailbox condition with appropriate error message
 *   - Provides warning and auto-cleanup for mailboxes over 100 messages
 *   - Main loop continues until ml_done flag set by quit functions
 *   - Critical function managing entire mail reading session lifecycle
 */
/* RMAIL_PARSE -- Peruse all of the mail for the user */
int
rmail_parse PARM_0(void)
{
  int first_time = TRUE;
  FNCI fnc_ptr;

  /* clean up the bottom for now */
  clear_bottom(0);

  /* initialize things first time in */
  if (first_time == TRUE) {
    sprintf(maillock, "%s%s.%s", nationname, msgtag, isontag);
    header_mode = conq_mheaders;
    first_time = FALSE;
  }

  /* perform the locking */
  if ((lockid = check_lock(maillock, TRUE)) == -1) {
    errormsg("Someone is sending you mail... Please Wait");
    return(0);
  }

  /* is there any mail? */
  if ((msg_count = open_mailbox()) == 0) {
    errormsg("You have no Conquer mail");
    kill_lock(lockid, maillock);
    return(0);
  }
  mail_ind = MAIL_READING;

  /* initialize mail settings */
  clear();
  ml_done = FALSE;
  cur_count = 1;
  ml_error[0] = '\0';
  align_reader_keys();

  /* find the first unread mail message */
  ml_first();

  /* give a warning for large mail files */
  if (msg_count > 100) {
    redraw = DRAW_FULL;
    ml_dmarkall(TRUE);
    ml_show();
    errormsg("WARNING:  You have over 100 messages, old mail marked for deletion");
  }

  /* peruse and manipulate the mail */
  redraw = DRAW_FULL;
  while (ml_done == FALSE) {

    /* show it */
    ml_show();

    /* now get the input */
    if ((fnc_ptr = parse_keys(reader_bindings, FALSE)) == NULL) {

      /* oops */
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Unknown key binding: ");
      show_str(string, FALSE);
      presskey();

    } else {

      /* perform the function */
      (*fnc_ptr)();

    }

  }

  /* close up shop */
  rmail_close();
  mail_ind = MAIL_DONE;
  conq_mail_status = STMAIL_NONE;
  return(0);
}

/*
 * ml_options - Launch options configuration interface from mail reader
 *
 * Provides access to the game's options configuration system from within
 * the mail reader interface. Allows users to adjust mail reader key bindings
 * and other configuration settings without exiting the mail reader.
 *
 * Parameters:
 *   void - No parameters required
 *
 * Returns:
 *   0 - Always returns success
 *
 * Side Effects:
 *   - Calls option_cmd() with reader key system parameters
 *   - May modify reader_bindings key binding configuration
 *   - Temporarily suspends mail reader interface
 *
 * Notes:
 *   - Bound to 'O' key in default bindings
 *   - Provides convenient access to configuration from mail reader
 *   - Returns to mail reader after options configuration completed
 *   - Allows customization of mail reader key bindings and behavior
 */
/* ML_OPTIONS -- Quickie command to allow the setting of options */
static int
ml_options PARM_0(void)
{
  option_cmd(reader_keysys, &reader_bindings);
  return(0);
}

/* function list */
PARSE_STRUCT reader_funcs[] = {
  {ml_options, "conquer-options",
     "Adjust the conquer environment by changing various options"},
  {ml_pageup, "reader-backward",
     "Reader-mode, scroll the current message backward one page"},
  {ml_bottom, "reader-bottom",
     "Reader-mode, shift the view to the bottom of the current message"},
  {ml_delete, "reader-delete",
     "Reader-mode, mark the current mail message for deletion"},
  {ml_delall, "reader-delete-all",
     "Reader-mode, delete all mail in the mailbox"},
  {ml_delnext, "reader-delete-and-next",
     "Reader-mode, delete the current message and find the next"},
  {ml_delprev, "reader-delete-and-previous",
     "Reader-mode, delete the current message and find the previous"},
  {ml_delread, "reader-delete-read-mail",
     "Reader-mode, delete all mail that has been read before"},
  {ml_downone, "reader-down-one",
     "Reader-mode, move the current message forward one line" },
  {ml_pandq, "reader-exit",
     "Reader-mode, purge deleted mail and then exit the mail reader"},
  {ml_pagedown, "reader-forward",
     "Reader-mode, scroll the current message forward one page"},
  {ml_goto, "reader-goto",
     "Reader-mode, jump to a specified mail message"},
  {ml_help, "reader-help",
     "Show the list of available mail reader commands and bindings"},
  {ml_mail, "reader-mail",
     "Reader-mode, send a mail message to another nation"},
  {ml_forward, "reader-mail-forward",
     "Reader-mode, forward the current message to another player"},
  {ml_reply, "reader-mail-reply",
     "Reader-mode, send a reply to the current mail message"},
  {ml_next, "reader-next",
     "Reader-mode, move to the next mail message"},
  {ml_nextund, "reader-next-undeleted",
     "Reader-mode, move to the next undeleted mail message"},
  {ml_prev, "reader-previous",
     "Reader-mode, move to the previous mail message"},
  {ml_prevund, "reader-previous-undeleted",
     "Reader-mode, move to the previous undeleted mail message"},
  {ml_purge, "reader-purge",
     "Reader-mode, flush any mail messages marked as deleted"},
  {ml_quit, "reader-quit",
     "Reader-mode, exit the mail reader, leaving deleted mail intact"},
  {ml_qforward, "reader-quoted-forward",
     "Reader-mode, forward the current message, quoted, to another player"},
  {ml_qreply, "reader-quoted-reply",
     "Reader-mode, send a reply and include the original message text"},
  {ml_toggle, "reader-toggle",
     "Reader-mode, toggle between header and full screen message mode"},
  {ml_top, "reader-top",
     "Reader-mode, shift the view to the top of the current message"},
  {ml_undelete, "reader-undelete",
     "Reader-mode, remove the deletion marker from a previously delete msg"},
  {ml_unread, "reader-unread",
     "Reader-mode, remove the indication that a message has been read"},
  {ml_upone, "reader-up-one",
     "Reader-mode, move the current message backward one line" },
  {do_redraw, "redraw-screen",
     "redraw the screen display of the mail message"},
  {do_ignore, "ignore-key",
      "I'll just pretend as if I never saw that key stroke" }
};

/* Default reader key bindings */
KBIND_STRUCT reader_klist[] = {
  { CSTR_B, do_ignore },
  { CSTR_D, ml_delprev },
  { CSTR_F, do_ignore },
  { CSTR_L, do_redraw },
  { CSTR_N, ml_next },
  { CSTR_P, ml_prev },
  { CSTR_R, do_redraw },
  { AKEY_UP1, ml_prev },
  { AKEY_UP2, ml_prev },
  { AKEY_DOWN1, ml_next },
  { AKEY_DOWN2, ml_next },
  { AKEY_LEFT1, do_ignore },
  { AKEY_LEFT2, do_ignore },
  { AKEY_RIGHT1, do_ignore },
  { AKEY_RIGHT2, do_ignore },
  { ESTR_DEL, ml_upone },
  { "<", ml_top },
  { ">", ml_bottom },
  { "?", ml_help },
  { " ", ml_pagedown },
  { "\b", ml_upone },
  { "\n", ml_downone },
  { "\r", ml_downone },
  { "b", ml_pageup },
  { "C", ml_delall },
  { "c", ml_delread },
  { "D", ml_delete },
  { "d", ml_delnext },
  { "F", ml_qforward },
  { "f", ml_forward },
  { "G", ml_goto },
  { "g", ml_goto },
  { "H", ml_toggle },
  { "h", ml_toggle },
  { "J", ml_next },
  { "j", ml_nextund },
  { "K", ml_prev },
  { "k", ml_prevund },
  { "M", ml_mail },
  { "m", ml_mail },
  { "N", ml_next },
  { "n", ml_nextund },
  { "O", ml_options },
  { "P", ml_prev },
  { "p", ml_prevund },
  { "Q", ml_quit },
  { "q", ml_pandq },
  { "R", ml_qreply },
  { "r", ml_reply },
  { "U", ml_unread },
  { "u", ml_undelete },
  { "x", ml_purge }
};

/*
 * align_reader_keys - Initialize mail reader key binding system
 *
 * Initializes the mail reader key binding system if not already configured.
 * Calculates binding and parse table sizes and creates the initial key
 * binding list from the default key binding array. Provides one-time
 * setup for the mail reader key binding infrastructure.
 *
 * Parameters:
 *   void - No parameters required (operates on global reader key system)
 *
 * Returns:
 *   void - No return value
 *
 * Side Effects:
 *   - Calculates reader_keysys.num_binds from reader_klist array size
 *   - Calculates reader_keysys.num_parse from reader_funcs array size
 *   - Calls init_keys() to create reader_bindings from default array
 *   - Only performs initialization if reader_bindings is NULL
 *
 * Notes:
 *   - Called from rmail_parse() during mail reader initialization
 *   - Idempotent function - safe to call multiple times
 *   - Essential setup function for mail reader key binding system
 *   - Uses sizeof calculations to determine array sizes automatically
 *   - Public function accessible from other modules if needed
 */
/* ALIGN_READER_KEYS -- Align all of the mail reader keys */
void
align_reader_keys PARM_0(void)
{
  /* initialize the system */
  if (reader_bindings == NULL) {
    reader_keysys.num_binds = (sizeof(reader_klist)/sizeof(KBIND_STRUCT));
    reader_keysys.num_parse = (sizeof(reader_funcs)/sizeof(PARSE_STRUCT));
    init_keys(&reader_bindings, reader_klist, reader_keysys.num_binds);
  }
}
