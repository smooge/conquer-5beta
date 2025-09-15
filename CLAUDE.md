# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

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

# Cross-platform build test (test on all target platforms)
# Debian/Ubuntu: gcc -std=c2x -D_POSIX_C_SOURCE=200809L *.c
# Fedora: gcc -std=c2x -D_POSIX_C_SOURCE=200809L *.c
# macOS: clang -std=c2x -D_POSIX_C_SOURCE=200809L *.c
# FreeBSD: clang -std=c2x -D_POSIX_C_SOURCE=200809L *.c

# Static analysis with clang
clang --analyze -std=c2x -D_POSIX_C_SOURCE=200809L -Wall -Wextra *.c

# Format code (if clang-format is available)
clang-format -i *.c *.h

# Python usage note: Always use python3 explicitly
python3 script.py  # Correct
python script.py   # May fail - don't use
```

## Modernization Workflow

**IMPORTANT: Corrected Phase Ordering**

Based on lessons learned during actual implementation, the phase ordering has been revised from the original plan. See `_modernization/claude/reports/PHASE_ORDERING_LESSONS_LEARNED.md` for detailed analysis.

**Corrected Phase Sequence:**
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

1. **Working Directory Documentation**: Determine and document the exact project working directory path
   - Use `pwd` command to get the current working directory
   - Document the full path in this CLAUDE.md file for future session reference
   - **Project Working Directory**: `/home/ssmoogen/conquer-project/conquer`
   - **CRITICAL PATH ACCURACY**: Always use the exact path `/home/ssmoogen/conquer-project/conquer`
     - **NEVER** change `ssmoogen` to `ssmoogan` (common typo that causes file not found errors)
     - **ALWAYS** double-check file paths before using Read, Write, Edit, or MultiEdit tools
     - When in doubt, use relative paths like `Src/filename.c` instead of absolute paths
     - If a file operation fails with "File does not exist", check for path typos first
2. **Version Control**: Place the entire codebase under Git version control
3. **Git Ignore Configuration**: Check if `.gitignore` exists; if not, create one to prevent build artifacts from being committed
   - Include common C build artifacts: `*.o`, compiled executables, temporary files
   - Include test executables: `tests/test_*` (without `.c` extension)
   - Include platform-specific files: `.DS_Store`, `Thumbs.db`
   - Include coverage files: `*.gcov`, `*.gcda`, `*.gcno`
4. **Modern Compiler and Strict Warnings**: Use GCC or Clang with highest warning levels:
   ```bash
   gcc -std=c2x -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -g -O2 *.c -o program
   ```
   - `-Wall`: Enables all standard, high-priority warnings
   - `-Wextra`: Enables additional useful warnings not covered by `-Wall`
   - `-Wpedantic`: Issues warnings required by the specified C standard
   - `-std=c2x`: Enforces the 202x C standard
3. **Establish a Testing Baseline**: Create basic regression tests before refactoring

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

**REVISED STRATEGY: One File Per Session Approach**

Due to the extensive nature of documenting 50+ source files, Phase 3 is now organized as a series of focused sessions, with each session documenting one complete file and committing immediately to preserve context.

**Documentation Session Workflow:**

1. **Session Preparation**:
   - Load `_modernization/memory/PHASE_3_DOCUMENTATION_STRATEGY.md` to check progress
   - Identify next priority file to document
   - Read target file to understand its function structure

2. **Documentation Process per File**:
   - Read and analyze the complete file to understand its purpose and functions
   - Manually document all functions with comprehensive analysis
   - Focus on quality and completeness for one file at a time

3. **Session Completion**:
   - Git commit the fully documented file immediately
   - Update progress tracking in strategy file
   - Save session memory for next file continuation
   - End session cleanly to preserve context

**File Priority Order:**
- **Priority 1**: Core Game Engine (mainA.c, dataA.c, combatA.c, moveA.c, etc.)
- **Priority 2**: I/O and Data Management (ioX.c, iodataX.c, dataX.c, etc.)
- **Priority 3**: User Interface (mainG.c, displayG.c, infoG.c, etc.)
- **Priority 4**: Game Content (magicA.c, monsterA.c, npcA.c, etc.)
- **Priority 5**: Remaining Interface and Utilities

**Progress Tracking**: See `_modernization/memory/PHASE_3_DOCUMENTATION_STRATEGY.md` for detailed file prioritization and current progress.

**Function Documentation Requirements:**

1. **Analyze Function Purpose**:
   - Read and understand what each function actually does
   - Trace data flow and side effects
   - Identify function dependencies and call patterns
   - Document any non-obvious behavior or edge cases

2. **Standard Documentation Format**:
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
    * Notes:
    *   - Thread safety information
    *   - Performance considerations
    *   - Historical context if relevant
    */
   ```

