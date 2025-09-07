/*
 * emailG.c - Interactive Mail Composition and Editing Interface
 *
 * This module provides a sophisticated interactive mail editor for composing
 * and sending in-game messages within the Conquer game system. The interface
 * supports multi-modal editing with distinct modes for recipient selection,
 * subject editing, and message body composition.
 *
 * Key Features:
 * - Multi-Modal Interface: Three distinct editing modes (To:, Subject:, Body)
 * - File Locking System: Prevents concurrent access to recipient mail files
 * - Customizable Key Bindings: Comprehensive key binding system for all operations
 * - Text Editor Functionality: Full-featured text editing with insert/overwrite modes
 * - External Editor Support: Optional integration with system text editors
 * - Interactive Navigation: Arrow key and command-based cursor movement
 * - Input Validation: Recipient name validation and message content checking
 * - Message Delivery: Safe message transmission with confirmation prompts
 *
 * Editor Modes:
 * - EM_S_TOLINE (1): Recipient selection and management mode
 * - EM_S_SUBJECT (2): Subject line editing mode  
 * - EM_S_BODY (3): Message body composition mode
 * - EM_S_INSERTON (0x0004): Insert mode flag (vs overwrite mode)
 *
 * File Locking Architecture:
 * - Uses em_locks[] array to track recipient file locks
 * - Prevents mail delivery conflicts in multi-user environment
 * - Automatic cleanup on editor exit or error conditions
 *
 * Key Binding System:
 * - Configurable key mappings for all editor operations
 * - Platform-specific bindings (VMS vs Unix/Linux)
 * - Support for both control sequences and escape sequences
 * - Arrow key navigation support with fallback sequences
 *
 * Message Structure:
 * - Dynamic recipient list with lock management
 * - Configurable subject line length limits
 * - Linked-list based message body for flexible content
 * - Sender identification and nickname assignment
 *
 * Integration Points:
 * - Nation management system for recipient validation
 * - File I/O system for mail delivery and storage
 * - Display system for real-time editor rendering
 * - Input system for character processing and key binding
 *
 * Security Features:
 * - File lock validation prevents mail corruption
 * - Recipient existence verification before message composition
 * - Safe memory management with proper cleanup procedures
 * - Input sanitization for mail content and commands
 *
 * Copyright (c) 1992 by Ed Barlow and Adam Bryant
 * Licensed for modification with attribution and notification requirements
 */
#include "dataG.h"
#include "rmailX.h"
#include "keyvalsX.h"

/* indicators current status */
#define EM_S_TOLINE	1
#define EM_S_SUBJECT	2
#define EM_S_BODY	3
#define EM_S_INSERTON	0x0004
#define EM_S_VICOMMAND	0x0008		/* vi mode not implemented */
#define EM_S_VIINSERT	0x0010		/* vi mode not implemented */

/* maximum number of mail targets; <= MAX_ADR */
#define EM_MAXLOCKS	MAX_ADR

/* declaration for lock file management */
static int em_locks[EM_MAXLOCKS];

/* declaration of keybindings at bottom */
extern KBIND_STRUCT email_klist[];
extern PARSE_STRUCT email_funcs[];
KLIST_PTR email_bindings = NULL;

/* information structure concerning movement keybindings */
KEYSYS_STRUCT email_keysys = {
  "mail", email_funcs, email_klist, 0, 0
};

/* declaration of location indicators */
static MAILD_PTR cur_line = NULL;
static int char_position = 0;

/* various indicator variables */
static int email_done;
static int email_status;
int email_collim;
int email_subjlim = 70;		/* 80 - sizeof(Subject:) */

/*
 * email_lock - Set file lock for mail recipient to prevent concurrent access
 *
 * Creates a file lock for the specified recipient's mail file to ensure
 * safe message delivery in a multi-user environment. This prevents mail
 * corruption when multiple users attempt to send messages simultaneously
 * to the same recipient.
 *
 * Parameters:
 *   lnum - Lock index position in em_locks array (0 to EM_MAXLOCKS-1)
 *   who - Nation ID of the intended mail recipient
 *
 * Returns:
 *   TRUE if lock successfully acquired and recipient added to message
 *   FALSE if lock acquisition failed or invalid parameters
 *
 * Side Effects:
 *   - Sets em_locks[lnum] to the acquired lock file descriptor
 *   - Updates cur_message->to_whom[lnum] with recipient nation ID
 *   - Displays error messages for lock failures or invalid recipients
 *
 * Notes:
 *   - Uses check_lock() to acquire exclusive file access
 *   - Validates recipient nation existence before lock attempt
 *   - Constructs lock filename using nation name, msgtag, and isontag
 *   - Prevents duplicate locks by checking em_locks[lnum] availability
 */
static int
email_lock PARM_2(int, lnum, int, who)
{
  char buf[NAMELTH + 1], tfname[LINELTH];

  /* is it used already */
  if (em_locks[lnum] > -1) {
    /* cannot supply a lock */
    errormsg("You cannot send mail to that many people");
    return(FALSE);
  } else {
    /* supply it */
    if (ntn_realname(&(buf[0]), who)) return(FALSE);
    sprintf(tfname, "%s%s.%s", buf, msgtag, isontag);
    if ((em_locks[lnum] = check_lock(tfname, TRUE)) == -1) {
      sprintf(string,
	      "Nation %s's mail file is in use; Try again later...",
	      buf);
      if (global_int == TRUE) next_char();
      errormsg(string);
      return(FALSE);
    }
    cur_message->to_whom[lnum] = who;
  }
  return(TRUE);
}

/*
 * email_unlock - Remove file lock for specified mail recipient
 *
 * Releases a previously acquired file lock for a mail recipient and
 * cleans up associated data structures. This ensures proper cleanup
 * when removing recipients from a message or when exiting the editor.
 *
 * Parameters:
 *   lnum - Lock index position in em_locks array to release
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Calls kill_lock() to release the file system lock
 *   - Resets em_locks[lnum] to -1 (unlocked state)
 *   - Sets cur_message->to_whom[lnum] to ABSMAXNTN (no recipient)
 *   - Constructs lock filename for proper cleanup
 *
 * Notes:
 *   - Only processes valid locks (em_locks[lnum] > -1)
 *   - Uses ntn_realname() to get recipient nation name for filename
 *   - Safe to call multiple times on the same lock index
 *   - Essential for preventing lock file accumulation in file system
 */
static void
email_unlock PARM_1(int, lnum)
{
  char buf[NAMELTH + 1], tfname[LINELTH];

  if (em_locks[lnum] > -1) {

    /* kill the lock */
    if (!ntn_realname(&(buf[0]), cur_message->to_whom[lnum])) {
      sprintf(tfname, "%s%s.%s", buf, msgtag, isontag);
      kill_lock(em_locks[lnum], tfname);
    }

    /* clear it */
    em_locks[lnum] = -1;
    cur_message->to_whom[lnum] = ABSMAXNTN;

  }
}

/*
 * email_close - Free allocated resources and remove all mail recipient locks
 *
 * Performs comprehensive cleanup when exiting the mail editor, ensuring
 * all file locks are released and memory is properly freed. This function
 * is essential for maintaining system integrity in multi-user environments.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Releases all file locks by calling email_unlock() for each index
 *   - Clears MAIL_SENDING flag from mail_ind status indicator
 *   - Frees all dynamically allocated mail message memory via free_mail()
 *   - Sets redraw flag to DRAW_FULL for screen refresh
 *
 * Notes:
 *   - Called automatically when exiting mail editor (normal or error exit)
 *   - Iterates through all EM_MAXLOCKS positions for complete cleanup
 *   - Critical for preventing file lock accumulation and memory leaks
 *   - Prepares display system for return to main game interface
 */
void
email_close PARM_0(void)
{
  int i;

  /* handle all of the locks */
  for (i = 0; i < EM_MAXLOCKS; i++) {
    email_unlock(i);
  }

  /* now free up the used space */
  mail_ind &= ~MAIL_SENDING;
  free_mail();
  redraw = DRAW_FULL;
}

/*
 * email_init - Initialize mail editor key binding system
 *
 * Sets up the key binding infrastructure for the mail editor, establishing
 * the mapping between key sequences and mail editor functions. This must
 * be called before the mail editor can process user input.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on successful initialization
 *
 * Side Effects:
 *   - Initializes email_bindings pointer with key mapping data
 *   - Links email_klist key definitions to corresponding functions
 *   - Establishes proper key sequence parsing for mail editor
 *
 * Notes:
 *   - Called automatically by email_prep() if bindings not initialized
 *   - Uses init_keys() to process the email_klist key binding array
 *   - Required for proper mail editor key sequence recognition
 *   - Sets up platform-specific key mappings and escape sequences
 */
int
email_init PARM_0(void)
{
  init_keys(&email_bindings, email_klist, email_keysys.num_binds);
  return(0);
}

