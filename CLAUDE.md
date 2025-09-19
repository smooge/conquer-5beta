# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## CRITICAL PATH REQUIREMENTS

**ALWAYS USE CORRECT PROJECT PATH:**
- **Correct path**: `/home/ssmoogen/conquer-project/conquer/`
- **NEVER use**: `/home/ssmoogan/` (common typo - incorrect spelling)
- **ALWAYS verify**: All file paths start with `/home/ssmoogen/` (double-o, not single-o)
- **Apply to**: All file operations, session memory files, documentation paths

## Project Overview

This is a C modernization project focused on upgrading legacy pre-ANSI C codebase to C2023 standards. The primary goals are to improve **correctness, security, portability, and maintainability** for use on today's 64-bit systems while preserving functionality.

## Target Platforms

**Supported Operating Systems:**
- Debian Linux
- Fedora Linux
- macOS
- FreeBSD

**Compliance Requirements:**
- Code must be POSIX-compliant
- Use portable system calls and library functions
- Avoid platform-specific extensions unless absolutely necessary

## Build Commands

```bash
# Build with C2023 standard (POSIX-compliant)
gcc -std=c2x -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -g -O2 -I Include Src/*.c -o program

# Build with additional safety flags
gcc -std=c2x -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -Werror -g -O2 -fsanitize=address -fsanitize=undefined -I Include Src/*.c -o program

# INTENSIVE ANALYSIS - Comprehensive warning detection (use for thorough analysis)
gcc -O2 -g -Wall -Wextra -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough -Wsign-conversion -fanalyzer -std=c2x -D_POSIX_C_SOURCE=200809L Src/*.c -o program

# INTENSIVE ANALYSIS - Single file testing (recommended for initial analysis)
gcc -O2 -g -Wall -Wextra -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough -Wsign-conversion -fanalyzer -std=c2x -D_POSIX_C_SOURCE=200809L -c filename.c

# Cross-platform build test
# All platforms: gcc/clang -std=c2x -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -g -O2 -I Include Src/*.c -o program

# Static analysis with clang
clang --analyze -std=c2x -D_POSIX_C_SOURCE=200809L -Wall -Wextra -I Include Src/*.c

# Static analysis with clang-tidy (modernization and security focus)
clang-tidy Src/filename.c -checks=clang-analyzer-security*,clang-analyzer-core* -- -I Include -std=c2x -D_POSIX_C_SOURCE=200809L

# Format code (if clang-format is available)
clang-format -i *.c *.h

# Python usage note: Always use python3 explicitly
python3 script.py  # Correct
python script.py   # May fail - don't use
```

## Modernization Workflow

**IMPORTANT: Corrected Phase Ordering**

**Phase Sequence:**
1. **Phase 1**: Triage and Environment Setup
2. **Phase 2**: Initial Assessment and Planning
3. **Phase 3**: Comprehensive Function Documentation
4. **Phase 4**: Warning Elimination and Compilation Health
5. **Phase 5**: Modern Build System (CMake)
6. **Phase 6**: Testing Infrastructure Setup
7. **Phase 7**: Analyze and Decouple Configuration
8. **Phase 7b**: Replace `#ifdef` Trees with Feature Detection
9. **Phase 8**: Syntactic and Mechanical Modernization
10. **Phase 9**: Deep Refactoring and Integer Portability
11. **Phase 10**: Advanced Analysis and Maintenance

### Phase 1: Triage and Environment Setup 🛡️

Before changing a single line of code, establishing a modern, strict, and controlled environment is critical.

1. **Working Directory**: `/home/ssmoogen/conquer-project/conquer` (use exact path, check for typos)
2. **Version Control**: Place codebase under Git, create `.gitignore` for build artifacts
3. **Modern Compiler**: Use strict warnings (`-Wall -Wextra -Wpedantic`)
4. **Testing Baseline**: Create basic regression tests before refactoring

### Phase 2: Initial Assessment and Planning (Complete Before Starting Code Changes)