3. **Special Documentation for Legacy Code**:
   - **Unclear Logic**: Document confusing or non-obvious code sections
   - **Magic Numbers**: Explain the meaning of hardcoded constants
   - **Workarounds**: Document any platform-specific hacks or workarounds
   - **Assumptions**: Note assumptions about input data, system state, etc.
   - **Historical Context**: Preserve information about why code was written this way

4. **Direct Analysis Approach**:
   - Analyze each function by reading and understanding the code directly
   - Understand function purpose, parameters, return values, and side effects
   - Document complex algorithms, data flow, and business logic
   - Identify and explain magic numbers, assumptions, and edge cases

**Documentation Workflow:**

1. **File Analysis**: Read and understand the complete file structure and purpose
2. **Function Analysis**: Analyze each function's implementation, algorithm, and behavior
3. **Documentation Writing**: Create comprehensive documentation using standard format
4. **Cross-Reference**: Verify documentation matches actual implementation
5. **Knowledge Capture**: Document any tribal knowledge or non-obvious behavior
6. **Version Control**: Commit documented code before any modernization

**Why Document Before Modernization:**

- **Preserve Knowledge**: Legacy code often lacks institutional knowledge
- **Enable Safe Refactoring**: Understanding prevents breaking changes
- **Improve Test Design**: Documentation guides comprehensive test creation
- **Reduce Risk**: Clear specifications prevent modernization errors
- **Future Maintenance**: Well-documented code is easier to maintain

#### Documentation Assessment Report Template

The `DOCUMENTATION_ASSESSMENT.md` report should include comprehensive analysis of current documentation state and provide actionable improvement strategy. Required sections:

**1. Executive Summary**
- Overall documentation coverage percentage
- Quality assessment (Poor/Fair/Good/Excellent)
- Critical gaps requiring immediate attention
- Estimated effort to bring to modernization standards

**2. Current Documentation Analysis**
- **File-by-File Analysis**: Function count, current documentation state, quality rating
- **Documentation Coverage**: Functions documented vs undocumented per file
- **Quality Assessment**: Rating documentation completeness, clarity, and usefulness
- **Standards Compliance**: Adherence to modern C documentation conventions
- **Common Issues**: Recurring documentation problems across files

**3. Documentation Standards Assessment**
- **Missing Elements**: Function purpose, parameters, return values, side effects
- **Legacy Issues**: Outdated comments, unclear descriptions, missing context
- **Consistency Problems**: Inconsistent formatting, style variations
- **Technical Debt**: Comments that don't match implementation

**4. File Prioritization Strategy**
- **Priority Classifications**: Critical, High, Medium, Low based on:
  - System importance (core engine vs utilities)
  - Function complexity and count
  - Current documentation state
  - Dependencies and call frequency
- **Session Planning**: Recommended order and checkpoint strategy
- **Large File Handling**: Files requiring 15-function checkpoint approach

**5. Implementation Strategy**
- **Documentation Workflow**: Step-by-step process for each file
- **Quality Standards**: Target documentation format and requirements
- **Checkpoint Strategy**: When and how to implement 15-function breaks
- **Progress Tracking**: Metrics and milestones for tracking improvement
- **Resource Estimation**: Time and effort required per priority group

