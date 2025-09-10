/*
 * header.h - Core System Configuration and Global Constants
 *
 * This file contains the fundamental configuration constants, system dependencies,
 * and compile-time options that control the behavior of the entire Conquer game
 * system. It serves as the central configuration hub that determines platform
 * compatibility, game balance parameters, system resource limits, and feature
 * availability.
 *
 * Key Components:
 *   - Version identification and administrator configuration
 *   - Data file format control and compression settings
 *   - System-dependent file operations and platform compatibility
 *   - Game balance parameters and economic constants
 *   - Unit specifications and combat mechanics
 *   - Network and security configuration options
 *
 * Integration Notes:
 *   - Used by: ALL source files in the Conquer system
 *   - Depends on: Standard C library headers (included by files that use this)
 *   - Provides: System-wide constants, feature flags, and configuration values
 *
 * Modernization Considerations:
 *   - Contains hardcoded paths that should use environment variables
 *   - File permission constants need review for modern security standards
 *   - Compression utilities should be configurable at runtime
 *   - Platform detection should use autotools or CMake feature detection
 *   - Magic numbers would benefit from named constants with explanatory comments
 *   - VMS support is obsolete and can be removed for modern systems
 */

/* conquer: Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * MODIFICATION OF THIS FILE IMPLIES THAT THE MODIFIER WILL ACCEPT
 * A LIMITED USE COPYRIGHT AS FOLLOWS:
 *
 * 1) This software is copyrighted and protected by law.  The owners
 *     of this software, which hereafter is known as "conquer", are
 *     Edward M. Barlow and Adam D. Bryant, who hereby grant you a
 *     personal, non-exclusive right to use this software.
 *     All rights on this software are reserved.
 * 2) conquer, once modified, may not be redistributed in any form.
 *     Any requests for new software shall, for now, be the perogative
 *     of the authors.
 * 3) Loss or damage caused by this software shall not be the
 *     responsibility of the authors.
 * 4) If any enhancements to this software are made, the authors shall
 *     be notified via electronic mail, and if there is no response,
 *     via US mail to:
 *
 *        Adam Bryant                       Ed Barlow
 *        120 Glenville Ave. Apt 7          115 E 34ths St.
 *        Allston, MA 02134                 NY, NY 10016
 *        (617) 787-4794                    (212) 679-1439
 *        adb@bu.edu
 *
 *     The home phone numbers listed above are to be used sparingly,
 *     and before 11PM if it is important.   Ed Barlow no longer has
 *     easy access to arpanet, so is not able to support conquer as
 *     well as he was originally able to.  If anybody knows of a cheep
 *     (preferably free) way to login to an account in New York with
 *     network access, feel free to contact Ed at the address listed
 *     above.  Also, Adam Bryant is providing all of the support for
 *     version 4.x and above, so Ed will likely not be able to help
 *     with problems associated with those versions.
 * 5) No attempt shall be made to make any money from this game or to
 *     use any portion of this code to make any money without the
 *     authors' permission.
 * 6) No attempt shall be made to port this software to any form of
 *     personal computer without the permission of Ed Barlow.
 * 7) You agree to use your best efforts to see that any user of
 *     conquer complies with the terms and conditions stated above.
 * 8) The above copyright agreement will not be tampered with in any
 *     form.
 *
 */

/* ============================================================================
 * SYSTEM IDENTIFICATION AND ADMINISTRATOR CONFIGURATION
 * ============================================================================
 * Purpose: Version identification and system administrator settings
 * Usage: These constants identify the software version and configure admin access
 * Notes: LOGIN constant is security-critical and controls god-mode access
 */

/*
 * VERSION - Software version identification string
 *
 * This constant identifies the specific version of the Conquer game system.
 * The version string is displayed to users during startup and in help text,
 * and may be used for compatibility checking in data files or network protocols.
 *
 * Value: "Version 5.0b" (beta release of version 5.0)
 * Usage: Displayed in game startup, help text, and error messages
 * Notes: Should be updated with each release; 'b' suffix indicates beta status
 * Modernization: Consider semantic versioning (e.g., "5.0.0-beta")
 */
#define VERSION "Version 5.0b"

/*
 * OWNER - System administrator display name
 *
 * Friendly name of the system administrator, used for display purposes in
 * game messages, help text, and administrative interfaces. This is purely
 * cosmetic and does not affect system security or access control.
 *
 * Value: "Smoogen" (current administrator's display name)
 * Usage: Shown in game interface, help screens, and administrative messages
 * Notes: Can be changed safely without affecting game functionality
 * Modernization: Consider loading from configuration file instead of compile-time constant
 */
#define OWNER	"Smoogen"	/* administrators name (just for show)	*/

/*
 * LOGIN - System administrator login identifier (SECURITY CRITICAL)
 *
 * Login name of the system administrator account that has god-mode access to
 * the game system. This constant is used by security checks to grant special
 * privileges including world modification, user management, and system control.
 * Changing this value affects who can access administrative functions.
 *
 * Value: "ssmoogen" (administrator's actual login name)
 * Usage: Security validation for god-mode access and administrative functions
 * Notes: SECURITY CRITICAL - controls access to system administration features
 * Modernization: Should use configuration file or environment variable for security
 */
#define LOGIN	"ssmoogen"		/* administrators login id. IMPORTANT!	*/

/* ============================================================================
 * DATA FILE FORMAT CONTROL (PRIMARY ADJUSTMENTS)
 * ============================================================================
 * Purpose: Core settings that control data file format and storage capabilities
 * Usage: These settings determine fundamental game limits and data persistence behavior
 * Notes: WARNING - Changing these values requires data file format migration
 *        Existing save games become incompatible when these values change
 */

/*
 * COMPRESS - Enable data file compression
 *
 * When defined, enables automatic compression of game data files using system
 * compression utilities. This significantly reduces disk space usage for save
 * games but requires compatible compression tools on the target system.
 *
 * Value: Defined (compression enabled)
 * Usage: Controls whether data files are compressed during save operations
 * Notes: Requires COMP_NAME, COMP_READ, and COMP_SFX to be properly configured
 * Modernization: Should use modern compression libraries (gzip, zlib) instead of external tools
 */
#define COMPRESS	/* use compression when storing the data file	*/

#ifdef COMPRESS
/*
 * COMP_NAME - Compression program command
 *
 * External command used to compress data files. The 'compress' utility is
 * a legacy UNIX compression tool that creates .Z files using LZW compression.
 *
 * Value: "compress" (standard UNIX compress utility)
 * Usage: Called via system() to compress data files during save operations
 * Notes: Legacy tool, availability varies on modern systems
 * Modernization: Replace with gzip or internal compression library
 */
#define COMP_NAME	"compress"	/* file compression program	*/

/*
 * COMP_READ - Decompression read command
 *
 * External command used to read compressed data files. The 'zcat' utility
 * decompresses .Z files to stdout, allowing the game to read compressed data.
 *
 * Value: "zcat" (decompress and output to stdout)
 * Usage: Called to read compressed data files during load operations
 * Notes: Must be compatible with COMP_NAME compression format
 * Modernization: Replace with gunzip -c or internal decompression
 */
#define COMP_READ	"zcat"		/* how to read compressed files	*/

/*
 * COMP_SFX - Compressed file suffix
 *
 * File extension added to compressed data files. The .Z suffix is specific
 * to the UNIX 'compress' utility and indicates LZW-compressed files.
 *
 * Value: ".Z" (compress utility file extension)
 * Usage: Appended to filenames when creating compressed data files
 * Notes: Must match the output format of COMP_NAME utility
 * Modernization: Use .gz for gzip or .zst for zstandard
 */
#define COMP_SFX	".Z"		/* compressed file suffix	*/
#endif

/*
 * MANY_UNITS - Enable extended unit limits
 *
 * When defined, allows more than 255 units per army, navy, or caravan.
 * This breaks the original 8-bit unit count limitation and enables larger
 * military formations and economic operations.
 *
 * Value: Defined (extended limits enabled)
 * Usage: Controls internal data structures for unit counting
 * Notes: Changes data file format - incompatible with original saves
 * Modernization: Should use configurable limits instead of compile-time flag
 */
#define MANY_UNITS	/* allow more than 255 army, navy, cvan units	*/

/*
 * HUGE_MAP - Enable large world maps
 *
 * When defined, allows world maps larger than 256x256 sectors. This breaks
 * the original 8-bit coordinate limitation and enables much larger game worlds
 * for extended gameplay and larger player counts.
 *
 * Value: Defined (large maps enabled)
 * Usage: Controls coordinate data types and map size calculations
 * Notes: Changes data file format - requires 16-bit coordinates
 * Modernization: Should use dynamic sizing instead of compile-time limits
 */
#define HUGE_MAP	/* allow really huge ( > 256x256 ) map sizes	*/

