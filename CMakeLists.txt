cmake_minimum_required(VERSION 3.10)

# Project definition
project(Conquer
    VERSION 5.0
    DESCRIPTION "Conquer Strategy Game - Modern C2023 Port"
    LANGUAGES C
)

# Set C standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Include necessary CMake modules
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckLibraryExists)

# Build configuration
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build.")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

# Compiler flags for POSIX compliance and modern safety
set(CMAKE_C_FLAGS "-D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -fsanitize=address -fsanitize=undefined")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/Include)
include_directories(${PROJECT_BINARY_DIR})  # For generated config.h

# Source file groups (from Makefile analysis)
set(GFILS
    Src/mainG.c Src/armyG.c Src/caravanG.c Src/customG.c Src/dataG.c
    Src/displayG.c Src/emailG.c Src/enlistG.c Src/hexmapG.c Src/ieditG.c
    Src/infoG.c Src/ioG.c Src/iodataG.c Src/keybindG.c Src/magicG.c
    Src/mailG.c Src/miscG.c Src/moveG.c Src/navyG.c Src/ntninfoG.c
    Src/pagerG.c Src/regionG.c Src/sectorG.c Src/selectG.c Src/xferG.c
    Src/time_ckG.c Src/jointG.c
)

set(AFILS
    Src/mainA.c Src/adduserA.c Src/combatA.c Src/configA.c Src/createA.c
    Src/dataA.c Src/economyA.c Src/magicA.c Src/mailA.c Src/miscA.c
    Src/monsterA.c Src/moveA.c Src/npcA.c Src/sectorA.c Src/updateA.c
    Src/jointA.c
)

set(XFILS
    Src/dataX.c Src/datamagX.c Src/datamilX.c Src/checkX.c Src/computeX.c
    Src/convertX.c Src/customX.c Src/executeX.c Src/hexmapX.c Src/ioX.c
    Src/iodataX.c Src/magicX.c Src/mailX.c Src/memoryX.c Src/miscX.c
    Src/moveX.c Src/sectorX.c Src/selectX.c Src/unitsX.c
)

# Platform-specific source files
if(CMAKE_SYSTEM_NAME STREQUAL "OpenVMS")
    list(APPEND XFILS Src/vms.c)
endif()

# Check for getopt and add if needed
check_function_exists(getopt HAVE_GETOPT)
if(NOT HAVE_GETOPT)
    list(APPEND XFILS Src/getopt.c)
endif()

# Configuration options (from Phase 5 analysis)
option(CONQUER_ENABLE_COMPRESSION "Enable data file compression" ON)
option(CONQUER_LARGE_MAPS "Support maps larger than 256x256" ON)
option(CONQUER_MANY_UNITS "Support more than 255 units per formation" ON)
option(CONQUER_SECURITY_MODE "Enable security restrictions" ON)
option(CONQUER_ENABLE_JOINT "Enable joint operation features" ON)

# Feature Detection (based on Phase 5 analysis)
message(STATUS "Performing feature detection...")

# Core system capabilities
check_function_exists(mkdir HAVE_MKDIR)
check_function_exists(crypt HAVE_CRYPT)
check_function_exists(flock HAVE_FLOCK)
check_function_exists(lockf HAVE_LOCKF)
check_function_exists(lrand48 HAVE_LRAND48)
check_function_exists(random HAVE_RANDOM)
check_function_exists(setreuid HAVE_SETREUID)
check_function_exists(setpriority HAVE_SETPRIORITY)
check_function_exists(uname HAVE_UNAME)
check_function_exists(gethostname HAVE_GETHOSTNAME)
check_function_exists(bzero HAVE_BZERO)
check_function_exists(cuserid HAVE_CUSERID)
check_function_exists(getdtablesize HAVE_GETDTABLESIZE)

