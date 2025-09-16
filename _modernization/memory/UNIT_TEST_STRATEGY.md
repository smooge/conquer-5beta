# Unit Test Strategy for Conquer Modernization

**Generated**: 2025-09-16
**Total Functions**: ~850+ across 63 source files
**Scope**: Comprehensive unit testing before modernization

## Overview

Based on the function inventory in TEST_FUNC.md, we need to create unit tests for approximately 850+ functions across 63 source files. This requires a systematic, multi-session approach with clear prioritization and tracking.

## File Classification by Priority

### Priority 1: Core Game Engine (High Risk, High Impact)
Critical systems that form the backbone of the game:

- `Src/combatA.c` (29 functions) - Combat system calculations
- `Src/sectorA.c` (19 functions) - Core sector management
- `Src/moveA.c` (1 function) - Movement validation
- `Src/economyA.c` (3 functions) - Economic calculations
- `Src/magicX.c` (11 functions) - Magic system logic
- `Src/mainA.c` (2 functions) - Core application entry points

**Total**: ~65 functions in 6 files

### Priority 2: Data Management & I/O (High Risk, Medium Impact)
Systems that handle data persistence and file operations:

- `Src/ioX.c` (35 functions) - Core I/O operations
- `Src/iodataX.c` (22 functions) - Data file management
- `Src/iodataG.c` (13 functions) - GUI data operations
- `Src/memoryX.c` (38 functions) - Memory management utilities
- `Src/configA.c` (9 functions) - Configuration management
- `Src/adduserA.c` (22 functions) - User management

**Total**: ~139 functions in 6 files

### Priority 3: Game Logic & Calculations (Medium Risk, High Impact)
Systems that implement game rules and calculations:

- `Src/unitsX.c` (63 functions) - Unit management system
- `Src/sectorX.c` (31 functions) - Sector calculations
- `Src/computeX.c` (18 functions) - General computations
- `Src/updateA.c` (17 functions) - Game state updates
- `Src/createA.c` (16 functions) - Entity creation
- `Src/monsterA.c` (9 functions) - Monster behavior
- `Src/miscA.c` (7 functions) - Miscellaneous algorithms

**Total**: ~161 functions in 7 files

### Priority 4: User Interface Systems (Medium Risk, Medium Impact)
GUI and display systems:

- `Src/displayG.c` (30 functions) - Main display system
- `Src/hexmapG.c` (37 functions) - Hexagonal map display
- `Src/infoG.c` (24 functions) - Information displays
- `Src/magicG.c` (25 functions) - Magic interface
- `Src/moveG.c` (24 functions) - Movement interface
- `Src/miscG.c` (33 functions) - GUI utilities
- `Src/ioG.c` (20 functions) - GUI I/O operations

**Total**: ~193 functions in 7 files

### Priority 5: Communication & Multiplayer (Medium Risk, Low Impact)
Mail, messaging, and multiplayer systems:

- `Src/mailG.c` (45 functions) - Mail interface
- `Src/mailA.c` (13 functions) - Mail algorithms
- `Src/mailX.c` (5 functions) - Mail utilities
- `Src/emailG.c` (32 functions) - Email interface
- `Src/ntninfoG.c` (43 functions) - Network information
- `Src/jointA.c` (6 functions) - Joint operations
- `Src/jointG.c` (14 functions) - Joint GUI

**Total**: ~158 functions in 7 files

### Priority 6: Specialized Systems (Low Risk, Various Impact)
Specialized features and utilities:

- **Interface Utilities**: `selectG.c` (15), `selectX.c` (13), `pagerG.c` (18), `keybindG.c` (15)
- **Game Features**: `caravanG.c` (12), `armyG.c` (14), `navyG.c` (13), `enlistG.c` (6)
- **Tools & Conversion**: `convertX.c` (15), `customG.c` (7), `customX.c` (3), `executeX.c` (9)
- **Validation & Checks**: `checkX.c` (12), `time_ckG.c` (11)
- **Editing & Transfer**: `ieditG.c` (15), `xferG.c` (38)
- **Regional Systems**: `regionG.c` (2), `sectorG.c` (3)
- **Platform Specific**: `vms.c` (2)
- **NPC Systems**: `npcA.c` (6)
- **Utilities**: `miscX.c` (12), `hexmapX.c` (2)

**Total**: ~134 functions in 20 files

### Auxiliary & Documentation
- `Auxil/psmap.c` (9 functions) - PostScript mapping
- `Auxil/sort.c` (6 functions) - Sorting utilities
- `Docs/ezconv.c` (1 function) - Documentation converter

**Total**: ~16 functions in 3 files

## Implementation Strategy