1. **System Analysis**:
   - Scan the entire codebase to understand what the initial system is and does
   - Document system architecture, data flow, and key functionality
   - Save findings to `_modernization/claude/reports/SYSTEM_ANALYSIS.md` for future Claude sessions

2. **C2023 Compliance Assessment**:
   - Scan code for all modernizations needed to make it C2023 compliant
   - Identify K&R functions, implicit declarations, deprecated functions
   - Save detailed findings and modernization tasks to `_modernization/claude/reports/C2023_MODERNIZATION.md`

3. **Security Analysis**:
   - Scan code for security problems that need fixing
   - Identify buffer overflows, unsafe string operations, memory leaks
   - Save security issues and remediation plans to `_modernization/claude/reports/SECURITY_FIXES.md`

4. **Compilation Health Assessment** (CRITICAL):
   - Test compilation with strict warning flags: `-Wall -Wextra -Wpedantic`
   - Count and categorize all compilation warnings
   - Identify blocking compilation errors that prevent testing
   - Save findings to `_modernization/claude/reports/COMPILATION_HEALTH.md`

5. **Documentation Assessment**:
   - Analyze current state of code documentation across all source files
   - Evaluate documentation quality, coverage, and consistency
   - Identify files with missing, incomplete, or poor-quality documentation
   - Assess documentation standards compliance and gaps
   - Create file prioritization strategy based on complexity and importance
   - Plan documentation workflow including checkpoint strategy for large files
   - Save comprehensive findings and strategy to `_modernization/claude/reports/DOCUMENTATION_ASSESSMENT.md`

6. **Testing Infrastructure Analysis**:
   - Analyze existing tests and testing frameworks in the codebase
   - Identify test coverage gaps and recommend appropriate testing infrastructure
   - Design test directory structure to keep tests separate from source code
   - Save detailed findings and testing strategy to `_modernization/claude/reports/TESTING_INFRASTRUCTURE.md`

7. **Project Planning**:
   - Create a comprehensive modernization plan with estimated time to complete
   - Prioritize tasks based on risk and complexity
   - Save plan to `_modernization/claude/reports/MODERNIZATION_PLAN.md`

### Phase 3: Comprehensive Function Documentation 📝

**CRITICAL: Document Functions Before Modernization**

**One File Per Session Approach** - document one complete file per session, commit immediately

**File Priority Order:**
- **Priority 1**: Core Game Engine (mainA.c, dataA.c, combatA.c, moveA.c, etc.)
- **Priority 2**: I/O and Data Management (ioX.c, iodataX.c, dataX.c, etc.)
- **Priority 3**: User Interface (mainG.c, displayG.c, infoG.c, etc.)
- **Priority 4**: Game Content (magicA.c, monsterA.c, npcA.c, etc.)
- **Priority 5**: Remaining Interface and Utilities

**Progress Tracking**: See `_modernization/memory/PHASE_3_DOCUMENTATION_STRATEGY.md` for detailed file prioritization and current progress.

**Documentation Requirements**: Analyze each function's purpose, parameters, returns, side effects. Use standard format from [Code Quality Standards](#code-quality-standards). Document before modernization to preserve knowledge and enable safe refactoring.

#### Documentation Assessment Report

Create `DOCUMENTATION_ASSESSMENT.md` with:
- Coverage percentage and quality rating
- File-by-file analysis with function counts
- Priority classifications based on importance and complexity
- Implementation workflow and progress tracking strategy

### Phase 4: Warning Elimination and Compilation Health 🚨

**CRITICAL: This phase is required before any testing or build system work can proceed.**

**Essential for clean compilation needed by testing frameworks.**

**Warning Elimination Priority**:
1. Compilation errors
2. Missing braces (data structure initialization)
3. Format warnings (sprintf/printf mismatches)
4. Implicit declarations
5. Multiple definitions

**Safety Improvements**: Replace sprintf with snprintf, add missing includes

