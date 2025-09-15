# Phase 7: Configuration Audit Report

**Generated**: 2025-09-15
**Phase**: Phase 7 - Analyze and Decouple Configuration
**Scope**: Complete audit of configuration options in header.h, sysconf.h, and Makefile systems

---

## Executive Summary

The Conquer project uses a complex multi-layered configuration system typical of 1990s Unix software. Configuration is split across several files with hardcoded values, platform-specific compile-time flags, and manual Makefile generation. This audit identifies all configuration elements and provides a modernization strategy for Phase 7b (Replace `#ifdef` Trees with Feature Detection).

**Key Findings:**
- **67 configuration constants** across header.h and sysconf.h
- **Legacy platform support** for 10+ obsolete systems (VMS, AIX370, Ultrix, etc.)
- **Manual Makefile generation** using sed-based template substitution
- **Hardcoded paths** and administrative credentials in source
- **Security risks** from external command execution and hardcoded admin accounts

---

## Configuration File Analysis

### 1. Primary Configuration: Include/header.h

**Purpose**: Central configuration hub with game balance, system behavior, and platform settings
**Scope**: 1677 lines with extensive documentation and configuration constants
**Usage**: Included by ALL source files in the project

#### 1.1 System Identity Configuration
```c
#define VERSION "Version 5.0b"           // Software version string
#define OWNER   "Smoogen"                // Admin display name (cosmetic)
#define LOGIN   "ssmoogen"               // Admin login (SECURITY CRITICAL)
```

**Modernization Priority**: HIGH - Security risk from hardcoded admin credentials

#### 1.2 Data Format Control (PRIMARY ADJUSTMENTS)
```c
#define COMPRESS                         // Enable data compression
#define COMP_NAME    "compress"          // External compression command
#define COMP_READ    "zcat"              // External decompression command
#define COMP_SFX     ".Z"                // Compressed file extension
#define MANY_UNITS                       // Allow >255 units per formation
#define HUGE_MAP                         // Allow >256x256 maps
#define ABSMAXNTN    100                 // Maximum nations (100)
```

**Impact**: These directly affect data file format and save game compatibility
**Modernization Priority**: HIGH - Replace external commands with internal libraries

#### 1.3 Security and File Permissions
```c
#define DEFAULT_UMASK    077             // Owner-only file permissions
#define CUSTOM_UMASK     066             // Config file permissions
#define FULLWRITE_UMASK  000             // Universal access (SECURITY RISK)
```

**Security Analysis**: FULLWRITE_UMASK creates world-writable files - significant security risk

#### 1.4 System Feature Control Flags
```c
#define ALLOW_EDIT_FORK                  // Allow god-level editor spawning
#define DO_TIME_CHECK                    // Enable SIGALRM time checking
#define SECURITY                         // Limit god access to LOGIN only
#define LISTUSERS                        // Extended user listing with hostname
#define REMAKE                           // Allow world rebuild over existing data
#define CHEAT                            // Enable NPC assistance features
#define SPEW                             // Enable random NPC messages
#define RUNSTOP                          // Stop updates when players active
```

#### 1.5 File System Operations
```c
#define REMOVE_NAME    "/bin/rm -f"      // File removal command (SECURITY RISK)
#define TMP_FILE_FMT   "/tmp/%s.%d"      // Temp file format (INSECURE)
#define TMP_DIR        "/tmp"            // Temp directory
#define ENV_EDITOR     "EDITOR"          // Editor environment variable
#define DEFAULT_EDITOR "/usr/bin/vi"     // Fallback editor path
```

**Security Issues**: External command execution, predictable temp files

#### 1.6 Game Balance Parameters (204 constants)
- Economic costs (MOVECOST, NAVYMAINT, FORTCOST, etc.)
- Combat mechanics (TAKESECTOR, MAXLOSS, FINDPERCENT, etc.)
- Population limits (TOOMANYPEOPLE, ABSMAXPEOPLE, MILLSIZE, etc.)
- Unit specifications (MAXNAVYCREW, NAVY_HOLD, CVN_HOLD, etc.)
- NPC AI behavior (CITYLIMIT, MILRATIO, NPCTOOFAR, etc.)
- Visibility ranges (LANDSEE, NAVYSEE, ARMYSEE, etc.)

### 2. Platform Configuration: Include/sysconf.h

**Purpose**: Cross-platform compatibility and feature detection
**Scope**: 606 lines with platform-specific configurations
**Usage**: Provides platform abstraction layer