/*
 * em_send - Deliver the composed mail message to all recipients
 *
 * Initiates the mail delivery process after validating that recipients
 * have been specified. Provides user confirmation before sending and
 * handles the actual message delivery through the mail system.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   TRUE if message was successfully sent
 *   FALSE if delivery was cancelled or no recipients specified
 *
 * Side Effects:
 *   - Displays confirmation prompt to user
 *   - Calls deliver_mail() to perform actual message delivery
 *   - Shows "Sent..." confirmation message with 1-second delay
 *   - Refreshes screen display after successful delivery
 *
 * Notes:
 *   - Validates that at least one recipient is specified (to_whom[0] != ABSMAXNTN)
 *   - Uses y_or_n() for user confirmation before sending
 *   - Essential function for completing the mail composition process
 *   - Called by em_exit() and available as standalone send command
 */
static int
em_send PARM_0(void)
{
  /* first check if there is anyone to send to */
  if (cur_message->to_whom[0] == ABSMAXNTN) {
    errormsg("You have not indicated anyone to send the message to");
    return(FALSE);
  }

  /* query it */
  standout();
  mvaddstr(LINES - 1, 0, "Deliver the mail message? ");
  standend();
  if (y_or_n()) {
    deliver_mail();
    bottommsg("Sent...");
    refresh();
    sleep(1);
    return(TRUE);
  }
  return(FALSE);
}

/*
 * em_exit - Deliver mail message and exit editor if successful
 *
 * Attempts to send the current mail message and exits the editor only
 * if the delivery is successful. This combines message sending with
 * editor termination in a single operation.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always returns 0, actual success determined by email_done flag)
 *
 * Side Effects:
 *   - Calls em_send() to attempt message delivery
 *   - Sets email_done = TRUE if delivery successful
 *   - Leaves editor open if delivery fails or is cancelled
 *
 * Notes:
 *   - Provides safe exit that ensures message is sent before closing
 *   - Prevents accidental loss of composed messages
 *   - Most common way to complete mail composition session
 *   - Mapped to Ctrl-X key binding by default
 */
static int
em_exit PARM_0(void)
{
  if (em_send() == TRUE) {
    email_done = TRUE;
  }
  return(0);
}

/*
 * em_quit - Exit mail editor without sending message after confirmation
 *
 * Provides a safe way to exit the mail editor without sending the
 * composed message. Requires user confirmation to prevent accidental
 * loss of work.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always returns 0, actual exit determined by email_done flag)
 *
 * Side Effects:
 *   - Displays confirmation prompt asking to exit without sending
 *   - Sets email_done = TRUE if user confirms exit
 *   - Leaves editor open if user cancels the quit operation
 *
 * Notes:
 *   - Mapped to Ctrl-G key binding by default
 *   - Uses bottommsg() for confirmation prompt display
 *   - Uses y_or_n() for user response processing
 *   - Important safety feature to prevent accidental message loss
 */
static int
em_quit PARM_0(void)
{
  /* query an exit */
  bottommsg("Do you wish to exit without sending the mail? ");
  if (y_or_n()) {
    email_done = TRUE;
  }
  return(0);
}

/*
 * em_toggle - Toggle between insert and overwrite editing modes
 *
 * Switches the mail editor between insert mode (where new characters
 * are inserted at cursor position) and overwrite mode (where new
 * characters replace existing characters at cursor position).
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Toggles EM_S_INSERTON bit in email_status variable
 *   - Changes character insertion behavior for subsequent input
 *
 * Notes:
 *   - Uses XOR operation to toggle the EM_S_INSERTON flag
 *   - Affects behavior in subject line and message body editing
 *   - Mapped to Ctrl-T key binding on Unix/Linux systems
 *   - Mapped to Ctrl-A key binding on VMS systems
 *   - Mode state persists throughout editing session
 */
static int
em_toggle PARM_0(void)
{
  email_status ^= EM_S_INSERTON;
  return(0);
}

/*
 * em_toeol - Move cursor to end of current line in current editing mode
 *
 * Positions the cursor at the end of the current line based on the
 * active editing mode (To:, Subject:, or Body). The end position
 * varies depending on the data structure being edited.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Updates char_position to end-of-line position for current mode
 *   - EM_S_TOLINE: Positions after last recipient in to_whom array
 *   - EM_S_SUBJECT: Positions after last character in subject string
 *   - EM_S_BODY: Positions after last character in current line data
 *
 * Notes:
 *   - Uses email_status % 4 to determine current editing mode
 *   - In To: mode, finds first ABSMAXNTN entry indicating end of recipients
 *   - In Subject mode, uses strlen() to find string end
 *   - In Body mode, uses strlen() on current line's data
 *   - Essential for quick end-of-line navigation
 */
static int
em_toeol PARM_0(void)
{
  switch (email_status % 4) {
  case EM_S_TOLINE:
    /* last element in the To: line */
    for (char_position = 0;
	 char_position < EM_MAXLOCKS;
	 char_position++) {
      if (cur_message->to_whom[char_position] == ABSMAXNTN) break;
    }
    break;
  case EM_S_SUBJECT:
    /* length of the subject */
    char_position = strlen(cur_message->subj);
    break;
  case EM_S_BODY:
    /* simply set the value to the length of the string */
    char_position = strlen(cur_line->line_data);
    break;
  }
  return(0);
}

/*
 * em_tobol - Move cursor to beginning of current line
 *
 * Positions the cursor at the beginning of the current line in any
 * editing mode. This provides quick navigation to the start of the
 * current editing context.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (char_position after setting to 0)
 *
 * Side Effects:
 *   - Sets char_position to 0 for all editing modes
 *   - Works consistently across To:, Subject:, and Body modes
 *
 * Notes:
 *   - Simple operation that works for all editing modes
 *   - Essential for quick beginning-of-line navigation
 *   - Commonly used in conjunction with em_toeol for line selection
 *   - Mapped to Ctrl-A key binding on Unix/Linux systems
 *   - Mapped to Ctrl-H key binding on VMS systems
 */
static int
em_tobol PARM_0(void)
{
  return(char_position = 0);
}

/*
 * em_insertchar - Insert space at specified position, shifting characters right
 *
 * Creates space for a new character by shifting all characters to the right
 * of the specified position. This is used in insert mode to make room for
 * new characters without overwriting existing content.
 *
 * Parameters:
 *   line - Character array to modify
 *   pos - Position where space should be inserted
 *   limit - Maximum line length to prevent buffer overflow
 *
 * Returns:
 *   None (void function)
 *
 * Side Effects:
 *   - Shifts characters from position 'pos' to 'limit-1' one position right
 *   - Inserts a space character at position 'pos'
 *   - Ensures null termination at position 'limit'
 *   - May truncate content if line is at maximum length
 *
 * Notes:
 *   - Used by email_addchar() when EM_S_INSERTON mode is active
 *   - Essential for insert mode functionality in subject and body editing
 *   - Performs bounds checking to prevent buffer overflow
 *   - Character at position 'limit-1' is lost if line is full
 */
static void
em_insertchar PARM_3(char *, line, int, pos, int, limit)
{
  int i;
  
  for (i = limit; i > pos; i--) {
    line[i] = line[i - 1];
  }
  line[i] = ' ';
  line[limit] = '\0';
}

/*
 * em_delchar - Delete character at specified position in current editing mode
 *
 * Removes a character from the current editing context (To:, Subject:, or Body)
 * and shifts remaining characters left to fill the gap. Handles different
 * data structures appropriately for each editing mode.
 *
 * Parameters:
 *   which - Position/index of character to delete (mode-dependent meaning)
 *
 * Returns:
 *   Character that was deleted, or '\0' if deletion invalid/failed
 *
 * Side Effects:
 *   - EM_S_TOLINE: Removes recipient and shifts remaining recipients left
 *   - EM_S_SUBJECT: Removes character and shifts subject string left  
 *   - EM_S_BODY: Removes character and shifts line data left
 *   - Calls email_unlock() for recipient removal in To: mode
 *   - Emits beep() for invalid deletion attempts
 *
 * Notes:
 *   - Mode-aware deletion handling for different data structures
 *   - In To: mode, 'which' is recipient index requiring lock management
 *   - In Subject/Body modes, 'which' is character position in string
 *   - Performs array shifting to maintain data integrity
 *   - Returns deleted character for potential undo operations
 */
static int
em_delchar PARM_1(int, which)
{
  int byechar = '\0', i;
  char *ch_ptr;

  /* check the mode */
  switch (email_status % 4) {
  case EM_S_TOLINE:
    /* delete within the toline */
    if ((which < EM_MAXLOCKS) &&
	(cur_message->to_whom[which] != ABSMAXNTN)) {

      /* remove the current lock */
      email_unlock(which);

      /* traverse and shift copy the target nations */
      byechar = 'a';
      cur_message->to_whom[which] = 0; /* email_unlock sets
					  this to ABSMAXNTN */
      for (i = which;
	   (i < EM_MAXLOCKS) &&
	   (cur_message->to_whom[i] != ABSMAXNTN);
	   i++) {
	cur_message->to_whom[i] = cur_message->to_whom[i + 1];
	em_locks[i] = em_locks[i + 1];
      }
      if (i < EM_MAXLOCKS) {
	cur_message->to_whom[i] = ABSMAXNTN;
	em_locks[i] = -1;
      }

    } else {
      /* yell at 'em */
      beep();
    }
    break;
  case EM_S_SUBJECT:
    /* delete within the subject line */
    if (which < strlen(cur_message->subj)) {

      /* traverse and shift copy */
      ch_ptr = &(cur_message->subj[which]);
      byechar = *ch_ptr;
      while (*ch_ptr != '\0') {
	*ch_ptr = *(ch_ptr + 1);
	ch_ptr++;
      }

    } else {
      /* yell at 'em */
      beep();
    }
    break;
  case EM_S_BODY:
    /* delete within the body */
    if ((cur_line != NULL) &&
	(which < strlen(cur_line->line_data))) {

      /* traverse and shift copy */
      ch_ptr = &(cur_line->line_data[which]);
      byechar = *ch_ptr;
      while (*ch_ptr != '\0') {
	*ch_ptr = *(ch_ptr + 1);
	ch_ptr++;
      }

    } else {
      /* yell at 'em */
      beep();
    }
    break;
  }
  return(byechar);
}

