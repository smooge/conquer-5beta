/*
 * sysconf.h - System Configuration and Platform Compatibility Framework
 *
 * This comprehensive header file provides cross-platform compatibility for the
 * Conquer game system by defining platform-specific configurations, function
 * prototypes, and data type abstractions. It enables the game to compile and
 * run correctly across diverse Unix systems from the early 1990s era.
 *
 * Key Components:
 *   - Platform detection and feature capability definitions
 *   - System-specific header inclusion and function prototype declarations
 *   - Cross-platform data type abstractions and size configurations
 *   - Optional feature toggles for different system capabilities
 *
 * Architecture:
 *   - Feature flags define what capabilities each platform supports
 *   - Conditional compilation blocks configure each supported platform
 *   - Universal defaults provide baseline functionality expectations
 *   - Data type abstractions hide platform size differences
 *
 * Integration Notes:
 *   - Used by: All source files throughout the system (universal header)
 *   - Depends on: System headers (<curses.h>, <stdio.h>, etc.)
 *   - Provides: Platform abstraction layer and universal system interface
 *
 * Modernization Considerations:
 *   - Contains extensive legacy platform support (SunOS, Ultrix, AIX, etc.)
 *   - Many function prototypes now provided by standard headers
 *   - Platform-specific macros could be replaced with autotools/cmake detection
 *   - Some features (FILELOCK, CRYPT) need security review for modern use
 *
 * Historical Context:
 *   - Designed for 1990s Unix ecosystem with diverse, incompatible platforms
 *   - Predates POSIX standardization and modern build systems
 *   - Contains manual feature detection and prototype declarations
 *   - Reflects era when system headers were inconsistent across platforms
 */

/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * See the file header.h for the full copyright notice.
 *
 * THIS FILE SHOULD NOT NEED MODIFYING
 *
 * If any changes should be necessary, please send such changes 
 * along with the OS, and machine type, to conquer-bugs@cs.bu.edu
 */

/* ============================================================================
 * SGI SYSTEM COMPATIBILITY
 * ============================================================================
 * Purpose: Enable ANSI C extensions on SGI IRIX systems
 * Problem: SGI systems require explicit __EXTENSIONS__ definition
 * Solution: Define __EXTENSIONS__ when ANSI C is available
 */

/*
 * SGI_EXTENSIONS_FIX - Enable ANSI C extensions on SGI IRIX systems
 *
 * SGI IRIX systems require the __EXTENSIONS__ macro to be defined to access
 * standard ANSI C library functions and features. This workaround enables
 * proper compilation on SGI workstations by detecting ANSI C support and
 * automatically enabling the required extensions.
 *
 * Historical Context:
 *   - SGI IRIX had non-standard approach to ANSI C feature detection
 *   - Required manual extension enabling rather than automatic detection
 *   - This was a common source of compilation failures on SGI systems
 */
#ifdef SGI
/* Enable ANSI C extensions when ANSI C compiler is detected */
#ifdef __STDC__
#define __EXTENSIONS__
#endif /* __STDC__ */
#endif /* SGI */

/* ============================================================================
 * UNIVERSAL FEATURE DEFINITIONS
 * ============================================================================
 * Purpose: Define standard capabilities assumed available on most systems
 * Usage: These features are enabled by default and disabled per-platform
 * Strategy: Assume modern capabilities, disable only where unsupported
 */

/*
 * CRYPT - Cryptographic password hashing capability
 *
 * Enables use of the crypt() function for password hashing and verification.
 * CRITICAL: DO NOT CHANGE IF GAME DATA WITH ENCRYPTED PASSWORDS EXISTS.
 * Disabling this feature will make existing encrypted passwords unreadable.
 *
 * Security Note: crypt() uses DES-based hashing, weak by modern standards.
 * Modern systems should migrate to bcrypt, scrypt, or Argon2.
 */
#define CRYPT		/* have crypt(): DO NOT CHANGE IF DATA IS IN PLACE */

/*
 * STDLIB - Standard library header availability
 *
 * Indicates system provides ANSI C <stdlib.h> header with standard functions
 * like malloc(), free(), exit(), and environment variable access.
 * Nearly universal on modern systems.
 */
#define STDLIB		/* system has the <stdlib.h> file in /usr/include */

/*
 * UNISTD - POSIX system calls header availability
 *
 * Indicates system provides <unistd.h> header with POSIX system call
 * prototypes like read(), write(), close(), and process management.
 * Standard on all POSIX-compliant systems.
 */
#define UNISTD		/* system has the <unistd.h> file in /usr/include */

/*
 * MKDIR - Directory creation capability
 *
 * Indicates system provides mkdir() function for creating directories.
 * Required for game data organization and file management operations.
 * Standard on all modern Unix systems.
 */
#define MKDIR		/* system has the mkdir() function in the library */

