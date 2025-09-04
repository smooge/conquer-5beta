# CURSOR.md

This file provides guidance to Cursor AI when working with code in this repository.

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
gcc -std=c2x -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -g -O2 *.c -o program

# Build with additional safety flags
gcc -std=c2x -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -Werror -g -O2 -fsanitize=address -fsanitize=undefined *.c -o program

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

## Cursor-Specific Guidelines

### Code Analysis and Understanding
- **Always read and understand code before making changes**
- Use Cursor's code navigation features to trace function calls and dependencies
- Leverage Cursor's AI context to understand complex legacy code patterns
- Create visual flowcharts using Mermaid syntax for complex functions
- Store flowcharts in `_modernization/cursor/flowcharts/` directory

### Code Modernization Workflow

#### Phase 1: Assessment and Documentation
1. **System Analysis**: Use Cursor's search and navigation to understand the codebase
2. **Function Documentation**: Document all functions before modernization
3. **Visual Documentation**: Create Mermaid flowcharts for complex functions
4. **Security Analysis**: Identify and document security vulnerabilities

#### Phase 2: Modernization Tasks
1. **Function Prototypes**: Convert K&R style to ANSI prototypes
2. **Type Safety**: Add proper type declarations and const qualifiers
3. **Standard Headers**: Replace legacy headers with standard ones
4. **Memory Safety**: Add bounds checking and proper error handling
5. **Modern Features**: Introduce C2023 features where beneficial

### Code Quality Standards

- All functions must have proper prototypes in header files
- **All routines must have comments at the beginning explaining what the code does**
- Use consistent indentation (4 spaces recommended)
- Add comprehensive error checking
- Document complex algorithms and data structures
- Prefer explicit over implicit type conversions
- Use meaningful variable and function names

### Common Legacy Patterns to Modernize

#### Before (Pre-ANSI):
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

#### After (C2023):
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

## Testing Strategy

### Test Directory Structure
- `tests/` - Main test directory containing all test files
- `tests/unit/` - Unit tests for individual functions and modules
- `tests/integration/` - Integration tests for component interactions
- `tests/regression/` - Regression tests to ensure modernization preserves functionality
- `tests/fixtures/` - Test data files and mock inputs
- `tests/scripts/` - Test runner scripts and utilities

### Testing Requirements
- **Preserve original functionality during modernization**
- **Test each modernized component thoroughly**
- **Create regression tests before making changes**
- Use compiler warnings as early error detection
- Add unit tests for all critical functions
- Test with different compiler versions and flags
- **Cross-platform testing required**: Verify functionality on all target platforms
- Test POSIX compliance using portable system calls
- **Memory safety testing**: Use AddressSanitizer, UndefinedBehaviorSanitizer, and Valgrind

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

When completing a set of work, create a git commit with a comprehensive commit message:

**Commit Message Format:**
```
Brief summary of changes

Prompt: [Description of what the user requested]

Work Completed:
- [List of specific changes made]
- [Include files modified, functions updated, etc.]
- [Note any modernization patterns applied]

Generated by Cursor AI
```

## Project Directory Structure

### Cursor-Specific Directories
- `_modernization/cursor/` - Cursor-specific project files
- `_modernization/cursor/flowcharts/` - Mermaid flowcharts for function documentation
- `_modernization/cursor/analysis/` - Code analysis and modernization reports
- `_modernization/cursor/scripts/` - Automation scripts for modernization tasks

### Documentation Structure
- `_modernization/cursor/analysis/SYSTEM_ANALYSIS.md` - Complete system architecture and functionality
- `_modernization/cursor/analysis/C2023_MODERNIZATION.md` - Detailed modernization tasks and priorities
- `_modernization/cursor/analysis/SECURITY_FIXES.md` - Critical security vulnerabilities and fixes
- `_modernization/cursor/analysis/MODERNIZATION_PLAN.md` - Complete implementation plan
- `_modernization/cursor/analysis/TESTING_INFRASTRUCTURE.md` - Comprehensive testing framework and strategy

## Cursor AI Best Practices

### Code Understanding
- **Always read the entire function before making changes**
- Use Cursor's "Explain Code" feature to understand complex logic
- Create visual flowcharts for functions with complex control flow
- Document assumptions and edge cases

### Code Changes
- **Make incremental changes and test frequently**
- Use Cursor's refactoring tools when available
- Always add proper error handling and bounds checking
- Preserve existing functionality while modernizing

### Documentation
- **Create Mermaid flowcharts for complex functions**
- Document all function parameters and return values
- Explain non-obvious code patterns and legacy workarounds
- Maintain visual documentation alongside code changes

### Testing
- **Create tests before modernizing code**
- Use Cursor's test generation features when available
- Verify functionality on all target platforms
- Maintain comprehensive test coverage

## Migration Notes

- Always make sure original code is checked into git before modernization
- **Complete the Initial Assessment and Planning phase before making any code changes**
- Modernize incrementally, not all at once
- Test frequently during the modernization process
- Document any behavioral changes from the original code
- Maintain compatibility with existing interfaces where possible
- **Create git commits after each completed work session using the format above**
- **Use Cursor's AI features to understand and document complex legacy code patterns**

## Cursor IDE Integration

### Recommended Extensions
- C/C++ Extension Pack
- Mermaid Preview
- GitLens
- Error Lens
- Bracket Pair Colorizer

### Workspace Settings
- Enable all C compiler warnings
- Use clang-format for code formatting
- Configure IntelliSense for C2023 standard
- Set up debugging configurations for all target platforms

### AI Features Usage
- Use "Explain Code" for complex legacy functions
- Leverage "Generate Tests" for critical functions
- Use "Refactor" tools for safe code transformations
- Apply "Fix Issues" for compiler warnings and errors
