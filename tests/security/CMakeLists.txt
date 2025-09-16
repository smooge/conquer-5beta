#
# Security Tests for Conquer C2023 Modernization
#
# Validate security vulnerability fixes and memory safety
#

# Basic security validation test
add_conquer_test(security_simple_security test_simple_security.c)

# Additional security tests will be added during modernization
# Note: Mock-based tests were removed in favor of unit tests that test actual functions
message(STATUS "Security test directory configured with baseline security tests")