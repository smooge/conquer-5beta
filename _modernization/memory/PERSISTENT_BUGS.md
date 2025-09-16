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

---

## Fixed Bugs

*(Bugs that have been resolved during modernization will be moved here)*

---

## Bug Statistics

**Total Active Bugs**: 4
- **CRITICAL**: 1
- **HIGH**: 2
- **MEDIUM**: 1
- **LOW**: 0

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