/*
 * LRAND48 - High-quality random number generation
 *
 * Indicates system provides lrand48() and srand48() for pseudo-random
 * number generation with better distribution than rand().
 * Preferred over RANDOM when available. Used for game mechanics.
 */
#define LRAND48		/* system has the lrand48() function available */

/*
 * RANDOM - Alternative random number generation
 *
 * Indicates system provides random() and srandom() functions.
 * Used as fallback when LRAND48 is not available.
 * Note: LRAND48 takes precedence if both are defined.
 */
#define RANDOM		/* system has random(), #if LRAND48, lrand48() used */

/*
 * FILELOCK - File locking capability for multi-user coordination
 *
 * Indicates system provides flock() (BSD-style) or lockf() (System V)
 * for coordinating file access between multiple game processes.
 * Critical for preventing data corruption in multi-user environments.
 */
#define FILELOCK	/* system has the "BSD" flock() function */

/*
 * SEARCH_PATHENV - PATH environment variable support
 *
 * Enables searching for executable programs using the PATH environment
 * variable. Used for locating external utilities and system commands.
 * Standard behavior on all Unix systems.
 */
#define SEARCH_PATHENV	/* provide support for the PATH environment variable */

/* ============================================================================
 * OPTIONAL FEATURE DEFINITIONS
 * ============================================================================
 * Purpose: Platform-specific capabilities enabled per-system configuration
 * Usage: Undefined by default, enabled in platform-specific sections below
 * Strategy: Conservative approach - only enable where explicitly supported
 */

/*
 * WINCH_HANDLER - Window size change signal support
 * Enables handling of SIGWINCH signals for terminal window resizing.
 * Used for dynamic UI adaptation to terminal size changes.
 */
/* #define WINCH_HANDLER */	/* system has the SIGWINCH signal handling */

/*
 * REGEXP - Regular expression pattern matching
 * Enables re_comp() and re_exec() functions for pattern matching.
 * Used for advanced text processing and search functionality.
 */
/* #define REGEXP */		/* system has the re_exec() function */

/*
 * NO_PERROR - Missing perror() function indicator
 * Indicates system lacks perror() for error message output.
 * Requires custom error reporting implementation.
 */
/* #define NO_PERROR */		/* system lacks perror() function */

/*
 * GETDTABLESIZE - File descriptor table size inquiry
 * Enables getdtablesize() function for determining maximum file descriptors.
 * Used for resource management and file handle allocation.
 */
/* #define GETDTABLESIZE */	/* the getdtablesize() function is available */

/*
 * SETPRIORITY - Process scheduling priority control
 * Enables setpriority() function for adjusting process scheduling.
 * Used for performance optimization and system resource management.
 */
/* #define SETPRIORITY */	/* the setpriority() function is available */

/*
 * SETREUID - Real/effective user ID manipulation
 * Enables setreuid() function for changing user identity.
 * Used for privilege management and security operations.
 */
/* #define SETREUID */		/* the setreuid() function is available */

/*
 * SWITCHID - Automatic user ID switching capability
 * Enables automatic switching between user identities when appropriate.
 * Depends on SETREUID functionality for implementation.
 */
/* #define SWITCHID */		/* switch uids when appropriate */

/*
 * BZERO - BSD-style memory clearing function
 * Enables bzero() function for clearing memory blocks.
 * Alternative to memset() on BSD-derived systems.
 */
/* #define BZERO */		/* has the BSD bzero() memory clearing */

/*
 * CUSERID - Current user identification
 * Enables cuserid() function for retrieving current user name.
 * Used for user identification and access control.
 */
/* #define CUSERID */		/* the cuserid() function is available */

/*
 * UNAME - System information inquiry
 * Enables uname() function for system identification.
 * Used for platform detection and system information display.
 */
/* #define UNAME */		/* the uname() function for system info */

/*
 * STRCHR - ANSI C string search functions
 * Indicates system uses strchr() instead of index() for string searching.
 * Part of ANSI C standardization transition from BSD functions.
 */
/* #define STRCHR */		/* use strchr() not index() */

/*
 * MALLOCH - malloc.h header availability
 * Indicates system provides <malloc.h> header for memory allocation.
 * Alternative location for malloc() prototypes on some systems.
 */
/* #define MALLOCH */		/* system has <malloc.h> in /usr/include */

/*
 * MEMORYH - memory.h header availability
 * Indicates system provides <memory.h> header for memory functions.
 * Contains memcpy(), memmove(), and related memory manipulation functions.
 */
/* #define MEMORYH */		/* system has <memory.h> in /usr/include */

/*
 * STRINGSH - BSD-style string header selection
 * Indicates system uses <strings.h> instead of <string.h>.
 * BSD systems used different header organization than ANSI C.
 */
/* #define STRINGSH */		/* system uses <strings.h> not <string.h> */

