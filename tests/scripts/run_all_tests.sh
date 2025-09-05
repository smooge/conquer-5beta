#!/bin/bash

#
# Comprehensive Test Runner for Conquer C2023 Modernization
#
# This script runs all test categories and provides detailed reporting
# for safe modernization validation.
#

set -e  # Exit on any error

# Configuration
BUILD_DIR="build"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="$(dirname "$SCRIPT_DIR")"
PROJECT_ROOT="$(dirname "$TEST_DIR")"

echo "=== Conquer C2023 Modernization Test Runner ==="
echo "Project Root: $PROJECT_ROOT"
echo "Test Directory: $TEST_DIR"
echo "Build Directory: $BUILD_DIR"
echo

# Create build directory if it doesn't exist
cd "$TEST_DIR"
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Configure with CMake
echo "Configuring tests with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build all tests
echo "Building all tests..."
make -j$(nproc)

echo
echo "=== Running Test Categories ==="

# Function to run a test category
run_test_category() {
    local category=$1
    local description=$2
    
    echo
    echo "--- $description ---"
    if make test_$category 2>&1; then
        echo "✅ $description: PASSED"
        return 0
    else
        echo "❌ $description: FAILED"
        return 1
    fi
}

# Track test results
PASSED=0
FAILED=0

# Run unit tests
if run_test_category "unit" "Unit Tests"; then
    ((PASSED++))
else
    ((FAILED++))
fi

# Run integration tests
if run_test_category "integration" "Integration Tests"; then
    ((PASSED++))
else
    ((FAILED++))
fi

# Run regression tests
if run_test_category "regression" "Regression Tests"; then
    ((PASSED++))
else
    ((FAILED++))
fi

# Run security tests
if run_test_category "security" "Security Tests"; then
    ((PASSED++))
else
    ((FAILED++))
fi

echo
echo "=== Test Summary ==="
echo "Categories Passed: $PASSED"
echo "Categories Failed: $FAILED"
echo "Total Categories: $((PASSED + FAILED))"

if [ $FAILED -eq 0 ]; then
    echo "🎉 ALL TESTS PASSED - Safe to proceed with modernization"
    exit 0
else
    echo "⚠️  SOME TESTS FAILED - Review failures before proceeding"
    exit 1
fi