**6. Risk Assessment**
- **Knowledge Loss Risk**: Functions with unclear or missing logic documentation
- **Maintenance Risk**: Poorly documented complex algorithms
- **Integration Risk**: Undocumented interfaces and dependencies

This assessment becomes the foundation for Phase 3 documentation work and ensures systematic improvement of code documentation quality.

### Phase 4: Warning Elimination and Compilation Health 🚨

**CRITICAL: This phase is required before any testing or build system work can proceed.**

**Why This Phase Is Essential:**
- Legacy codebases often have 100+ warnings that mask real bugs
- Modern testing frameworks require clean compilation
- Build system modernization depends on reliable compilation
- Warnings often indicate data corruption or memory safety issues

**4 Compilation Baseline**:
- Test compile all source files with strict C2023 flags
- Document all warnings by category and severity
- Identify critical errors that prevent compilation

**4 Warning Elimination Priority**:
1. **Compilation Errors**: Fix anything that prevents building
2. **Missing Braces**: Fix data structure initialization warnings (often real bugs)
3. **Missing Field Initializers**: Fix union and struct initialization
4. **Format Warnings**: Fix sprintf/printf format mismatches
5. **Implicit Declarations**: Add missing function prototypes
6. **Multiple Definitions**: Fix header file variable definition conflicts

**4 Basic Safety Improvements**:
- Replace sprintf with snprintf for buffer safety
- Add missing includes for standard library functions
- Fix obvious memory safety issues found during warning fixes

**Completion Criteria**: All source files compile with zero warnings using:
```bash
gcc -std=c2x -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic
```

### Phase 5: Modern Build System (CMake) 🛠️

**CRITICAL: Establish modern build system before testing infrastructure**

Replace legacy Makefiles with CMake to enable proper testing integration, cross-platform compatibility, and modern development workflows.

**Why CMake First:**
- Testing frameworks integrate best with modern build systems
- Feature detection replaces hardcoded `#ifdef` trees
- Cross-platform library detection (ncurses, crypt, etc.)
- Enables automated testing and CI/CD integration

**5.1 Makefile Analysis**:
- Document current build structure (dual executables: conquer/conqrun)
- Identify library dependencies and platform-specific code
- Extract compiler flags and feature requirements

**5.2 CMake Implementation**:
- Create root `CMakeLists.txt` with proper C2023 standards
- Implement library detection (FindPkgConfig for ncurses, crypt)
- Configure feature detection to replace `#ifdef` trees
- Set up separate targets for user interface and admin programs

**5.3 Cross-Platform Configuration**:
- Support target platforms: Debian, Fedora, macOS, FreeBSD
- Implement POSIX-compliant feature detection
- Replace hardcoded system flags with CMake tests

**5.4 Testing Integration Preparation**:
- Configure CTest integration for future testing framework
- Set up test directory structure within CMake
- Prepare for Unity testing framework integration

### Phase 6: Testing Infrastructure Setup 🧪

**CRITICAL: Establish comprehensive testing framework after build system modernization**

With CMake in place, establish robust testing infrastructure to ensure that remaining modernization preserves all original functionality.

**6.1 Testing Framework Selection and Setup**:
- **Unity C Testing Framework**: Lightweight, C89 compatible, perfect for legacy code
- **Test Directory Structure**: Separate tests from source code
  ```
  tests/
  ├── framework/          # Unity testing framework
  ├── unit/              # Unit tests for individual functions
  ├── integration/       # Integration tests for modules
  ├── regression/        # Regression tests for modernization
  ├── security/          # Security-focused tests
  ├── performance/       # Performance benchmarks
  ├── fixtures/          # Test data and mock files
  └── scripts/           # Test automation scripts
  ```

**6.2 Baseline Testing Creation**:
- **Behavioral Baseline Tests**: Document current behavior before changes
- **Critical Function Tests**: Test core game systems (combat, economics, movement)
- **Multi-User Integration Tests**: Test file locking and concurrent access
- **Performance Benchmarks**: Establish performance baselines

