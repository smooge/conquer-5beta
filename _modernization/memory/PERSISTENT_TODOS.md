# Persistent Todo List - Conquer Modernization

**Last Updated**: 2025-09-15
**Current Phase**: Phase 6A - Function Testability Analysis (Option A Complete)

## High Priority Todos

### Phase 6 Testing (COMPLETED ✅)
- [x] Fix failing CTest `regression_compilation_verification`
- [x] Establish Unity testing framework
- [x] Create comprehensive test directory structure
- [x] Build baseline regression tests

### Phase 6B - Additional Baseline Tests (COMPLETED ✅)
- [x] **Application Startup Tests**: Both executables launch without crashing, parse arguments correctly
- [x] **Data File Integrity Tests**: Game data files read without corruption, save/load preserves state
- [x] **Core Game Logic Tests**: Combat calculations, economic system, movement validation work correctly
- [x] **Memory Safety Baseline**: No memory leaks, buffer overflow protection, clean shutdown
- [ ] **Multi-User Concurrent Access**: File locking prevents corruption, multiple players access safely
- [ ] **Cross-Platform Compatibility**: Runs on target platforms (Debian, Fedora, macOS, FreeBSD)
- [ ] **Performance Benchmarks**: Turn processing time, memory usage, file I/O performance baselines

### Phase 7 Configuration Analysis (COMPLETED ✅)
- [x] **Audit Configuration Options**: Go through main configuration header (config.h) and Makefile
- [x] **Identify Dependencies**: Document all external libraries the project depends on
- [x] **Extract Environmental Logic**: Remove all environmental and user-choice logic from source code and Makefiles

### Phase 6A Function Analysis (COMPLETED ✅)
- [x] **Analyze economyA.c**: Economic system functions (3 functions - upd_sectors, upd_produce, upd_consume)
- [x] **Analyze moveA.c**: Movement system functions (1 function - npc_movearmy)
- [x] **Analyze dataA.c**: Core data definitions (3 components - name generation, global state)
- [x] **Create Priority Matrix**: Comprehensive Phase 6B testing strategy with critical function identification

### Phase 6B Testing Implementation (READY)
- [ ] **Critical Function Testing**: upd_consume() - 790+ line economic function with security implications
- [ ] **Core Production Testing**: upd_produce() - Resource production system with overflow protection
- [ ] **Security Function Testing**: checkX.c functions (canseelogin, user_exists, fork_edit_on_file)
- [ ] **Economic Integration Testing**: End-to-end resource flow validation
- [ ] **Baseline Performance**: Economic processing benchmarks before modernization

### Phase 7B Next Priority (DEFERRED)
- [ ] **Create config.h.in Template**: Replace `#ifdef` trees with CMake feature detection
- [ ] **Implement Security Fixes**: Remove hardcoded credentials and external command execution
- [ ] **Modernize Platform Detection**: Replace legacy platform flags with POSIX standards

### Future Phase Todos
- [ ] Create automation scripts for K&R function conversion
- [ ] Implement security vulnerability fixes
- [ ] Update integer types for 64-bit portability
- [ ] Replace deprecated string functions (strcpy → strncpy)

## User-Added Todos

### CLAUDE.md Maintenance (COMPLETED ✅)
- [x] **Fix Build Command Inconsistencies**: Standardize all gcc commands in CLAUDE.md (lines 26, 29, 88, 32-33 have variations)
- [x] **Consolidate Documentation Standards**: Move scattered documentation requirements to dedicated section
- [ ] **Monthly CLAUDE.md Audits**: Use maintenance strategy for regular conflict detection and cleanup

### Build System Cleanup & Modernization
- [ ] **Convert Legacy Makefiles**: Replace all old Makefiles in subdirectories (Src/, Auxil/, Docs/, Include/) with CMake
- [ ] **Remove Old Makefiles**: Clean up legacy Makefile.src, Makefile.aux, Makefile.dcm, Makefile.inc from repository
- [ ] **Header Consolidation**: Convert Include/header.h and Include/sysconf.h into modern config.h with CMake variable definitions
- [x] **Static Analysis Integration**: Add clang-tidy testing to identify code quality issues for future modernization
- [ ] **Enhanced GCC Analysis**: Add more GCC flags for comprehensive code analysis and safety checking

### Implementation Notes
- **Makefile Conversion**: Use CMake subdirectory approach or integrate into main CMakeLists.txt
- **Header Modernization**: Use CMake configure_file() to generate config.h from template
- **Static Analysis**: Integrate clang-tidy into CMake build process with custom rules
- **GCC Flags**: Add flags like -Wconversion, -Wsign-conversion, -Wformat-security, -fanalyzer

---
**Note**: This file persists across sessions. Update it using Edit tool during sessions.
**Git Tracked**: Yes, automatically committed with session progress.
