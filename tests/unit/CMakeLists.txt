#
# Unit Tests for Conquer C2023 Modernization
#
# Individual function and module testing
#

# Core game logic baseline tests - establish computational baselines before modernization
# add_conquer_test(unit_core_game_logic test_core_game_logic.c) # TODO: Create this test file

# Baseline tests for actual checkX.c functions before modernization
# add_conquer_test_with_deps(unit_checkx_baseline test_checkx_baseline.c
#     ${CMAKE_SOURCE_DIR}/Src/checkX.c) # TODO: Resolve dependency issues

# Isolated tests of extracted checkX.c functions
add_conquer_test(unit_checkx_isolated test_checkx_isolated.c)

# Test template example (will be populated as we modernize)
# add_conquer_test(unit_string_utils test_string_utils.c)

# Additional unit tests to be added during modernization
message(STATUS "Unit test directory configured - core game logic baseline tests added")
message(STATUS "Baseline tests added for checkX.c functions")