**6.3 Automated Test Infrastructure**:
- **Test Runner Scripts**: Automated execution of all test categories
- **Security Analysis Scripts**: Automated vulnerability scanning
- **Coverage Reporting**: Code coverage analysis for modernization validation
- **CI/CD Integration**: Continuous testing during modernization

**6.4 Modern Build System Implementation**:
- **CMake Integration**: Modern build system with testing support
- **Compiler Safety Flags**: Enable all warnings and sanitizers
- **Cross-Platform Support**: Ensure compatibility across target platforms
- **Feature Detection**: Replace manual configuration with automated detection

**Why Testing Infrastructure First:**
- **Safe Modernization**: Catch regressions immediately during code changes
- **Confidence**: Ensure no functionality is lost during modernization
- **Automated Validation**: Reduce manual testing overhead
- **Documentation Validation**: Verify documented behavior matches implementation
- **Security Verification**: Confirm security fixes don't break functionality

### Phase 7: Analyze and Decouple Configuration 🧐

- **Audit the Options**: Go through main configuration header (e.g., `config.h`) and Makefile
- **Identify Dependencies**: Document all external libraries the project depends on
- Extract all environmental and user-choice logic from source code and Makefiles

### Phase 7b: Replace `#ifdef` Trees with Feature Detection 🌳

Create a template file `config.h.in` and use CMake to generate configuration:
```c
// config.h.in
#define BUFFER_SIZE @BUFFER_SIZE@
#cmakedefine HAVE_STRNLEN 1
```

```cmake
include(CheckFunctionExists)
check_function_exists(strnlen HAVE_STRNLEN)
set(BUFFER_SIZE 2048 CACHE STRING "The default buffer size")

configure_file(
    ${PROJECT_SOURCE_DIR}/config.h.in
    ${PROJECT_BINARY_DIR}/config.h
)
```

### Phase 8: Syntactic and Mechanical Modernization ⚙️

**IMPORTANT: Create Automation Scripts**

When doing a task multiple times, Clause should create

Before beginning manual modernization, Claude should test to see if it
can write an automotive script which will do the task that could save
time and ensure consistency across sessions. Each script should be
written to attempt a task and then tested across several *.c and *.h
files as needed. If it is found to consistently work, then it should be
used. If it does not work, then the work will need to be done manually
with files being broken up in short sessions to keep accuracy.

**Possible Example Automation Scripts:**

1. **`convert_kr_functions.py`** - Convert K&R style functions to C2023 prototypes
   - Handles PARM_X macro conversions
   - Preserves comprehensive function documentation
   - Modernizes syntax while maintaining functionality
   - Example usage: `python3 _modernization/scripts/convert_kr_functions.py src/*.c`
   - Thoroughly test per file that conversion worked.

2. **`modernize_headers.py`** - Update include statements and header usage
   - Replace legacy headers with standard equivalents
   - Add missing includes for used functions
   - Remove redundant includes
   - Example usage: `python3 _modernization/scripts/modernize_headers.py src/`

3. **`add_safety_checks.py`** - Insert memory safety and bounds checking
   - Add NULL pointer checks
   - Insert buffer overflow protection
   - Add error handling for malloc/calloc
   - Example usage: `python3 _modernization/scripts/add_safety_checks.py src/module.c`

4. **`fix_integer_types.py`** - Update integer types for 64-bit portability
   - Convert int to size_t for array indices
   - Update printf format specifiers
   - Handle pointer-to-integer conversions
   - Example usage: `python3 _modernization/scripts/fix_integer_types.py src/`

5. **`generate_tests.py`** - Create basic test templates
   - Generate unit test skeletons for functions
   - Create test harness boilerplate
   - Set up test directory structure
   - Example usage: `python3 _modernization/scripts/generate_tests.py src/module.c`

**Script Development Guidelines:**

