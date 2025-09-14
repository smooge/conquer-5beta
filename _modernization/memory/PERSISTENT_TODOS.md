# Persistent Todo List - Conquer Modernization

**Last Updated**: 2025-09-14
**Current Phase**: Phase 4 - Testing Infrastructure

## High Priority Todos

### Phase 4 Testing (Current Phase)
- [ ] Fix failing CTest `regression_compilation_verification`
- [ ] Establish Unity testing framework
- [ ] Create comprehensive test directory structure
- [ ] Build baseline regression tests

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