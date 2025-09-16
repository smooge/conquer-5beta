# Persistent Bug Tracking - Conquer C2023 Modernization

**Purpose**: Track bugs discovered during unit testing and modernization that need to be fixed during the modernization process.

**Usage**: Claude Code should add bugs found during testing, analysis, or modernization work to this file. Each bug should include reproduction steps, severity, and proposed fixes.

---

## Bug Tracking Guidelines

### Bug Priority Levels
- **CRITICAL**: Crashes, security vulnerabilities, data corruption
- **HIGH**: Functional bugs that affect core gameplay or data integrity
- **MEDIUM**: Logic errors, memory leaks, undefined behavior
- **LOW**: Code quality issues, potential improvements

### Bug Entry Format
```markdown
### BUG-XXX: Brief Description
**Priority**: CRITICAL/HIGH/MEDIUM/LOW
**File**: Source file location
**Function**: Function name where bug occurs
**Discovered**: Date and context (e.g., "During unit testing of checkX.c")

**Description**: Detailed description of the bug

**Reproduction Steps**:
1. Step 1
2. Step 2
3. Expected vs actual behavior

**Impact**: What this bug affects

**Proposed Fix**: How to fix this bug during modernization

**Status**: OPEN/IN_PROGRESS/FIXED
```

---

## Active Bugs

### BUG-001: user_exists() NULL Pointer Crash
**Priority**: CRITICAL
**File**: Src/checkX.c
**Function**: user_exists()
**Discovered**: During unit testing in test_checkx_isolated.c

**Description**: Function crashes with segmentation fault when passed a NULL pointer argument.

**Reproduction Steps**:
1. Call `user_exists(NULL)`
2. Program crashes with segmentation fault
3. Expected: Should return 0 (FALSE) safely

**Impact**: Any code that passes unvalidated input to user_exists() will crash the program. This is a security and stability issue.

**Proposed Fix**: Add NULL pointer check at function start:
```c
int user_exists(char *who) {
    if (who == NULL) {
        return 0;  /* FALSE - NULL user does not exist */
    }
    /* ... existing logic ... */
}
```

**Status**: OPEN

### BUG-002: canseelogin() Unsafe String Comparison
**Priority**: HIGH
**File**: Src/checkX.c
**Function**: canseelogin()
**Discovered**: During unit testing of access control logic

**Description**: Function performs strcmp() without NULL checks on loginname global variable, potentially causing crashes if loginname is uninitialized.

**Reproduction Steps**:
1. Set loginname global variable to NULL
2. Set world.hide_login = 1
3. Call canseelogin()
4. Crashes due to strcmp(NULL, LOGIN)

**Impact**: Server crashes if loginname is not properly initialized, affecting system stability.

**Proposed Fix**: Add NULL safety checks:
```c
int canseelogin(void) {
    if (loginname == NULL) {
        return 0;  /* Fail safe - deny access if user unknown */
    }
    return (!world.hide_login ||
            (strcmp(loginname, LOGIN) == 0) ||
            (strcmp(loginname, world.demigod) == 0));
}
```

**Status**: OPEN

### BUG-003: goto_cvn() Incorrect Coordinate Comparison
**Priority**: MEDIUM
**File**: Src/selectG.c
**Function**: goto_cvn()
**Discovered**: During Phase 4 documentation of selectG.c

**Description**: Function incorrectly compares YREAL with c1_ptr->xloc when it should compare XREAL with xloc. This is a logic error in coordinate validation that could cause incorrect caravan navigation behavior.

**Reproduction Steps**:
1. Navigate to a caravan using goto_cvn()
2. Function checks `if (YREAL != c1_ptr->xloc)` at line 1012
3. Expected: Should check `if (XREAL != c1_ptr->xloc)` for X-coordinate validation
4. Bug causes incorrect coordinate comparison (Y vs X mismatch)

**Impact**: Navigation to caravans may work incorrectly when caravan X-coordinate differs from current Y-coordinate, potentially causing navigation failures or unexpected map positioning.

**Proposed Fix**: Change the coordinate comparison to match X with X:
```c
/* Before (buggy) */
if (YREAL != c1_ptr->xloc) {

/* After (fixed) */
if (XREAL != c1_ptr->xloc) {
```

**Status**: OPEN

### BUG-004: Multiple Insecure fprintf() Usage (Security)
**Priority**: HIGH
**File**: Src/checkX.c
**Function**: checkout()
**Discovered**: During clang-tidy static analysis testing

**Description**: Multiple calls to fprintf() without bounds checking or security validation. clang-tidy identifies these as security risks that should use safer alternatives with length arguments or boundary checks.

**Reproduction Steps**:
1. Run `clang-tidy Src/checkX.c -checks=clang-analyzer-security*`
2. Multiple warnings about insecure fprintf() usage at lines 128, 135, 144, 150, 158
3. Expected: Use fprintf_s() or similar bounded alternatives