/*
 * SYS_SIGNAL - signal.h location specification
 * Indicates system places signal.h in sys subdirectory.
 * Platform-specific header organization variation.
 */
/* #define SYS_SIGNAL */	/* system has signal.h in the sys subdir */

/*
 * SIZET_FREAD - size_t return type for file I/O
 * Indicates system declares fread()/fwrite() returning size_t.
 * Part of ANSI C standardization (older systems used int).
 */
/* #define SIZET_FREAD */	/* system declares fread() as size_t */

/*
 * Function Declaration Groups - Platform-specific prototype requirements
 * These enable groups of function prototypes needed on specific platforms
 * where system headers don't provide complete declarations.
 */
/* #define DCLR_A */		/* declare first set of functions */
/* #define DCLR_B */		/* declare second set of definitions */
/* #define DCLR_C */		/* declare third set of definitions */
/* #define DCLR_D */		/* declare fourth set of definitions */

/* ============================================================================
 * PLATFORM-SPECIFIC CONFIGURATIONS
 * ============================================================================
 * Purpose: Configure feature sets for specific operating systems and hardware
 * Strategy: Override universal defaults with platform-specific capabilities
 * Organization: Grouped by operating system family and version
 */

/* ============================================================================
 * MACH OS / NeXT SYSTEMS
 * ============================================================================
 * Platform: NeXT workstations running Mach-based NeXTSTEP
 * Characteristics: BSD-derived with Mach microkernel architecture
 * Special Requirements: Alternative random functions, signal handling
 */

/*
 * MACHOS_CONFIGURATION - NeXT/Mach system feature configuration
 *
 * NeXT systems running NeXTSTEP use a Mach microkernel with BSD user-space.
 * These systems lack lrand48() but provide BSD-style memory and signal
 * functions. They require explicit function declarations for system calls.
 *
 * Feature Modifications:
 *   - Disable LRAND48 (use RANDOM fallback)
 *   - Enable BSD-style bzero() memory clearing
 *   - Enable signal.h in sys/ subdirectory
 *   - Enable process priority control
 *   - Enable basic function declarations (DCLR_A)
 */
#ifdef MACHOS
#undef LRAND48			/* NeXT lacks lrand48(), use random() instead */
#define SYS_SIGNAL		/* signal.h located in sys/ subdirectory */
#define BZERO			/* BSD bzero() available for memory clearing */
#define SETPRIORITY		/* setpriority() available for process control */
#define DCLR_A			/* require basic function declarations */
#endif /* MACHOS */

/* ============================================================================
 * SUN OS 4.1.x SYSTEMS
 * ============================================================================
 * Platform: Sun SPARC workstations running SunOS 4.1.x
 * Characteristics: Mature BSD-derived system with comprehensive features
 * Capabilities: Full feature set including regex, user ID control, windowing
 */

/*
 * SUN41_CONFIGURATION - SunOS 4.1.x comprehensive feature configuration
 *
 * SunOS 4.1.x represents a mature Unix implementation with extensive BSD
 * compatibility and additional Sun-specific enhancements. These systems
 * provide the most complete feature set supported by the game.
 *
 * Feature Set:
 *   - Complete window management with SIGWINCH support
 *   - Regular expression processing with re_comp()/re_exec()
 *   - Full user ID manipulation for privilege management
 *   - BSD memory management and string functions
 *   - Alternative header locations for malloc and memory functions
 *   - Comprehensive function declaration requirements
 */
#ifdef SUN41
#define WINCH_HANDLER		/* SIGWINCH signal for window resize events */
#define REGEXP			/* re_comp()/re_exec() regular expressions */
#define SETREUID		/* setreuid() for user ID manipulation */
#define CUSERID			/* cuserid() for current user identification */
#define GETDTABLESIZE		/* getdtablesize() for file descriptor limits */
#define SETPRIORITY		/* setpriority() for process scheduling */
#define BZERO			/* BSD bzero() memory clearing function */
#define MALLOCH			/* <malloc.h> header for memory allocation */
#define MEMORYH			/* <memory.h> header for memory functions */
#define STRINGSH		/* <strings.h> instead of <string.h> */
#define DCLR_C			/* third set of function declarations */
#define DCLR_D			/* fourth set of function declarations */
#endif /* SUN41 */

/* ============================================================================
 * SYSTEM V RELEASE 3 (SVR3)
 * ============================================================================
 * Platform: AT&T System V Release 3 and derivatives
 * Characteristics: Commercial Unix with ANSI C support but limited features
 * Focus: Basic ANSI C compliance with alternative header organization
 */

/*
 * SYSV3_CONFIGURATION - System V R3 minimal feature configuration
 *
 * System V Release 3 provides basic ANSI C support but lacks many BSD
 * extensions. These systems use different header organization and provide
 * minimal system functionality compared to BSD derivatives.
 *
 * Feature Set:
 *   - Alternative memory and malloc headers
 *   - System identification via uname()
 *   - ANSI C string functions (strchr vs index)
 *   - Limited function declaration requirements
 */