/*
 * ABSMAXNTN - Maximum number of nations (absolute limit)
 *
 * Hard limit on the total number of nations (player and NPC) that can exist
 * in a single game world. This limit affects memory allocation, data structures,
 * and diplomatic relationship matrices.
 *
 * Value: 100 nations maximum
 * Usage: Array sizing for nation-related data structures and diplomatic relations
 * Notes: Increasing requires careful review of O(n²) diplomatic algorithms
 * Modernization: Consider dynamic allocation instead of fixed arrays
 */
#define ABSMAXNTN 100	/* maximum number of nations in the world       */

/* ============================================================================
 * SYSTEM SECURITY AND FILE PERMISSIONS
 * ============================================================================
 * Purpose: Security settings and file permission masks for system operation
 * Usage: Controls file access permissions and security-related system behavior
 * Notes: These settings affect system security and multi-user access control
 */

/*
 * DEFAULT_UMASK - Default file creation permissions mask
 *
 * Standard umask for files created by the Conquer system. The value 077
 * removes all permissions for group and other users, allowing only the
 * owner (running user) to read, write, and execute created files.
 *
 * Value: 077 (octal) = owner-only access (rwx------)
 * Usage: Applied to most files created by the game system
 * Notes: Restrictive setting enhances security in multi-user environments
 * Modernization: Good security practice, but consider configurable permissions
 */
#define DEFAULT_UMASK 077	/* Default permissions mask for conquer	*/

/*
 * CUSTOM_UMASK - Configuration file permissions mask
 *
 * Permission mask for user configuration files (rcfiles). The value 066
 * removes write access for group and other users while allowing read access,
 * enabling other users to view but not modify configuration files.
 *
 * Value: 066 (octal) = owner read/write, others read-only (rw-r--r--)
 * Usage: Applied when creating user preference and configuration files
 * Notes: Allows sharing configuration while preventing unauthorized modification
 * Modernization: Consider user-configurable permission policies
 */
#define CUSTOM_UMASK  066	/* permissions mask for rcfile output	*/

/*
 * FULLWRITE_UMASK - Full access permissions mask
 *
 * Permission mask that grants full access to all users. Used for temporary
 * files or shared resources that need broad accessibility.
 *
 * Value: 000 (octal) = full access for all (rwxrwxrwx)
 * Usage: Applied to files requiring universal read/write access
 * Notes: Security risk - should be used sparingly and with caution
 * Modernization: Avoid or replace with more granular permission control
 */
#define FULLWRITE_UMASK 000	/* permissions to do anything to file	*/

/* ============================================================================
 * SYSTEM FEATURE CONTROL FLAGS
 * ============================================================================
 * Purpose: Enable/disable optional system features and capabilities
 * Usage: Compile-time feature selection for platform compatibility and security
 * Notes: Features can be disabled by commenting out the #define statements
 */

/*
 * ALLOW_EDIT_FORK - Enable secure editor spawning
 *
 * When defined, allows god-level users to spawn a secure editor for in-game
 * text editing. The editor runs in a controlled environment with limited
 * system access to prevent security breaches.
 *
 * Value: Defined (secure editor access enabled)
 * Usage: Enables editor functionality in administrative interfaces
 * Notes: Security consideration - editor access could be exploited
 * Modernization: Implement stronger sandboxing or use internal editor
 */
#define ALLOW_EDIT_FORK	/* allow (demi-)god to spawn a secure(?) editor	*/

/*
 * DO_TIME_CHECK - Enable time-based access control
 *
 * When defined, implements time checking for game access control using
 * SIGALRM signal handling. This enables features like session timeouts,
 * scheduled maintenance windows, and time-based restrictions.
 *
 * Value: Defined (time checking enabled)
 * Usage: Controls session timeouts and time-based game restrictions
 * Notes: Requires proper signal handling implementation
 * Modernization: Use modern timer APIs instead of signal-based timing
 */
#define DO_TIME_CHECK	/* implement time checking for conquer access;
			   this requires the signal() SIGALRM routines.	*/

/*
 * SYSMAIL - Enable system mail integration (currently disabled)
 *
 * When defined, enables integration with system mail to notify players
 * about system mail and game-related messages. Currently commented out,
 * indicating this feature is disabled or unsupported.
 *
 * Value: Undefined (system mail integration disabled)
 * Usage: Would integrate with system mail spool for player notifications
 * Notes: Disabled - modern systems use different mail architectures
 * Modernization: Replace with modern notification systems (email, webhooks)
 */
/*#define SYSMAIL*/		/* if your system supports mail; conquer will
			   notify players about system mail.		*/

/* ============================================================================
 * FILE SYSTEM OPERATIONS AND LOCKING
 * ============================================================================
 * Purpose: File system operation configuration and network file locking
 * Usage: Controls file operations and concurrent access in networked environments
 * Notes: Critical for data integrity in multi-user environments
 */

/*
 * LOCKF - Enable NFS file locking
 *
 * When defined, enables NFS (Network File System) file locking routines
 * for systems that support distributed file locking. Requires the lockd
 * daemon to be running on the system for proper operation.
 *
 * Value: Defined (NFS file locking enabled)
 * Usage: Prevents concurrent access conflicts in networked file systems
 * Notes: Essential for data integrity when game files are on NFS mounts
 * Modernization: Ensure compatibility with modern NFS implementations
 */
#define LOCKF		/* If your site uses NFS file locking routines
			   and has the lockd daemon running */

/*
 * REMOVE_NAME - File removal command
 *
 * System command used to remove files. Uses the standard UNIX 'rm -f'
 * command to forcefully remove files without prompting for confirmation.
 *
 * Value: "/bin/rm -f" (force remove command)
 * Usage: Called via system() to delete temporary and obsolete files
 * Notes: Hardcoded path may not be portable across all systems
 * Modernization: Use unlink() system call instead of external command
 */
#define REMOVE_NAME	"/bin/rm -f"	/* how are files removed?	*/

#ifdef SYSMAIL
/*
 * SPOOLDIR - System mail spool directory
 *
 * Directory path where system mail files are stored. Used for checking
 * whether players have unread system mail when SYSMAIL feature is enabled.
 *
 * Value: "/var/spool/mail/" (standard UNIX mail spool location)
 * Usage: Directory scanning for unread mail notifications
 * Notes: Only relevant when SYSMAIL is defined (currently disabled)
 * Modernization: Path varies across systems, should be configurable
 */
#define SPOOLDIR "/var/spool/mail/"	/* location of mail spool w '/'	*/
#endif

/* ============================================================================
 * TIMING AND TIMEOUT CONFIGURATION
 * ============================================================================
 * Purpose: Time intervals for various system operations and timeouts
 * Usage: Controls timing behavior for mail checks and forced logouts
 * Notes: All values are in seconds
 */

/*
 * MAIL_TIME - Mail check interval
 *
 * Time interval in seconds between checks for new system mail. When SYSMAIL
 * is enabled, the system periodically checks for new mail for logged-in players.
 *
 * Value: 60 seconds (1 minute intervals)
 * Usage: Periodic mail checking when SYSMAIL feature is enabled
 * Notes: Only relevant when SYSMAIL is defined
 * Modernization: Should be configurable, not compile-time constant
 */
#define MAIL_TIME 60	/* time in seconds between checks on mail files	*/

/*
 * TIMETOBOOT - Forced logout timeout
 *
 * Time in seconds before the -T (termination) flag forces player logouts.
 * This provides a grace period for players to finish their current actions
 * before being forcibly disconnected during system maintenance.
 *
 * Value: 120 seconds (2 minute warning period)
 * Usage: Countdown timer for forced disconnections during maintenance
 * Notes: Provides reasonable time for players to save and exit gracefully
 * Modernization: Should be configurable and support longer grace periods
 */
#define TIMETOBOOT 120	/* time in seconds before -T forces logouts */

/* ============================================================================
 * TEMPORARY FILE MANAGEMENT
 * ============================================================================
 * Purpose: Configuration for temporary file creation and management
 * Usage: Controls where and how temporary files are created during operations
 * Notes: Platform-specific settings for different operating systems
 */

/*
 * TMP_FILE_FMT - Temporary file format string
 *
 * Format string for generating temporary file names. The format requires
 * a string (%s) followed by an integer (%d) to create unique temporary
 * filenames based on a base name and process/sequence identifier.
 *
 * Value: "/tmp/%s.%d" (UNIX-style temporary file format)
 * Usage: Used with sprintf() to generate unique temporary file paths
 * Notes: Only defined for non-VMS systems; requires %s and %d placeholders
 * Modernization: Use mkstemp() or modern temporary file APIs for security
 */
#ifndef VMS
#define TMP_FILE_FMT "/tmp/%s.%d"

/*
 * TMP_DIR - Temporary file directory
 *
 * Directory path where temporary files are created. Points to the standard
 * UNIX temporary directory, which should be writable by all users and
 * cleaned regularly by the system.
 *
 * Value: "/tmp" (standard UNIX temporary directory)
 * Usage: Directory for temporary file creation and cleanup operations
 * Notes: Should have proper permissions and cleanup policies
 * Modernization: Use TMPDIR environment variable or system temp directory API
 */