**Impact**: Potential buffer overflow vulnerabilities in logging and output functions. Could be exploited if format strings are controlled by user input.

**Proposed Fix**: Replace fprintf() calls with safer alternatives:
```c
/* Before (insecure) */
fprintf(fupdate, "%s[%d]: %s has repro > 15 of %d\n", ...);

/* After (secure) */
snprintf(buffer, sizeof(buffer), "%s[%d]: %s has repro > 15 of %d\n", ...);
fprintf(fupdate, "%s", buffer);
/* OR use fprintf_s if available */
```

**Status**: OPEN

### BUG-005: get_userid() Memory Management Issues
**Priority**: MEDIUM
**File**: Src/checkX.c
**Function**: get_userid()
**Discovered**: During security pattern testing with test_checkx_security_patterns.c

**Description**: Multiple memory management and buffer safety issues in get_userid() function.

**Reproduction Steps**:
1. Call `get_userid(NULL)` - function allocates 12 bytes with malloc()
2. Function doesn't provide clear ownership of allocated memory
3. Call `get_userid()` with small buffer causes strcpy() without bounds checking
4. Multiple calls create potential memory leaks without clear free() responsibility

**Impact**: Memory leaks during normal operation, potential buffer overflow from strcpy() without bounds checking.

**Proposed Fix**: Implement comprehensive memory safety:
```c
char* get_userid(char *outname) {
    struct passwd *pwtemp = NULL;

    if ((pwtemp = getpwuid(getuid())) == NULL) {
        if (outname != NULL) {
            /* Use safe string copy with bounds checking */
            outname[0] = '\0';
        }
        return NULL;
    }

    if (outname == NULL) {
        /* Clear documentation: caller must free() */
        if ((outname = malloc(strlen(pwtemp->pw_name) + 1)) == NULL) {
            fprintf(stderr, "Memory allocation failure\n");
            return NULL;
        }
    }

    /* Use safe string copy */
    strncpy(outname, pwtemp->pw_name, 11);
    outname[11] = '\0';  /* Ensure null termination */
    return outname;
}
```

**Status**: OPEN

### BUG-006: Security Pattern - Systematic NULL Pointer Vulnerabilities
**Priority**: HIGH
**File**: Src/checkX.c
**Function**: Multiple functions (user_exists, check_lock, canseelogin)
**Discovered**: During systematic security pattern analysis

**Description**: Multiple functions in checkX.c lack NULL pointer validation, creating a pattern of vulnerability.

**Functions Affected**:
- `user_exists()`: No NULL check before getpwnam(who)
- `check_lock()`: No NULL check before open(filename, ...)
- `canseelogin()`: No NULL check before strcmp(loginname, ...)

**Impact**: Systematic crashes when NULL pointers are passed to these security-critical functions.

**Proposed Fix**: Implement systematic NULL checks across all checkX.c functions:
```c
/* Pattern for all functions */
if (parameter == NULL) {
    return appropriate_error_value;
}
```

**Status**: OPEN

### BUG-009: npc_movearmy() High Movement Points Edge Case
**Priority**: LOW
**File**: Src/moveA.c
**Function**: npc_movearmy()
**Discovered**: During Phase 6B-1 unit testing (11/12 tests pass)

**Description**: Function handles high movement points (≥100) with unexpected calculation logic in edge case scenario.

**Reproduction Steps**:
1. Create test army with 150 movement points
2. Set mock movement cost to 200 (higher than available)
3. Call `npc_movearmy(11, 21)`
4. Expected: Returns TRUE, movement points reduced to -50 (over-expenditure allowed)
5. Actual: Returns TRUE, movement points set to 206 (unexpected calculation)

**Impact**: Minor edge case behavior difference that may affect game balance in high-movement scenarios, but doesn't break core functionality.

**Proposed Fix**: Investigate the high movement point logic in the function to understand the intended behavior when umove ≥ 100. May be working as designed.

**Status**: OPEN

---

## Fixed Bugs

*(Bugs that have been resolved during modernization will be moved here)*

---

## Bug Statistics

**Total Active Bugs**: 7
- **CRITICAL**: 1 (BUG-001)
- **HIGH**: 3 (BUG-002, BUG-004, BUG-006)
- **MEDIUM**: 2 (BUG-003, BUG-005)
- **LOW**: 1 (BUG-009)

**Total Fixed Bugs**: 0

---

## Notes for Claude Code Sessions

**When adding bugs**:
1. Always include reproduction steps
2. Assess impact on system stability and security
3. Provide specific proposed fixes with code examples
4. Reference the source file and function clearly
5. Include discovery context (unit testing, analysis, etc.)

**When fixing bugs**:
1. Update bug status to IN_PROGRESS when starting work
2. Move to Fixed Bugs section when completed
3. Update statistics
4. Reference bug numbers in commit messages

---

Generated: 2025-09-16
Last Updated: 2025-09-16