#ifdef SYSV3
#define MEMORYH			/* <memory.h> for memory manipulation functions */
#define MALLOCH			/* <malloc.h> for memory allocation functions */
#define UNAME			/* uname() for system identification */
#define STRCHR			/* ANSI C strchr() instead of BSD index() */
#define DCLR_C			/* third set of function declarations */
#endif /* SYSV3 */

/* ============================================================================
 * SYSTEM V RELEASE 4 (SVR4)
 * ============================================================================
 * Platform: AT&T System V Release 4 and derivatives (Solaris 2.x base)
 * Characteristics: Enhanced System V with improved features and POSIX support
 * Improvements: Better process control and user ID management over SVR3
 */

/*
 * SYSV4_CONFIGURATION - System V R4 enhanced feature configuration
 *
 * System V Release 4 builds on SVR3 with additional features including
 * better process management and user ID control. This formed the basis
 * for commercial Unix systems like Solaris 2.x.
 *
 * Feature Set:
 *   - Enhanced beyond SVR3 with process priority control
 *   - User ID switching capabilities for privilege management
 *   - Same header organization and string function support as SVR3
 *   - Maintained ANSI C compliance with System V extensions
 */
#ifdef SYSV4
#define MEMORYH			/* <memory.h> for memory manipulation functions */
#define MALLOCH			/* <malloc.h> for memory allocation functions */
#define SWITCHID		/* automatic user ID switching capabilities */
#define SETPRIORITY		/* setpriority() for process scheduling control */
#define UNAME			/* uname() for system identification */
#define STRCHR			/* ANSI C strchr() instead of BSD index() */
#define DCLR_C			/* third set of function declarations */
#endif /* SYSV4 */

/* ============================================================================
 * DEC ULTRIX SYSTEMS
 * ============================================================================
 * Platform: Digital Equipment Corporation Ultrix (MIPS and VAX)
 * Characteristics: DEC's Unix variant with BSD heritage and DEC enhancements
 * Specifics: ANSI C file I/O, BSD memory functions, comprehensive declarations
 */

/*
 * ULTRIX_CONFIGURATION - DEC Ultrix feature configuration
 *
 * Digital Ultrix provides a BSD-based Unix implementation with DEC-specific
 * enhancements. These systems support ANSI C file I/O conventions and
 * require comprehensive function declarations for proper compilation.
 */
#ifdef ULTRIX
#define BZERO			/* BSD bzero() memory clearing function */
#define SETPRIORITY		/* setpriority() process scheduling control */
#define SIZET_FREAD		/* ANSI C size_t return type for fread() */
#define DCLR_D			/* fourth set of function declarations */
#endif /* ULTRIX */

/* ============================================================================
 * HP-UX SYSTEMS
 * ============================================================================
 * Platform: Hewlett-Packard Unix (PA-RISC and later Itanium)
 * Characteristics: Commercial Unix with HP-specific limitations and features
 * Issues: No file locking, missing perror(), alternative I/O functions
 */

/*
 * HPUX_CONFIGURATION - HP-UX platform-specific adaptations
 *
 * HP-UX systems lack certain standard Unix features like file locking and
 * have alternative implementations for some I/O functions. These systems
 * require special handling for buffered I/O and error reporting.
 *
 * Notable Limitations:
 *   - No flock() support (file locking disabled)
 *   - No perror() function available
 *   - Alternative buffered I/O function names
 */
#ifdef HPUX
#undef FILELOCK		/* HP-UX lacks flock() file locking capability */
#define REGEXP			/* re_comp()/re_exec() regular expressions */
#define NO_PERROR		/* perror() function not available */
#define SIZET_FREAD		/* ANSI C size_t return type for fread() */
#endif /* HPUX */

/* ============================================================================
 * BSD GENERIC SYSTEMS
 * ============================================================================
 * Platform: Generic BSD Unix (4.3BSD and derivatives)
 * Characteristics: Full BSD feature set but lacks ANSI C standardization
 * Requirements: Extensive function declarations, BSD-specific features
 */

/*
 * BSD_CONFIGURATION - Generic BSD Unix comprehensive feature configuration
 *
 * Generic BSD systems provide the most comprehensive Unix feature set but
 * predate ANSI C standardization. These systems require extensive function
 * declarations and use BSD-specific alternatives to standard library functions.
 *
 * BSD Characteristics:
 *   - Pre-ANSI C (no stdlib.h, unistd.h)
 *   - Comprehensive window and signal management
 *   - Full privilege management capabilities
 *   - Maximum function declaration requirements
 */
