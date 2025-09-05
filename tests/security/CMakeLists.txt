#
# Security Tests for Conquer C2023 Modernization
#
# Validate security vulnerability fixes and memory safety
#

# Basic security validation test
add_conquer_test(security_simple_security test_simple_security.c)

# Additional security tests will be added during modernization
message(STATUS "Security test directory configured with initial validation tests")