#### 2.1 Universal Feature Definitions (Enabled by Default)
```c
#define CRYPT           // Cryptographic password hashing (DES-based)
#define STDLIB          // <stdlib.h> header availability
#define UNISTD          // <unistd.h> POSIX header availability
#define MKDIR           // mkdir() function availability
#define FILELOCK        // flock() BSD-style file locking
#define SEARCH_PATHENV  // PATH environment variable support
```

#### 2.2 Optional Features (Platform-Specific)
```c
#define WINCH_HANDLER   // SIGWINCH window resize signal support
#define REGEXP          // re_comp()/re_exec() regular expressions
#define SETPRIORITY     // setpriority() process scheduling
#define CUSERID         // cuserid() user identification
#define UNAME           // uname() system information
#define STRCHR          // strchr() vs index() string functions
```

#### 2.3 Legacy Platform Support
- **MACHOS**: NeXT workstations (obsolete)
- **SYSV4**: System V Release 4 derivatives
- **BSD**: Generic BSD Unix systems
- **VMS**: DEC VMS systems (completely obsolete)

#### 2.4 Data Type Abstractions
```c
typedef unsigned short int uns_short;   // 16-bit unsigned
typedef unsigned char uns_char;         // 8-bit unsigned
typedef uns_char ntntype;               // Nation IDs (max 255)

#ifdef MANY_UNITS
typedef uns_short idtype;               // 16-bit entity IDs (max 65,535)
#else
typedef uns_char idtype;                // 8-bit entity IDs (max 255)
#endif

#ifdef HUGE_MAP
typedef uns_short maptype;              // 16-bit coordinates (max 65,535)
#else
typedef uns_char maptype;               // 8-bit coordinates (max 255)
#endif

#ifdef DOUBLE_ITEMS
typedef double itemtype;                // High-precision quantities
#else
typedef long itemtype;                  // Standard integer quantities
#endif
```

### 3. Build Configuration: Legacy Makefiles

#### 3.1 Main Makefile Structure
- **Makefile.top**: Template with %%VARIABLE%% placeholders
- **Makefile**: Generated from Makefile.top via sed substitution
- **Subdirectory Makefiles**: Generated from .src/.aux/.dcm/.inc templates

#### 3.2 Platform Detection (SYSFLG Variable)
```makefile
# Currently: SYSFLG = -DBSD (but file shows -DSUN41)
# Available options:
SYSFLG = -DSYSV3    # System V Release 3
SYSFLG = -DSYSV4    # System V Release 4
SYSFLG = -DBSD      # BSD derivatives
SYSFLG = -DSUN41    # SunOS 4.1+
SYSFLG = -DMACHOS   # NeXT Mach systems
SYSFLG = -DAIX      # IBM AIX
SYSFLG = -DAIX370   # IBM 370 AIX
SYSFLG = -DULTRIX   # DEC Ultrix
SYSFLG = -DHPUX     # HP-UX
SYSFLG = -DVMS      # DEC VMS (obsolete)
```

#### 3.3 Library Dependencies
```makefile
# Current (Makefile):
LIBS = -lcurses -ltermcap -lcrypt

# Template shows different options per platform:
# BSD: -lcurses -ltermcap -lcrypt
# SYSV: -lcurses
# Xenix: -ltermlib -ltcap -lcrypt
# HPUX: -lcurses
# SCO: -lcurses -lcrypt_i
```

#### 3.4 Directory Configuration
```makefile
TOPDIR   = ${PWD}                           # Current directory
DATADIR  = /usr/local/games/lib/conquer     # Game data (template)
DATADIR  = /usr/games/lib/conquer           # Game data (Makefile.top)
BINDIR   = /usr/local/games/bin             # Executables (template)
BINDIR   = /usr/games                       # Executables (Makefile.top)
```

#### 3.5 Build Tool Configuration
```makefile
CC       = gcc                              # Compiler
CFLAGS   = -O -g -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wid-clash-12 -ansi -DDEBUG
LIBS     = -lcurses -ltermcap -lcrypt      # Required libraries
SHELL    = /bin/sh                         # Build shell
```

### 4. Modern CMake Configuration (Already Implemented)

**Status**: CMake build system already implemented in Phase 5
**Features**: Automatic feature detection, cross-platform library discovery, configuration generation

#### 4.1 Configuration Options
```cmake
option(CONQUER_ENABLE_COMPRESSION "Enable data file compression" ON)
option(CONQUER_LARGE_MAPS "Support maps larger than 256x256" ON)
option(CONQUER_MANY_UNITS "Support more than 255 units per formation" ON)
option(CONQUER_SECURITY_MODE "Enable security restrictions" ON)
option(CONQUER_ENABLE_JOINT "Enable joint operation features" ON)
```

