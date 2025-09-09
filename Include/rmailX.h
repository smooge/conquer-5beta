/*
 * rmailX.h - In-Game Mail System Data Structures and Definitions
 *
 * This header defines the comprehensive data structures and constants for the in-game
 * messaging system, supporting multi-recipient mail communication between nations,
 * message status tracking, and complete message lifecycle management. The system
 * provides diplomatic communication, news distribution, and player interaction
 * capabilities within the strategic gameplay environment.
 *
 * Key Features:
 * - Multi-recipient messaging: Support for up to 20 simultaneous recipients per message
 * - Message status tracking: Read/unread, deletion, reply, and forwarding states
 * - Linked list message storage: Efficient memory management for variable-length content
 * - Doubly-linked message queues: Bi-directional navigation through message collections
 * - Complete message metadata: Sender identification, timestamps, subject lines
 * - Diplomatic integration: Nation-to-nation communication for strategic coordination
 *
 * Usage Context:
 * This header is conditionally included only when direct mail system access is required
 * for message reading, writing, or administrative operations. Core game functionality
 * may use higher-level mail interfaces without requiring these detailed structures.
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

/* THIS FILE IS ONLY NEEDED IF DIRECT ACCESS TO
   READING OR WRITING MAIL MESSAGES IS NECESSARY */

/* Mail System Configuration Constants - Messaging Limits and Constraints */
/*
 * Mail System Limits - Define the operational constraints and capacity limits
 * for the in-game messaging system to ensure reasonable resource usage and
 * prevent abuse while supporting effective diplomatic communication.
 */
#define MAX_ADR	20	/* Maximum recipient count - upper limit for simultaneous message distribution.
                     * Supports group messaging for alliances, councils, and diplomatic negotiations
                     * while preventing mass spam and maintaining reasonable processing overhead. */

/* Mail Message Status Flags - Message Lifecycle and State Tracking */
/*
 * Message Status Constants - Bit flag system for tracking message state throughout
 * its lifecycle from creation to deletion. These flags support mailbox management,
 * user interface display, and message organization functionality.
 *
 * The status system uses bit flags to enable multiple simultaneous states and
 * efficient status testing operations through bitwise operations.
 */
#define MSTAT_NONE	0	    /* Clean status - newly created message with no special state flags.
                           * Represents unread, unprocessed message in pristine condition. */
#define MSTAT_READ	0x0001	/* Read status - message has been opened and viewed by recipient.
                           * Used for mailbox organization and unread message counting. */
#define MSTAT_DELETED	0x0002	/* Deletion marker - message marked for removal from mailbox.
                           * Enables soft deletion with potential recovery before final cleanup. */
#define MSTAT_REPLIED	0x0004	/* Reply indicator - response has been sent to original sender.
                           * Tracks diplomatic correspondence and conversation threading. */
#define MSTAT_FORWARDED	0x0008	/* Forward indicator - message has been forwarded to other recipients.
                           * Supports information distribution and diplomatic relay communications. */

/* Mail Content Data Structure - Variable-Length Message Text Storage */
/*
 * MAILD_STRUCT - Individual line storage for message content using doubly-linked
 * list architecture. This structure enables efficient storage and manipulation
 * of variable-length message text while supporting line-by-line editing,
 * insertion, and formatting operations.
 *
 * Design Features:
 * - Dynamic memory allocation: Each line allocated independently for flexibility
 * - Doubly-linked list: Bi-directional navigation for editing and display
 * - Line-based organization: Natural text formatting and editing granularity
 * - Memory efficiency: Only allocates storage for actual message content
 */
typedef struct s_maildata {
  char *line_data;		/* Message line content - single line of message text with dynamic allocation.
                         * Null-terminated string containing one line of the message body,
                         * supporting variable-length content from short notes to detailed diplomatic correspondence. */
  struct s_maildata *next;	/* Forward link - pointer to next line in message sequence.
                         * Enables sequential reading and display of complete message content
                         * with efficient traversal through entire message body. */
  struct s_maildata *prev;	/* Backward link - pointer to previous line in message sequence.
                         * Supports bi-directional navigation for editing, cursor movement,
                         * and efficient insertion/deletion operations within message text. */
} MAILD_STRUCT, *MAILD_PTR;

/* Complete Mail Message Structure - Comprehensive Message Management */
/*
 * RMAIL_STRUCT - Complete mail message data structure containing all metadata,
 * recipient information, content, and organizational links. This structure
 * serves as the primary message object for the entire mail system, supporting
 * multi-recipient distribution, message threading, and complete lifecycle management.
 *
 * Architecture Features:
 * - Complete message metadata: Sender, timestamp, subject, status tracking
 * - Multi-recipient support: Array-based recipient list for group messaging
 * - Linked content storage: Pointer to doubly-linked message text lines
 * - Message threading: Doubly-linked message queue for mailbox organization
 * - Status integration: Bit-flag status system for message state management
 */