### Phase 6B-1: Foundation & High Priority (Sessions 1-3)
**Target**: Priority 1 files (Core Game Engine)
- Start with single-function files (`moveA.c`) for template development
- Progress to complex files (`combatA.c`, `sectorA.c`)
- Establish testing patterns and automation scripts

### Phase 6B-2: Data & Logic Systems (Sessions 4-8)
**Target**: Priority 2 & 3 files (Data Management & Game Logic)
- Focus on data integrity and memory safety
- Test file operations and data persistence
- Validate calculation accuracy

### Phase 6B-3: Interface & Communication (Sessions 9-12)
**Target**: Priority 4 & 5 files (UI & Communication)
- Test display functions (may require mocking)
- Validate communication protocols
- Test user input handling

### Phase 6B-4: Specialized & Auxiliary (Sessions 13-15)
**Target**: Priority 6 files and auxiliary code
- Complete remaining specialized systems
- Test utility functions and tools
- Finalize auxiliary programs

## Session Planning Template

### Per-Session Approach
**Target**: 1-3 files per session (20-50 functions)
**Time Estimate**: 2-3 hours per session
**Deliverables**:
1. Unit test file(s) for target source file(s)
2. Updated CMakeLists.txt with new test targets
3. Documentation of test coverage and findings
4. Git commit with test implementation

### Session Structure
1. **Analysis** (20 mins): Review target file(s), understand functions
2. **Test Design** (30 mins): Plan test cases for each function
3. **Implementation** (60-90 mins): Write unit tests
4. **Validation** (20 mins): Run tests, fix issues
5. **Documentation** (10 mins): Update tracking and commit

## Automation & Templates

### Test File Naming Convention
- Unit tests: `tests/unit/test_[filename].c`
- Example: `tests/unit/test_combatA.c` for `Src/combatA.c`

### Required Automation Scripts
1. **`generate_unit_test_skeleton.py`** - Create test file templates
2. **`analyze_functions.py`** - Extract function signatures from source
3. **`update_cmake_tests.py`** - Add new tests to CMakeLists.txt
4. **`run_file_tests.py`** - Execute tests for specific source files

### Test Template Structure
```c
/*
 * Unit Tests for Src/[filename].c
 * Generated: [date]
 * Functions tested: [count]
 */

#include "unity.h"
#include "../Include/[headers].h"

void setUp(void) {
    // Test setup
}

void tearDown(void) {
    // Test cleanup
}

// Test functions for each source function
void test_function_name_basic(void) {
    // Basic functionality test
}

void test_function_name_edge_cases(void) {
    // Edge case testing
}

void test_function_name_error_conditions(void) {
    // Error handling tests
}

int main(void) {
    UNITY_BEGIN();

    // Add test cases
    RUN_TEST(test_function_name_basic);
    RUN_TEST(test_function_name_edge_cases);
    RUN_TEST(test_function_name_error_conditions);

    return UNITY_END();
}
```

## Progress Tracking

### Tracking Format
We'll use a structured format to track progress:

```
| File | Functions | Tests Written | Tests Passing | Coverage % | Status | Session |
|------|-----------|---------------|---------------|------------|--------|---------|
| Src/moveA.c | 1 | 0 | 0 | 0% | Pending | - |
```

### Success Metrics
- **Coverage Target**: 95%+ function coverage
- **Quality Target**: All critical paths tested
- **Safety Target**: Memory leaks and crashes eliminated
- **Documentation Target**: All test purposes documented

## Risk Mitigation

### High-Risk Areas Requiring Extra Attention
1. **Memory Management** (`memoryX.c`) - Memory leaks, double-free
2. **File I/O** (`ioX.c`, `iodataX.c`) - Data corruption, file locking
3. **Combat System** (`combatA.c`) - Game balance, calculation accuracy
4. **Unit Management** (`unitsX.c`) - Data integrity, complex state

### Testing Challenges
1. **GUI Functions**: May require mocking display systems
2. **File Operations**: Need temporary test files and cleanup
3. **Legacy Code**: Unclear function contracts may need documentation first
4. **Complex State**: Functions with many dependencies need setup/teardown

## Integration with Modernization Plan

### Before Modernization
- Complete unit test coverage for target functions
- Establish baseline behavior documentation
- Create regression test suite

### During Modernization
- Run tests after each modernization change
- Update tests if function signatures change
- Maintain behavioral compatibility

### After Modernization
- Verify all tests still pass
- Add new tests for modernized features
- Document any behavioral changes

---

**Next Action**: Begin Phase 6B-1 with Priority 1 files, starting with `Src/moveA.c` (1 function) to establish testing patterns.