#define TMP_DIR	"/tmp"	/* this is the name of the directory for above */
#else
#endif /* VMS */

/* ============================================================================
 * EDITOR CONFIGURATION
 * ============================================================================
 * Purpose: Configuration for external text editor integration
 * Usage: Controls which editor is used for in-game text editing operations
 * Notes: Supports both environment variable and default fallback
 */

/*
 * ENV_EDITOR - Environment variable for editor selection
 *
 * Name of the environment variable that contains the user's preferred
 * text editor command. This allows users to specify their preferred
 * editor without recompiling the game.
 *
 * Value: "EDITOR" (standard UNIX environment variable for user's editor)
 * Usage: Checked first when determining which editor to spawn
 * Notes: Standard UNIX convention for user editor preference
 * Modernization: Good practice, but ensure proper input validation
 */
#define ENV_EDITOR	"EDITOR"	/* environment string of editor */

/*
 * DEFAULT_EDITOR - Fallback editor command
 *
 * Default text editor command used when the ENV_EDITOR environment
 * variable is not set or contains an invalid value. Uses 'vi' as
 * it's available on virtually all UNIX systems.
 *
 * Value: "/usr/bin/vi" (absolute path to vi editor)
 * Usage: Fallback editor when user preference is not available
 * Notes: Hardcoded path may not be portable; vi is universal but minimal
 * Modernization: Check multiple common editor locations or use which command
 */
#define DEFAULT_EDITOR	"/usr/bin/vi"	/* the name for the default */

/* ============================================================================
 * LEGACY VMS OPERATING SYSTEM SUPPORT
 * ============================================================================
 * Purpose: Configuration and compatibility for VMS (OpenVMS) operating system
 * Usage: Platform-specific settings for VMS environments (mostly obsolete)
 * Notes: VMS support is legacy and likely unused in modern deployments
 */

/*
 * VMS - VMS operating system detection (currently disabled)
 *
 * When defined, enables VMS-specific code paths and configuration settings.
 * Currently commented out, indicating VMS support is disabled or unused.
 *
 * Value: Undefined (VMS support disabled)
 * Usage: Would enable VMS-specific compilation and behavior
 * Notes: VMS is obsolete; support can likely be removed entirely
 * Modernization: Remove VMS support code to simplify maintenance
 */
/* #define VMS */	/* if the machine is running under the VMS OS */

#ifdef VMS
/*
 * VAXC - VAX C compiler flag (VMS-specific)
 *
 * Indicates that the default VMS compiler (VAX C) is being used.
 * This affects compiler-specific code generation and features.
 *
 * Value: Defined when VMS is defined
 * Usage: Compiler-specific code generation for VAX C
 * Notes: VAX C is obsolete; modern VMS uses different compilers
 * Modernization: Remove along with general VMS support
 */
#define VAXC		/* the default VMS compiler */

/*
 * TSERVER - Terminal server support (VMS-specific)
 *
 * Indicates that terminals are controlled via the VMS terminal server
 * system, affecting how terminal I/O and control sequences are handled.
 *
 * Value: Defined when VMS is defined
 * Usage: Terminal handling in VMS terminal server environments
 * Notes: Specific to VMS terminal architecture
 * Modernization: Remove along with general VMS support
 */
#define TSERVER		/* terms controlled via the VMS terminal server */
#endif /* VMS */

/* ============================================================================
 * NULL DEVICE CONFIGURATION
 * ============================================================================
 * Purpose: Platform-specific null device paths for I/O redirection
 * Usage: Used to redirect unwanted input/output to the system null device
 * Notes: Different operating systems use different null device paths
 */

#ifndef VMS
/*
 * NULL_INPUT - Null input device (UNIX)
 *
 * Path to the null device for input redirection on UNIX systems.
 * Reading from /dev/null immediately returns end-of-file, providing
 * a reliable way to provide empty input to processes.
 *
 * Value: "/dev/null" (UNIX null device)
 * Usage: Input redirection for processes that need empty input
 * Notes: Standard across all UNIX and UNIX-like systems
 * Modernization: Portable and still appropriate for modern systems
 */
#define NULL_INPUT "/dev/null"

/*
 * NULL_OUTPUT - Null output device (UNIX)
 *
 * Path to the null device for output redirection on UNIX systems.
 * Writing to /dev/null discards all data, providing a reliable way
 * to suppress unwanted output from processes.
 *
 * Value: "/dev/null" (UNIX null device)
 * Usage: Output redirection to discard unwanted process output
 * Notes: Standard across all UNIX and UNIX-like systems
 * Modernization: Portable and still appropriate for modern systems
 */
#define NULL_OUTPUT "/dev/null"
#else
/*
 * NULL_INPUT/NULL_OUTPUT - Null devices (VMS)
 *
 * VMS versions of null device paths. Set to empty strings, which
 * may indicate a different mechanism for null I/O redirection on VMS.
 *
 * Value: "" (empty string for VMS null redirection)
 * Usage: VMS-specific null I/O handling
 * Notes: VMS-specific implementation details
 * Modernization: Remove along with general VMS support
 */
#define NULL_INPUT ""
#define NULL_OUTPUT ""
#endif /* VMS */

/* ============================================================================
 * OPTIONAL GAME FEATURES AND BEHAVIOR CONTROL
 * ============================================================================
 * Purpose: Enable/disable optional game features and behavioral modifications
 * Usage: Compile-time feature selection for different gameplay experiences
 * Notes: These features can be disabled by commenting out the #define statements
 */

/*
 * SECURITY - Enable god access security restrictions
 *
 * When defined, limits god-level access to specific login names only,
 * preventing unauthorized users from gaining administrative privileges
 * even if they discover god-mode commands or access methods.
 *
 * Value: Defined (security restrictions enabled)
 * Usage: Validates LOGIN constant against actual user login for god access
 * Notes: Essential security feature for multi-user environments
 * Modernization: Should use more sophisticated authentication mechanisms
 */
#define SECURITY	/* limit god access to proper login names only.	*/

/*
 * LISTUSERS - Enable extended user listing information
 *
 * When defined, provides extra information in user listings (using -l flag),
 * including hostname and system information. Requires gethostbyname() or
 * uname() system calls for network and system identification.
 *
 * Value: Defined (extended user info enabled)
 * Usage: Enhanced user listing with network and system details
 * Notes: Useful for debugging and administration but may have privacy implications
 * Modernization: Ensure compatibility with modern networking APIs
 */
#define LISTUSERS	/* extra info -l: need gethostbyname or uname.	*/

/*
 * REMAKE - Allow world rebuilding over existing data
 *
 * When defined, allows the world generation system to rebuild the game
 * world even when a data file already exists, providing a way to reset
 * or regenerate the game world for testing or administrative purposes.
 *
 * Value: Defined (world rebuilding enabled)
 * Usage: Enables world regeneration commands even with existing save data
 * Notes: Potentially dangerous feature that can destroy existing game state
 * Modernization: Should require explicit confirmation and backup creation
 */
#define REMAKE		/* allow world rebuild even if datafile exists	*/

/*
 * CHEAT - Enable NPC assistance features
 *
 * When defined, allows NPC (Non-Player Character) nations to receive
 * small computational or informational advantages to help them compete
 * effectively against human players in the game.
 *
 * Value: Defined (NPC assistance enabled)
 * Usage: Provides NPCs with minor advantages for balanced gameplay
 * Notes: Balance feature to make NPCs competitive without being overpowered
 * Modernization: Consider configurable difficulty levels instead of compile-time flag
 */
#define CHEAT		/* allow npcs to obtain small help		*/

/*
 * SPEW - Enable random NPC messages
 *
 * When defined, enables NPCs to generate random messages and communications,
 * adding personality and immersion to NPC interactions in the game world.
 *
 * Value: Defined (NPC random messages enabled)
 * Usage: NPCs generate random flavor text and communications
 * Notes: Cosmetic feature that enhances game atmosphere
 * Modernization: Could use more sophisticated text generation systems
 */
#define	SPEW		/* spew random messages from npcs 		*/

/*
 * RUNSTOP - Enable player-aware update stopping
 *
 * When defined, prevents world updates from running while players are
 * still active in the game, ensuring that players can complete their
 * actions before the world state changes through automated updates.
 *
 * Value: Defined (update stopping enabled)
 * Usage: Pauses automated updates when players are online
 * Notes: Critical for maintaining game state consistency
 * Modernization: Should be standard behavior, not optional
 */
#define RUNSTOP		/* stop update if players are still in the game	*/

#ifdef RUNSTOP
/*
 * UPDATESLEEP - Update retry interval
 *
 * Time in seconds to wait before retrying an automated update when players
 * are still active in the game. This provides a reasonable interval for
 * players to complete their actions before the next update attempt.
 *
 * Value: 30 seconds (retry interval for blocked updates)
 * Usage: Sleep duration between update attempts when RUNSTOP blocks updates
 * Notes: Essential for automatic update systems to work properly
 * Modernization: Should be configurable based on game pace and player needs
 */
