# Persistent Todo List - Conquer Modernization

**Last Updated**: 2025-09-15
**Current Phase**: Phase 7 - Analyze and Decouple Configuration

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

### Phase 7B Next Priority (READY)
- [ ] **Create config.h.in Template**: Replace `#ifdef` trees with CMake feature detection
- [ ] **Implement Security Fixes**: Remove hardcoded credentials and external command execution
- [ ] **Modernize Platform Detection**: Replace legacy platform flags with POSIX standards

### Future Phase Todos
- [ ] Create automation scripts for K&R function conversion
- [ ] Implement security vulnerability fixes
- [ ] Update integer types for 64-bit portability
- [ ] Replace deprecated string functions (strcpy → strncpy)

## User-Added Todos

### Build System Cleanup & Modernization
- [ ] **Convert Legacy Makefiles**: Replace all old Makefiles in subdirectories (Src/, Auxil/, Docs/, Include/) with CMake
- [ ] **Remove Old Makefiles**: Clean up legacy Makefile.src, Makefile.aux, Makefile.dcm, Makefile.inc from repository
- [ ] **Header Consolidation**: Convert Include/header.h and Include/sysconf.h into modern config.h with CMake variable definitions
- [ ] **Static Analysis Integration**: Add clang-tidy testing to identify code quality issues for future modernization
- [ ] **Enhanced GCC Analysis**: Add more GCC flags for comprehensive code analysis and safety checking

### Implementation Notes
- **Makefile Conversion**: Use CMake subdirectory approach or integrate into main CMakeLists.txt
- **Header Modernization**: Use CMake configure_file() to generate config.h from template
- **Static Analysis**: Integrate clang-tidy into CMake build process with custom rules
- **GCC Flags**: Add flags like -Wconversion, -Wsign-conversion, -Wformat-security, -fanalyzer

---
**Note**: This file persists across sessions. Update it using Edit tool during sessions.
**Git Tracked**: Yes, automatically committed with session progress.