typedef struct s_rmail {
  char *sender;			/* Message originator - nation name or identifier of message sender.
                         * Dynamically allocated string containing the official nation name
                         * or player identification for diplomatic correspondence tracking. */
  char *nickname;		/* Sender alias - alternative or display name for message sender.
                         * Optional friendly name or title for enhanced diplomatic interaction
                         * and player recognition in message displays and mailbox listings. */
  int to_whom[MAX_ADR];	/* Recipient list - array of nation identifiers for message distribution.
                         * Fixed-size array supporting up to MAX_ADR (20) simultaneous recipients
                         * for group messaging, alliance communications, and diplomatic announcements. */
  char *date;			/* Timestamp - message creation date and time for chronological organization.
                         * Human-readable date string supporting mailbox sorting, message aging,
                         * and diplomatic correspondence timeline tracking. */
  char *subj;			/* Subject line - brief message description for mailbox organization.
                         * Dynamically allocated string containing message topic for quick
                         * identification, sorting, and diplomatic correspondence categorization. */
  int status;			/* Message status - bit-flag combination of MSTAT_* constants.
                         * Integer field containing message state information including read status,
                         * deletion markers, reply indicators, and forwarding history. */
  MAILD_PTR text;		/* Message content - pointer to first line of message text.
                         * Head pointer to doubly-linked list of MAILD_STRUCT nodes containing
                         * the complete message body with line-by-line organization. */
  struct s_rmail *next;		/* Forward queue link - pointer to next message in mailbox sequence.
                         * Enables sequential traversal through mailbox contents for display,
                         * processing, and organizational operations. */
  struct s_rmail *prev;		/* Backward queue link - pointer to previous message in mailbox sequence.
                         * Supports bi-directional navigation for efficient mailbox management,
                         * message deletion, and insertion operations. */
} RMAIL_STRUCT, *RMAIL_PTR;

/* Global Mail System Variables - Message Management State */
/*
 * Global Mail Variables - Provide access to the current mail system state
 * and active message context for mail processing operations throughout
 * the system. These variables maintain the current position and context
 * within the mail system for user interface and processing operations.
 */
extern RMAIL_PTR cur_message;  /* Current message pointer - active message for mail operations.
                               * Global pointer to the currently selected or active message
                               * within the mail system, used for reading, editing, replying,
                               * and other message-specific operations. Maintained by the
                               * mail interface to track user navigation through mailbox contents. */

/*
 * Mail System Integration and Usage:
 *
 * Data Structure Relationships:
 * - RMAIL_STRUCT contains MAILD_PTR to message content lines
 * - MAILD_STRUCT forms doubly-linked list for message text
 * - RMAIL_STRUCT forms doubly-linked list for mailbox organization
 * - cur_message provides global access to active message context
 *
 * Memory Management:
 * - Dynamic allocation for all string fields (sender, nickname, date, subj)
 * - Dynamic allocation for MAILD_STRUCT nodes and line_data content
 * - Careful cleanup required for complete message deletion
 * - Doubly-linked lists enable efficient insertion and removal operations
 *
 * Multi-Recipient Support:
 * - to_whom[] array supports up to MAX_ADR (20) recipients per message
 * - Nation identifiers stored as integers for efficient processing
 * - Group messaging enables alliance coordination and diplomatic announcements
 * - Message distribution handled by mail processing systems
 *
 * Status Management:
 * - Bit-flag status system using MSTAT_* constants
 * - Multiple simultaneous states supported (read + replied + forwarded)
 * - Status tracking enables sophisticated mailbox organization
 * - UI indicators and filtering based on message status combinations
 *
 * Performance Characteristics:
 * - O(1) message insertion and deletion with doubly-linked lists
 * - O(n) search operations for message lookup by criteria
 * - Memory usage scales linearly with message count and content length
 * - Efficient navigation through mailbox contents with linked structure
 *
 * Modernization Considerations for C2023 Upgrade:
 * - Replace manual memory management with smart pointer systems
 * - Add const qualifiers for immutable message data fields
 * - Implement bounds checking for to_whom[] array access
 * - Add message validation functions with comprehensive error checking
 * - Consider message encryption and security enhancements
 * - Implement message compression for large diplomatic correspondence
 * - Add message indexing and search capabilities for large mailboxes
 * - Convert to structured message format (JSON/XML) for extensibility
 * - Add message threading and conversation grouping features
 * - Implement message archiving and backup systems
 */