#define UPDATESLEEP 30	/* sleep for UPDATESLEEP seconds and try again	*/
#endif /* RUNSTOP */

/* ============================================================================
 * NPC ARTIFICIAL INTELLIGENCE BEHAVIOR OPTIONS (UNIMPLEMENTED)
 * ============================================================================
 * Purpose: Control NPC "cheating" abilities for game balance (currently disabled)
 * Usage: Would provide NPCs with information advantages if enabled
 * Notes: All options are currently disabled/unimplemented
 */

/*
 * NPC_COUNT_ARMIES - Enable NPC army counting abilities (disabled)
 *
 * When defined, would allow NPC nations to always accurately count enemy
 * armies, even when they are magically disguised or hidden. This represents
 * a form of "cheating" that helps NPCs make better strategic decisions.
 *
 * Value: Undefined (NPC army counting disabled)
 * Usage: Would enhance NPC tactical decision-making capabilities
 * Notes: Currently unimplemented; could be controversial if enabled
 * Modernization: Consider implementing as configurable difficulty option
 */
/* #define NPC_COUNT_ARMIES */	/* defined if NPC nations can always count
				   armies This makes them to cheat by by seeing
				   even magically disguised troops */

/*
 * NPC_SEE_SECTORS - Enable NPC sector vision abilities (disabled)
 *
 * When defined, would allow NPC nations to see all sector attributes
 * even when sectors are magically disguised or hidden from normal view.
 * This information advantage helps NPCs make better economic and strategic decisions.
 *
 * Value: Undefined (NPC sector vision disabled)
 * Usage: Would improve NPC economic and strategic planning
 * Notes: Currently unimplemented; provides significant information advantage
 * Modernization: Consider implementing with configurable visibility levels
 */
/* #define NPC_SEE_SECTORS */	/* defined if NPC nations can always see
				   sectors. This allows them to cheat by being
				   allowed to see all sector attributes of even
				   disguised sectors */

/*
 * NPC_SEE_CITIES - Enable NPC city detection abilities (disabled)
 *
 * When defined, would allow NPC nations to always detect cities and towns
 * even when they appear as VOID sectors to human players. This simulates
 * the human ability to infer city presence through movement patterns and
 * other indirect indicators.
 *
 * Value: Undefined (NPC city detection disabled)
 * Usage: Would help NPCs identify strategic targets and opportunities
 * Notes: Currently unimplemented; simulates human pattern recognition
 * Modernization: Consider implementing as AI enhancement rather than "cheating"
 */
/* #define NPC_SEE_CITIES */	/* defined if NPC nations can always see cities
				   This allows them to cheat by being able to
				   see if a VOID sector is a city/town.
				   Simulates the players ability to tell cities
				   via movement. */

/* ============================================================================
 * CORE GAME BALANCE PARAMETERS *
 ============================================================================
 * Purpose: Fundamental game balance constants that control core
 * mechanics
 * Usage: These values affect game difficulty, progression, and
 * strategic balance
 * Notes: Changes significantly impact gameplay balance and should be tested carefully */

/* NRAND_DIGITS - Non-random number precision
 * Number of decimal digits used in "non-random" number generation
 * systems.
 * This affects the precision of deterministic calculations and
 * pseudo-random sequences used for game balance and reproducible
 * random events.
 * Value: 9 digits (maximum precision without overflow in 32-bit
 * integers)
 * Usage: Deterministic random number generation and game balance
 * calculations
 * Notes: Limited to 9 digits to prevent integer overflow in
 * calculations
 * Modernization: Consider using 64-bit integers for higher
 * precision
*/

#define NRAND_DIGITS 9	/* decimal digits in "non-random" number [<= 9]	*/

/* MIN_WORLD_SIZE - Minimum world dimension
 Minimum allowed value for either axis of the game world map. This
 ensures that generated worlds are large enough to provide
 meaningful gameplay and prevent degenerate cases in world
 generation algorithms.
 Value: 24 sectors (minimum world axis size)
 Usage: World generation validation and minimum size enforcement
 Notes: Ensures sufficient space for nations and strategic
        gameplay
 Modernization: Consider dynamic sizing based on player count
*/
#define MIN_WORLD_SIZE 24  /* minimum value for a world axis.	*/

/* MAXFORTVAL - Maximum fortification level * Maximum internal
   value for sector fortification. Higher fortification provides
   better defensive bonuses but requires more resources to
   achieve. This limit prevents excessive defensive advantages and
   maintains game balance.
   Value: 24 (maximum fortification points)
   Usage: Fortification construction limits and defensive bonus
          calculations
   Notes: Affects combat balance and siege warfare dynamics
   Modernization: Consider making fortification limits configurable */
#define MAXFORTVAL 24 	/* the maximum internal value for fortification	*/

/* MAXCHARITY - Maximum charity/reputation level
 Absolute limit for charity or reputation values that nations can
 achieve. This affects diplomatic relations, trade bonuses, and
 certain game mechanics that depend on a nation's reputation with
 others.
 Value: 25 (maximum charity/reputation points)
 Usage: Diplomatic calculations and reputation-based game mechanics
 Notes: Affects diplomatic and economic interactions between nations
 Modernization: Consider more nuanced reputation systems */
#define MAXCHARITY 25	/* absolute limit which charity may reach.*/

/* * MAXTAX - Maximum taxation rate * * Maximum allowable taxation
     rate that can be set by nations. Higher taxation generates more
     revenue but may cause population unrest or economic penalties. *
     This limit prevents excessive taxation that could break game
     balance. * * Value: 20 (maximum tax rate percentage) * Usage:
     Economic policy limits and revenue calculations * Notes: Affects
     economic balance and population happiness * Modernization:
     Consider dynamic tax limits based on government type */
#define MAXTAX 20	/* maximum setting of the taxation rate.	*/

/* * MAXNEWS - News history retention * * Total number of game turns
     worth of news and events that are saved and * available for
     players to review. This affects memory usage and provides * a
     reasonable history window for players to track important
     events. * * Value: 12 turns (news retention period) * Usage: News
     system storage limits and historical event tracking * Notes:
     Balance between useful history and memory consumption *
     Modernization: Consider configurable retention based on server
     capacity */
#define MAXNEWS 12	/* total number of turns worth of news saved.	*/

/* ==========================================================
 * LEADERSHIP AND EXPERIENCE MECHANICS
 * ==========================================================

 * Purpose: Control character development and leadership advancement
 * Usage: Affects long-term character progression and strategic
 * planning
 * Notes: These values influence the pace of character
 * development */

/* * PLEADER_EXP - Leader experience gain probability * * Percentage
     chance for a leader to gain 1 experience point per year. * This
     controls the rate of character development and long-term
     strategic * planning around leader advancement and
     capabilities. * * Value: 100% (guaranteed experience gain per
     year) * Usage: Annual leader development and experience point
     calculations * Notes: Affects long-term strategic planning and
     character progression * Modernization: Consider more varied
     experience gain mechanisms */
#define PLEADER_EXP 100	/* % chance for leader to gain 1 exp pt / year	*/

/* ============================================================================
 * * MOVEMENT AND ATTRITION MECHANICS *
 * ============================================================================
 * * Purpose: Control unit movement costs and attrition rates * Usage:
 * Affects strategic mobility and resource management * Notes: These
 * values significantly impact tactical and strategic gameplay */

/* * LONGTRIP - Naval civilian attrition threshold * * Navy trip
     length at which 100% attrition of civilians occurs. Longer *
     naval journeys become increasingly dangerous for civilian
     passengers, * encouraging strategic planning of naval operations
     and supply lines. * * Value: 100 (trip length for maximum
     civilian attrition) * Usage: Naval attrition calculations and
     civilian transport mechanics * Notes: Encourages strategic naval
     base placement and route planning * Modernization: Consider more
     sophisticated attrition models */

#define LONGTRIP 100	/* navy trip lth for 100% attrition of civs	*/

/* * SPEEDUP_COST - Movement speed increase cost * * Movement point
     decrease applied just before increasing unit speed. * This
     creates a cost for achieving higher movement rates and balances *
     the advantage of increased mobility against resource
     expenditure. * * Value: 1 (movement point cost for speed
     increase) * Usage: Movement speed calculations and mobility cost
     balancing * Notes: Affects tactical movement decisions and
     resource allocation * Modernization: Consider variable costs
     based on unit type and terrain */

#define SPEEDUP_COST 1	/* move decrease just before increasing speed	*/

 /* ============================================================================
  * * POPULATION AND RESOURCE MANAGEMENT *
  * ============================================================================
  * * Purpose: Control population dynamics and resource consumption *
  * Usage: Affects economic planning and resource allocation
  * strategies * Notes: These percentages significantly impact
  * economic game balance */

/* * PSTARVE - Starvation mortality rate * * Percentage of population
     that dies when not adequately fed. This creates * pressure for
     food production and distribution, making agriculture and * trade
     essential for maintaining large populations. * * Value: 25%
     (population loss rate during starvation) * Usage: Population
     management and food requirement calculations * Notes: Critical
     for economic balance and food production incentives *
     Modernization: Consider graduated starvation effects */