# Header availability
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(crypt.h HAVE_CRYPT_H)
check_include_file(sys/file.h HAVE_SYS_FILE_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(memory.h HAVE_MEMORY_H)
check_include_file(malloc.h HAVE_MALLOC_H)

# String function variants
check_symbol_exists(strchr string.h HAVE_STRCHR)
check_symbol_exists(index strings.h HAVE_INDEX)

# Advanced features
check_include_file(regex.h HAVE_REGEX_H)
if(HAVE_REGEX_H)
    check_function_exists(regcomp HAVE_REGEXP)
endif()

# Signal handling
check_include_file(signal.h HAVE_SIGNAL_H)
if(HAVE_SIGNAL_H)
    check_symbol_exists(SIGWINCH signal.h HAVE_SIGWINCH)
endif()

# Library Discovery and Linking
message(STATUS "Discovering required libraries...")

# Terminal libraries (REQUIRED)
find_package(Curses REQUIRED)
if(CURSES_FOUND)
    message(STATUS "Found curses library: ${CURSES_LIBRARIES}")
    set(HAVE_CURSES_H ${CURSES_HAVE_CURSES_H})
    set(HAVE_NCURSES_H ${CURSES_HAVE_NCURSES_H})
    list(APPEND REQUIRED_LIBS ${CURSES_LIBRARIES})
else()
    message(FATAL_ERROR "Curses library is required but not found")
endif()

# Cryptographic libraries (CRITICAL for save compatibility)
find_library(CRYPT_LIB NAMES crypt crypt_i)
if(CRYPT_LIB)
    message(STATUS "Found crypt library: ${CRYPT_LIB}")
    list(APPEND REQUIRED_LIBS ${CRYPT_LIB})
    set(HAVE_LIBCRYPT 1)
else()
    # Check if crypt is in libc
    check_function_exists(crypt HAVE_CRYPT_IN_LIBC)
    if(HAVE_CRYPT_IN_LIBC)
        message(STATUS "crypt function found in libc")
        set(HAVE_LIBCRYPT 1)
    else()
        message(WARNING "crypt function not found - save file compatibility may be affected")
    endif()
endif()

# Math library (usually implicit but check anyway)
find_library(MATH_LIB m)
if(MATH_LIB)
    list(APPEND REQUIRED_LIBS ${MATH_LIB})
endif()

# Compression libraries (optional, for modern compression support)
find_package(ZLIB)
if(ZLIB_FOUND AND CONQUER_ENABLE_COMPRESSION)
    option(USE_ZLIB_COMPRESSION "Use zlib instead of external compress" ON)
    if(USE_ZLIB_COMPRESSION)
        message(STATUS "Found zlib: ${ZLIB_LIBRARIES}")
        list(APPEND REQUIRED_LIBS ${ZLIB_LIBRARIES})
        set(HAVE_ZLIB 1)
    endif()
endif()

# Regular expression library (if available and requested)
if(HAVE_REGEX_H AND HAVE_REGEXP)
    find_library(REGEX_LIB regex)
    if(REGEX_LIB)
        list(APPEND REQUIRED_LIBS ${REGEX_LIB})
    endif()
endif()

# Configuration values
set(CONQUER_MAX_NATIONS 100 CACHE STRING "Maximum number of nations")
set(CONQUER_VERSION "${PROJECT_VERSION}" CACHE STRING "Game version string")
set(CONQUER_ADMIN_USER "$ENV{USER}" CACHE STRING "Default administrator user")

# Data directories
set(CONQUER_DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/conquer" CACHE PATH "Game data directory")
set(CONQUER_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Executable directory")

# Generate configuration header
configure_file(
    ${PROJECT_SOURCE_DIR}/config.h.in
    ${PROJECT_BINARY_DIR}/config.h
    @ONLY
)

# CMake build system test program
add_executable(cmake_test cmake_test.c)
target_link_libraries(cmake_test ${REQUIRED_LIBS})

# Main executables (enabled - Phase 3 implementation)
add_executable(conquer ${GFILS} ${XFILS})
add_executable(conqrun ${AFILS} ${XFILS})

# Special compilation flags for specific files (following legacy Makefile)
set_source_files_properties(Src/checkX.c PROPERTIES
    COMPILE_FLAGS "-g -DDEBUG"
)

set_source_files_properties(Src/miscA.c PROPERTIES
    COMPILE_DEFINITIONS "CONQ_SORT=\"conqsort\""
)

set_source_files_properties(Src/customX.c PROPERTIES
    COMPILE_DEFINITIONS "DEFAULTDIR=\"${CONQUER_DATA_DIR}\";EXEDIR=\"${CONQUER_BIN_DIR}\""
)

# Link libraries to executables
target_link_libraries(conquer ${REQUIRED_LIBS})
target_link_libraries(conqrun ${REQUIRED_LIBS})

# Set target properties
set_target_properties(conquer PROPERTIES
    OUTPUT_NAME conquer
    COMPILE_DEFINITIONS "CONQUER_GAME_INTERFACE"
)

set_target_properties(conqrun PROPERTIES
    OUTPUT_NAME conqrun
    COMPILE_DEFINITIONS "CONQUER_ADMIN_INTERFACE"
)

# Testing infrastructure - Phase 7A.2 Unity framework
enable_testing()
add_subdirectory(tests)

# Installation targets
install(TARGETS conquer conqrun
    RUNTIME DESTINATION bin
)

install(DIRECTORY Auxil/
    DESTINATION share/conquer/auxil
    FILES_MATCHING PATTERN "*"
)

# Print configuration summary
message(STATUS "Conquer Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Standard: C${CMAKE_C_STANDARD}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Data Directory: ${CONQUER_DATA_DIR}")
message(STATUS "  Large Maps: ${CONQUER_LARGE_MAPS}")
message(STATUS "  Many Units: ${CONQUER_MANY_UNITS}")
message(STATUS "  Compression: ${CONQUER_ENABLE_COMPRESSION}")
message(STATUS "  Security Mode: ${CONQUER_SECURITY_MODE}")