**Completion Criteria**: All source files compile with zero warnings using:
```bash
gcc -std=c2x -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -g -O2 -I Include Src/*.c -o program
```

### Phase 5: Modern Build System (CMake) 🛠️

**CRITICAL: Establish modern build system before testing infrastructure**

Replace legacy Makefiles with CMake to enable proper testing integration, cross-platform compatibility, and modern development workflows.

**CMake enables testing integration and cross-platform builds.**

**Implementation**:
- Analyze current Makefile structure (dual executables)
- Create `CMakeLists.txt` with C2023 standards
- Implement library detection (ncurses, crypt)
- Configure feature detection to replace `#ifdef` trees
- Prepare CTest integration for testing

### Phase 6: Testing Infrastructure Setup 🧪

**CRITICAL: Establish comprehensive testing framework after build system modernization**

With CMake in place, establish robust testing infrastructure to ensure that remaining modernization preserves all original functionality.

**Unity C Testing Framework** - lightweight, C89 compatible

**Test Structure**: `tests/` with subdirectories for unit, integration, regression, security, performance

**Implementation**: Baseline tests for current behavior, automated test runners, coverage reporting, CMake integration

### Phase 7: Configuration Modernization 🧐

**Audit**: Review `config.h` and Makefile options, document dependencies
**Feature Detection**: Replace `#ifdef` trees with CMake-generated configuration

### Phase 8: Syntactic and Mechanical Modernization ⚙️

**IMPORTANT: Create Automation Scripts**

Create automation scripts for repetitive tasks to save time and ensure consistency across sessions.

**Automation Scripts**:
- `convert_kr_functions.py` - K&R to ANSI conversion
- `modernize_headers.py` - Update includes
- `add_safety_checks.py` - Memory safety
- `fix_integer_types.py` - 64-bit portability
- `generate_tests.py` - Test templates

**Script Guidelines**: Use uv shebang format, make idempotent, include `--dry-run` and `--backup` options, log changes

**Modernization Tasks**:
- Convert K&R functions to ANSI prototypes
- Add explicit types and const qualifiers
- Replace deprecated functions (strcpy→strncpy, sprintf→snprintf)
- Add memory safety checks
- Use size_t for indices, proper headers
- Consider C2023 features (_Generic, _Static_assert)

### Phase 9: Deep Refactoring and Integer Portability 🧠

**The Integer Portability Crisis**: Code from the 32-bit era assumes `sizeof(int) == sizeof(long) == sizeof(void *)`. On 64-bit systems using LP64 data model, `long` and pointers are 64 bits while `int` remains 32 bits.

**Decision Framework for Integer Types:**

1. **For counting or general arithmetic**: Use `int`
2. **When exact bit-width is essential**: Use `<stdint.h>` types (`int32_t`, `uint64_t`, etc.)
3. **For memory size, object counts, or array indexing**: Use `size_t` from `<stddef.h>`
4. **For storing pointers as integers**: Use `uintptr_t` or `intptr_t`

**Updating Formatted I/O**: Use macros from `<inttypes.h>` for portable format specifiers:
- Printing `size_t`: use `%zu`
- Printing `uint64_t`: use `%" PRIu64 "`

### Phase 10: Advanced Analysis and Maintenance 🔬

**Static Analysis**: Use multiple analysis tools for comprehensive code quality assessment

**clang-tidy** (Primary modernization tool):
```bash
# Security and core issue detection
clang-tidy Src/filename.c -checks=clang-analyzer-security*,clang-analyzer-core* -- -I Include -std=c2x -D_POSIX_C_SOURCE=200809L

# Comprehensive analysis for modernization
clang-tidy Src/filename.c -checks=clang-analyzer-*,bugprone-*,readability-* -- -I Include -std=c2x -D_POSIX_C_SOURCE=200809L
```

**Additional Static Analysis**: Clang Static Analyzer, cppcheck, or PVS-Studio

