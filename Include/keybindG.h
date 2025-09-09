/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * keybindG.h - Key Binding System Definitions and Input Mapping Framework
 *
 * This header defines the comprehensive key binding system that allows users to
 * customize keyboard shortcuts and input mappings for different game interfaces.
 * The system supports multiple independent key binding contexts (email, reader,
 * magic, etc.) with configurable function mappings and multi-key sequences.
 *
 * Key Components:
 *   - KBIND_STRUCT: Individual key binding entries with multi-character sequences
 *   - KLIST_STRUCT: Linked list management for dynamic key binding collections
 *   - PARSE_STRUCT: Function registration and metadata for bindable operations
 *   - KEYSYS_STRUCT: Complete key binding system contexts with default configurations
 *
 * Integration Notes:
 *   - Used by: All G-series interface modules for customizable input handling
 *   - Depends on: header.h for FNCI function pointer type definitions
 *   - Provides: Flexible key binding framework for multi-context user interfaces
 *
 * Architecture:
 *   The system supports multiple independent key binding contexts, each with their
 *   own function registry and binding configurations. This allows different game
 *   interfaces (email, magic, transfer, etc.) to have context-appropriate key
 *   mappings while sharing the same underlying binding infrastructure.
 *
 * Modernization Considerations:
 *   - Multi-key sequences limited to MAXKEYS (10) - consider dynamic allocation
 *   - Function pointers (FNCI) may need type safety improvements
 *   - String storage uses fixed arrays - consider const string management
 *   - Global binding lists could benefit from initialization validation
 */

/* ============================================================================
 * KEY BINDING SYSTEM CONFIGURATION
 * ============================================================================
 * Purpose: Core configuration constants for multi-key sequence support
 * Usage: Defines limits and constraints for key binding operations
 * Notes: MAXKEYS determines maximum complexity of key sequences
 */

/* has this been used before? */
#ifndef MAXKEYS

/*
 * MAXKEYS - Maximum number of keys for a binding sequence
 *
 * Defines the maximum length of multi-key sequences that can be bound to
 * a single function. This allows for complex key combinations like "Ctrl+X
 * Ctrl+S" style bindings common in text editors and advanced interfaces.
 *
 * Value: 10 characters maximum per binding sequence
 * Usage: Used to size the chlist array in KBIND_STRUCT for sequence storage
 * Notes: Includes null terminator, so effective sequence length is 9 keys
 * Modernization: Consider dynamic allocation for variable-length sequences
 */
#define MAXKEYS	10

/* ============================================================================
 * CORE KEY BINDING DATA STRUCTURES
 * ============================================================================
 * Purpose: Fundamental data types for storing and managing key bindings
 * Usage: Used throughout the key binding system for configuration storage
 * Notes: These structures form the foundation of the binding architecture
 */

/*
 * struct s_keybind - Individual key binding entry
 *
 * Represents a single key binding that maps a sequence of characters to a
 * specific function. This is the atomic unit of the key binding system,
 * storing both the input sequence and the target function for execution.
 *
 * Usage Patterns:
 *   - Created during key binding configuration and loading
 *   - Stored in arrays and linked lists for different binding contexts
 *   - Searched during input processing to find matching key sequences
 *
 * Relationships:
 *   - Embedded in KLIST_STRUCT for linked list management
 *   - Used by KEYSYS_STRUCT for default binding storage
 *   - Function pointer type FNCI defined in header.h
 *
 * Fields:
 *   chlist - Key sequence string (null-terminated, max MAXKEYS length)
 *   func   - Function pointer to execute when sequence is matched
 */
typedef struct s_keybind {
  char chlist[MAXKEYS + 1];	/* characters for this binding		*/
  FNCI func;			/* what function the key is bound to	*/
} KBIND_STRUCT, *KBIND_PTR;

/*
 * struct s_keylist - Linked list node for dynamic key binding collections
 *
 * Provides dynamic storage and management for collections of key bindings
 * within specific contexts. This allows for runtime modification of key
 * bindings and supports variable numbers of bindings per context.
 *
 * Usage Patterns:
 *   - Built during key binding system initialization
 *   - Modified when users customize their key bindings
 *   - Traversed during input processing to find matching sequences
 *   - Freed during system cleanup or binding reset operations
 *
 * Relationships:
 *   - Contains KBIND_STRUCT for actual binding data storage
 *   - Forms linked lists referenced by global binding pointers
 *   - Used by key binding search and modification algorithms
 *
 * Fields:
 *   key_data - The actual key binding entry (sequence + function)
 *   next     - Pointer to next binding in the list (NULL for last entry)
 */
typedef struct s_keylist {
  KBIND_STRUCT key_data;	/* function and key binding	*/
  struct s_keylist *next;	/* access to the next binding	*/
} KLIST_STRUCT, *KLIST_PTR;

/*
 * struct s_parse - Function registry entry for bindable operations
 *
 * Stores metadata about functions that can be bound to key sequences,
 * including the function pointer and human-readable information for
 * configuration interfaces and help systems.
 *
 * Usage Patterns:
 *   - Populated during system initialization with available functions
 *   - Used by key binding configuration interfaces to show options
 *   - Referenced during help system display for function descriptions
 *   - Searched when parsing user key binding configuration files
 *
 * Relationships:
 *   - Function pointer must match FNCI type from header.h
 *   - Used in arrays within KEYSYS_STRUCT for context function lists
 *   - Names and descriptions used by user interface systems
 *
 * Fields:
 *   func     - Function pointer for the bindable operation
 *   realname - Internal function name for configuration and debugging
 *   descript - Human-readable description for user interfaces and help
 */