#define PSTARVE	25	/* % of population that starves if not fed	*/

/* * PCOLLAPSE - Infrastructure deterioration rate * * Percentage
     chance of unsupplied sector infrastructure deteriorating per
     turn. * This creates ongoing maintenance requirements and
     prevents players from * ignoring infrastructure after initial
     construction. * * Value: 10% (deterioration chance for unsupplied
     infrastructure) * Usage: Infrastructure maintenance calculations
     and decay mechanics * Notes: Encourages ongoing resource
     allocation to infrastructure * Modernization: Consider variable
     decay rates based on infrastructure type */
#define PCOLLAPSE 10	/* % change of unsupplied sector deterioration	*/

/* * PDISBAND - Unit desertion rate * * Percentage of a military unit
     that disbands when lacking adequate supplies. * This creates
     logistical pressure and prevents indefinite maintenance of *
     large armies without proper supply lines. * * Value: 10% (unit
     strength loss rate without supplies) * Usage: Military supply
     calculations and unit maintenance mechanics * Notes: Critical for
     military logistics and strategic planning * Modernization:
     Consider morale-based desertion mechanics */
#define PDISBAND 10	/* % of a unit that disbands w/out supplies.	*/

/* * PVOLUNTEERS - Military recruitment rate * * Percentage of a
     sector's population available for military duty. This * controls
     the rate at which civilian populations can be converted to *
     military forces, affecting military buildup capabilities. * *
     Value: 20% (population available for military recruitment) *
     Usage: Military recruitment calculations and population
     mobilization * Notes: Affects military buildup speed and
     population economics * Modernization: Consider variable rates
     based on government type and crisis */
#define PVOLUNTEERS 20 /* % of a sector population available for duty	*/

/* * PBARNICLES - Naval maintenance failure rate * * Percentage chance
     for ship damage when naval units lack adequate supplies. * This
     represents hull fouling, equipment failure, and other maintenance
     * issues that affect naval readiness and operational
     capability. * * Value: 2% (chance of ship damage without proper
     maintenance) * Usage: Naval maintenance calculations and fleet
     readiness mechanics * Notes: Encourages regular naval supply and
     maintenance operations * Modernization: Consider cumulative
     damage over time */
#define PBARNICLES 2	/* % chance for ship damage w/out supplies	*/

/* ============================================================================
 * * NEW PLAYER MECHANICS *
 * ============================================================================
 * * Purpose: Control late-joining player compensation and balance *
 * Usage: Affects game balance when new players join ongoing games *
 * Notes: Helps maintain competitive balance for late-starting
 * nations */

/* * LATESTART - Late starter compensation rate * * Compensation rate
     for new nations that start after the game has begun. * New
     nations receive 1 point per LATESTART turns to help them catch up
     * to established nations and remain competitive in ongoing
     games. * * Value: 2 (1 point per 2 turns of late start
     compensation) * Usage: New player compensation calculations and
     game balance mechanics * Notes: Helps maintain competitive
     gameplay for late-joining players * Modernization: Consider more
     sophisticated catch-up mechanics */
#define	LATESTART 2	/* 1 pt / LATESTART turns after beginning for
			   new nations when they start late in the
			   game */

 /* ============================================================================
  * * COMBAT STRENGTH AND BONUS MECHANICS *
  * ============================================================================
  * * Purpose: Control combat effectiveness and strength-based bonuses
  * * Usage: Affects combat calculations and unit effectiveness
  * scaling * Notes: These values significantly impact combat balance
  * and unit utility */

/* * MIN_GAIN_STR - Minimum strength for attack bonus * * Minimum unit
     strength required before gaining attack bonus benefits. * Units
     below this threshold fight at base effectiveness, while stronger
     * units receive combat bonuses that scale with their strength. *
     * Value: 250 (minimum strength for bonus eligibility) * Usage:
     Combat bonus calculations and unit effectiveness thresholds *
     Notes: Creates meaningful distinction between weak and strong
     units * Modernization: Consider more granular strength
     thresholds */
#define MIN_GAIN_STR 250 /* minimum strength needed to gain attack bonus */

/* * PROB_GAIN_STR - Full attack bonus threshold * * Unit strength at
     which there is 100% chance to gain attack bonus. * This creates a
     scaling system where stronger units are increasingly * likely to
     receive combat bonuses, up to guaranteed bonuses at this
     threshold. * * Value: 1500 (strength for guaranteed attack bonus)
     * Usage: Combat bonus probability calculations and unit scaling
     mechanics * Notes: Provides linear scaling for combat
     effectiveness with unit strength * Modernization: Consider
     non-linear scaling or cap effects */
#define PROB_GAIN_STR 1500 /* 100% chance to gain attack bonus for strength */

/* ============================================================================
 * * VISIBILITY AND ENGAGEMENT RANGES *
 * ============================================================================
 * * Purpose: Control visual range, engagement distances, and
 * strategic awareness * Usage: Affects tactical planning,
 * reconnaissance, and strategic positioning * Notes: Larger values
 * increase CPU usage but provide more tactical depth */

/* * LANDSEE - Land-based visibility range * * How far players can see
     from land-based sectors they control. This affects *
     reconnaissance capabilities, early warning systems, and strategic
     planning * around controlled territory. * * Value: 2 sectors
     (land-based sight range) * Usage: Map visibility calculations and
     territorial awareness * Notes: CPU usage increases with larger
     values; affects strategic positioning * Modernization: Consider
     terrain-based visibility modifiers */

#define LANDSEE 2	/* how far you can see from your land		*/

/* * NAVYSEE - Naval unit visibility range * * How far naval units can
     see around their current position. This affects * naval
     reconnaissance, threat detection, and strategic naval positioning
     * for both offensive and defensive operations. * * Value: 1
     sector (naval unit sight range) * Usage: Naval reconnaissance and
     threat detection calculations * Notes: Limited range encourages
     close-range naval tactics * Modernization: Consider ship type and
     weather effects on visibility */

#define NAVYSEE 1	/* how far navies can see around them		*/

/* * ARMYSEE - Army unit visibility range * * How far army units can
     see around their current position. This affects * military
     reconnaissance, tactical awareness, and early warning
     capabilities * for land-based military operations. * * Value: 2
     sectors (army unit sight range) * Usage: Military reconnaissance
     and tactical awareness calculations * Notes: Larger range than
     naval units reflects land-based advantage * Modernization:
     Consider unit type and terrain effects on visibility */

#define ARMYSEE 2	/* how far armies can see around them		*/

/* * CVNSEE - Caravan visibility range * * How far caravan units can
     see around their current position. Limited * visibility reflects
     the civilian nature of caravans and their focus * on trade rather
     than reconnaissance activities. * * Value: 1 sector (caravan
     sight range) * Usage: Trade route reconnaissance and threat
     awareness * Notes: Limited range reflects civilian/commercial
     nature * Modernization: Consider escort effects on caravan
     visibility */

#define CVNSEE 1	/* how far caravans can see around them		*/

/* * PRTZONE - Pirate operational range * * Maximum distance pirates
     will roam from their base camp when conducting * raids and
     attacks. This controls pirate behavior and creates predictable *
     safe zones at sufficient distance from pirate bases. * * Value: 3
     sectors (pirate operational radius) * Usage: Pirate AI behavior
     and threat zone calculations * Notes: Creates tactical
     considerations for trade route planning * Modernization: Consider
     dynamic ranges based on pirate strength */

#define PRTZONE	3	/* how far pirates roam from their basecamp	*/

/* * MEETNTN - Diplomatic contact range * * Maximum distance between
     nations required for diplomatic status adjustments. * Nations
     must be within this range to establish or modify diplomatic *
     relationships, encouraging geographic proximity for diplomacy. *
     * Value: 2 sectors (diplomatic contact range) * Usage: Diplomatic
     system calculations and relationship management * Notes:
     Encourages geographic considerations in diplomacy *
     Modernization: Consider communication technology effects on
     range */
#define MEETNTN	2	/* how close nations must be to adjust status	*/

/* * NAVYRANGE - Naval engagement range * * Maximum distance at which
     naval fleets can engage each other in combat. * This controls
     naval tactical positioning and determines when naval * battles
     can be initiated between opposing fleets. * * Value: 3 sectors
     (naval engagement range) * Usage: Naval combat initiation and
     tactical positioning calculations * Notes: Longer range than
     visibility allows for strategic positioning * Modernization:
     Consider ship type and weapon technology effects */
#define NAVYRANGE 3	/* how close fleets need to be to engage	*/

/* * VISRANGE - Sector identification range * * Range within which
     sector numbers are automatically known to players. * This
     provides basic geographic awareness of nearby areas without *
     requiring detailed reconnaissance or exploration. * * Value: 4
     sectors (automatic sector identification range) * Usage: Map
     display and geographic awareness calculations * Notes: Provides
     basic navigation information for strategic planning *
     Modernization: Consider map-making technology and exploration
     bonuses */