**Dynamic Analysis**: Compile with sanitizers:
- **AddressSanitizer** (`-fsanitize=address`): Detects memory errors
- **UndefinedBehaviorSanitizer** (`-fsanitize=undefined`): Detects undefined behavior
- **Valgrind**: Detects memory leaks and management problems

## Common Legacy Patterns to Modernize

### Before (Pre-ANSI):
```c
/* K&R style function */
int process(data, size)
char *data;
int size;
{
    char buffer[100];
    strcpy(buffer, data);
    return size;
}
```

### After (C2023):
```c
/*
 * process - Copy data to internal buffer with bounds checking
 *
 * Copies the input data to a local buffer ensuring no buffer overflow.
 * Validates input parameters and ensures null termination.
 *
 * Parameters:
 *   data - Input string to process (must not be NULL)
 *   size - Size of data to process
 *
 * Returns:
 *   Size of processed data on success, -1 on error
 */
int process(const char *data, size_t size) {
    if (data == NULL || size == 0) {
        return -1;
    }

    char buffer[100];
    if (size >= sizeof(buffer)) {
        return -1;
    }

    strncpy(buffer, data, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    return (int)size;
}
```

## Code Quality Standards

### Function Documentation Requirements

All functions must be documented before modernization using this standard format:

```c
/*
 * function_name - Brief one-line description
 *
 * Detailed description explaining the function's purpose,
 * algorithm, and any important implementation details.
 *
 * Parameters:
 *   param1 - Description of first parameter (constraints, valid ranges)
 *   param2 - Description of second parameter (must not be NULL)
 *
 * Returns:
 *   Description of return value and meaning of different return codes
 *   NULL on error, valid pointer on success
 *   -1 on failure, 0 on success, positive value for count/size
 *
 * Side Effects:
 *   - Modifies global state if applicable
 *   - Allocates memory that caller must free
 *   - May block on I/O operations
 *
 * Testing Notes:
 *   Category: A (Unit) | B (Integration) | C (System) | D (Mock) | E (Skip)
 *   Approach: [Unit tests with mocks | Integration testing | System testing]
 *   Key Tests: [List of critical test scenarios]
 *   Dependencies: [Global variables, initialization requirements]
 *   Mock Requirements: [What needs to be mocked for testing]
 *   Complexity: [Simple | Moderate | Complex] - [suitability assessment]
 *
 * Notes:
 *   - Thread safety information
 *   - Performance considerations
 *   - Historical context if relevant
 */
```

### Testing Notes Guidelines

**MANDATORY**: All function documentation must include Testing Notes section to prevent retesting attempts.

**Category Classification**:
- **Category A (Unit Testable)**: Isolated functions, minimal dependencies
- **Category B (Integration Required)**: Requires world state or multiple modules
- **Category C (System Level Only)**: Requires full game engine initialization
- **Category D (Mock Intensive)**: Testable with extensive mocking
- **Category E (Deferred/Skip)**: Skip until post-modernization

**Reference Documents**:
- Always check `_modernization/claude/reports/FUNCTION_TESTING_CLASSIFICATION.md` before testing
- Follow `_modernization/memory/TESTING_SESSION_GUIDELINES.md` for session planning

**Testing Notes Benefits**:
- **Immediate Context**: Testing approach visible with function code
- **Knowledge Preservation**: Testing insights travel with function during refactoring
- **Session Efficiency**: Prevents retesting classified functions
- **Modernization Planning**: Clear testing roadmap for each function

### Special Documentation for Legacy Code
- **Unclear Logic**: Document confusing or non-obvious code sections
- **Magic Numbers**: Explain the meaning of hardcoded constants
- **Workarounds**: Document any platform-specific hacks or workarounds
- **Assumptions**: Note assumptions about input data, system state, etc.
- **Historical Context**: Preserve information about why code was written this way

### General Code Standards
- All functions must have proper prototypes in header files
- **All routines must have comments at the beginning explaining what the code does**
- Use consistent indentation (4 spaces recommended)
- Add comprehensive error checking
- Document complex algorithms and data structures
- Prefer explicit over implicit type conversions
- Use meaningful variable and function names