#ifdef BSD
#undef UNISTD			/* BSD predates <unistd.h> standardization */
#undef STDLIB			/* BSD predates <stdlib.h> standardization */
#undef LRAND48			/* BSD uses random() instead of lrand48() */
#define WINCH_HANDLER		/* SIGWINCH signal for window resize events */
#define SETREUID		/* setreuid() user ID manipulation */
#define GETDTABLESIZE		/* getdtablesize() file descriptor limits */
#define REGEXP			/* re_comp()/re_exec() regular expressions */
#define BZERO			/* BSD bzero() memory clearing function */
#define SETPRIORITY		/* setpriority() process scheduling */
#define DCLR_A			/* first set of function declarations */
#define DCLR_B			/* second set of function declarations */
#define DCLR_C			/* third set of function declarations */
#define DCLR_D			/* fourth set of function declarations */
#endif /* BSD */

/* ============================================================================
 * IBM AIX SYSTEMS
 * ============================================================================
 * Platform: IBM AIX (RS/6000 and POWER systems)
 * Characteristics: Commercial Unix with IBM-specific features and limitations
 * Focus: Alternative memory headers, process control, ANSI C file I/O
 */

/*
 * AIX_CONFIGURATION - IBM AIX feature configuration
 *
 * IBM AIX provides a commercial Unix implementation with alternative header
 * organization and ANSI C compliance. These systems support process priority
 * control and use size_t return types for file operations.
 */
#ifdef AIX
#define MALLOCH			/* <malloc.h> for memory allocation functions */
#define SETPRIORITY		/* setpriority() process scheduling control */
#define SIZET_FREAD		/* ANSI C size_t return type for fread() */
#endif /* AIX */

/* ============================================================================
 * IBM AIX/370 MAINFRAME SYSTEMS
 * ============================================================================
 * Platform: IBM AIX on System/370 mainframe architecture
 * Characteristics: Mainframe variant of AIX with specialized requirements
 * Inheritance: Based on standard AIX configuration with architecture specifics
 */

/*
 * AIX370_CONFIGURATION - IBM AIX/370 mainframe configuration
 *
 * AIX/370 runs IBM's Unix implementation on System/370 mainframe hardware.
 * This configuration inherits standard AIX features while defining the
 * base AIX macro for compatibility with AIX-specific code sections.
 */
#ifdef AIX370
#define MALLOCH			/* <malloc.h> for memory allocation functions */
#define SETPRIORITY		/* setpriority() process scheduling control */
#define AIX			/* identify as AIX system for compatibility */
#endif /* AIX370 */

/* ============================================================================
 * FEATURE DEPENDENCY RESOLUTION
 * ============================================================================
 * Purpose: Ensure dependent features are properly enabled together
 * Strategy: Automatically enable required dependencies for selected features
 * Safety: Prevent configuration errors and missing functionality
 */

/*
 * LOCKF_FILELOCK_DEPENDENCY - Ensure file locking is enabled for lockf()
 *
 * If LOCKF is enabled (System V style file locking), ensure FILELOCK is
 * also enabled to provide the necessary file locking infrastructure.
 * This prevents compilation errors when lockf() is used without the
 * underlying file locking support.
 */
#ifdef LOCKF
#ifndef FILELOCK
#define FILELOCK		/* automatically enable file locking for lockf() */
#endif /* FILELOCK */
#endif /* LOCKF */

/*
 * SETREUID_SWITCHID_DEPENDENCY - Enable user ID switching for setreuid()
 *
 * If SETREUID is enabled (real/effective user ID manipulation), automatically
 * enable SWITCHID to provide the user ID switching functionality that depends
 * on setreuid(). This ensures privilege management features work correctly.
 */
#ifdef SETREUID
#ifndef SWITCHID
#define SWITCHID		/* automatically enable user ID switching */
#endif /* SWITCHID */
#endif /* SETREUID */

/* ============================================================================
 * SYSTEM HEADER INCLUSION
 * ============================================================================
 * Purpose: Include required system headers based on platform configuration
 * Strategy: Conditional inclusion based on feature availability and platform
 * Organization: Core headers first, then platform-specific alternatives
 */

/*
 * CORE_SYSTEM_HEADERS - Essential headers required by all configurations
 *
 * These headers provide fundamental functionality required by the game system
 * including terminal I/O, character classification, and timing functions.
 * The inclusion order is carefully arranged to handle dependencies.
 */
#include <curses.h>		/* terminal I/O and screen management */
#ifndef stdin			/* stdio.h should be included by curses.h */
#include <stdio.h>		/* standard I/O functions (fallback) */
#endif /* stdin */
#include <ctype.h>		/* character classification functions */
#include <sys/time.h>		/* timing and interval timer functions */

/*
 * ANSI_C_HEADERS - Standard library headers for ANSI C compliant systems
 *
 * Include ANSI C standard library headers when available and when the
 * compiler supports ANSI C (__STDC__ defined). This provides access to
 * modern standard library functions and proper prototypes.
 */
