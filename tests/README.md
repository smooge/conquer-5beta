# Testing Infrastructure for Conquer C2023 Modernization

This directory contains comprehensive testing infrastructure for the safe modernization of the Conquer game codebase from pre-ANSI C to C2023 standards.

## Directory Structure

```
tests/
├── framework/          # Unity C Testing Framework and configuration
├── unit/              # Unit tests for individual functions
├── integration/       # Integration tests for component interactions  
├── regression/        # Regression tests to ensure modernization preserves functionality
├── security/          # Security-focused tests for vulnerability validation
├── performance/       # Performance benchmarks
├── fixtures/          # Test data and mock files
├── scripts/           # Test automation scripts
└── build/             # CMake build directory (generated)
```

## Quick Start

### Run All Tests
```bash
cd tests
./scripts/run_all_tests.sh
```

### Quick Validation (Essential Tests Only)
```bash
cd tests  
./scripts/quick_test.sh
```

### Security Tests with Memory Sanitizers
```bash
cd tests
./scripts/run_security_tests.sh
```

### Manual CMake Build
```bash
cd tests
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
make test_all
```

## Testing Framework

- **Unity C Testing Framework**: Lightweight, C89 compatible testing framework
- **CMake Integration**: Modern build system with automated test discovery
- **Memory Sanitizers**: AddressSanitizer and UndefinedBehaviorSanitizer support
- **Cross-Platform**: Compatible with all target platforms (Debian, Fedora, macOS, FreeBSD)

## Test Categories

### Regression Tests (`tests/regression/`)
Ensure that modernization preserves all original functionality. These tests establish baseline behavior before code changes and validate no regressions occur.

**Current Tests:**
- `test_input_validation.c` - Basic input validation and string handling

### Security Tests (`tests/security/`) 
Validate security vulnerability fixes and memory safety improvements during modernization.

**Current Tests:**
- `test_simple_security.c` - Basic security validation including buffer overflow protection

### Unit Tests (`tests/unit/`)
Individual function and module testing. Tests will be added as functions are modernized.

### Integration Tests (`tests/integration/`)
Component interaction and system-level testing for complex game systems.

## Usage During Modernization

1. **Before Code Changes**: Run full test suite to establish baseline
2. **During Modernization**: Run quick tests frequently for immediate feedback  
3. **After Changes**: Run security tests to validate vulnerability fixes
4. **Before Commits**: Run full test suite to ensure no regressions

## CMake Targets

- `make test_all` - Run all test categories
- `make test_unit` - Run only unit tests
- `make test_integration` - Run only integration tests  
- `make test_regression` - Run only regression tests
- `make test_security` - Run only security tests

## Security Features

- **Memory Sanitizers**: Compile with `-fsanitize=address` and `-fsanitize=undefined`
- **Buffer Overflow Detection**: Validate safe string operations
- **Memory Leak Detection**: Ensure proper memory management
- **Format String Safety**: Prevent format string vulnerabilities

## Adding New Tests

1. Create test file in appropriate category directory
2. Include Unity framework headers: `#include "../framework/unity.h"`
3. Add test to corresponding `CMakeLists.txt` using `add_conquer_test()` function
4. Run tests to validate

## Integration with C2023 Modernization

This testing infrastructure supports the 10-phase modernization plan:

- **Phase 3**: Testing Infrastructure Setup ✅ **COMPLETE**
- **Phase 4**: Function Documentation (tests validate documented behavior)
- **Phase 8**: Syntactic Modernization (regression tests ensure compatibility)
- **Phase 9**: Security Fixes (security tests validate vulnerability remediation)
- **Phase 10**: Final Validation (comprehensive test suite confirms success)

## Generated Files

The following files are generated during testing and should not be committed:
- `tests/build/` - CMake build directory  
- `tests/*/test_*` - Compiled test executables (without .c extension)
- `*.o`, `*.gcov`, `*.tmp` - Build artifacts

See `.gitignore` for complete list of excluded files.