## Testing Strategy

### Test Directory Structure

**All tests must be organized in dedicated directories separate from source code:**

- `tests/` - Main test directory containing all test files
- `tests/unit/` - Unit tests for individual functions and modules
- `tests/integration/` - Integration tests for component interactions
- `tests/regression/` - Regression tests to ensure modernization preserves functionality
- `tests/fixtures/` - Test data files and mock inputs
- `tests/scripts/` - Test runner scripts and utilities

**Test File Naming Conventions:**
- Unit tests: `test_<module_name>.c` (e.g., `test_string_utils.c`)
- Integration tests: `test_integration_<feature>.c`
- Regression tests: `test_regression_<issue_id>.c`

### Testing Framework Selection

**Recommended C Testing Frameworks (in order of preference):**

1. **Unity** - Lightweight, portable, C89 compatible
   - Perfect for legacy code modernization
   - Minimal dependencies, easy integration
   - Supports test fixtures and mocking

2. **CMocka** - Modern unit testing framework
   - Built-in mocking capabilities
   - Memory leak detection
   - Exception handling

3. **Check** - GNU Autotools compatible
   - Good for POSIX-compliant projects
   - Fork-based test isolation

4. **Criterion** - Modern C/C++ testing framework
   - Automatic test discovery
   - Parameterized tests

### Testing Requirements

- **Preserve original functionality during modernization**
- **Test each modernized component thoroughly**
- **Create regression tests before making changes**
- Use compiler warnings as early error detection
- Add unit tests for all critical functions
- Test with different compiler versions and flags
- **Cross-platform testing required**: Verify functionality on all target platforms (Debian, Fedora, macOS, FreeBSD)
- Test POSIX compliance using portable system calls
- **Memory safety testing**: Use AddressSanitizer, UndefinedBehaviorSanitizer, and Valgrind
- **Code coverage analysis**: Aim for >90% coverage on critical paths

### Test Execution Guidelines

**CRITICAL: Always run tests from the correct build directory**

**Project Working Directory**: `/home/ssmoogen/conquer-project/conquer`

**Test Execution Commands:**
```bash
# Navigate to main project build directory (REQUIRED)
cd /home/ssmoogen/conquer-project/conquer/build

# IMPORTANT: Regenerate CMake configuration after test changes
cmake ..                  # Regenerate after CMakeLists.txt changes

# Run all tests (recommended)
ctest

# Run tests with verbose output
ctest --output-on-failure

# Build and run tests
make && ctest

# Full rebuild cycle (when adding new tests)
cmake .. && make && ctest

# Run specific test category
ctest -R "unit_"          # Unit tests only
ctest -R "security_"      # Security tests only
ctest -R "regression_"    # Regression tests only
```

**Directory Structure Clarification:**
- **Main Build Directory**: `/home/ssmoogen/conquer-project/conquer/build/` - Use this for all test execution
- **Tests Source Directory**: `/home/ssmoogen/conquer-project/conquer/tests/` - Contains test source files
- **Tests Build Directory**: `/home/ssmoogen/conquer-project/conquer/tests/build/` - NOT used for execution

**Path Resolution:**
- Tests running from `/build/` correctly find source files at `../Src/`
- Tests have dynamic path detection to locate project root via CMakeLists.txt
- All test executables should be run via `ctest` from the main build directory

**Common Errors to Avoid:**
- ❌ Running tests from `/tests/` directory (wrong paths)
- ❌ Running individual test executables directly from `/tests/build/`
- ✅ Always use `cd /home/ssmoogen/conquer-project/conquer/build && ctest`

### Test Build Integration

**CMake Integration Example:**
```cmake
# Enable testing
enable_testing()

# Add test directory
add_subdirectory(tests)

# Create test executables
add_executable(test_string_utils tests/unit/test_string_utils.c src/string_utils.c)
target_link_libraries(test_string_utils unity)

# Add tests to CTest
add_test(NAME string_utils_test COMMAND test_string_utils)
```

