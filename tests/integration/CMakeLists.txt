#
# Integration Tests for Conquer C2023 Modernization
#
# Component interaction and system-level testing
#

# Application Startup Tests
add_executable(integration_application_startup test_application_startup.c)
target_link_libraries(integration_application_startup unity)

# Register with CTest
add_test(NAME integration_application_startup COMMAND integration_application_startup)

message(STATUS "Integration tests configured - application startup tests added")