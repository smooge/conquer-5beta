# Modernization Plan - Conquer Game

**Target**: C2023 with POSIX compliance
**Effort**: 12-16 weeks
**Files**: 67 C source files, 49 header files

## Status & Critical Issues

**Current Priority**: Fix 43 of 67 files that fail compilation with C2023 standards

**Phase Progress**:
- ✅ Phase 1-2: Environment & Analysis complete
- ✅ Phase 7A.1-2: Header modernization, Unity testing
- ⏳ **CRITICAL**: Phase 7A.3 - Fix compilation failures

**Compilation Results** (67 files tested):
- ✅ Clean: 16 files (23.9%)
- ⚠️ Warnings: 8 files (11.9%)
- ❌ Errors: 43 files (64.2%)

**Error Categories**:
1. Syntax errors: 27 files
2. Integer signedness: 13 files
3. Fallthrough warnings: 11 files
4. Missing declarations: 10 files
5. K&R conflicts: 3 files
6. Signal handlers: 2 files

## Security Vulnerabilities - IMMEDIATE ACTION REQUIRED

### Critical (CVSS 9.8)
**gets() Function** - Complete system compromise possible
- Files: mainA.c:285, mainG.c:296/482, vms.c:49
- Fix: Replace with fgets() and bounds checking

### High Risk (CVSS 8.1)
**scanf() Functions** - Buffer overflow potential
- Files: mainA/G.c, executeX.c, unitsX.c, hexmapG.c
- Fix: Add buffer size limits to scanf("%s") calls

### String Safety Issues
**1121 instances** of unsafe functions across 46 files:
- strcpy(), strcat(), sprintf() without bounds checking
- Fix: Replace with strncpy(), strncat(), snprintf()

### Other Security Issues
- **Password Security**: 2-char salt, plaintext fallback, 8-char limit
- **File Security**: Directory traversal risk, race conditions
- **Memory Safety**: Stack buffers without bounds checking

## C2023 Compliance Issues

### 1. Function Declarations (Critical)
**Issue**: All functions use PARM_X macros for K&R/ANSI compatibility
```c
static int fname_char PARM_1(int, ch)  // Current
static int fname_char(int ch)          // Target
```
**Required**: Convert 500+ functions, remove PARM_X macros, update headers

### 2. Platform Configuration
**Issue**: Manual platform detection with `#ifdef` trees
**Required**: Replace with CMake feature detection, eliminate platform-specific blocks

### 3. Type System
**Issues**: Custom types, 32-bit assumptions, mixed definitions
**Required**: Replace with `<stdint.h>` types, use `size_t` for indices, update format specifiers

### 4. Header Organization
**Issues**: Circular dependencies, missing guards, platform-specific includes
**Required**: Reorganize includes, add header guards, standardize on POSIX

## Implementation Phases

### Phase 7A.3: Compilation Fixing (2-3 weeks) 🚨
**Week 1**: Fix header syntax errors (27 files), K&R conflicts (3 files), missing declarations (10 files)
**Week 2**: Signal handler signatures (2 files), function signature updates
**Week 3**: Remaining errors, validation
**Target**: 100% compilation success, zero errors

### Phase 4: Security & Function Modernization (3-4 weeks)
**Week 1 (Critical)**: Replace gets(), fix scanf(), audit string functions
**Week 2-3**: Convert PARM_X macros (500+ functions), type improvements
**Week 4**: Memory safety, error handling, password security

### Phase 5: Build System & Configuration (2 weeks)
- CMake implementation with C2023 standards
- Feature detection replacing `#ifdef` trees
- Library detection (ncurses, crypt)
- CTest integration

### Phase 6: Testing Infrastructure (1 week)
- Unity C testing framework (complete)
- Baseline tests for current behavior
- Security and regression test suites

### Phase 8: Type System & Portability (2-3 weeks)
- Convert to size_t, int32_t/uint32_t
- Update format specifiers
- 64-bit portability fixes
- Modern C features

### Phase 9: Analysis & Optimization (1 week)
- Static analysis (clang-tidy, cppcheck)
- Dynamic analysis (sanitizers, valgrind)
- Final validation

## Immediate Action Plan

**Phase 1 (Critical - 24 hours)**:
1. Replace all gets() calls with fgets()
2. Fix scanf("%s") calls with buffer limits
3. Audit and test all security fixes

**Phase 2 (High - 1 week)**:
1. Replace unsafe string functions
2. Fix compilation errors systematically
3. Improve password security

**Phase 3 (Medium - 2 weeks)**:
1. Convert PARM_X macros to ANSI prototypes
2. Implement CMake feature detection
3. File validation and signal handler safety

## Risk Mitigation

**High Risks**: Security vulnerabilities, compilation failures, legacy complexity
**Strategy**: Security fixes first, systematic compilation fixing, comprehensive testing

**Testing Foundation**: Unity framework operational, regression tests ready

## Resources

**Team**: Primary developer (8-12 weeks), testing specialist, security expert
**Infrastructure**: Multi-platform development environment, modern compilers

## Success Criteria

1. 100% compilation success with C2023 standards
2. All critical security vulnerabilities fixed
3. Complete PARM_X macro conversion
4. Modern build system operational
5. Comprehensive test coverage

**Goal**: Zero regressions during modernization, maintained functionality