# External Library Dependencies Analysis

**Generated**: 2025-09-15
**Phase**: Phase 7 - Analyze and Decouple Configuration
**Scope**: Complete documentation of all external libraries and system dependencies

---

## Executive Summary

The Conquer project depends on several external libraries and system utilities, ranging from essential (curses, crypt) to optional (compression, regular expressions) to obsolete (legacy platform libraries). This analysis documents all dependencies for modernization planning and cross-platform compatibility.

**Key Findings:**
- **3 critical libraries**: curses, crypt, standard C library
- **2 optional libraries**: compression (zlib), regular expressions
- **5+ obsolete libraries**: legacy platform-specific variants
- **4 external commands**: compression, file removal, editor spawning
- **Security risks**: DES-based crypt(), external command execution

---

## Critical Dependencies (Required)

### 1. Terminal Interface Libraries

#### 1.1 Curses/NCurses
- **Library**: libcurses or libncurses
- **Purpose**: Terminal manipulation, screen control, keyboard input
- **Usage**: All user interface components (displayG.c, ioG.c, pagerG.c, etc.)
- **Modern Name**: ncurses (backward compatible with curses)
- **Availability**: Universal on Unix systems
- **Package Names**:
  - Debian/Ubuntu: `libncurses5-dev` or `libncurses-dev`
  - Fedora/RHEL: `ncurses-devel`
  - macOS: Built-in or via Homebrew `ncurses`
  - FreeBSD: Built-in `ncurses`

**Configuration Detection**:
```cmake
find_package(Curses REQUIRED)
target_link_libraries(conquer ${CURSES_LIBRARIES})
```

**Headers Required**:
```c
#include <curses.h>     // Terminal control functions
```

**Key Functions Used**:
- `initscr()`, `endwin()`: Screen initialization/cleanup
- `mvprintw()`, `printw()`: Positioned text output
- `getch()`, `getstr()`: Input functions
- `clear()`, `refresh()`: Screen management
- `move()`, `addch()`: Cursor and character control

#### 1.2 Terminal Capability Database
- **Legacy**: libtermcap (Berkeley)
- **Modern**: libtinfo (part of ncurses)
- **Purpose**: Terminal capability information
- **Status**: Usually integrated with ncurses
- **Configuration**: Automatically handled by ncurses detection

### 2. Cryptographic Libraries

#### 2.1 Password Hashing (crypt)
- **Library**: libcrypt or built into libc
- **Purpose**: Password hashing and verification
- **Algorithm**: DES-based (SECURITY CONCERN: cryptographically broken)
- **Compatibility**: CRITICAL - existing save files use crypt() hashes
- **Modern Alternatives**: bcrypt, scrypt, Argon2 (future migration)

**Availability by Platform**:
- **Linux**: Separate libcrypt package
- **FreeBSD**: Built into libc
- **macOS**: May require separate installation
- **Legacy**: crypt_i (SCO systems)

**Configuration Detection**:
```cmake
find_library(CRYPT_LIB NAMES crypt crypt_i)
if(CRYPT_LIB)
    target_link_libraries(conquer ${CRYPT_LIB})
else()
    check_function_exists(crypt HAVE_CRYPT_IN_LIBC)
endif()
```

**Security Analysis**:
- **Current**: DES-based hashing (56-bit effective key)
- **Vulnerability**: Brute force attacks feasible with modern hardware
- **Mitigation**: Cannot change without breaking save file compatibility
- **Future**: Implement migration system for stronger hashing

### 3. Standard System Libraries

#### 3.1 Standard C Library (libc)
- **Purpose**: Core C functions (malloc, string operations, I/O)
- **Availability**: Universal (automatically linked)
- **Headers**: stdio.h, stdlib.h, string.h, unistd.h, etc.

#### 3.2 Math Library (libm)
- **Purpose**: Mathematical functions (implicit usage)
- **Linking**: Usually automatic on modern systems
- **Configuration**: `find_library(MATH_LIB m)`

---

## Optional Dependencies

### 1. Compression Libraries

#### 1.1 Legacy External Commands (Current)
- **Commands**: `compress`, `zcat`
- **Format**: Unix compress (.Z files, LZW compression)
- **Security Risk**: External command execution via system()
- **Availability**: Limited on modern systems
- **Performance**: Poor (external process overhead)

**Current Configuration**:
```c
#define COMP_NAME "compress"    // Compression command
#define COMP_READ "zcat"        // Decompression command
#define COMP_SFX  ".Z"          // File extension
```

**Problems**:
- Command injection vulnerabilities
- Limited availability on modern systems
- Poor performance due to external process spawning
- LZW compression is outdated