typedef struct s_parse {
  FNCI func;		/* actual function		*/
  char *realname;	/* name associated w/function	*/
  char *descript;	/* description of the function	*/
} PARSE_STRUCT, *PARSE_PTR;

/*
 * struct s_keysys - Complete key binding system context
 *
 * Defines a complete key binding context with available functions, default
 * key bindings, and metadata. Each interface context (email, magic, transfer,
 * etc.) has its own KEYSYS_STRUCT that defines what functions are available
 * and how they are bound by default.
 *
 * Usage Patterns:
 *   - Initialized during system startup for each interface context
 *   - Used to populate default key bindings when user has no custom config
 *   - Referenced during key binding configuration to show available functions
 *   - Used for context switching when moving between different interfaces
 *
 * Relationships:
 *   - Contains arrays of PARSE_STRUCT for available functions
 *   - Contains arrays of KBIND_STRUCT for default key bindings
 *   - Referenced by global keysys variables for each interface context
 *   - Used by key binding management and configuration systems
 *
 * Fields:
 *   name      - Human-readable name for this key binding context
 *   parse_p   - Array of available functions that can be bound
 *   kbind_p   - Array of default key bindings for this context
 *   num_parse - Count of available functions in parse_p array
 *   num_binds - Count of default bindings in kbind_p array
 */
typedef struct s_keysys {
  char *name;		/* the name of this key binding system */
  PARSE_PTR parse_p;	/* pointer to the list of system functions */
  KBIND_PTR kbind_p;	/* pointer to the list of default bindings */
  int num_parse;	/* number of entries in the function list */
  int num_binds;	/* number of entries in the key binding list */
} KEYSYS_STRUCT, *KEYSYS_PTR;

/* ============================================================================
 * GLOBAL KEY BINDING LISTS
 * ============================================================================
 * Purpose: Runtime key binding storage for different interface contexts
 * Usage: Modified during user customization, searched during input processing
 * Notes: These lists contain the actual active key bindings for each context
 */

/*
 * Global Key Binding List Declarations
 *
 * These global pointers maintain the active key binding lists for different
 * interface contexts. Each list contains user-customized or default key
 * bindings that are searched during input processing to determine which
 * function to execute for a given key sequence.
 *
 * List Contexts:
 *   email_bindings  - Email interface key bindings (composition, reading)
 *   reader_bindings - Text reader/pager interface key bindings (scrolling, navigation)
 *   mg_bindings     - Magic system interface key bindings (spell casting, browsing)
 *   ni_bindings     - Nation information interface key bindings (reports, statistics)
 *   xfer_bindings   - Transfer interface key bindings (item/resource movement)
 *   mparse_bindings - Magic parsing interface key bindings (spell analysis)
 *   cq_bindings     - Command queue interface key bindings (batch operations)
 *
 * Memory Management:
 *   - Initialized during system startup with default bindings
 *   - Modified when user customizes key bindings through configuration
 *   - Freed during system shutdown or binding reset operations
 *   - Each list is independent and context-specific
 */
extern KLIST_PTR email_bindings;
extern KLIST_PTR reader_bindings;
extern KLIST_PTR mg_bindings;
extern KLIST_PTR ni_bindings;
extern KLIST_PTR xfer_bindings;
extern KLIST_PTR mparse_bindings;
extern KLIST_PTR cq_bindings;

/* ============================================================================
 * KEY BINDING SYSTEM CONTEXTS
 * ============================================================================
 * Purpose: Default configuration templates for different interface contexts
 * Usage: Used to initialize and reset key bindings to default configurations
 * Notes: Each context defines available functions and default key mappings
 */

/*
 * Global Key System Context Declarations
 *
 * These global structures define the complete key binding system contexts
 * for different interfaces. Each context contains the available functions,
 * default key bindings, and metadata needed to support a specific interface.
 *
 * System Contexts:
 *   global_keysys - Global/shared key bindings available across all contexts
 *   email_keysys  - Email interface context (composition, reading, management)
 *   reader_keysys - Text reader/pager context (scrolling, navigation, search)
 *   magic_keysys  - Magic system context (spell casting, browsing, research)
 *   mparse_keysys - Magic parsing context (spell analysis, syntax checking)
 *   xfer_keysys   - Transfer interface context (item/resource movement, trading)
 *   ninfo_keysys  - Nation information context (reports, statistics, analysis)
 *
 * Usage Patterns:
 *   - Referenced during system initialization to populate default bindings
 *   - Used by configuration interfaces to show available functions
 *   - Consulted when resetting key bindings to factory defaults
 *   - Provides metadata for help systems and user documentation
 *
 * Initialization:
 *   - Populated with static data during compilation
 *   - Contains function pointers, names, descriptions, and default bindings
 *   - Used as templates for creating runtime binding lists
 */
extern KEYSYS_STRUCT global_keysys;
extern KEYSYS_STRUCT email_keysys;
extern KEYSYS_STRUCT reader_keysys;
extern KEYSYS_STRUCT magic_keysys;
extern KEYSYS_STRUCT mparse_keysys;
extern KEYSYS_STRUCT xfer_keysys;
extern KEYSYS_STRUCT ninfo_keysys;

#endif /* MAX_KEYS */
