#
# CMake Configuration for Conquer C2023 Modernization Testing
#
# This file sets up comprehensive testing infrastructure for safe
# modernization of legacy C code to C2023 standards.
#

cmake_minimum_required(VERSION 3.10)

# Configure testing project
project(ConquerTests C)

# Set C2023 standard with POSIX compliance
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Compiler flags for modernization testing
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c2x -D_POSIX_C_SOURCE=200809L")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -g -O2")

# Enable testing
enable_testing()

# Unity testing framework setup
set(UNITY_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/framework)
add_library(unity STATIC 
    ${UNITY_ROOT}/unity.c
)
target_include_directories(unity PUBLIC ${UNITY_ROOT})

# Include directories for Conquer source code
set(CONQUER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(unity PUBLIC ${CONQUER_SRC_DIR})

# Function to create a test executable
function(add_conquer_test TEST_NAME TEST_SOURCE)
    # Create test executable
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    # Link with Unity framework
    target_link_libraries(${TEST_NAME} unity)
    
    # Set C2023 compliance flags
    target_compile_options(${TEST_NAME} PRIVATE 
        -std=c2x -D_POSIX_C_SOURCE=200809L
        -Wall -Wextra -Wpedantic -g -O2
    )
    
    # Add to CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    
    # Set test properties
    set_tests_properties(${TEST_NAME} PROPERTIES
        TIMEOUT 30
        FAIL_REGULAR_EXPRESSION "FAIL"
    )
endfunction()

# Function to create a test with source dependencies
function(add_conquer_test_with_deps TEST_NAME TEST_SOURCE)
    # Get additional source files from ARGN
    set(EXTRA_SOURCES ${ARGN})
    
    # Create test executable with dependencies
    add_executable(${TEST_NAME} ${TEST_SOURCE} ${EXTRA_SOURCES})
    
    # Link with Unity framework
    target_link_libraries(${TEST_NAME} unity)
    
    # Set C2023 compliance flags
    target_compile_options(${TEST_NAME} PRIVATE 
        -std=c2x -D_POSIX_C_SOURCE=200809L
        -Wall -Wextra -Wpedantic -g -O2
    )
    
    # Add to CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    
    # Set test properties
    set_tests_properties(${TEST_NAME} PROPERTIES
        TIMEOUT 60
        FAIL_REGULAR_EXPRESSION "FAIL"
    )
endfunction()

# Add subdirectories for different test categories
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(regression)
add_subdirectory(security)

# Custom targets for running different test categories
add_custom_target(test_unit
    COMMAND ${CMAKE_CTEST_COMMAND} -R "unit_.*" --output-on-failure
    DEPENDS unity
    COMMENT "Running unit tests"
)

add_custom_target(test_integration
    COMMAND ${CMAKE_CTEST_COMMAND} -R "integration_.*" --output-on-failure
    DEPENDS unity
    COMMENT "Running integration tests"
)

add_custom_target(test_regression
    COMMAND ${CMAKE_CTEST_COMMAND} -R "regression_.*" --output-on-failure
    DEPENDS unity
    COMMENT "Running regression tests"
)

add_custom_target(test_security
    COMMAND ${CMAKE_CTEST_COMMAND} -R "security_.*" --output-on-failure
    DEPENDS unity
    COMMENT "Running security tests"
)

add_custom_target(test_all
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS unity
    COMMENT "Running all tests"
)

# Coverage support (optional)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
if(ENABLE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Memory sanitizer support for security testing
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
if(ENABLE_ASAN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
if(ENABLE_UBSAN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()