#define VISRANGE 4	/* sector number known if within this range	*/

/* ============================================================================
 * * PAGER INTERFACE CONFIGURATION *
 * ============================================================================
 * * Purpose: Control the built-in text pager system for viewing large
 * files * Usage: Affects user interface behavior when displaying
 * large text content * Notes: These settings balance usability with
 * memory consumption */

 /* * MAX_FILE_LINES - Maximum pager file size * * Maximum number of
      lines that the internal pager system will read and * display
      from a single file. This prevents excessive memory usage when *
      viewing very large files while providing reasonable file viewing
      capabilities. * * Value: 5000 lines (maximum file size for
      pager) * Usage: File size validation before loading into pager
      system * Notes: Balance between functionality and memory
      consumption * Modernization: Consider dynamic sizing based on
      available memory */
#define MAX_FILE_LINES 5000	/* max file size for pager to read in	*/

/* * D_PAGEOFF - Default pager offset * * Default offset of the
     current line from the top of the display in the * pager
     interface. This controls the initial positioning when opening *
     files in the pager system. * * Value: 0 (start at top of file) *
     Usage: Initial pager display positioning and default view
     settings * Notes: Starting at top provides predictable user
     experience * Modernization: Consider user-configurable default
     positioning */
#define D_PAGEOFF 0	/* offset of current line from the top in pager	*/

/* * D_PAGETAB - Tab spacing in pager * * Number of spaces used for
     tab character expansion in the pager display. * This controls
     text formatting and alignment when viewing files that * contain
     tab characters. * * Value: 8 spaces (standard tab width) * Usage:
     Tab expansion and text formatting in pager display * Notes:
     Standard 8-space tabs provide consistent formatting *
     Modernization: Consider configurable tab stops for user
     preference */
#define D_PAGETAB 8	/* spacing for tabs in the pager		*/

/* ============================================================================
 * * SECTOR MANAGEMENT AND POPULATION LIMITS *
 * ============================================================================
 * * Purpose: Control sector development, population dynamics, and
 * resource management * Usage: Affects economic planning, population
 * growth, and resource extraction * Notes: These limits significantly
 * impact economic game balance and strategy */

/* * DESFOOD - Minimum food for sector redesignation * * Minimum food
     production value required before a sector can be redesignated *
     to a different type. This prevents wasteful redesignation of
     productive * agricultural sectors and encourages thoughtful
     economic planning. * * Value: 4 (minimum food production for
     redesignation) * Usage: Sector redesignation validation and
     economic planning * Notes: Protects food production from
     accidental or wasteful changes * Modernization: Consider variable
     requirements based on sector type */
#define DESFOOD	4		/* min food val to redesignate sector	*/

/* * TOOMANYPEOPLE - Population efficiency threshold * * Population
     level at which a sector becomes overcrowded, resulting in *
     reduced reproduction rates and production efficiency. This
     encourages * population distribution and city development for
     large populations. * * Value: 5000 people (overcrowding threshold
     for non-city sectors) * Usage: Population efficiency calculations
     and reproduction rate adjustments * Notes: Does not apply to
     cities, which can handle larger populations efficiently *
     Modernization: Consider sector type and infrastructure effects on
     capacity */
#define TOOMANYPEOPLE 5000L	/* too many people in sector - 1/2 repro
				   and 1/2 production; not in cities.	*/

/* * ABSMAXPEOPLE - Absolute population limit * * Hard limit on the
     maximum number of people that can exist in any single * sector,
     including cities. This prevents infinite population growth and *
     maintains game balance by forcing territorial expansion. * *
     Value: 50000 people (absolute maximum population per sector) *
     Usage: Population growth limits and sector capacity enforcement *
     Notes: Applies to all sector types including cities *
     Modernization: Consider dynamic limits based on infrastructure
     development */
#define ABSMAXPEOPLE 50000L	/* absolute max people in any sector	*/

/* * MILLSIZE - Minimum workforce for mill operation * * Minimum
     number of people required to operate a mill effectively. This *
     creates workforce requirements for industrial development and
     encourages * population growth in industrial sectors. * * Value:
     500 people (minimum mill workforce) * Usage: Industrial
     development calculations and workforce requirements * Notes:
     Affects industrial development strategy and population allocation
     * Modernization: Consider technology effects on workforce
     requirements */
#define	MILLSIZE 500L		/* min number of people to work a mill	*/

/* * TOOMUCHMINED - Metal depletion threshold * * Number of metal
     units that must be mined for a 100% chance of metal * depletion
     in a sector. The actual depletion chance is prorated based * on
     the amount mined relative to this threshold. * * Value: 50000
     units (threshold for guaranteed metal depletion) * Usage:
     Resource depletion calculations and mining sustainability *
     Notes: Encourages sustainable mining practices and resource
     management * Modernization: Consider regeneration mechanics for
     depleted resources */
#define TOOMUCHMINED 50000L	/* units mined for 100% chance of metal
				   depletion actual chance is prorated	*/

/* ============================================================================
 * * ECONOMIC COSTS AND RESOURCE REQUIREMENTS *
 * ============================================================================
 * * Purpose: Control economic costs for various game actions and
 * constructions * Usage: Affects economic planning, resource
 * allocation, and strategic decisions * Notes: These costs
 * significantly impact economic balance and player strategies */

/* * MOVECOST - Command execution cost * * Cost in talons (game
     currency) for each command entered by a player. * This creates a
     basic resource cost for game actions and prevents * excessive
     micromanagement by making every action have an economic impact. *
     * Value: 20 talons (cost per command) * Usage: Economic
     calculations for player actions and command processing * Notes:
     Creates economic pressure and prevents excessive micromanagement
     * Modernization: Consider variable costs based on command
     complexity */
#define MOVECOST 20L		/* talons cost for each command entered	*/

/* * PEOPLE_MCOST - Civilian movement cost * * Cost for moving one
     civilian using the move_people command. This creates * economic
     costs for population redistribution and encourages careful *
     planning of population movements. * * Value: 50 talons (cost per
     civilian moved) * Usage: Population movement calculations and
     demographic planning * Notes: Makes population redistribution a
     significant economic decision * Modernization: Consider
     distance-based costs and transportation efficiency */
#define PEOPLE_MCOST 50L	/* cost for one civilian in move_people	*/

/* * NAVYMAINT - Naval maintenance cost * * Maintenance cost per naval
     hold capacity unit. This creates ongoing * economic pressure for
     maintaining naval forces and encourages efficient * fleet
     composition and strategic naval planning. * * Value: 4000 talons
     (maintenance cost per naval hold unit) * Usage: Naval economics
     and fleet maintenance calculations * Notes: High cost encourages
     strategic naval force management * Modernization: Consider ship
     type and age effects on maintenance costs */
#define NAVYMAINT 4000L		/* navy maintainance cost / hold	*/

/* * CVNMAINT - Caravan maintenance cost * * Maintenance cost for
     caravan units. This creates ongoing economic * costs for
     maintaining trade networks and encourages efficient * caravan
     management and route planning. * * Value: 1000 talons (caravan
     maintenance cost) * Usage: Trade economics and caravan fleet
     management * Notes: Lower than naval costs, reflecting civilian
     nature of caravans * Modernization: Consider route efficiency and
     cargo effects on costs */
#define CVNMAINT 1000L		/* caravan maintainance cost		*/

/* * FORTCOST - Fortification construction cost * * Cost to build one
     point of fortification in a sector. This affects * defensive
     strategy economics and creates trade-offs between offensive * and
     defensive investments. * * Value: 1000 talons (cost per
     fortification point) * Usage: Military engineering costs and
     defensive investment calculations * Notes: Affects military
     strategy and resource allocation decisions * Modernization:
     Consider terrain and technology effects on construction costs */
#define FORTCOST 1000L		/* cost to build a fort point		*/

/* * CARAVANCOST - Caravan construction cost * * Cost to construct
     caravan units, specified per 10 wagons. This affects * trade
     network development costs and economic expansion strategies. * *
     Value: 5000 talons (cost per 10-wagon caravan unit) * Usage:
     Trade network development and economic expansion planning *
     Notes: Significant investment required for trade network
     expansion * Modernization: Consider technology and infrastructure
     effects on construction costs */
#define CARAVANCOST 5000L	/* cost for caravans (per 10 wagons)	*/

/* * CARAVANWOOD - Caravan wood requirement * * Amount of wood
     required to construct caravan units, specified per * 10
     wagons. This creates resource requirements for trade network *
     development beyond just monetary costs. * * Value: 400 wood units
     (per 10-wagon caravan unit) * Usage: Resource planning for
     caravan construction and trade expansion * Notes: Creates
     resource-based constraints on trade network development *
     Modernization: Consider alternative materials and construction
     technologies */
#define CARAVANWOOD 400L	/* how much wood per 10 wagons		*/