**Make Integration Example:**
```makefile
# Test targets
test: build_tests
	./tests/run_all_tests.sh

build_tests:
	$(CC) $(CFLAGS) -o tests/test_string_utils tests/unit/test_string_utils.c src/string_utils.c -lunity
	$(CC) $(CFLAGS) -o tests/test_memory tests/unit/test_memory.c src/memory.c -lunity

clean_tests:
	rm -f tests/test_*
	rm -f tests/*.o
```

## Git Workflow

### Build Artifacts and .gitignore Management

**IMPORTANT**: When creating executable files during development (test binaries, compiled programs), always add them to `.gitignore` to prevent accidental commits.

**Common build artifacts to exclude:**
- Test executables: `tests/test_*` (without `.c` extension)
- Compiled object files: `*.o`
- Build directories: `*/build/`, `*/target/`
- Temporary files: `*.tmp`, `*~`
- Coverage files: `*.gcov`, `*.gcda`, `*.gcno`

**Example .gitignore entries:**
```
# Compiled test executables
tests/test_input_validation
tests/test_simple_security
tests/test_*
!tests/test_*.c
!tests/test_*.h

# Build artifacts
*.o
*/build/
*.gcov
*.gcda
*.gcno
*.tmp
*~

# Platform specific
.DS_Store
Thumbs.db
```

### Commit Guidelines

When completing a set of work, Claude must create a git commit with a comprehensive commit message:

**Commit Message Format:**
```
Brief summary of changes

Prompt: [Description of what the user requested]

Work Completed:
- [List of specific changes made]
- [Include files modified, functions updated, etc.]
- [Note any modernization patterns applied]

Generated by Claude (claude-sonnet-4@20250514)
Co-Authored-By: Claude <noreply@anthropic.com>
```

## Session Management

### Project Directory Structure
At the beginning of each project, Claude must create a `_modernization/` directory structure:
1. Create `_modernization/` directory in the project root
2. Create `_modernization/scripts/` subdirectory for automation scripts created during sessions
3. Create `_modernization/claude/reports/` subdirectory for analysis and planning documents
4. Create `_modernization/claude/sessions/` subdirectory for session logs
5. Create `_modernization/memory/` subdirectory for session progress snapshots
6. **All automation scripts created by Claude sessions must be stored in `_modernization/scripts/` and added to git**
7. **Scripts must be immediately useful and follow the standardized naming convention**
8. All analysis documents must be stored in `_modernization/claude/reports/` including:
   - `_modernization/claude/reports/SYSTEM_ANALYSIS.md`
   - `_modernization/claude/reports/C2023_MODERNIZATION.md`
   - `_modernization/claude/reports/SECURITY_FIXES.md`
   - `_modernization/claude/reports/DOCUMENTATION_ASSESSMENT.md`
   - `_modernization/claude/reports/TESTING_INFRASTRUCTURE.md`
   - `_modernization/claude/reports/MODERNIZATION_PLAN.md`

### Memory File Naming Convention

**Standard Format:**
```
PHASE{N}_{TYPE}_{COMPONENT}_{YYYYMMDD}_{HHMMSS}.md
```

**Components:**
- **PHASE{N}**: Current phase number (PHASE4, PHASE5, etc.)
- **TYPE**: Type of memory file
  - `SESSION` - Individual session completion
  - `MILESTONE` - Major milestone achievement
  - `STATUS` - Current progress status
  - `STRATEGY` - Planning and strategy documents
  - `CHECKPOINT` - Mid-session progress saves
- **COMPONENT**: What was worked on (e.g., `buildA_h`, `PRIORITY4`, `HEADERS`)
- **YYYYMMDD**: Date (e.g., `20250909`)
- **HHMMSS**: Time in 24-hour format (e.g., `143022`)