#ifdef __STDC__
#ifdef STDLIB
#include <stdlib.h>		/* ANSI C standard library functions */
#endif /* STDLIB */
#endif /* __STDC__ */

/*
 * POSIX_HEADERS - POSIX system call interface headers
 *
 * Include POSIX-compliant system call headers when available. These provide
 * access to standardized Unix system calls and process management functions.
 */
#ifdef UNISTD
#include <unistd.h>		/* POSIX system calls and constants */
#endif /* UNISTD */

/*
 * STRING_FUNCTION_HEADERS - String manipulation function headers
 *
 * Include appropriate string function headers based on platform conventions.
 * BSD systems traditionally used <strings.h> while ANSI C uses <string.h>.
 * The platform configuration determines which is appropriate.
 */
#ifdef STRINGSH
#include <strings.h>		/* BSD-style string functions */
#else
#include <string.h>		/* ANSI C string functions */
#endif /* STRINGSH */

/*
 * SYSTEM_TYPE_HEADERS - System data types and file status headers
 *
 * Include headers for system data types and file operations. VMS systems
 * use different header organization than Unix systems, requiring special
 * handling for file status and type definitions.
 */
#ifndef VMS
#include <sys/types.h>		/* Unix system data types */
#include <sys/stat.h>		/* Unix file status structures */
#else
#include <types.h>		/* VMS system data types */
#include <stat.h>		/* VMS file status structures */
#endif /* VMS */

/*
 * MEMORY_ALLOCATION_HEADERS - Memory management function headers
 *
 * Include memory allocation headers when the platform provides <malloc.h>
 * as an alternative or supplementary location for malloc() prototypes.
 * Some systems require this for proper memory management function access.
 */
#ifdef MALLOCH
#include <malloc.h>		/* alternative memory allocation prototypes */
#endif /* MALLOCH */

/* ============================================================================
 * LEGACY FUNCTION PROTOTYPE DECLARATIONS
 * ============================================================================
 * Purpose: Provide function prototypes for systems lacking complete headers
 * Status: MODERNIZATION TARGET - Replace with standard header detection
 * Context: Pre-ANSI C era when system headers were incomplete or missing
 *
 * MODERNIZATION NOTE:
 * This entire section should be replaced with CMake feature detection in
 * future modernization phases. Modern build systems can automatically detect
 * function availability and generate appropriate configuration headers,
 * eliminating the need for manual platform-specific prototype declarations.
 * This file should eventually be merged with header.h and both replaced
 * by automated CMake configuration detection.
 */

/*
 * CURSES_FUNCTION_PROTOTYPES - Terminal I/O function declarations (DISABLED)
 *
 * These curses function prototypes are commented out because modern systems
 * provide complete prototypes in <curses.h>. Historically, these were needed
 * for systems with incomplete curses headers.
 *
 * Historical Note: Early Unix systems often had incomplete or missing function
 * prototypes, requiring manual declaration for proper compilation.
 */
/*extern int printw(), mvprintw(), waddch(), waddstr(), wrefresh(), wmove(); */
/*extern int wclear(), wclrtobot(), wclrtoeol(), wstandout(), wstandend();*/
/*extern int wgetch(), gtty(), stty(), ioctl(), endwin();*/
/*extern int winsertln(), scroll();*/

/*
 * STANDARD_SYSTEM_PROTOTYPES - Basic system function prototypes
 *
 * Provides prototypes for standard system functions that may be missing
 * from system headers on older platforms. These functions are fundamental
 * to system operation and process management.
 *
 * Functions:
 *   getpass() - Secure password input with echo disabled
 *   perror() - Error message output to stderr
 *   fclose(), pclose() - File and pipe stream closing
 *   system() - Execute shell commands
 *   fputs(), getopt() - String output and command-line parsing
 *   time() - System time retrieval (platform-specific availability)
 */
extern char *getpass();		/* secure password input function */
extern void perror();		/* system error message output */
extern int fclose(), pclose(), system();	/* file and process operations */
extern int fputs(), getopt();	/* string output and argument parsing */
#ifndef VAXC
#ifndef ULTRIX
extern long time();		/* system time (excluded on VAX C and Ultrix) */
#endif /* ULTRIX */
#endif /* VAXC */

/* System Dependent Definitions */

/* Things not properly declared on BSD systems */
#ifdef DCLR_A
/*extern int getuid(), geteuid(), sleep(), umask();*/
#endif /* DCLR_A */
#ifdef DCLR_B
extern int exit(), abort();
extern int setuid(), chdir(), fprintf(), free(), unlink(), stat();
extern int sscanf(), close(), link(), mkdir();
extern char *malloc(), *getenv();
#endif /* DCLR_B */

#ifdef GETDTABLESIZE
extern int getdtablesize();
#endif /* GETDTABLESIZE */