/*
 * em_delright - Delete character at current cursor position
 *
 * Removes the character under the cursor in the current editing mode.
 * Includes special handling for empty body lines that can trigger
 * mail sending when deleted.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Result of em_delchar() or em_exit() depending on context
 *
 * Side Effects:
 *   - Deletes character at char_position in current editing context
 *   - Special case: empty body line at position 0 with no next line calls em_exit()
 *   - May trigger mail sending and editor exit in specific conditions
 *
 * Notes:
 *   - Uses char_position as the deletion target for em_delchar()
 *   - Provides convenient "delete under cursor" functionality
 *   - Special exit behavior for empty final line in message body
 *   - Mapped to Ctrl-D key binding by default
 *   - Forward deletion complements em_delleft() for full editing control
 */
static int
em_delright PARM_0(void)
{
  if (((email_status % 4) == EM_S_BODY) &&
      (char_position == 0) &&
      (cur_line->line_data[0] == '\0') &&
      (cur_line->next == NULL)) {
    return(em_exit());
  }
  return(em_delchar(char_position));
}

/*
 * em_delleft - Delete character to the left of cursor (backspace functionality)
 *
 * Removes the character immediately to the left of the cursor position,
 * implementing standard backspace behavior. Includes bounds checking
 * and cursor position adjustment.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Result of em_delchar() if deletion successful, '\0' if invalid
 *
 * Side Effects:
 *   - Decrements char_position before deletion
 *   - Adjusts cursor position if beyond end of line
 *   - Emits beep() if deletion not possible (at beginning of line)
 *   - Calls em_delchar() with adjusted position
 *
 * Notes:
 *   - Implements standard backspace key functionality
 *   - Prevents deletion when cursor is at beginning of line (char_position 0)
 *   - Automatically adjusts cursor if positioned beyond line end
 *   - Mode-aware bounds checking for Subject and Body editing
 *   - Mapped to Ctrl-H key binding on Unix/Linux systems
 *   - Essential for text editing and error correction
 */
static int
em_delleft PARM_0(void)
{
  /* check if a deletion is possible */
  if (char_position > 0) {

    /* check for being beyond end */
    if (email_status % 4 == EM_S_BODY) {
      if (char_position > strlen(cur_line->line_data)) {
	char_position = strlen(cur_line->line_data);
      }
    } else if ((email_status % 4 == EM_S_SUBJECT) &&
	       char_position > strlen(cur_message->subj)) {
      char_position = strlen(cur_message->subj);
    }
    return(em_delchar(--char_position));
  }
  beep();
  return('\0');
}

/*
 * em_prekill - Delete all content from cursor position to beginning of line
 *
 * Removes all characters from the current cursor position back to the
 * beginning of the line, implementing "kill to beginning" functionality
 * common in Unix text editors.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 (always successful)
 *
 * Side Effects:
 *   - Repeatedly calls em_delleft() until char_position reaches 0
 *   - Removes all content to the left of the cursor
 *   - Leaves cursor at beginning of line (position 0)
 *
 * Notes:
 *   - Implements Unix-style "kill to beginning of line" functionality  
 *   - Uses em_delleft() in a loop for consistent deletion behavior
 *   - Works across all editing modes (To:, Subject:, Body)
 *   - Mapped to Ctrl-U key binding by default
 *   - Useful for quickly clearing content before cursor position
 *   - Complements em_kill() for full line editing control
 */
static int
em_prekill PARM_0(void)
{
  /* just delete 'em */
  while (char_position > 0) {
    em_delleft();
  }
  return(0);
}

/*
 * em_kill - Kill (delete) text from cursor to end of line
 *
 * Implements Unix-style "kill to end of line" functionality by deleting
 * all text from the current cursor position to the end of the current line.
 * The behavior is mode-aware and handles different data structures for each
 * editing mode.
 *
 * Mode-Specific Behavior:
 *   EM_S_TOLINE: Removes all recipients from current position to end
 *   EM_S_SUBJECT: Truncates subject line at cursor position
 *   EM_S_BODY: Truncates current line at cursor, or removes empty line
 *
 * Special Case Handling:
 *   - Body mode: If cursor is at position 0 on empty line with next line,
 *     removes the entire empty line and advances to next line
 *   - Properly manages linked list structure for line removal
 *   - Ensures proper memory cleanup for removed lines
 *
 * Returns:
 *   0 on success (always successful)
 *
 * Side Effects:
 *   - Modifies text content in current editing mode
 *   - May remove entire lines in body mode
 *   - Updates cur_line pointer when line is removed
 *   - Frees memory for removed line structures
 *   - May update recipient list and file locks
 *
 * Notes:
 *   - Complements em_prekill() for complete line editing functionality
 *   - Essential for efficient text editing operations
 *   - Handles boundary conditions and memory management gracefully
 */
static int
em_kill PARM_0(void)
{
  int i;

  switch (email_status % 4) {
  case EM_S_TOLINE:
    /* delete all to the right on the To: line */
    for (i = char_position;
	 i < EM_MAXLOCKS;
	 i++) {
      email_unlock(i);
    }
    break;
  case EM_S_SUBJECT:
    /* delete all to the right on the subject line */
    cur_message->subj[char_position] = '\0';
    break;
  case EM_S_BODY:
    /* do we kill the line? */
    if ((char_position == 0) &&
	(cur_line->line_data[0] == '\0') &&
	(cur_line->next != NULL)) {
      MAILD_PTR temp_ptr;

      /* remove the current line */
      temp_ptr = cur_line;
      if (cur_line->prev == NULL) {
	cur_message->text = cur_line->next;
      } else {
	cur_line->prev->next = cur_line->next;
      }
      cur_line = cur_line->next;
      cur_line->prev = temp_ptr->prev;

      /* now free it up */
      temp_ptr->prev = temp_ptr->next = NULL;
      free(temp_ptr->line_data);
      free(temp_ptr);

    } else {
      /* delete all on the current line to the right */
      cur_line->line_data[char_position] = '\0';
    }
    break;
  }
  return(0);
}

/*
 * em_up - Move cursor up one line or to previous editing mode
 *
 * Implements upward navigation in the mail editor with mode-aware behavior.
 * The function either moves up one line within the current mode or transitions
 * to the previous editing mode when at boundaries.
 *
 * Mode-Specific Behavior:
 *   EM_S_BODY: Moves to previous line in message body
 *     - Sets cur_line to previous line if available
 *     - If at first line, transitions to EM_S_SUBJECT mode
 *     - Resets char_position to 0 when changing modes
 *
 *   EM_S_SUBJECT: Transitions to EM_S_TOLINE (recipient editing)
 *     - Decrements email_status to move to previous mode
 *     - Resets char_position to 0 for beginning of recipient list
 *
 *   EM_S_TOLINE: Cannot move up (already at top)
 *     - Emits audible beep to indicate boundary condition
 *     - No state changes occur
 *
 * Returns:
 *   0 on success (always returns 0)
 *
 * Side Effects:
 *   - May change cur_line pointer in body mode
 *   - May change email_status (editing mode)
 *   - Resets char_position when changing modes
 *   - May emit beep for boundary conditions
 *
 * Notes:
 *   - Part of comprehensive cursor navigation system
 *   - Coordinates with em_down() for bidirectional movement
 *   - Essential for multi-modal editing interface
 *   - Handles mode transitions smoothly
 */
static int
em_up PARM_0(void)
{
  /* inside the body? */
  if ((email_status % 4) == EM_S_BODY) {

    /* bounce around */
    cur_line = cur_line->prev;
    if (cur_line == NULL) {
      email_status--;
      char_position = 0;
    }

  } else {

    if ((email_status % 4) > EM_S_TOLINE) {

      /* up to the next stage */
      email_status--;
      char_position = 0;

    } else {

      /* can't go anywhere */
      beep();

    }
  }
  return(0);
}

/*
 * em_down - Move cursor down one line or to next editing mode
 *
 * Implements downward navigation in the mail editor with mode-aware behavior.
 * The function either moves down one line within the current mode or transitions
 * to the next editing mode when at boundaries.
 *
 * Mode-Specific Behavior:
 *   EM_S_BODY: Moves to next line in message body
 *     - Advances cur_line to next line if available
 *     - If at last line, emits beep to indicate boundary
 *     - No mode changes occur in body mode
 *
 *   EM_S_TOLINE/EM_S_SUBJECT: Transitions to next editing mode
 *     - Increments email_status to advance to next mode
 *     - When entering EM_S_BODY, sets cur_line to first text line
 *     - Resets char_position to 0 for new mode positioning
 *
 * Special Handling:
 *   - When transitioning into body mode, properly initializes cur_line
 *   - Ensures cur_line points to cur_message->text for body editing
 *   - Maintains consistent cursor positioning across mode changes
 *
 * Returns:
 *   0 on success (always returns 0)
 *
 * Side Effects:
 *   - May change cur_line pointer when entering body mode
 *   - May change email_status (editing mode)
 *   - Resets char_position when changing modes
 *   - May emit beep for boundary conditions in body mode
 *
 * Notes:
 *   - Part of comprehensive cursor navigation system
 *   - Coordinates with em_up() for bidirectional movement
 *   - Essential for sequential editing workflow
 *   - Handles mode initialization properly
 */
