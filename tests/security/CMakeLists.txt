#
# Security Tests for Conquer C2023 Modernization
#
# Validate security vulnerability fixes and memory safety
#

# Basic security validation test
add_conquer_test(security_simple_security test_simple_security.c)

# Phase 6B Critical Security Function Tests - checkX.c
add_conquer_test(security_checkx_canseelogin test_checkx_canseelogin.c)
add_conquer_test(security_checkx_user_exists test_checkx_user_exists.c)
add_conquer_test(security_checkx_check_lock test_checkx_check_lock.c)

# Additional security tests will be added during modernization
message(STATUS "Security test directory configured with baseline security tests")
message(STATUS "Phase 6B critical security tests added: canseelogin()")