**Examples:**
- `PHASE4_SESSION_nclassX_h_20250909_143022.md` - Session completing nclassX.h
- `PHASE4_MILESTONE_PRIORITY4_25PCT_20250909_143500.md` - 25% milestone for Priority 4
- `PHASE4_STATUS_CURRENT_20250909_144000.md` - Current status update
- `PHASE4_CHECKPOINT_buildA_h_20250909_141500.md` - Mid-session checkpoint

**Special Files (no timestamp):**
- `CURRENT_STATUS.md` - Always current status
- `DOCUMENTATION_STRATEGY.md` - Always current strategy
- `NEXT_SESSION_INSTRUCTIONS.md` - Instructions for next session
- `PERSISTENT_TODOS.md` - General project todos that persist across sessions
- `PERSISTENT_BUGS.md` - Bug tracking fallback (only when GitHub/gh CLI unavailable)

### Session End Management

**Two Types of Session Documentation:**

#### 1. Memory Files (Every Session End)
When the user indicates it's time to end a session, Claude must:
1. Save current progress to a memory file in `_modernization/memory/` using the naming convention above
2. Include completed tasks, current status, and next steps
3. Document any important decisions or discoveries made during the session
4. Ensure the memory file provides sufficient context for future sessions
5. **Git commit memory files**: Always `git add` and `git commit` session memory files to preserve progress tracking