static int
em_down PARM_0(void)
{
  /* are we within the body */
  if ((email_status % 4) == EM_S_BODY) {

    /* just go forward */
    if (cur_line->next != NULL) {
      cur_line = cur_line->next;
    } else {
      beep();
    }

  } else {

    /* on to the next stage */
    email_status++;
    if ((email_status % 4) == EM_S_BODY) {
      cur_line = cur_message->text;
    } else {
      cur_line = NULL;
    }
    char_position = 0;

  }
  return(0);
}

/*
 * em_forward - Move cursor forward one character with intelligent navigation
 *
 * Implements forward character navigation in the mail editor with intelligent
 * boundary handling. When reaching the end of content in any mode, automatically
 * transitions to the next logical editing position.
 *
 * Mode-Specific Behavior:
 *   EM_S_TOLINE: Navigate through recipient list
 *     - Advances through recipient array until EM_MAXLOCKS or ABSMAXNTN
 *     - When reaching end of recipients, calls em_down() and em_tobol()
 *     - Seamlessly transitions to subject line editing
 *
 *   EM_S_SUBJECT: Navigate through subject line text
 *     - Advances through subject string characters
 *     - When reaching end of subject, calls em_down() and em_tobol()
 *     - Seamlessly transitions to message body editing
 *
 *   EM_S_BODY: Navigate through message body text
 *     - Advances through current line characters
 *     - When reaching end of line, attempts to move to next line
 *     - If no next line available, emits beep for boundary condition
 *
 * Intelligent Transitions:
 *   - Automatically moves to next editing mode when appropriate
 *   - Positions cursor at beginning of new content area
 *   - Maintains user workflow continuity across mode boundaries
 *
 * Returns:
 *   0 on success (always returns 0)
 *
 * Side Effects:
 *   - May change char_position for normal forward movement
 *   - May change email_status through em_down() calls
 *   - May change cur_line through em_down() calls
 *   - May emit beep for boundary conditions
 *   - Calls em_tobol() for proper positioning after transitions
 *
 * Notes:
 *   - Part of comprehensive cursor navigation system
 *   - Coordinates with em_backward() for bidirectional movement
 *   - Essential for natural text editing flow
 *   - Enhances user experience with smart transitions
 */
static int
em_forward PARM_0(void)
{
  /* dependent on where we are */
  switch (email_status % 4) {
  case EM_S_TOLINE:
    /* traverse or go to the subject line */
    if ((char_position < EM_MAXLOCKS) &&
	(cur_message->to_whom[char_position] != ABSMAXNTN)) {
      char_position++;
    } else {
      em_down();
      em_tobol();
    }
    break;
  case EM_S_SUBJECT:
    /* traverse or go to the first body line */
    if (char_position < strlen(cur_message->subj)) {
      char_position++;
    } else {
      em_down();
      em_tobol();
    }
    break;
  case EM_S_BODY:
    /* traverse or go the next following line */
    if (char_position < strlen(cur_line->line_data)) {
      char_position++;
    } else if (cur_line->next != NULL) {
      em_down();
      em_tobol();
    } else {
      beep();
    }
    break;
  }
  return(0);
}

/*
 * em_backward - Move cursor backward one character with intelligent navigation
 *
 * Implements backward character navigation in the mail editor with intelligent
 * boundary handling. When reaching the beginning of content in any mode,
 * automatically transitions to the previous logical editing position.
 *
 * Mode-Specific Behavior:
 *   EM_S_TOLINE: Navigate backward through recipient list
 *     - Moves backward through recipient positions
 *     - Handles boundary conditions for invalid/empty recipients
 *     - When at position 0, emits beep (cannot go further back)
 *
 *   EM_S_SUBJECT: Navigate backward through subject line text
 *     - Moves backward through subject characters
 *     - Handles cursor beyond string length gracefully
 *     - When at position 0, calls em_up() and em_toeol()
 *     - Seamlessly transitions to end of recipient list
 *
 *   EM_S_BODY: Navigate backward through message body text
 *     - Moves backward through current line characters
 *     - Handles cursor beyond line length gracefully
 *     - When at position 0, calls em_up() and em_toeol()
 *     - Seamlessly transitions to end of subject line
 *
 * Intelligent Cursor Handling:
 *   - Automatically corrects cursor position if beyond string/line end
 *   - Ensures cursor remains within valid character bounds
 *   - Provides smooth transitions between editing modes
 *
 * Returns:
 *   0 on success (always returns 0)
 *
 * Side Effects:
 *   - May change char_position for normal backward movement
 *   - May change email_status through em_up() calls
 *   - May change cur_line through em_up() calls
 *   - May emit beep for boundary conditions
 *   - Calls em_toeol() for proper positioning after transitions
 *   - Automatically corrects invalid cursor positions
 *
 * Notes:
 *   - Part of comprehensive cursor navigation system
 *   - Coordinates with em_forward() for bidirectional movement
 *   - Essential for natural text editing flow
 *   - Handles edge cases and invalid positions gracefully
 */
static int
em_backward PARM_0(void)
{
  switch(email_status % 4) {
  case EM_S_TOLINE:
    /* go back along it */
    if (char_position > 0) {
      if ((char_position >= EM_MAXLOCKS) ||
	  (cur_message->to_whom[char_position] == ABSMAXNTN)) {
	em_toeol();
      }
      char_position--;
    } else {
      /* woops */
      beep();
    }
    break;
  case EM_S_SUBJECT:
    /* go back along the subject */
    if (char_position > 0) {
      if (char_position > strlen(cur_message->subj)) {
	char_position = strlen(cur_message->subj);
      }
      char_position--;
    } else {
      /* go to the To: line */
      em_up();
      em_toeol();
    }
    break;
  case EM_S_BODY:
    /* simply travel backwards */
    if (char_position > 0) {
      if (char_position > strlen(cur_line->line_data)) {
	char_position = strlen(cur_line->line_data);
      }
      char_position--;
    } else {
      /* go to the subject line */
      em_up();
      em_toeol();
    }
    break;
  }
  return(0);
}

/*
 * em_newline - Create new line or advance to next editing mode
 *
 * Implements newline functionality in the mail editor with mode-aware behavior.
 * In recipient and subject modes, advances to the next editing mode. In body mode,
 * creates a new line by splitting the current line at the cursor position.
 *
 * Mode-Specific Behavior:
 *   EM_S_TOLINE/EM_S_SUBJECT: Simple mode advancement
 *     - Calls em_down() to advance to next editing mode
 *     - No line creation or text manipulation
 *
 *   EM_S_BODY: Complex line splitting and creation
 *     - Validates and corrects cursor position if beyond line end
 *     - Allocates memory for new line structure and data
 *     - Splits current line content at cursor position
 *     - Updates linked list pointers for proper insertion
 *     - Positions cursor at beginning of new line
 *
 * Memory Management:
 *   - Allocates MAILD_STRUCT for new line node
 *   - Allocates character buffer based on email_collim
 *   - Handles allocation errors with appropriate error messages
 *   - Calls abrt() on memory allocation failure (fatal error)
 *
 * Linked List Maintenance:
 *   - Properly inserts new line into doubly-linked list
 *   - Updates next/prev pointers for current and new lines
 *   - Handles both middle insertion and end insertion cases
 *   - Maintains list integrity throughout operation
 *
 * Returns:
 *   0 on success (always returns 0, aborts on memory error)
 *
 * Side Effects:
 *   - May call em_down() for mode transitions
 *   - Allocates memory for new line structures in body mode
 *   - Modifies linked list structure in body mode
 *   - Updates cur_line to point to newly created line
 *   - Resets char_position to 0 after line creation
 *   - May call abrt() on memory allocation failure
 *
 * Notes:
 *   - Essential for text editing functionality in body mode
 *   - Provides natural line break behavior
 *   - Handles memory management with proper error checking
 *   - Maintains data structure integrity
 */
static int
em_newline PARM_0(void)
{
  MAILD_PTR temp_ptr;

  /* dependent on where we are */
  switch (email_status % 4) {
  case EM_S_TOLINE:
  case EM_S_SUBJECT:
    /* just go the the next line */
    em_down();
    break;
  case EM_S_BODY:
    /* check for past end of line */
    if (char_position > strlen(cur_line->line_data)) {
      char_position = strlen(cur_line->line_data);
    }

    /* now allocate space for a new line of data */
    if ((temp_ptr =
	 (MAILD_PTR) malloc(sizeof(MAILD_STRUCT))) == NULL) {
      errormsg("Major Error:  Memory Allocation Error");
      abrt();
    }
    if ((temp_ptr->line_data =
	 (char *) malloc(sizeof(char) * (email_collim + 1))) == NULL) {
      errormsg("Major Error:  Memory allocation error");
      abrt();
    }

    /* place it within the list */
    temp_ptr->next = cur_line->next;
    temp_ptr->prev = cur_line;
    cur_line->next = temp_ptr;
    if (temp_ptr->next != NULL) {
      temp_ptr->next->prev = temp_ptr;
    }

    /* copy it */
    strcpy(cur_line->next->line_data,
	   &(cur_line->line_data[char_position]));

    /* now adjust location */
    cur_line->line_data[char_position] = '\0';
    cur_line = cur_line->next;
    char_position = 0;
    break;
  }
  return(0);
}