/* ============================================================================
 * * COMBAT AND TACTICAL MECHANICS *
 * ============================================================================
 * * Purpose: Control combat effectiveness, siege mechanics, and
 * tactical operations * Usage: Affects military strategy, siege
 * warfare, and tactical decision-making * Notes: These values
 * significantly impact combat balance and military tactics */

/* * TAKESECTOR - Base capture requirement * * Base number of soldiers
     required to capture a sector from enemy control. * This
     establishes minimum force requirements for territorial conquest *
     and affects strategic planning for military operations. * *
     Value: 75 soldiers (base capture requirement) * Usage: Siege
     calculations and territorial conquest mechanics * Notes: Modified
     by fortification levels and defender strength * Modernization:
     Consider terrain and fortification effects on requirements */
#define TAKESECTOR 75		/* base number of soldiers for capture  */

/* * TAKE_RATIO - Capture force ratio * * Ratio of attacking to
     defending forces required to successfully capture * a
     sector. This creates strategic depth in siege warfare and
     encourages * concentration of forces for successful attacks. * *
     Value: 7:1 (attacker to defender ratio for capture) * Usage:
     Siege combat calculations and force requirement determination *
     Notes: Creates significant advantage for defenders in siege
     situations * Modernization: Consider variable ratios based on
     fortification and terrain */
#define TAKE_RATIO 7		/* Ratio N:1 needed to take a sector	*/

/* * BASE_TAKEPCT - Capture force percentage * * Percentage of
     civilian population that attacking troops must represent * to
     successfully capture a sector. This scales capture requirements *
     with population size and prevents easy conquest of populated
     areas. * * Value: 10% (troop percentage of civilian population
     for capture) * Usage: Population-based capture calculations and
     urban warfare mechanics * Notes: Makes heavily populated areas
     harder to conquer * Modernization: Consider government type and
     civilian resistance effects */
#define BASE_TAKEPCT 10		/* Troop size, % of civs, for capturing */

/* * MAXLOSS - Maximum battle casualties * * Maximum percentage of
     military units that can be lost in a single * 1:1 battle
     engagement. This prevents total unit annihilation and * maintains
     some military capacity even after devastating defeats. * * Value:
     60% (maximum casualty rate in balanced combat) * Usage: Combat
     resolution and casualty calculations * Notes: Ensures some
     military survivors even in decisive defeats * Modernization:
     Consider morale and experience effects on casualty rates */
#define MAXLOSS	 60		/* maximum % of men lost in 1:1 battle	*/

/* * FINDPERCENT - Resource discovery chance * * Percentage chance to
     discover gold or metal deposits when prospecting * in a
     sector. This controls the rate of resource discovery and affects
     * economic expansion and exploration strategies. * * Value: 1%
     (chance to find precious resources per prospecting attempt) *
     Usage: Resource discovery mechanics and exploration economics *
     Notes: Low percentage makes resource discovery a long-term
     investment * Modernization: Consider terrain type and technology
     effects on discovery rates */
#define	FINDPERCENT 1		/* percent to find gold/metal in sector	*/

/* ============================================================================
 * * LOGISTICS AND MOVEMENT COSTS *
 * ============================================================================
 * * Purpose: Control movement penalties for loading/unloading
 * operations * Usage: Affects tactical planning and logistical
 * considerations * Notes: Different costs for cities versus other
 * locations reflect infrastructure */

/* * LOAD_CITYCOST - Urban loading cost * * Movement points lost when
     loading or unloading cargo in cities. * Lower cost reflects
     better infrastructure and handling facilities * available in
     urban areas. * * Value: 4 movement points (urban
     loading/unloading penalty) * Usage: Logistical planning and urban
     tactical considerations * Notes: Infrastructure advantage makes
     cities valuable for logistics * Modernization: Consider city size
     and infrastructure development effects */
#define LOAD_CITYCOST 4		/* move lost in (un)loading in cities	*/

/* * LOAD_LANDCOST - Rural loading cost * * Movement points lost when
     loading or unloading cargo in non-urban * areas. Higher cost
     reflects lack of infrastructure and handling * equipment in rural
     or undeveloped areas. * * Value: 12 movement points (rural
     loading/unloading penalty) * Usage: Logistical planning and rural
     tactical considerations * Notes: Penalty encourages development
     of urban logistics centers * Modernization: Consider road
     networks and infrastructure development effects */
#define LOAD_LANDCOST 12	/* move lost in (un)loading elsewhere	*/


/* ============================================================================
 * * UNIT SPECIFICATIONS AND CAPABILITIES *
 * ============================================================================
 * * Purpose: Define unit characteristics, capacities, and
 * organizational costs * Usage: Affects military organization,
 * logistics, and economic planning * Notes: These values determine
 * fundamental unit capabilities and limitations */

/* * MAXNAVYCREW - Maximum naval crew strength * * Full strength crew
     complement for a naval fleet unit. This represents * the optimal
     manning level for maximum combat effectiveness and * operational
     capability of naval forces. * * Value: 100 crew members (full
     strength naval unit) * Usage: Naval combat calculations and crew
     management mechanics * Notes: Affects naval combat effectiveness
     and operational readiness * Modernization: Consider ship type
     variations and technology effects */
#define MAXNAVYCREW 100		/* full strength crew on a naval fleet	*/

/* * MAXCVNCREW - Maximum caravan crew strength * * Full strength crew
     complement for a single caravan unit. This represents * the
     optimal manning level for maximum efficiency and security of *
     trade operations. * * Value: 30 crew members (full strength
     caravan unit) * Usage: Caravan operations and trade efficiency
     calculations * Notes: Affects trade security and operational
     effectiveness * Modernization: Consider caravan size and route
     danger effects */
#define MAXCVNCREW 30		/* full strength crew on a 1 "caravan"	*/

/* * NAVY_HOLD - Naval cargo capacity * * Storage space available in a
     single naval unit for transporting * cargo, supplies, and
     passengers. This affects naval logistics * and strategic mobility
     capabilities. * * Value: 100000 units (naval cargo capacity) *
     Usage: Naval logistics calculations and cargo transport planning
     * Notes: Large capacity reflects naval vessels' strategic
     transport role * Modernization: Consider ship type variations and
     cargo handling technology */
#define NAVY_HOLD 100000L	/* storage space of a ship unit		*/

/* * CVN_HOLD - Caravan cargo capacity * * Storage space available in
     a single caravan wagon for transporting * trade goods and
     supplies. This affects trade economics and * commercial transport
     planning. * * Value: 50000 units (caravan wagon capacity) *
     Usage: Trade calculations and commercial transport planning *
     Notes: Half naval capacity reflects smaller, land-based transport
     * Modernization: Consider wagon technology and road
     infrastructure effects */
#define CVN_HOLD 50000L		/* storage space of a caravan wagon	*/

/* ============================================================================
 * * UNIT ORGANIZATION COSTS *
 * ============================================================================
 * * Purpose: Control costs for maintaining separate military and
 * commercial units * Usage: Affects organizational strategy and force
 * structure decisions * Notes: These costs encourage consolidation
 * while allowing tactical flexibility */

/* * ARMYUNITCOST - Army unit organization cost * * Additional cost
     per separate army unit beyond basic maintenance. * This
     represents command overhead, logistics complexity, and *
     organizational costs of maintaining multiple army formations. * *
     Value: 500 talons (additional cost per separate army unit) *
     Usage: Military organization economics and force structure
     planning * Notes: Encourages consolidation while allowing
     tactical flexibility * Modernization: Consider command efficiency
     and communication technology effects */
#define ARMYUNITCOST 500L	/* added cost per separate army unit	*/

/* * NAVYUNITCOST - Naval unit organization cost * * Additional cost
     per separate naval unit beyond basic maintenance. * Higher cost
     than army units reflects greater complexity and * resource
     requirements of naval operations and maintenance. * * Value: 1000
     talons (additional cost per separate naval unit) * Usage: Naval
     organization economics and fleet structure planning * Notes:
     Higher cost reflects naval operational complexity *
     Modernization: Consider port infrastructure and naval technology
     effects */
#define NAVYUNITCOST 1000L	/* added cost per separate navy unit	*/

/* * CVNUNITCOST - Caravan unit organization cost * * Additional cost
     per separate caravan unit beyond basic maintenance. * Same as
     army units, reflecting similar organizational complexity * for
     land-based commercial operations. * * Value: 500 talons
     (additional cost per separate caravan unit) * Usage: Trade
     organization economics and commercial fleet planning * Notes:
     Equal to army costs, reflecting similar organizational complexity
     * Modernization: Consider trade route efficiency and commercial
     technology effects */

#define CVNUNITCOST 500L	/* added cost per separate caravan unit	*/

/* * WAGONS_IN_CVN - Caravan unit composition * * Number of individual
     wagons that make up a single caravan size unit. * This affects
     unit scaling, capacity calculations, and organizational *
     structure of commercial transport operations. * * Value: 10
     wagons (per caravan size unit) * Usage: Caravan capacity
     calculations and unit scaling mechanics * Notes: Provides
     granular control over trade capacity and organization *
     Modernization: Consider wagon technology and transport efficiency
     improvements */