#### 4.2 Automatic Feature Detection
```cmake
check_function_exists(crypt HAVE_CRYPT)
check_function_exists(flock HAVE_FLOCK)
check_function_exists(lrand48 HAVE_LRAND48)
check_include_file(crypt.h HAVE_CRYPT_H)
find_package(Curses REQUIRED)
find_library(CRYPT_LIB NAMES crypt crypt_i)
```

---

## External Library Dependencies

### 1. Required Libraries

#### 1.1 Terminal Interface
- **curses/ncurses**: Terminal manipulation and screen control
- **termcap/terminfo**: Terminal capability database
- **Status**: CRITICAL - Required for all user interfaces
- **Modern Availability**: Universal (ncurses standard)

#### 1.2 Cryptographic Functions
- **libcrypt**: Password hashing using crypt() function
- **Algorithm**: DES-based (weak by modern standards)
- **Status**: CRITICAL for save file compatibility
- **Security Issue**: DES hashing is cryptographically broken

#### 1.3 Standard System Libraries
- **libc**: Standard C library functions
- **libm**: Math library (implicit in modern systems)
- **Status**: Universal availability

### 2. Optional Libraries

#### 2.1 Compression Support
- **Legacy**: External compress/zcat commands
- **Modern**: zlib library (already detected by CMake)
- **Recommendation**: Migrate to internal zlib compression

#### 2.2 Regular Expressions
- **Legacy**: re_comp()/re_exec() BSD functions
- **Modern**: POSIX regex.h (regcomp/regexec)
- **Status**: Optional feature, limited usage

### 3. Obsolete Dependencies

#### 3.1 Legacy Platform Libraries
- **termlib/tcap**: Old terminal libraries (replaced by ncurses)
- **crypt_i**: SCO-specific crypt library
- **Status**: No longer needed on modern systems

---

## Environmental Logic Analysis

### 1. Compile-Time Environment Dependencies

#### 1.1 Platform Detection Logic
```c
#ifdef VMS
    // VMS-specific code paths (completely obsolete)
#endif

#ifdef BSD
    // BSD-specific function calls and headers
#endif

#ifdef SYSV4
    // System V R4 specific features
#endif
```

**Impact**: Creates platform-specific code branches throughout source
**Modernization**: Replace with CMake feature detection

#### 1.2 Feature Flag Dependencies
```c
#ifdef COMPRESS
    // Compression-related code paths
#endif

#ifdef MANY_UNITS
    // Extended unit count support
#endif

#ifdef HUGE_MAP
    // Large map coordinate handling
#endif
```

**Impact**: Changes data structures and file formats
**Risk**: Save game compatibility depends on consistent flags

### 2. Runtime Environment Dependencies

#### 2.1 Environment Variables
- **EDITOR**: User's preferred text editor
- **PATH**: Program location searching
- **USER**: Default administrator detection (CMake only)
- **TMPDIR**: Temporary directory (not used, should be)

#### 2.2 File System Assumptions
- **/tmp**: Temporary file storage (hardcoded)
- **/bin/rm**: File removal command (hardcoded)
- **/usr/bin/vi**: Default editor (hardcoded)
- **compress/zcat**: Compression utilities (hardcoded)

#### 2.3 Administrative Assumptions
- **LOGIN constant**: Hardcoded admin user "ssmoogen"
- **File permissions**: Hardcoded umask values
- **Installation paths**: Hardcoded in Makefiles

---

## Security Assessment

### 1. Critical Security Issues

#### 1.1 Hardcoded Administrative Credentials
- **Location**: header.h:127 `#define LOGIN "ssmoogen"`
- **Risk**: Anyone with source access knows admin account
- **Impact**: Bypass of security restrictions
- **Priority**: IMMEDIATE fix required

#### 1.2 External Command Execution
- **REMOVE_NAME**: Calls `/bin/rm -f` via system()
- **COMP_NAME**: Calls `compress` via system()
- **Risk**: Command injection if filenames not sanitized
- **Priority**: HIGH - Replace with native functions

#### 1.3 Insecure Temporary Files
- **TMP_FILE_FMT**: Predictable filenames `/tmp/%s.%d`
- **Risk**: Race conditions, information disclosure
- **Priority**: HIGH - Use mkstemp() or similar