#ifdef __mips__
#ifndef mips
#define mips
#endif /* mips */
#endif /* __mips__ */
#ifdef __LANGUAGE_C__
#ifndef LANGUAGE_C
#define LANGUAGE_C
#endif /* LANGUAGE_C */
#endif /* __LANGUAGE_C__ */

#ifndef AIX
#ifndef SYSV4
/*extern int open();*/
#endif /* SYSV4 */
#endif /* AIX */

#ifdef HPUX
extern int __flsbuf(),__filbuf();
extern void scrollok();
extern void crmode(),nocrmode(),echo(),noecho();
#else /* HPUX */
extern int _flsbuf(), _filbuf();
#endif /* HPUX */

#ifdef DCLR_C
/* Ultrix and AIX already have these defined? */
extern int scanf(), sscanf(), fscanf(), printf(), fprintf();
#endif /* DCLR_C */

#ifdef DCLR_D
/* the system timing stuff */
extern int setitimer(), gethostname();
#ifdef SETPRIORITY
extern int setpriority();
#endif /* SETPRIORITY */
#endif /* DCLR_D */

/* Index/strchr weirdness */
#ifdef STRCHR
#define index(s,c)	strchr(s,c)
#endif /* STRCHR */

/* check the ctype "function"s */
#ifndef toupper
extern int toupper();
#endif /* toupper */
#ifndef isprint
extern int isprint();
#endif /* isprint */
#ifndef isdigit
extern int isdigit();
#endif /* isdigit */
#ifndef islower
extern int islower();
#endif /* islower */
#ifndef isalpha
extern int isalpha();
#endif /* isalpha */
#ifndef isspace
extern int isspace();
#endif /* isspace */

/* the memory function and sprintf weirdness */
#ifdef MEMORYH
/* grab the system definitions */
#include <memory.h>
#else
#ifndef ULTRIX
#ifdef BSD
#ifdef MACHOS
extern char *sprintf();
#endif /* MACHOS */
#else
#ifndef AIX
extern int sprintf();
#endif /* AIX */
#endif /* BSD */
#endif /* ULTRIX */
#endif /* MALLOCH */

#ifdef BZERO
extern void bzero();
#endif /* BSD */

/* so, VAX C doesn't like unlink or uid stuff, huh? */
#ifdef VAXC
extern char *cuserid();
extern int rename(), remove(), sleep();
#ifndef SIZET_FREAD
#define SIZET_FREAD
#endif
#define unlink remove
#else
#endif /* VAXC */

#ifdef SETREUID
extern int setreuid();
#endif /* SETREUID */

#ifdef SIZET_FREAD
extern size_t fread(), fwrite();
#else
#ifndef HPUX
#ifndef MACHOS
/*extern int fread(), fwrite();*/
#endif /* MACHOS */
#endif /* HPUX */
#endif /* SIZET_FREAD */

/* Optional routine usage */

/* Let's be paranoid */
#ifdef CRYPT
extern char *crypt();
#endif /* CRYPT */

/* Randomizer */
#ifdef LRAND48
extern long lrand48();
extern void srand48();
#else
#ifdef RANDOM
extern long random();
extern void srandom();
#endif /* RANDOM */
#endif /* LRAND48 */

/* File locking */
#ifdef FILELOCK
#ifdef LOCKF
extern int lockf();
#else
extern int flock();
#endif /* LOCKF */
#endif /* FILELOCK */

/* Regular expression handlers */
#ifdef REGEXP
extern char *re_comp();
extern int re_exec();
#endif /* REGEXP */

/* The user listing stuff */
#ifdef LISTUSERS
extern int read(), write();
extern char *ttyname();
#endif /* LISTUSERS */

/* ============================================================================
 * GAME DATA TYPE ABSTRACTIONS AND SCALABILITY CONFIGURATION
 * ============================================================================
 * Purpose: Define scalable data types for different game world sizes
 * Strategy: Conditional compilation for memory vs capacity trade-offs
 * Importance: Critical for game balance, performance, and world size limits
 *
 * MODERNIZATION NOTE:
 * These type definitions should be replaced with standard fixed-width types
 * from <stdint.h> (uint8_t, uint16_t, uint32_t) in future modernization.
 * The conditional compilation should be replaced with CMake configuration
 * options that generate appropriate type definitions automatically.
 */

/*
 * FUNDAMENTAL_TYPE_ALIASES - Basic unsigned type abstractions
 *
 * Provides convenient aliases for fundamental unsigned types used throughout
 * the game system. These types form the foundation for more specialized
 * game-specific type definitions.
 *
 * Modern Equivalent: uint16_t and uint8_t from <stdint.h>
 */
typedef unsigned short int uns_short;	/* 16-bit unsigned integer */
typedef unsigned char uns_char;		/* 8-bit unsigned integer */