#define WAGONS_IN_CVN 10	/* number of wagons per cvn size unit	*/

/* ============================================================================
 * * NPC ARTIFICIAL INTELLIGENCE BEHAVIOR PARAMETERS *
 * ============================================================================
 * Purpose: Control NPC nation behavior, development patterns, and
 * military organization
 *
 * Usage: Affects NPC competitiveness,territorial behavior, and
 * strategic planning
 *
 * Notes: These settings significantly impact game balance and NPC
 * challenge level
 */

/* * CITYLIMIT - NPC urbanization threshold

 * Percentage of NPC population in a sector required before the sector
   is developed into a city. This controls NPC urbanization patterns
   and city development strategies.

  * * Value: 5% (population threshold for
     NPC city development) * Usage: NPC city development decisions and
     urbanization calculations * Notes: Affects NPC economic
     development and population distribution * Modernization: Consider
     economic factors and strategic location effects */
#define CITYLIMIT 5L		/* % of npc pop in sctr before => city
				 * */
/* * CITYPERCENT - NPC urban population limit * * Maximum percentage
     of total NPC population that can live in cities. * This controls
     NPC urban development and ensures balanced population *
     distribution between urban and rural areas. * * Value: 20%
     (maximum urban population percentage for NPCs) * Usage: NPC
     population distribution and urban development planning * Notes:
     Prevents excessive urbanization and maintains rural population
     base * Modernization: Consider economic development effects on
     urbanization rates */
#define CITYPERCENT 20L		/* % of npc pop able to be in cities
				 * */

/* * MILRATIO - NPC civilian to military ratio * * Ratio of civilian
     to military population maintained by NPC nations. * This controls
     NPC military buildup and ensures sustainable population *
     distribution between civilian and military sectors. * * Value:
     8:1 (civilian to military ratio for NPCs) * Usage: NPC military
     planning and population allocation decisions * Notes: Militia are
     not considered military for this calculation * Modernization:
     Consider threat level and strategic situation effects */
#define MILRATIO 8L /* ratio civ:mil for NPCs		*/

/* * MILINCAP - NPC capital military concentration * * Ratio of
     military forces in the capital to total military forces * for NPC
     nations. This controls defensive concentration and strategic *
     reserve allocation in NPC military planning. * * Value: 8:1
     (capital military to total military ratio for NPCs) * Usage: NPC
     military deployment and capital defense planning * Notes: Ensures
     significant defensive reserves in NPC capitals * Modernization:
     Consider strategic situation and threat assessment effects */
#define MILINCAP 8L		/* ratio (mil in cap):mil for NPCs	*/

/* * MILINCITY - Urban militia organization * * Population to militia
     ratio in NPC cities and capitals. This determines * local defense
     capabilities and urban security forces maintained * by NPC
     nations in their urban centers. * * Value: 10:1 (population to
     militia ratio in NPC urban areas) * Usage: NPC urban defense
     calculations and local security planning * Notes: Provides basic
     urban defense without excessive militarization * Modernization:
     Consider city size and strategic importance effects */
#define	MILINCITY 10L		/* militia=people/MILINCITY in city/cap */

/* * NPCTOOFAR - NPC territorial range limit * * Maximum distance from
     their capital that NPCs will normally operate * or expand. This
     controls NPC territorial behavior and prevents * excessive
     expansion that would dilute their defensive capabilities. * *
     Value: 15 sectors (maximum NPC operational range from capital) *
     Usage: NPC expansion decisions and territorial management *
     Notes: Encourages concentrated development around NPC capitals *
     Modernization: Consider transportation technology and
     communication effects */

#define NPCTOOFAR 15		/* npcs stay within this distance of cap*/

/* * METALORE - Weapon improvement resource requirement * * Amount of
     metal per soldier required to achieve a 1% improvement * in
     weapons technology. This controls NPC military technology *
     advancement and resource allocation for military improvements. *
     * Value: 7 metal units per soldier (for 1% weapon improvement) *
     Usage: NPC military technology development and resource planning
     * Notes: Creates resource cost for military technology
     advancement * Modernization: Consider research technology and
     industrial capacity effects */
#define METALORE 7L		/* metal/soldier needed for +1% weapons	*/

/* ============================================================================
 * * CAMPAIGN CONFIGURATION REFERENCE *
 * ============================================================================
 * * Purpose: Reference to additional campaign-specific settings *
 * Usage: Points developers to location of campaign balance parameters
 * * Notes: Separates general system settings from specific campaign
 * balance */

/* * Campaign Settings Reference * * For campaign-specific balance
     parameters, economic settings, and * scenario configurations, see
     the buildA.h header file. This separation * allows system-wide
     settings to remain stable while enabling * campaign-specific
     customization. * * Location: buildA.h * Purpose:
     Campaign-specific balance and scenario configuration * Usage:
     Modify campaign balance without affecting core system
     parameters */

/* To change the default campaign settings, see buildA.h */

/* ============================================================================
 * SYSTEM INTEGRATION ANALYSIS
 * ============================================================================
 *
 * This header file serves as the central configuration hub for the entire
 * Conquer game system. Every source file in the project includes this header,
 * making it the foundation for system-wide behavior and compatibility.
 *
 * CRITICAL DEPENDENCIES:
 * - All .c files in Src/ directory depend on these constants
 * - Data file format compatibility requires stable primary adjustment values
 * - Cross-platform compatibility depends on platform detection logic
 * - Game balance depends on parameter settings and NPC behavior constants
 *
 * CONFIGURATION CATEGORIES:
 * 1. System Identity: VERSION, OWNER, LOGIN constants
 * 2. Data Format Control: COMPRESS, MANY_UNITS, HUGE_MAP, ABSMAXNTN
 * 3. Platform Compatibility: VMS support, file paths, null devices
 * 4. Security Settings: File permissions, editor access, god-mode control
 * 5. Game Balance: Economic costs, combat parameters, population limits
 * 6. NPC Behavior: AI decision-making parameters and limitations
 * 7. User Interface: Pager settings, visibility ranges, interaction costs
 *
 * MODERNIZATION PRIORITY ANALYSIS:
 *
 * HIGH PRIORITY (Security and Compatibility):
 * - Remove VMS support code (obsolete, complicates maintenance)
 * - Replace hardcoded paths with environment variables or config files
 * - Update compression system to use modern libraries (gzip, zlib)
 * - Implement proper temporary file creation (mkstemp instead of sprintf)
 * - Add input validation for external commands and file paths
 *
 * MEDIUM PRIORITY (Usability and Flexibility):
 * - Make admin credentials configurable via config file
 * - Replace compile-time flags with runtime configuration options
 * - Add dynamic memory allocation instead of fixed array limits
 * - Implement configurable game balance parameters
 * - Add support for multiple admin users instead of single LOGIN
 *
 * LOW PRIORITY (Enhancement):
 * - Implement semantic versioning for VERSION string
 * - Add configuration validation and error reporting
 * - Implement more sophisticated NPC AI difficulty levels
 * - Add support for modern notification systems (replace SYSMAIL)
 * - Implement dynamic resource discovery and depletion mechanics
 *
 * BACKWARD COMPATIBILITY CONSIDERATIONS:
 * - Primary adjustment values (COMPRESS, MANY_UNITS, HUGE_MAP, ABSMAXNTN)
 *   directly affect data file format and save game compatibility
 * - Changing game balance parameters affects existing game balance
 * - Platform-specific code removal requires careful testing across target systems
 * - External command dependencies need verification on target platforms
 *
 * SECURITY IMPLICATIONS:
 * - LOGIN constant controls administrative access (security-critical)
 * - External command execution (COMP_NAME, REMOVE_NAME) creates security risks
 * - File permission constants affect multi-user security
 * - Editor spawning capability requires security review
 * - Hardcoded paths could be exploited if not properly validated
 *
 * PERFORMANCE IMPLICATIONS:
 * - Visibility range constants directly affect CPU usage (O(n²) algorithms)
 * - Large ABSMAXNTN values affect diplomatic calculation complexity
 * - Compression settings affect disk I/O vs CPU trade-offs
 * - Population limits affect memory usage and calculation complexity
 *
 * MAINTENANCE RECOMMENDATIONS:
 * 1. Migrate security-critical settings to external configuration
 * 2. Remove obsolete platform support (VMS) to simplify codebase
 * 3. Implement feature detection instead of compile-time platform flags
 * 4. Add configuration validation and error handling
 * 5. Document interdependencies between constants and game mechanics
 * 6. Create migration tools for data format changes
 * 7. Implement automated testing for configuration changes
 *
 * INTEGRATION POINTS:
 * - dataA.h: Depends on ABSMAXNTN for nation array sizing
 * - fileA.h: Depends on compression and file format settings
 * - buildA.h: Contains campaign-specific balance parameters
 * - All combat modules: Depend on combat and unit specification constants
 * - All economic modules: Depend on cost and resource constants
 * - All AI modules: Depend on NPC behavior parameters
 */