#### 1.2 Modern Internal Compression (Recommended)
- **Library**: zlib (gzip-compatible)
- **Advantages**: Internal API, better compression, standard format
- **Performance**: Faster than external commands
- **Security**: No external command execution

**Configuration Detection**:
```cmake
find_package(ZLIB)
if(ZLIB_FOUND)
    option(USE_ZLIB_COMPRESSION "Use zlib instead of external compress" ON)
    target_link_libraries(conquer ${ZLIB_LIBRARIES})
    set(HAVE_ZLIB 1)
endif()
```

**Migration Strategy**:
1. Implement zlib compression as option
2. Support reading legacy .Z files during transition
3. Default to .gz format for new save files
4. Provide conversion utility for existing saves

### 2. Regular Expression Libraries

#### 2.1 Legacy BSD Functions
- **Functions**: re_comp(), re_exec()
- **Availability**: Limited to BSD-derived systems
- **Usage**: Optional pattern matching features
- **Status**: Rarely used in current codebase

#### 2.2 POSIX Regular Expressions
- **Header**: regex.h
- **Functions**: regcomp(), regexec(), regfree()
- **Availability**: POSIX-compliant systems (universal)
- **Performance**: Better than legacy BSD functions

**Configuration Detection**:
```cmake
check_include_file(regex.h HAVE_REGEX_H)
if(HAVE_REGEX_H)
    check_function_exists(regcomp HAVE_REGEXP)
endif()
```

---

## Platform-Specific Libraries (Legacy)

### 1. Obsolete Unix Variants

#### 1.1 VMS Libraries (Completely Obsolete)
- **Platform**: DEC OpenVMS
- **Status**: No longer supported
- **Recommendation**: Remove all VMS-specific code

#### 1.2 AIX370 Libraries (Obsolete)
- **Platform**: IBM 370 systems running AIX
- **Status**: Hardware discontinued decades ago
- **Recommendation**: Remove support

#### 1.3 Terminal Library Variants (Legacy)
- **termlib**: Old terminal library (pre-ncurses)
- **tcap**: Terminal capability library
- **Status**: Replaced by ncurses/terminfo
- **Configuration**: No longer needed

### 2. Compiler-Specific Libraries

#### 2.1 GNU C Library Extensions
- **Usage**: GCC-specific warnings and features
- **Compatibility**: Generally portable to clang
- **Modern**: Use standard C2023 features instead

---

## System Command Dependencies

### 1. File System Operations

#### 1.1 File Removal (SECURITY RISK)
- **Current**: `/bin/rm -f` via system() call
- **Security Issue**: Command injection vulnerability
- **Location**: header.h:371 `#define REMOVE_NAME "/bin/rm -f"`
- **Replacement**: Use unlink() system call

**Modernization**:
```c
// Replace this:
system(REMOVE_NAME " filename");

// With this:
if (unlink(filename) != 0) {
    perror("Failed to remove file");
}
```

#### 1.2 Directory Creation
- **Current**: mkdir() function (already properly implemented)
- **Status**: No external command dependency

### 2. Editor Integration

#### 2.1 Text Editor Spawning
- **Environment**: EDITOR variable
- **Default**: `/usr/bin/vi`
- **Security Risk**: External command execution
- **Usage**: ALLOW_EDIT_FORK feature

**Current Configuration**:
```c
#define ENV_EDITOR     "EDITOR"
#define DEFAULT_EDITOR "/usr/bin/vi"
```

**Security Considerations**:
- Validate editor path before execution
- Restrict to approved editor list
- Consider internal editor instead

### 3. Compression Commands (Legacy)

#### 3.1 External Compression
- **Commands**: compress, zcat, gzip, gunzip
- **Status**: Being replaced by internal zlib
- **Timeline**: Phase 7b modernization

---

## Cross-Platform Compatibility Matrix

### Target Platforms (Modern)

| Platform | Curses | Crypt | Zlib | Notes |
|----------|--------|-------|------|-------|
| Debian Linux | ✓ ncurses | ✓ libcrypt | ✓ zlib1g-dev | Full support |
| Fedora Linux | ✓ ncurses | ✓ libcrypt | ✓ zlib-devel | Full support |
| macOS | ✓ built-in | ⚠ may need install | ✓ built-in | Crypt may need Homebrew |
| FreeBSD | ✓ built-in | ✓ built-in | ✓ built-in | Native support |

### Legacy Platforms (Obsolete)