#### 2. Session Logs (Major Sessions Only)
Create session logs in `_modernization/claude/sessions/` for:
- **Major milestone completions** (Phase completions, significant feature implementations)
- **Multi-file work sessions** (sessions spanning multiple source files or major changes)
- **Complex problem-solving sessions** (sessions involving significant debugging or analysis)
- **Weekly summary sessions** (consolidating week's progress)

**Regular single-file documentation or minor bug fixes do not require session logs.**

### Session Log Creation Process
**When creating a session log**, Claude must execute this sequence:

#### 1. Work Completion Summary
Create a comprehensive session summary including:
- **Tasks Completed**: List all completed work items
- **Files Created/Modified**: Document all file changes made
- **Git Commits**: List all commits made during session
- **Key Decisions**: Important technical decisions and rationale
- **Next Steps**: Recommendations for subsequent sessions
- **Blockers/Issues**: Any unresolved problems or concerns

#### 2. Session Log Export
Export the conversation to a structured file named `SESSION_LOG_[YYYYMMDD]_[HHMMSS].md` to the directory `_modernization/claude/sessions/` with format:

```markdown
# Claude Code Session Log

**Session Date**: [YYYY-MM-DD]
**Session Duration**: [Wall time] (API time: [API time])
**Total Cost**: [Cost if available]
**Code Changes**: [Lines added/removed]

## Session Objectives
[What was the user trying to accomplish]

## Work Completed
### Files Created
- `filename.ext` - Description of purpose and content

### Files Modified
- `filename.ext` - Description of changes made

### Git Commits
- `commit_hash` - Commit message summary

## Key Technical Decisions
[Important architectural or implementation decisions made]

## Testing/Validation Performed
[Any testing, verification, or validation completed]

## Session Outcomes
### Successful Completions
[Tasks that were fully completed]

### Partial Progress
[Tasks that were started but not finished]

### Deferred Items
[Tasks identified but deferred to future sessions]

## Recommendations for Next Session
[Specific next steps and priorities]

## Issues/Blockers Identified
[Any problems that need resolution]

## Session Context Preservation
[Important context that future sessions should know]

---
Generated by Claude (claude-sonnet-4@20250514)
Session Export Date: [YYYY-MM-DD HH:MM:SS]
```

## Bug Tracking During Modernization

### GitHub Issues Integration

**Purpose**: Track bugs discovered during unit testing, code analysis, and modernization work using GitHub Issues for better visibility and collaboration.

**When to Create GitHub Issues**:
- Unit tests reveal crashes, segmentation faults, or undefined behavior
- Code analysis identifies security vulnerabilities or memory safety issues
- Function modernization uncovers logic errors or data corruption risks
- Integration testing finds race conditions or concurrency problems

**Issue Creation Using gh CLI**:
```bash
# Create bug report with template
gh issue create --title "Bug: Brief Description" --body "$(cat <<'EOF'
**Priority**: CRITICAL/HIGH/MEDIUM/LOW
**File**: Src/filename.c
**Function**: function_name()
**Discovered**: During unit testing of checkX.c functions

**Description**: Detailed explanation of the bug

**Reproduction Steps**:
1. Call function_name(NULL)
2. Program crashes with segmentation fault
3. Expected: Should return error code safely

**Impact**: Crashes affect system stability and security

**Proposed Fix**: Add NULL pointer validation:
```c
if (param == NULL) {
    return -1;  /* Error code */
}
```
EOF
)" --label "bug,modernization"

# For security issues, add security label
gh issue create --title "Security: Buffer overflow in function_name" --body "..." --label "bug,security,modernization"

# For critical issues, add priority label
gh issue create --title "Critical: Segfault in core function" --body "..." --label "bug,critical,modernization"
```

**Fallback to PERSISTENT_BUGS.md**:
**Only use `_modernization/memory/PERSISTENT_BUGS.md` when GitHub/gh CLI is unavailable**:
- Network connectivity issues preventing GitHub access
- Authentication problems with gh CLI
- Repository access restrictions

**GitHub Issues Integration with Modernization**:
- Reference issue numbers in commit messages when fixing bugs: `Fix #123: Add NULL pointer validation`
- Use `gh issue edit` to update status and add progress comments
- Close issues automatically with commit messages: `Closes #123: Fix segfault in function_name`
- Use issue labels for organization: `bug`, `security`, `modernization`, `critical`, `documentation`
- Link related issues for tracking dependencies

## Script-Driven Modernization Benefits

**When to Create Automation Scripts:**
- **Pattern Recognition**: When the same type of change needs to be applied to 3+ locations
- **Complex Transformations**: Converting K&R functions, PARM_X macros, or legacy patterns
- **Repetitive Tasks**: Adding safety checks, updating headers, or fixing format specifiers
- **Batch Operations**: Processing multiple files with similar changes
- **Error-Prone Manual Work**: Tasks requiring precise regex matching or complex replacements

**Automation Advantages:**
- **Consistency**: Ensures uniform application of modernization patterns
- **Speed**: Processes entire codebase in minutes vs. hours of manual work
- **Accuracy**: Reduces human error in complex transformations
- **Repeatability**: Can be applied to future projects or code additions
- **Documentation**: Scripts serve as living documentation of modernization decisions
- **Reversibility**: Scripts can be modified to undo changes if needed

**Script Effectiveness Examples:**
- **K&R Function Conversion**: Manual work (2-3 hours for 50 functions) vs. Script (5 minutes)
- **Header Modernization**: Manual work (1-2 hours) vs. Script (2 minutes)
- **Integer Type Updates**: Manual work (4-6 hours) vs. Script (10 minutes)
- **Safety Check Insertion**: Manual work (3-4 hours) vs. Script (5 minutes)

## Migration Notes

- Always make sure original code is checked into git before modernization
- **Complete the Initial Assessment and Planning phase before making any code changes**
- Modernize incrementally, not all at once
- Test frequently during the modernization process
- Document any behavioral changes from the original code
- Maintain compatibility with existing interfaces where possible
- **Create git commits after each completed work session using the format above**
- **Export conversation before compaction when context is below 4%**
- Refer to analysis files for guidance throughout the project:
  - `_modernization/claude/reports/MODERNIZATION_PLAN.md` - Complete modernization guide with security fixes, C2023 compliance, and implementation plan
  - `_modernization/claude/reports/SYSTEM_ANALYSIS.md` - Complete system architecture and functionality
  - `_modernization/claude/reports/DOCUMENTATION_ASSESSMENT.md` - Current documentation state and improvement strategy
  - `_modernization/claude/reports/TESTING_INFRASTRUCTURE.md` - Comprehensive testing framework and strategy