#### 1.4 World-Writable File Creation
- **FULLWRITE_UMASK**: Creates files writable by all users
- **Risk**: Unauthorized modification of game files
- **Priority**: MEDIUM - Review usage and restrict

### 2. Cryptographic Weaknesses

#### 2.1 Obsolete Password Hashing
- **Algorithm**: DES-based crypt()
- **Status**: Cryptographically broken since 1990s
- **Impact**: Password recovery attacks feasible
- **Recommendation**: Migrate to bcrypt/scrypt/Argon2

### 3. Platform Security Considerations

#### 3.1 Privilege Management
- **SETREUID/SWITCHID**: User ID switching capabilities
- **Risk**: Privilege escalation if misused
- **Status**: Currently disabled in configuration

---

## Modernization Strategy

### 1. Phase 7b Implementation Plan

#### 1.1 Create config.h.in Template
```c
/* Generated configuration header */
#define CONQUER_VERSION "@CONQUER_VERSION@"
#define CONQUER_ADMIN_USER "@CONQUER_ADMIN_USER@"

/* Feature availability (detected by CMake) */
#cmakedefine HAVE_CRYPT 1
#cmakedefine HAVE_FLOCK 1
#cmakedefine HAVE_LRAND48 1
#cmakedefine HAVE_ZLIB 1

/* Configuration options */
#cmakedefine CONQUER_ENABLE_COMPRESSION 1
#cmakedefine CONQUER_LARGE_MAPS 1
#cmakedefine CONQUER_MANY_UNITS 1
```

#### 1.2 Replace Platform Detection
- **Remove**: All VMS, AIX370, ULTRIX legacy support
- **Standardize**: Use POSIX-compliant functions only
- **Feature Detection**: CMake CheckFunctionExists() for everything

#### 1.3 Eliminate External Commands
- **Compression**: Replace compress/zcat with zlib
- **File Operations**: Replace `/bin/rm` with unlink()
- **Editor**: Use internal editor or validate external commands

#### 1.4 Configuration File Migration
- **Admin Credentials**: Move to config file or environment variables
- **Game Balance**: Create balance.conf for runtime modification
- **Paths**: Use CMake install directories and environment variables

### 2. Implementation Priority

#### 2.1 Immediate (Security Critical)
1. Remove hardcoded LOGIN credential
2. Replace external command execution
3. Fix insecure temporary file creation
4. Remove FULLWRITE_UMASK usage

#### 2.2 Phase 7b (Feature Detection)
1. Create config.h.in template
2. Remove legacy platform support
3. Implement CMake feature detection
4. Replace compile-time flags with CMake options

#### 2.3 Future Phases
1. Migrate to modern cryptographic functions
2. Implement configuration file system
3. Add runtime game balance modification
4. Create installation and deployment tools

---

## Configuration Migration Checklist

### 1. Security Fixes (Immediate)
- [ ] Remove hardcoded admin credentials from source
- [ ] Replace system() calls with native functions
- [ ] Implement secure temporary file creation
- [ ] Audit and restrict file permission settings

### 2. Platform Modernization (Phase 7b)
- [ ] Create CMake config.h.in template
- [ ] Remove VMS, AIX370, ULTRIX legacy code
- [ ] Replace platform #ifdef with feature detection
- [ ] Migrate Makefile variables to CMake equivalents

### 3. External Dependencies (Phase 7b)
- [ ] Replace compress/zcat with zlib
- [ ] Eliminate hardcoded external tool paths
- [ ] Implement library version detection
- [ ] Add optional dependency graceful degradation

### 4. Configuration Architecture (Phase 8+)
- [ ] Design configuration file format
- [ ] Implement runtime configuration loading
- [ ] Create configuration validation system
- [ ] Add configuration migration tools

---

## Conclusion

The Conquer project's configuration system reflects 1990s Unix software practices with extensive platform support, manual configuration management, and hardcoded values. While functional, it presents significant security risks and maintenance challenges.

**Immediate Actions Required:**
1. **Security**: Remove hardcoded credentials and external command execution
2. **Modernization**: Replace manual platform detection with CMake automation
3. **Simplification**: Remove obsolete platform support (VMS, AIX370, etc.)

**Phase 7b Success Criteria:**
- All platform-specific #ifdef trees replaced with CMake feature detection
- Security vulnerabilities eliminated
- Configuration centralized in generated config.h
- External library dependencies properly detected and managed

The existing CMake infrastructure (implemented in Phase 5) provides a solid foundation for this modernization, requiring primarily migration of existing configuration constants rather than building new systems from scratch.