| Platform | Status | Recommendation |
|----------|--------|----------------|
| VMS | Obsolete | Remove all support |
| AIX370 | Obsolete | Remove all support |
| Ultrix | Obsolete | Remove all support |
| NeXT/Mach | Obsolete | Remove all support |
| SCO Unix | Obsolete | Remove all support |

---

## Build System Integration

### 1. CMake Library Detection (Already Implemented)

```cmake
# Terminal libraries (REQUIRED)
find_package(Curses REQUIRED)

# Cryptographic libraries (CRITICAL)
find_library(CRYPT_LIB NAMES crypt crypt_i)

# Compression libraries (OPTIONAL)
find_package(ZLIB)

# Math library (AUTOMATIC)
find_library(MATH_LIB m)

# Regular expressions (OPTIONAL)
check_include_file(regex.h HAVE_REGEX_H)
```

### 2. Fallback Strategies

#### 2.1 Missing Crypt Library
```cmake
if(NOT CRYPT_LIB)
    check_function_exists(crypt HAVE_CRYPT_IN_LIBC)
    if(NOT HAVE_CRYPT_IN_LIBC)
        message(WARNING "crypt function not found - save compatibility affected")
    endif()
endif()
```

#### 2.2 Missing Compression
```cmake
if(NOT ZLIB_FOUND)
    message(STATUS "zlib not found - using external compression commands")
    # Fall back to external compress/zcat
endif()
```

---

## Security Assessment

### 1. Critical Security Issues

#### 1.1 Weak Cryptography
- **Issue**: DES-based password hashing
- **Risk**: Password recovery attacks
- **Impact**: User account compromise
- **Priority**: MEDIUM (compatibility constraint)

#### 1.2 External Command Execution
- **Issue**: system() calls to rm, compress, editor
- **Risk**: Command injection attacks
- **Impact**: System compromise
- **Priority**: HIGH

#### 1.3 Predictable File Names
- **Issue**: /tmp/%s.%d format for temporary files
- **Risk**: Race condition attacks
- **Impact**: Information disclosure
- **Priority**: HIGH

### 2. Recommended Mitigations

#### 2.1 Immediate (Phase 7b)
1. Replace external command execution with native functions
2. Implement secure temporary file creation (mkstemp)
3. Validate all external tool paths

#### 2.2 Future Phases
1. Implement cryptographic library migration system
2. Add password hash upgrade mechanism
3. Implement internal editor for security

---

## Modernization Roadmap

### 1. Phase 7b: Replace External Commands
- [ ] Replace `/bin/rm` with unlink() system call
- [ ] Migrate to internal zlib compression
- [ ] Implement secure temporary file creation
- [ ] Remove obsolete platform library support

### 2. Phase 8: Internal Library Integration
- [ ] Implement zlib compression internally
- [ ] Add POSIX regex support
- [ ] Create library version detection
- [ ] Add graceful degradation for optional libraries

### 3. Future: Security Hardening
- [ ] Design cryptographic migration system
- [ ] Implement modern password hashing
- [ ] Add input validation for all external tools
- [ ] Create internal editor option

### 4. Future: Platform Simplification
- [ ] Remove all VMS, AIX370, Ultrix support
- [ ] Standardize on POSIX-compliant functions only
- [ ] Eliminate platform-specific library variants
- [ ] Consolidate build system configuration

---

## Installation Requirements

### 1. Development Packages Required

#### Debian/Ubuntu:
```bash
sudo apt-get install libncurses5-dev libcrypt-dev zlib1g-dev
```

#### Fedora/RHEL:
```bash
sudo yum install ncurses-devel libcrypt-devel zlib-devel
```

#### macOS:
```bash
brew install ncurses zlib
# May need: brew install libcrypt
```

#### FreeBSD:
```bash
# Built-in libraries, no additional packages needed
```

### 2. Runtime Dependencies

#### Essential:
- ncurses library
- crypt function (libcrypt or built-in)

#### Optional:
- zlib library (for modern compression)
- regex library (for pattern matching)

---

## Conclusion

The Conquer project's external dependencies are manageable and mostly standard, with key exceptions being the security risks from external command execution and weak cryptographic functions. The existing CMake build system already handles most library detection correctly.

**Immediate Priorities:**
1. **Security**: Eliminate external command execution
2. **Modernization**: Migrate to internal compression library
3. **Cleanup**: Remove obsolete platform support

**Long-term Goals:**
1. Upgrade cryptographic functions while maintaining compatibility
2. Implement internal alternatives to external tools
3. Simplify cross-platform support to modern POSIX systems only

The dependency analysis shows that modernization is achievable with minimal risk to compatibility, as the core dependencies (curses, crypt) are universally available on target platforms.