- **All scripts must use the shebang format specified in global CLAUDE.md**:
  ```python
  #!/usr/bin/env -S uv run --script
  # /// script
  dependencies = ["regex", "pathlib", "argparse"]
  # ///
  ```
- Scripts must be **idempotent** (safe to run multiple times)
- Include **comprehensive error handling** and validation
- Add **--dry-run** option to preview changes without modifying files
- Include **--backup** option to create temporary .orig backups for change validation
- Remove .orig backups after successful validation (either automatically or manually)
- Use backups for diff comparison to verify script changes are correct
- Log all changes made to a timestamped log file in `_modernization/claude/reports/`
- Support **batch processing** of multiple files
- Include **detailed usage documentation** and examples

**Implementation Priority Order:**
1. **Create Automation Scripts**: Build reusable tools for common tasks
2. **Function Documentation**: Complete comprehensive documentation (Phase 3)
3. **Function Prototypes**: Convert K&R style to ANSI prototypes (using convert_kr_functions.py)
4. **Type Safety**: Add proper type declarations and const qualifiers (using script)
5. **Standard Headers**: Replace legacy headers with standard ones (using modernize_headers.py)
6. **Memory Safety**: Add bounds checking and proper error handling (using add_safety_checks.py)
7. **Modern Features**: Introduce C2023 features where beneficial

**Key Modernization Tasks:**

#### Function Declarations
- Convert K&R style function definitions to ANSI C prototypes
- Add `void` to parameterless functions
- Use proper parameter types instead of implicit int

#### Type System Improvements
- Replace implicit int declarations with explicit types
- Add const qualifiers where appropriate
- Use size_t for array indices and memory sizes
- Replace char with explicit signed/unsigned char where needed

#### Standard Library Updates
- Replace deprecated functions (strcpy → strncpy/strlcpy)
- Use safer alternatives (sprintf → snprintf)
- Include proper headers (#include <string.h>, #include <stdlib.h>)
- Use POSIX-compliant functions only
- Avoid GNU extensions or BSD-specific functions

#### Memory Management
- Add proper error checking for malloc/calloc
- Ensure all allocated memory is freed
- Initialize pointers to NULL
- Check for buffer overflows

#### C2023 Features to Consider
- Generic selections (_Generic)
- Static assertions (_Static_assert)
- Thread-local storage (_Thread_local)
- Alignment specifiers (_Alignas, _Alignof)
- Anonymous structs and unions

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

**Static Analysis**: Use tools like Clang Static Analyzer, cppcheck, or PVS-Studio

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

### Session End Management
When the user indicates it's time to end a session, Claude must:
1. Save current progress to a memory file in `_modernization/memory/` using the naming convention above
2. Include completed tasks, current status, and next steps
3. Document any important decisions or discoveries made during the session
4. Ensure the memory file provides sufficient context for future sessions
5. **Git commit session memory files and session logs**: Always remember to `git add` and `git commit` the session memory files in `_modernization/memory/` and session logs in `_modernization/claude/sessions/` at the end of each session to preserve context and progress tracking

### Formal Session Termination
**When the user formally quits a session**, Claude must automatically execute this sequence:

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
  - `_modernization/claude/reports/SYSTEM_ANALYSIS.md` - Complete system architecture and functionality
  - `_modernization/claude/reports/C2023_MODERNIZATION.md` - Detailed modernization tasks and priorities
  - `_modernization/claude/reports/SECURITY_FIXES.md` - Critical security vulnerabilities and fixes
  - `_modernization/claude/reports/DOCUMENTATION_ASSESSMENT.md` - Current documentation state and improvement strategy
  - `_modernization/claude/reports/TESTING_INFRASTRUCTURE.md` - Comprehensive testing framework and strategy
  - `_modernization/claude/reports/MODERNIZATION_PLAN.md` - Complete implementation plan
  - `_modernization/claude/reports/FUTURE_DEVELOPMENT.md` - Long-term development roadmap beyond modernization