/*
 * email_addchar - Add character to current editing position with mode-aware processing
 *
 * Implements comprehensive character input processing for the mail editor with
 * mode-specific behavior and intelligent input handling. This is the central
 * character processing function that handles all text input across different
 * editing modes.
 *
 * Mode-Specific Behavior:
 *   EM_S_TOLINE: Interactive recipient name input
 *     - Limits recipients to EM_MAXLOCKS maximum
 *     - Handles space/comma as name completion triggers
 *     - Calls get_country() for interactive nation selection
 *     - Validates nation existence and handles errors
 *     - Manages file locking for valid recipients
 *
 *   EM_S_SUBJECT: Subject line text editing
 *     - Respects email_subjlim character limit
 *     - Handles tab expansion to spaces (8-character stops)
 *     - Supports both insert and overwrite modes
 *     - Filters non-printable characters with beep feedback
 *     - Manages string termination for newly added characters
 *
 *   EM_S_BODY: Message body text editing
 *     - Respects email_collim character limit per line
 *     - Handles tab expansion to spaces (8-character stops)
 *     - Supports both insert and overwrite modes
 *     - Filters non-printable characters with beep feedback
 *     - Manages string termination for newly added characters
 *
 * Insert/Overwrite Mode Handling:
 *   - Checks EM_S_INSERTON flag in email_status
 *   - Insert mode: Calls em_insertchar() to shift existing content
 *   - Overwrite mode: Directly replaces character at cursor position
 *   - Handles end-of-line detection for proper insertion behavior
 *
 * Character Processing:
 *   - Tab characters: Expands to spaces at 8-character boundaries
 *   - Space characters: Handled specially for proper spacing
 *   - Printable characters: Added directly to content
 *   - Non-printable/invalid: Rejected with beep feedback
 *
 * Parameters:
 *   inp_ch - Input character to process and add
 *
 * Side Effects:
 *   - May modify recipient list and file locks (EM_S_TOLINE)
 *   - May modify subject string content (EM_S_SUBJECT)
 *   - May modify current line content (EM_S_BODY)
 *   - Updates char_position for successful character additions
 *   - May call em_insertchar() for insert mode operations
 *   - May call get_country() for interactive input
 *   - May emit beep for invalid input or limit violations
 *   - May display error messages for invalid nations
 *
 * Notes:
 *   - Central input processing function for all modes
 *   - Handles complex recipient selection workflow
 *   - Supports both insert and overwrite editing paradigms
 *   - Essential for all text input functionality
 *   - Maintains data integrity across all editing modes
 */
static void
email_addchar PARM_1(int, inp_ch)
{
  int i, j, was_eol = FALSE;

  switch (email_status % 4) {
  case EM_S_TOLINE:
    /* check the limit */
    if (char_position >= EM_MAXLOCKS) {
      errormsg("You may not send to any more nations");
      break;
    } else if (em_locks[char_position] != -1) {
      errormsg("You may only add nations to the end of the list");
      break;
    }

    /* check the type of character */
    switch (inp_ch) {
    case ' ':
    case ',':
      /* simply go and get the new name */
      break;
    default:
      /* just add it in */
      push_char(inp_ch);
      break;
    }

    /* find the current location and give a quick prompt */
    if (char_position > 0) addstr(", ");
    getyx(stdscr, j, i);

    /* display the bottom prompt line */
    errorbar("Mail Editor", "  Hit [RETURN] when done");

    /* now get the input */
    move(j, i);
    refresh();
    if ((i = get_country(TRUE)) == MAXNTN) {
      if (no_input == FALSE) {
	if (global_int == TRUE) next_char();
	errormsg("That nation does not seem to exist");
      }
      break;
    }

    /* now add it */
    if (email_lock(char_position, i) == TRUE) {
      char_position++;
    }
    break;
  case EM_S_SUBJECT:
    if (char_position > strlen(cur_message->subj)) {
      char_position = strlen(cur_message->subj);
    }
    /* check the limit */
    if (char_position >= email_subjlim) {
      beep();
      break;
    }

    /* check the boundary */
    if (cur_message->subj[char_position] == '\0') {
      was_eol = TRUE;
    }

    /* add to the line in the body */
    if (inp_ch == '\t') {
      do {
	if (cur_message->subj[char_position] == '\0') {
	  was_eol = TRUE;
	}
	if (email_status & EM_S_INSERTON && !was_eol) {
	  em_insertchar(cur_message->subj, char_position, email_subjlim);
	}
	cur_message->subj[char_position++] = ' ';
      } while ((char_position < email_subjlim) &&
	       ((char_position % 8) != 0));
    } else if (inp_ch == ' ') {
      if (email_status & EM_S_INSERTON && !was_eol) {
	em_insertchar(cur_message->subj, char_position, email_subjlim);
      }
      cur_message->subj[char_position++] = ' ';
    } else if (isprint(inp_ch) &&
	       !isspace(inp_ch)) {
      if (email_status & EM_S_INSERTON && !was_eol) {
	em_insertchar(cur_message->subj, char_position, email_subjlim);
      }
      cur_message->subj[char_position++] = inp_ch;
    } else {
      /* ain't a good character */
      beep();
      break;
    }

    /* check if that was the end of the line */
    if (was_eol == TRUE) {
      cur_message->subj[char_position] = '\0';
    }
    break;
  case EM_S_BODY:
    if (char_position > strlen(cur_line->line_data)) {
      char_position = strlen(cur_line->line_data);
    }
    /* check the limit */
    if (char_position >= email_collim) {
      beep();
      break;
    }

    /* check the boundary */
    if (cur_line->line_data[char_position] == '\0') {
      was_eol = TRUE;
    }

    /* add to the line in the body */
    if (inp_ch == '\t') {
      do {
	if (cur_line->line_data[char_position] == '\0') {
	  was_eol = TRUE;
	}
	if (email_status & EM_S_INSERTON && !was_eol) {
	  em_insertchar(cur_line->line_data, char_position, email_collim);
	}
	cur_line->line_data[char_position++] = ' ';
      } while ((char_position < email_collim) &&
	       ((char_position % 8) != 0));
    } else if (inp_ch == ' ') {
      if (email_status & EM_S_INSERTON && !was_eol) {
	em_insertchar(cur_line->line_data, char_position, email_collim);
      }
      cur_line->line_data[char_position++] = ' ';
    } else if (isprint(inp_ch) &&
	       !isspace(inp_ch)) {
      if (email_status & EM_S_INSERTON && !was_eol) {
	em_insertchar(cur_line->line_data, char_position, email_collim);
      }
      cur_line->line_data[char_position++] = inp_ch;
    } else {
      /* ain't a good character */
      beep();
      break;
    }

    /* check if that was the end of the line */
    if (was_eol == TRUE) {
      cur_line->line_data[char_position] = '\0';
    }
    break;
  }
}

/*
 * em_help - Display mail editor help documentation
 *
 * Provides comprehensive help documentation for the mail editor by creating
 * an interactive help system. Displays all available mail editor commands,
 * their key bindings, and descriptions in a user-friendly format.
 *
 * Help System Integration:
 *   - Uses create_help() function from the game's help system
 *   - Displays "Conquer Mail Editor Command List" as the title
 *   - Shows email_bindings key binding mappings
 *   - Shows email_funcs function descriptions
 *   - Uses email_keysys.num_parse for the number of commands
 *
 * Documentation Coverage:
 *   - All mail editor navigation commands
 *   - Text editing and manipulation functions
 *   - Mode switching and control operations
 *   - Send/quit/exit functionality
 *   - Insert/overwrite mode operations
 *   - Line editing capabilities
 *
 * User Experience:
 *   - Provides contextual help within the mail editor
 *   - Allows users to learn commands without leaving the editor
 *   - Essential for discoverability of mail editor features
 *   - Supports both novice and experienced users
 *
 * Returns:
 *   0 on success (always returns 0)
 *
 * Side Effects:
 *   - Temporarily displays help screen overlay
 *   - May clear/modify screen display during help presentation
 *   - Returns to mail editor after help is dismissed
 *   - No permanent state changes to mail editor
 *
 * Notes:
 *   - Essential for user accessibility and learning
 *   - Integrates with game's standard help system
 *   - Provides complete command reference
 *   - Supports user workflow without interruption
 */
static int
em_help PARM_0(void)
{
  /* create the help system */
  create_help("Conquer Mail Editor Command List",
	      email_bindings, email_funcs, email_keysys.num_parse);
  return(0);
}

