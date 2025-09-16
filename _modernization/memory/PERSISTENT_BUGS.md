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

---

## Fixed Bugs

*(Bugs that have been resolved during modernization will be moved here)*

---

## Bug Statistics

**Total Active Bugs**: 2
- **CRITICAL**: 1
- **HIGH**: 1
- **MEDIUM**: 0
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