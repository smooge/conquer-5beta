#
# Regression Tests for Conquer C2023 Modernization
#
# Ensure modernization preserves original functionality
#

# Basic input validation regression test
add_conquer_test(regression_input_validation test_input_validation.c)

# Compilation verification regression test - validates Phase 7A.1 success
add_conquer_test(regression_compilation_verification test_compilation_verification.c)

# Data file integrity baseline test - validates file operations before modernization
# add_conquer_test(regression_data_file_integrity test_data_file_integrity.c) # TODO: Create this test file

# Additional regression tests will be added during modernization
message(STATUS "Regression test directory configured with baseline tests")