/*
 * ems_subj - Display subject line at specified screen position
 *
 * Renders the subject line of the current mail message at the specified
 * screen line with proper formatting and width constraints. This is a
 * display helper function used by email_show() to present the subject
 * line consistently.
 *
 * Display Formatting:
 *   - Prefixes subject with "Subject: " label
 *   - Constrains display width to COLS - 9 characters
 *   - Uses mvprintw() for precise screen positioning
 *   - Handles subject line truncation if too long for screen
 *
 * Screen Integration:
 *   - Positions output at specified line, column 0
 *   - Respects terminal width limitations
 *   - Provides consistent formatting across different terminal sizes
 *   - Part of the overall mail message display system
 *
 * Parameters:
 *   line - Screen line number where subject should be displayed
 *
 * Side Effects:
 *   - Modifies screen content at specified line
 *   - May truncate long subject lines to fit screen width
 *   - Updates cursor position to end of displayed text
 *
 * Notes:
 *   - Used exclusively by email_show() for display consistency
 *   - Handles terminal width variations gracefully
 *   - Essential for mail editor screen layout
 *   - Maintains consistent "Subject: " labeling
 */
static void
ems_subj PARM_1(int, line)
{
  mvprintw(line, 0, "Subject: %.*s", COLS - 9, cur_message->subj);
}

/*
 * email_show - Display complete mail message with mode-aware cursor positioning
 *
 * Implements comprehensive mail message display with intelligent layout and
 * cursor positioning. This is the primary display function for the mail editor,
 * handling all visual aspects of message presentation including recipients,
 * subject, and body content.
 *
 * Display Components:
 *   To: Line Construction:
 *     - Builds formatted recipient list with nation names
 *     - Handles line wrapping for long recipient lists
 *     - Manages cursor positioning within recipient display
 *     - Shows continuation lines with proper indentation
 *
 *   Subject Line Display:
 *     - Uses ems_subj() for consistent formatting
 *     - Calculates cursor position for subject editing mode
 *     - Integrates with overall message layout
 *
 *   Message Body Display:
 *     - Renders linked list of message lines
 *     - Handles scrolling for large messages ("...cont..." / "...more...")
 *     - Calculates optimal display window based on screen size
 *     - Maintains cursor position tracking for body editing
 *
 * Cursor Position Management:
 *   - Calculates exact screen coordinates (xpos, ypos) for cursor
 *   - Mode-aware positioning logic:
 *     * EM_S_TOLINE: Position within recipient list display
 *     * EM_S_SUBJECT: Position within subject line (offset by 9)
 *     * EM_S_BODY: Position within message body content
 *   - Handles line wrapping and multi-line displays
 *
 * Screen Layout Optimization:
 *   - Dynamically calculates available screen space
 *   - Adjusts content display based on terminal size
 *   - Implements intelligent scrolling for large content
 *   - Reserves space for status/error lines at bottom
 *
 * Interactive Elements:
 *   - Shows context-appropriate prompts and instructions
 *   - Displays help hint for key bindings ("ESC-?" for Bindings)
 *   - Provides mode-specific user guidance
 *   - Maintains consistent error bar usage
 *
 * Returns:
 *   0 on success (always returns 0)
 *
 * Side Effects:
 *   - Completely redraws mail editor screen
 *   - Updates cursor position to calculated coordinates
 *   - Calls refresh() to update display
 *   - May display scrolling indicators for large content
 *   - Updates error bar with context information
 *
 * Notes:
 *   - Central display function called from main editing loop
 *   - Essential for all visual feedback in mail editor
 *   - Handles complex layout calculations efficiently
 *   - Provides seamless user experience across all modes
 *   - Critical for editor usability and navigation
 */
static int
email_show PARM_0(void)
{
  char tostr[(NAMELTH + 3) * EM_MAXLOCKS + 10], nstr[NAMELTH + 1];
  int tolines = 1, base;
  int xpos = 0, ypos = 0, count1, count2;
  MAILD_PTR travel_line;

  /* build the To: line? or simply display it? */
  base = email_status % 4;

  /* build/display the To: line */
  count1 = 4;
  strcpy(tostr, "To: ");
  for (count2 = 0; count2 < EM_MAXLOCKS; count2++) {

    /* check for the end of it */
    if (cur_message->to_whom[count2] == ABSMAXNTN) {
      if ((base == EM_S_TOLINE) &&
	  (count2 == char_position)) {
	xpos = count1;
	ypos = tolines - 1;
      }
      break;
    }

    /* add it on */
    if (count2 != 0) {
      strcat(tostr, ", ");
      count1 += 2;
    }
    (void) ntn_realname(&(nstr[0]), cur_message->to_whom[count2]);
    if (count1 + strlen(nstr) + 2 > COLS - 8) {
      mvaddstr(tolines - 1, 0, tostr);
      strcpy(tostr, "    ");
      count1 = 4;
      tolines++;
    }

    /* check positioning */
    if ((base == EM_S_TOLINE) &&
	(count2 == char_position)) {
      xpos = count1;
      ypos = tolines - 1;
    }

    /* now tag it on */
    strcat(tostr, nstr);
    count1 += strlen(nstr);
  }

  /* show the bottom To: line */
  mvaddstr(tolines - 1, 0, tostr);
  if (base == EM_S_TOLINE) {
    mvaddstr(LINES - 1, 0,
  "Enter a nation, \"god\" (to mail deity), or \"news\" (to send a personal)");
  }

  /* now handle the subject line */
  if (base == EM_S_SUBJECT) {
    xpos = char_position + 9;
    ypos = tolines;
  }
  ems_subj(tolines);

  /* now prepare the body */
  if (base != EM_S_BODY) {

    /* setup the body */
    travel_line = cur_message->text;

  } else {

    /* find the length of the body upto and including current line */
    count1 = 1;
    for (travel_line = cur_line;
	 travel_line->prev != NULL;
	 travel_line = travel_line->prev) {
      count1++;
    }

    /* may only fit so many lines */
    if (count1 > LINES - 7 - tolines) {

      /* indicate that there is more to it */
      mvaddstr(tolines + 1, 0, "...cont...");

      /* only allow so many lines */
      count1 %= (LINES - 7 - tolines);
      count1++;
      for (travel_line = cur_line;
	   travel_line->prev != NULL;
	   travel_line = travel_line->prev) {
	if (--count1 == 0) break;
      }

    }

  }

  /* now show the body */
  count2 = tolines + 2;
  for (; travel_line != NULL; travel_line = travel_line->next) {
    if ((base == EM_S_BODY) &&
	(travel_line == cur_line)) {
      xpos = min(char_position, strlen(travel_line->line_data));
      ypos = count2;
    }
    mvaddstr(count2++, 0, travel_line->line_data);
    if (count2 == LINES - 5) break;
  }
  if ((travel_line != NULL) &&
      (travel_line->next != NULL)) {
    mvaddstr(count2, 0, "...more...");
  }

  /* display the bottom prompt line */
  errorbar("Mail Editor", "Hit \"ESC-?\" for Bindings");

  /* now relocate the cursor */
  move(ypos, xpos);
  refresh();
  return(0);
}

/*
 * email_prep - Initialize mail message structure and acquire necessary resources
 *
 * Performs comprehensive initialization of the mail message structure and
 * acquires all necessary resources for mail editing. This function ensures
 * that all components of the mail editor are properly initialized before
 * editing begins.
 *
 * Recipient Management:
 *   - Initializes em_locks[] array to -1 (unlocked state)
 *   - Processes existing recipients in cur_message->to_whom[]
 *   - Attempts to acquire file locks for each existing recipient
 *   - Removes recipients that cannot be locked (sets to ABSMAXNTN)
 *   - Ensures multi-user safety through proper file locking
 *
 * Sender Information Setup:
 *   - Allocates and initializes sender name field
 *   - Sets sender to current nation name (nationname)
 *   - Handles memory allocation errors with appropriate error messages
 *   - Essential for message attribution and delivery
 *
 * Nickname Assignment:
 *   - Allocates and initializes nickname field
 *   - Special handling for deity (UNOWNED country):
 *     * Randomly selects from: "The All Knowing", "The All Mighty", "The All Powerful"
 *     * Provides mystical persona for system messages
 *   - For normal nations: Uses nation leader name (ntn_ptr->leader)
 *   - Enhances message personalization and game immersion
 *
 * Subject Line Initialization:
 *   - Allocates subject buffer based on email_subjlim
 *   - Initializes to empty string for new messages
 *   - Preserves existing subject content for message editing
 *   - Ensures proper memory allocation and bounds
 *
 * Message Body Structure:
 *   - Creates initial text line structure if none exists
 *   - Allocates MAILD_STRUCT for linked list management
 *   - Allocates character buffer based on email_collim
 *   - Initializes empty first line for new messages
 *   - Sets up proper doubly-linked list structure
 *
 * Key Binding System:
 *   - Ensures email_bindings is initialized
 *   - Calls email_init() if bindings are not yet set up
 *   - Essential for proper mail editor command processing
 *
 * Error Handling:
 *   - Comprehensive memory allocation error checking
 *   - Calls abrt() on critical memory allocation failures
 *   - Ensures system stability under low memory conditions
 *   - Provides appropriate error messages for user feedback
 *
 * Side Effects:
 *   - Allocates memory for message components
 *   - Acquires file locks for recipient mail files
 *   - Modifies cur_message structure extensively
 *   - May call abrt() on memory allocation failure
 *   - May call email_init() for key binding setup
 *   - Sets global_int to FALSE for proper nation input handling
 *
 * Notes:
 *   - Must be called before any mail editing operations
 *   - Essential for proper mail editor initialization
 *   - Handles both new message creation and existing message editing
 *   - Critical for multi-user safety and data integrity
 *   - Provides foundation for all mail editor functionality
 */