/*
 * ntntype - Nation identifier data type
 *
 * Used to identify nations throughout the game system. Fixed as 8-bit unsigned
 * type, limiting the maximum number of nations to 255. This provides adequate
 * capacity for most game scenarios while minimizing memory usage.
 *
 * Range: 0-255 nations maximum
 * Memory: 1 byte per nation reference
 * Modern Equivalent: uint8_t
 */
typedef uns_char ntntype;		/* nation identifier type (max 255 nations) */

/*
 * itemtype - Game item quantity data type configuration
 *
 * Configurable data type for representing item quantities, gold, resources,
 * and other numeric game values. Can be compiled for either high precision
 * (double) or standard integer storage based on game requirements.
 *
 * DOUBLE_ITEMS Configuration:
 *   - Uses 'double' for fractional quantities and high precision
 *   - Enables complex economic calculations and fractional resources
 *   - Higher memory usage but greater numerical accuracy
 *
 * Standard Configuration:
 *   - Uses 'long' for integer quantities (typically 32-bit)
 *   - Simpler calculations, lower memory usage
 *   - Adequate for most game scenarios
 */
#ifdef DOUBLE_ITEMS
typedef double itemtype;		/* high-precision item quantities */
#define BIGITEM	BIGDOUBLE		/* maximum item value reference */
#else
typedef long itemtype;			/* standard integer item quantities */
#define BIGITEM	BIGINT			/* maximum item value reference */
#endif /* DOUBLE_ITEMS */

/*
 * idtype - Entity identifier data type configuration
 *
 * Configurable data type for entity IDs (units, cities, items, etc.).
 * Size determined by expected number of entities in the game world.
 *
 * MANY_UNITS Configuration:
 *   - Uses 16-bit unsigned (65,535 maximum entities)
 *   - Suitable for large game worlds with many units
 *   - Higher memory usage per entity reference
 *
 * Standard Configuration:
 *   - Uses 8-bit unsigned (255 maximum entities)
 *   - Memory-efficient for smaller game worlds
 *   - Adequate for typical game scenarios
 */
#ifdef MANY_UNITS
typedef uns_short idtype;		/* 16-bit entity IDs (max 65,535) */
#else
typedef uns_char idtype;		/* 8-bit entity IDs (max 255) */
#endif /* MANY_UNITS */

/*
 * maptype - Map coordinate data type configuration
 *
 * Configurable data type for map coordinates and terrain references.
 * Size determined by maximum map dimensions required.
 *
 * HUGE_MAP Configuration:
 *   - Uses 16-bit unsigned (65,535 x 65,535 maximum map)
 *   - Enables very large game worlds
 *   - Higher memory usage for coordinate storage
 *
 * Standard Configuration:
 *   - Uses 8-bit unsigned (255 x 255 maximum map)
 *   - Memory-efficient for standard game worlds
 *   - Adequate for most gameplay scenarios
 */
#ifdef HUGE_MAP
typedef uns_short maptype;		/* 16-bit map coordinates (max 65,535) */
#else
typedef uns_char maptype;		/* 8-bit map coordinates (max 255) */
#endif /* HUGE_MAP */

/* ============================================================================
 * SYSTEM CONSTANTS AND TYPE LIMITS
 * ============================================================================
 * Purpose: Define system constants and maximum values for data types
 * Usage: Used for bounds checking, array sizing, and value validation
 * Calculation: Based on configured data type sizes and bit patterns
 */

/*
 * NBBY - Number of bits per byte
 *
 * Defines the number of bits in a byte for bit manipulation calculations.
 * Should be provided by system headers but defaults to 8 if unavailable.
 * Used in maximum value calculations for type safety.
 */
#ifndef NBBY
#define NBBY	8			/* bits per byte (universal standard) */
#endif /* NBBY */

/*
 * TYPE_MAXIMUM_VALUES - Calculated maximum values for game data types
 *
 * These macros calculate the maximum representable values for each
 * configurable data type based on their bit width. Used for bounds
 * checking and array allocation throughout the game system.
 *
 * Calculation: (2^(sizeof(type) * NBBY)) - 1
 * Purpose: Prevent overflow and ensure type safety
 */
#define MAX_MAPTYPE	((1L<<(sizeof(maptype) * NBBY)) - 1L)	/* max map coordinate */
#define MAX_IDTYPE	((1L<<(sizeof(idtype) * NBBY)) - 1L)	/* max entity ID */
#define MAX_NTNTYPE	((1L<<(sizeof(ntntype) * NBBY)) - 1L)	/* max nation ID */

/*
 * TIME_DEAD - File staleness timeout for file locking
 *
 * Defines how old (in seconds) a lock file must be before it's considered
 * "dead" and can be safely removed. Used when FILELOCK is not available
 * and manual file age checking is required for multi-user coordination.
 *
 * Value: 3600 seconds (1 hour)
 * Purpose: Prevent indefinite lock file persistence from crashed processes
 */
#define TIME_DEAD 3600			/* dead lock file age threshold (seconds) */