static void
email_prep PARM_0(void)
{
  int i;

  /* who is it to? */
  global_int = FALSE;
  for (i = 0; i < EM_MAXLOCKS; i++) {
    em_locks[i] = -1;
    if (cur_message->to_whom[i] != ABSMAXNTN) {
      if (email_lock(i, cur_message->to_whom[i]) != TRUE) {
	cur_message->to_whom[i] = ABSMAXNTN;
      }
    }
  }

  /* who is it???? */
  if (cur_message->sender == NULL) {
    /* allocate the space */
    if ((cur_message->sender =
	 (char *) malloc((NAMELTH + 1) * sizeof(char))) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    strcpy(cur_message->sender, nationname);
  }
  if (cur_message->nickname == NULL) {
    /* allocate the space */
    if ((cur_message->nickname =
	 (char *) malloc(LINELTH * sizeof(char))) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    if (country == UNOWNED) {
      switch (rand_val(3)) {
      case 0:
	strcpy(cur_message->nickname, "The All Knowing");
      case 1:
	strcpy(cur_message->nickname, "The All Mighty");
      case 2:
	strcpy(cur_message->nickname, "The All Powerful");
	break;
      }
    } else {
      strcpy(cur_message->nickname, ntn_ptr->leader);
    }
  }

  /* check what it is about */
  if (cur_message->subj == NULL) {

    /* allocate the space */
    if ((cur_message->subj =
	 (char *) malloc((email_subjlim + 1) * sizeof(char))) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    cur_message->subj[0] = '\0';

  }

  /* check if there is any text yet */
  if (cur_message->text == NULL) {

    /* allocate it */
    if ((cur_message->text =
	 (MAILD_PTR) malloc( sizeof(MAILD_STRUCT) )) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    if ((cur_message->text->line_data =
	 (char *) malloc( (email_collim + 1) * sizeof(char) )) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    cur_message->text->line_data[0] = '\0';
    cur_message->text->prev = NULL;
    cur_message->text->next = NULL;

  }

  /* assign bindings */
  if (email_bindings == NULL) {
    email_init();
  }
}

#ifdef ALLOW_EDIT_FORK
/* EMAIL_TOEDIT -- Write the current mail message to the given file */
static int
email_toedit PARM_1(char *, fname)
{
  FILE *fpout;
  char temp_str[BIGLTH];
  MAILD_PTR travel_line;
  int i;

  /* open the output file */
  if ((fpout = fopen(fname, "w")) == NULL) {
    /* oops */
    errormsg("Error: Unable to open the temporary file for editing");
    return(TRUE);
  }

  /* write out the header */
  fprintf(fpout, "To: ");
  for (i = 0; i < EM_MAXLOCKS; i++) {
    if (cur_message->to_whom[i] == ABSMAXNTN) break;
    if (i > 0) fprintf(fpout, ", ");
    (void) ntn_realname(&(temp_str[0]), cur_message->to_whom[i]);
    fprintf(fpout, temp_str);
  }
  fprintf(fpout, "\nSubject: %s\n\n", cur_message->subj);

  /* now show the body */
  for (travel_line = cur_message->text;
       travel_line != NULL;
       travel_line = travel_line->next) {
    fprintf(fpout, "%s\n", travel_line->line_data);
  }

  /* finish it */
  fclose(fpout);
  return(FALSE);
}

/* EMAIL_FROMEDIT -- Read the current mail message from the given file */
static void
email_fromedit PARM_1(char *, fname)
{
  FILE *fpin;
  MAILD_PTR travel_ptr, placement_ptr;
  int i, j, numtos = 0, lval, first_blank = FALSE;
  int found_to = FALSE, found_subj = FALSE;
  char line[BIGLTH], nname[LINELTH];

  /* open the file */
  if ((fpin = fopen(fname, "r")) == NULL) return;

  /* clear up the old space */
  cur_message->subj[0] = '0';
  for (i = 0; i < EM_MAXLOCKS; i++) {
    if (cur_message->to_whom[i] != ABSMAXNTN) {
      email_unlock(i);
    }
  }
  placement_ptr = cur_message->text;
  for (travel_ptr = placement_ptr;
       travel_ptr != NULL;
       travel_ptr = placement_ptr) {
    placement_ptr = placement_ptr->next;
    travel_ptr->next = NULL;
    if (travel_ptr->line_data != NULL) {
      free(travel_ptr->line_data);
    }
    free(travel_ptr);
  }

  /* read all of the data, line by line */
  while (!feof(fpin)) {

    /* read in a line of data */
    if (fgets(line, BIGLTH - 1, fpin) == NULL) break;
  more_to_line:
    lval = strlen(line);

    /* look for the To: field */
    if ((found_to == FALSE) &&
	(strncmp(line, "To:", 3) == 0)) {

      /* parse through all of the line */
      if (line[lval - 1] == '\n') {
	line[lval - 1] = '\0';
      }
      found_to = TRUE;
      j = 0;
      for (i = 3; i < lval; i++) {
	/* look for a name portion */
	if (isalnum(line[i])) {
	  nname[j++] = line[i];
	} else {
	  /* is there a name up to here? */
	  if (j == 0) continue;
	  nname[j] = '\0';
	  if ((ntnbyname(nname) == NULL) &&
	      (global_int == MAXNTN)) {
	    sprintf(string, "Unknown nation name <%s> encountered", nname);
	    errormsg(string);
	    continue;
	  }
	  if (numtos < EM_MAXLOCKS) {
	    cur_message->to_whom[numtos] = global_int;
	    email_lock(numtos, global_int);
	    numtos++;
	  }
	  nname[j = 0] = '\0';

	}
      }

    } else if ((found_subj == FALSE) &&
	       (strncmp(line, "Subject:", 8) == 0)) {

      /* get the subject line */
      if (line[lval - 1] == '\n') {
	line[lval - 1] = '\0';
      }
      trim_str(&(line[8]), FALSE);
      strncpy(cur_message->subj, &(line[8]), email_subjlim);
      cur_message->subj[email_subjlim] = '\0';
      found_subj = TRUE;

    } else {

      /* was the first line blank? */
      if (line[lval - 1] == '\n') {
	line[lval - 1] = '\0';
      }
      lval--;
      if (first_blank == FALSE) {
	first_blank = TRUE;
	if (line[0] == '\0') continue;
      }

      /* make the storage and copy the text */
      if ((travel_ptr = (MAILD_PTR) malloc( sizeof(MAILD_STRUCT) )) == NULL) {
	errormsg("MAJOR ERROR: Memory Allocation Error");
	abrt();
      }
      if ((travel_ptr->line_data =
	   (char *) malloc( (email_collim + 1) * sizeof(char) )) == NULL) {
	errormsg("MAJOR ERROR: Memory Allocation Error");
	abrt();
      }
      strncpy(travel_ptr->line_data, line, email_collim);
      travel_ptr->line_data[email_collim] = '\0';
      travel_ptr->next = NULL;

      /* now place it */
      if (placement_ptr == NULL) {
	cur_message->text = travel_ptr;
      } else {
	placement_ptr->next = travel_ptr;
      }
      travel_ptr->prev = placement_ptr;
      placement_ptr = travel_ptr;

      /* check if there is more to this line */
      if (lval > email_collim) {
	for (i = 0; i < email_collim; i++) {
	  line[i] = ' ';
	}
	trim_str(line, FALSE);
	goto more_to_line;
      }

    }
    
  }

  /* finish up */
  if ((cur_line = cur_message->text) == NULL) {
    if ((cur_line = (MAILD_PTR) malloc( sizeof(MAILD_STRUCT) )) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    if ((cur_line->line_data =
	 (char *) malloc( (email_collim + 1) * sizeof(char) )) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    cur_line->line_data[0] = '\0';
    cur_line->next = NULL;
    cur_line->prev = NULL;
    cur_message->text = cur_line;
  }
  char_position = 0;
  fclose(fpin);
  unlink(fname);
  clear();
}
#endif /* ALLOW_EDIT_FORK */

/* EM_SPAWNEDIT -- Spawn an editor to edit the file, if available */
static int
em_spawnedit PARM_0(void)
{
#ifdef ALLOW_EDIT_FORK
  char tmp_edit_filename[FILELTH];

  /* check the input */
  if (cur_message == NULL) {
    errormsg("Error: you have no current message to edit");
    return(0);
  }

  /* build the file name */
  sprintf(tmp_edit_filename, "%s.etmp", (ntn_ptr != NULL) ?
	  ntn_ptr->name : "god");
  
  /* first translate the mail message to normal text */
  if (email_toedit(tmp_edit_filename)) return(0);

  /* run the editor on it */
  fork_edit_on_file(tmp_edit_filename,
    "SPAWNING EDIT: Be sure that the To: line is properly formed.");

  /* now translate it back into the mail format */
  email_fromedit(tmp_edit_filename);
#endif /* ALLOW_EDIT_FORK */
  return(0);
}

/*
 * email_parse - Main mail editor interface and event loop
 *
 * Implements the complete mail editor interface by providing the main editing
 * loop and coordinating all mail editor functionality. This is the primary
 * entry point for interactive mail composition and editing.
 *
 * Initialization Phase:
 *   - Sets email_done to FALSE for editing loop control
 *   - Calculates email_collim based on terminal width and nation name length
 *   - Sets MAIL_SENDING flag in mail_ind for system state tracking
 *   - Initializes email_status to EM_S_TOLINE (recipient editing mode)
 *   - Applies current email_mode settings to status
 *   - Calls email_prep() for complete message initialization
 *   - Clears screen for clean editing environment
 *
 * Main Event Loop:
 *   - Continuously displays current message state via email_show()
 *   - Processes user input through parse_keys() with email_bindings
 *   - Handles both function key commands and character input
 *   - Distinguishes between command sequences and character input
 *   - Provides error feedback for unknown key bindings
 *   - Continues until email_done flag is set by exit/send commands
 *
 * Input Processing Logic:
 *   - parse_keys() returns NULL for character input, function pointer for commands
 *   - Character input: Passed to email_addchar() for mode-specific processing
 *   - Command input: Function executed directly via function pointer
 *   - Multi-character sequences: Displayed as unknown if not bound to functions
 *   - Single characters: Processed as text input for current editing mode
 *
 * Cleanup and Resource Management:
 *   - Calls email_close() after editing loop completion
 *   - Ensures all file locks are released
 *   - Frees all allocated message memory
 *   - Resets display system for return to main game interface
 *
 * Screen Management:
 *   - Maintains consistent screen layout throughout editing
 *   - Provides real-time visual feedback for all operations
 *   - Handles screen clearing and redrawing efficiently
 *   - Manages cursor positioning across different editing modes
 *
 * Side Effects:
 *   - Modifies global mail editor state extensively
 *   - Changes mail_ind flags for system state tracking
 *   - May send mail messages if user chooses to send
 *   - Acquires and releases file locks for recipients
 *   - Completely manages screen display during editing
 *   - Sets email_done flag when editing is complete
 *
 * Notes:
 *   - Central orchestration function for entire mail editor
 *   - Essential for all interactive mail composition
 *   - Provides complete user interface for mail editing
 *   - Handles all aspects of mail editor lifecycle
 *   - Critical for game's communication system
 */
void
email_parse PARM_0(void)
{
  FNCI fnc_ptr;

  /* initialize things */
  email_done = FALSE;
  email_collim = 80 - (strlen(nationname) + 4);
  mail_ind |= MAIL_SENDING;
  email_status = EM_S_TOLINE;
  email_status |= email_mode;
  email_prep();
  clear();

  /* have we finished? */
  while (email_done == FALSE) {

    /* display what there is of the message */
    move(0, 0);
    clrtobot();
    email_show();

    /* now get the input */
    if ((fnc_ptr = parse_keys(email_bindings, FALSE)) == NULL) {

      /* now is it an input key or is it memorex? */
      if (strlen(string) > 1) {
	/* memorex */
	clear_bottom(1);
	mvaddstr(LINES - 1, 0, "Unknown key binding: ");
	show_str(string, FALSE);
	presskey();
      } else {
	/* new input */
	email_addchar(string[0]);
      }

    } else {

      /* perform the function */
      (*fnc_ptr)();

    }
    
  }
  
  /* remove locks and free up the space */
  email_close();
}

/*
 * em_options - Access mail editor configuration options
 *
 * Provides access to mail editor configuration and customization options
 * through the game's standard option system. Allows users to modify mail
 * editor behavior, key bindings, and preferences during editing sessions.
 *
 * Option System Integration:
 *   - Calls option_cmd() with mail editor-specific parameters
 *   - Passes email_keysys for key binding system context
 *   - Passes email_bindings for current binding configuration
 *   - Integrates with game's unified option management system
 *
 * Available Configurations:
 *   - Key binding modifications and customizations
 *   - Mail editor behavior preferences
 *   - Display and interface options
 *   - Input handling customizations
 *
 * User Experience:
 *   - Accessible during mail editing without losing current message
 *   - Provides immediate option changes without restart
 *   - Integrates seamlessly with mail editor workflow
 *   - Maintains consistent option interface with rest of game
 *
 * Returns:
 *   0 on success (always returns 0)
 *
 * Side Effects:
 *   - May modify email_bindings based on user selections
 *   - May change mail editor behavior preferences
 *   - May update key binding configurations
 *   - Temporarily displays option interface overlay
 *
 * Notes:
 *   - Essential for mail editor customization
 *   - Provides user control over editor behavior
 *   - Integrates with game's comprehensive option system
 *   - Supports user workflow customization
 */
static int
em_options PARM_0(void)
{
  option_cmd(email_keysys, &email_bindings);
  return(0);
}

/* function list */
PARSE_STRUCT email_funcs[] = {
  {em_options, "conquer-options",
     "Adjust the conquer environment by changing various options"},
  {do_ignore, "ignore-key",
     "Just pretend this key press didn't really happen"},
  {em_backward, "mail-backward",
     "Mail-mode, move cursor one to the left"},
  {em_tobol, "mail-beginning-of-line",
     "Mail-mode, move cursor to the beginning of the current line"},
/*{em_cmdline, "mail-command",
   "Mail-mode, execute a specified mail command by name"}, */
  {em_delleft, "mail-delete-backward",
     "Mail-mode, delete the character to the left of the cursor"},
  {em_delright, "mail-delete-forward",
     "Mail-mode, delete the character under the cursor"},
  {em_down, "mail-downline",
     "Mail-mode, move cursor down one line"},
  {em_toeol, "mail-end-of-line",
     "Mail-mode, move cursor to the end of the current line"},
  {em_exit, "mail-exit",
     "Mail-mode, deliver the message and then leave mail"},
  {em_forward, "mail-forward",
     "Mail-mode, move cursor one to the right"},
  {em_help, "mail-help",
     "Mail-mode, provide documentation on the mail mode"},
  {em_prekill, "mail-kill-to-beginning",
     "Mail-mode, delete from the cursor to the start of the current line"},
  {em_kill, "mail-kill-line",
     "Mail-mode, delete from the cursor to the end of the current line"},
  {em_newline, "mail-newline",
     "Mail-mode, break the current line at the cursor, creating a new line"},
  {em_quit, "mail-quit",
     "Mail-mode, leave mail without sending the message"},
  {em_send, "mail-send",
     "Mail-mode, send the message to all of the recipients"},
#ifdef ALLOW_EDIT_FORK
  {em_spawnedit, "mail-spawn-editor",
     "Mail-mode, use your personal EDITOR to edit your mail message"},
#else
  {em_spawnedit, "mail-spawn-editor",
     "Mail-mode, this command is not supported in this version of conquer"},
#endif /* ALLOW_EDIT_FORK */
  {em_toggle, "mail-toggle",
     "Mail-mode, toggle between overwrite and insert mode in the editor"},
  {em_up, "mail-upline",
     "Mail-mode, move cursor up one line"},
  {do_redraw, "redraw-screen",
     "redraw the screen display of the mail message"}
};

/* Default editor key bindings */
KBIND_STRUCT email_klist[] = {
  { ESTR_DEL, em_delleft },
  { "\n", em_newline },
  { "\r", em_newline },
  { CSTR_B, em_backward },
  { CSTR_C, do_ignore },
  { CSTR_D, em_delright },
  { CSTR_E, em_toeol },
  { CSTR_F, em_forward },
  { CSTR_G, em_quit },
  { CSTR_K, em_kill },
  { CSTR_L, do_redraw },
  { CSTR_O, em_options },
  { CSTR_N, em_down },
  { CSTR_P, em_up },
  { CSTR_R, do_redraw },
  { CSTR_U, em_prekill },
  { CSTR_W, em_send },
  { CSTR_X, em_exit },
#ifdef VMS
  { CSTR_A, em_toggle },
  { CSTR_H, em_tobol },
  { CSTR_Z, em_exit },
#else
  { CSTR_A, em_tobol },
  { CSTR_H, em_delleft },
  { CSTR_T, em_toggle },
#endif /* VMS */
  { "\033?", em_help },
/*{ "\033x", em_cmdline }, */
  { "\033$", em_spawnedit },
  { AKEY_UP1, em_up },
  { AKEY_UP2, em_up },
  { AKEY_DOWN1, em_down },
  { AKEY_DOWN2, em_down },
  { AKEY_RIGHT1, em_forward },
  { AKEY_RIGHT2, em_forward },
  { AKEY_LEFT1, em_backward },
  { AKEY_LEFT2, em_backward }
};

/* ALIGN_EMAIL_KEYS -- Align all of the mail editor keys */
void
align_email_keys PARM_0(void)
{
  /* initialize the system */
  if (email_bindings == NULL) {
    email_keysys.num_binds = (sizeof(email_klist)/sizeof(KBIND_STRUCT));
    email_keysys.num_parse = (sizeof(email_funcs)/sizeof(PARSE_STRUCT));
    init_keys(&email_bindings, email_klist, email_keysys.num_binds);
  }
}
